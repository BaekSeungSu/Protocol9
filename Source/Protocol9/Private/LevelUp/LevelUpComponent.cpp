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

		// ĳ������ ������ �̺�Ʈ�� ���� ������Ʈ�� OnCharacterLeveledUp �Լ��� ���ε�
		MyCharacter->LevelUPEvent.AddDynamic(this, &ULevelUpComponent::OnCharacterLeveledUp);
	}
}

void ULevelUpComponent::OnCharacterLeveledUp(int32 CharacterLevel)
{
	//���� �ð� ����
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.0f);
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
	// ������ ���̺��� ��� ������ �ɼ� ��������
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
		// ������ ���� UI ���� ����
		ULevelUpUserWidget* Widget = CreateWidget<ULevelUpUserWidget>(PlayerController, LevelUpUserWidgetClass);
		if (Widget)
		{
			// ������ ���õ� �ɼ� ���� + �ڱ� �ڽ� ���� ���� (���� �� ȣ���)
			Widget->SetLevelUpOptions(ChosenOptions, this);

			// ȭ�� ǥ��
			Widget->AddToViewport();
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;

			LevelUpUserWidget = Widget; // ����
		}
	}
}

void ULevelUpComponent::ApplyLevelUpChoice(FLevelUpRow ChosenOption)
{
    if (ChosenOption.Name==TEXT("Increase Speed"))
    {
       ApplySpeedStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("Increase Damage"))
    {
       ApplyAttackStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("Increase Health"))
    {
       ApplyHealthStat(ChosenOption.Value);
    }
    else if (ChosenOption.Name==TEXT("Decrease DashCD"))
    {
       ApplyStaminaStat(ChosenOption.Value);
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
