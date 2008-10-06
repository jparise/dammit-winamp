#ifndef __CROSS_H__
#define __CROSS_H__

extern winampVisModule Cross_mod;
extern void Cross_Config(struct winampVisModule *mod);
extern int Cross_Init(struct winampVisModule *mod);
extern int Cross_Render(struct winampVisModule *mod);
void Cross_Quit(struct winampVisModule *mod);
LRESULT CALLBACK Cross_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif