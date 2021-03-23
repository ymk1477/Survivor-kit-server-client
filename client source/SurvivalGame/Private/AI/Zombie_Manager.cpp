// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "MyGameInstance.h"

#include "Zombie_Manager.h"

// Sets default values
AZombie_Manager::AZombie_Manager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AZombie_Manager::FindSpawnPoints()
{
	UWorld* CurrentWorld = GetWorld();
	for (TActorIterator<AZombieSpawnPoint> It(CurrentWorld); It; ++It)
	{
		AZombieSpawnPoint* NewPoint = *It;
		ZombieSpawnPoints.Emplace(NewPoint);
	}

	for (TActorIterator< APatrolZombieSpawnPoint> It(CurrentWorld); It; ++It)
	{
		APatrolZombieSpawnPoint* NewPoint = *It;
		PatrolZombieSpawnPoints.Emplace(NewPoint);
	}

	for (TActorIterator< AFastZombieSpawnPoint> It(CurrentWorld); It; ++It)
	{
		AFastZombieSpawnPoint* NewPoint = *It;
		FastZombieSpawnPoints.Emplace(NewPoint);
	}

	for (TActorIterator< ACoronaZombieSpawnPoint> It(CurrentWorld); It; ++It)
	{
		ACoronaZombieSpawnPoint* NewPoint = *It;
		CoronaZombieSpawnPoints.Emplace(NewPoint);
	}

	for (TActorIterator< AZombieAttackPoint> It(CurrentWorld); It; ++It)
	{
		AZombieAttackPoint* NewPoint = *It;
		ZombieAttackPoints.Emplace(NewPoint);
	}

}

void AZombie_Manager::SpawnZombies()
{
	UWorld* CurrentWorld = GetWorld();
	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/FreeModels/Zombie_5/Bite_neck.Bite_neck_C'")));
	UClass* GenerateBp = Cast<UClass>(SpawnActor);

	FActorSpawnParameters Spawnparams;
	Spawnparams.Owner = this;
	Spawnparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FRotator NewRotator;

	int ZombieNum = 0;

	for (auto i = ZombieSpawnPoints.begin(); i != ZombieSpawnPoints.end(); ++i)
	{
		//NewRotator.Yaw = FMath::RandRange(0.0f, 360.0f);
		ASZombieCharacter* NewZombie = CurrentWorld->SpawnActor<ASZombieCharacter>(GenerateBp, (*i)->GetActorLocation(), FRotator::ZeroRotator, Spawnparams);
		ASZombieAIController* ZombieController = Cast<ASZombieAIController>(NewZombie->GetController());
		Zombies.Emplace(NewZombie);
		ZombieController->Possess(NewZombie);
		Zombie_info.HP[ZombieNum] = NewZombie->GetHealth();
		Zombie_info.IsAlive[ZombieNum] = true;
		Zombie_info.Target[ZombieNum] = -1;
		Zombie_info.Loc[ZombieNum].x = NewZombie->GetActorLocation().X;
		Zombie_info.Loc[ZombieNum].y = NewZombie->GetActorLocation().Y;
		Zombie_info.Loc[ZombieNum].z = NewZombie->GetActorLocation().Z;
		NewZombie->Zombie_Index = ZombieNum;
		++ZombieNum;
	}

	UObject* SpawnActor2 = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/zombie/ZombiePatrol1.ZombiePatrol1_C'")));
	UClass* GenerateBp2 = Cast<UClass>(SpawnActor2);

	for (auto i = PatrolZombieSpawnPoints.begin(); i != PatrolZombieSpawnPoints.end(); ++i)
	{
		ASZombieCharacter* NewPatrolZombie = CurrentWorld->SpawnActor<ASZombieCharacter>(GenerateBp2, (*i)->GetActorLocation(), FRotator::ZeroRotator, Spawnparams);
		ASZombieAIController* ZombieController = Cast<ASZombieAIController>(NewPatrolZombie->GetController());
		Zombies.Emplace(NewPatrolZombie);
		ZombieController->Possess(NewPatrolZombie);
		Zombie_info.HP[ZombieNum] = NewPatrolZombie->GetHealth();
		Zombie_info.IsAlive[ZombieNum] = true;
		Zombie_info.Target[ZombieNum] = -1;
		Zombie_info.Loc[ZombieNum].x = NewPatrolZombie->GetActorLocation().X;
		Zombie_info.Loc[ZombieNum].y = NewPatrolZombie->GetActorLocation().Y;
		Zombie_info.Loc[ZombieNum].z = NewPatrolZombie->GetActorLocation().Z;
		NewPatrolZombie->Zombie_Index = ZombieNum;
		++ZombieNum;
	}
	
	UObject* SpawnActor3 = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/FreeModels/Zombie_3/Zombie_Fast.Zombie_Fast_C'")));
	UClass* GenerateBp3 = Cast<UClass>(SpawnActor3);

	for (auto i = FastZombieSpawnPoints.begin(); i != FastZombieSpawnPoints.end(); ++i)
	{
		ASZombieCharacter* NewFastZombie = CurrentWorld->SpawnActor<ASZombieCharacter>(GenerateBp3, (*i)->GetActorLocation(), FRotator::ZeroRotator, Spawnparams);
		ASZombieAIController* ZombieController = Cast<ASZombieAIController>(NewFastZombie->GetController());
		Zombies.Emplace(NewFastZombie);
		ZombieController->Possess(NewFastZombie);
		Zombie_info.HP[ZombieNum] = NewFastZombie->GetHealth();
		Zombie_info.IsAlive[ZombieNum] = true;
		Zombie_info.Target[ZombieNum] = -1;
		Zombie_info.Loc[ZombieNum].x = NewFastZombie->GetActorLocation().X;
		Zombie_info.Loc[ZombieNum].y = NewFastZombie->GetActorLocation().Y;
		Zombie_info.Loc[ZombieNum].z = NewFastZombie->GetActorLocation().Z;
		NewFastZombie->Zombie_Index = ZombieNum;
		++ZombieNum;;
	}

	UObject* SpawnActor4 = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/FreeModels/Zombie_4/CoronaZombie.CoronaZombie_C'")));
	UClass* GenerateBp4 = Cast<UClass>(SpawnActor4);

	for (auto i = CoronaZombieSpawnPoints.begin(); i != CoronaZombieSpawnPoints.end(); ++i)
	{
		ASZombieCharacter* NewCoronaZombie = CurrentWorld->SpawnActor<ASZombieCharacter>(GenerateBp4, (*i)->GetActorLocation(), FRotator::ZeroRotator, Spawnparams);
		ASZombieAIController* ZombieController = Cast<ASZombieAIController>(NewCoronaZombie->GetController());
		Zombies.Emplace(NewCoronaZombie);
		ZombieController->Possess(NewCoronaZombie);
		Zombie_info.HP[ZombieNum] = NewCoronaZombie->GetHealth();
		Zombie_info.IsAlive[ZombieNum] = true;
		Zombie_info.Target[ZombieNum] = -1;
		Zombie_info.Loc[ZombieNum].x = NewCoronaZombie->GetActorLocation().X;
		Zombie_info.Loc[ZombieNum].y = NewCoronaZombie->GetActorLocation().Y;
		Zombie_info.Loc[ZombieNum].z = NewCoronaZombie->GetActorLocation().Z;
		NewCoronaZombie->Zombie_Index = ZombieNum;
		++ZombieNum;
	}

	AllZombies = ZombieNum;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("SPAWN ZOMBIES() ")));
}

void AZombie_Manager::FindWayPoints()
{
	UWorld* CurrentWorld = GetWorld();
	for (TActorIterator<ASBotWaypoint> It(CurrentWorld); It; ++It)
	{
		ASBotWaypoint* NewPoint = *It;
		Waypoints.Emplace(NewPoint);
	}
}

// Called when the game starts or when spawned
void AZombie_Manager::BeginPlay()
{
	Super::BeginPlay();

	AllZombies = 0;
	PatrolZombieNumber = 0;
	FindSpawnPoints();
	FindWayPoints();
	SpawnZombies();
}

// Called every frame
void AZombie_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (HostPlayer == PlayerId)
	{*/
		int32 indexNum = -1;
		if (Zombies.Num() > 0)
		{
			for (int32 i = 0; i < Zombies.Num(); ++i)
			{
				if (Zombies[i] != nullptr && Zombies[i]->IsDie())
				{
					Zombie_info.IsAlive[i] = false;
					Zombie_info.Target[i] = -1;
					indexNum = i;
					Zombies[i] = nullptr;
				}
			}
		}
	//}
	/*for (int32 i = 0; i < Zombies.Num(); ++i)
	{
		if (Zombies[i] == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Zombie %d : NULL "),i));
		}
	}*/
}

TArray<ASZombieCharacter*>* AZombie_Manager::GetZombieArray()
{
	return &Zombies;
}

TArray<ASBotWaypoint*> AZombie_Manager::GetWayPoints()
{
	return Waypoints;
}

TArray<ASZombieCharacter*> AZombie_Manager::GetPatrolZombies()
{
	return PatrolZombies;
}

int32 AZombie_Manager::GetPatrolNumber()
{
	PatrolZombieNumber++;
	return PatrolZombieNumber;
}

void AZombie_Manager::ZombieAttackSpawn()
{
	for (int i = 0; i < Zombies.Num(); ++i)
	{
		if(Zombie_info.IsAlive[i] && Zombies[i] != nullptr)
			Zombies[i]->Destroy();
	}
	Zombies.Empty();
	SpawnZombies();
	/*auto AttackPoints = ZombieAttackPoints.begin();
	for (auto p = Zombies.begin(); p != Zombies.end(); ++p)
	{
		(*p)->SetActorLocation((*AttackPoints)->GetActorLocation(), true, nullptr, ETeleportType::None);
		AttackPoints++;
	}*/

	for (int i = 0; i < Zombies.Num(); ++i)
	{
		Zombies[i]->SetActorLocation(ZombieAttackPoints[i]->GetActorLocation());
	}
}