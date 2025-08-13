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
		if (APlayerController* PC = GetOwner()->GetInstigatorController<APlayerController>())
		{
			if (!CrosshairWidgetInstance)
			{
				CrosshairWidgetInstance = CreateWidget<UCrosshairWidget>(PC, CrosshairWidgetClass);
			}
		}
	}
	// UI : 바로 표시 UI호출 했던 부분 취소
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

// UI : 크로스헤어 숨김, 표시 기능 구현
void UPlayerUIComponent::ShowCrosshair()
{
	if (!CrosshairWidgetInstance)
	{
		InitializeCrosshair();
	}
	if (CrosshairWidgetInstance && !CrosshairWidgetInstance->IsInViewport())
	{
		CrosshairWidgetInstance->AddToViewport(/*ZOrder=*/50);
	}
}
void UPlayerUIComponent::HideCrosshair()
{
	if (CrosshairWidgetInstance && CrosshairWidgetInstance->IsInViewport())
	{
		CrosshairWidgetInstance->RemoveFromParent();
	}
}