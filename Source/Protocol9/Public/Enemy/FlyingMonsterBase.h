#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "FlyingMonsterBase.generated.h"


UCLASS()
class PROTOCOL9_API AFlyingMonsterBase : public AMonsterBase
{
	GENERATED_BODY()

public:
	AFlyingMonsterBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Projectile")
	TSubclassOf<class AMonsterProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Flying")
	float FlyingHeight = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Combat")
	FVector ProjectileSpawnOffset = FVector(0.0, 0.0f, -100.0f);

	UFUNCTION()
	void SpawnProjectile();
	
protected:
	virtual void BeginPlay() override;
	void InitializeMovement();
	virtual FVector GetTargetMonsterLocation() const override;
	virtual void PerformAttack() override;
	
	virtual bool IsInAttackRange(float ExtraDistance) const override;
	virtual void MoveToTarget() override;
private:
	FVector GetProjectileSpawnLocation() const;
};
