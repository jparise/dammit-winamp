// Pong module
//-----------------------------------------------

// Includes
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>

#include "vis.h"
#include "dammit.h"
#include "resource.h"

#include "Pong.h"

//-----------------------------------------------
// Function Delcarations
//-----------------------------------------------

// WinAMP functions
void Pong_Config(struct winampVisModule *mod);
int Pong_Init(struct winampVisModule *mod);
int Pong_Render(struct winampVisModule *mod);
void Pong_Quit(struct winampVisModule *this_mod);

// Preferences
void Pong_LoadPrefs ( void );
void Pong_WritePrefs ( void );

// Window callbacks
LRESULT CALLBACK Pong_WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Pong_DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------
// Global variables
//-----------------------------------------------

// Window class name
char Pong_szAppName[] = "Pong";

// Module variables
int Pong_Current;
int Pong_SpeedCount = 10;
int Pong_CurSpeed;
int Pong_GravTable7[7] = {0,1,2,3,2,1,0};
int Pong_GravTable8[8] = {0,1,2,3,3,2,1,0};
int Pong_GravCount = 0;
int Pong_GravCur = 1;

// Preferences
typedef struct
{
	int		num_lights;
	int		speed;
	bool	inverse;
	bool	gravity;
}sPrefsStruct,*pPrefsStructPtr;

pPrefsStructPtr		pPrefs;

// Pong module
winampVisModule Pong_mod =
{
	"Pong",			// Description
	NULL,			// Parent window (filled in by calling app)
	NULL,			// DLL instance (filled in by calling app)
	0,				// Sample rate (filled in by calling app)
	0,				// Number of channels (filled in by calling app)
	25,				// Latency from call of RenderFrame to actual drawing
	25,				// Delay between calls in ms
	0,				// Number of Spectrum Analysis Channels
	0,				// Number of Waveform Analysis Channels
	{ 0, },			// spectrumData[2][576]
	{ 0, },			// waveformData[2][576]
	Pong_Config,	// Configuration dialog
	Pong_Init,		// Initialization (0 on success, 1 if vis should end)
	Pong_Render,	// Render (0 on success, 1 if vis should end)
	Pong_Quit		// Exit procedure
};


//-----------------------------------------------
// Functions
//-----------------------------------------------

void Pong_Config(struct winampVisModule *mod)
{
	// Build the configuration dialog box
	DialogBox(	mod->hDllInstance,
				MAKEINTRESOURCE(IDD_PONGCFG),
				mod->hwndParent,
				(DLGPROC)Pong_DialogProc );
}

//-----------------------------------------------

int Pong_Init(struct winampVisModule *mod)
{
	// Load preferences
	Pong_LoadPrefs();

	// Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = Pong_WndProc;			// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = Pong_szAppName;		// our window class name
	
	if (!RegisterClass(&wc)) 
	{
		MessageBox( mod->hwndParent,"Error registering window class","Error",MB_OK);
		return 1;
	}

	// Inititialize the port
	_outp( LPTPort, 0 );
	Pong_Current = 0;
	Pong_CurSpeed = pPrefs->speed;

	return 0;
}

//-----------------------------------------------

int Pong_Render(struct winampVisModule *mod)
{
	char ch;

	// Reset the speed counter if the speed changes
	if (Pong_CurSpeed != pPrefs->speed)
	{
		Pong_CurSpeed = pPrefs->speed;
		Pong_SpeedCount = 10;
	}

	if (Pong_SpeedCount < 109) Pong_SpeedCount++;
		else Pong_SpeedCount = 10;
	
	if (Pong_SpeedCount % pPrefs->speed == 0)
	{	
		if ( (pPrefs->gravity) && 
			(
			((pPrefs->num_lights == 7) && (Pong_GravCount != Pong_GravTable7[Pong_GravCur]))
			||
			((pPrefs->num_lights == 8) && (Pong_GravCount != Pong_GravTable8[Pong_GravCur]))
			)
			)
		{
			Pong_GravCount++;
		}
		else
		{
			if (pPrefs->gravity)
			{
				if (Pong_GravCur == pPrefs->num_lights) 
				{
					Pong_GravCur = 1;
				}
				else
				{
					Pong_GravCur++;
				}
				Pong_GravCount = 0;
			}
			if( Pong_Current < pPrefs->num_lights )
			{
				ch= 1 << Pong_Current;
			}
			else
			{
				ch= (1 << pPrefs->num_lights) >> (Pong_Current - (pPrefs->num_lights-2) ) ;
			}

			// Write to the port
			if (pPrefs->inverse) _outp( LPTPort, ~ch );
				else _outp( LPTPort, ch );

			Pong_Current++;
			if( Pong_Current >= pPrefs->num_lights*2-2 ) Pong_Current = 0;
		}
	}

	return 0;
}

//-----------------------------------------------

void Pong_Quit(struct winampVisModule *this_mod)
{
	// Cut off the lights
	_outp( LPTPort, 0 );
	// Unregister the window and class
	UnregisterClass(Pong_szAppName,this_mod->hDllInstance);
}

//-----------------------------------------------

LRESULT CALLBACK Pong_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd,message,wParam,lParam);
}

//-----------------------------------------------

BOOL CALLBACK Pong_DialogProc ( HWND hDlg,UINT iMsg,WPARAM wParam, LPARAM lParam ) 
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			Pong_LoadPrefs ();

			// Number of Lights slider
			SendMessage (GetDlgItem(hDlg,IDC_SLIDER_LIGHTS ),TBM_SETRANGE,1,MAKELONG(1, 8));
			SendMessage (GetDlgItem(hDlg,IDC_SLIDER_LIGHTS ),TBM_SETPOS,1,pPrefs->num_lights);

			// Pong Speed slider
			SendMessage (GetDlgItem(hDlg,IDC_SLIDER_SPEED ),TBM_SETRANGE,1,MAKELONG(1, 10));
			SendMessage (GetDlgItem(hDlg,IDC_SLIDER_SPEED ),TBM_SETPOS,1,pPrefs->speed);

			// Invert Lights checkbox
			if (pPrefs->inverse)
				SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_SETCHECK,BST_CHECKED,0);

			// Use Gravity checkbox
			if (pPrefs->gravity)
			{
				SendMessage (GetDlgItem(hDlg,IDC_GRAVITY ),BM_SETCHECK,BST_CHECKED,0);
			}

		return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDOK:
					pPrefs->num_lights = SendMessage (GetDlgItem(hDlg,IDC_SLIDER_LIGHTS ),TBM_GETPOS,0,0 );
					pPrefs->speed = SendMessage (GetDlgItem(hDlg,IDC_SLIDER_SPEED ),TBM_GETPOS,0,0 );
					if ( SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_GETCHECK,0,0) == BST_CHECKED )
						pPrefs->inverse = true;
					else
						pPrefs->inverse = false;
					if ( SendMessage (GetDlgItem(hDlg,IDC_GRAVITY ),BM_GETCHECK,0,0) == BST_CHECKED )
						pPrefs->gravity = true;
					else
						pPrefs->gravity = false;
					Pong_WritePrefs();
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

void Pong_LoadPrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Pong.ini" );

	if (pPrefs == NULL)
	{
		pPrefs = (pPrefsStructPtr) malloc ( sizeof(sPrefsStruct) );
		
		fp = fopen (szPrefsPath,"rb");
		if (fp == NULL)
		{
			pPrefs->num_lights = 7;
			pPrefs->speed = 6;
			pPrefs->inverse = false;
			pPrefs->gravity = false;
			Pong_WritePrefs();
		}
		else
		{
			fread ( (char*)pPrefs, sizeof(sPrefsStruct), 1, fp );
			fclose (fp);
		}
	}
}

//-----------------------------------------------

void Pong_WritePrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Pong.ini" );
	
	fp = fopen (szPrefsPath,"wb");

	if (fp != NULL)
		fwrite ( (char*)pPrefs,sizeof (sPrefsStruct), 1, fp );

	fclose (fp);
}
