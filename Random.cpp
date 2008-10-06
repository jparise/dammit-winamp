#include <windows.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#include "vis.h"
#include "dammit.h"

#include "Random.h"


char	Random_szAppName[]	=	"DAMMIT Random";

int Random_Wait=0;


//Our Mod 
winampVisModule Random_mod =
{
	"Random",
	NULL,
	NULL,
	0,
	0,
	25,		//Latency
	25,		//Latency
	0,			//Number of Spectrum Analysis Channels
	0,			//Number of Waveform Analysis Channels
	{ 0, },		// spectrumData
	{ 0, },		// waveformData
	Random_Config,
	Random_Init,
	Random_Render, 
	Random_Quit
};


//
//		Configuration
//
void Random_Config(struct winampVisModule *mod)
{
	MessageBox( mod->hwndParent, 
				"Random Plugin\n"
				"Soco\n"
				"Computer Science House\n"
				"Rochester Institute of Technology\n",
				"DAMMIT",
				MB_OK);
}

//
//		Initialization
//
int Random_Init( struct winampVisModule *mod)
{

	//Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = Random_WndProc;				// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = Random_szAppName;
	
	if (!RegisterClass(&wc)) 
	{
		MessageBox( mod->hwndParent,"Error registering window class","blah",MB_OK);
		return 1;
	}


	_outp( LPTPort, 0 );

	srand( (unsigned)time( NULL ) );

	return 0;
}


// render function for VU meter. Returns 0 if successful, 1 if visualization should end.
int Random_Render( struct winampVisModule *mod)
{
	if(!Random_Wait)
	{
		Random_Wait = (rand()%10) +2;
		_outp( LPTPort, rand()%255 );
	}
	else Random_Wait--;
	return 0;
}

//
//		Quit 
//
void Random_Quit( struct winampVisModule *mod )
{
	//Cut off the lights
	_outp( LPTPort, 0 );
	//Unregister the window and class
	UnregisterClass(Random_szAppName,mod->hDllInstance); // unregister window class
}


//
// window procedure for our window
//
LRESULT CALLBACK Random_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/*
		case WM_CREATE:		return 0;
		case WM_ERASEBKGND: return 0;
		case WM_PAINT:
			{ // update from doublebuffer
				PAINTSTRUCT ps;
				RECT r;
				HDC hdc = BeginPaint(hwnd,&ps);
				GetClientRect(hwnd,&r);
				BitBlt(hdc,0,0,r.right,r.bottom,memDC,0,0,SRCCOPY);
				EndPaint(hwnd,&ps);
			}
		return 0;
		case WM_DESTROY: PostQuitMessage(0); return 0;
		case WM_KEYDOWN: // pass keyboard messages to main winamp window (for processing)
		case WM_KEYUP:
			{	// get this_mod from our window's user data
				winampVisModule *this_mod = (winampVisModule *) GetWindowLong(hwnd,GWL_USERDATA);
				PostMessage(this_mod->hwndParent,message,wParam,lParam);
			}
		return 0;
		case WM_MOVE:
			{	// get config_x and config_y for configuration
				RECT r;
				GetWindowRect(hMainWnd,&r);
				config_x = r.left;
				config_y = r.top;
			}
		return 0;
	*/
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}