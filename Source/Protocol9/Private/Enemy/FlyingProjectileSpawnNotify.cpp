// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/FlyingProjectileSpawnNotify.h"

void UFlyingProjectileSpawnNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	if (AFlyingMonsterBase* FlyingMonster = Cast<AFlyingMonsterBase>(MeshComp->GetOwner()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Spawn Projectile"));
		FlyingMonster->SpawnProjectile();
	}
}
