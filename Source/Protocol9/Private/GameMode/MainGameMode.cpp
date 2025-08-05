#include "GameMode/MainGameMode.h"
#include "Blueprint/UserWidget.h"
#include "UI/UWBP_HUD.h"
#include "TimerManager.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("AMainGameMode::BeginPlay called"));

	if (HUDWidgetClass)
	{
		HUDWidget = Cast<UUWBP_HUD>(CreateWidget<UUWBP_HUD>(GetWorld(), HUDWidgetClass));
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}

	// 타이머 설정 및 결과 로그 출력
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AMainGameMode::UpdateGameTimer, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("Timer Set Attempted"));

}


void AMainGameMode::UpdateGameTimer()
{
	ElapsedTime += 1.0f;

	if (HUDWidget)
	{
		HUDWidget->UpdateTimer(ElapsedTime);
		UE_LOG(LogTemp, Warning, TEXT("UpdateGameTimer called. ElapsedTime: %f"), ElapsedTime);

	}
}
