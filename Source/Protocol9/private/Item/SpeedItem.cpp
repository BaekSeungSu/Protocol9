#include "Item/SpeedItem.h"

#include "Components/SphereComponent.h"

ASpeedItem::ASpeedItem()
{
	MultiSpeed = 2.0f;
	ItemDuration = 5.0f;
	ItemType = "SpeedItem";
}

void ASpeedItem::ActivateItem(AActor* Activator)
{
	TArray<AActor*> OverlapActors;
	Collision->GetOverlappingActors(OverlapActors);

	for (AActor* Actor : OverlapActors)
	{
		if (Actor && Actor ->ActorHasTag("Player"))
		{
			GEngine->AddOnScreenDebugMessage(-1,
				ItemDuration,
				FColor::Blue,
				FString::Printf(TEXT("Speed Up"))
				);														//속도 증가 기능 추가
			GetWorld()->GetTimerManager().SetTimer(
				EffectTimerHandle,
				this,
				&ASpeedItem::EndEffect,
				ItemDuration,
				false);										// 일정 시간 뒤에 효과가 끝나는 함수로 이동
		}
	}
	DestroyItem();
}

void ASpeedItem::EndEffect() 
{
}