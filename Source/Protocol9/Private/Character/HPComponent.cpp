#include "Character/HPComponent.h"

UHPComponent::UHPComponent()
	:MaxHP(100.0f),
	CurrentHP(MaxHP),
	bIsDead(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHPComponent::BeginPlay()
{
	Super::BeginPlay();

	//Owner = GetOwner();
}

UHPComponent* UHPComponent::GetHPComponent()
{
	return this;
}

float UHPComponent::GetMaxHP() const
{
	return MaxHP;
}

float UHPComponent::GetCurrentHP() const
{
	return CurrentHP;
}

void UHPComponent::SetMaxHP(float NewMaxHP)
{
	MaxHP = NewMaxHP;
	CurrentHP = NewMaxHP;
}

void UHPComponent::SetCurrentHP(float NewCurrentHP)
{
	if (NewCurrentHP > MaxHP)
	{
		NewCurrentHP = MaxHP;
	}
	else
	{
		CurrentHP = NewCurrentHP;
	}
}

void UHPComponent::TakeDamage(float Damage)
{
	if (Damage > 0.0f)
	{
		CurrentHP = FMath::Max(0.0f, CurrentHP - Damage);
	}

	IsDead();
}

void UHPComponent::AddHealth(float HealAmount)
{
	if (HealAmount > 0.0f)
	{
		CurrentHP = FMath::Min(MaxHP, CurrentHP + HealAmount);
	}
}

bool UHPComponent::IsDead()
{
	if (CurrentHP <= 0.0f)
	{
		bIsDead = true;
	}
	else
	{
		bIsDead = false;
	}
	
	return bIsDead;
}







