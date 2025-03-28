// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WifiDirectDiscoverPC.generated.h"

/**
 * 
 */
UCLASS()
class WIFIDIRECT_API AWifiDirectDiscoverPC final : public APlayerController
{
	GENERATED_BODY()

public:
	AWifiDirectDiscoverPC();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly)
	float PeerDiscoveringInterval = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	float PeerDiscoveringElapsed;
	
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> WifiDirectDiscoverUIClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> WifiDirectDiscoverUI;
};
