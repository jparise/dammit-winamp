#ifndef __RANDOM_H__
#define __RANDOM_H__

extern winampVisModule Random_mod;
extern void Random_Config(struct winampVisModule *mod);
extern int Random_Init(struct winampVisModule *mod);
extern int Random_Render(struct winampVisModule *mod);
extern void Random_Quit( struct winampVisModule *mod );
LRESULT CALLBACK Random_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif