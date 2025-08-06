#include "Item/ItemBox.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item/PoolingManager.h"
#include "Components/BoxComponent.h"



AItemBox::AItemBox()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(StaticMesh);

	BoxComponent -> OnComponentBeginOverlap.AddDynamic(this,&AItemBox::OnItemOverlap);
}

void AItemBox::OnItemOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateItem(OtherActor);
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}

void AItemBox::ActivateItem(AActor* Activator)
{
	if (Activator && OwningPool)
	{
		OwningPool->SpawnRandomItem(GetActorLocation());
	}
}




void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	// ���忡�� APoolingManager Ŭ������ ���͸� ã��
	AActor* PoolingActor = UGameplayStatics::GetActorOfClass(GetWorld(), APoolingManager::StaticClass());
	if (PoolingActor)
	{
		OwningPool = PoolingActor->FindComponentByClass<UObjectPoolingComponent>();
	}
	
	
}


