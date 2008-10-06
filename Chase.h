#ifndef __CHASE_H__
#define __CHASE_H__

extern winampVisModule Chase_mod;
extern void Chase_Config(struct winampVisModule *mod);
extern int Chase_Init(struct winampVisModule *mod);
extern int Chase_Render(struct winampVisModule *mod);
void Chase_Quit(struct winampVisModule *mod);
LRESULT CALLBACK Chase_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif