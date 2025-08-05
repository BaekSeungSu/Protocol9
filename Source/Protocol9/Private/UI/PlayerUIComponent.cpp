#include "UI/PlayerUIComponent.h"
#include "Character/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/CrosshairWidget.h"

UPlayerUIComponent::UPlayerUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	CrosshairWidgetInstance = nullptr;

	DefaultAimSize = 0.0f;
	JumpingAimSize = 50.0f;
	MovingAimSize = 25.0f;
	AimInterpolationSpeed = 10.0f;
	
	CurrentAimSize = 0.0f;
	TargetAimSize = 0.0f;
}

void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeCrosshair();

	CurrentAimSize = DefaultAimSize;
	TargetAimSize = DefaultAimSize;
}


void UPlayerUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CrosshairWidgetInstance)
	{
		UpdateCrosshair(DeltaTime);
	}

}

void UPlayerUIComponent::InitializeCrosshair()
{
	if (CrosshairWidgetClass)
	{
		APlayerController* PlayerController = GetOwner()->GetInstigatorController<APlayerController>();
		if (PlayerController)
		{
			CrosshairWidgetInstance = CreateWidget<UCrosshairWidget>(PlayerController, CrosshairWidgetClass);
			if (CrosshairWidgetInstance)
			{
				CrosshairWidgetInstance->AddToViewport();
			}
		}
	}
}

void UPlayerUIComponent::UpdateCrosshair(float DeltaTime)
{
	AMainCharacter* Player = Cast<AMainCharacter>(GetOwner());
	if (!Player) return;

	UCharacterMovementComponent* MovementComponent = Player->GetCharacterMovement();
	if (!MovementComponent) return;

	bool bIsFalling = MovementComponent->IsFalling();
	float Speed = Player->GetVelocity().Size2D();

	if (bIsFalling)
	{
		TargetAimSize = JumpingAimSize;
	}
	else if (Speed > 0.0f)
	{
		TargetAimSize = MovingAimSize;
	}
	else
	{
		TargetAimSize = DefaultAimSize;
	}

	CurrentAimSize = FMath::FInterpTo(CurrentAimSize, TargetAimSize, DeltaTime, AimInterpolationSpeed);
	CrosshairWidgetInstance->UpdateCrosshairSize(CurrentAimSize);
}
