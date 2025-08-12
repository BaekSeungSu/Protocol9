#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseProjectile.h"
#include "PulseProjectile.generated.h"

class USoundBase;
UCLASS()
class PROTOCOL9_API APulseProjectile : public ABaseProjectile
{
	GENERATED_BODY()
public:
	APulseProjectile();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float DamageRadius;
	
};
