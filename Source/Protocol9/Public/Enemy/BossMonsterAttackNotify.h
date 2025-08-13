// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BossMonsterAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class PROTOCOL9_API UBossMonsterAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	UBossMonsterAttackNotify();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FVector BoxTraceHalfSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float BoxTraceDistance;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage;
};
