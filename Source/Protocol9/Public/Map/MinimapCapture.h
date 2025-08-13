// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniMapCapture.generated.h"

UCLASS()
class PROTOCOL9_API AMiniMapCapture : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMiniMapCapture();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void CaptureOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* CaptureComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ACharacter* PlayerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OrthoWidth = 10000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rotation = -90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height = 3000.0f;

	FTimerHandle TimerHandle;
};
