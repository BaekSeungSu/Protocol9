#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};


class PROTOCOL9_API IWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category= "Weapon")
	void PrimaryFire();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category= "Weapon")
	void StopFire();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category= "Weapon")
	void Reload();
	
};
