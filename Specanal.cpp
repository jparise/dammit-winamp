// Spectrum Analysis module
//-----------------------------------------------

// Include
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "vis.h"
#include "dammit.h"
#include "resource.h"

#include "SpecAnal.h"

//-----------------------------------------------
// Function declarations
//-----------------------------------------------

// WinAMP functions
void SpecAnal_Config(struct winampVisModule *mod);
int SpecAnal_Init(struct winampVisModule *mod);
int SpecAnal_Render(struct winampVisModule *mod);
void SpecAnal_Quit(struct winampVisModule *this_mod);

// Preferences
void SpecAnal_LoadPrefs ( void );
void SpecAnal_WritePrefs ( void );

// Window callbacks
LRESULT CALLBACK SpecAnal_WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SpecAnal_DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------
// Global variables
//-----------------------------------------------

// Window class name
char SpecAnal_szAppName[] = "Spectrum Analysis";

// Module variables
int		CHAN_MAX = 16;

// Preferences
typedef struct
{
	int		num_lights;
	int		channels[8];
}sSAPrefsStruct,*pSAPrefsStructPtr;

pSAPrefsStructPtr		pSAPrefs;

// Spectrum Analysis module
winampVisModule SpecAnal_mod =
{
	"Spectrum Analysis",	// Description
	NULL,					// Parent window (filled in by calling app)
	NULL,					// DLL instance (filled in by calling app)
	0,						// Sample rate (filled in by calling app)
	0,						// Number of channels (filled in by calling app)
	2,						// Latency from call of RenderFrame to actual drawing
	2,						// Delay between calls in ms
	2,						// Number of Spectrum Analysis Channels
	0,						// Number of Waveform Analysis Channels
	{ 0, },					// spectrumData[2][576]
	{ 0, },					// waveformData[2][576]
	SpecAnal_Config,		// Configuration dialog
	SpecAnal_Init,			// Initialization (0 on success, 1 if vis should end)
	SpecAnal_Render,		// Render (0 on success, 1 if vis should end)
	SpecAnal_Quit			// Exit procedure
};

//-----------------------------------------------
// Functions
//-----------------------------------------------

void SpecAnal_Config(struct winampVisModule *mod)
{
	// Build the configuration dialog box
	DialogBox(	mod->hDllInstance,
				MAKEINTRESOURCE(IDD_SPECANALCFG),
				mod->hwndParent,
				(DLGPROC)SpecAnal_DialogProc );
}

//-----------------------------------------------

int SpecAnal_Init( struct winampVisModule *mod)
{
	// Load preferences
	SpecAnal_LoadPrefs();

	// Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = SpecAnal_WndProc;		// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = SpecAnal_szAppName;
	
	if (!RegisterClass(&wc)) 
	{
	    MessageBox( mod->hwndParent,"Error registering window class","Error",MB_OK);
		return 1;
	}

	// Setup parallel port output
	_outp( LPTPort, 0 );
	return 0;
}

//-----------------------------------------------

int SpecAnal_Render( struct winampVisModule *mod)
{
	char ch;
	int tmp;
	int start;

	ch = 0;
	start = 0;

	for( int y = 0; y < pSAPrefs->num_lights; y++)
	{
		start = y << 2;
		tmp=0;
		for( int x = 0; x < pSAPrefs->channels[y]; x++)
		{
			tmp += mod->spectrumData[0][start+x] + mod->spectrumData[1][start+x];
		}
		tmp >>= 7;
		if(tmp)
			ch |= 1 << y;
	}

	_outp( LPTPort, ch );

	return 0;
}

//-----------------------------------------------

void SpecAnal_Quit( struct winampVisModule *mod )
{
	// Cut off the lights
	_outp( LPTPort, 0 );	
	// Unregister the window and class
	UnregisterClass(SpecAnal_szAppName,mod->hDllInstance);
}

//-----------------------------------------------

LRESULT CALLBACK SpecAnal_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd,message,wParam,lParam);
}

//-----------------------------------------------

BOOL CALLBACK SpecAnal_DialogProc ( HWND hDlg,UINT iMsg,WPARAM wParam, LPARAM lParam ) 
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			SpecAnal_LoadPrefs ();
			
			// Number of Lights slider
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETRANGE,1,MAKELONG(1, 8));
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETPOS,1,pSAPrefs->num_lights);

			// Light Channel sliders
			SendMessage (GetDlgItem(hDlg,IDC_CHAN1 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN1 ),TBM_SETPOS,1,pSAPrefs->channels[0]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN2 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN2 ),TBM_SETPOS,1,pSAPrefs->channels[1]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN3 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN3 ),TBM_SETPOS,1,pSAPrefs->channels[2]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN4 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN4 ),TBM_SETPOS,1,pSAPrefs->channels[3]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN5 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN5 ),TBM_SETPOS,1,pSAPrefs->channels[4]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN6 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN6 ),TBM_SETPOS,1,pSAPrefs->channels[5]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN7 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN7 ),TBM_SETPOS,1,pSAPrefs->channels[6]);
			SendMessage (GetDlgItem(hDlg,IDC_CHAN8 ),TBM_SETRANGE,1,MAKELONG(1, CHAN_MAX));
			SendMessage (GetDlgItem(hDlg,IDC_CHAN8 ),TBM_SETPOS,1,pSAPrefs->channels[7]);

		return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDOK:
					pSAPrefs->num_lights = SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[0] = SendMessage (GetDlgItem(hDlg,IDC_CHAN1 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[1] = SendMessage (GetDlgItem(hDlg,IDC_CHAN2 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[2] = SendMessage (GetDlgItem(hDlg,IDC_CHAN3 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[3] = SendMessage (GetDlgItem(hDlg,IDC_CHAN4 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[4] = SendMessage (GetDlgItem(hDlg,IDC_CHAN5 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[5] = SendMessage (GetDlgItem(hDlg,IDC_CHAN6 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[6] = SendMessage (GetDlgItem(hDlg,IDC_CHAN7 ),TBM_GETPOS,0,0 );
					pSAPrefs->channels[7] = SendMessage (GetDlgItem(hDlg,IDC_CHAN8 ),TBM_GETPOS,0,0 );
					SpecAnal_WritePrefs();
				case IDCANCEL:
					EndDialog (hDlg, TRUE);
					break;
			}
		return TRUE;

		case WM_HSCROLL:
			break;

		case WM_CLOSE:
			EndDialog (hDlg, TRUE);
		return true;
	}
	// default case
	return FALSE;
}

//-----------------------------------------------

void SpecAnal_LoadPrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Dammit.ini" );

	if (pSAPrefs == NULL)
	{
		pSAPrefs = (pSAPrefsStructPtr) malloc ( sizeof(sSAPrefsStruct) );
		
		fp = fopen (szPrefsPath,"rb");
		if (fp == NULL)
		{
			pSAPrefs->num_lights = 7;
			pSAPrefs->channels[0] = 4;
			pSAPrefs->channels[1] = 4;
			pSAPrefs->channels[2] = 4;
			pSAPrefs->channels[3] = 4;
			pSAPrefs->channels[4] = 4;
			pSAPrefs->channels[5] = 4;
			pSAPrefs->channels[6] = 4;
			pSAPrefs->channels[7] = 4;
			SpecAnal_WritePrefs();
		}
		else
		{
			fread ( (char*)pSAPrefs, sizeof(sSAPrefsStruct), 1, fp );
			fclose (fp);
		}
	}
}

//-----------------------------------------------

void SpecAnal_WritePrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Dammit.ini" );
	
	fp = fopen (szPrefsPath,"wb");

	if (fp != NULL)
		fwrite ( (char*)pSAPrefs,sizeof (sSAPrefsStruct), 1, fp );

	fclose (fp);
}