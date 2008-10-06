// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// DAMMIT Winamp Plugin v0.7
//  Jon Parise		(jon@csh.rit.edu)
//	soco			(soco@csh.rit.edu)
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <windows.h>
#include <conio.h>
#include "vis.h"
#include "dammit.h"

#include "Random.h"
#include "Pong.h"
#include "Cross.h"
#include "Chase.h"
#include "vu.h"
#include "SpecAnal.h"


winampVisHeader hdr = { VIS_HDRVER, 
			"Computer Science House DAMMIT v0.7", 
			getModule };


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) winampVisHeader *winampVisGetHeader()
{
	return &hdr;
}
#ifdef __cplusplus
}
#endif


// Returns a pointer to the plugin module
winampVisModule *getModule(int which)
{
	switch (which) {
		case 0: return &Random_mod;		// Random.cpp
		case 1: return &Pong_mod;		// Pong.cpp
		case 2: return &Cross_mod;		// Cross.cpp
		case 3: return &Chase_mod;		// Chase.cpp
		case 4: return &VU_mod;			// Vu.cpp
		case 5: return &SpecAnal_mod;	// SpecAnal.cpp
		default:return NULL;			// Invalid Module
	}
}