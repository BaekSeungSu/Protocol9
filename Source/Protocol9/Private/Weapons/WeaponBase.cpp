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
	
	FVector FireDirection = EyeRotation.Vector();
	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + FireDirection * CurrentWeaponData->Range;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams);

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	DrawDebugPoint(GetWorld(), MuzzleLocation, 10.0f, FColor::Red, false, 1.0f);

	if (bHit)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, HitResult.ImpactPoint, FColor::Green, false, 2.0f, 0, 2.0f);
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 10, FColor::Blue, false, 2.0f, 0, 3.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitResult.GetActor()->GetName());
	}
	else
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);
		UE_LOG(LogTemp, Error, TEXT("Miss"));
	}
	
}

void AWeaponBase::LoadWeaponData()
{
	if (!WeaponDataTable) return;
	if (WeaponDataRowName.IsNone()) return;

	CurrentWeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponDataRowName, TEXT(""));
}



