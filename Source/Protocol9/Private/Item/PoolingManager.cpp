#include "Item/PoolingManager.h"
#include "Item/ObjectPoolingComponent.h"


APoolingManager::APoolingManager()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	ObjectPoolingComponent = CreateDefaultSubobject<UObjectPoolingComponent>(TEXT("ObjectPoolingComponent"));
}


void APoolingManager::BeginPlay()
{
	Super::BeginPlay();
	
}


