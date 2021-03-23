// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "SurvivalGame.h"
#include "Player_Manager.h"

#include "Math/Vector.h"

// Sets default values
APlayer_Manager::APlayer_Manager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void APlayer_Manager::BeginPlay()
{
	Super::BeginPlay();

	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player_Manager Begin Play!! ")));

	StopChange = true;
	Stage++;

	SetActorTickInterval(0.033f);
	MyInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	MyGameState = Cast<ASGameState>(GetWorld()->GetAuthGameMode()->GameState);

	MakeStartLocation();
	SpawnPlayers();

	TActorIterator<AZombie_Manager> It(GetWorld());
	zombie_manager = *It;

	if (Stage >= 2)
	{
		TActorIterator<AItem_Manager> It2(GetWorld());
		item_manager = *It2;
	}

	for (int i = 0; i < MAX_ZOMBIE; ++i)
	{
		Zombie_info.Target[i] = -1;
	}

	
	
}


// Called every frame
void APlayer_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//auto ItemArray = item_manager->GetItemArray();
	//for (int i = 0; i < MAX_ITEM; ++i)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d ITEM IS USED : %d"), i, Item_info.IsUsed[i]));
	//	if ((*ItemArray).IsValidIndex(i))
	//	{
	//		if((*ItemArray)[i] != nullptr)
	//			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d ITEM IS NOT NULL"), i));
	//		else
	//			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d ITEM IS NULL"), i));

	//	}
	//}

	if (Connected)
	{
		if (!AllChanged)
		{	
			if (StopChange)
			{
				Changed = true;
				MyInstance->SendChange();

				MySocket::RecvPacket();
				//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("AllChanged : %d"), AllChanged));
			}
			for (int i = 0; i < MAX_ZOMBIE; ++i)
			{
				Zombie_info.IsAlive[i] = true;
			}
			for (int i = 0; i < MAX_ITEM; ++i)
			{
				Item_info.IsUsed[i] = false;
			}
		}
		else
		{
			StopChange = false;
			if (!(players[PlayerId]->IsDie()))
			{
				ASCharacter* MyPawn = players[PlayerId];
				Player_info.HP[PlayerId] = MyPawn->GetHealth();
				/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player HP : %f"),
					 Player_info.HP[PlayerId]));*/
				FVector MyLocation = MyPawn->GetActorLocation();
				Player_info.Loc[PlayerId].x = MyLocation.X;
				Player_info.Loc[PlayerId].y = MyLocation.Y;
				Player_info.Loc[PlayerId].z = MyLocation.Z;

				FRotator MyRotation = MyPawn->GetActorRotation();
				Player_info.Rot[PlayerId].yaw = MyRotation.Yaw;
				Player_info.Rot[PlayerId].pitch = MyRotation.Pitch;
				Player_info.Rot[PlayerId].roll = MyRotation.Roll;

				FVector MyVelocity = MyPawn->GetVelocity();
				Player_info.Vel[PlayerId].x = MyVelocity.X;
				Player_info.Vel[PlayerId].y = MyVelocity.Y;
				Player_info.Vel[PlayerId].z = MyVelocity.Z;

				FRotator MyAim = MyPawn->GetAimOffsets();
				Player_info.Aim[PlayerId].yaw = MyAim.Yaw;
				Player_info.Aim[PlayerId].pitch = MyAim.Pitch;
				Player_info.Aim[PlayerId].roll = MyAim.Roll;

				FVector POVLoc;
				FRotator POVRot;
				if (MyPawn->GetHealth() > 0)
					MyPawn->Controller->GetPlayerViewPoint(POVLoc, POVRot);
				Player_info.View[PlayerId].Loc.x = POVLoc.X;
				Player_info.View[PlayerId].Loc.y = POVLoc.Y;
				Player_info.View[PlayerId].Loc.z = POVLoc.Z;
				Player_info.View[PlayerId].Rot.yaw = POVRot.Yaw;
				Player_info.View[PlayerId].Rot.pitch = POVRot.Pitch;
				Player_info.View[PlayerId].Rot.roll = POVRot.Roll;

				/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Player View Rot : Pitch - %f , Roll - %f , Yaw - %f"),
				   Player_info.View[PlayerId].Rot.pitch, Player_info.View[PlayerId].Rot.roll, Player_info.View[PlayerId].Rot.yaw ));*/

				Player_info.WeaponState[PlayerId] = MyPawn->GetOtherWeaponState();
			}

			Player_info.SirenButton = MyInstance->IsSirenPushed();
			if (PlayerId == HostPlayer)
			{
				Player_info.ElapsedTime = MyGameState->ElapsedGameMinutes;
			}

			S_Players* S_Player_Packet = new S_Players;
			if (Player_info.HP[PlayerId] <= 0.0f)
				S_Player_Packet->HP = 0;
			S_Player_Packet->HP = Player_info.HP[PlayerId];
			S_Player_Packet->Loc = Player_info.Loc[PlayerId];
			S_Player_Packet->Rot = Player_info.Rot[PlayerId];
			S_Player_Packet->Vel = Player_info.Vel[PlayerId];
			S_Player_Packet->Aim = Player_info.Aim[PlayerId];
			S_Player_Packet->IsJump = Player_info.IsJump[PlayerId];
			S_Player_Packet->IsTargeting = Player_info.IsTargeting[PlayerId];
			S_Player_Packet->IsSprinting = Player_info.IsSprinting[PlayerId];
			S_Player_Packet->onCrouchToggle = Player_info.onCrouchToggle[PlayerId];
			S_Player_Packet->WeaponState = Player_info.WeaponState[PlayerId];
			S_Player_Packet->View = Player_info.View[PlayerId];
			S_Player_Packet->WeaponNum = Player_info.WeaponNum[PlayerId];
			S_Player_Packet->Kit = Player_info.Kit[PlayerId];
			S_Player_Packet->SirenButton = Player_info.SirenButton;
			S_Player_Packet->ElapsedTime = Player_info.ElapsedTime;



			MySocket::sendBuffer(PACKET_CS_PLAYERS, &(*S_Player_Packet));

			MySocket::RecvPacket();

			////

			

			if (Playing > 1)
			{
				if (Player_info.SirenButton == true)
					MyInstance->SirenPush();

				for (int i = 0; i < MAX_USER; ++i)
				{
					if ((i != PlayerId) && Player_info.IsUsed[i])
					{
						if (players.IsValidIndex(i))
						{
							if (players[i]->IsDie())
								players.RemoveAt(i);
							else
							{

								players[i]->SetOtherHealth(Player_info.HP[i]);
								FVector NewLocation;
								NewLocation.X = Player_info.Loc[i].x;
								NewLocation.Y = Player_info.Loc[i].y;
								NewLocation.Z = Player_info.Loc[i].z;

								FRotator NewRotation;
								NewRotation.Pitch = Player_info.Rot[i].pitch;
								NewRotation.Yaw = Player_info.Rot[i].yaw;
								NewRotation.Roll = Player_info.Rot[i].roll;
								FVector NewVelocity;
								NewVelocity.X = Player_info.Vel[i].x;
								NewVelocity.Y = Player_info.Vel[i].y;
								NewVelocity.Z = Player_info.Vel[i].z;
								FRotator NewAim;
								NewAim.Pitch = Player_info.Aim[i].pitch;
								NewAim.Yaw = Player_info.Aim[i].yaw;
								NewAim.Roll = Player_info.Aim[i].roll;

								players[i]->SetActorRelativeRotation(NewRotation);
								players[i]->SetAimOffset(NewAim);
								players[i]->GetMovementComponent()->Velocity = NewVelocity;
								players[i]->AddMovementInput(NewVelocity);

								const FVector NewLoc = NewLocation;

								if ((FVector::Dist(players[i]->GetActorLocation(), NewLoc)) > 100.0f)
								{
									players[PlayerId]->SetActorEnableCollision(false);
									players[i]->SetActorEnableCollision(false);
									//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d Player Distance > 200.0f"), i + 1));
									//const FVector InterpVec = FMath::VInterpTo(players[i]->GetActorLocation(), NewLoc, DeltaTime, NewVelocity.Size());
									players[i]->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);
									players[PlayerId]->SetActorEnableCollision(true);
									players[i]->SetActorEnableCollision(true);

								}
								else
								{
									//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d Player Distance < 10.0f"), i + 1));
								}

								players[i]->SetIsJumping(Player_info.IsJump[i]);
								players[i]->SetIsTargeting(Player_info.IsTargeting[i]);
								players[i]->SetSprinting(Player_info.IsSprinting[i]);

								if (Player_info.onCrouchToggle[i] != players[i]->GetCrouched())
									players[i]->OnCrouchToggle();
								/*   GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d Player Crouched : %d, %d"),
								   i, Player_info.onCrouchToggle[i], players[i]->GetCrouched()));*/

								players[i]->OtherChangeWeapon(Player_info.WeaponNum[i]);

								if (Player_info.WeaponState[i] == WEAPON_FIRING)
								{
									players[i]->StartFiringOther();
								}
								else if (Player_info.WeaponState[i] == WEAPON_RELOADING)
								{

									players[i]->ReloadingOther();
								}
								else
									players[i]->StopFiringOther();

								players[i]->SetKit(Player_info.Kit[i]);
							}
						}
					}
				}
				if (!(MyInstance->IsHost()))
					MyGameState->ElapsedGameMinutes = Player_info.ElapsedTime;

				////
				auto ZombieArray = zombie_manager->GetZombieArray();
				if (MyInstance->IsHost())
				{
					for (int i = 0; i < MAX_ZOMBIE; ++i)
					{
						if ((*ZombieArray).IsValidIndex(i))
						{
							if ((*ZombieArray)[i] != nullptr)
							{
								Zombie_info.HP[i] = (*ZombieArray)[i]->GetHealth();
								Zombie_info.Loc[i].x = (*ZombieArray)[i]->GetActorLocation().X;
								Zombie_info.Loc[i].y = (*ZombieArray)[i]->GetActorLocation().Y;
								Zombie_info.Loc[i].z = (*ZombieArray)[i]->GetActorLocation().Z;
							}
						}
					}
				}
				S_Zombies* S_Zombie_Packet = new S_Zombies;

			/*	if (MyInstance->IsHost())
				{*/
					for (int i = 0; i < MAX_ZOMBIE; ++i)
					{
						S_Zombie_Packet->IsAlive[i] = Zombie_info.IsAlive[i];
						S_Zombie_Packet->Target[i] = Zombie_info.Target[i];
						S_Zombie_Packet->HP[i] = Zombie_info.HP[i];
						S_Zombie_Packet->Loc[i] = Zombie_info.Loc[i];
					}
				//}
			
				MySocket::sendBuffer(PACKET_CS_ZOMBIE, &(*S_Zombie_Packet));

				MySocket::RecvPacket();

				if (!(MyInstance->IsHost()))
				{
					if (HostZombie)
					{
						for (int i = 0; i < (*ZombieArray).Num(); ++i)
						{
							if (Zombie_info.IsAlive[i] && (Zombie_info.HP[i] > 0))
							{
								if ((*ZombieArray).IsValidIndex(i))
								{
									if ((*ZombieArray)[i] != nullptr)
									{
										if (!((*ZombieArray)[i]->IsPendingKill()))
										{
											if (Zombie_info.HP[i] < (*ZombieArray)[i]->GetHealth())
												(*ZombieArray)[i]->SetHP(Zombie_info.HP[i]);

											if (Zombie_info.Target[i] != -1)
											{
												if (Zombie_info.Target[i] < Playing)
												{
													//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("%d Zombie Target : %d "),
													//	i, Zombie_info.Target[i]));
													ASZombieAIController* ZombieController = Cast<ASZombieAIController>((*ZombieArray)[i]->GetController());
													if (ZombieController != nullptr)
													{
														ZombieController->SetTargetEnemy(Cast<APawn>(players[Zombie_info.Target[i]]));
														(*ZombieArray)[i]->SetSensedTarget(true);
													}
													/*else
														GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, FString::Printf(TEXT("%d Zombie Contrller Is NULL "),i));*/
												}
											}
											else
											{
												ASZombieAIController* ZombieController = Cast<ASZombieAIController>((*ZombieArray)[i]->GetController());
												if (ZombieController != nullptr)
												{
													ZombieController->SetTargetEnemy(nullptr);
													(*ZombieArray)[i]->SetSensedTarget(false);
												}
												/*else
													GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, FString::Printf(TEXT("%d Zombie Contrller Is NULL "), i));*/
											}

											FVector Location;
											Location.X = Zombie_info.Loc[i].x;
											Location.Y = Zombie_info.Loc[i].y;
											Location.Z = Zombie_info.Loc[i].z;
											const FVector NewLoc = Location;

											if (FVector::Dist((*ZombieArray)[i]->GetActorLocation(), NewLoc) > 20.0f)
											{
												/*	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("ZombieArray[%d] X : %f, Y : %f, Z : %f"),
														i, (*ZombieArray)[i]->GetActorLocation().X, (*ZombieArray)[i]->GetActorLocation().Y, (*ZombieArray)[i]->GetActorLocation().Z));
													GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("ZombieInfo[%d] X : %f, Y : %f, Z : %f"),
														i, Zombie_info.Loc[i].x, Zombie_info.Loc[i].y, Zombie_info.Loc[i].z));*/
												(*ZombieArray)[i]->SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);
											}
										}
									}
								}

							}
							else
							{
								if ((*ZombieArray)[i] != nullptr)
								{
									(*ZombieArray)[i]->Destroy();
									(*ZombieArray)[i] = nullptr;
								}
							}
						}
					}
				}
				else
				{
					if (HostZombie)
					{
						for (int i = 0; i < (*ZombieArray).Num(); ++i)
						{
							if (!(Zombie_info.IsAlive[i]))
							{
								if ((*ZombieArray).IsValidIndex(i))
								{
									if ((*ZombieArray)[i] != nullptr)
									{
										if (!((*ZombieArray)[i]->IsPendingKill()))
										{
											(*ZombieArray)[i]->Destroy();
											(*ZombieArray)[i] = nullptr;
											GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, FString::Printf(TEXT("%d Zombie Is DELTED !!"), i));
										}
									}
								}
							}
						}

					}
				}

				if (Stage == 2)
				{
					S_Item* S_Item_Packet = new S_Item;
					for (int i = 0; i < MAX_ITEM; ++i)
					{
						S_Item_Packet->IsUsed[i] = Item_info.IsUsed[i];
					}
				
					MySocket::sendBuffer(PACKET_CS_ITEM, &(*S_Item_Packet));
					MySocket::RecvPacket();

					auto ItemArray = item_manager->GetItemArray();

					for (int i = 0; i < MAX_ITEM; ++i)
					{
						if (Item_info.IsUsed[i])
						{
							if ((*ItemArray).IsValidIndex(i))
							{
								if ((*ItemArray)[i] != nullptr)
								{
									(*ItemArray)[i]->Destroy();
									(*ItemArray)[i] = nullptr;
									GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, FString::Printf(TEXT("%d ITEM Is DELTED !!"), i));
								}
							}
						}
					}
				}


			}
		}
	}
}



void APlayer_Manager::MakeStartLocation()
{
	UWorld* World = GetWorld();
	ASCharacter* MyCharacter = Cast<ASCharacter>(World->GetFirstPlayerController()->GetCharacter());

	players.Emplace(MyCharacter);

	FVector PlayerStart = MyCharacter->GetActorLocation();      // 플레이어별 시작지점 지정
	PlayerStartLocation.Emplace(PlayerStart);                     // Player1
	PlayerStart.Y += 150;
	PlayerStartLocation.Emplace(PlayerStart);                     // Player2
	PlayerStart.X -= 150;
	PlayerStartLocation.Emplace(PlayerStart);                     // Player3
	PlayerStart.Y -= 150;
	PlayerStartLocation.Emplace(PlayerStart);                     // Player4
}

void APlayer_Manager::SpawnPlayers()
{
	UWorld* World = GetWorld();
	ASCoopGameMode* MyGameMode = Cast<ASCoopGameMode>(World->GetAuthGameMode());
	ASPlayerController* aiController = nullptr;

	if (Playing > 1)
	{
		UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Class'/Game/Player/Main_Player.Main_Player_C'")));

		UClass* GenerateBp = Cast<UClass>(SpawnActor);

		if (!SpawnActor)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CAN'T FIND OBJECT TO SPAWN")));
		}

		UClass* SpawnClass = SpawnActor->StaticClass();
		if (SpawnClass == NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CLASS == NULL")));
		}


		FActorSpawnParameters Spawnparams;
		Spawnparams.Owner = this;
		Spawnparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASPlayerController* Mycontroller = Cast<ASPlayerController>(World->GetFirstPlayerController());

		ASCharacter* MyCharacter = Cast<ASCharacter>(Mycontroller->GetPawn());
		ASPlayerState* MyCharacterState = Cast< ASPlayerState>(MyCharacter->GetPlayerState());


		for (int i = 1; i < Playing; ++i) {
			ASCharacter* NewCharacter = World->SpawnActor<ASCharacter>(GenerateBp, PlayerStartLocation[i], FRotator::ZeroRotator, Spawnparams);
			NewCharacter->SetPlayerState(MyCharacterState);
			ASPlayerState* NewPlayerState = Cast< ASPlayerState>(NewCharacter->GetController()->PlayerState);
			if (NewPlayerState)
			{

				NewPlayerState->SetTeamNumber(MyCharacterState->GetTeamNumber());
			}
			if (aiController == nullptr)
			{
				aiController = Cast<ASPlayerController>(NewCharacter->GetController());
			}

			MyGameMode->SetOtherPlayerDefaults(NewCharacter);
			players.Emplace(NewCharacter);
		}

		if (!(PlayerId == 0))
			Mycontroller->Possess(players[PlayerId]);


		int num = 0;
		for (auto i = players.begin(); i != players.end(); ++i)
		{
			if (!((*i)->IsPlayerControlled()))
			{
				aiController->Possess(*i);
				(*i)->SetPlayerState(MyCharacterState);

			}
			(*i)->PlayerNum = num;
			++num;
		}
	}



}

TArray<ASCharacter*> APlayer_Manager::GetPlayerArray()
{
	return players;
}