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

	// 현재 선택 가능한 레벨업 옵션 리스트
	UPROPERTY(BlueprintReadWrite, Category = "LevelUp")
	TArray<FLevelUpRow> LevelUpOptions;

	// 선택된 레벨업 옵션을 적용하는 함수
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

	// 레벨업 관련 데이터가 저장된 데이터 테이블
	UPROPERTY(EditAnywhere,Category="Data")
	UDataTable* LevelUpDataTable;

	// 생성할 레벨업 UI 위젯 클래스 
	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<UUserWidget> LevelUpUserWidgetClass; 

	// 생성된 레벨업 UI 위젯 인스턴스
	UPROPERTY()
	UUserWidget* LevelUpUserWidget;

	// 캐릭터가 레벨업했을 때 호출되는 함수
	UFUNCTION()
	void OnCharacterLeveledUp(int32 CharacterLevel);

	// 레벨업 UI를 띄우는 함수
	void ShowLevelUpUI();
	
	
	void ApplyAttackStat(float Value);
	void ApplyHealthStat(float Value);
	void ApplyStaminaStat(float Value);
	void ApplySpeedStat(float Value);
		
};
