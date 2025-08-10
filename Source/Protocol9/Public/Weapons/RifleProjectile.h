#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseProjectile.h"
#include "RifleProjectile.generated.h"


UCLASS()
class PROTOCOL9_API ARifleProjectile : public ABaseProjectile
{
	GENERATED_BODY()
public:
	ARifleProjectile();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
