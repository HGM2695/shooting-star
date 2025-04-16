// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ObstacleGenerator.h"
#include "ProceduralMapGenerator.h"

UObstacleGenerator::UObstacleGenerator()
{
}

void UObstacleGenerator::Initialize(UProceduralMapGenerator* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        obstacleMeshes = Owner->obstacleMeshes;
    }
}

// 건물, 큰 바위 등 주요 Obstacles를 생성하는 함수
void UObstacleGenerator::GenerateObjects()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Obstacles Started"));

    if (!Owner)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("Owner is not initialized!"));
        return;
    }
    if (obstacleMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numObstacles && SpawnAttempts < numObstacles * 5)
    {
        FVector RandomLocation = Owner->GetRandomPosition();
        if (!Owner->CheckLocation(RandomLocation))
        {
            RandomLocation = Owner->FindNearestValidLocation(RandomLocation, obstacleMinDistance, EObjectMask::ObstacleMask);
        }

        int32 RandomIndex = FMath::RandRange(0, obstacleMeshes.Num() - 1);
        if (obstacleMeshes.IsValidIndex(RandomIndex))
        {
            UStaticMesh* RandomMesh = obstacleMeshes[RandomIndex];
            if (Owner->PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                Owner->SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::ObstacleMask);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("Generated Obstacle: %s at %s"), *RandomMesh->GetName(), *RandomLocation.ToString());
            }
        }
        SpawnAttempts++;
    }
}
