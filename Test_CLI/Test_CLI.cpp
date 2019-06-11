// Test_CLI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CLI.h"

#include <windows.h>
#include <time.h>
#include "flash.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

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


#pragma warning(disable: 4996)


#define CLI_CMD_LEN 		256
DWORD WINAPI Thread_Read(void *arg)
{

	SOCKET* pcli_fd;
	SOCKET cli_fd;
	pcli_fd = (SOCKET*)arg;
	cli_fd = *pcli_fd;
	int ret;
	char cmd[CLI_CMD_LEN + 10];
	char testbuf[100] = { 0, };
	while (1)
	{
		Sleep(10);
		ret = recv(cli_fd, cmd, CLI_CMD_LEN, 0);

		if ((cmd[0] == 'E' && cmd[1] == 'X' && cmd[2] == 'I' && cmd[3] == 'T') || \
			(cmd[0] == 'e' && cmd[1] == 'x' && cmd[2] == 'i' && cmd[3] == 't')){		// For escape terminal	EX) exit EXIT				
			closesocket(cli_fd);
			printf("Socket closed(%d)!\n", cli_fd);
			break;
		}

		if (ret > 2){
	
		}

	}

	return 0;

}

#define ECM_CONNECT_PORT 5556
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


		DWORD dwThreadID0;
		HANDLE CliThread = CreateThread(NULL, 0, Thread_Read, (LPVOID)&new_socket, 0, &dwThreadID0);

	}

	closesocket(master);
	WSACleanup();

	return TRUE;
}

#define SEND_SIZE 0x1000000
#define ROUND_TEST 0x100000

int _tmain(int argc, _TCHAR* argv[])
{

	int fd = 0;
	int fd_data = 0;
	int cnt = 1;
	double result = 0;
	int i = 0;
	unsigned int n = 0;
	int k = 0;
	int len = 0;
	unsigned char* val = (unsigned char*)malloc(SEND_SIZE);
	unsigned char* val_2 = (unsigned char*)malloc(SEND_SIZE);
	
	
	BYTE checksumval = 0;
	clock_t before;

	for (i = 0; i < cnt; i++){
		printf("ROUND:%d\n", i);

		fd_data = NetCon("localhost", "5557");
		if (fd_data == -1){
			printf("NetCon fail \n");
			return 0;
		}

		for (n = 0; n < ROUND_TEST; n++){
			val[n] = n;
		}
	//	getDataRSP_(fd_data, 0xa0000000, 1, (void*)val_2);
	//	Sleep(1000);
	//	getDataRSP_(fd_data, 0xa0000000, 125, (void*)val_2);
	//	Sleep(1000);
	//	getDataRSP_(fd_data, 0xa0000000, 125, (void*)val_2);
	//	setDataRSP_(fd_data, 0xa0000000,8, (void*)val);
		/*
		for (n = 0; n < ROUND_TEST; n+=10000){
			before = clock();
			setDataRSP_(fd_data, 0x82000000, n, (void*)val);
			result = (double)(clock() - before) / CLOCKS_PER_SEC;
			printf("WR>ROUND(%08x) TIME : %f , Byte per Second %f \n", n, result, (double)(n) / result);
		}
		*/
		

		
	
		
	
		
	//	sendMsg(fd_data, "EXIT");
	//	while (1){
	//		Sleep(1000);
	//	}
		
#if 0
		before = clock();

		for (n = 0; n < 256; n++){
			
			getDataRSP_(fd_data, 0xa0000000+n, 1, (void*)val_2);
			if (val_2[0] != n){
				printf("Error Data is not same! val_2[0] %d  %d \n", val_2[0], n);
				while (1);
			}
		}
		
		result = (double)(clock() - before) / CLOCKS_PER_SEC;
		printf("TIME : %f\n", result);
#else
		
		
	
	//	sfls_sect_erase(fd_data,0);
		continueRSP_(fd_data);
		Sleep(100);
		sendMsg(fd_data, "NRET");

		unsigned int err_cnt = 0;

		for (n = 0; n < ROUND_TEST; n += 0x1000){

			resetRSP_(fd_data);
			Sleep(100);
			sendMsg(fd_data, "NRET");

			sfls_init(fd_data);

			sfls_sect_erase(fd_data, n);
			Sleep(100);
			before = clock();

			hexDump("READ", val, 0x10);

			if (setDataRSP_(fd_data, 0xc0000000+n, 0x1000, (void*)val) == -1){
				printf("setDataRSP_ fail \n");

			}
			result = (double)(clock() - before) / CLOCKS_PER_SEC;
			printf("Write - ROUND(%08x) TIME : %f , Byte per Second %f \n", n, result, (double)(n) / result);

			before = clock();
			if (getDataRSP_(fd_data, 0xc0000000+n, 0x1000, (void*)val_2) == -1){
				printf("getDataRSP_ fail \n");
			}
			hexDump("READ", val_2, 0x10);

			result = (double)(clock() - before) / CLOCKS_PER_SEC;
			printf("READ - ROUND(%08x) TIME : %f , Byte per Second %f \n",n ,result, (double)(n) / result);
			if (memcmp(val, val_2, 0x1000) != 0){
				printf("Error%d round %x \n", err_cnt, n);
				err_cnt++;

			}
		}
		
#endif
		

		sendMsg(fd_data, "EXIT");

	
		hexDump("READ", val_2, 10);
	

		NetClo(fd_data);
	}
	
	printf("Finish!");

	free(val);
	free(val_2);

	while (1){
		Sleep(1000);
	}
	return 0;
}

