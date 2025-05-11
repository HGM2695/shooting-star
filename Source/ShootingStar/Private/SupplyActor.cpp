#include "SupplyActor.h"
#include "CompetitiveGameMode.h"
#include "CompetitiveSystemComponent.h"
#include "Components/StaticMeshComponent.h"

ASupplyActor::ASupplyActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsOpened = false;

    // Supply 태그 추가
    Tags.Add(FName("Supply"));

    // 로켓 런처 데이터 설정
    StoredWeapon.WeaponName = FText::FromString("RocketLauncher");
    StoredWeapon.NeedResourceCounts = 0;
}

void ASupplyActor::BeginPlay()
{
    Super::BeginPlay();
}

void ASupplyActor::PlayOpeningAnimation()
{
    if (!bIsOpened)
    {
        bIsOpened = true;
        PlayOpenTimeline();
        UE_LOG(LogTemp, Log, TEXT("Playing Supply Open Animation"));
        
        if (ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode()))
        {
            if (UCompetitiveSystemComponent* CompSystem = GameMode->GetCompetitiveSystemComponent())
            {
                CompSystem->OnSupplyOpened.Broadcast(GetActorLocation());
            }
        }
    }
}
