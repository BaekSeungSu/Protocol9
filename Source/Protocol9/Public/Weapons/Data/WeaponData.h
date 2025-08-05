#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapons/EWeaponType.h"
#include "Weapons/EFireType.h"
#include "Weapons/WeaponBase.h"
#include "Weapons/BaseProjectile.h"
#include "WeaponData.generated.h"


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
		  ReloadTime(0),
		  SpreadAngle(0),
		  CrosshairRecoilAmount(0),
		  RecoilCameraShake(nullptr)
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	float ReloadTime;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float SpreadAngle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	float CrosshairRecoilAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Recoil")
	TSubclassOf<UCameraShakeBase> RecoilCameraShake;
};

