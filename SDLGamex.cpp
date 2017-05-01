#include "SDLGamex.h"

void SDLGameXSaveSDLGameModelFile(const char *filepath,const char* picfilepath,SDLGameModelFile* psmf,SDLGameModelFileFrameInfo* psmffi,Uint32 count)
{
	FILE* fp = fopen(filepath,"wb");
	
	FILE* picfp = fopen(picfilepath,"rb");
	fseek(picfp,0,SEEK_END);
	long picsize = ftell(picfp);
	rewind(picfp);
	Uint8* ppic = new Uint8[picsize];
	fread(ppic,picsize,1,picfp);
	fclose(picfp);
	psmf->picsize = picsize;
	fwrite(psmf,sizeof(SDLGameModelFile),1,fp);
	fwrite(ppic,picsize,1,fp);
	
	for(Uint32 i = 0;i < count; i++)
	{
		FILE *wavfp = fopen(psmffi[i].wavpath,"rb");
		fseek(wavfp,0,SEEK_END);
		long size = ftell(wavfp);
		rewind(wavfp);
		Uint8* pwav = new Uint8[size];
		fread(pwav,size,1,wavfp);
		fclose(wavfp);
		psmffi[i].mf.wavsize = size;
		psmffi[i].mf.used = true;
		fwrite(&psmffi[i].mf,sizeof(ModelFrame),1,fp);
		fwrite(psmffi[i].parr,psmffi[i].mf.arrsize,1,fp);
		fwrite(pwav,size,1,fp);
		delete pwav;
	}
	ModelFrame mf;
	mf.used = false;
	fwrite(&mf,sizeof(ModelFrame),1,fp);
	fclose(fp);
}

void SDLGameXSaveScrollModelFile(const char *filepath,const char *picpath,ScrollModelFile *psmf)
{
	FILE* fpic = fopen(picpath,"rb");
	fseek(fpic,0,SEEK_END);
	int size = ftell(fpic);
	rewind(fpic);
	Uint8 *pic = new Uint8[size];
	fread(pic,size,1,fpic);
	fclose(fpic);
	FILE* fp = fopen(filepath,"wb");
	psmf->picsize = size;
	fwrite(psmf,sizeof(ScrollModelFile),1,fp);
	fwrite(pic,size,1,fp);
	fclose(fp);
	
}
