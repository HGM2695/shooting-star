// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ObstacleGenerator.h"
#include "MapGeneratorComponent.h"

UObstacleGenerator::UObstacleGenerator()
{
}

void UObstacleGenerator::Initialize(UMapGeneratorComponent* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numObstacles = Owner->GetNumObstacles();
        obstacleMinDistance = Owner->GetObstacleMinDistance();
        obstacleMeshes = Owner->GetObstacleMeshes();
    }

    UE_LOG(MapGenerator, Log, TEXT("ObstacleGenerator Initialized, obstacle Meshes: %d,"), obstacleMeshes.Num());
}

// 건물, 큰 바위 등 주요 Obstacles를 생성하는 함수
void UObstacleGenerator::GenerateObjects()
{
    UE_LOG(MapGenerator, Log, TEXT("Generating Obstacles Started"));

    if (!Owner)
    {
        UE_LOG(MapGenerator, Error, TEXT("Owner is not initialized!"));
        return;
    }
    if (obstacleMeshes.Num() == 0)
    {
        UE_LOG(MapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
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
            if (Owner && Owner->PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                Owner->SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::ObstacleMask);
                UE_LOG(MapGenerator, Log, TEXT("Generated Obstacle: %s at %s"), *RandomMesh->GetName(), *RandomLocation.ToString());
            }
        }
        SpawnAttempts++;
    }
}
