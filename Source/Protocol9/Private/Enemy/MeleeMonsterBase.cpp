#include "Enemy/MeleeMonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
void AMeleeMonsterBase::PerformAttack()
{
	//StopMovement();
	Super::PerformAttack();
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation());
	}
}

void AMeleeMonsterBase::AttackCheck()
{
	if (!TargetPlayer.IsValid())
	{
		return;
	}
	//
	// const FVector Start = GetActorLocation() + GetActorRotation().RotateVector(AttackBoxOffset);
	// const FVector End = Start;
	//
	// const FVector BoxSize = AttackBoxHalfSize;
	//
	// const FRotator BoxOrientation = GetActorRotation();
	//
	// TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	// ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	//
	// TArray<AActor*> ActorsToIgnore;
	// ActorsToIgnore.Add(this);
	//
	// TArray<FHitResult> HitResults;
	//
	// bool bResult = UKismetSystemLibrary::BoxTraceMultiForObjects(
	// 	GetWorld(),
	// 	Start,
	// 	End,
	// 	BoxSize,
	// 	BoxOrientation,
	// 	ObjectTypes,
	// 	false,
	// 	ActorsToIgnore,
	// 	EDrawDebugTrace::ForDuration,
	// 	HitResults,
	// 	true
	// );

	FVector BoxCenter = GetActorLocation() + GetActorForwardVector() * (AttackBoxHalfSize.X);
	
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	TArray<AActor*> OverlappedActors;

	UE_LOG(LogTemp, Warning, TEXT("BoxOverlapActors"));
	bool bHit = UKismetSystemLibrary::BoxOverlapActors(
		GetWorld(),                
		BoxCenter,          
		AttackBoxHalfSize,   
		ObjectTypes,         
		nullptr,           
		ActorsToIgnore,     
		OverlappedActors    
	);
	const FColor Color = bHit ? FColor::Red : FColor::Green;
	DrawDebugBox(
		GetWorld(),                
		BoxCenter,                      
		AttackBoxHalfSize,                   
		GetActorRotation().Quaternion(), 
		Color,
		false,                     
		1.0f,                     
		0,                               
		2.0f                         
	);
	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap Hit: %d"), OverlappedActors.Num());

		TSet<AActor*> DamagedActors;
		for (AActor* HitActor : OverlappedActors)
		{
			AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(HitActor);
			if (PlayerCharacter && !DamagedActors.Contains(PlayerCharacter))
			{
				UGameplayStatics::ApplyDamage(
					PlayerCharacter,
					AttackDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
				UE_LOG(LogTemp, Warning, TEXT("%s에게 %f의 데미지를 입혔습니다!"),
					   *PlayerCharacter->GetName(), AttackDamage);
				DamagedActors.Add(PlayerCharacter);
			}
		}
	}


	
	// if (bResult)
	// {
	// 	for (const FHitResult& Hit : HitResults)
	// 	{
	// 		if (Hit.GetActor() == TargetPlayer)
	// 		{
	// 			UGameplayStatics::ApplyDamage(
	// 				TargetPlayer,
	// 				AttackDamage,
	// 				GetController(),
	// 				this,
	// 				UDamageType::StaticClass()
	// 			);
	// 			break;
	// 		}
	// 	}
	// }
}

