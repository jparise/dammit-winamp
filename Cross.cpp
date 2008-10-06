// Crossover module
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

#include "Cross.h"

//-----------------------------------------------
// Function Delcarations
//-----------------------------------------------

// WinAMP functions
void Cross_Config(struct winampVisModule *mod);
int Cross_Init(struct winampVisModule *mod);
int Cross_Render(struct winampVisModule *mod);
void Cross_Quit(struct winampVisModule *this_mod);

// Preferences
void Cross_LoadPrefs ( void );
void Cross_WritePrefs ( void );

// Window callbacks
LRESULT CALLBACK Cross_WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Cross_DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------
// Global variables
//-----------------------------------------------

// Window class name
char Cross_szAppName[] = "Crossover";

// Module variables
int Cross_CurSpeed;
int Cross_SpeedCount = 10;
int Cross_CurLights;
int Cross_LoopMax;
int Cross_LoopCount = 1;
int Cross_left;
int Cross_right;

// Preferences
typedef struct
{
	int		num_lights;
	int		speed;
	bool	inverse;
	bool	rebound;
}sCrossPrefsStruct,*pCrossPrefsStructPtr;

pCrossPrefsStructPtr		pCrossPrefs;

// Crossover module
winampVisModule Cross_mod =
{
	"Crossover",	// Description
	NULL,			// Parent window (filled in by calling app)
	NULL,			// DLL instance (filled in by calling app)
	0,				// Sample rate (filled in by calling app)
	0,				// Number of channels (filled in by calling app)
	50,				// Latency from call of RenderFrame to actual drawing
	50,				// Delay between calls in ms
	0,				// Number of Spectrum Analysis Channels
	0,				// Number of Waveform Analysis Channels
	{ 0, },			// spectrumData[2][576]
	{ 0, },			// waveformData[2][576]
	Cross_Config,	// Configuration dialog
	Cross_Init,		// Initialization (0 on success, 1 if vis should end)
	Cross_Render,	// Render (0 on success, 1 if vis should end)
	Cross_Quit		// Exit procedure
};


//-----------------------------------------------
// Functions
//-----------------------------------------------

void Cross_Config(struct winampVisModule *mod)
{
	// Build the configuration dialog box
	DialogBox(	mod->hDllInstance,
				MAKEINTRESOURCE(IDD_CROSSCFG),
				mod->hwndParent,
				(DLGPROC)Cross_DialogProc );
}

//-----------------------------------------------

int Cross_Init(struct winampVisModule *mod)
{
	// Load preferences
	Cross_LoadPrefs();

	// Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = Cross_WndProc;			// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = Cross_szAppName;		// our window class name
	
	if (!RegisterClass(&wc)) 
	{
		MessageBox( mod->hwndParent,"Error registering window class","Error",MB_OK);
		return 1;
	}

	// Inititialize the port
	_outp( LPTPort, 0 );
	Cross_CurSpeed = pCrossPrefs->speed;
	Cross_CurLights = pCrossPrefs->num_lights;
	Cross_left = 0;
	Cross_right = (pCrossPrefs->num_lights - 1);

	return 0;
}

//-----------------------------------------------

int Cross_Render(struct winampVisModule *mod)
{
	char ch = 0;

	// Set the variable loop max
	if (pCrossPrefs->rebound)
	{
		Cross_LoopMax = pCrossPrefs->num_lights;
	}
	else
	{
		Cross_LoopMax = ((pCrossPrefs->num_lights + 1) / 2);
	}
	
	// Reset the lights if the number changes
	if (Cross_CurLights != pCrossPrefs->num_lights)
	{
		Cross_CurLights = pCrossPrefs->num_lights;
		Cross_left = 0;
		Cross_right = (pCrossPrefs->num_lights - 1);
	}
	
	// Reset the speed counter if the speed changes
	if (Cross_CurSpeed != pCrossPrefs->speed)
	{
		Cross_CurSpeed = pCrossPrefs->speed;
		Cross_SpeedCount = 10;
	}

	if (Cross_SpeedCount < 109) Cross_SpeedCount++;
		else Cross_SpeedCount = 10;
	
	if (Cross_SpeedCount % pCrossPrefs->speed == 0)
	{	
		ch = ( (1 << Cross_left) | (1 << Cross_right) );
		
		// Write to the port
		if (pCrossPrefs->inverse) _outp( LPTPort, ~ch );
			else _outp( LPTPort, ch );

		if (Cross_LoopCount < Cross_LoopMax)
		{
			Cross_LoopCount++;
			Cross_left++;
			Cross_right--;
		}
		else
		{
			Cross_LoopCount = 1;
			Cross_left = 0;
			Cross_right = (pCrossPrefs->num_lights - 1);
		}
	}

	return 0;
}

//-----------------------------------------------

void Cross_Quit(struct winampVisModule *this_mod)
{
	// Cut off the lights
	_outp( LPTPort, 0 );
	// Unregister the window and class
	UnregisterClass(Cross_szAppName,this_mod->hDllInstance);
}

//-----------------------------------------------

LRESULT CALLBACK Cross_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd,message,wParam,lParam);
}

//-----------------------------------------------

BOOL CALLBACK Cross_DialogProc ( HWND hDlg,UINT iMsg,WPARAM wParam, LPARAM lParam ) 
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			Cross_LoadPrefs ();

			// Number of Lights slider
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETRANGE,1,MAKELONG(1, 8));
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETPOS,1,pCrossPrefs->num_lights);

			// Crossover Speed slider
			SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_SETRANGE,1,MAKELONG(1, 10));
			SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_SETPOS,1,pCrossPrefs->speed);

			// Invert Lights checkbox
			if (pCrossPrefs->inverse)
				SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_SETCHECK,BST_CHECKED,0);

			// Rebound Lights checkbox
			if (pCrossPrefs->rebound)
				SendMessage (GetDlgItem(hDlg,IDC_REBOUND ),BM_SETCHECK,BST_CHECKED,0);

		return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDOK:
					pCrossPrefs->num_lights = SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_GETPOS,0,0 );
					pCrossPrefs->speed = SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_GETPOS,0,0 );
					if ( SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_GETCHECK,0,0) == BST_CHECKED )
						pCrossPrefs->inverse = true;
					else
						pCrossPrefs->inverse = false;	
					if ( SendMessage (GetDlgItem(hDlg,IDC_REBOUND ),BM_GETCHECK,0,0) == BST_CHECKED )
						pCrossPrefs->rebound = true;
					else
						pCrossPrefs->rebound = false;	

					Cross_WritePrefs();
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

void Cross_LoadPrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Cross.ini" );

	if (pCrossPrefs == NULL)
	{
		pCrossPrefs = (pCrossPrefsStructPtr) malloc ( sizeof(sCrossPrefsStruct) );
		
		fp = fopen (szPrefsPath,"rb");
		if (fp == NULL)
		{
			pCrossPrefs->num_lights = 7;
			pCrossPrefs->speed = 6;
			pCrossPrefs->inverse = false;
			pCrossPrefs->rebound = false;
			Cross_WritePrefs();
		}
		else
		{
			fread ( (char*)pCrossPrefs, sizeof(sCrossPrefsStruct), 1, fp );
			fclose (fp);
		}
	}
}

//-----------------------------------------------

void Cross_WritePrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Cross.ini" );
	
	fp = fopen (szPrefsPath,"wb");

	if (fp != NULL)
		fwrite ( (char*)pCrossPrefs,sizeof (sCrossPrefsStruct), 1, fp );

	fclose (fp);
}
