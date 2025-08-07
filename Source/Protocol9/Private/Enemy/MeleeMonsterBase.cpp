#include "Enemy/MeleeMonsterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
void AMeleeMonsterBase::PerformAttack()
{
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
	if (!TargetPlayer)
	{
		return;
	}
	
	const FVector Start = GetActorLocation() + GetActorRotation().RotateVector(AttackBoxOffset);
	const FVector End = Start;
	
	const FVector BoxSize = AttackBoxHalfSize;
	
	const FRotator BoxOrientation = GetActorRotation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitResults;

	bool bResult = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		Start,
		End,
		BoxSize,
		BoxOrientation,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true
	);

	if (bResult)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (Hit.GetActor() == TargetPlayer)
			{
				UGameplayStatics::ApplyDamage(
					TargetPlayer,
					AttackDamage,
					GetController(),
					this,
					UDamageType::StaticClass()
				);
				break;
			}
		}
	}
}

