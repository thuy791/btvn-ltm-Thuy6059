#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);

SOCKET clients[64];
int numClients;
CRITICAL_SECTION CriticalSection;
FILE* db;
errno_t err;
int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	numClients = 0;

	while (true) {
		SOCKET client = accept(listener, NULL, NULL);
		printf("New client accepted: %d\n", client);
		clients[numClients] = client;
		numClients++;
		InitializeCriticalSection(&CriticalSection);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
		DeleteCriticalSection(&CriticalSection);
	}

	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	EnterCriticalSection(&CriticalSection);
    SOCKET client = *(SOCKET*)lpParam;
	const char* loginRequest = "Enter your username and password!";
	send(client, loginRequest, strlen(loginRequest), 0);

	err = fopen_s(&db, "C:\\Users\\NamNgo\\Downloads\\20202\\NetworkProgramming\\db.txt", "r");
	
    char buf[256], line[100];
    int ret, exist = 0;
	ret = recv(client, buf, sizeof(buf), 0);

	while (fgets(line, sizeof(line), db)) {
		if (strcmp(line, buf)) {
			exist = 1;
			break;
		}
	}

	if (!exist) {
		printf("User does not exist!");
		return -1;
	}
	
    while (1) {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            RemoveClient(client);
            break;
        }
		strcat_s(buf, " > C:\\Users\\NamNgo\\Downloads\\20202\\NetworkProgramming\\out.txt");
		system(buf);
    }
    closesocket(client);
	LeaveCriticalSection(&CriticalSection);
	return 1;
}

void RemoveClient(SOCKET client) {
    int i = 0;
    while (clients[i] != client) i++;
    if (i < numClients - 1) clients[i] = clients[numClients - 1];

    numClients--;
}