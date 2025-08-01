
#include "Character/CharacterStateMachine.h"
#include "Character/MainCharacter.h"

UCharacterStateMachine::UCharacterStateMachine()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterStateMachine::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AMainCharacter>(GetOwner());

}

void UCharacterStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


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

	if (UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_Stop(0.25f);
		}
	}
}

