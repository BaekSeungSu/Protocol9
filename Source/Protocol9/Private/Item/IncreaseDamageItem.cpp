#include "Item/IncreaseDamageItem.h"
#include "Components/SphereComponent.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"

AIncreaseDamageItem::AIncreaseDamageItem()
: AffectedPlayer(nullptr)
{
	MultiDamage = 1.3f;
	ItemDuration = 3.0f;
	ItemType = "DamageIncreaseItem";
}

void AIncreaseDamageItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Blue,
			FString::Printf(TEXT("Increase Damage ")));				
			
			//데미지 증가 로직 추가 
			AffectedPlayer = MyCharacter; 
			GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&AIncreaseDamageItem::EndEffect,
			ItemDuration,
			false);										// 일정 시간 뒤에 효과가 끝나는 함수로 이동
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AIncreaseDamageItem::EndEffect() 
{
	//데미지 증가 함수 제거 
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Increase Damage Effect End! ")));
		AffectedPlayer = nullptr;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	DestroyItem();
}