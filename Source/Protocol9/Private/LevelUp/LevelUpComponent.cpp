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
	//게임 시간 정지
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.0f);
	//레벨업 UI 함수 호출
	ShowLevelUpUI();
}

void ULevelUpComponent::ShowLevelUpUI()
{
	// 1. 레벨업 데이터 테이블에서 랜덤으로 선택지 3개를 가져오는 로직 구현
	// 2. 이 선택지들을 UI 위젯에 전달
	// 3. UI 위젯을 생성하고 화면에 추가 (Add to Viewport)
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
