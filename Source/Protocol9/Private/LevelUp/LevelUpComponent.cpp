#include "LevelUp/LevelUpComponent.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ControlComponent.h"
#include "LevelUp/LevelUpUserWidget.h"
#include "Character/StaminaComponent.h"
#include "Blueprint/UserWidget.h"
#include "LevelUp/LevelUpRow.h"




ULevelUpComponent::ULevelUpComponent()
{
	
	PrimaryComponentTick.bCanEverTick = false;


}


void ULevelUpComponent::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = Cast<AMainCharacter>(GetOwner());
	if (MyCharacter)
	{
		HPComp = MyCharacter->FindComponentByClass<UHPComponent>();
		SpeedComp = MyCharacter->FindComponentByClass<UControlComponent>();
		StaminaComp = MyCharacter->FindComponentByClass<UStaminaComponent>();

		MyCharacter->LevelUPEvent.AddDynamic(this, &ULevelUpComponent::OnCharacterLeveledUp);
	}
}

void ULevelUpComponent::OnCharacterLeveledUp(int32 CharacterLevel)
{
	//게임 시간 정지
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.0f);
	//레벨업 UI 함수 호출
	ShowLevelUpUI();
}

void ULevelUpComponent::ShowLevelUpUI()
{
    if (!LevelUpDataTable || !LevelUpUserWidgetClass)
    {
       UE_LOG(LogTemp, Error, TEXT("LevelUpDataTable or LevelUpUserWidgetClass is not set."));
       return;
    }

    TArray<FLevelUpRow*> AllRows;
    static const FString ContextString = TEXT("LevelUp");
    LevelUpDataTable->GetAllRows<FLevelUpRow>(ContextString, AllRows);

    TArray<FLevelUpRow> AvailableOptions;
    for (FLevelUpRow* Row : AllRows)
    {
    	if (Row) 
    	{
    		AvailableOptions.Add(*Row);
    	}
    }

    TArray<FLevelUpRow> ChosenOptions;
    int32 OptionsToChoose = FMath::Min(3, AvailableOptions.Num());
	if (AvailableOptions.Num() > 0)
	{
		for (int32 i = 0; i < OptionsToChoose; ++i)
		{
			int32 RandomIndex = FMath::RandRange(0, AvailableOptions.Num() - 1);
			ChosenOptions.Add(AvailableOptions[RandomIndex]);
			AvailableOptions.RemoveAt(RandomIndex);
		}
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		ULevelUpUserWidget* Widget = CreateWidget<ULevelUpUserWidget>(PlayerController, LevelUpUserWidgetClass);
		if (Widget)
		{
			// BP 이벤트 호출
			Widget->SetLevelUpOptions(ChosenOptions, this);

			// 화면 표시
			Widget->AddToViewport();
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;

			LevelUpUserWidget = Widget; // 저장
		}
	}
}

void ULevelUpComponent::ApplyLevelUpChoice(FLevelUpRow ChosenOption)
{
    if (ChosenOption.Name==TEXT("IncreaseSpeed"))
    {
       ApplySpeedStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("IncreaseDamage"))
    {
       ApplyAttackStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("IncreaseHealth"))
    {
       ApplyHealthStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("DecreaseDashCD"))
    {
       ApplyStaminaStat(ChosenOption.Value);
    }
    
	if (UWorld* World = GetWorld()) // GetWorld()가 유효한지 확인
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		if(PlayerController)
		{
			// 위젯 제거
			if (IsValid(LevelUpUserWidget))
			{
				LevelUpUserWidget->RemoveFromParent();
				LevelUpUserWidget = nullptr;
			}

			// 입력 모드 복구
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
		}
    
		// GetWorldSettings()를 사용하기 전에 GetWorld()가 유효한지 확인
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->SetTimeDilation(1.0f);
		}
	}
}

void ULevelUpComponent::ApplyAttackStat(float Value)
{
	if (MyCharacter)
	{
		MyCharacter->SetAttack(MyCharacter->GetAttack()+Value);
	}
}

void ULevelUpComponent::ApplyHealthStat(float Value)
{
	if (HPComp)
	{
		HPComp->SetMaxHP(HPComp->GetMaxHP()+Value);
	}
}
void ULevelUpComponent::ApplyStaminaStat(float Value)
{
	if (StaminaComp)
	{
		const float NewTime = FMath::Max(1.0f, StaminaComp->GetStaminaChargeTime() - Value);
		StaminaComp->SetStaminaChargeTime(NewTime);
	}
}

void ULevelUpComponent::ApplySpeedStat(float Value)
{
	if (SpeedComp)
	{
		SpeedComp->SetMaxSpeed(SpeedComp->GetMaxSpeed()+Value);
	}
}
