#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapons/EWeaponType.h"
#include "Weapons/EFireType.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/BaseProjectile.h"
#include "WeaponData.generated.h"

class UNiagaraSystem;
class UAnimMontage;
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()
public:

	FWeaponData()
		: WeaponType(EWeaponType::Pistol),
		  FireType(EFireType::HitScan),
		  WeaponClass(nullptr),
		  ProjectileClass(nullptr),
		  Damage(0.0f),
		  FireRate(0.0f),
		  Range(0.0f),
		  MagazineSize(0),
		  RecoilPitch(0),
		  RecoilYaw(0),
		  SpreadAngle(0),
		  CrosshairRecoilAmount(0),
		  RecoilCameraShake(nullptr),
		  MuzzleFlash(nullptr),
		  HitParticle(nullptr),
		  FireSound(nullptr),
		  FireMontage(nullptr),
		  ReloadMontage(nullptr),
		  EquipMontage(nullptr),
		  AttachLocationOffset(FVector::ZeroVector),
		  AttachRotationOffset(FRotator::ZeroRotator),
		  AttachScale(1.0f),
		  ProjectileSpeed(0.0f)
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Type")
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Type")
	EFireType FireType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Class")
	TSubclassOf<AWeaponBase> WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Class")
	TSubclassOf<ABaseProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	float FireRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MagazineSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilPitch;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilYaw;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float SpreadAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float CrosshairRecoilAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	TSubclassOf<UCameraShakeBase> RecoilCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|FX")
	UNiagaraSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|FX")
	UNiagaraSystem* HitParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|FX")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Offset")
	FVector AttachLocationOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Offset")
	FRotator AttachRotationOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Offset")
	FVector AttachScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Projectile")
	float ProjectileSpeed;
};

