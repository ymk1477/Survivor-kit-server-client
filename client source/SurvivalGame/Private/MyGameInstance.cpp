// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "MyGameInstance.h"
#include "Player_Manager.h"

static FSocket* inst = nullptr;

char largebuff[500'000];
char *buff = largebuff;
char *packet_start = largebuff;
int packet_size;
int R_Data = 0;

FSocket& MySocket::getInstance() {
	return *inst;
}

void MySocket::initializeServer()
{
	inst = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("My_Server"), false);

	inst->SetNonBlocking();
	inst->SetRecvErr();

	//FString ipaddress = TEXT("127.0.0.1");

	//FIPv4Address ip(127, 0, 0, 1);
	FIPv4Address ip;
	FIPv4Address::Parse(Adress, ip);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	addr->SetIp(ip.Value);
	addr->SetPort(SERVERPORT);

	bool connected = inst->Connect(*addr);

	if (!connected) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fail to Connect!! ")));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Success to Connect!! ")));
		Connected = true;

	}
	
}

void MySocket::sendBuffer(int PacketType, void* BUF) {

	int32 zero = 0;
	bool Success;
	int32 ByteSent;

	if (ESocketConnectionState::SCS_Connected == inst->GetConnectionState())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("SocketState : SCS_Connected")));

		switch (PacketType)
		{
		case PACKET_TEST:
		{
			Success = inst->Send((uint8*)BUF, sizeof(TESTING), ByteSent);
			if (Success)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Test SIZE : %d"), ByteSent));
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Test Send :")));
			}
		}
		break;
		case PACKET_CS_LOGIN:
		{
			Success = inst->Send((uint8*)BUF, sizeof(S_Login), zero);
			ESocketErrors LastError = ISocketSubsystem::Get()->GetLastErrorCode();
			const TCHAR* err = ISocketSubsystem::Get()->GetSocketError(LastError);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, err);
		}
		break;
		case PACKET_CS_GAME_START:
		{

			Success = inst->Send((uint8*)BUF, sizeof(S_Start), ByteSent);
			if (Success)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("S_Start Send! SIZE : %d"), ByteSent));
			}
			ESocketErrors LastError = ISocketSubsystem::Get()->GetLastErrorCode();
			const TCHAR* err = ISocketSubsystem::Get()->GetSocketError(LastError);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, err);
		}
		break;
		case PACKET_CS_PLAYERS:
		{
			Success = inst->Send((uint8*)BUF, sizeof(S_Players), ByteSent);
			if (Success)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SUCCESS to Send S_PLAYERS !! SIZE : %d"), ByteSent));
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fail to Send S_PLAYERS !!")));
			}
		}
		break;
		case PACKET_CS_CHANGE:
		{
			Success = inst->Send((uint8*)BUF, sizeof(S_Change), ByteSent);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SUCCESS to Send S_CHANGE !! SIZE : %d"), ByteSent));
		}
		break;
		case PACKET_CS_ZOMBIE :
		{
			Success = inst->Send((uint8*)BUF, sizeof(S_Zombies), zero);
		}
		break;
		case PACKET_CS_ITEM:
		{
			Success = inst->Send((uint8*)BUF, sizeof(S_Item), zero);
		}
		break;
		}

		delete BUF;
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SCS_ConnectionError")));
		MySocket::CloseSocket();
	}
}

void MySocket::RecvPacket() {

	/*uint8* RECV_BUF = new uint8[MAX_BUFFER];
	R_Test* p_tmp;
	int32 zero = 0;*/

	int32 ByteRead;

	bool Success = inst->Recv((uint8*)buff, (int32)(sizeof(largebuff) - (buff - largebuff)), ByteRead);
	   
	if (!Success)
	{
		if (ESocketConnectionState::SCS_Connected == inst->GetConnectionState())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("SocketState : SCS_Connected")));
		}
		else
		{
			MySocket::CloseSocket();
		}
	}
	else
	{
		
		//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Success to Recv !! : %d Bytes "), ByteRead));
		buff += ByteRead;
		R_Data += ByteRead;
		packet_size = Get_Packet_Size(packet_start);
		//GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Black, FString::Printf(TEXT("PACKET SIZE : %d Bytes "), packet_size));
		while (true) {

			if (packet_size <= (buff - packet_start)) {

				Process_Packet();

				packet_start += packet_size;

				if (packet_start < buff)
					packet_size = Get_Packet_Size(packet_start);
				else
					break;

			}
			else
				break;
		}

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("R_DATA : %d Bytes "), R_Data));
		if (R_Data > 300'000)
		{
			char tmpbuffer[200'000] = { NULL };
			memcpy(tmpbuffer, packet_start, buff - packet_start);
			memset(largebuff, 0x00, buff - largebuff);
			memcpy(largebuff, tmpbuffer, sizeof(*tmpbuffer));
			buff = largebuff;
			packet_start = largebuff;
			packet_size = 0;
			R_Data = 0;
		}
			
	}
}

int MySocket::Get_Packet_Size(char* type)
{

	int size = 0;

	switch (type[0])
	{
	case PACKET_CS_LOGIN:
	{
		size = sizeof(S_Login);
	}break;

	case PACKET_SC_LOGIN:
	{
		size = sizeof(R_Login);
	}
	break;
	case PACKET_CS_GAME_START:
	{
		size = sizeof(S_Start);
	}break;
	case PACKET_SC_GAME_START:
	{
		size = sizeof(R_Start);
	}
	break;
	case PACKET_CS_PLAYERS:
	{
		size = sizeof(S_Players);
	}break;
	case PACKET_SC_PLAYERS:
	{
		size = sizeof(R_Players);
	}
	break;
	case PACKET_CS_ZOMBIE:
	{
		size = sizeof(S_Zombies);
	}
	break;
	case PACKET_SC_ZOMBIE:
	{
		size = sizeof(R_Zombies);
	}
	break;
	case PACKET_CS_CHANGE:
	{
		size = sizeof(S_Change);
	}
	break;
	case PACKET_SC_CHANGE:
	{
		size = sizeof(R_Change);
	}
	break;
	case PACKET_CS_ITEM:
	{
		size = sizeof(S_Item);
	}
	break;
	case PACKET_SC_ITEM:
	{
		size = sizeof(R_Item);
	}
	break;
	}
	return size;
}

void MySocket::Process_Packet()
{

	switch (packet_start[0])
	{
	case PACKET_SC_LOGIN:
	{
		Playing = 0;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Success to Recv LOGIN!! ")));
		R_Login* packet = reinterpret_cast<R_Login*>(packet_start);
		if (PlayerId == -1)
			PlayerId = packet->clientId;
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("PlayerId : %d "),
			PlayerId));*/
		if (HostPlayer == -1) {
			HostPlayer = packet->Host;
			Player_info.Host = HostPlayer;
		}
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Host Player : %d "),
			HostPlayer));*/
		for (int i = 0; i < MAX_USER; ++i) {
			Player_info.IsUsed[i] = packet->Player[i];
			if (Player_info.IsUsed[i])
				Playing++;
			/*	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%d Player Login : %d "),
					i + 1, packet->Player[i]));*/
		}

	}
	break;
	case PACKET_SC_GAME_START:
	{
		R_Start* packet = reinterpret_cast<R_Start*>(packet_start);
		GameStart = packet->Started;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Started : %d "), GameStart));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("GAME START! ")));
	}
	break;
	case PACKET_SC_PLAYERS:
	{
		R_Players* packet = reinterpret_cast<R_Players*>(packet_start);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Players RECV! ")));
		for (int i = 0; i < MAX_USER; ++i)
		{
			Player_info.Host = packet->Host;
			Player_info.IsUsed[i] = packet->IsUsed[i];
			if ((i != PlayerId) && Player_info.IsUsed[i])
			{
				Player_info.HP[i] = packet->HP[i];
				Player_info.Loc[i] = packet->Loc[i];
				Player_info.Rot[i] = packet->Rot[i];
				Player_info.Vel[i] = packet->Vel[i];
				Player_info.Aim[i] = packet->Aim[i];
				Player_info.IsJump[i] = packet->IsJump[i];
				Player_info.IsTargeting[i] = packet->IsTargeting[i];
				Player_info.IsSprinting[i] = packet->IsSprinting[i];
				Player_info.onCrouchToggle[i] = packet->onCrouchToggle[i];
				Player_info.WeaponState[i] = packet->WeaponState[i];
				Player_info.View[i] = packet->View[i];
				Player_info.WeaponNum[i] = packet->WeaponNum[i];
				Player_info.Kit[i] = packet->Kit[i];
			}

		}
		if (!Player_info.SirenButton && packet->SirenButton)
			Player_info.SirenButton = packet->SirenButton;
		if (Player_info.Host != PlayerId)
		{
			Player_info.ElapsedTime = packet->ElapsedTime;
		}
		//AllChanged = packet->AllChanged;

	}
	break;
	case PACKET_SC_ZOMBIE:
	{
		R_Zombies* packet = reinterpret_cast<R_Zombies*>(packet_start);
		if (PlayerId != HostPlayer)
		{
			if (packet->HostZombieInfo)
			{
				HostZombie = packet->HostZombieInfo;
				for (int i = 0; i < MAX_ZOMBIE; ++i)
				{
					Zombie_info.IsAlive[i] = packet->IsAlive[i];
					if (packet->IsAlive[i])
					{
						if (packet->Target[i] < -1 || packet->Target[i] > (Playing - 1))
							Zombie_info.Target[i] = -1;
						else
							Zombie_info.Target[i] = packet->Target[i];
						Zombie_info.HP[i] = packet->HP[i];
						Zombie_info.Loc[i] = packet->Loc[i];
					}
				}
			}
		}
		else
		{
			if (packet->HostZombieInfo)
			{
				HostZombie = packet->HostZombieInfo;
				for(int i = 0 ; i < MAX_ZOMBIE; ++i)
					Zombie_info.IsAlive[i] = packet->IsAlive[i];
			}
		}
	}
	break;
	case PACKET_SC_CHANGE:
	{
		R_Change* packet = reinterpret_cast<R_Change*>(packet_start);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("PACKET->ALLCHANGED : %d"), packet->AllChanged));
		AllChanged = packet->AllChanged;
	}
	break;
	case PACKET_SC_ITEM:
	{
		R_Item* packet = reinterpret_cast<R_Item*>(packet_start);
		for (int i = 0; i < MAX_ITEM; ++i)
		{
			if(packet->IsUsed[i])
				Item_info.IsUsed[i] = packet->IsUsed[i];
		}
	}
	break;
	}
}

void MySocket::CloseSocket()
{
	inst->Close();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Close the Socket")));
}

UMyGameInstance::UMyGameInstance()
{
	PlayerId = -1;
	HostPlayer = -1;
	Connected = false;
	GameStart = false;
	PlayerLogin = false;
	All_level_Changed = false;
	SirenPushed = false;
	Changed = false;
	StageChange = false;
	HostZombie = false;
	Stage = 0;
}


void UMyGameInstance::SetIpAdress(FString Ip_Adress)
{
	Adress = Ip_Adress;
}

bool UMyGameInstance::ConnectServer()
{
	MySocket::initializeServer();
	if (Connected)
		return true;
	else
		return false;
}

bool UMyGameInstance::IsConected()
{
	if (Connected)
		return true;
	else
		return false;
}

int UMyGameInstance::GetPlayer()
{
	return Playing;
}

bool UMyGameInstance::IsHost()
{
	if (PlayerId == HostPlayer)
		return true;
	else
		return false;
}

bool UMyGameInstance::IsStart()
{
	if (GameStart)
		return true;
	else
		return false;
}

void UMyGameInstance::Start()
{
	S_Start* start = new S_Start;
	MySocket::sendBuffer(PACKET_CS_GAME_START, &(*start));
}

bool UMyGameInstance::IsAllLevelChange()
{
	return All_level_Changed;
}
void UMyGameInstance::FinishGame()
{
	MySocket::CloseSocket();
	PlayerId = -1;
	HostPlayer = -1;
	Connected = false;
	GameStart = false;
	PlayerLogin = false;
	SirenPushed = false;
	Changed = false;
	StageChange = false;
	HostZombie = false;
	Stage = 0;
}

void UMyGameInstance::SirenPush()
{
	SirenPushed = true;
}

bool UMyGameInstance::IsSirenPushed()
{
	return SirenPushed;
}

void UMyGameInstance::SetChange(bool Change)
{
	Changed = Change;
}

bool UMyGameInstance::GetChange()
{
	return Changed;
}

void UMyGameInstance::SendChange()
{
	S_Change* change = new S_Change;
	change->Changed = Changed;
	MySocket::sendBuffer(PACKET_CS_CHANGE, &(*change));
	HostZombie = false;
	
}

void UMyGameInstance::ZeroBuff()
{
	memset(largebuff, 0x00, sizeof(*largebuff));
	buff = largebuff;
	packet_start = largebuff;
	packet_size = 0;
	R_Data = 0;
	
}

void UMyGameInstance::SetAllChange(bool val)
{
	AllChanged = val;
}

UMyGameInstance::~UMyGameInstance()
{
	if (Connected)
		inst->Close();
}

