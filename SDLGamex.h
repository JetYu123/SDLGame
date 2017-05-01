#ifndef __SDLGAMEX__H__
#define __SDLGAMEX__H__

#include "SDLGame.h"

struct SDLGameModelFileFrameInfo
{
	char wavpath[512];
	Uint32* parr;
	ModelFrame mf;
};

void SDLGameXSaveSDLGameModelFile(const char *filepath,const char* picfilepath,SDLGameModelFile* psmf,SDLGameModelFileFrameInfo* smffi,Uint32 count);
void SDLGameXSaveScrollModelFile(const char *filepath,const char *picpath,ScrollModelFile *psmf);






#endif
