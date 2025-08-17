#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectilePool.generated.h"

class ABaseProjectile;
UCLASS()
class PROTOCOL9_API AProjectilePool : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectilePool();

	UFUNCTION(BlueprintCallable, Category = "Pool")
	ABaseProjectile* Acquire(const FVector& SpawnLocation,
							 const FRotator& SpawnRotation,
							 const FVector& InitialVelocity,
							 AActor* InOwner = nullptr,
							 APawn* InInstigator = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void Release(ABaseProjectile* Projectile);
	
	UFUNCTION(BlueprintCallable, Category = "Pool")
	int32 GetActiveCount() const {return ActiveProjectiles.Num();}
	UFUNCTION(BlueprintCallable, Category = "Pool")
	int32 GetInActiveCount() const {return InactiveProjectiles.Num();}
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSubclassOf<ABaseProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 InitialSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bAutoExpand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 MaxSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 ExpandStep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bAttachPooledAsChild;
	

private:
	UPROPERTY(Transient)
	TArray<ABaseProjectile*> InactiveProjectiles;
	UPROPERTY(Transient)
	TSet<ABaseProjectile*> ActiveProjectiles;
	
	void Prewarm(int32 Count);
	bool TryExpand(int32 NeededCount);
	ABaseProjectile* SpawnOne();

	void LogConfigErrorsIfAny() const;

};
