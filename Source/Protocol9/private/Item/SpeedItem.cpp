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
				);														//�ӵ� ���� ��� �߰�
			GetWorld()->GetTimerManager().SetTimer(
				EffectTimerHandle,
				this,
				&ASpeedItem::EndEffect,
				ItemDuration,
				false);										// ���� �ð� �ڿ� ȿ���� ������ �Լ��� �̵�
		}
	}
	DestroyItem();
}

void ASpeedItem::EndEffect() 
{
}