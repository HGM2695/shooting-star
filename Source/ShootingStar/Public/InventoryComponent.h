#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceDataAsset.h" // UResourceDataAsset ����
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // �ڿ� �κ��丮: �ڿ� ������ ���� �� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TMap<UResourceDataAsset*, int32> ResourceInventory;

    // ����


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
};
