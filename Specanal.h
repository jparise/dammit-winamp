#ifndef __SpecAnal_H__
#define __SpecAnal_H__

extern winampVisModule SpecAnal_mod;
extern void SpecAnal_Config(struct winampVisModule *mod);
extern int SpecAnal_Init(struct winampVisModule *mod);
extern int SpecAnal_Render(struct winampVisModule *mod);
extern void SpecAnal_Quit( struct winampVisModule *mod );
LRESULT CALLBACK SpecAnal_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif