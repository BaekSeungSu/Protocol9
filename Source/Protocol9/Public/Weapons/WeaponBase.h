#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponInterface.h"
#include "WeaponBase.generated.h"

class AMainCharacter;
class UStaticMeshComponent;
class UDataTable;
class UAnimMontage;
struct FWeaponData;

UCLASS()
class PROTOCOL9_API AWeaponBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void PrimaryFire_Implementation() override;
	virtual void StopFire_Implementation() override;
	virtual void Reload_Implementation() override;
	virtual void Tick(float DeltaTime) override;

	int32 GetCurrentAmmo() const {return CurrentAmmo;}
	void SetOwningCharacter(AMainCharacter* NewOwner);
	UAnimMontage* GetFireMontage() const;
	UAnimMontage* GetReloadMontage() const;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly, Category="Data")
	UDataTable* WeaponDataTable;
	UPROPERTY(EditDefaultsOnly, Category="Data")
	FName WeaponDataRowName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 CurrentAmmo;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="State")
	bool bIsReloading;
	UPROPERTY()
	AMainCharacter* OwningCharacter;
	
	const FWeaponData* CurrentWeaponData;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime;

	
	// 발사 관련 함수
	void FireAction();
	bool CanFire() const;
	void ApplyRecoil();

	// HitScan 관련 함수
	void FireHitScan();
	FVector CalculateHitScanDirection() const;
	
	// Projectile 관련 함수
	void FireProjectile();

	void ProcessHit(const FHitResult& HitResult, const FVector& ShotDirection);

	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void LoadWeaponData();
	void FinishReload();

	
	
private:
};
