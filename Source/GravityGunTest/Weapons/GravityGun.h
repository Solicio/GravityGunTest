// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/WeaponBase.h"
#include "GravityGun.generated.h"


/**
 * 
 */
UCLASS()
class GRAVITYGUNTEST_API AGravityGun : public AWeaponBase
{
	GENERATED_BODY()

	public:
	
		AGravityGun();

		/** The component used for the gravity effect when picking up objects */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity Gun")
		UPhysicsHandleComponent* PhysicsHandle;

		/** The particle effect that is used when the gravity gun fires */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity Gun")
		UParticleSystemComponent* BurstParticleComponent;

		/** The particle effect that is used when the gravity gun is grabbing an object */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity Gun")
		UParticleSystemComponent* PullParticleComponent;


		/** How long the trace used for grabbing and shooting away physics objects is */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		float TraceLength;

		/** How far away the player can get to the grabbed object.
		*	Example, if the player makes the object get stuck behind something and walks away.
		*	When the distance reaches this value, drop the object.
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		float MaxObjectDistance;

		/** How large the impulse should be when left clicking to fire 
		*	The end impulse is gotten from ImpulsePower * Mass
		*	Mass being that of the object that is being shot.
		*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		float ImpulsePower;

		/** Time between the weapon can fire it's impulses */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		float FireCooldown;

		/** Sound that plays when the gun pushes objects away */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		USoundBase* FireSound;

		/** Sound that plays when the gun pulls an object in */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		USoundBase* PullSound;

		/** The camera shake that plays when the gun fires */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity Gun")
		TSubclassOf<UCameraShake> CameraShake;


		/** Fires the weapon, returns true if the weapon was successfully fired
		*	Takes the start location of the trace and the direction as parameter.
		*	This is done to match the crosshair to what you are hitting.
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual bool Fire(FVector TraceStart, FVector Direction) override;

		/** Alt Fires the weapon, returns true if the weapon was successfully fired
		*	Takes the start location of the trace and the direction as parameter.
		*	This is done to match the crosshair to what you are hitting.
		*/
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual bool AltFire(FVector TraceStart, FVector Direction) override;

		/** Called when a character drops this weapon. */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void DropWeapon() override;


	protected:

		float CurrentFireDelay;

		/** Trace parameters for the line trace used for grabbing objects, or firing */
		FCollisionQueryParams TraceParams;

		/** Handle to manage the timer that calls the burst deactivate system
		*	Which deactivates the burst particle system
		*/
		FTimerHandle BurstTimerHandle;
		void BurstSystemDeactivate();

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called when the objects is destroyed/removed or level transition */
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
	
};
