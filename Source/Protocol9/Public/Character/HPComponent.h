#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/MainCharacter.h"
#include "HPComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHPChangedSignature, float, CurrentHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API UHPComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HP")
	float MaxHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HP")
	float CurrentHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HP")
	bool bIsDead;
	bool bIsInvisible = false;
	
public:	
	UHPComponent();
	//이벤트
	UPROPERTY(BlueprintAssignable, Category = "HP")
	FOnDeathSignature OnDeathEvent;
	UPROPERTY(BlueprintAssignable, Category = "HP")
	FHPChangedSignature HPChanged;
	
	UFUNCTION(BlueprintPure, Category = "HP")
	float GetMaxHP() const {return MaxHP;};
	UFUNCTION(BlueprintPure, Category = "HP")
	float GetCurrentHP() const {return CurrentHP;};
	UFUNCTION(BlueprintPure, Category = "HP")
	bool GetIsDead() const {return bIsDead;}

	UFUNCTION(BlueprintCallable, Category = "HP")
	void SetMaxHP(float NewMaxHP);
	UFUNCTION(BlueprintCallable, Category = "HP")
	void SetCurrentHP(float NewCurrentHP);
	UFUNCTION(BlueprintCallable, Category = "HP")
	void HandleDamage(float DamageAmount, 
						struct FDamageEvent const& DamageEvent, 
						class AController* EventInstigator, 
						AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "HP")
	void AddHealth(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "HP")
	void OnDeath();

	UFUNCTION(BlueprintCallable, Category = "HP")
	void LockHealth();
	UFUNCTION(BlueprintCallable, Category = "HP")
	void UnlockHealth();
	
protected:
	virtual void BeginPlay() override;
};
