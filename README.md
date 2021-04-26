# gbaxxdumper
DS Download Play-compatible (flashme/haxxstation) GBA cart dumper over Wi-Fi, made by vappster. Uses softmods only, no flashcart/dongle/GC<->GBA link cable required.

Crappy stream showing the dumper in action can be found here: https://www.twitch.tv/videos/1001495693 (jump to 1:23:39 for a succesful dump, will edit the stream into a better video soon™)


In order to use this tool you will need:

- One of the following consoles: Wii/Wii U/DSi/3DS (console must be softmodded. please note - Wii U is currently untested, but technically compatible)
- Nintendo DS or DS Lite (optional - running flashme)
- A way to configure Wi-Fi on your DS/DS Lite (Wi-Fi enabled DS game or homebrew)
- A Wi-Fi access point (open or WEP-protected, the DS/DS Lite must be able to connect to it)
- FTP server (protip: you can also use ftpii or DSFTP directly from your own console - **ftpd-3ds does not seem to work**)
- (Required if your NDS/NDSL doesn't have flashme) DS Download Station - Volume 1 US ROM

# Instructions
## Step 1: Loading gbaxxdumper via Download Play
### Wii or Wii U
1) Download [wii-ds-rom-sender](https://github.com/FIX94/wii-ds-rom-sender) and copy it to your SD card
2) Create a folder called "srl" to your SD card and copy gbaxxdumper.nds inside it
2b) (Required if your NDS/NDSL doesn't have flashme) Copy your DS Download Station - Volume 1 US ROM to the root of your SD card and rename it to haxxstation.nds
3) Either download [ftpii](https://wiibrew.org/wiki/Ftpii) and copy it to your SD card or set up the FTP server of your choice
4) Configure and connect DS/DS Lite to your Wi-Fi access point (and your Wii/Wii U if running ftpii)
5) Open the Homebrew Channel (in vWii if using a Wii U) and launch wii-ds-rom-sender, choose to send gbaxxdumper.nds
6) Insert the GBA cart in your DS/DS Lite, power up the console and choose DS Download Play
7) Download and run gbaxxdumper (if using haxxstation, you'll be prompted to get into the DS Download Station menu first and then you can choose gbaxxdumper from it)

### DSi/3DS
1) Download and install [TWiLightMenu](https://github.com/DS-Homebrew/TWiLightMenu)
2) Choose one of the following:


**haxxstation**: Either use [dspatch-gui](https://github.com/RetroHeim-Soft/dspatch-gui) or [dspatch](https://github.com/Gericom/dspatch) to inject gbaxxdumper into the DS Download Station - Volume 1 US ROM, then copy the patched ROM to your DSi/3DS SD card


**flashme**: Either use the instructions above or, as a more legally accessible way, dump a ROM of a DS Download Play-compatible game and use NitroExplorer or a similar software to inject gbaxxdumper into it (search for .srl files in the DS ROM, usually called main.srl, and reinsert gbaxxdumper over it), then copy the patched ROM to your DSi/3DS SD card

3) Either download [DSFTP](https://www.gamebrew.org/wiki/DSFTP) and copy it to your SD card then set up its configuration file or set up the FTP server of your choice
4) Configure and connect the DS/DS Lite to your Wi-Fi access point (and your DSi/3DS in DS mode if running DSFTP)
5) Boot up your DSi/3DS, open TWiLightMenu and run the patched download station/DS game ROM that you made in step 2
6) Insert the GBA cart in your DS/DS Lite, power up the console and choose DS Download Play
7) If you're using haxxstation, the option to download and run DS Download Station should be shown on the DS/DS Lite: do so and then download & run gbaxxdumper. If using flashme with a patched DS game, choose DS Download Play on your game to send the ROM to your DS Lite and the option to run gbaxxdumper should pop up on the DS/DS Lite - download & run it.

## Step 2: Dumping the GBA game over Wi-Fi
1) (If using ftpii/DSFTP) Quit wii-ds-rom-sender (Wii/Wii U) or TWiLightMenu (DSi/3DS) and open ftpii/DSFTP before proceeding further
2) Your DS/DS Lite should show you a number of prompt asking for the FTP server IP address, auth info, etc on the bottom screen:

**There is a known bug which sometimes shows a blank prompt on the bottom screen. If this happens, just press return on the onscreen keyboard**

**FTP Server IP:** Self-explanatory, ftpii should show it on screen when you launch it

**FTP Server port:** Defaults to 21 (ftpii: no need to change it by default, press return / DSFTP: the port currently in use should be shown on screen)

**FTP Username:** Defaults to anonymous (ftpii: no need to change it by default, press return / DSFTP: choose the user from your config file)

**FTP Password:** Defaults to guest (ftpii: no need to change it by default, press return / DSFTP: choose the user from your config file)

**CWD to:**  If your FTP server does not have a writable root directory, you can use this to CWD to a directory that you can write to with the given user (ftpii: write "sd", no quotes, and press return / DSFTP: if you set the root dir to be writable then no need to CWD, press return, otherwise check your configuration file)

3) ROM dumping should start, you can check the progress on the bottom screen of your DS/DS Lite. Once done, a message saying "All done!" should pop up on the top screen.

# Credits
gbaxxdumper was made by vappster

ftplib was originally ported to the Nintendo DS by AdmiralCurtiss, some fixes added by vappster

Some code has been adapted from gba-link-cable-dumper by FIX94 (specifically line 35 @ gbhaxxdumper's main.c) and Robz8 (help with testing & bugfixing)

And last but not least, thanks to joedj for ftpii, Bjoern Giesler for DSFTP, shutterbug2000 for haxxstation, Gericom for dspatch, RetroHeim-Soft for dspatch-gui, Treeki for Nitro Explorer and (further) thanks to FIX94 for gba-link-cable-dumper!
