// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityGunTest.h"
#include "GGTPlayerController.h"


AGGTPlayerController::AGGTPlayerController()
{

	InteractTraceLength = 300.0f;
}

// Called to bind functionality to input
void AGGTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Jumping
	InputComponent->BindAction("Jump", IE_Pressed, this, &AGGTPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AGGTPlayerController::StopJumping);

	// Mouse click
	InputComponent->BindAction("LeftButton", IE_Pressed, this, &AGGTPlayerController::LeftClick);
	InputComponent->BindAction("RightButton", IE_Pressed, this, &AGGTPlayerController::RightClick);

	// Movement
	InputComponent->BindAxis("Move", this, &AGGTPlayerController::Move);
	InputComponent->BindAxis("Strafe", this, &AGGTPlayerController::Strafe);

	// Interact
	InputComponent->BindAction("Interact", IE_Pressed, this, &AGGTPlayerController::Interact);

	// Player mouse movement
	InputComponent->BindAxis("Turn", this, &APlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &APlayerController::AddPitchInput);

	// Weapon drop
	InputComponent->BindAction("DropWeapon", IE_Pressed, this, &AGGTPlayerController::DropWeapon);
}

void AGGTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Cast and store the casted value here so we don't need to cast the character every time we want to use it
	if(GetCharacter())
		ControlledCharacter = Cast<AGGTCharacter>(GetCharacter());

	if (MainWidgetClass)
	{
		// Create the widget
		MainWidget = CreateWidget<UMainWidget>(this, MainWidgetClass);

		// Add it to viewport
		if (MainWidget)
			MainWidget->AddToViewport();
	}

	// Lock the mouse to the game window.
	SetInputMode(FInputModeGameOnly());
}

void AGGTPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGGTPlayerController::LeftClick()
{
	if (ControlledCharacter == nullptr)
		return;

	// Call the character to fire the weapon
	ControlledCharacter->FireWeapon();
}

void AGGTPlayerController::RightClick()
{
	if (ControlledCharacter == nullptr)
		return;

	// Call the character to fire the weapon
	ControlledCharacter->AltFireWeapon();
}

void AGGTPlayerController::Move(float Value)
{
	if (ControlledCharacter == nullptr)
		return;

	// Add movement input to the character
	ControlledCharacter->AddMovementInput(ControlledCharacter->GetActorForwardVector(), Value);
}

void AGGTPlayerController::Strafe(float Value)
{
	if (ControlledCharacter == nullptr)
		return;

	// Add movement input to the character
	ControlledCharacter->AddMovementInput(ControlledCharacter->GetActorRightVector(), Value);
}

void AGGTPlayerController::Jump()
{
	if (ControlledCharacter == nullptr)
		return;

	// Make the character jump
	ControlledCharacter->Jump();

}

void AGGTPlayerController::StopJumping()
{
	if (ControlledCharacter == nullptr)
		return;

	// Tell the character to stop jumping
	ControlledCharacter->StopJumping();

}

void AGGTPlayerController::Interact()
{
	if (ControlledCharacter == nullptr)
		return;

	// Perform a line trace to see if something was hit that can be interacted with
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Interct Trace")), false, nullptr);
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;

	FHitResult hitResult(ForceInit);
	FVector startFVector = ControlledCharacter->CameraComponent->GetComponentLocation();
	FVector endFVector = startFVector + (ControlledCharacter->CameraComponent->GetForwardVector() * InteractTraceLength);

	if (GetWorld()->LineTraceSingleByChannel(hitResult, startFVector, endFVector, ECC_Visibility, TraceParams))
	{
		// Check if the hit object is a weapon by checking tag
		if (hitResult.GetComponent()->ComponentHasTag("Weapon"))
		{
			// Make sure that the weapon is free
			AWeaponBase* Weapon = Cast<AWeaponBase>(hitResult.GetActor());
			if (Weapon && Weapon->GetState() == EWeaponStates::WS_Free)
			{
				// Then tell the character to equip the weapon
				ControlledCharacter->EquipWeapon(Weapon);
			}
		}
	}
}

void AGGTPlayerController::DropWeapon()
{
	if (ControlledCharacter == nullptr)
		return;

	// Tell the character to drop the currently equipped weapon
	ControlledCharacter->DropWeapon();
}