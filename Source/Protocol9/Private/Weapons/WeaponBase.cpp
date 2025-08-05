#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Character/MainPlayerController.h"
#include "Weapons/Data/WeaponData.h"
#include "UI/PlayerUIComponent.h"
#include "Character/MainCharacter.h"
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
	if (bIsReloading || CurrentAmmo == CurrentWeaponData->MagazineSize)
	{
		return;
	}

	bIsReloading = true;
	UE_LOG(LogTemp, Warning, TEXT("Reload"));

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::FinishReload, CurrentWeaponData->ReloadTime, false);
	
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
	if (!CanFire())
	{
		return;
	}
	CurrentAmmo--;
	ApplyRecoil();
	
	if (CurrentWeaponData)
	{
		if (CurrentWeaponData->FireType == EFireType::HitScan)
		{
			FireHitScan();
		}
		else if (CurrentWeaponData->FireType == EFireType::Projectile)
		{
			FireProjectile();
		}
	}
}

bool AWeaponBase::CanFire() const
{
	if (bIsReloading || CurrentAmmo <=0 || CurrentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not Fire"));
		return false;
	}
	return true;
}

void AWeaponBase::ApplyRecoil()
{
	AController* OwnerController = GetOwner()->GetInstigatorController();
	if (OwnerController)
	{
		APlayerController* PlayerController = Cast<APlayerController>(OwnerController);
		if (PlayerController)
		{
			if (CurrentWeaponData->RecoilCameraShake)
			{
				PlayerController->ClientStartCameraShake(CurrentWeaponData->RecoilCameraShake);
			}
		}

		AMainCharacter* OwnerCharacter = Cast<AMainCharacter>(GetOwner());
		if (OwnerCharacter)
		{
			UPlayerUIComponent* UIComponent = OwnerCharacter->FindComponentByClass<UPlayerUIComponent>();
			if (UIComponent)
			{
				float CurrentAimSize = UIComponent->GetAimSize();
				UIComponent->SetAimSize(CurrentAimSize + CurrentWeaponData->CrosshairRecoilAmount);
			}
		}
	}
}



void AWeaponBase::FireHitScan()
{
	FVector FireDirection = CalculateHitScanDirection();
	if (FireDirection.IsZero()) return;
	
	FVector TraceStart;
	FRotator UnusedRotation;
	GetOwner()->GetInstigatorController()->GetPlayerViewPoint(TraceStart,UnusedRotation);
	FVector TraceEnd = TraceStart + FireDirection * CurrentWeaponData->Range;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(this);

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams);

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	
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

FVector AWeaponBase::CalculateHitScanDirection() const
{
	AController* OwnerController = GetOwner()->GetInstigatorController();
	if (!OwnerController) return FVector::ZeroVector;

	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);

	FVector FireDirection = EyeRotation.Vector();
	if (CurrentWeaponData&& CurrentWeaponData->SpreadAngle > 0.0f)
	{
		const float ConeHalfAngleRad = FMath::DegreesToRadians(CurrentWeaponData->SpreadAngle * 0.5f);
		FireDirection = FMath::VRandCone(FireDirection, ConeHalfAngleRad);
	}
	
	return FireDirection;
}

void AWeaponBase::FireProjectile()
{
}


void AWeaponBase::LoadWeaponData()
{
	if (!WeaponDataTable) return;
	if (WeaponDataRowName.IsNone()) return;

	CurrentWeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponDataRowName, TEXT(""));
	CurrentAmmo = CurrentWeaponData->MagazineSize;
}

void AWeaponBase::FinishReload()
{
	if (!CurrentWeaponData) return;

	CurrentAmmo = CurrentWeaponData->MagazineSize;
	bIsReloading = false;
	UE_LOG(LogTemp, Warning, TEXT("Reload Finish"));
}



