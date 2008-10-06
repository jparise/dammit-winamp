#ifndef __VU_H__
#define __VU_H__
extern winampVisModule VU_mod;
extern void VU_Config( struct winampVisModule *);
extern int VU_Init( struct winampVisModule *mod);
extern int VU_Render( struct winampVisModule *mod );
extern void VU_Quit( struct winampVisModule *mod );
extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif