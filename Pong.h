#ifndef __PONG_H__
#define __PONG_H__

extern winampVisModule Pong_mod;
extern void Pong_Config(struct winampVisModule *mod);
extern int Pong_Init(struct winampVisModule *mod);
extern int Pong_Render(struct winampVisModule *mod);
void Pong_Quit(struct winampVisModule *mod);
LRESULT CALLBACK Pong_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif