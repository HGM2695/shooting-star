// Copyright 2025 ShootingStar. All Rights Reserved.

#include "FenceGenerator.h"
#include "MapGeneratorComponent.h"

void UFenceGenerator::Initialize(UMapGeneratorComponent* InOwner)
{
	Owner = InOwner;

	if (Owner)
	{
		numFences = Owner->GetNumFences();
		fenceMinDistance = Owner->GetFenceMinDistance();
        fenceGenerationProbability = Owner->GetFenceGenerationProbability();
		fenceMesh = Owner->GetFenceMesh();
	}
}

void UFenceGenerator::GenerateObjects()
{
	UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating Fences Started"));

	if (!Owner || !fenceMesh)
	{
		UE_LOG(MapGenerator, Error, TEXT("(Fence) Owner or fenceMesh is not set!"));
		return;
	}

	int32 SpawnAttempts = 0;
	int32 PlacedObjects = 0;

	while (PlacedObjects < numFences && SpawnAttempts < numFences * 2)
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
				RandomLocation = Owner->FindNearestValidLocation(RandomLocation, fenceMinDistance * 4, fenceMesh, EObjectMask::FenceMask);

                if (RandomLocation == FVector::ZeroVector)
                    continue;
			}

			EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
			TArray<FFenceData> FencePositions;
			GenerateFencePattern(RandomLocation, RandomPattern, GetRandomFenceRadius(600.f, 1200.f), FencePositions);

			if (FencePositions.Num() == 0)
            {
                UE_LOG(MapGenerator, Warning, TEXT("(Fence) No fence positions generated."));
            }
			else
			{
				PlaceFence(FencePositions);
				PlacedObjects++;
			}
				
		}
		SpawnAttempts++;
	}

	UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating Fences Completed"));
}

// 생성할 펜스의 모양을 랜덤하게 결정하고, 펜스의 위치들을 Fvector 배열에 담아주는 함수
// 펜스의 모양은 Rectangle, UShape, LShape, Surrounding이 존재하며 생성할 패턴 enum을 인수로 받는다
void UFenceGenerator::GenerateFencePattern(const FVector& Center, EPatternType PatternType, float Radius,
                                           TArray<FFenceData>& OutPositions)
{
    const float fenceDistOffset = fenceMinDistance * 0.5f;
    const EPatternDirection direction = static_cast<EPatternDirection>(FMath::RandRange(0, 3));
    
    // 패턴 생성 전 범위 검사
    const FVector CheckMin(Center.X - Radius, Center.Y - Radius, 0.f);
    const FVector CheckMax(Center.X + Radius, Center.Y + Radius, 0.f);
    
    // 전체 영역이 맵 밖이면 패턴 생성 중단
    if (!Owner->IsInMap(CheckMin) || !Owner->IsInMap(CheckMax))
        return;

    switch (PatternType)
    {
    case EPatternType::Rectangle:
        {
            // 세로 방향 펜스 위치 계산
            for (float X = -Radius; X <= Radius; X += fenceMinDistance)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y + Radius + fenceDistOffset, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y - Radius - fenceDistOffset, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                }
            }

            // 가로 방향 펜스 위치 계산
            for (float Y = -Radius; Y <= Radius; Y += fenceMinDistance)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + Radius + fenceDistOffset, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X - Radius - fenceDistOffset, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                }
            }
        }
        break;
    case EPatternType::UShape:
        {
            const int32 UWidth = FMath::RandRange(2, 4);
            const int32 UHeight = FMath::RandRange(2, 4);
            
            // 회전된 위치 계산을 위한 변환 행렬
            FRotator BaseRotation(0.f, 0.f, 0.f);
            switch (direction)
            {
                case EPatternDirection::South: BaseRotation.Yaw = 180.f; break;
                case EPatternDirection::East: BaseRotation.Yaw = 90.f; break;
                case EPatternDirection::West: BaseRotation.Yaw = -90.f; break;
                default: break;
            }

            // U자 바닥 부분 (ㄷ자 왼쪽)
            for (int32 x = -UWidth; x <= UWidth; ++x)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    FVector LocalPos(x * fenceMinDistance, -UHeight * fenceMinDistance - fenceDistOffset, 0.f);
                    FVector WorldPos = BaseRotation.RotateVector(LocalPos) + Center;

                    OutPositions.Add(FFenceData(WorldPos, BaseRotation));
                }
            }

            // U자 위아래 벽
            for (int32 y = -UHeight; y < UHeight; ++y)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    // 아래쪽 벽
                    FVector LocalPosLeft(-UWidth * fenceMinDistance - fenceDistOffset, y * fenceMinDistance, 0.f);
                    FVector WorldPosLeft = BaseRotation.RotateVector(LocalPosLeft) + Center;
                    OutPositions.Add(FFenceData(WorldPosLeft, BaseRotation + FRotator(0.f, 90.f, 0.f)));

                    // 위쪽 벽
                    FVector LocalPosRight(UWidth * fenceMinDistance + fenceDistOffset, y * fenceMinDistance, 0.f);
                    FVector WorldPosRight = BaseRotation.RotateVector(LocalPosRight) + Center;
                    OutPositions.Add(FFenceData(WorldPosRight, BaseRotation + FRotator(0.f, 90.f, 0.f)));
                }
            }
        }
        break;

    case EPatternType::LShape:
        {
            const int32 LWidth = FMath::RandRange(2, 4);
            const int32 LHeight = FMath::RandRange(2, 4);
            
            FRotator BaseRotation(0.f, 0.f, 0.f);
            switch (direction)
            {
                case EPatternDirection::South: BaseRotation.Yaw = 180.f; break;
                case EPatternDirection::East: BaseRotation.Yaw = 90.f; break;
                case EPatternDirection::West: BaseRotation.Yaw = -90.f; break;
                default: break;
            }

            // L자 세로로 부분
            for (int32 x = -LWidth; x <= LWidth; ++x)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    FVector LocalPos(x * fenceMinDistance, -LHeight * fenceMinDistance - fenceDistOffset, 0.f);
                    FVector WorldPos = BaseRotation.RotateVector(LocalPos) + Center;
                    OutPositions.Add(FFenceData(WorldPos, BaseRotation));
                }
            }

            // L자 가로 부분
            for (int32 y = -LHeight; y < LHeight; ++y)
            {
                if (FMath::RandRange(1, 100) <= fenceGenerationProbability)
                {
                    FVector LocalPos(-LWidth * fenceMinDistance - fenceDistOffset, y * fenceMinDistance, 0.f);
                    FVector WorldPos = BaseRotation.RotateVector(LocalPos) + Center;
                    OutPositions.Add(FFenceData(WorldPos, BaseRotation + FRotator(0.f, 90.f, 0.f)));
                }
            }
        }
        break;
    default:
        UE_LOG(MapGenerator, Warning, TEXT("(Fence) Invalid PatternType . . ."));
        break;
    }
}

void UFenceGenerator::PlaceFence(const TArray<FFenceData>& Positions)
{
    // 펜스 위치 정보를 MapGeneratorComponent에 전달
    Owner->SetCurrentFencePositions(Positions);

    TArray<FTransform> FenceTransforms;
    for (const FFenceData& FenceData : Positions)
    {
        if (Owner->CheckLocation(FenceData.Location, fenceMesh, EObjectMask::FenceMask))
        {
            FenceTransforms.Add(FTransform{FenceData.Rotation, FenceData.Location});
            Owner->SetObjectRegion(FenceData.Location, fenceMesh, EObjectMask::FenceMask);
        }
    }
    // 위치 정보 초기화
    Owner->SetCurrentFencePositions(TArray<FFenceData>());
    Owner->GetMapInstancedMeshActor()->SetFenceInstances(FenceTransforms);
}

// 현재 맵에 생성되어 있는 Obstacle를 탐색하고, 그중 하나를 선택하여 주변에 펜스를 생성하는 함수
bool UFenceGenerator::GenerateFenceAroundObstacle()
{
	TArray<FVector> ObstacleLocations;
	const int32 MapHalfSize = Owner->GetMapHalfSize();

	for (int32 X = -MapHalfSize; X < MapHalfSize; X += 100)
	{
		for (int32 Y = -MapHalfSize; Y < MapHalfSize; Y += 100)
		{
			if (Owner->HasObjectAtArray(X, Y, EObjectMask::ObstacleMask))
				ObstacleLocations.Add(FVector(X, Y, 0.f));
		}
	}

	if (ObstacleLocations.Num() == 0)
	{
		UE_LOG(MapGenerator, Warning, TEXT("(Fence) No obstacles found to generate fence around."));
		return false;
	}

	// 탐색이 완료되어 완성된 Obstacle 배열에서 랜덤하게 Obstacle 선택
	FVector TargetLocation = ObstacleLocations[FMath::RandRange(0, ObstacleLocations.Num() - 1)];

	// 패턴 랜덤 선택 및 생성
	EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
	TArray<FFenceData> FencePositions;
	GenerateFencePattern(TargetLocation, RandomPattern, GetRandomFenceRadius(900.f, 1500.f), FencePositions);

	if (FencePositions.Num() == 0)
	{
		UE_LOG(MapGenerator, Warning, TEXT("(Fence) AroundObstacles - No fence positions generated."));
		return false;
	}
	else
	{
		PlaceFence(FencePositions);
		UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating fence pattern around obstacle at %s"),
	       *TargetLocation.ToString());
		return true;
	}
}

float UFenceGenerator::GetRandomFenceRadius(float minRadius, float maxRadius) const
{
	int32 n = (int32)((maxRadius - minRadius) / fenceMinDistance);
	float randomRadius = minRadius + FMath::RandRange(0, n) * fenceMinDistance;
	return randomRadius;
}