// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

/** The different states a weapon can be in */
UENUM(BlueprintType)
enum class EWeaponStates : uint8
{
	WS_Free			UMETA(DisplayName = "Free"),
	WS_Held			UMETA(DisplayName = "Held")
};

/** The different types of weapons */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None			UMETA(DisplayName = "None"),
	WT_GravityGun	UMETA(DisplayName = "Gravity Gun")
};

UCLASS()
class GRAVITYGUNTEST_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	public:	
		
		// Sets default values for this actor's properties
		AWeaponBase();

		/** The mesh of the gun. Used to attach to the player */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* WeaponMesh;
		
		/** The muzzle location used to spawn projectiles
		*	Or for the gravity gun, where the physic objects should be 
		*/
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USceneComponent* MuzzleLocation;


		/** The weapon type this weapon is */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		EWeaponType WeaponType;

		/** The state the weapon should start out as */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		EWeaponStates StartState;


		/** Setter function for CurrentState
		*	This function also sets the weapons collision and physics depending on the new state
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		void SetState(EWeaponStates NewState);

		/** Get the weapons current state */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		EWeaponStates GetState();


		/** Fires the weapon, returns true if the weapon was successfully fired
		*	Override in child classes to implement fire logic.
		*	Takes the start location of the trace and the direction as parameter.
		*	This is done to match the crosshair to what you are hitting.
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual bool Fire(FVector TraceStart, FVector Direction);

		/** Alt Fires the weapon, returns true if the weapon was successfully fired
		*	Override in child classes to implement fire logic.
		*	Takes the start location of the trace and the direction as parameter.
		*	This is done to match the crosshair to what you are hitting.
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual bool AltFire(FVector TraceStart, FVector Direction);

		/** Called when a character drops this weapon.
		*	Virtual so override in childs and implement special logic if there should be any.
		*	Example, gravity gun dropping currently grabbed object.
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void DropWeapon();


	protected:

		/** The current state the weapon is in, if it's being held by a character or free in the world. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		EWeaponStates CurrentState;


		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;	
};
