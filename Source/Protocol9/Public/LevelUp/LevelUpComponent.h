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

	// ���� ���� ������ ������ �ɼ� ����Ʈ
	UPROPERTY(BlueprintReadWrite, Category = "LevelUp")
	TArray<FLevelUpRow> LevelUpOptions;

	// ���õ� ������ �ɼ��� �����ϴ� �Լ�
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

	// ������ ���� �����Ͱ� ����� ������ ���̺�
	UPROPERTY(EditAnywhere,Category="Data")
	UDataTable* LevelUpDataTable;

	// ������ ������ UI ���� Ŭ���� 
	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> LevelUpUserWidgetClass; 

	// ������ ������ UI ���� �ν��Ͻ�
	UPROPERTY()
	UUserWidget* LevelUpUserWidget;

	// ĳ���Ͱ� ���������� �� ȣ��Ǵ� �Լ�
	UFUNCTION()
	void OnCharacterLeveledUp(int32 CharacterLevel);

	// ������ UI�� ���� �Լ�
	void ShowLevelUpUI();
	
	
	void ApplyAttackStat(float Value);
	void ApplyHealthStat(float Value);
	void ApplyStaminaStat(float Value);
	void ApplySpeedStat(float Value);
		
};
