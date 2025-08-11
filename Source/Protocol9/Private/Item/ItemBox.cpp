#include "Item/ItemBox.h"
#include "Item/ObjectPoolingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Item/PoolingManager.h"




AItemBox::AItemBox()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	RootComponent = StaticMesh;
	
}


void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;
}


float AItemBox::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurrentHP -= DamageAmount;
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);
	if (CurrentHP == 0.0f)
	{
		OnDestroyed();
	}

	return DamageAmount;
}

void AItemBox::OnDestroyed()
{
	OnBoxDestroyedLocation.Broadcast(GetActorLocation());

	Destroy();
}




