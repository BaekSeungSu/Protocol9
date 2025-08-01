// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainPlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "EnhancedInputSubsystems.h"

AMainPlayerController::AMainPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	FireAction(nullptr),
	JumpAction(nullptr),
	DashAction(nullptr),
	ReloadAction(nullptr)
{
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
				Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
}