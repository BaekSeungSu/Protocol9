#include "LevelUp/LevelUpComponent.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ControlComponent.h"
#include "LevelUp/LevelUpUserWidget.h"
#include "Character/StaminaComponent.h"
#include "Blueprint/UserWidget.h"
#include "LevelUp/LevelUpRow.h"
#include "UI/PlayerUIComponent.h"




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
		PlayerUIComp= MyCharacter->FindComponentByClass<UPlayerUIComponent>(); 

		// 캐릭터의 레벨업 이벤트에 현재 컴포넌트의 OnCharacterLeveledUp 함수를 바인딩
		MyCharacter->LevelUPEvent.AddDynamic(this, &ULevelUpComponent::OnCharacterLeveledUp);
	}

	//스탯 레벨 초기화 
	StatLevels.Add(TEXT("Speed Bonus"),   0);
	StatLevels.Add(TEXT("Damage Bonus"),  0);
	StatLevels.Add(TEXT("Health Bonus"),  0);
	StatLevels.Add(TEXT("CDDash"),  0);

	// bIsLevelUpOpen: 현재 레벨업 UI가 떠 있는지
	// PendingLevelUps: UI가 열린 동안 추가로 들어온 레벨업 개수
	bIsLevelUpOpen = false;
	PendingLevelUps = 0;
}

int32 ULevelUpComponent::GetStatLevel(FName StatName) const
{
	// 현재 스탯 강화 레벨 조회. 키가 없으면 0 리턴
	if (const int32* Found = StatLevels.Find(StatName)) return *Found;
	return 0;
}

void ULevelUpComponent::IncrementStatLevel(const FName& StatName)
{
	// 해당 스탯 레벨 +1 (상한 10)
	int32& Lvl = StatLevels.FindOrAdd(StatName);
	Lvl = FMath::Clamp(Lvl + 1, 0, 10);
}

void ULevelUpComponent::OnCharacterLeveledUp(int32 CharacterLevel)
{
	//이미 UI가 떠있으면 대기카운트만 증가 
	if (bIsLevelUpOpen)
	{
		++PendingLevelUps;
		return;
	}
	//최초 한번만 일시정지
	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* WS = World->GetWorldSettings())
		{
			WS->SetTimeDilation(0.0f);
		}
	}
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
	if (PlayerUIComp) { PlayerUIComp->HideCrosshair(); }        //크로스헤어 숨김 
	// 데이터 테이블에서 모든 레벨업 옵션 가져오기
    TArray<FLevelUpRow*> AllRows;
    static const FString ContextString = TEXT("LevelUp");
    LevelUpDataTable->GetAllRows<FLevelUpRow>(ContextString, AllRows);

	// 유효한 옵션만 리스트에 저장
    TArray<FLevelUpRow> AvailableOptions;
    for (FLevelUpRow* Row : AllRows)
    {
    	if (Row) 
    	{
    		AvailableOptions.Add(*Row);
    	}
    }

	// 랜덤하게 최대 3개의 옵션을 뽑기
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

	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		//남아있던 위젯 정리
		if (IsValid(LevelUpUserWidget))
		{
			LevelUpUserWidget->RemoveFromRoot();
			LevelUpUserWidget = nullptr;
		}
		// 실제 레벨업 위젯 생성
		ULevelUpUserWidget* Widget = CreateWidget<ULevelUpUserWidget>(PlayerController, LevelUpUserWidgetClass);
		if (Widget)
		{
			Widget ->SetLevelUpOptions(ChosenOptions,this);
			Widget->AddToViewport();

			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;

			// 멤버에 보관(재사용/제거/검사용)
			LevelUpUserWidget = Widget;
			bIsLevelUpOpen = true;
		}
	}
}

void ULevelUpComponent::ApplyLevelUpChoice(FLevelUpRow ChosenOption)
{
    if (ChosenOption.Name==TEXT("Speed Bonus"))
    {
       ApplySpeedStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("Damage Bonus"))
    {
       ApplyAttackStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("Health Bonus"))
    {
       ApplyHealthStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("CDDash"))
    {
       ApplyStaminaStat(ChosenOption.Value);
    }
	IncrementStatLevel(ChosenOption.Name);

	//Ui가 떠 있는 동안 추가 레벨업이 있는 경우
	if (PendingLevelUps>0)
	{
		// UI는 닫지 말고 같은 위젯에 “새 옵션만” 다시 채워서 한 번 더 고르게 함
		--PendingLevelUps;
		BuildAndSendOptions();   //새로운 옵션을 뽑아 위젯에 세팅하는 함수 
		return;
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
			UE_LOG(LogTemp, Display, TEXT("LevelUp Level Up Event END!!"));
		}
	}
	if (PlayerUIComp) { PlayerUIComp->ShowCrosshair(); }  // 크로스헤어 다시 표시
	bIsLevelUpOpen = false;
}

void ULevelUpComponent::BuildAndSendOptions()
{
	if (!LevelUpDataTable || !IsValid(LevelUpUserWidget)) return;

	TArray<FLevelUpRow*> AllRows;
	static const FString ContextString = TEXT("LevelUp");
	LevelUpDataTable->GetAllRows<FLevelUpRow>(ContextString, AllRows);

	TArray<FLevelUpRow> AvailableOptions;
	for (FLevelUpRow* Row : AllRows) if (Row) AvailableOptions.Add(*Row);

	TArray<FLevelUpRow> ChosenOptions;
	const int32 OptionsToChoose = FMath::Min(3, AvailableOptions.Num());
	for (int32 i = 0; i < OptionsToChoose; ++i)
	{
		const int32 RandomIndex = FMath::RandRange(0, AvailableOptions.Num() - 1);
		ChosenOptions.Add(AvailableOptions[RandomIndex]);
		AvailableOptions.RemoveAt(RandomIndex);
	}

	LevelUpUserWidget->SetLevelUpOptions(ChosenOptions, this);
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
