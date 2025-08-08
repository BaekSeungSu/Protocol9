// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/MeleeAttackAnimNotify.h"
#include "Character/MainCharacter.h"
#include "Character/ControlComponent.h"

void UMeleeAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (AMainCharacter* Character = Cast<AMainCharacter>(MeshComp->GetOwner()))
	{
		if (UControlComponent* Control = Character->GetControlComponent())
		{
			Control->MeleeAttack();
		}
	}
}
