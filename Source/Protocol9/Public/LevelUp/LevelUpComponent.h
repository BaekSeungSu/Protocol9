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

	// 생성될 위젯의 클래스 정보를 저장하는 변수
	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> LevelUpUserWidgetClass; 
	// 생성된 위젯 인스턴스를 저장하는 변수
	UPROPERTY()
	class ULevelUpUserWidget* LevelUpUserWidget = nullptr;

	void IncrementStatLevel(const FName& StatName);
	
	UFUNCTION()
	void OnCharacterLeveledUp(int32 CharacterLevel);

	// 레벨업 UI를 띄우는 함수
	void ShowLevelUpUI();

	// UI에서 선택한 항목의 스탯을 적용하는 함수 (블루프린트에서 호출 가능)
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
