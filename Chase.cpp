// Chase module
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

#include "Chase.h"

//-----------------------------------------------
// Function Delcarations
//-----------------------------------------------

// WinAMP functions
void Chase_Config(struct winampVisModule *mod);
int Chase_Init(struct winampVisModule *mod);
int Chase_Render(struct winampVisModule *mod);
void Chase_Quit(struct winampVisModule *this_mod);

// Preferences
void Chase_LoadPrefs ( void );
void Chase_WritePrefs ( void );

// Window callbacks
LRESULT CALLBACK Chase_WndProc (HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Chase_DialogProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------
// Global variables
//-----------------------------------------------

// Window class name
char Chase_szAppName[] = "Chase";

// Module variables
int Chase_Current;
int Chase_CurSpeed;
int Chase_CurLights;
int Chase_SpeedCount = 10;

// Preferences
typedef struct
{
	int		num_lights;
	int		num_chasing;
	int		speed;
	bool	inverse;
	bool	l_to_r;
}sChasePrefsStruct,*pChasePrefsStructPtr;

pChasePrefsStructPtr		pChasePrefs;

// Chase module
winampVisModule Chase_mod =
{
	"Chase",		// Description
	NULL,			// Parent window (filled in by calling app)
	NULL,			// DLL instance (filled in by calling app)
	0,				// Sample rate (filled in by calling app)
	0,				// Number of channels (filled in by calling app)
	10,				// Latency from call of RenderFrame to actual drawing
	10,				// Delay between calls in ms
	0,				// Number of Spectrum Analysis Channels
	0,				// Number of Waveform Analysis Channels
	{ 0, },			// spectrumData[2][576]
	{ 0, },			// waveformData[2][576]
	Chase_Config,	// Configuration dialog
	Chase_Init,		// Initialization (0 on success, 1 if vis should end)
	Chase_Render,	// Render (0 on success, 1 if vis should end)
	Chase_Quit		// Exit procedure
};


//-----------------------------------------------
// Functions
//-----------------------------------------------

void Chase_Config(struct winampVisModule *mod)
{
	// Build the configuration dialog box
	DialogBox(	mod->hDllInstance,
				MAKEINTRESOURCE(IDD_CHASECFG),
				mod->hwndParent,
				(DLGPROC)Chase_DialogProc );
}

//-----------------------------------------------

int Chase_Init(struct winampVisModule *mod)
{
	// Load preferences
	Chase_LoadPrefs();

	// Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = Chase_WndProc;			// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = Chase_szAppName;		// our window class name
	
	if (!RegisterClass(&wc)) 
	{
		MessageBox( mod->hwndParent,"Error registering window class","Error",MB_OK);
		return 1;
	}

	// Inititialize the port
	_outp( LPTPort, 0 );
	Chase_CurSpeed = pChasePrefs->speed;
	Chase_Current = 2;

	return 0;
}

//-----------------------------------------------

int Chase_Render(struct winampVisModule *mod)
{
	char ch = 0, tmp = 0;
	int next;

	// Reset the lights if the number changes
	if (Chase_CurLights != pChasePrefs->num_lights)
	{
		Chase_CurLights = pChasePrefs->num_lights;
		Chase_Current = 2;
	}

	// Reset the speed counter if the speed changes
	if (Chase_CurSpeed != pChasePrefs->speed)
	{
		Chase_CurSpeed = pChasePrefs->speed;
		Chase_SpeedCount = 10;
	}

	if (Chase_SpeedCount < 109) Chase_SpeedCount++;
		else Chase_SpeedCount = 10;
	
	if (Chase_SpeedCount % pChasePrefs->speed == 0)
	{	
		if (Chase_Current == (pChasePrefs->num_lights + 1))
			Chase_Current = 1;
					
		if (Chase_Current == 1)
			next = (pChasePrefs->num_lights);
		else
			next = (Chase_Current - 1);

		if ( (pChasePrefs->num_lights - pChasePrefs->num_chasing) < 2 )
			pChasePrefs->num_chasing = (pChasePrefs->num_lights - 2);

		switch (pChasePrefs->num_chasing)
		{
			case 1: ch = (1 << (Chase_Current-1)); break;
			case 2: ch = ( (1 << (next-1)) | (1 << (Chase_Current-1)) ); break;
			case 3: ch = ( (1 << (next-2)) | (1 << (next-1)) | (1 << (Chase_Current-1)) ); break;
			case 4: ch = ( (1 << (next-3)) | (1 << (next-2)) | (1 << (next-1)) | (1 << (Chase_Current-1)) ); break;
			case 5: ch = ( (1 << (next-4)) | (1 << (next-3)) | (1 << (next-2)) | (1 << (next-1)) | (1 << (Chase_Current-1)) ); break;
			case 6: ch = ( (1 << (next-5)) | (1 << (next-4)) | (1 << (next-3)) | (1 << (next-2)) | (1 << (next-1)) | (1 << (Chase_Current-1)) ); break;
		   default: ch = (1 << (Chase_Current-1)); break;
		}

		// Write to the port
		if (pChasePrefs->inverse) _outp( LPTPort, ~ch );
			else _outp( LPTPort, ch );

		if (pChasePrefs->l_to_r) Chase_Current++; else Chase_Current--;
	}

	return 0;
}

//-----------------------------------------------

void Chase_Quit(struct winampVisModule *this_mod)
{
	// Cut off the lights
	_outp( LPTPort, 0 );
	// Unregister the window and class
	UnregisterClass(Chase_szAppName,this_mod->hDllInstance);
}

//-----------------------------------------------

LRESULT CALLBACK Chase_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd,message,wParam,lParam);
}

//-----------------------------------------------

BOOL CALLBACK Chase_DialogProc ( HWND hDlg,UINT iMsg,WPARAM wParam, LPARAM lParam ) 
{
	switch (iMsg)
	{
		case WM_INITDIALOG:

			Chase_LoadPrefs ();

			// Number of Lights slider
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETRANGE,1,MAKELONG(1, 8));
			SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_SETPOS,1,pChasePrefs->num_lights);

			// Number of Chasing Lights slider
			SendMessage (GetDlgItem(hDlg,IDC_CHASING ),TBM_SETRANGE,1,MAKELONG(1, 6));
			SendMessage (GetDlgItem(hDlg,IDC_CHASING ),TBM_SETPOS,1,pChasePrefs->num_chasing);

			// Chase Speed slider
			SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_SETRANGE,1,MAKELONG(1, 10));
			SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_SETPOS,1,pChasePrefs->speed);

			// Direction radio's
			if (pChasePrefs->l_to_r)
				SendMessage (GetDlgItem(hDlg,IDC_LR ),BM_SETCHECK,BST_CHECKED,0);
			else
				SendMessage (GetDlgItem(hDlg,IDC_RL ),BM_SETCHECK,BST_CHECKED,0);

			// Invert Lights checkbox
			if (pChasePrefs->inverse)
				SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_SETCHECK,BST_CHECKED,0);

		return TRUE;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDOK:
					pChasePrefs->num_lights = SendMessage (GetDlgItem(hDlg,IDC_LIGHTS ),TBM_GETPOS,0,0 );
					pChasePrefs->num_chasing = SendMessage (GetDlgItem(hDlg,IDC_CHASING ),TBM_GETPOS,0,0 );
					pChasePrefs->speed = SendMessage (GetDlgItem(hDlg,IDC_SPEED ),TBM_GETPOS,0,0 );
					if ( SendMessage (GetDlgItem(hDlg,IDC_LR ),BM_GETCHECK,0,0) == BST_CHECKED )
						pChasePrefs->l_to_r = true;
					else if ( SendMessage (GetDlgItem(hDlg,IDC_RL ),BM_GETCHECK,0,0) == BST_CHECKED )
						pChasePrefs->l_to_r = false;
					if ( SendMessage (GetDlgItem(hDlg,IDC_INVERT ),BM_GETCHECK,0,0) == BST_CHECKED )
						pChasePrefs->inverse = true;
					else
						pChasePrefs->inverse = false;

					Chase_WritePrefs();
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

void Chase_LoadPrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Chase.ini" );

	if (pChasePrefs == NULL)
	{
		pChasePrefs = (pChasePrefsStructPtr) malloc ( sizeof(sChasePrefsStruct) );
		
		fp = fopen (szPrefsPath,"rb");
		if (fp == NULL)
		{
			pChasePrefs->num_lights = 7;
			pChasePrefs->num_chasing = 3;
			pChasePrefs->speed = 6;
			pChasePrefs->l_to_r = true;
			pChasePrefs->inverse = false;
			Chase_WritePrefs();
		}
		else
		{
			fread ( (char*)pChasePrefs, sizeof(sChasePrefsStruct), 1, fp );
			fclose (fp);
		}
	}
}

//-----------------------------------------------

void Chase_WritePrefs ( void )
{
	FILE*	fp;
	char	szPrefsPath[255];

	GetWindowsDirectory ( szPrefsPath,255 );
	strcat (szPrefsPath,"\\Chase.ini" );
	
	fp = fopen (szPrefsPath,"wb");

	if (fp != NULL)
		fwrite ( (char*)pChasePrefs,sizeof (sChasePrefsStruct), 1, fp );

	fclose (fp);
}
