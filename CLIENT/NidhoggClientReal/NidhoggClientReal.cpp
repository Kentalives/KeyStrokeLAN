// NidhoggClientReal.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#define BOOLEANARRAYSIZE 128

SOCKET socketCl;
int vkCodeSize = sizeof(KBDLLHOOKSTRUCT::vkCode);
boolean releasedArray[BOOLEANARRAYSIZE];
struct pulseInfo
{
	DWORD vkCode;
	WPARAM pulse;
};

LRESULT CALLBACK LowLevelKeyboardProc(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
if((nCode) < 0)
{
	return CallNextHookEx(0,nCode,wParam,lParam);
}

if (!nCode) //Keyboard event
{
	KBDLLHOOKSTRUCT kbStruct = *(KBDLLHOOKSTRUCT*) lParam;

	pulseInfo myInfo;
	int indexArray = kbStruct.vkCode;
	myInfo.vkCode = kbStruct.vkCode;
	myInfo.pulse = wParam;
	switch (wParam) 
	{
	case WM_KEYDOWN:
		if (releasedArray[indexArray])
		{
			releasedArray[indexArray] = 0;
			send(socketCl, (char*)&myInfo, sizeof(myInfo), 0);
		}
		break;

	case WM_KEYUP:
		releasedArray[indexArray] = 1;
		send(socketCl, (char*)&myInfo, sizeof(myInfo), 0);
		break;
	
	}

	

		
		return 1;

}

}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Uso: ./Client.exe <ip> <port>";
		return 0;
	}
	for (int i = 0; i < BOOLEANARRAYSIZE; ++i)
	{
		releasedArray[i] = 1;
	}
	WSAData wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequired, &wsaData))
	{
		std::cerr << "Error inicializando WSA";
		return 0;
	}
	socketCl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addr_serv;
	char* IP_text = argv[1];

	uint16_t port;
	sscanf_s(argv[2], "%hu", &port);

	addr_serv.sin_port = htons(port);
	if (inet_pton(AF_INET, IP_text, &(addr_serv.sin_addr)) < 1)
	{
		perror("Error codificando IP a addr_serv");
		std::cout << WSAGetLastError();
		exit(EXIT_FAILURE);
	}
	addr_serv.sin_family = AF_INET;

	if ((connect(socketCl, (struct sockaddr*)&addr_serv, sizeof(addr_serv))) == SOCKET_ERROR)
	{
		perror("Error creando conexión con el servidor");
		std::cout << WSAGetLastError();
		exit(EXIT_FAILURE);
	}


	#define tam 30
	char buffer[tam] = "\0";
	std::cout << "Cargando...\n";

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	std::cout << "Conexion correcta\n";
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	UnhookWindowsHookEx(hook);

	
	
	closesocket(socketCl);



	return 0;



}

