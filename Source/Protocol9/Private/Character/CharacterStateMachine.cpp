
#include "Character/CharacterStateMachine.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"

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
	}

}

void UCharacterStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UCharacterStateMachine::HandleCharacterDeath()
{
	SetState(ECharacterState::Dead);
	StopCurrentMontage();
	
	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	{
		if (AnimInstance && DeathMontage)
		{
			float duration = AnimInstance->Montage_Play(DeathMontage);

		}
	}
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

