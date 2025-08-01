#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MainPlayerController.h"
#include "Weapons/Data/WeaponData.h"
#include "DrawDebugHelpers.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	SetRootComponent(WeaponMesh);

	CurrentWeaponData = nullptr;
}

void AWeaponBase::PrimaryFire_Implementation()
{
	IWeaponInterface::PrimaryFire_Implementation();
}

void AWeaponBase::StopFire_Implementation()
{
	IWeaponInterface::StopFire_Implementation();
}

void AWeaponBase::Reload_Implementation()
{
	IWeaponInterface::Reload_Implementation();
}



void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	LoadWeaponData();
}



void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AWeaponBase::FireAction()
{
	if (CurrentWeaponData)
	{
		if (CurrentWeaponData->FireType == EFireType::HitScan)
		{
			FireHitScan();
		}
	}
}

void AWeaponBase::FireHitScan()
{
	AMainPlayerController* OwnerController = Cast<AMainPlayerController>(GetOwner()->GetInstigatorController());
	if (!OwnerController) return;

	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector Start = EyeLocation;
	FVector Direction = EyeRotation.Vector();
	FVector End = Start + Direction * CurrentWeaponData->Range;
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("MuzzleSocket"));

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (bHit)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.ImpactPoint, FColor::Red, false, 4.0f, 0, 3.0f);
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5.f), FColor::Green, false, 4.0f, 0, 3.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());
	}
	else  
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 4.0f, 0, 3.0f);
		UE_LOG(LogTemp, Warning, TEXT("Miss"));
	}
}

void AWeaponBase::LoadWeaponData()
{
	if (!WeaponDataTable) return;
	if (WeaponDataRowName.IsNone()) return;

	CurrentWeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponDataRowName, TEXT(""));
}



