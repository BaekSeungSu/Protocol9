#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup_Weapon.generated.h"

class UParticleSystemComponent;
class AMainCharacter;
class AWeaponBase;
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class PROTOCOL9_API APickup_Weapon : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup_Weapon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	bool TryConsume(AMainCharacter* Interactor);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* SphereCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UParticleSystemComponent* PickupEffect;
	UPROPERTY()
	AMainCharacter* OverlappingCharacter;
	
	bool bConsumed;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void BeginPlay() override;

	

private:
};
