#include "Character/HPComponent.h"
#include "Character/MainCharacter.h"

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

	if (GetOwner())
	{
		
	}

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
	if (bIsInvisible)
	{
		return;
	}
	if (Damage > 0.0f)
	{
		CurrentHP = FMath::Max(0.0f, CurrentHP - Damage);
	}

	if (CurrentHP == 0)
	{
		OnDeath();
	}
}

void UHPComponent::AddHealth(float HealAmount)
{
	if (HealAmount > 0.0f)
	{
		CurrentHP = FMath::Min(MaxHP, CurrentHP + HealAmount);
	}
	UE_LOG(LogTemp,Warning,TEXT("HP : %f"),CurrentHP);
}

void UHPComponent::LockHealth()
{
	bIsInvisible =true;
	UE_LOG(LogTemp,Warning,TEXT("Invinsible Time!"));
}

void UHPComponent::UnlockHealth()
{
	bIsInvisible = false;
	UE_LOG(LogTemp,Warning,TEXT("Invinsible Time End!"));
}


void UHPComponent::OnDeath()
{
	
	bIsDead = true;

	OnDeathEvent.Broadcast();
}








