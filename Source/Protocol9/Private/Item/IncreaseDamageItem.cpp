#include "Item/IncreaseDamageItem.h"
#include "Components/SphereComponent.h"
#include "Character/MainCharacter.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"


AIncreaseDamageItem::AIncreaseDamageItem()
: AffectedPlayer(nullptr)
{
	MultiDamage = 1.3f;
	ItemDuration = 7.0f;
	ItemType = "DamageIncreaseItem";
}

void AIncreaseDamageItem::ActivateItem(AActor* Activator)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	Super::ActivateItem(Activator);
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Blue,
			FString::Printf(TEXT("Increase Damage ")));				
			
			MyCharacter->AddAttack(MultiDamage);

			MyCharacter->HandleAttackBoostEffect(); //UI : 아이템 아이콘 점등 기능 추가
			
			AffectedPlayer = MyCharacter; 
			GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&AIncreaseDamageItem::EndEffect,
			ItemDuration,
			false);										// ���� �ð� �ڿ� ȿ���� ������ �Լ��� �̵�
		}
	}
}

void AIncreaseDamageItem::EndEffect() 
{
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Increase Damage Effect End! ")));
			AffectedPlayer->ResetAttack();							
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	AffectedPlayer = nullptr;	
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}