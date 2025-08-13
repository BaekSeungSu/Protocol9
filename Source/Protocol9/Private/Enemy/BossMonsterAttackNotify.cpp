// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossMonsterAttackNotify.h"
#include "Character/MainCharacter.h"
#include "Enemy/BossMonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UBossMonsterAttackNotify::UBossMonsterAttackNotify()
{
	BoxTraceHalfSize = FVector(500.f, 1000.f, 500.f);

	BoxTraceDistance = 1000.f;
	
	Damage = 30.f;
}

void UBossMonsterAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	UE_LOG(LogTemp, Warning, TEXT("Notify Called"));
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshComp is null"));
		return;
	}
		
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is null"));
		return;
	}
	ABossMonsterBase* Boss = Cast<ABossMonsterBase>(Owner);
	if (!Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss is null"));
		return;
	}

	const FVector StartLocation = Boss->GetActorLocation();
	const FVector EndLocation = StartLocation + (Boss->GetActorForwardVector() * BoxTraceDistance);
	
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Boss);
	
    TArray<FHitResult> HitResults;
	UE_LOG(LogTemp, Warning, TEXT("BoxTraceMultiForObjects"));
    bool bHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
        Boss,
        StartLocation,          
        EndLocation,            
        BoxTraceHalfSize,       
        Boss->GetActorRotation(), 
        ObjectTypes,            
        false,                 
        ActorsToIgnore,         
        EDrawDebugTrace::ForDuration, 
        HitResults,           
        true                    
    );
	
    if (bHit)
    {
		UE_LOG(LogTemp, Warning, TEXT("BoxTraceMultiForObjects Hit"));
    	TSet<AActor*> DamagedActors;

        for (const FHitResult& Hit : HitResults)
        {
            AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(Hit.GetActor());
            if (PlayerCharacter && !DamagedActors.Contains(PlayerCharacter))
            {
                UGameplayStatics::ApplyDamage(
                    PlayerCharacter,           
                    Damage,                   
                    Boss->GetController(),      
                    Boss,               
                    UDamageType::StaticClass()  
                );
                UE_LOG(LogTemp, Warning, TEXT("%s에게 %f의 데미지를 입혔습니다!"), *PlayerCharacter->GetName(), Damage);
            }
        	DamagedActors.Add(PlayerCharacter);
        }
    	DamagedActors.Empty();
    }
}

