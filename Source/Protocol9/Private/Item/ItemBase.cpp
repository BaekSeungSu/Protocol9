#include "Item/ItemBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"

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
	Collision -> OnComponentEndOverlap.AddDynamic(this,&AItemBase::OnItemEndOverlap);
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
void AItemBase::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
	)
{
	
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



