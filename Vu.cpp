
#include <windows.h>
#include <conio.h>

#include "vis.h"
#include "dammit.h"

#include "vu.h"


char	szAppName[]	=	"DAMMIT VU";

#define VU_NUM   2
#define VU_NUM_SHIFT 1
#define VU_NUM_MAX 1


//Blah blah blah
int lvu[VU_NUM];
int rvu[VU_NUM];
int cur;



// Our VU Meter
winampVisModule VU_mod =
{
	"VU from center",
	NULL,
	NULL,
	0,
	0,
	2,
	2,
	0,			//Number of Spectrum Analysis Channels
	2,			//Number of Waveform Analysis Channels
	{ 0, },		// spectrumData
	{ 0, },		// waveformData
	VU_Config,
	VU_Init,
	VU_Render, 
	VU_Quit
};

//
//		Configuration
//
void VU_Config(struct winampVisModule *mod)
{
	MessageBox( mod->hwndParent, 
				"DAMMIT Winamp Plugin\n"
				"Soco\n"
				"Computer Science House\n"
				"Rochester Institute of Technology\n",
				"DAMMIT",
				MB_OK);
}

//
//		Initialization
//
int VU_Init(struct winampVisModule *mod)
{

	//Setup and Register our Window Class
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = WndProc;				// our window procedure
	wc.hInstance = mod->hDllInstance;		// hInstance of DLL
	wc.lpszClassName = szAppName;			// our window class name
	
	if (!RegisterClass(&wc)) 
	{
		MessageBox( mod->hwndParent,"Error registering window class","blah",MB_OK);
		return 1;
	}


	_outp( LPTPort, 0 );
	cur = 0;

	return 0;
}


// render function for VU meter. Returns 0 if successful, 1 if visualization should end.
int VU_Render(struct winampVisModule *mod)
{
	int x;
	
	int tmp;

	int tr,tl;
	int left, right;

	
	int last=mod->waveformData[0][0];
	int total=0;
	for (x = 1; x < 576; x++)
	{
		total += abs(last - mod->waveformData[0][x]);
		last = mod->waveformData[0][x];
	}
	total /= 1728;
	if (total > 127) total=127;
	lvu[cur]=total;

	total=0;
	last=mod->waveformData[1][0];	
	for (x = 1; x < 576; x ++)
	{
		total += abs(last - mod->waveformData[1][x]);
		last = mod->waveformData[1][x];
	}
	total /= 1728;
	if (total > 127) total=127;
	rvu[cur]=total;

	if( cur == VU_NUM_MAX )
	{
		tl = 0;
		tr = 0;
		for( x = 0 ; x < VU_NUM_MAX; x++)
		{
			tl+=lvu[x];
			tr+=rvu[x];
		}
		tl>>=VU_NUM_SHIFT+1;
		tr>>=VU_NUM_SHIFT+1;

		switch( tl )
		{
			case 0: left = 0; break;
			case 1: left = 1; break;
			case 2: left = 3; break;
			case 3: left = 7; break;
			default: left = 7;
		}
		switch( tr )
		{
			case 0: right = 0; break;
			case 1: right = 4; break;
			case 2: right = 6; break;
			case 3: right = 7; break;
			default: right = 7;
		}

		tmp = 0x08 | (left << 4) | right ;

		_outp( LPTPort, tmp );
	}
	cur++;
	cur&=VU_NUM_MAX;

	return 0;
}

//
//		Quit 
//
void VU_Quit(struct winampVisModule *this_mod)
{
	//Cut off the lights
	_outp( LPTPort, 0 );
	//Unregister the window and class
	UnregisterClass(szAppName,this_mod->hDllInstance); // unregister window class
}


//
// window procedure for our window
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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