#include "Enemy/MeleeMonsterBase.h"

void AMeleeMonsterBase::PerformAttack()
{
	if (AttackMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	}
}

