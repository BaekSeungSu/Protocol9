
#include "Character/StaminaComponent.h"
#include "TimerManager.h"

UStaminaComponent::UStaminaComponent()
	:MaxStaminaCount(0),
	CurrentStaminaCount(0),
	StaminaChargeTime(5.0f)
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UStaminaComponent::UseStamina()
{
	CurrentStaminaCount--;
	
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

		GetOwner()->GetWorldTimerManager().SetTimer(
		StaminaChargeTimer,
		this,
		&UStaminaComponent::ChargeStamina,
		5.0f,
		false);
	}
}