// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MinimapCapture.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "UI/UWBP_HUD.h"


// Sets default values
AMiniMapCapture::AMiniMapCapture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponenet2D");
	CaptureComponent->SetupAttachment(SceneComponent);

	CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;

	CaptureComponent->PostProcessSettings.bOverride_AutoExposureMethod=true;
	CaptureComponent->PostProcessSettings.AutoExposureMethod=AEM_Manual;
	CaptureComponent->PostProcessSettings.bOverride_AutoExposureBias=true;
	CaptureComponent->PostProcessSettings.AutoExposureBias=1.0f;
	
}

// Called when the game starts or when spawned
void AMiniMapCapture::BeginPlay()
{
	Super::BeginPlay();
	CaptureComponent->SetRelativeRotation(FRotator(Rotation,0,0));
	PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	CaptureComponent->TextureTarget = RenderTarget;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle,this, &AMiniMapCapture::CaptureOnce,0.041f, true);
	CaptureComponent->OrthoWidth = OrthoWidth;
}
// Called every frame
void AMiniMapCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMiniMapCapture::CaptureOnce()
{
	if (PlayerCharacter)
	{
		SetActorLocation(PlayerCharacter->GetActorLocation()+FVector(0,0,Height));
		CaptureComponent->CaptureScene();

		if (!HUD)
		{
			return;
		}
		HUD->SetMiniMapArrow(PlayerCharacter->GetActorRotation().Yaw);
	}
	
}