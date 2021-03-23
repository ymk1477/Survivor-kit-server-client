#include <iostream>
#include "server.h"

using namespace std;

SOCKET ListenSock{};
HANDLE IocpHandle{};
int StartId{ 0 };

Client Clients[4]{};

int Players = 0;
bool GameStart;
Player Player_info;
Zombie Zombie_info;
bool Changed[MAX_USER];
bool AllChanged = false;
bool HostZombie = false;

int Get_Packet_Size(char* type)
{
	int size = 0;

	switch (type[0])
	{
	case PACKET_TEST:
	{
		size = sizeof(TEST);
	}
	break;
	case PACKET_CS_LOGIN:
	{
		size = sizeof(R_Login);
	}break;

	case PACKET_SC_LOGIN:
	{
		size = sizeof(S_Login);
	}
	break;
	case PACKET_CS_GAME_START:
	{
		size = sizeof(R_Start);
	}break;
	case PACKET_SC_GAME_START:
	{
		size = sizeof(S_Start);
	}
	break;
	case PACKET_CS_PLAYERS:
	{
		size = sizeof(R_Players);
	}break;
	case PACKET_SC_PLAYERS:
	{
		size = sizeof(S_Players);
	}
	break;
	case PACKET_CS_CHANGE:
	{
		size = sizeof(R_Change);
	}
	break;
	case PACKET_SC_CHANGE:
	{
		size = sizeof(S_Change);
	}
	break;
	case PACKET_CS_ZOMBIE:
	{
		size = sizeof(R_Zombies);
	}
	break;
	case PACKET_SC_ZOMBIE:
	{
		size = sizeof(S_Zombies);
	}
	break;
	}

	return size;
}

void ZeroPacketBuff(int UserId)
{
	ZeroMemory(Clients[UserId].packetBuffer, MAX_PACKET_SIZE);
	Clients[UserId].prevSize = 0;
}

void SendPacket(int UserId, void* Data)
{
	char* buf{ reinterpret_cast<char*>(Data) };
	ExOverlapped* exOver{ new ExOverlapped{} };

	exOver->Operation = EOperation::E_SEND;
	exOver->WsaBuf.buf = exOver->IoBuffer;
	exOver->WsaBuf.len = Get_Packet_Size(buf);
	memcpy(exOver->IoBuffer, buf, exOver->WsaBuf.len);

	WSASend(Clients[UserId].socket, &exOver->WsaBuf, 1, NULL, 0, &exOver->Overlapped, NULL);
}

void SendLoginPacket(int UserId)
{
	S_Login Login{};
	Login.packet_type = PACKET_SC_LOGIN;
	Login.clientId = UserId;
	for (int i = 0; i < MAX_USER; ++i)
	{
		Login.Player[i] = Player_info.IsUsed[i];
	}
	Login.Host = Player_info.Host;

	SendPacket(UserId, &Login);
}

void SendPacketStart(int UserId)
{
	S_Start Start{};
	Start.packet_type = PACKET_SC_GAME_START;
	Start.Started = GameStart;
	
	SendPacket(UserId, &Start);
}

void SendPacketPlayer(int UserId)
{
	S_Players s_packet{};
	for (int i = 0; i < MAX_USER; ++i) {
		s_packet.Host = Player_info.Host;
		s_packet.IsUsed[i] = Player_info.IsUsed[i];
		s_packet.HP[i] = Player_info.HP[i];
		s_packet.Loc[i] = Player_info.Loc[i];
		s_packet.Rot[i] = Player_info.Rot[i];
		s_packet.Vel[i] = Player_info.Vel[i];
		s_packet.Aim[i] = Player_info.Aim[i];
		s_packet.IsJump[i] = Player_info.IsJump[i];
		s_packet.isTargeting[i] = Player_info.isTargeting[i];
		s_packet.IsSprinting[i] = Player_info.IsSprinting[i];
		s_packet.onCrouchToggle[i] = Player_info.onCrouchToggle[i];
		s_packet.WeaponState[i] = Player_info.WeaponState[i];
		s_packet.View[i] = Player_info.View[i];
		s_packet.WeaponNum[i] = Player_info.WeaponNum[i];
		s_packet.Kit[i] = Player_info.Kit[i];
		s_packet.SirenButton = Player_info.SirenButton;
		s_packet.ElapsedTime = Player_info.ElapsedTime;
	}

	SendPacket(UserId, &s_packet);
}

void SendPacketChange(int UserId)
{
	S_Change s_packet{};
	s_packet.AllChanged = AllChanged;
	
	SendPacket(UserId, &s_packet);

}

void SendPacketZombie(int UserId)
{
	S_Zombies s_packet;
	for (int i = 0; i < MAX_ZOMBIE; ++i)
	{
		s_packet.IsAlive[i] = Zombie_info.IsAlive[i];
		s_packet.Target[i] = Zombie_info.Target[i];
		s_packet.HP[i] = Zombie_info.HP[i];
		s_packet.Loc[i] = Zombie_info.Loc[i];
	}
	s_packet.HostZombie = HostZombie;

	SendPacket(UserId, &s_packet);
}

void ProcessPacket(int UserId, char* Buf)
{
	int Type{ static_cast<unsigned int>(Buf[0]) };
	switch (Type)	// Type
	{
	case PACKET_TEST:
	{
		cout << UserId << "번 플레이어 PACKET_TEST !! " << endl;
	}
	break;
	case PACKET_CS_LOGIN:
	{
		cout << UserId << "번 플레이어 Login! " << endl;
		if (Player_info.Host == -1)
			Player_info.Host = UserId;
		cout << "Host : " << Player_info.Host << endl;

		Player_info.IsUsed[UserId] = true;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (Clients[i].isUsed)
			{
				SendLoginPacket(i);
			}
		}
	}
	break;
	case PACKET_CS_GAME_START:
	{
		cout << "호스트가 게임 시작함" << endl;
		GameStart = true;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (Clients[i].isUsed)
			{
				SendPacketStart(i);
			}
		}
	}
	break;
	case PACKET_CS_PLAYERS:
	{
		R_Players* packet = reinterpret_cast<R_Players*>(Buf);

		Player_info.HP[UserId] = packet->HP;
		Player_info.Loc[UserId] = packet->Loc;
		Player_info.Rot[UserId] = packet->Rot;
		Player_info.Vel[UserId] = packet->Vel;
		Player_info.Aim[UserId] = packet->Aim;
		Player_info.IsJump[UserId] = packet->IsJump;
		Player_info.isTargeting[UserId] = packet->isTargeting;
		Player_info.IsSprinting[UserId] = packet->IsSprinting;
		Player_info.onCrouchToggle[UserId] = packet->onCrouchToggle;
		Player_info.WeaponState[UserId] = packet->WeaponState;
		Player_info.View[UserId] = packet->View;
		Player_info.WeaponNum[UserId] = packet->WeaponNum;
		Player_info.Kit[UserId] = packet->Kit;

		if (!(Player_info.SirenButton) && packet->SirenButton)
			Player_info.SirenButton = packet->SirenButton;
		if (UserId == Player_info.Host)
		{
			Player_info.ElapsedTime = packet->ElapsedTime;
		}
		//Changed[UserId] = packet->change;

		//int ChangedPlayer = 0;
		//int UsingPlayer = 0;

		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	if (Clients[i].isUsed)
		//	{
		//		UsingPlayer++;
		//		if (Changed[i])
		//			ChangedPlayer++;
		//	}
		//}
		//cout << "UsingPlayer : " << UsingPlayer << endl;
		//cout << "ChangedPlayer : " << ChangedPlayer << endl;
		//if (ChangedPlayer == UsingPlayer)
		//	AllChanged = true;
		//else
		//	AllChanged = false;

		cout << UserId << " 플레이어 PROCESS !! " << endl;

		for (int i = 0; i < MAX_USER; ++i)
		{
			SendPacketPlayer(i);
		}
	}	
	break;
	case PACKET_CS_CHANGE:
	{
		R_Change* packet = reinterpret_cast<R_Change*>(Buf);
		Changed[UserId] = packet->Changed;
		if (packet->Changed)
		{
			int UsingPlayer = 0;
			int ChangedPlayer = 0;
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (Clients[i].isUsed)
				{
					UsingPlayer++;
					if (Changed[i])
					{
						ChangedPlayer++;
					}
				}
			}
			if (UsingPlayer == ChangedPlayer)
			{
				AllChanged = true;
			}
			else
			{
				AllChanged = false;
			}

			cout << UserId << " 플레이어 맵 변경 완료 !! " << endl;

			SendPacketChange(UserId);

		}
		else 
		{
			cout << UserId << " 플레이어 맵 변경 시작 !! " << endl;
			HostZombie = false;
			/*for (int i = 0; i < MAX_USER; ++i)
			{
				Changed[i] = false;
			}*/
		}
	}
	break;
	case PACKET_CS_ZOMBIE:
	{
		R_Zombies* packet = reinterpret_cast<R_Zombies*>(Buf);
		if (UserId == Player_info.Host)
		{
			for (int i = 0; i < MAX_ZOMBIE; ++i)
			{
			   Zombie_info.IsAlive[i] = packet->IsAlive[i];
			   Zombie_info.Target[i] = packet->Target[i];
			   Zombie_info.HP[i] = packet->HP[i];
			   Zombie_info.Loc[i] = packet->Loc[i];
			}
			HostZombie = true;
		}
		cout << UserId << " 플레이어 좀비 정보 갱신 !! " << endl;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (Clients[i].isUsed)
			{
				if (Changed[i])
					SendPacketZombie(i);
			}
		}
	}
	break;
	}
	
}

void RecvConstruct(int UserId, int IoByte)
{
	Client& user{ Clients[UserId] };
	ExOverlapped& RecvOver{ Clients[UserId].RecvOverlap };
	int restByte{ IoByte };
	char* currentBuffer{ RecvOver.IoBuffer };
	int packetSize{ 0 };

	if (user.prevSize != 0)
		packetSize = Get_Packet_Size(user.packetBuffer);

	while (restByte > 0) {
		if (packetSize == 0)
			packetSize = Get_Packet_Size(currentBuffer);
		if (packetSize <= restByte + user.prevSize) {
			memcpy(user.packetBuffer + user.prevSize, currentBuffer, packetSize - user.prevSize);
			currentBuffer += packetSize - user.prevSize;
			restByte -= packetSize - user.prevSize;
			packetSize = 0;
			user.prevSize = 0;
			ProcessPacket(UserId, user.packetBuffer);
		}
		else {
			memcpy(user.packetBuffer + user.prevSize, currentBuffer, restByte);
			user.prevSize += restByte;
			restByte = 0;
			currentBuffer += restByte;
		}
	}
}



void Work()
{
	while (true)
	{
		DWORD IoByte{ };
		ULONG_PTR Key{};
		WSAOVERLAPPED* Over{};

		bool retval;
		retval = GetQueuedCompletionStatus(IocpHandle, &IoByte, &Key, &Over, INFINITE);

		if (!retval)
		{
			cout << "G Error!! : " << (int)WSAGetLastError << endl;
		}

		ExOverlapped* ExOver{ reinterpret_cast<ExOverlapped*>(Over) };
		int CurrentId{ static_cast<int>(Key) };
		switch(ExOver->Operation)
		{
			case E_ACCEPT:
			{
				SOCKET ClientSock = ExOver->ClientSocket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(ClientSock), IocpHandle, StartId, 0);
				Clients[StartId].prevSize = 0;
				Clients[StartId].RecvOverlap.Operation = E_RECV;
				Clients[StartId].RecvOverlap.WsaBuf.buf = Clients[StartId].RecvOverlap.IoBuffer;
				Clients[StartId].RecvOverlap.WsaBuf.len = MAX_BUFFER_SIZE;
				Clients[StartId].socket = ClientSock;
				Clients[StartId].isUsed = true;
				Clients[StartId].id = StartId;

				cout << StartId << "번 클라이언트 접속" << endl;
				Players++;
				cout << "Player : " << Players << endl;


				DWORD flags{ 0 };
				WSARecv(ClientSock, &Clients[StartId].RecvOverlap.WsaBuf, 1, NULL,
					&flags, &Clients[StartId].RecvOverlap.Overlapped, NULL);

				ClientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
				ExOver->ClientSocket = ClientSock;
				ZeroMemory(&ExOver->Overlapped, sizeof(ExOver->Overlapped));
				AcceptEx(ListenSock, ClientSock, ExOver->IoBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
					NULL, &ExOver->Overlapped);
				++StartId;
			}
			break;
			case E_RECV:
			{
				if (IoByte == 0)
				{
					cout << "E_RECV Error - ErrorCode : " << (int)WSAGetLastError() << endl;
					closesocket(ExOver->ClientSocket);
					cout << CurrentId << " 플레이어 종료" << endl;
				}
				else
				{
					RecvConstruct(CurrentId, IoByte);
					DWORD flags{ 0 };
					ZeroMemory(&Clients[CurrentId].RecvOverlap.Overlapped, sizeof(Clients[CurrentId].RecvOverlap.Overlapped));

					WSARecv(Clients[CurrentId].socket, &Clients[CurrentId].RecvOverlap.WsaBuf, 1, NULL,
						&flags, &Clients[CurrentId].RecvOverlap.Overlapped, NULL);
				}
			}
			break;
			case E_SEND:
			{
				if (IoByte == 0)
				{
					cout << "E_SEND Error - ErrorCode : " << (int)WSAGetLastError() << endl;
					closesocket(ExOver->ClientSocket);
				}
				delete ExOver;
			}
			break;
		}
	}
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	ListenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(ListenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

	listen(ListenSock, SOMAXCONN);

	IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(ListenSock), IocpHandle, 99999, 0);

	SOCKET ClientSock{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED) };
	ExOverlapped Accept{};
	Accept.Operation = E_ACCEPT;
	Accept.ClientSocket = ClientSock;

	AcceptEx(ListenSock, ClientSock, Accept.IoBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, &Accept.Overlapped);

	Work();
}