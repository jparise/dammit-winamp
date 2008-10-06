#ifndef __DAMMIT_H__
#define __DAMMIT_H__

#define LPTPort		0x378

extern "C" __declspec( dllexport ) winampVisHeader *winampVisGetHeader();
extern winampVisModule *getModule(int);



#endif