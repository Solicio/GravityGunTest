// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityGunTest.h"
#include "GGTCharacter.h"

#include "GGTPlayerController.h"
#include "Weapons/GravityGun.h"

// Sets default values
AGGTCharacter::AGGTCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f);
	CameraComponent->bUsePawnControlRotation = true;

	// Create the mesh that will represent this character. Arms only.
	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(CameraComponent);
	PlayerMesh->bCastDynamicShadow = false;
	PlayerMesh->CastShadow = false;
	PlayerMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	PlayerMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);
}

// Called when the game starts or when spawned
void AGGTCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawn and equip the start weapon if there is a valid class
	if (StartWeaponClass)
	{
		// Spawn parameters
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Instigator = Instigator;
		SpawnInfo.bDeferConstruction = false;

		// Spawning and equipping of the weapon
		AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(StartWeaponClass, FTransform::Identity, SpawnInfo);
		EquipWeapon(SpawnedWeapon);
	}

	// Get the controller and store it
	if (GetController())
		GGTController = Cast<AGGTPlayerController>(GetController());

	// Trace parameters
	TraceParams = FCollisionQueryParams(FName(TEXT("Hover Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;
}

// Called every frame
void AGGTCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GGTController)
	{
		// Reset the values on the widget
		GGTController->MainWidget->ShowInteractAlert = false;
		GGTController->MainWidget->ShowPickupText = false;

		// Do a line traces to see if there is something that can be interacted with
		// First check if the player is looking at weapons
		FHitResult hitResult(ForceInit);
		FVector startFVector = CameraComponent->GetComponentLocation();
		FVector endFVector = startFVector + (CameraComponent->GetForwardVector() * GGTController->InteractTraceLength);

		if (GetWorld()->LineTraceSingleByChannel(hitResult, startFVector, endFVector, ECC_Visibility, TraceParams))
		{
			// Check if the trace hits a weapon
			if (hitResult.GetComponent()->ComponentHasTag("Weapon"))
			{
				GGTController->MainWidget->ShowInteractAlert = true;
				GGTController->MainWidget->ShowPickupText = true;
			}
		}
		
		// Only do next check for gravity guns
		if (EquippedWeapon && EquippedWeapon->WeaponType == EWeaponType::WT_GravityGun)
		{
			AGravityGun* GravityGun = Cast<AGravityGun>(EquippedWeapon);
			if (GravityGun)
			{
				// Do the next line trace for objects that can be manipulated by the gravity gun
				endFVector = startFVector + (CameraComponent->GetForwardVector() * GravityGun->TraceLength);
				hitResult.Init();

				if (GetWorld()->LineTraceSingleByChannel(hitResult, startFVector, endFVector, ECC_Visibility, TraceParams))
				{
					// Make sure that the object is simulating physics and is not a weapon
					if (hitResult.GetComponent()->IsSimulatingPhysics() && !hitResult.GetComponent()->ComponentHasTag("Weapon"))
					{
						GGTController->MainWidget->ShowInteractAlert = true;
					}
				}
			}
		}
	}
}

bool AGGTCharacter::FireWeapon()
{
	if (EquippedWeapon == nullptr)
		return false;

	// Fire the weapon
	EquippedWeapon->Fire(CameraComponent->GetComponentLocation(), CameraComponent->GetForwardVector());

	return true;
}

bool AGGTCharacter::AltFireWeapon()
{
	if (EquippedWeapon == nullptr)
		return false;

	// Alt Fire the weapon
	EquippedWeapon->AltFire(CameraComponent->GetComponentLocation(), CameraComponent->GetForwardVector());

	return true;
}

void AGGTCharacter::DropWeapon()
{
	if (EquippedWeapon == nullptr)
		return;
	
	// Let the weapon know it's being dropped
	EquippedWeapon->DropWeapon();

	// Detaches, sets the state and removes the reference to the equipped weapon.
	EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EquippedWeapon->SetState(EWeaponStates::WS_Free);
	EquippedWeapon->WeaponMesh->AddImpulse((FVector::UpVector + FVector(0.0f, 0.5f, 0.0f)) * 2000.0f);
	EquippedWeapon = nullptr;
}

void AGGTCharacter::EquipWeapon(AWeaponBase* NewWeapon)
{
	// Make sure it's valid
	if (NewWeapon == nullptr)
		return;

	// Drop the weapon if there is one equipped
	if (EquippedWeapon)
		DropWeapon();

	// Set the reference
	EquippedWeapon = NewWeapon;
	EquippedWeapon->SetState(EWeaponStates::WS_Held);
	
	// Attach gun mesh component to the player mesh
	EquippedWeapon->AttachToComponent(PlayerMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}