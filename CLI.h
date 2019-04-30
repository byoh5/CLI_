// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLI_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLI_EXPORTS
#define CLI_API __declspec(dllexport)
#else
#define CLI_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif
	CLI_API int NetCon(char* addr, char* port);
	CLI_API int NetClo(int fd);
	CLI_API int getImagefromRemote(int fd, int cnt);
	CLI_API int getImgDataSizeFromList(int n, int* size, void* data);
	CLI_API int getImgSizeFromList(int n);
	CLI_API int getImgDataFromList(int n, void* data);
	CLI_API int deleteList_ALL(void);
	CLI_API int getDataFromRemote(int fd, int addr, int size, int module, void* value);
	CLI_API int setDataFromRemote(int fd, int addr, int size, int module, void* value);
#ifdef __cplusplus
}
#endif

#define WB0		0x00
#define WB1		0x01
#define WB2		0x02
#define WBCOM	0x03
#define CPU0	0x04
#define CPU1	0x05
#define CPU2	0x06