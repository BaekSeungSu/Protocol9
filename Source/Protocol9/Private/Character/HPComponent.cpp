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

void UHPComponent::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvisible)
	{
		return;
	}
	if (DamageAmount > 0.0f)
	{
		CurrentHP = FMath::Max(0.0f, CurrentHP - DamageAmount);

		HPChanged.Broadcast(CurrentHP);
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

		HPChanged.Broadcast(CurrentHP);
	}
	UE_LOG(LogTemp,Warning,TEXT("HP : %f"),CurrentHP);
}

//아이템 무적 효과 적용 함수
void UHPComponent::LockHealth()
{
	bIsInvisible =true;
	UE_LOG(LogTemp,Warning,TEXT("Invinsible Time!"));
}

//아이템 무적 효과 제거 함수
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








