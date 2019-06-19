// Test_CLI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CLI.h"
#include <windows.h>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

int gfd_ocd = 0;
CRITICAL_SECTION	CriticalSection;

UINT32 loopflagbit = 0;

enum {
	eESC_send = 0x00000001,
	eESC_DageSend = 0x00000002
};



void hexDump(char *desc, void *addr, int len) {
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Output description if given.
	if (desc != NULL)
		printf("%s:\n", desc);

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf("  %s\n", buff);

			// Output the offset.
			printf("  %04x ", i);
		}

		// Now the hex code for the specific character.
		printf(" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf("  %s\n", buff);
}

unsigned char check_sum_256(char* buf, int size){
	unsigned char val = 0;
	int i = 0;
	for (i = 0; i < size; i++){
		val += buf[i];
	}
	printf("CHECKSUM[%02x]\n", val);
	return val;
}

#define SENDSIZE 1024*512

#pragma warning(disable: 4996)

DWORD WINAPI Thread_ReadFromOCD(void *arg)
{
	//	EscapeLoopUnSet_(eESC_send);
	SOCKET* pcli_fd;
	SOCKET cli_fd;
	pcli_fd = (SOCKET*)arg;
	cli_fd = *pcli_fd;
	int ret;
	char cmd[SENDSIZE];
	//	char testbuf[100] = { 0, };
	char* start = 0;
	char* end = 0;
	while (1)
	{
		cmd[0] = 0;
		
		ret = recv(gfd_ocd, cmd, SENDSIZE-1, 0);

		


		if (ret > 0){
			cmd[ret] = 0;
//						printf("OCD -> %s -> ECM\n", cmd);
			send(cli_fd, cmd, ret, 0);
		}
		else{
		//	printf("TIME OUT %d \n",ret);
		//	break;
		}

		if (strchr(cmd, '#')){		// �޼��� ���� ó�� -> OCD�κ��� ���̻� ���� �����Ͱ� ���� , ������ ���� 				
			//	printf("Escape\n");
			break;
		}
		
		Sleep(1);

	}
		printf("Kill Thread_ReadFromOCD (%08x) \n",cli_fd);
	return 0;

}
//#define CLI_CMD_LEN 		1024*100
DWORD WINAPI Thread_ReadFromECM(void *arg)
{

	SOCKET* pcli_fd;
	SOCKET cli_fd;
	pcli_fd = (SOCKET*)arg;
	cli_fd = *pcli_fd;
	int ret;
	char cmd[SENDSIZE];
	DWORD dwThreadID0_B;
	static int critical = 0;

	critical = 0;

	while (1)
	{
		cmd[0] = 0;


		ret = recv(cli_fd, cmd, SENDSIZE-1, 0);

		if (ret > 0){
			cmd[ret] = 0;

			if ((cmd[0] == 'E' && cmd[1] == 'X' && cmd[2] == 'I' && cmd[3] == 'T') || \
				(cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't')){								// ���� ���� For escape EX) exit EXIT			
				
				if (critical){
					critical = 0;
					printf("Critical %d \n", critical);
					LeaveCriticalSection(&CriticalSection);															// ����ñ��� critical�� ������� �ʾҴٸ� �������� 
				}
				closesocket(cli_fd);
				printf("Socket closed(%d)!\n", cli_fd);
				break;
			}
			if ((cmd[0] == 'N' && cmd[1] == 'R' && cmd[2] == 'E' && cmd[3] == 'T')){								// Socket�� �����ϸ鼭 Critical clear �ϱ����� ó��  			

				if (critical){
					critical = 0;
					printf("Critical %d \n", critical);
					LeaveCriticalSection(&CriticalSection);															// ����ñ��� critical�� ������� �ʾҴٸ� �������� 
				}
				continue;																							// RSP �޼����� �ƴϱ⶧����  ������ �ݵ�� continue �ؾ� ��. 
			
			}
			if ((strchr(cmd, '$')) && (strchr(cmd, '#')) && (strchr(cmd, '+'))){									// ECM���κ��� �����Ͱ� �ö� +$m~~~~~# ���� �پ�ö� ó��, Critical ������ �׳� pass  
				HANDLE CliThread_B = CreateThread(NULL, 0, Thread_ReadFromOCD, (LPVOID)&cli_fd, 0, &dwThreadID0_B);
				send(gfd_ocd, cmd, ret, 0);
				//				printf("OCD <- %s <- ECM\n", cmd);
			}
			else if (strchr(cmd, '+')){																			// �޼����� ������ - LeaceCritical
				send(gfd_ocd, cmd, ret, 0);
				//				printf("OCD <- %s <- ECM\n", cmd);
				LeaveCriticalSection(&CriticalSection);
				critical = 0;
				printf("Critical %d \n", critical);
			}
			else if (strchr(cmd, '$')){																			// �޼����� ó�� - EnterCritical
				EnterCriticalSection(&CriticalSection);
				critical = 1;
				printf("Critical %d \n", critical);
				HANDLE CliThread_B = CreateThread(NULL, 0, Thread_ReadFromOCD, (LPVOID)&cli_fd, 0, &dwThreadID0_B);
				send(gfd_ocd, cmd, ret, 0);
				//				printf("OCD <- %s <- ECM\n", cmd);
			}
			else{																									// �����Ͱ� �ɰ��� ���ð���� ó�� 
				send(gfd_ocd, cmd, ret, 0);
				//				printf("OCD <- %s <- ECM\n", cmd);
			}

		}

		
		Sleep(1);
	}

	return 0;

}



#define ECM_CONNECT_PORT 5557
int server_port = ECM_CONNECT_PORT;

DWORD WINAPI ThreadAFunc(void *arg)
{
	WSADATA wsa;
	SOCKET master, new_socket;
	struct sockaddr_in server, address;
	int addrlen;


	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Initialised.\n");

	//Create a socket
	if ((master = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(server_port);

	//Bind
	if (bind(master, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(master, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	addrlen = sizeof(struct sockaddr_in);

	while (TRUE)
	{

		if ((new_socket = accept(master, (struct sockaddr *)&address, (int *)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

		DWORD dwThreadID0_A;
		HANDLE CliThread_A = CreateThread(NULL, 0, Thread_ReadFromECM, (LPVOID)&new_socket, 0, &dwThreadID0_A);


		//	WaitForSingleObject(CliThread_A, INFINITE);
		//	WaitForSingleObject(CliThread_B, INFINITE);

	}

	closesocket(master);
	WSACleanup();

	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{

	char data[8] = { 0, 0 };
	
	if (!InitializeCriticalSectionAndSpinCount(&CriticalSection, 0x00000400)) return 0;

	gfd_ocd = NetCon("localhost", "3334");
	if (gfd_ocd == -1){
		printf("NetCon Error!\n");
		return 0;
	}
	if (getDataRSP_(gfd_ocd, 0xc0000000, 4, (void*)data) == -1){
		printf("getDataRSP_ Error!\n");
		return 0;
	}

	DWORD dwThreadID0;
	HANDLE CliThread = CreateThread(NULL, 0, ThreadAFunc, NULL, 0, &dwThreadID0);

	while (1){
		Sleep(1);
	}
	DeleteCriticalSection(&CriticalSection);
	return 0;
}

