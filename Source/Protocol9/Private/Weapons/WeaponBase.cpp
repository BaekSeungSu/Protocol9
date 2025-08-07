#include "Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/Data/WeaponData.h"
#include "UI/PlayerUIComponent.h"
#include "Character/MainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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
	if (!CanFire()) return;
	
	LastFireTime = GetWorld()->GetTimeSeconds();
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

	if (CurrentWeaponData->MuzzleFlash)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeaponData->MuzzleFlash, WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket")));
	}
	if (CurrentWeaponData->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeaponData->FireSound, GetActorLocation());
	}
}

bool AWeaponBase::CanFire() const
{
	if (bIsReloading || CurrentAmmo <=0 || CurrentWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not Fire"));
		return false;
	}

	const float TimeBetweenShots = 60.0f / CurrentWeaponData->FireRate;
	if (GetWorld()->GetTimeSeconds() < LastFireTime + TimeBetweenShots)
	{
		return false;
	}
	
	return true;
}

void AWeaponBase::ApplyRecoil()
{
	APlayerController* PlayerController = GetOwner()->GetInstigatorController<APlayerController>();
	if (!PlayerController || !CurrentWeaponData) return;

	const float Pitch = -CurrentWeaponData->RecoilPitch;
	const float Yaw = FMath::RandRange(-CurrentWeaponData->RecoilYaw, CurrentWeaponData->RecoilYaw);
	PlayerController->AddPitchInput(Pitch);
	PlayerController->AddYawInput(Yaw);
	
	if (CurrentWeaponData->RecoilCameraShake)
	{
		PlayerController->ClientStartCameraShake(CurrentWeaponData->RecoilCameraShake);
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
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Pawn, CollisionParams);

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	
	if (bHit)
	{
		ProcessHit(HitResult,FireDirection);
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
	AMainCharacter* OwnerCharacter = Cast<AMainCharacter>(GetOwner());
	if (!OwnerController || !OwnerCharacter || !CurrentWeaponData)
	{
		return FVector::ZeroVector;
	}
	
	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
	FVector FireDirection = EyeRotation.Vector();

	float BaseSpreadAngle = CurrentWeaponData->SpreadAngle;
	float DynamicSpread = 0.0f;
	UPlayerUIComponent* UIComponent = OwnerCharacter->FindComponentByClass<UPlayerUIComponent>();
	if (UIComponent)
	{
		DynamicSpread = UIComponent->GetAimSize() * 0.01f;
	}
	float FinalSpreadAngle = BaseSpreadAngle + DynamicSpread;

	if (FinalSpreadAngle > 0.0f)
	{
		const float ConeHalfAngle = FMath::DegreesToRadians(FinalSpreadAngle * 0.5f);
		FireDirection = FMath::VRandCone(FireDirection, ConeHalfAngle);
	}
	
	return FireDirection;
}

void AWeaponBase::FireProjectile()
{
}

void AWeaponBase::ProcessHit(const FHitResult& HitResult, const FVector& ShotDirection)
{
	AActor* HitActor = HitResult.GetActor();
	if (HitActor)
	{
		AController* OwnerController = GetOwner()->GetInstigatorController();
		UGameplayStatics::ApplyPointDamage(HitActor, CurrentWeaponData->Damage, ShotDirection, HitResult, OwnerController, this, UDamageType::StaticClass());
	}

	if (CurrentWeaponData->HitParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeaponData->HitParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
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



