#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class AProjectilePool;

UCLASS(Abstract)
class PROTOCOL9_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseProjectile();
	void FireInDirection(const FVector& ShootDirection);
	void ActivateProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FVector& Velocity, AActor* InOwner = nullptr, APawn* InInstigator = nullptr);
	void DeactivateProjectile();
	void ResetProjectile();

	void SetDamage(float NewDamage) {Damage = NewDamage;}
	void SetOwningPool(AProjectilePool* InPool) { OwningPool = InPool;}
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* ProjectileCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* ProjectileEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UNiagaraSystem* HitEffect;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Property")
	float Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Property")
	float LifeTime = 3.0f;
	
	FTimerHandle LifeTimerHandle;
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void ApplyDamageOnHit(const FHitResult& HitResult);
	
	virtual  void OnActivated() {}
	virtual void OnDeactivated() {}
	virtual void OnReset() {}
	
private:
	UPROPERTY()
	AProjectilePool* OwningPool = nullptr;
};
