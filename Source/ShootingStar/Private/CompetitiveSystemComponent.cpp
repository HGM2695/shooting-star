// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveSystemComponent.h"

#include "GameFramework/PlayerState.h"
#include "ShootingStar/ShootingStar.h"

UCompetitiveSystemComponent::UCompetitiveSystemComponent()
	: BlueTeamKillScore{0},
	  RedTeamKillScore{0},
	  BlueTeamGameScore{0},
	  RedTeamGameScore{0},
	  CurrentPhaseTime{0.0f},
	  CurrentPhase{ECompetitiveGamePhase::WaitingForStart}
{
}

void UCompetitiveSystemComponent::Update(const float DeltaTime)
{
	CurrentPhaseTime += DeltaTime;

	if (CurrentPhase == ECompetitiveGamePhase::WaitingForStart)
	{
		Update_WaitingForStart();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::Game)
	{
		Update_Game();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::RoundEnd)
	{
		Update_RoundEnd();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::GameEnd)
	{
		Update_GameEnd();
	}
	else
	{
		Update_GameDestroyed();
	}
}

void UCompetitiveSystemComponent::StartGame()
{
	check(CurrentPhase == ECompetitiveGamePhase::WaitingForStart);

	CurrentPhase = ECompetitiveGamePhase::Game;
	CurrentPhaseTime = 0.0f;
	OnGameStarted.Broadcast();
}

void UCompetitiveSystemComponent::EndGame()
{
	check(CurrentPhase != ECompetitiveGamePhase::GameDestroyed);
	check(CurrentPhase != ECompetitiveGamePhase::GameEnd);

	CurrentPhase = ECompetitiveGamePhase::GameEnd;
	CurrentPhaseTime = 0.0f;
}

void UCompetitiveSystemComponent::GiveKillScoreForTeam(const ETeam Team)
{
	if (Team == ETeam::None || CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}

	int& TeamKillScore = Team == ETeam::Blue ? BlueTeamKillScore : RedTeamKillScore;
	TeamKillScore += 1;
	
	// 골든 킬(점수를 얻는 팀이 즉시 승리)인지?
	if (IsGoldenKillTime())
	{
		WinTeam(Team);
	}
}

ETeam UCompetitiveSystemComponent::GetTeamForNextPlayer(const TArray<APlayerState*>& PlayerArray) const
{
	int NumBlueTeamPlayers = 0;
	int NumRedTeamPlayers = 0;
	for (const auto PlayerState : PlayerArray)
	{
		UTeamComponent* TeamComponent = Cast<UTeamComponent>(PlayerState->GetPlayerController()->GetComponentByClass(UTeamComponent::StaticClass()));
		check(TeamComponent != nullptr);

		if (TeamComponent->GetTeam() == ETeam::Red)
		{
			NumRedTeamPlayers += 1;
		}
		else if (TeamComponent->GetTeam() == ETeam::Blue)
		{
			NumBlueTeamPlayers += 1;
		}
	}

	if (NumRedTeamPlayers >= MaxPlayersPerTeam && NumBlueTeamPlayers >= MaxPlayersPerTeam)
	{
		return ETeam::None;	
	}
	else if (NumBlueTeamPlayers <= NumRedTeamPlayers)
	{
		return ETeam::Blue;
	}
	else
	{
		return ETeam::Red;
	}
}

void UCompetitiveSystemComponent::Update_WaitingForStart()
{
}

void UCompetitiveSystemComponent::Update_Game()
{
	// 3선승 팀이 결정되었다면 게임 즉시 종료
	ensure(BlueTeamGameScore < GameWinningScore && RedTeamGameScore < GameWinningScore);
	if (BlueTeamGameScore >= GameWinningScore || RedTeamGameScore >= GameWinningScore)
	{
		CurrentPhase = ECompetitiveGamePhase::GameEnd;
		CurrentPhaseTime = 0.0f;
		
		return;
	}
	
	// 승리 팀 결정됨
	if (BlueTeamKillScore >= RoundWinningKillScore || RedTeamKillScore >= RoundWinningKillScore // 점수 기준을 넘겼거나
	|| (CurrentPhaseTime >= GameTime && (BlueTeamKillScore > RedTeamKillScore || RedTeamKillScore > BlueTeamKillScore))) // 시간이 다 되었을 때 앞선 팀이 있다면
	{
		const ETeam Team = BlueTeamKillScore >= RoundWinningKillScore ? ETeam::Blue : ETeam::Red;

		WinTeam(Team);
	}
}

void UCompetitiveSystemComponent::Update_RoundEnd()
{
	if (CurrentPhaseTime >= RoundEndTime)
	{
		CurrentPhase = ECompetitiveGamePhase::Game;
		BlueTeamKillScore = 0;
		RedTeamKillScore = 0;
		CurrentPhaseTime = 0.0f;
		OnGameStarted.Broadcast();
	}
}

void UCompetitiveSystemComponent::Update_GameEnd()
{
	if (CurrentPhaseTime >= GameEndTime)
	{
		CurrentPhase = ECompetitiveGamePhase::GameDestroyed;
		CurrentPhaseTime = 0.0f;
	}
}

void UCompetitiveSystemComponent::Update_GameDestroyed()
{
}

void UCompetitiveSystemComponent::WinTeam(const ETeam Team)
{
	ensure(CurrentPhase == ECompetitiveGamePhase::Game);
	if (CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}
	
	int& CurrentTeamGameScore = Team == ETeam::Blue ? BlueTeamGameScore : RedTeamGameScore;
	ensure(CurrentTeamGameScore < GameWinningScore);
	if (CurrentTeamGameScore < GameWinningScore)
	{
		CurrentTeamGameScore += 1;
	}
		
	if (BlueTeamGameScore >= GameWinningScore
		|| RedTeamGameScore >= GameWinningScore)
	{
		CurrentPhase = ECompetitiveGamePhase::GameEnd;
	}
	else
	{
		CurrentPhase = ECompetitiveGamePhase::RoundEnd;
	}
	CurrentPhaseTime = 0.0f;
}
