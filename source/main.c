#include <nds.h>
#include <dswifi9.h>
#include <memory.h>
#include <stdio.h>

#include "ftplib.h"
#include "libSave.h"

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

int wrote, romsize, savesize;

PrintConsole top, bot;

u8 save_data[0x20000];

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

void dumpRom(void) {
	romsize = getGameSize();
	wrote = 0;

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
	consoleSelect(&top);
	printf("All done!\n");
}

void dumpSave(void) {
	romsize = getGameSize();
	savesize = SaveSize(save_data, romsize);
	wrote = 0;

	switch (savesize){
		case 0x200:
			printf("Attempting to read EEPROM (512B). This might not work");
			GetSave_EEPROM_512B(save_data);
			break;
		case 0x2000:
			printf("Attempting to read EEPROM (8KB). This might not work");
			GetSave_EEPROM_8KB(save_data);
			break;
		case 0x8000:
			GetSave_SRAM_32KB(save_data);
			break;
		case 0x10000:
			GetSave_FLASH_64KB(save_data);
			break;
		case 0x20000:
			GetSave_FLASH_128KB(save_data);
			break;
		default:
			break;
	}

	sprintf(fname, "%.18s.sav", (char*)GBAROM+0xa0);
	FtpAccess(fname, FTPLIB_FILE_WRITE, FTPLIB_IMAGE, buf, &ndata);
	printf("Dumping %s\n",fname);
	printf("SAV size: 0x%x\n\n", savesize);
	consoleSelect(&bot);
	do {
		wrote += FtpWrite(save_data + wrote, savesize-wrote, ndata);
		printf("Progress: 0x%x\n", wrote);
	} while (savesize-wrote > 0);
	swiDelay(10000);
	FtpClose(ndata);
	consoleSelect(&top);
	printf("All done!\n");
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

	if (isDSiMode()){
		printf("This homebrew can only be run\non a Nintendo DS or DS Lite!\n\n");
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
				for (int i = 0; i < strlen(ftp_ip); i++)
				{
					if ((ftp_ip[i] < '0' || ftp_ip[i] > '9') && ftp_ip[i] != '.')
						invalidIp = true;
				}
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
			
			printf("Insert or switch cartridges\n");
			printf("Then press (A) to backup save file or (B) to backup ROM\n\n");
				
			while (1) { 
				swiWaitForVBlank();
				scanKeys();
				if (keysDown() & KEY_A) {
					dumpSave();
				} else if (keysDown() & KEY_B) {
					dumpRom();
				} else if (keysDown() & KEY_START) break;
			}

			printf("Done backing up! Exit by pressing START");
			FtpQuit(buf);
			Wifi_DisconnectAP();
		}
	}

	while (1) {
		swiWaitForVBlank();
		scanKeys();
		if (keysDown() & KEY_START) break;
	}

	return 0;
}
