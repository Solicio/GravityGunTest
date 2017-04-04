// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/WeaponBase.h"

#include "GameFramework/Character.h"
#include "GGTCharacter.generated.h"


class AGGTPlayerController;


UCLASS()
class GRAVITYGUNTEST_API AGGTCharacter : public ACharacter
{
	GENERATED_BODY()

	public:
		/** Sets default values for this character's properties */
		AGGTCharacter();

		/** First person camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComponent;

		/** The pawn mesh. Only arms. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* PlayerMesh;


		/** Fires the weapon, if there is one */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool FireWeapon();

		/** Alt Fires the weapon, if there is one */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		bool AltFireWeapon();

		/** Drops the currently equiped weapon on the floor */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		void DropWeapon();

		/** Equips the provided weapon */
		UFUNCTION(BlueprintCallable, Category = "Weapon")
		void EquipWeapon(AWeaponBase* NewWeapon);


		/** A reference to the currently equipped weapon, if there is any */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		AWeaponBase* EquippedWeapon;

		/** A reference to the weapon class that the player character should start the game holding.
		*	Leave empty to start the game without a weapon equipped.
		*/
		UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<AWeaponBase> StartWeaponClass;


	private:

		UPROPERTY()
		AGGTPlayerController* GGTController;

	protected:

		/** Trace parameters for the line trace used for interaction and crosshair alert traces */
		FCollisionQueryParams TraceParams;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
};
