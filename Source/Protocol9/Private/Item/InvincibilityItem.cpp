#include "Item/InvincibilityItem.h"
#include "Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
AInvincibilityItem::AInvincibilityItem()
	: AffectedPlayer(nullptr)
{
	ItemDuration = 3.0f;
	ItemType = "InvincibilityItem";
}

void AInvincibilityItem::ActivateItem(AActor* Activator)
{
	if (Activator && Activator ->ActorHasTag("Player"))
	{
		AMainCharacter* MyCharacter = Cast<AMainCharacter>(Activator);
		if (MyCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1,
			2.0f,
			FColor::Blue,
			FString::Printf(TEXT("Invincibility Time! ")));				
			
			//데미지 증가 로직 추가 


			AffectedPlayer = MyCharacter;                       //이 효과를 받는 엑터 저장 
			GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&AInvincibilityItem::EndEffect,
			ItemDuration,
			false);										// 일정 시간 뒤에 효과가 끝나는 함수로 이동
		}
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void AInvincibilityItem::EndEffect() 
{
	//데미지 증가 함수 제거

	
	if (AffectedPlayer)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Invincibility Time End! ")));
		AffectedPlayer = nullptr;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	DestroyItem();
}