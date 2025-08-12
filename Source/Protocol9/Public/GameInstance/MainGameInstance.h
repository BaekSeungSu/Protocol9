#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

UCLASS()
class PROTOCOL9_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	// 결과 데이터 (레벨 넘어가도 유지됨)
	UPROPERTY(BlueprintReadWrite)
	int32 LastKillCount = 0;

	UPROPERTY(BlueprintReadWrite)
	float LastElapsedTime = 0.f;

	UPROPERTY(BlueprintReadWrite)
	bool bVictory = false;

	// 결과창을 띄울지 여부 (MenuLevel 진입 시 체크)
	UPROPERTY(BlueprintReadWrite)
	bool bPendingResults = false;

	UFUNCTION(BlueprintCallable)
	void SetFinalStats(int32 InKill, float InTime, bool bInVictory)
	{
		LastKillCount   = InKill;
		LastElapsedTime = InTime;
		bVictory        = bInVictory;
		bPendingResults = true;
	}
};
