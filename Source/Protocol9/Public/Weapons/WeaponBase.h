#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WeaponInterface.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;
class UDataTable;
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

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category="Data")
	UDataTable* WeaponDataTable;
	
	UPROPERTY(EditDefaultsOnly, Category="Data")
	FName WeaponDataRowName;

	const FWeaponData* CurrentWeaponData;
	
	void FireAction();
	void FireHitScan();
	void LoadWeaponData();

	
private:
};
