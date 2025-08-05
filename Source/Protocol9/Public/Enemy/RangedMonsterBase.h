#pragma once

#include "CoreMinimal.h"
#include "MonsterBase.h"
#include "RangedMonsterBase.generated.h"

UCLASS()
class PROTOCOL9_API ARangedMonsterBase : public AMonsterBase
{
	GENERATED_BODY()

public:
	ARangedMonsterBase();

protected:
	virtual void PerformAttack() override;
	virtual void ChasePlayer() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Combat")
	TSubclassOf<class AMonsterProjectile> ProjectileClass;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Combat")
	FVector ProjectileSpawnOffset = FVector(100.0f, 0.0f, 50.0f);
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Combat")
	float ProjectileArcHeight = 350.0f;

	bool HasLineOfSightToPlayer();
	FVector GetProjectileSpawnLocation();
};
