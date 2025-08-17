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

class UTexture2D; //UI
class AProjectilePool;

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

	void AttachToOwnerSocket();
	void LoadWeaponData();
	void SetOwningCharacter(AMainCharacter* NewOwner);
	UAnimMontage* GetFireMontage() const;
	UAnimMontage* GetReloadMontage() const;
	
	void FireAction();
	void CancelReload();
	
	int32 GetCurrentAmmo() const {return CurrentAmmo;}
	UDataTable* GetWeaponDataTable() const {return WeaponDataTable;}
	FName GetWeaponDataRowName() const {return WeaponDataRowName;}

	// UI : 무기 이미지 블루프린트 연동
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	UTexture2D* UIIcon = nullptr;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	UChildActorComponent* ProjectilePoolChild;
	UPROPERTY(Transient)
	AProjectilePool* ProjectilePool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly, Category="Data")
	UDataTable* WeaponDataTable;
	UPROPERTY(EditDefaultsOnly, Category="Data")
	FName WeaponDataRowName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 CurrentAmmo;
	
	UPROPERTY()
	AMainCharacter* OwningCharacter;
	
	FWeaponData* CurrentWeaponData;
	FTimerHandle ReloadTimerHandle;
	float LastFireTime;
	bool bIsFullAuto;
	
	// 발사 관련 함수
	bool CanFire() const;
	void ApplyRecoil();
	
	// HitScan 관련 함수
	void FireHitScan();
	FVector CalculateFireDirection() const;
	
	// Projectile 관련 함수
	void FireProjectile();

	void ProcessHit(const FHitResult& HitResult, const FVector& ShotDirection);

	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void FinishReload();
	
private:
};
