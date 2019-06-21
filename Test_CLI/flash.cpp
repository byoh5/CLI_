#include "stdafx.h"
#include <windows.h>
#include "CLI.h"
#include "flash.h"
#include <time.h>

#define FLASH_TIME_OUT_SECOND 100
#define SFLS_REG_BASE 0x40500000
#define REG_SIZE 4

#define BUS_REQ 1
#define USR_BUS_REQ 3
UINT32 sfls_wait_for_bus_request(int fd)
{
	UINT32 read = 0;
	UINT32 err = 0;
	clock_t start;
	start = clock();
	do{
		//Sleep(100);
		getDataRSP_(fd, SFLS_REG_BASE+(8<<3), REG_SIZE, &read);
		
	//	if (((clock() - start) / CLOCKS_PER_SEC) > FLASH_TIME_OUT_SECOND) return -1;
	} while (read & (1 << BUS_REQ));

	return 0;
}

UINT32 sfls_wait_for_user_request(int fd)
{
	UINT32 read = 0;
	UINT32 err = 0;
	clock_t start;
	start = clock();
	do{
		//Sleep(100);
		getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &read);

		//	if (((clock() - start) / CLOCKS_PER_SEC) > FLASH_TIME_OUT_SECOND) return -1;
	} while (read & (0x1));

	return 0;
}


#define ID_VAL 0
UINT32 sfls_wait_for_reg_idval(int fd)
{
	UINT32 read = 0;
	UINT32 err = 0;
	clock_t start;
	start = clock();
	do{
		//Sleep(100);
		getDataRSP_(fd, SFLS_REG_BASE + (0 << 3), REG_SIZE, &read);

		if (((clock() - start) / CLOCKS_PER_SEC) > FLASH_TIME_OUT_SECOND) return -1;
	} while (!(read & (1 << ID_VAL)));

	return 0;
}

void sfls_init(int fd)
{
	sfls_wait_for_reg_idval(fd);

	_SFLS_1 sf1;
	getDataRSP_(fd, SFLS_REG_BASE + (1 << 3), REG_SIZE, &sf1);
	printf("get sf1 %x \n", sf1);
	sf1.BUS_CMD_RD = 0xEB;
	sf1.BUS_CMD_WR = 0x02;
	sf1.BUS_CMD_WREN = 0x06;
	sf1.BUS_CMD_RDREG = 0x05;
	printf("set sf1 %x \n", sf1);
	setDataRSP_(fd, SFLS_REG_BASE + (1 << 3), REG_SIZE, &sf1);

	_SFLS_2 sf2;
	getDataRSP_(fd, SFLS_REG_BASE + (2 << 3), REG_SIZE, &sf2);
	printf("get sf2 %x \n", sf2);
	sf2.BUS_RD_CMD_MODE = 2;
	sf2.BUS_RD_ADR_MODE = 2;
	sf2.BUS_RD_DAT_MODE = 2;
	sf2.BUS_WR_CMD_MODE = 2;
	sf2.BUS_WR_ADR_MODE = 2;
	sf2.BUS_WR_DAT_MODE = 2;
	sf2.BUS_WREN_CMD_MODE = 2;
	sf2.BUS_RDREG_CMD_MODE = 2;
	sf2.BUS_RDREG_DAT_MODE = 2;
	sf2.BUS_GAP_EN = 1;
	sf2.BUS_GAP = 5; // Flash 마다 다름 Winbond 1  Eon 5 
	printf("set sf2 %x \n", sf2);
	setDataRSP_(fd, SFLS_REG_BASE + (2 << 3), REG_SIZE, &sf2);

	_SFLS_4 sf4;
	getDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);
	printf("get sf4 %x \n", sf4);
	sf4.USR_CMD = 0x38;
	printf("set sf4 %x \n", sf4);
	setDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);

	_SFLS_8 sf8;
	getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);
	printf("get sf8 %x \n", sf8);
	sf8.IO_CLKDIV = 1;
	sf8.IO_RDLTC = 2;
	sf8.USRBUS_REQ = 1;
	printf("set sf8 %x \n", sf8);
	setDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);

	sfls_wait_for_bus_request(fd);

}

void sfls_write_enable(int fd)
{
	_SFLS_4 sf4;
	getDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);
	printf("get sf4 %x \n", sf4);
	sf4.USR_CMD_MODE = 2;
	sf4.USR_ADR_EN = 0;
	sf4.USR_GAP_EN = 0;
	sf4.USR_RD_EN = 0;
	sf4.USR_WR_EN = 0;
	sf4.USR_GAP = 0;
	sf4.USR_LEN = 0;
	sf4.USR_ADR_EXT = 0;
	sf4.USR_BUSY_EN = 0;
	sf4.USR_RDLTC = 2;
	sf4.USR_CMD = 0x06;
	printf("set sf4 %x \n", sf4);
	setDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);

	_SFLS_8 sf8;
	getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);
	printf("get sf8 %x \n", sf8);
	sf8.USR_REQ = 1;
	printf("set sf8 %x \n", sf8);
	setDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);

	sfls_wait_for_user_request(fd);
}

void sfls_sect_erase(int fd, UINT Adr)
{
	sfls_write_enable(fd);

	_SFLS_4 sf4;
	getDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);
	printf("get sf4 %x \n", sf4);
	sf4.USR_CMD_MODE = 2;
	sf4.USR_ADR_MODE = 2;
	sf4.USR_ADR_EN = 1;
	sf4.USR_GAP_EN = 0;
	sf4.USR_RD_EN = 0;
	sf4.USR_WR_EN = 0;
	sf4.USR_GAP = 0;
	sf4.USR_LEN = 0;
	sf4.USR_ADR_EXT = 0;
	sf4.USR_BUSY_EN = 1;
	sf4.USR_RDLTC = 2;
	sf4.USR_CMD = 0x20;
	printf("set sf4 %x \n", sf4);
	setDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);

	_SFLS_3 sf3;
	getDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);
	printf("get sf3 %x \n", sf3);
	sf3.USR_RDREG_CMD_MODE = 2;
	sf3.USR_RDREG_DAT_MODE = 2;
	printf("set sf3 %x \n", sf3);
	setDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);

	_SFLS_5 sf5;
	getDataRSP_(fd, SFLS_REG_BASE + (5 << 3), REG_SIZE, &sf5);
	printf("get sf5 %x \n", sf5);
	sf5.USR_ADR = Adr;
	printf("set sf5 %x \n", sf5);
	setDataRSP_(fd, SFLS_REG_BASE + (5 << 3), REG_SIZE, &sf5);

	_SFLS_8 sf8;
	getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);
	printf("get sf8 %x \n", sf8);
	sf8.USR_REQ = 1;
	printf("set sf8 %x \n", sf8);
	setDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);

	sfls_wait_for_user_request(fd);

}

void sfls_block_erase(int fd, UINT Adr)
{
	sfls_write_enable(fd);

	_SFLS_4 sf4;
	getDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);
	printf("get sf4 %x \n", sf4);
	sf4.USR_CMD_MODE = 2;
	sf4.USR_ADR_MODE = 2;
	sf4.USR_ADR_EN = 1;
	sf4.USR_GAP_EN = 0;
	sf4.USR_RD_EN = 0;
	sf4.USR_WR_EN = 0;
	sf4.USR_GAP = 0;
	sf4.USR_LEN = 0;
	sf4.USR_ADR_EXT = 0;
	sf4.USR_RDLTC = 2;
	sf4.USR_BUSY_EN = 1;
	sf4.USR_CMD = 0xd8;
	printf("set sf4 %x \n", sf4);
	setDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);

	_SFLS_3 sf3;
	getDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);
	printf("get sf3 %x \n", sf3);
	sf3.USR_RDREG_CMD_MODE = 2;
	sf3.USR_RDREG_DAT_MODE = 2;
	printf("set sf3 %x \n", sf3);
	setDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);

	_SFLS_5 sf5;
	getDataRSP_(fd, SFLS_REG_BASE + (5 << 3), REG_SIZE, &sf5);
	printf("get sf5 %x \n", sf5);
	sf5.USR_ADR = Adr;
	printf("set sf5 %x \n", sf5);
	setDataRSP_(fd, SFLS_REG_BASE + (5 << 3), REG_SIZE, &sf5);

	_SFLS_8 sf8;
	getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);
	printf("get sf8 %x \n", sf8);
	sf8.USR_REQ = 1;
	printf("set sf8 %x \n", sf8);
	setDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);

	sfls_wait_for_user_request(fd);

}

void sfls_chip_erase(int fd)
{
	sfls_write_enable(fd);

	_SFLS_4 sf4;
	getDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);
	printf("get sf4 %x \n", sf4);
	sf4.USR_CMD_MODE = 2;

	sf4.USR_ADR_EN = 0;
	sf4.USR_GAP_EN = 0;
	sf4.USR_RD_EN = 0;
	sf4.USR_WR_EN = 0;
	sf4.USR_GAP = 0;
	sf4.USR_LEN = 0;
	sf4.USR_ADR_EXT = 0;
	sf4.USR_RDLTC = 2;
	sf4.USR_BUSY_EN = 1;
	sf4.USR_CMD = 0xC7;
	printf("set sf4 %x \n", sf4);
	setDataRSP_(fd, SFLS_REG_BASE + (4 << 3), REG_SIZE, &sf4);

	_SFLS_3 sf3;
	getDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);
	printf("get sf3 %x \n", sf3);
	sf3.USR_RDREG_CMD_MODE = 2;
	sf3.USR_RDREG_DAT_MODE = 2;
	printf("set sf3 %x \n", sf3);
	setDataRSP_(fd, SFLS_REG_BASE + (3 << 3), REG_SIZE, &sf3);

	_SFLS_8 sf8;
	getDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);
	printf("get sf8 %x \n", sf8);
	sf8.USR_REQ = 1;
	printf("set sf8 %x \n", sf8);
	setDataRSP_(fd, SFLS_REG_BASE + (8 << 3), REG_SIZE, &sf8);

	sfls_wait_for_user_request(fd);

}


