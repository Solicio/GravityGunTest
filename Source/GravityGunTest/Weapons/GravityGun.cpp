// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityGunTest.h"
#include "GravityGun.h"

// Sets default values
AGravityGun::AGravityGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the physics handle component
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->LinearDamping = 50.0f;
	PhysicsHandle->InterpolationSpeed = 15.0f;

	// Create the particle system components
	BurstParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BurstParticleComponent"));
	BurstParticleComponent->SetupAttachment(MuzzleLocation);
	PullParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PullParticleComponent"));
	PullParticleComponent->SetupAttachment(MuzzleLocation);

	TraceLength = 1500.0f;
	MaxObjectDistance = 1600.0f;
	ImpulsePower = 10000.0f;

	FireCooldown = 0.5f;
	CurrentFireDelay = 0.0f;

	// Set the weapon type
	WeaponType = EWeaponType::WT_GravityGun;
}

// Called when the game starts or when spawned
void AGravityGun::BeginPlay()
{
	Super::BeginPlay();

	// Trace parameters
	TraceParams = FCollisionQueryParams(FName(TEXT("Gravity Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

// Called every frame
void AGravityGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Count down the fire delay
	if (CurrentFireDelay > 0.0f)
		CurrentFireDelay -= DeltaTime;


	if (PhysicsHandle->GetGrabbedComponent())
	{
		// Calculate the new target location.
		// The target location is relative to the object that the gun has grabbed, the bigger the gun, the further away the object is.
		FVector HandleTargetLocation;
		HandleTargetLocation = MuzzleLocation->GetComponentLocation() + (MuzzleLocation->GetForwardVector() * (PhysicsHandle->GetGrabbedComponent()->Bounds.GetSphere().W + 100.0f));

		// Move the object up a bit to have more in the middle of the screen
		// And then clamp the value so that there is a limit to how far down it can go, to prevent the physics from bugging with the ground.
		HandleTargetLocation.Z += 25.0f;
		HandleTargetLocation.Z = FMath::Clamp(HandleTargetLocation.Z, MuzzleLocation->GetComponentLocation().Z + (PhysicsHandle->GetGrabbedComponent()->Bounds.GetSphere().W * 0.1f), MuzzleLocation->GetComponentLocation().Z + 300.0f);
		
		// Set the target location of the physics handle.
		PhysicsHandle->SetTargetLocation(HandleTargetLocation);


		// Make sure that the player cannot walk too far away from the grabbed object.
		float Distance = FMath::Abs(FVector::Distance(PhysicsHandle->GetGrabbedComponent()->GetComponentLocation(), GetActorLocation()));
		if (Distance > MaxObjectDistance)
		{
			// Set the collision response with the player back to block and reduce it's velocity
			PhysicsHandle->GetGrabbedComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
			PhysicsHandle->GetGrabbedComponent()->SetAllPhysicsLinearVelocity(PhysicsHandle->GetGrabbedComponent()->GetPhysicsLinearVelocity() * 0.25f);

			// And release the object
			PhysicsHandle->ReleaseComponent();
		}


		// Active the pull particle system
		if (PullParticleComponent && PullParticleComponent->FXSystem && !PullParticleComponent->IsActive())
		{
			PullParticleComponent->ActivateSystem();
		}
	}
	else
	{
		// Deactive the pull system
		if (PullParticleComponent && PullParticleComponent->FXSystem)
		{
			PullParticleComponent->DeactivateSystem();
		}
	}

}

bool AGravityGun::Fire(FVector TraceStart, FVector Direction)
{
	if (CurrentFireDelay > 0.0f)
		return false;

	// Play the different effects.
	if (CameraShake && FireSound && BurstParticleComponent && BurstParticleComponent->FXSystem)
	{
		UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh);
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, GetActorLocation(), 0.0f, 300.0f);
		
		// Activate the system and set the timer to deactivate it
		BurstParticleComponent->ActivateSystem();
		GetWorldTimerManager().SetTimer(BurstTimerHandle, this, &AGravityGun::BurstSystemDeactivate, 0.3f, false);
	}

	// Temporary reference to the component the gun is holding. Since it will be released here.
	UPrimitiveComponent* ReleasedComp = PhysicsHandle->GetGrabbedComponent();

	if (PhysicsHandle->GetGrabbedComponent())
	{
		PhysicsHandle->GetGrabbedComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		PhysicsHandle->ReleaseComponent();
	}

	// Set the cooldown
	CurrentFireDelay = FireCooldown;

	// First check if the gun held something.
	if (ReleasedComp)
	{
		// Check if the object is close enough to the gun.
		// Do this since the object can potentially be quite a distance away and it would look strange if it was fired away at that distance
		FVector HandleTargetLocation = MuzzleLocation->GetComponentLocation() + (MuzzleLocation->GetForwardVector() * (ReleasedComp->Bounds.GetSphere().W + 100.0f));
		HandleTargetLocation.Z += 25.0f;
		HandleTargetLocation.Z = FMath::Clamp(HandleTargetLocation.Z, MuzzleLocation->GetComponentLocation().Z + (ReleasedComp->Bounds.GetSphere().W * 0.1f), MuzzleLocation->GetComponentLocation().Z + 300.0f);

		float Distance = FMath::Abs(FVector::Distance(ReleasedComp->GetComponentLocation(), HandleTargetLocation));
		if (Distance <= 200.0f)
		{
			// First set all velocity to 0
			ReleasedComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

			// Calculate the impulse using the objects mass and then add it.
			FVector Impulse = MuzzleLocation->GetForwardVector() * (ImpulsePower * ReleasedComp->GetMass());
			ReleasedComp->AddImpulse(Impulse);
			
			return true;	
		}
	}

	// If the held object could not be fired, because it was too far away from the gun, do a line trace to see if there is anything else to hit.
	FHitResult hitResult(ForceInit);
	FVector startFVector = TraceStart;
	FVector endFVector = startFVector + (Direction * TraceLength);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, startFVector, endFVector, ECC_Visibility, TraceParams))
	{
		// Make sure that the object is simulating physics
		if (hitResult.GetComponent()->IsSimulatingPhysics() && !hitResult.GetComponent()->ComponentHasTag("Weapon"))
		{
			hitResult.GetComponent()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

			// Calculate the impulse using the objects mass and then add it.
			FVector Impulse = MuzzleLocation->GetForwardVector() * (ImpulsePower * hitResult.GetComponent()->GetMass());
			hitResult.GetComponent()->AddImpulse(Impulse);
		}
	}

	return true;
}

bool AGravityGun::AltFire(FVector TraceStart, FVector Direction)
{
	// If the handle already has something grabbed, release it.
	if (PhysicsHandle->GetGrabbedComponent())
	{
		// Set the collision response with the player back to block and reduce it's velocity
		PhysicsHandle->GetGrabbedComponent()->SetAllPhysicsLinearVelocity(PhysicsHandle->GetGrabbedComponent()->GetPhysicsLinearVelocity() * 0.25f);
		PhysicsHandle->GetGrabbedComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		PhysicsHandle->ReleaseComponent();
		return true;
	}

	// Do a trace to see if there is any objects that can be picked up
	FHitResult hitResult(ForceInit);
	FVector startFVector = TraceStart;
	FVector endFVector = startFVector + (Direction * TraceLength);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, startFVector, endFVector, ECC_Visibility, TraceParams))
	{
		// Make sure that the object is simulating physics, and stop interaction between weapons
		if (hitResult.GetComponent()->IsSimulatingPhysics() && !hitResult.GetComponent()->ComponentHasTag("Weapon"))
		{
			// Grab the physics object
			PhysicsHandle->GrabComponentAtLocationWithRotation(hitResult.GetComponent(), "", hitResult.GetComponent()->GetComponentLocation(), hitResult.GetComponent()->GetComponentRotation());

			// Set the collision response with the player to ignore so that they don't constantly collide
			PhysicsHandle->GetGrabbedComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

			// Play the pull sound
			if(PullSound)
				UGameplayStatics::SpawnSoundAttached(PullSound, WeaponMesh);
		}
	}

	return false;
}


void AGravityGun::DropWeapon()
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// Set the collision response with the player back to block and reduce it's velocity before release
		PhysicsHandle->GetGrabbedComponent()->SetAllPhysicsLinearVelocity(PhysicsHandle->GetGrabbedComponent()->GetPhysicsLinearVelocity() * 0.25f);
		PhysicsHandle->GetGrabbedComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		PhysicsHandle->ReleaseComponent();
	}
}

void AGravityGun::BurstSystemDeactivate()
{
	// Stop the system
	BurstParticleComponent->DeactivateSystem();
}

void AGravityGun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clear the timer
	GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
}