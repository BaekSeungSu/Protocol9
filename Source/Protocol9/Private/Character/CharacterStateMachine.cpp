
#include "Character/CharacterStateMachine.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Weapons/InventoryComponent.h"

UCharacterStateMachine::UCharacterStateMachine()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterStateMachine::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AMainCharacter>(GetOwner());

	if (Owner)
	{
		UHPComponent* HPComp = Owner->GetHPComponent();
		if (HPComp)
		{
			HPComp->OnDeathEvent.AddDynamic(this,&UCharacterStateMachine::HandleCharacterDeath);
		}
		UInventoryComponent* InventoryComp = Owner->GetInventoryComponent();
		if (InventoryComp)
		{
			InventoryComp->WeaponChanged.AddDynamic(this,&UCharacterStateMachine::HandleWeaponChange);
		}
	}

}

void UCharacterStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UCharacterStateMachine::HandleCharacterDeath()
{
	SetHPState(EHPState::Dead);
	StopCurrentMontage();
	
	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (AnimInstance && DeathMontage)
		{
			float duration = AnimInstance->Montage_Play(DeathMontage);

		}
	}
}

void UCharacterStateMachine::HandleWeaponChange(EWeaponType NewWeaponType)
{
	CurrentWeapon = NewWeaponType;
	FString EnumAsString = UEnum::GetDisplayValueAsText(CurrentWeapon).ToString();
}

void UCharacterStateMachine::SetState(ECharacterState NewState)
{
	if (NewState == CurrentState) return;

	ECharacterState OldState = CurrentState;


	CurrentState = NewState;
	
	OnStateChanged.Broadcast(OldState, CurrentState);
}

void UCharacterStateMachine::ResetState()
{
	SetState(ECharacterState::Idle);
}

void UCharacterStateMachine::SetHPState(EHPState NewHPState)
{
	if (NewHPState == CurrentHPState) return;

	if (NewHPState == EHPState::LowHealth && CurrentHPState == EHPState::NormalHealth)
	{
		LowHealthEvent.Broadcast();
	}
	
	CurrentHPState = NewHPState;
	
}

void UCharacterStateMachine::ResetHPState()
{
	SetHPState(EHPState::NormalHealth);
}

void UCharacterStateMachine::StopCurrentMontage()
{
	if (!Owner) return;

	Owner->SetupDeathCamera();
	Owner->ShowDefalutMesh();
	
	if (UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_Stop(0.25f);
		}
	}
}

