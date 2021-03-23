// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Sockets/Public/SocketSubsystem.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Engine/GameInstance.h"

#include "MyGameInstance.generated.h"

#define MAX_USER 4
#define MAX_ZOMBIE 30
#define MAX_ITEM 6
#define SERVERPORT 9000
#define MAX_BUFFER 2048

#define PACKET_TEST 100

#define WEAPON_IDLE 50
#define WEAPON_FIRING 51
#define WEAPON_RELOADING 52

#define PACKET_SC_LOGIN 0
#define PACKET_CS_LOGIN 1

#define PACKET_SC_GAME_START 2
#define PACKET_CS_GAME_START 3

#define PACKET_SC_PLAYERS 4
#define PACKET_CS_PLAYERS 5

#define PACKET_CS_CHANGE 6

#define PACKET_SC_CHANGE 7

//////////

#define PACKET_SC_LOCATION 8
#define PACKET_SC_JUMP 9

#define PACKET_SC_ITEM 10
#define PACKET_CS_ITEM 11

#define PACKET_SC_ZOMBIE 13
#define PACKET_CS_ZOMBIE 16

typedef struct LOCATION {
	float x;
	float y;
	float z;
}Location;

typedef struct Rotation {
	float yaw;
	float pitch;
	float roll;
}Rotation;

typedef struct Velocity {
	float x;
	float y;
	float z;
}Velocity;

typedef struct AimOffset {
	float yaw;
	float pitch;
	float roll;
}AimOffset;

typedef struct ViewPoint
{
	Location Loc;
	Rotation Rot;
}ViewPoint;

typedef struct Info_Player {
	bool IsUsed[MAX_USER] = { false };
	int Host = -1;
	float HP[MAX_USER];
	Location Loc[MAX_USER];
	Rotation Rot[MAX_USER];
	Velocity Vel[MAX_USER];
	AimOffset Aim[MAX_USER];
	bool IsJump[MAX_USER] = { false };
	bool IsTargeting[MAX_USER] = { false };
	bool IsSprinting[MAX_USER] = { false };
	bool onCrouchToggle[MAX_USER] = { false };
	//bool UseWeapon[MAX_USER] = { false };
	int WeaponState[MAX_USER] = { WEAPON_IDLE };
	ViewPoint View[MAX_USER];
	int WeaponNum[MAX_USER] = { 1 };
	int Kit[MAX_USER] = { 0 };
	bool SirenButton = false;
	int ElapsedTime = 0;
}Player;

typedef struct Info_Zombie {
	float HP[MAX_ZOMBIE];
	bool IsAlive[MAX_ZOMBIE] = { true, };
	int Target[MAX_ZOMBIE] = { -1 };
	Location Loc[MAX_ZOMBIE];
	//bool Hit[MAX_ZOMBIE] = { false };

}Zombie;

typedef struct Info_Item
{
	bool IsUsed[MAX_ITEM];
}Item;

#pragma pack(push ,1)


typedef struct Test_Packet {
	int packet_type;
	int i;
}R_Test;

typedef struct Send_Packet_Login {
	char packet_type = PACKET_CS_LOGIN;
}S_Login;

typedef struct Recv_Packet_Login {
	char packet_type = PACKET_SC_LOGIN;
	int clientId;
	bool Player[MAX_USER] = { false };
	int Host;
}R_Login;

typedef struct TTT {
	char packet_type = PACKET_TEST;
}TESTING;


typedef struct Send_Packet_GAME_START {
	char packet_type = PACKET_CS_GAME_START;
}S_Start;

typedef struct Recv_Packet_GAME_START {
	char packet_type = PACKET_SC_GAME_START;
	bool Started;
}R_Start;

typedef struct Send_Packet_Players {
	char packet_type = PACKET_CS_PLAYERS;
	float HP;
	Location Loc;
	Rotation Rot;
	Velocity Vel;
	AimOffset Aim;
	bool IsJump;
	bool IsTargeting;
	bool IsSprinting;
	bool onCrouchToggle;
	int WeaponState;
	ViewPoint View;
	int WeaponNum;
	int Kit;
	bool SirenButton;
	int ElapsedTime = 0;
}S_Players;

typedef struct Recv_Packet_Players {
	char packet_type = PACKET_SC_PLAYERS;
	int Host;
	bool IsUsed[MAX_USER];
	float HP[MAX_USER];
	Location Loc[MAX_USER];
	Rotation Rot[MAX_USER];
	Velocity Vel[MAX_USER];
	AimOffset Aim[MAX_USER];
	bool IsJump[MAX_USER];
	bool IsTargeting[MAX_USER];
	bool IsSprinting[MAX_USER];
	bool onCrouchToggle[MAX_USER];
	int WeaponState[MAX_USER];
	ViewPoint View[MAX_USER];
	int WeaponNum[MAX_USER] = { 1 };
	int Kit[MAX_USER];
	bool SirenButton;
	int ElapsedTime;
}R_Players;

typedef struct Recv_Packet_Zombie
{
	char packet_type = PACKET_SC_ZOMBIE;
	bool IsAlive[MAX_ZOMBIE];
	int Target[MAX_ZOMBIE];
	float HP[MAX_ZOMBIE];
	Location Loc[MAX_ZOMBIE];
	bool HostZombieInfo;
}R_Zombies;

typedef struct Send_Packet_Zombie
{
	char packet_type = PACKET_CS_ZOMBIE;
	bool IsAlive[MAX_ZOMBIE]; 
	int Target[MAX_ZOMBIE];
	float HP[MAX_ZOMBIE];
	Location Loc[MAX_ZOMBIE];
}S_Zombies;

typedef struct Send_Packet_Stage_Change
{
	char packet_type = PACKET_CS_CHANGE;
	bool Changed;
}S_Change;

typedef struct Recv_Packet_Stage_Change
{
	char packet_type = PACKET_SC_CHANGE;
	bool AllChanged;
}R_Change;

typedef struct Send_Packet_Item
{
	char packet_type = PACKET_CS_ITEM;
	bool IsUsed[MAX_ITEM];
}S_Item;

typedef struct Recv_Packet_Item
{
	char packet_type = PACKET_SC_ITEM;
	bool IsUsed[MAX_ITEM];		
}R_Item;

#pragma pack(pop)

class MySocket {
private:
	MySocket() {}
public:
	static FSocket& getInstance();

	static void initializeServer();
	static void sendBuffer(int PacketType, void* BUF);
	static void RecvPacket();
	static void CloseSocket();
	static int Get_Packet_Size(char* start);
	static void Process_Packet();
};

static Player Player_info;
static Zombie Zombie_info;
static int PlayerId;
static int Playing;
static FString Adress;
static bool Connected;
static int HostPlayer;
static bool GameStart;
static bool PlayerLogin;
static bool All_level_Changed;
static int Elapsed_Time;
static bool SirenPushed;
static bool AllChanged;
static bool Changed;
static bool StageChange;
static bool HostZombie;
static Item Item_info;
static int Stage;
/**
 *
 */
UCLASS()
class SURVIVALGAME_API UMyGameInstance : public UGameInstance
{

	GENERATED_BODY()

private:

public:

	UMyGameInstance();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SetIpAdress(FString Ip_Adress);

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool ConnectServer();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsConected();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		int GetPlayer();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsHost();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsStart();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void Start();

	UFUNCTION(BlueprintCallable, Category = "TimeOfDay")
		bool IsAllLevelChange();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void FinishGame();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SirenPush();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool IsSirenPushed();
	
	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SetChange(bool Change);

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		bool GetChange();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SendChange();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void ZeroBuff();

	UFUNCTION(BlueprintCallable, Category = "My_Server")
		void SetAllChange(bool val);

	~UMyGameInstance();
};
