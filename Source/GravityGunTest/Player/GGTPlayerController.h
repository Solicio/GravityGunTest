// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GGTCharacter.h"
#include "HUD/MainWidget.h"

#include "GameFramework/PlayerController.h"
#include "GGTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYGUNTEST_API AGGTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:

		/** Set the default values of thie controller */
		AGGTPlayerController();

		/** The distance the player can interact with objects with */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		float InteractTraceLength;


		/** The main widget class used for spawning the umg widget */
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
		TSubclassOf<UMainWidget> MainWidgetClass;

		/** Reference to the main UMG widget */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		UMainWidget* MainWidget;


	private:
		
		/** The character controlled by this controller */
		UPROPERTY()
		AGGTCharacter* ControlledCharacter;


	protected:

		/** Input binding functions */
		
		/** Called for forwards/backward input*/
		void Move(float Value);

		/** Called for side to side input*/
		void Strafe(float Value);

		/** Called when the player wants to jump, and when the player releases the jump key */
		void Jump();
		void StopJumping();

		void LeftClick();
		void RightClick();
		void Interact();
		void DropWeapon();

		/** Called to bind functionality to input */
		virtual void SetupInputComponent() override;

		/** Called when the game starts or when spawned */
		virtual void BeginPlay() override;

		/** Called every frame */
		virtual void Tick(float DeltaTime) override;
	
};
