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

void AItemBase::OnAcquireFromPool()
{
	bAvailableInPool = false;

	// �ð�/�浹 ����
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	if (Collision) {
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Collision->SetGenerateOverlapEvents(true);
	}

	// ���̾ư��� �����
	if (NiagaraComp) {
		NiagaraComp->Deactivate();
		// ���� ������ ���� ResetSystem() �Ǵ� ReinitializeSystem() ���
#if ENGINE_MAJOR_VERSION >= 5
		NiagaraComp->ReinitializeSystem();
#else
		NiagaraComp->ResetSystem();
#endif
		NiagaraComp->Activate(true); // 0�� ���¿��� ���
	}

	// ���� Ÿ�̸� �����
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);
	if (ItemLifeDuration > 0.f) {
		GetWorld()->GetTimerManager().SetTimer(
			ItemLIfeTimerHandle, this, &AItemBase::ReturnToPool, ItemLifeDuration, false);
	}
}

void AItemBase::OnReturnToPool()
{
	// Ÿ�̸� ����
	GetWorld()->GetTimerManager().ClearTimer(ItemLIfeTimerHandle);

	// ���̾ư���/�浹 ����
	if (NiagaraComp) NiagaraComp->Deactivate();
	if (Collision) {
		Collision->SetGenerateOverlapEvents(false);
		Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// ���� ó��
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	bAvailableInPool = true;
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
		   GetActorLocation(),					// �������� �ִ� ��ġ���� ����
		   FRotator::ZeroRotator,
		   FVector(1.0f),
		   true,						// bAutoDestroy: ����Ʈ ����� ������ �ڵ����� �ı�
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



