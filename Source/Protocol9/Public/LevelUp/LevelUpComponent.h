#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelUp/LevelUpRow.h"
#include "LevelUpComponent.generated.h"

class UHPComponent;
class UControlComponent;
class AMainCharacter;
class UStaminaComponent;
class UPlayerUIComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API ULevelUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelUpComponent();

	UPROPERTY(BlueprintReadOnly, Category="LevelUp")
	TMap<FName, int32> StatLevels;

	UFUNCTION(BlueprintCallable, Category="LevelUp")
	int32 GetStatLevel(FName StatName) const;

	
protected:
	
	virtual void BeginPlay() override;


private:
	UPROPERTY()
	UHPComponent* HPComp;
	UPROPERTY()
	UControlComponent* SpeedComp;
	UPROPERTY()
	UStaminaComponent* StaminaComp;
	UPROPERTY()
	AMainCharacter* MyCharacter;

	UPROPERTY(EditAnywhere,Category="Data")
	UDataTable* LevelUpDataTable;

	// ������ ������ Ŭ���� ������ �����ϴ� ����
	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> LevelUpUserWidgetClass; 
	// ������ ���� �ν��Ͻ��� �����ϴ� ����
	UPROPERTY()
	class ULevelUpUserWidget* LevelUpUserWidget = nullptr;

	void IncrementStatLevel(const FName& StatName);
	
	UFUNCTION()
	void OnCharacterLeveledUp(int32 CharacterLevel);

	// ������ UI�� ���� �Լ�
	void ShowLevelUpUI();

	// UI���� ������ �׸��� ������ �����ϴ� �Լ� (�������Ʈ���� ȣ�� ����)
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void ApplyLevelUpChoice(FLevelUpRow ChosenOption);
	
	void ApplyAttackStat(float Value);
	void ApplyHealthStat(float Value);
	void ApplyStaminaStat(float Value);
	void ApplySpeedStat(float Value);
	
	UPROPERTY()
	UPlayerUIComponent* PlayerUIComp = nullptr;

	bool bIsLevelUpOpen = false;
	int32 PendingLevelUps = 0 ;
	
	void BuildAndSendOptions();
};
