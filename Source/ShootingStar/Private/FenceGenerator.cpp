// Copyright 2025 ShootingStar. All Rights Reserved.

#include "FenceGenerator.h"
#include "ProceduralMapGenerator.h"

UFenceGenerator::UFenceGenerator()
{
}

void UFenceGenerator::Initialize(UProceduralMapGenerator* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numFences = Owner->numFences;
        fenceMinDistance = Owner->fenceMinDistance;
        fenceMesh = Owner->fenceMesh;

        // FenceInstancedMeshComponent 가져오기 및 확인
        if (Owner->FenceInstancedMeshComponent)
        {
            SetInstancedMeshComponent(Owner->FenceInstancedMeshComponent);
            UE_LOG(ProceduralMapGenerator, Log, TEXT("FenceGenerator: InstancedMeshComponent set successfully"));
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("FenceGenerator: Failed to get InstancedMeshComponent"));
    }
}

void UFenceGenerator::SetInstancedMeshComponent(UInstancedStaticMeshComponent* InMeshComponent) 
{
    if (!InMeshComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("FenceGenerator: Invalid InstancedMeshComponent!"));
        return;
    }
    FenceInstancedMeshComponent = InMeshComponent;

    FenceInstancedMeshComponent->SetVisibility(true);
    FenceInstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UFenceGenerator::GenerateObjects()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("FenceGenerator: Generating Fences Started"));

    if (!Owner || !fenceMesh)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("FenceGenerator: Owner or fenceMesh is not set!"));
        return;
    }

    if (!FenceInstancedMeshComponent)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("FenceGenerator: InstancedMeshComponent is not set!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numFences && SpawnAttempts < numFences * 5)
    {
        if (FMath::RandBool())
        {
            if (GenerateFenceAroundObstacle())
                PlacedObjects++;
        }
        else
        {
            FVector RandomLocation = Owner->GetRandomPosition();
            if (!Owner->CheckLocation(RandomLocation))
            {
                RandomLocation = Owner->FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::FenceMask);
            }

            EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
            TArray<FFenceData> FencePositions;
            GenerateFencePattern(RandomLocation, RandomPattern, FMath::RandRange(400.f, 800.f), FencePositions);

            if (PlaceFencePattern(FencePositions))
                PlacedObjects++;
        }
        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("FenceGenerator: Generating Fences Completed"));
}

// 생성할 펜스의 모양을 랜덤하게 결정하고, 펜스의 위치들을 Fvector 배열에 담아주는 함수
// 펜스의 모양은 Rectangle, UShape, LShape, Surrounding이 존재하며 생성할 패턴 enum을 인수로 받는다
void UFenceGenerator::GenerateFencePattern(const FVector& Center, EPatternType PatternType, float Radius, TArray<FFenceData>& OutPositions)
{
    switch (PatternType)
    {
        case EPatternType::Rectangle:
            {
                // 세로 방향 펜스
                for (float X = -Radius; X < Radius; X += fenceMinDistance)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y + Radius, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y - Radius, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                }
                // 가로 방향 펜스
                for (float Y = -Radius; Y < Radius; Y += fenceMinDistance)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + Radius, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X - Radius, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                }
            }
            break;
        case EPatternType::UShape:
            {
                const int32 UWidth = 2;
                const int32 UHeight = 2;
                for (int32 x = -UWidth; x <= UWidth; ++x)
                {
                    for (int32 y = -UHeight; y <= UHeight; ++y)
                    {
                        if (y == -UHeight)
                        {
                            // U자 아래쪽 가로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 0.f, 0.f)
                            ));
                        }
                        else if ((x == -UWidth || x == UWidth) && y != UHeight)
                        {
                            // U자 양쪽 세로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 90.f, 0.f)
                            ));
                        }
                    }
                }
            }
            break;
        case EPatternType::LShape:
            {
                const int32 LWidth = 2;
                const int32 LHeight = 2;
                for (int32 x = -LWidth; x <= LWidth; ++x)
                {
                    for (int32 y = -LHeight; y <= LHeight; ++y)
                    {
                        if (y == -LHeight)
                        {
                            // L자 아래쪽 가로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 0.f, 0.f)
                            ));
                        }
                        else if (x == -LWidth && y != LHeight)
                        {
                            // L자 왼쪽 세로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 90.f, 0.f)
                            ));
                        }
                    }
                }
            }
            break;
        default:
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("FenceGenerator: Invalid PatternType . . ."));
            break;
    }
}

bool UFenceGenerator::PlaceFencePattern(const TArray<FFenceData>& Positions)
{
    if (Positions.Num() == 0 || !FenceInstancedMeshComponent) return false;

    bool bPlacedAny = false;
    for (const FFenceData& FenceData : Positions)
    {
        if (Owner->CheckLocation(FenceData.Location))
        {
            FenceInstancedMeshComponent->AddInstance(
                FTransform(FenceData.Rotation, FenceData.Location)
            );
            Owner->SetObjectRegion(FenceData.Location, fenceMesh, EObjectMask::FenceMask);
            bPlacedAny = true;
        }
    }
    return bPlacedAny;
}

// 현재 맵에 생성되어 있는 Obstacle를 탐색하고, 그중 하나를 선택하여 주변에 펜스를 생성하는 함수
bool UFenceGenerator::GenerateFenceAroundObstacle()
{
    TArray<FVector> ObstacleLocations;
    
    for (int32 X = -Owner->mapHalfSize; X < Owner->mapHalfSize; X += 100)
    {
        for (int32 Y = -Owner->mapHalfSize; Y < Owner->mapHalfSize; Y += 100)
        {
            if (Owner->HasObjectAtArray(X, Y, EObjectMask::ObstacleMask))
                ObstacleLocations.Add(FVector(X, Y, 0.f));
        }
    }

    if (ObstacleLocations.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Warning, TEXT("FenceGenerator: No obstacles found to generate fence around."));
        return false;
    }

    // 탐색이 완료되어 완성된 Obstacle 배열에서 랜덤하게 Obstacle 선택
    FVector TargetLocation = ObstacleLocations[FMath::RandRange(0, ObstacleLocations.Num() - 1)];
    
    // 패턴 랜덤 선택 및 생성
    EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
    
    float randomTemp = FMath::FRandRange(0.f, 1.f);
    float randomRadius = 0.f;
    if (randomTemp <= 0.33f)
        randomRadius = 900.f;
    else if (randomTemp <= 0.66f)
        randomRadius = 1200.f;
    else
        randomRadius = 1500.f;
    
    TArray<FFenceData> FencePositions;
    GenerateFencePattern(TargetLocation, RandomPattern, randomRadius, FencePositions);

    UE_LOG(ProceduralMapGenerator, Log, TEXT("FenceGenerator: Generating fence pattern around obstacle at %s"), *TargetLocation.ToString());
    return PlaceFencePattern(FencePositions);
}