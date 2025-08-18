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

		// ĳ������ ������ �̺�Ʈ�� ���� ������Ʈ�� OnCharacterLeveledUp �Լ��� ���ε�
		MyCharacter->LevelUPEvent.AddDynamic(this, &ULevelUpComponent::OnCharacterLeveledUp);
	}

	//���� ���� �ʱ�ȭ 
	StatLevels.Add(TEXT("Speed Bonus"),   0);
	StatLevels.Add(TEXT("Damage Bonus"),  0);
	StatLevels.Add(TEXT("Health Bonus"),  0);
	StatLevels.Add(TEXT("CDDash"),  0);

	// bIsLevelUpOpen: ���� ������ UI�� �� �ִ���
	// PendingLevelUps: UI�� ���� ���� �߰��� ���� ������ ����
	bIsLevelUpOpen = false;
	PendingLevelUps = 0;
}

int32 ULevelUpComponent::GetStatLevel(FName StatName) const
{
	// ���� ���� ��ȭ ���� ��ȸ. Ű�� ������ 0 ����
	if (const int32* Found = StatLevels.Find(StatName)) return *Found;
	return 0;
}

void ULevelUpComponent::IncrementStatLevel(const FName& StatName)
{
	// �ش� ���� ���� +1 (���� 10)
	int32& Lvl = StatLevels.FindOrAdd(StatName);
	Lvl = FMath::Clamp(Lvl + 1, 0, 10);
}

void ULevelUpComponent::OnCharacterLeveledUp(int32 CharacterLevel)
{
	//�̹� UI�� �������� ���ī��Ʈ�� ���� 
	if (bIsLevelUpOpen)
	{
		++PendingLevelUps;
		return;
	}
	//���� �ѹ��� �Ͻ�����
	if (UWorld* World = GetWorld())
	{
		if (AWorldSettings* WS = World->GetWorldSettings())
		{
			WS->SetTimeDilation(0.0f);
		}
	}
	//������ UI �Լ� ȣ��
	ShowLevelUpUI();
}

void ULevelUpComponent::ShowLevelUpUI()
{
    if (!LevelUpDataTable || !LevelUpUserWidgetClass)
    {
       UE_LOG(LogTemp, Error, TEXT("LevelUpDataTable or LevelUpUserWidgetClass is not set."));
       return;
    }
	if (PlayerUIComp) { PlayerUIComp->HideCrosshair(); }        //ũ�ν���� ���� 
	// ������ ���̺����� ��� ������ �ɼ� ��������
    TArray<FLevelUpRow*> AllRows;
    static const FString ContextString = TEXT("LevelUp");
    LevelUpDataTable->GetAllRows<FLevelUpRow>(ContextString, AllRows);

	// ��ȿ�� �ɼǸ� ����Ʈ�� ����
    TArray<FLevelUpRow> AvailableOptions;
    for (FLevelUpRow* Row : AllRows)
    {
    	if (Row) 
    	{
    		AvailableOptions.Add(*Row);
    	}
    }

	// �����ϰ� �ִ� 3���� �ɼ��� �̱�
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

	// �÷��̾� ��Ʈ�ѷ� ��������
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		//�����ִ� ���� ����
		if (IsValid(LevelUpUserWidget))
		{
			LevelUpUserWidget->RemoveFromRoot();
			LevelUpUserWidget = nullptr;
		}
		// ���� ������ ���� ����
		ULevelUpUserWidget* Widget = CreateWidget<ULevelUpUserWidget>(PlayerController, LevelUpUserWidgetClass);
		if (Widget)
		{
			Widget ->SetLevelUpOptions(ChosenOptions,this);
			Widget->AddToViewport();

			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;

			// ����� ����(����/����/�˻��)
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

	//Ui�� �� �ִ� ���� �߰� �������� �ִ� ���
	if (PendingLevelUps>0)
	{
		// UI�� ���� ���� ���� ������ ���� �ɼǸ��� �ٽ� ä���� �� �� �� ������ ��
		--PendingLevelUps;
		BuildAndSendOptions();   //���ο� �ɼ��� �̾� ������ �����ϴ� �Լ� 
		return;
	}
	if (UWorld* World = GetWorld()) // GetWorld()�� ��ȿ���� Ȯ��
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
		if(PlayerController)
		{
			// ���� ����
			if (IsValid(LevelUpUserWidget))
			{
				LevelUpUserWidget->RemoveFromParent();
				LevelUpUserWidget = nullptr;
			}

			// �Է� ��� ����
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
		}
    
		// GetWorldSettings()�� ����ϱ� ���� GetWorld()�� ��ȿ���� Ȯ��
		if (AWorldSettings* WorldSettings = World->GetWorldSettings())
		{
			WorldSettings->SetTimeDilation(1.0f);
			UE_LOG(LogTemp, Display, TEXT("LevelUp Level Up Event END!!"));
		}
	}
	if (PlayerUIComp) { PlayerUIComp->ShowCrosshair(); }  // ũ�ν���� �ٽ� ǥ��
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
