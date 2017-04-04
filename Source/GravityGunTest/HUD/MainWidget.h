// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITYGUNTEST_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:

		/** If the hud should show the alert graphics to indicate that something can be interacted with */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool ShowInteractAlert;

		/** If the hud should show that a certain weapon can be equipped by pressing a key */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool ShowPickupText;
	
};
