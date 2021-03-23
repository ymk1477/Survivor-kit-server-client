#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define SERVER_PORT 9000
#define MAX_BUFFER_SIZE 2048
#define MAX_PACKET_SIZE 2048
#define MAX_USER 4
#define MAX_ZOMBIE 30

// Clients -> Server
//#define PACKET_CS_LOGIN 0
//#define PACKET_CS_START 1
//
//#define PACKET_SC_LOGIN 100

#define PACKET_TEST 100

#define WEAPON_IDLE 50
#define WEAPON_FIRING 51
#define WEAPON_RERODING 52

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


#define PACKET_CS_LOCATION 10
#define PACKET_CS_JUMP 11

#define PACKET_SC_ZOMBIE_INIT 12
#define PACKET_SC_ZOMBIE 13
#define PACKET_SC_ZOMBIE_LOC 14

#define PACKET_CS_ZOMBIE_INIT 15
#define PACKET_CS_ZOMBIE 16
#define PACKET_CS_ZOMBIE_LOC 17

#define PACKET_SC_TIME 18
#define PACKET_CS_TIME 19

#define PACKET_SC_COMBINE 20
#define PACKET_CS_COMBINE 21


enum EOperation
{
	E_RECV,
	E_SEND,
	E_ACCEPT
};

struct ExOverlapped {
	WSAOVERLAPPED Overlapped{};
	EOperation Operation{};
	char IoBuffer[MAX_BUFFER_SIZE]{};
	union 
	{
		WSABUF WsaBuf;
		SOCKET ClientSocket;
	};
	
};

struct Client {
	SOCKET socket{};
	int id{};
	ExOverlapped RecvOverlap{};
	int prevSize{};
	char packetBuffer[MAX_PACKET_SIZE]{};
	bool isUsed = false;
};

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

typedef struct ViewPoint {
	Location Loc;
	Rotation Rot;
}ViewPoint;

typedef struct Info_Player {
	int Host = -1;
	bool IsUsed[MAX_USER] = { false };
	float HP[MAX_USER];
	Location Loc[MAX_USER];
	Rotation Rot[MAX_USER];
	Velocity Vel[MAX_USER];
	AimOffset Aim[MAX_USER];
	bool IsJump[MAX_USER] = { false };
	bool isTargeting[MAX_USER] = { false };
	bool IsSprinting[MAX_USER] = { false };
	bool onCrouchToggle[MAX_USER] = { false };
	int WeaponState[MAX_USER] = { WEAPON_IDLE };
	ViewPoint View[MAX_USER];
	int WeaponNum[MAX_USER] = { 1 };
	int Kit[MAX_USER] = { 0 };
	bool SirenButton = false;
	int ElapsedTime = 0;
}Player;

typedef struct Info_Zombie {
	float HP[MAX_ZOMBIE];
	bool IsAlive[MAX_ZOMBIE] = { false };
	int Target[MAX_ZOMBIE] = { -1 };
	Location Loc[MAX_ZOMBIE];
}Zombie;

#pragma pack(push, 1)

typedef struct Testing_Packet
{
	char packet_type = PACKET_TEST;

}TEST;

typedef struct Send_Packet_Login 
{
	char packet_type = PACKET_SC_LOGIN;
	int clientId;
	bool Player[MAX_USER] = { false };
	int Host;
}S_Login;

typedef struct Recv_Packet_Login {
	char packet_type = PACKET_CS_LOGIN;
}R_Login;

typedef struct Send_Packet_GAME_START 
{
	char packet_type = PACKET_SC_GAME_START;
	bool Started;
}S_Start;

typedef struct Recv_Packet_GAME_START {
	char packet_type = PACKET_CS_GAME_START;
}R_Start;

typedef struct Send_Packet_Players {
	char packet_type = PACKET_SC_PLAYERS;
	int Host;
	bool IsUsed[MAX_USER];
	float HP[MAX_USER];
	Location Loc[MAX_USER];
	Rotation Rot[MAX_USER];
	Velocity Vel[MAX_USER];
	AimOffset Aim[MAX_USER];
	bool IsJump[MAX_USER];
	bool isTargeting[MAX_USER];
	bool IsSprinting[MAX_USER];
	bool onCrouchToggle[MAX_USER];
	int WeaponState[MAX_USER];
	ViewPoint View[MAX_USER];
	int WeaponNum[MAX_USER];
	int Kit[MAX_USER];
	bool SirenButton;
	int ElapsedTime;
	//bool AllChanged;
}S_Players;

typedef struct Recv_Packet_Players {
	char packet_type = PACKET_CS_PLAYERS;
	float HP;
	Location Loc;
	Rotation Rot;
	Velocity Vel;
	AimOffset Aim;
	bool IsJump;
	bool isTargeting;
	bool IsSprinting;
	bool onCrouchToggle;
	int WeaponState;
	ViewPoint View;
	int WeaponNum;
	int Kit;
	bool SirenButton;
	int ElapsedTime;
	//bool change;
}R_Players;

typedef struct Recv_Packet_Stage_Change
{
	char packet_type = PACKET_CS_CHANGE;
	bool Changed;
}R_Change;

typedef struct Send_Packet_Stage_Change
{
	char packet_type = PACKET_SC_CHANGE;
	bool AllChanged;
}S_Change;

typedef struct Recv_Packet_Zombie
{
	char packet_type = PACKET_CS_ZOMBIE;
	bool IsAlive[MAX_ZOMBIE];
	int Target[MAX_ZOMBIE] = { -1 };
	float HP[MAX_ZOMBIE];
	Location Loc[MAX_ZOMBIE];
}R_Zombies;

typedef struct Send_Packet_Zombie
{
	char packet_type = PACKET_SC_ZOMBIE;
	bool IsAlive[MAX_ZOMBIE];
	int Target[MAX_ZOMBIE] = { -1 };
	float HP[MAX_ZOMBIE];
	Location Loc[MAX_ZOMBIE];
	bool HostZombie;
}S_Zombies;

#pragma pack(pop)