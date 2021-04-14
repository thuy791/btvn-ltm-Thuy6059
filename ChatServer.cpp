#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#pragma comment(lib, "ws2_32")

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);

SOCKET clients[64];
int numClients;

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
		while (true) {
			const char* name_asking = "What is your name?";
			send(client, name_asking, strlen(name_asking), 0);
			char buf[256];
			int ret = recv(client, buf, sizeof(buf), 0);
			int right_form = 1;
			// Check form... Need help
			if (right_form) break;
		}
		clients[numClients] = client;
		numClients++;
		
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
	closesocket(listener);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET*)lpParam;
	char buf[256], sender[512];
	int ret;
	time_t rawtime;
	struct tm* timeinfo;
	while (true) {
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0) {
			RemoveClient(client);
			break;
		}

		buf[ret] = 0;
		printf("%d: %s\n", client, buf);
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		_itoa_s(client, sender, 10);
		strcat_s(sender, ": ");
		strcat_s(sender, buf);
		strcat_s(sender, " ");
		strcat_s(sender, asctime(timeinfo));
		for (int i = 0; i < numClients; i++) {
			if (clients[i] != client) {
				send(clients[i], sender, strlen(sender), 0);
			}
		}
		
	}
	closesocket(client);
}

void RemoveClient(SOCKET client) {
	int i = 0;
	while (clients[i] != client) i++;
	if (i < numClients - 1) clients[i] = clients[numClients - 1];
	numClients--;
}