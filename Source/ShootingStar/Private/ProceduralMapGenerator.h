// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMapGenerator.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ProceduralMapGenerator, Log, All);

UCLASS()
class AProceduralMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralMapGenerator();

    void InitializeMapCoordinate(int32 GridSize);
    FORCEINLINE int32 GetIndex(int32 X, int32 Y, int32 Size) { return X + (Y * Size); } // 2D 좌표 => 1D 변환 함수
	void GenerateMap();
    void GenerateBuildings();
    void GenerateWalls();
    void GenerateResources();
    bool IsLocationValid(FVector Location);
    bool PlaceObject(FVector Location, UStaticMesh* ObjectMesh);
    FVector GetRandomPosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 MapHalfSize = 4500;

    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 NumObjects = 50;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 NumBuildings = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 NumWalls = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 NumResources = 30;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    TArray<int32> mapCoordinate; // 맵 좌표를 저장할 1차원 배열

    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    AActor* PlaneActor; // Plane 액터를 저장할 변수

    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> ObjectMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> buildingMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> wallMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> resourceMeshes;

    UWorld* world; // 현재 World 포인터
};