#include "Weapons/Pickup_Weapon.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/CharacterStateMachine.h"
#include "Weapons/WeaponBase.h"
#include "Character/MainCharacter.h"
#include "Components/SphereComponent.h"
#include "Weapons/InventoryComponent.h"

APickup_Weapon::APickup_Weapon()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->InitSphereRadius(120.f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetRootComponent());
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickupEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PickupEffect"));
	PickupEffect->SetupAttachment(GetRootComponent());
	
	OverlappingCharacter = nullptr;
	bConsumed = false;
}


void APickup_Weapon::BeginPlay()
{
	Super::BeginPlay();
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickup_Weapon::OnBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &APickup_Weapon::OnEndOverlap);
	
}


void APickup_Weapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bConsumed) return;
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
	{
		OverlappingCharacter = MainCharacter;
	}
}

void APickup_Weapon::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
	{
		if (OverlappingCharacter == MainCharacter)
		{
			OverlappingCharacter = nullptr;
		}
	}
}

bool APickup_Weapon::TryConsume(AMainCharacter* Interactor)
{
	if (bConsumed || !Interactor || !WeaponClass) return false;
	if (OverlappingCharacter && OverlappingCharacter != Interactor) return false;

	if (UCharacterStateMachine* StateMachine = Interactor->GetStateMachine())
	{
		if (!StateMachine->CanSwapping())
		{
			return false;
		}
	}
	
	if (UInventoryComponent* Inventory = Interactor->GetInventoryComponent())
	{
		const bool bAdded = Inventory->AddOrReplaceWeapon(WeaponClass);
		if (bAdded)
		{
			bConsumed = true;
			Destroy();
			return true;
		}
	}
	return false;
}

