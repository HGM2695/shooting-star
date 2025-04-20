// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "CompetitivePlayerController.generated.h"

class UClientComponent;
class UServerComponent;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UTeamComponent;
class UInventoryComponent;
/**
 *
 */

UCLASS()
class ACompetitivePlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	ACompetitivePlayerController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;
	
	UTeamComponent* GetTeamComponent() const
	{
		return TeamComponent;
	}
	
	UServerComponent* GetServerComponent() const
	{
		return ServerComponent;
	}

	UClientComponent* GetClientComponent() const
	{
		return ClientComponent;
	}

	/** Inventory */
	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UUserWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryWidget();
	
	/**
	 * Actor에 부착되어 공격 판정 등에 이용되는 일반적인 TeamComponent와는 달리,
	 * 이 컴포넌트는 게임 동안의 플레이어의 소속 팀을 정의하며, 레벨 이동시 소속 팀 유지를 위해 사용됩니다.
	 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTeamComponent> TeamComponent;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay();

	// Move fuction
	void Move(const FInputActionValue& Value);
	// Rotation Control
	void LookMouse();
	void Attack();
	void EquipWeapon();

	// HUD
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<UUserWidget> ScoreBoardUIClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UUserWidget> ScoreBoardUI;

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetInventoryComponent)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UServerComponent> ServerComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UClientComponent> ClientComponent;
};
