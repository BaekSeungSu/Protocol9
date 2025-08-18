// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossMonsterPattern1Notify.h"
#include "Character/MainCharacter.h"
#include "Enemy/BossMonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UBossMonsterPattern1Notify::UBossMonsterPattern1Notify()
{
	BoxTraceHalfSize = FVector(700.f, 300.f, 400.f);
	
	Damage = 30.f;
}

void UBossMonsterPattern1Notify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MeshComp is null"));
		return;
	}
		
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Owner is null"));
		return;
	}
	ABossMonsterBase* Boss = Cast<ABossMonsterBase>(Owner);
	if (!Boss)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Boss is null"));
		return;
	}
	
	FVector BoxCenter = Boss->GetActorLocation() + Boss->GetActorForwardVector() * (BoxTraceHalfSize.X);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Boss);
	
	TArray<AActor*> OverlappedActors;

	//UE_LOG(LogTemp, Warning, TEXT("BoxOverlapActors"));
	bool bHit = UKismetSystemLibrary::BoxOverlapActors(
		Boss,                
		BoxCenter,          
		BoxTraceHalfSize,   
		ObjectTypes,         
		nullptr,           
		ActorsToIgnore,     
		OverlappedActors    
	);
	const FColor Color = bHit ? FColor::Red : FColor::Green;
	// DrawDebugBox(
	// 	Boss->GetWorld(),              
	// 	BoxCenter,                      
	// 	BoxTraceHalfSize,                    
	// 	Boss->GetActorRotation().Quaternion(), 
	// 	Color,
	// 	false,                         
	// 	1.0f,                             
	// 	0,                              
	// 	2.0f                            
	// );
	if (bHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Overlap Hit: %d"), OverlappedActors.Num());

		TSet<AActor*> DamagedActors;
		for (AActor* HitActor : OverlappedActors)
		{
			AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(HitActor);
			if (PlayerCharacter && !DamagedActors.Contains(PlayerCharacter))
			{
				UGameplayStatics::ApplyDamage(
					PlayerCharacter,
					Damage*Boss->DamageModifier,
					Boss->GetController(),
					Boss,
					UDamageType::StaticClass()
				);
				//UE_LOG(LogTemp, Warning, TEXT("%s에게 %f의 데미지를 입혔습니다!"),
					   //*PlayerCharacter->GetName(), Damage);
				DamagedActors.Add(PlayerCharacter);
			}
		}
	}
}

