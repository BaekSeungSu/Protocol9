// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangedProjectileSpawnNotify.h"

void URangedProjectileSpawnNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	{
		if (!MeshComp || !MeshComp->GetOwner())
		{
			return;
		}
		if (ARangedMonsterBase* RangedMonster = Cast<ARangedMonsterBase>(MeshComp->GetOwner()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn Projectile"));
			RangedMonster->SpawnProjectile();
		}
	}
}
