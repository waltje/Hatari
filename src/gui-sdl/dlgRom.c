/*
  Hatari - dlgRom.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/
const char DlgRom_fileid[] = "Hatari dlgRom.c";

#include "main.h"
#include "configuration.h"
#include "dialog.h"
#include "sdlgui.h"
#include "file.h"


#define DLGROM_TOSBROWSE  4
#define DLGROM_TOSNAME    5
#define DLGROM_CARTEJECT  9
#define DLGROM_CARTBROWSE 10
#define DLGROM_CARTNAME   11
#define DLGROM_EXIT       14


/* The ROM dialog: */
static SGOBJ romdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 52,24, NULL },
	{ SGBOX, 0, 0, 1,1, 50,8, NULL },
	{ SGTEXT, 0, 0, 22,2, 9,1, "TOS setup" },
	{ SGTEXT, 0, 0, 2,5, 25,1, "TOS image:" },
	{ SGBUTTON, 0, 0, 42,5, 8,1, "_Browse" },
	{ SGTEXT, 0, 0, 2,7, 46,1, NULL },
	{ SGBOX, 0, 0, 1,10, 50,9, NULL },
	{ SGTEXT, 0, 0, 18,11, 15,1, "Cartridge setup" },
	{ SGTEXT, 0, 0, 2,13, 25,1, "Cartridge image:" },
	{ SGBUTTON, 0, 0, 32,13, 8,1, "_Eject" },
	{ SGBUTTON, 0, 0, 42,13, 8,1, "B_rowse" },
	{ SGTEXT, 0, 0, 2,15, 46,1, NULL },
	{ SGTEXT, 0, 0, 2,17, 25,1, "NOTE: To use, disable both VDI mode & GEMDOS HD!" },
	{ SGTEXT, 0, 0, 2,20, 25,1, "A reset is needed after changing these options." },
	{ SGBUTTON, SG_DEFAULT, 0, 16,22, 20,1, "Back to main menu" },
	{ SGSTOP, 0, 0, 0,0, 0,0, NULL }
};


/*-----------------------------------------------------------------------*/
/**
 * Show and process the ROM dialog.
 */
void DlgRom_Main(void)
{
	char szDlgTosName[47];
	char szDlgCartName[47];
	int but;

	SDLGui_CenterDlg(romdlg);

	File_ShrinkName(szDlgTosName, ConfigureParams.Rom.szTosImageFileName, sizeof(szDlgTosName)-1);
	romdlg[DLGROM_TOSNAME].txt = szDlgTosName;

	File_ShrinkName(szDlgCartName, ConfigureParams.Rom.szCartridgeImageFileName, sizeof(szDlgCartName)-1);
	romdlg[DLGROM_CARTNAME].txt = szDlgCartName;

	do
	{
		but = SDLGui_DoDialog(romdlg);
		switch (but)
		{
		 case DLGROM_TOSBROWSE:
			/* Show and process the file selection dlg */
			SDLGui_FileConfSelect("TOS ROM image:", szDlgTosName,
					      ConfigureParams.Rom.szTosImageFileName,
					      sizeof(szDlgTosName)-1,
					      false);
			break;

		 case DLGROM_CARTEJECT:
			szDlgCartName[0] = 0;
			ConfigureParams.Rom.szCartridgeImageFileName[0] = 0;
			break;

		 case DLGROM_CARTBROWSE:
			/* Show and process the file selection dlg */
			SDLGui_FileConfSelect("Cartridge image:", szDlgCartName,
					      ConfigureParams.Rom.szCartridgeImageFileName,
					       sizeof(szDlgCartName)-1,
					      false);
			break;
		}
	}
	while (but != DLGROM_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);
}
