#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceDataAsset.h"
#include "InventoryComponent.generated.h"

struct FWeaponData;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UInventoryComponent();

    // �ڿ� �κ��丮: �ڿ� ������ ���� �� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TMap<UResourceDataAsset*, int32> ResourceInventory;

    // Weapon
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* ModifierDataTable{};

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FWeaponData Craft_Weapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources);

    // �ڿ� �߰�
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddResource(UResourceDataAsset* Resource, int32 Amount = 1);

    // �ڿ� ���� ��������
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetResourceQuantity(UResourceDataAsset* Resource) const;

    // ��ü �κ��丮 �������� (�������Ʈ���� ForEach ��� ����)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TMap<UResourceDataAsset*, int32>& GetAllResources() { return ResourceInventory; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UResourceDataAsset* GetResourceDataAsset_ByResourceEnum(const int32& Enum);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Clear_ZeroResources();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<UResourceDataAsset*> Get_SortedResources();
};
