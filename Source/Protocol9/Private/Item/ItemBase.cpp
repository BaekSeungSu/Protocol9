#include "Item/ItemBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	RootComponent = NiagaraComp;
	NiagaraComp->bAutoActivate = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision -> SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision -> SetupAttachment(NiagaraComp);
	
	Collision -> OnComponentBeginOverlap.AddDynamic(this,&AItemBase::OnItemOverlap);

	ItemLifeDuration = 30.0f;
}

void AItemBase::OnItemOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) 
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateItem(OtherActor);
	}
}

void AItemBase::ActivateItem(AActor* Activator)
{
	if (OverlapEffectSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		   GetWorld(),
		   OverlapEffectSystem,
		   GetActorLocation(),					// 아이템이 있던 위치에서 스폰
		   FRotator::ZeroRotator,
		   FVector(1.0f),
		   true,						// bAutoDestroy: 이펙트 재생이 끝나면 자동으로 파괴
		   true
		);
	}

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),
			PickupSound,
			GetActorLocation()
			);
	}
	
}
FName AItemBase::GetItemType() const 
{
	return ItemType;
}

void AItemBase::EndEffect() 
{
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraSystem)
	{
		NiagaraComp->SetAsset(NiagaraSystem);
	}
}


void AItemBase::ItemLifeTime()
{
	if (ItemLifeDuration>0)
	{
		GetWorld()->GetTimerManager().SetTimer(
				ItemLIfeTimerHandle,
				this,
				&AItemBase::ReturnToPool,
				ItemLifeDuration,
				false);
	}
}

void AItemBase::ReturnToPool()
{
	if (OwningPool)
	{
		OwningPool->ReturnObjectToPool(this);
	}
}



