// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityGunTest.h"
#include "WeaponBase.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	WeaponMesh->ComponentTags.Add("Weapon");
	RootComponent = WeaponMesh;

	// Create the muzzle location
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(WeaponMesh);

	StartState = EWeaponStates::WS_Free;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Set the start state
	SetState(StartState);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWeaponBase::Fire(FVector TraceStart, FVector Direction)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Base Weapon - Fire");

	return false;
}

bool AWeaponBase::AltFire(FVector TraceStart, FVector Direction)
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, "Base Weapon - Alt Fire");

	return false;
}

void AWeaponBase::SetState(EWeaponStates NewState)
{
	CurrentState = NewState;
	
	// Set collision and physics
	// When it's free = Collision On
	// When it's held = Collision Off
	switch(NewState)
	{
		case EWeaponStates::WS_Free:
			WeaponMesh->SetSimulatePhysics(true);
			WeaponMesh->SetCollisionProfileName("WeaponFree");
			break;

		case EWeaponStates::WS_Held:
			WeaponMesh->SetSimulatePhysics(false);
			WeaponMesh->SetCollisionProfileName("NoCollision");
			break;
	};
}

EWeaponStates AWeaponBase::GetState()
{
	return CurrentState;
}

void AWeaponBase::DropWeapon()
{

}