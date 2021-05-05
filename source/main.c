#include <nds.h>
#include <dswifi9.h>
#include <memory.h>
#include <stdio.h>

#include "ftplib.h"

static netbuf *buf = NULL;
netbuf *ndata;
bool invalidIp = true;

const char* EMPTYSTR = "";
int keypresses = 0;

char ftp_ip[15];
char ftp_port[5];
char ftp_user[32];
char ftp_pass[32];
char ftpstr[21];
char ftpdir[256];

char fname[256];

int wrote, romsize;

PrintConsole top, bot;

int getGameSize(void)
{
	if(*(vu32*)(0x08000004) != 0x51AEFF24)
		return 0;
	int i;
	for(i = (1<<20); i < (1<<25); i<<=1)
	{
		vu16 *rompos = (vu16*)(0x08000000+i);
		int j;
		bool romend = true;
		for(j = 0; j < 0x1000; j++)
		{
			if(rompos[j] != j)
			{
				romend = false;
				break;
			}
		}
		if(romend) break;
	}
	return i;
}

void OnKeyPressed(int key) {
	keypresses++;
	if(key > 0 && key != 10)
		iprintf("%c", key);
}

void kbdPrompt(char* promptTop, char* defVal, char* str, int maxlen) {
	int hasDefValue = strcmp(defVal, EMPTYSTR);
	int len;
	
	consoleSelect(&top);
	printf(promptTop);
	consoleSelect(&bot);
	if (hasDefValue == 0) {
		printf("(No default value)\n");
	} else {
		iprintf("(Default: %s)\n", defVal);
	}
	do {
		fgets(str, maxlen+1, stdin);	//maxlen+1 because newline gets discarded later
		len=strlen(str);
		if(str[len-1] == '\n')
			str[len-1] = '\0';
	} while (hasDefValue == 0 && strcmp(str, EMPTYSTR) == 0);
	if (hasDefValue != 0 && strcmp(str, EMPTYSTR) == 0)
		strcpy(str, defVal);
	keypresses++;
	consoleClear();
	consoleSelect(&top);
	if (keypresses > maxlen) {	//Basic overflow protection
		char strbuf[256];	//256 is the size of dkp's keyboard buffer, used here to avoid hang on backspace
		fgets(strbuf, 256, stdin);
	}
	keypresses = 0;
}

int main(void) {
	videoSetMode(MODE_0_2D);
	videoSetModeSub(MODE_0_2D);
	vramSetBankC(VRAM_C_SUB_BG);
	consoleInit(&top, 0,BgType_Text4bpp, BgSize_T_256x256, 2, 0, true, true);
	consoleInit(&bot, 0,BgType_Text4bpp, BgSize_T_256x256, 22, 3, false, true);

	Keyboard *kbd = keyboardInit(NULL, 3, BgType_Text4bpp, BgSize_T_256x512, 20, 0, false, true);

	kbd->OnKeyPressed = OnKeyPressed;

	sysSetBusOwners(true, true);
	sysSetCartOwner(true);

	consoleSelect(&top);
	printf("gbaxxdumper v0.66 by vappster\n\n");

	romsize = getGameSize();
	if (isDSiMode()){
		printf("This homebrew can only be run\non a Nintendo DS or DS Lite!\n\n");
	} else {
		if (romsize == 0){
			printf("GBA cartridge not found!\n\nDouble check that the cart is\nrecognized correctly by your DS\nand try again.");
		} else {
			printf("Connecting to Wi-Fi AP\n");
			if (!Wifi_InitDefault(WFC_CONNECT)) {
				printf("Wi-Fi AP connection failed!\n\nMake sure your Wi-Fi settings\nare correct and try again.");
			} else {
				keyboardShow();
				while (invalidIp) {
					ftp_ip[0] = 0;
					kbdPrompt("Enter FTP server IP: \n", "", ftp_ip, 15);
					invalidIp = (strcmp(ftp_ip, "") == 0 || strncmp(ftp_ip, " ", 1) == 0 || strlen(ftp_ip) < 7 || strlen(ftp_ip) > 15);
					iprintf("%s\n\n", invalidIp ? "Invalid!" : ftp_ip);
				}

				kbdPrompt("Enter FTP server port: ", "21", ftp_port, 5);
				iprintf("%s\n\n", ftp_port);
				
				kbdPrompt("Enter FTP username: ", "anonymous", ftp_user, 32);
				iprintf("%s\n\n", ftp_user);
				
				kbdPrompt("Enter FTP password: ", "guest", ftp_pass, 32);
				iprintf("%s\n\n", ftp_pass);
				
				kbdPrompt("CWD to:", ".", ftpdir, 256);
				iprintf("%s\n\n", ftpdir);
				keyboardHide();

				sprintf(ftpstr, "%s:%s", ftp_ip, ftp_port);
				printf("Connecting to ");
				printf("%s\n",ftpstr);
				while (!FtpConnect(ftpstr, &buf)) {
					swiDelay(10000);
				}
				printf("Logging in\n");
				while (!FtpLogin(ftp_user, ftp_pass, buf)) {
					swiDelay(10000);
				}
				if (ftpdir[0] != '.'){
					printf("Changing working directory\n");
					FtpChdir(ftpdir, buf);
				}
				sprintf(fname, "%.18s.gba", (char*)GBAROM+0xa0);
				FtpAccess(fname, FTPLIB_FILE_WRITE, FTPLIB_IMAGE, buf, &ndata);
				printf("Dumping %s\n",fname);
				printf("ROM size: 0x%x\n\n", romsize);
				consoleSelect(&bot);
				do {
					wrote += FtpWrite((u8*)GBAROM + wrote, romsize-wrote, ndata);
					printf("Progress: 0x%x\n", wrote);
				} while (romsize-wrote > 0);
				swiDelay(10000);
				FtpClose(ndata);
				FtpQuit(buf);
				Wifi_DisconnectAP();
				consoleSelect(&top);
				printf("All done!\n");
			}
		}
	}

	while (1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START) break;
	}

	return 0;
}
