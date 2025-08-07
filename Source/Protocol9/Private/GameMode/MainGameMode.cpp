#include "GameMode/MainGameMode.h"
#include "UI/UWBP_HUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter.h"
#include "TimerManager.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevel = GetWorld()->GetMapName();
	// 맵 이름은 "UEDPIE_0_MenuLevel" 이런 식으로 붙기 때문에 Contains 사용
	if (CurrentLevel.Contains(TEXT("MenuLevel")))
	{
		ShowMainMenu(false);
	}
	else if (CurrentLevel.Contains(TEXT("TestMap")))
	{
		ShowHUD();
	}
}

void AMainGameMode::ShowMainMenu(bool bIsRestart)
{
	if (CurrentWidget)
		CurrentWidget->RemoveFromParent();

	if (WBP_MainMenu)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_MainMenu);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Main Menu UI shown"));

			if (bIsRestart)
			{
				// 재시작이라면 추가 메시지 출력 가능
			}
		}
	}
}

void AMainGameMode::ShowHUD()
{
	if (CurrentWidget)
		CurrentWidget->RemoveFromParent();

	if (WBP_HUD)
	{
		HUDWidget = CreateWidget<UUWBP_HUD>(GetWorld(), WBP_HUD);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
			CurrentWidget = HUDWidget;

			ElapsedTime = 0.0f;
			GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AMainGameMode::UpdateGameTimer, 1.0f, true);

			NotifyCharacterHUDReady();
		}
	}
}


void AMainGameMode::ShowGameOver()
{
	if (CurrentWidget)
		CurrentWidget->RemoveFromParent();

	if (WBP_GameOver)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_GameOver);
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Game Over UI shown"));
		}
	}
}

void AMainGameMode::StartGame()
{
	// MenuLevel → TestMap 전환
	UGameplayStatics::OpenLevel(this, FName("TestMap"));
}

void AMainGameMode::UpdateGameTimer()
{
	ElapsedTime += 1.0f;

	if (HUDWidget)
	{
		HUDWidget->UpdateTimer(ElapsedTime);
	}
}

void AMainGameMode::NotifyCharacterHUDReady()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (AMainCharacter* MC = Cast<AMainCharacter>(PlayerPawn))
	{
		MC->CacheHUD(); // MainCharacter에 바로 알려줌
	}
}