// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"

#include "StartUI_Manager.h"

// Sets default values
AStartUI_Manager::AStartUI_Manager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStartUI_Manager::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(0.16f);

}

// Called every frame
void AStartUI_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connected : %d"), Connected));
	


	if (Connected)
	{
		/*UWorld* world = GetWorld();
		UMyGameInstance* MyInstance = Cast< UMyGameInstance>(world->GetGameInstance());

		MyInstance->Start();*/


		/*TESTING* test = new TESTING;
		MySocket::sendBuffer(PACKET_TEST, &(*test));*/

		if (!PlayerLogin)
		{
			S_Login* Login = new S_Login;
			MySocket::sendBuffer(PACKET_CS_LOGIN, &(*Login));
			PlayerLogin = true;

		}		

		MySocket::RecvPacket();
		//Recving = true;
	}
}

void AStartUI_Manager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

}