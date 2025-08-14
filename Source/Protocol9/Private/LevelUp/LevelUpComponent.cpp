#include "LevelUp/LevelUpComponent.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Character/ControlComponent.h"
#include "Character/StaminaComponent.h"
#include "Blueprint/UserWidget.h"




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
	//���� �ð� ����
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.0f);
	//������ UI �Լ� ȣ��
	ShowLevelUpUI();
}

void ULevelUpComponent::ShowLevelUpUI()
{
	// 1. ������ ������ ���̺��� �������� ������ 3���� �������� ���� ����
	// 2. �� ���������� UI ������ ����
	// 3. UI ������ �����ϰ� ȭ�鿡 �߰� (Add to Viewport)
	// LevelUpUIWidget = CreateWidget<UUserWidget>(GetWorld(), ...);
	// LevelUpUIWidget->AddToViewport();
}

void ULevelUpComponent::ApplyLevelUpChoice(const FLevelUpRow& ChoseOption)
{
	if (ChoseOption.Name==TEXT("IncreaseSpeed"))
	{
		ApplySpeedStat(ChoseOption.Value);
	}
	else if (ChoseOption.Name==TEXT("IncreaseDamage"))
	{
		ApplyAttackStat(ChoseOption.Value);
	}
	else if (ChoseOption.Name==TEXT("IncreaseHealth"))
	{
		ApplyHealthStat(ChoseOption.Value);
	}
	else if (ChoseOption.Name==TEXT("AddStamina"))
	{
		ApplyStaminaStat(ChoseOption.Value);
	}
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.0f);
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
	
}

void ULevelUpComponent::ApplySpeedStat(float Value)
{
	if (SpeedComp)
	{
		SpeedComp->SetMaxSpeed(SpeedComp->GetMaxSpeed()+Value);
	}
}
