
#include "Character/StaminaComponent.h"
#include "TimerManager.h"

UStaminaComponent::UStaminaComponent()
	:MaxStaminaCount(3),
	CurrentStaminaCount(3),
	StaminaChargeTime(5.0f)
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

int UStaminaComponent::GetMaxStaminaCount() const
{
	return MaxStaminaCount;
}

int UStaminaComponent::GetCurrentStaminaCount() const
{
	return CurrentStaminaCount;
}

float UStaminaComponent::GetStaminaChargeTime() const
{
	return StaminaChargeTime;
}

void UStaminaComponent::SetMaxStaminaCount(int NewMaxStaminaCount)
{
	MaxStaminaCount = NewMaxStaminaCount;
}

void UStaminaComponent::SetCurrentStaminaCount(int NewCurremStaminaCount)
{
	if (NewCurremStaminaCount > MaxStaminaCount)
	{
		NewCurremStaminaCount = MaxStaminaCount;
	}
	else
	{
		CurrentStaminaCount = NewCurremStaminaCount;
	}
}

void UStaminaComponent::SetStaminaChargeTime(float NewStaminaChargeTime)
{
	StaminaChargeTime = NewStaminaChargeTime;
}

void UStaminaComponent::UseStamina()
{
	CurrentStaminaCount--;
	StaminaChanged.Broadcast(CurrentStaminaCount);

	if (!GetOwner()->GetWorldTimerManager().IsTimerActive(StaminaChargeTimer))
	{
		GetOwner()->GetWorldTimerManager().SetTimer(
		StaminaChargeTimer,
		this,
		&UStaminaComponent::ChargeStamina,
		5.0f,
		false);
	}
}

void UStaminaComponent::ChargeStamina()
{
	if (CurrentStaminaCount < MaxStaminaCount)
	{
		CurrentStaminaCount++;
		StaminaChanged.Broadcast(CurrentStaminaCount);
		
		GetOwner()->GetWorldTimerManager().SetTimer(
		StaminaChargeTimer,
		this,
		&UStaminaComponent::ChargeStamina,
		5.0f,
		false);

		GetOwner()->GetWorldTimerManager().SetTimer(
		SChecktaminaChargeTimer,
		this,
		&UStaminaComponent::CheckStaminaChargeTime,
		0.2f,
		true);
		
	}
}

void UStaminaComponent::CheckStaminaChargeTime()
{
	float RemainTime = GetOwner()->GetWorldTimerManager().GetTimerRemaining(StaminaChargeTimer);

	if (RemainTime > 0.0f)
	{
		RemainStaminaChargeTime.Broadcast(RemainTime);
	}
	else
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(StaminaChargeTimer);
	}
}
