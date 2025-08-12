#include "GameMode/MainGameMode.h"
#include "UI/UWBP_HUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter.h"
#include "Character/HPComponent.h"
#include "Enemy/MonsterSpawner.h"
#include "Enemy/MonsterBase.h"
#include "GameInstance/MainGameInstance.h"
#include "UI/UWBP_ResultStats.h"
#include "TimerManager.h"
#include "Components/Button.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();
	const FString MapName = GetWorld()->GetMapName();

	if (MapName.Contains(TEXT("MenuLevel")))
	{
		if (UMainGameInstance* GI = GetGameInstance<UMainGameInstance>())
		{
			if (GI->bPendingResults)
			{
				GI->bPendingResults = false;

				// 결과값을 GameMode 멤버로 옮겨서 기존 ShowResultStats() 재사용
				KillCount   = GI->LastKillCount;
				ElapsedTime = GI->LastElapsedTime;
				// bWinCache = GI->bVictory; // 승패도 쓰려면(선택)

				ShowResultStats();   // ← 메뉴에서 바로 결과창 띄움
				return;
			}
		}
		ShowPressAnyKey(); // 평소처럼
	}
	else if (MapName.Contains(TEXT("TestMap")))
	{
		ShowHUD();
	}
}

void AMainGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// ★ 남아있는 타이머는 모두 정리 (람다/Delegate 호출로 인한 유효하지 않은 this 접근 방지)
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	GetWorldTimerManager().ClearTimer(PauseTimerHandle);

	Super::EndPlay(EndPlayReason);
}
// MainGameMode.cpp



void AMainGameMode::ShowPressAnyKey()
{
	// 기존 위젯 정리
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	// 멈춰있을 수 있으니 Pause 상태로
	UGameplayStatics::SetGamePaused(this, true);

	if (!WBP_PressAnyKey) return;

	// 위젯 생성/표시
	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_PressAnyKey);
	if (!CurrentWidget) return;

	CurrentWidget->AddToViewport();

	// UI 전용 입력 + 커서 On
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(CurrentWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
	}
}

void AMainGameMode::ShowMainMenu()
{
	// 혹시 남아있는 HUD/다른 위젯 제거
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	// 혹시 멈춰있을 수 있으니 풀고 시작
	UGameplayStatics::SetGamePaused(this, false);
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	GetWorldTimerManager().ClearTimer(PauseTimerHandle);

	if (!WBP_MainMenu) return;

	// 위젯 생성/표시
	CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WBP_MainMenu);
	if (!CurrentWidget) return;

	CurrentWidget->AddToViewport();
	UE_LOG(LogTemp, Warning, TEXT("Main Menu UI shown"));

	// 여기서 UI 전용 입력 모드 적용 + 커서 on
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(CurrentWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
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

			KillCount = 0;
			HUDWidget->UpdateKillCount(KillCount);
			BindSpawnerForKillCount();

			// 사망 이벤트 구독
			if (AMainCharacter* MC = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				if (UHPComponent* HP = MC->GetHPComponent())
				{
					HP->OnDeathEvent.AddDynamic(this, &AMainGameMode::HandlePlayerDeath);
				}
			}
		}
	}
}

void AMainGameMode::StartGame()
{
	// 입력 모드 초기화 (GameOnly + 마우스 숨김)
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
	}

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
		MC->CacheHUD();
	}
}

void AMainGameMode::AddKillCount()
{
	KillCount++;

	if (HUDWidget) // HUDWidget은 BeginPlay에서 생성된 UWBP_HUD
	{
		HUDWidget->UpdateKillCount(KillCount);
	}
}
void AMainGameMode::BindSpawnerForKillCount()
{
	if (bKillBindingInitialized) return;  // 중복 바인딩 방지(레벨 리로드 대비)

	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterSpawner::StaticClass(), Spawners);

	for (AActor* A : Spawners)
	{
		if (AMonsterSpawner* S = Cast<AMonsterSpawner>(A))
		{
			S->OnMonsterSpawned.AddDynamic(this, &AMainGameMode::OnMonsterSpawnedForUI);
		}
	}
	TArray<AActor*> ExistingMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonsterBase::StaticClass(), ExistingMonsters);

	for (AActor* M : ExistingMonsters)
	{
		if (AMonsterBase* Monster = Cast<AMonsterBase>(M))
		{
			Monster->OnMonsterDead.AddDynamic(this, &AMainGameMode::OnMonsterDeadForUI);
		}
	}

	bKillBindingInitialized = true;
}

void AMainGameMode::OnMonsterSpawnedForUI(AMonsterBase* Monster)
{
	if (!Monster) return;
	Monster->OnMonsterDead.AddDynamic(this, &AMainGameMode::OnMonsterDeadForUI);
}

void AMainGameMode::OnMonsterDeadForUI(AMonsterBase* /*Monster*/)
{
	AddKillCount();
}

void AMainGameMode::ShowGameOver(bool bVictory, int32 InKillCount)
{
	// 기존 위젯 제거
	if (CurrentWidget) CurrentWidget->RemoveFromParent();

	if (WBP_GameOver)
	{
		// UWBP_GameOver로 캐스팅해서 결과 전달
		if (UWBP_GameOver* Screen = CreateWidget<UWBP_GameOver>(GetWorld(), WBP_GameOver))
		{
			Screen->AddToViewport();
			Screen->Setup(bVictory, InKillCount, FSimpleDelegate::CreateUObject(this, &AMainGameMode::OnRetryClicked));
			CurrentWidget = Screen;

			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;

				FInputModeUIOnly Mode;
				Mode.SetWidgetToFocus(Screen->TakeWidget());
				Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(Mode);
			}
		}
	}
	GetWorldTimerManager().ClearTimer(PauseTimerHandle);
	// 약간의 지연 뒤 UI Only + Pause
	GetWorldTimerManager().SetTimer(PauseTimerHandle,
		FTimerDelegate::CreateLambda([this]()
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			UGameplayStatics::SetGamePaused(this, true);
		}
	}), 0.0f, false);
}


void AMainGameMode::OnRetryClicked()
{
	GetWorldTimerManager().ClearTimer(PauseTimerHandle);

	if (UMainGameInstance* GI = GetGameInstance<UMainGameInstance>())
	{
		// bVictory는 ShowGameOver 호출 때 멤버로 캐시해 둔다면 함께 전달 가능
		const bool bWinCached = false;
		GI->SetFinalStats(KillCount, ElapsedTime, bWinCached);
	}

	// UI 정리 후 메뉴로 이동(조용한 상태)
	UGameplayStatics::SetGamePaused(this, false);
	if (CurrentWidget) { CurrentWidget->RemoveFromParent(); CurrentWidget = nullptr; }
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Maps/MenuLevel")));
}



void AMainGameMode::HandlePlayerDeath()
{
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	ShowGameOver(/*bVictory=*/false, /*InKillCount=*/KillCount);
}

void AMainGameMode::HandleBossDefeated()
{
	GetWorldTimerManager().ClearTimer(GameTimerHandle);
	ShowGameOver(/*bVictory=*/true, /*InKillCount=*/KillCount);
}
void AMainGameMode::PostShowGameOver()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(true);
		FInputModeUIOnly Mode;
		PC->SetInputMode(Mode);
	}
	UGameplayStatics::SetGamePaused(this, true);
}

void AMainGameMode::OnAnyKeyPressed()
{
	// PressAnyKey → MainMenu 전환
	ShowMainMenu(); // 이미 구현되어 있음
}

void AMainGameMode::ShowResultStats()
{
	// 기존 위젯 정리
	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
	if (!WBP_ResultStats)
	{
		UE_LOG(LogTemp, Error, TEXT("WBP_ResultStats is NOT set."));
		return;
	}

	// 결과 위젯 생성 (BP 부모가 UWBP_ResultStats 여도 OK)
	UUWBP_ResultStats* Stats = CreateWidget<UUWBP_ResultStats>(GetWorld(), WBP_ResultStats);
	if (!Stats)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateWidget for ResultStats FAILED"));
		return;
	}

	Stats->AddToViewport(100);
	CurrentWidget = Stats;

	// 텍스트 채우기
	Stats->SetStats(KillCount, ElapsedTime);

	// 버튼 바인딩
	if (Stats->Btn_Restart)
	{
		Stats->Btn_Restart->OnClicked.Clear();
		Stats->Btn_Restart->OnClicked.AddDynamic(this, &AMainGameMode::OnRestartClicked);
	}
	if (Stats->Btn_Return)
	{
		Stats->Btn_Return->OnClicked.Clear();
		Stats->Btn_Return->OnClicked.AddDynamic(this, &AMainGameMode::OnReturnMenuClicked);
	}

	// UIOnly + 커서 ON, 게임은 일시정지 유지
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(Stats->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
	}
	UGameplayStatics::SetGamePaused(this, true);
}
void AMainGameMode::OnRestartClicked()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
	}

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	// 원하는 맵으로 재시작 (지금은 TestMap로 고정)
	UGameplayStatics::OpenLevel(this, FName(TEXT("TestMap")));
}

void AMainGameMode::OnReturnMenuClicked()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;

		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);
	}

	if (CurrentWidget)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}

	// 메뉴로
	UGameplayStatics::OpenLevel(this, FName(TEXT("/Game/Maps/MenuLevel")));
}

