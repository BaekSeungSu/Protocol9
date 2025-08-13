#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelUp/LevelUpRow.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "LevelUpComponent.generated.h"

class UHPComponent;
class UControlComponent;
class AMainCharacter;
class UStaminaComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROTOCOL9_API ULevelUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelUpComponent();

	UPROPERTY(BlueprintReadWrite, Category = "LevelUp")
	TArray<FLevelUpRow> LevelUpOptions;

	UFUNCTION(BlueprintCallable, Category="LevelUp")
	void ApplyLevelUpChoice(FLevelUpRow ChosenOption);

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
	UUserWidget* LevelUpUserWidget;

	
	TMap<FName, int32> SelectedOptions;
	
	UFUNCTION()
	void OnCharacterLeveledUp(int32 CharacterLevel);

	// ������ UI�� ���� �Լ�
	void ShowLevelUpUI();
	
	
	void ApplyAttackStat(float Value);
	void ApplyHealthStat(float Value);
	void ApplyStaminaStat(float Value);
	void ApplySpeedStat(float Value);
		
};
