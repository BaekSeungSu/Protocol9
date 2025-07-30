#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROTOCOL9_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;
	
public:	
	virtual void Tick(float DeltaTime) override;

};
