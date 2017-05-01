#ifndef __SDLGAME__H__
#define __SDLGAME__H__


#include "stdio.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "string"
#include "string.h"
#include "map"
#include "vector"

using namespace std;

class SDLGame;
class SDLGameUIBase;
class SDLGameUIScroll;
class SDLGameUITrackBar;
class SDLGameUIProcessBar;
class SDLGameFileBuffer;
class UIntArray;
class SDLGameImageBase;
class SDLGameImage;
class SDLGameSprite;
class SDLGameSpriteUIButton;
class SDLGameFontBase;
class SDLGameFont;
class SDLGameText;
class SDLGameEffectSoundBase;
class SDLGameEffectSound;
class SDLGameModel;
class SDLGameModelUIButton;

class Vector2f;
class Particle;
class SDLGameParticleSet;

//in SDLGame
typedef void (*RenderProc)(SDLGame*);
typedef void (*KeyBoardProc)(SDL_KeyboardEvent);
typedef bool (*QuitProc)(SDL_QuitEvent);
typedef void (*MouseMotionProc)(SDL_MouseMotionEvent);
typedef void (*MouseButtonProc)(SDL_MouseButtonEvent);
typedef void (*MouseWheelProc)(SDL_MouseWheelEvent);
typedef void (*DropProc)(SDL_DropEvent);
typedef void (*WindowProc)(SDL_WindowEvent);
typedef void (*SysWMProc)(SDL_SysWMEvent);

//in SDLGameSpriteUI
typedef void (*MouseEnterProc)(SDLGameUIBase*);
typedef void (*MouseLeaveProc)(SDLGameUIBase*);
typedef void (*MouseButtonClickProc)(SDLGameUIBase*);


#define	UIS_NONE 0
#define	UIS_MOUSEENTER 1
#define	UIS_MOUSELBUTTONDOWN 2

#define PAS_FD 0
#define PAS_LD 1
#define PAS_RD 2

#define SDLGAMEVERSIONHIWORD 2
#define SDLGAMEVERSIONLOWORD 2

struct SDLGameModelFile
{
	Uint8 r,g,b;
	bool usecolorkey;
	Uint32 x_num;
	Uint32 y_num;
	Uint32 sprite_cx;
	Uint32 sprite_cy;
	Uint32 picsize;
	Uint32 fps;
};

struct ModelFrame
{
	bool used;
	char name[64];
	Uint32 wavsize;
	Uint32 arrsize;
};

struct ScrollModelFile
{
	Uint8 r,g,b;
	bool usecolorkey;
	Uint32 trackcx;
	Uint32 cy;
	Uint32 slidercx;
	Uint32 picsize;
};

struct SDLGameVerInfo
{
	int hiword;
	int loword;
	char infostr[64];
};

void SDLGameGetInfo(SDLGameVerInfo&);


class SDLGameUIBase
{
public:
	SDLGameUIBase(void);
	virtual ~SDLGameUIBase(void);
	virtual void Render(SDLGame *p) = 0;
	virtual void SetUIPos(int,int);
	void SetMouseEnterProc(MouseEnterProc mep);
	void SetMouseLeaveProc(MouseLeaveProc mlp);
	void SetMouseButtonClickProc(MouseButtonClickProc mbcp);
	void Visible(bool visible);
	void GetUIPos(int& x,int& y)const;
private:
	void OnMouseEnter(void);
	void OnMouseLeave(void);
	void OnMouseButtonDown(void);
	void OnMouseButtonUp(void);
	
	MouseEnterProc m_mep;
	MouseLeaveProc m_mlp;
	MouseButtonClickProc m_mbcp;
protected:
	Uint32 m_uicx,m_uicy;
	int m_x,m_y;
	bool m_visible;
	Uint32 m_uistate;
	Uint32 m_mousex,m_mousey;
	friend class SDLGame;
};

class SDLGameImageBase
{
public:
	SDLGameImageBase(void);
	~SDLGameImageBase(void);
private:
	static int m_imgusecount;
};


class SDLGame:public SDLGameImageBase
{
public:
	SDLGame(bool ua = true,int width = 800,int height = 600,
	bool busevsync = true);
	~SDLGame(void);
	void MainLoop(void);
	void UpdateScreen(void)const;
	void AddUI(SDLGameUIBase* p);
	void DeleteUI(SDLGameUIBase* p);
	void OnQuit(SDL_QuitEvent);
	void OnRender(void);
	void SetTitle(const char*)const;
	void SetWindowSize(int,int)const;
	void GetWindowSize(int&,int&)const;
	void SetWindowFullScreen(bool busedesktopsize);
	void MinimizeWindow(void)const;
	void MaximizeWindow(void)const;
	void RaiseWindow(void)const;
	void SetClearColor(Uint8,Uint8,Uint8);
	void SetBkPicture(const char*);
	void SetBkPictureViewPort(const int x,const int y,const int w = -1,const int h = -1);
	void SetBlendMode(SDL_BlendMode)const;
	void SetTimer(string name,SDL_TimerCallback fn,Uint32 msec,void *param);
	void KillTimer(string name);
	void EnableScreenSaver(void)const;
	void DisableScreenSaver(void)const;
	void ShowWindow(void)const;
	void HideWindow(void)const;
	void ShowCursor(void)const;
	void HideCursor(void)const;
	int ShowMessageBox(SDL_MessageBoxFlags flag,const char *msg,const char *title)const;
	void InitSubSystem(Uint32 flags)const;
	void SetFps(Uint32);
	Uint32 GetFps(void)const;
	void SetQuitProc(QuitProc);
	void SetRenderProc(RenderProc);
	void SetKeyboardProc(KeyBoardProc,KeyBoardProc);
	void SetMouseMotionProc(MouseMotionProc);
	void SetMouseButtonProc(MouseButtonProc,MouseButtonProc);
	void SetMouseWheelProc(MouseWheelProc);
	void SetDropProc(DropProc);
	void SetWindowProc(WindowProc);
	void SetSysWMProc(SysWMProc);
	bool __fastcall IsKeyDown(SDL_Scancode scancode)const;
private:
	void OnKeyDown(SDL_KeyboardEvent);
	void OnKeyUp(SDL_KeyboardEvent);
	void OnMouseMotion(SDL_MouseMotionEvent);
	void OnMouseButtonDown(SDL_MouseButtonEvent);
	void OnMouseButtonUp(SDL_MouseButtonEvent);
	void OnMouseWheel(SDL_MouseWheelEvent);
	void OnDrop(SDL_DropEvent);
	void OnWindow(SDL_WindowEvent);
	void OnSysWM(SDL_SysWMEvent);
	vector<SDLGameUIBase*> m_ui;
	map<string,SDL_TimerID> m_timermap;
	SDL_Surface *m_screen;
	SDL_Renderer *m_renderer;
	SDL_Window *m_window;
	const Uint8 *m_pkeys;
	Uint8 m_cr,m_cg,m_cb;
	SDL_Event m_evt;
	bool m_gameover;
	Uint32 m_fps;
	RenderProc m_rp;
	KeyBoardProc m_kbdp,m_kbup;
	QuitProc m_qp;
	MouseMotionProc m_mmp;
	MouseButtonProc m_mbdp,m_mbup;
	MouseWheelProc m_mwp;
	DropProc m_dp;
	WindowProc m_wp;
	SysWMProc m_swmp;
	SDL_Texture *m_bkpictex;
	bool m_usebkpicscale;
	SDL_Rect m_bkpicrect;
	SDL_Rect m_bkscrrect;
	friend class SDLGameSprite;
	friend class SDLGameText;
	friend class SDLGameImage;
	friend class SDLGameParticleSet;
	friend class SDLGameUIScroll;
	friend class SDLGameUITrackBar;
	friend class SDLGameUIProcessBar;
};

class UIntArray
{
public:
	UIntArray(const UIntArray&);
	UIntArray(void);
	~UIntArray(void);
	Uint32* GetPointer(void)const;
	Uint32 GetSize(void)const;
	void CopyFromUIntArray(Uint32 size,Uint32* p); 
private:
	Uint32 *m_parray;
	Uint32 m_size;
};

class SDLGameFileBuffer
{
	Uint8 *m_p;
	int m_size;
	bool m_nofree;
public:
	SDLGameFileBuffer(const char *filepath);
	SDLGameFileBuffer(Uint8* pdata,int size);
	~SDLGameFileBuffer(void);
	const void* GetPointer(void)const;
	int GetSize(void)const;
};


class SDLGameImage
{
public:
	SDLGameImage(SDLGame*,SDLGameFileBuffer*,bool usecolorkey,
		Uint8 r = 0,Uint8 g = 0,Uint8 b = 0);
	SDLGameImage(SDLGame*,const char*,bool usecolorkey,
		Uint8 r = 0,Uint8 g = 0,Uint8 b = 0);
	~SDLGameImage(void);
	void Draw(SDLGame*);
	void SetPos(int x,int y);
	void GetSize(int& cx,int& cy)const;
	void GetPos(int& x,int& y)const;
	void SetAngle(double angle,int centerx = -1,int centery = -1);//used in texture render
	void SetSrcRect(int x,int y,int w = -1,int h = -1);
	void SetDstRect(int x,int y,int w = -1,int h = -1);
	void SetFlip(SDL_RendererFlip);//used in texture render
	void SetAlpha(Uint8);
	Uint8 GetAlpha(void)const;
protected:
	SDLGameImage(void);
	SDL_Texture *m_texture;
	SDL_Rect m_srcrect;
	SDL_Rect m_dstrect;
	SDL_Point m_point;
	int m_cx,m_cy;
	double m_angle;//used in texture render
	SDL_RendererFlip m_flip;//used in texture render
};

class SDLGameSprite
{
public:
	SDLGameSprite(SDLGame *p,const char* filepath,Uint32 spritecx,Uint32 spritecy,
	Uint32 x_num,Uint32 y_num,bool busecolorkey,
	Uint8 colorkeyr = 0,Uint8 colorkeyg = 0,Uint8 colorkeyb = 0);
	SDLGameSprite(SDLGame *p,SDLGameFileBuffer *pfb,Uint32 spritecx,Uint32 spritecy,
	Uint32 x_num,Uint32 y_num,bool busecolorkey,
	Uint8 colorkeyr = 0,Uint8 colorkeyg = 0,Uint8 colorkeyb = 0); 
	SDLGameSprite(SDLGame *p,SDLGameFileBuffer *pfb,SDLGameSprite *psgs);
	~SDLGameSprite(void);
	void DrawSubset(SDLGame* p,string name,bool bloop = true);
	void SetGroup(string,const UIntArray&);
	void SetPos(int,int);
	void GetPos(int&,int&)const;
	void SetFps(Uint32);
	Uint32 GetFps(void)const;
	void SetAlpha(Uint8);
	Uint8 GetAlpha(void);
	void Visible(bool);
	bool IsNewFrame(void);
private:
	SDL_Texture *m_texture;
	Uint32 m_fps;
	Uint8 m_alpha;
	const Uint32 m_spritecx,m_spritecy;
	int m_spritex,m_spritey;
	Uint32 m_surfacecx,m_surfacecy;
	Uint32 m_x_num,m_y_num;
	map<string,UIntArray*> m_mapping;
	string m_nowplaysubset;
	Uint32 m_nowframeindex;
	Uint32 m_time;
	bool m_visible;
	const bool m_busecolorkey;
	const Uint8 m_colorkeyr,m_colorkeyg,m_colorkeyb;
	bool m_isnewframe;
	friend bool SDLGameHC(SDLGameSprite* s1,SDLGameSprite* s2);
};

class SDLGameSpriteUIButton : public SDLGameUIBase
{
public:
	SDLGameSpriteUIButton(SDLGame *p, const char* filepath,Uint32 UIcx,Uint32 UIcy,
	Uint32 x_num,Uint32 y_num,bool busecolorkey,
	Uint8 colorkeyr = 0,Uint8 colorkeyg = 0,Uint8 colorkeyb = 0);
	~SDLGameSpriteUIButton(void);
	void SetPressGroup(const UIntArray& ua);
	void SetEnterGroup(const UIntArray& ua);
	void SetLeaveGroup(const UIntArray& ua);
	void Visible(bool visible);
	void SetFps(Uint32);
	Uint32 GetFps(void)const;
	void SetUIPos(int,int);
	void GetUIPos(int&,int&);
	void Render(SDLGame *p);
	void SetLoop(bool bloop);
	void SetAlpha(Uint8);
private:
	SDLGameSprite *m_sprite;
	bool m_loop;
};

class SDLGameFontBase
{
public:
	SDLGameFontBase(void);
	~SDLGameFontBase(void);
private:
	static int num_font;
};

class SDLGameFont : public SDLGameFontBase
{
public:
	SDLGameFont(const char* file,int ptsize);
	~SDLGameFont(void);
private:
	TTF_Font *m_font;
	friend class SDLGameText;
};

class SDLGameText : public SDLGameImage
{
public:
	SDLGameText(SDLGameFont*);
	~SDLGameText(void);
	int GetFontStyle(void);
	void SetFontStyle(int);
	int GetFontHinting(void);
	void SetFontHinting(int);
	int GetFontHeight(void);
	void GetUTF8TextSize(const char*,int& w,int& h);
	void GetUnicodeTextSize(const Uint16*,int& w,int& h);
	void SetUTF8Text_Solid(SDLGame*,const char*,
		Uint8 r,Uint8 g,Uint8 b,Uint8 a);
	void SetUnicodeText_Solid(SDLGame*,const Uint16*,
		Uint8 r,Uint8 g,Uint8 b,Uint8 a);
	void SetUTF8Text_Shaded(SDLGame*,const char*,
		Uint8 fr,Uint8 fg,Uint8 fb,
		Uint8 br,Uint8 bg,Uint8 bb,Uint8 a);
	void SetUnicodeText_Shaded(SDLGame*,const Uint16*,
		Uint8 fr,Uint8 fg,Uint8 fb,
		Uint8 br,Uint8 bg,Uint8 bb,Uint8 a);
	void SetUTF8Text_Blended(SDLGame*,const char*,
		Uint8 r,Uint8 g,Uint8 b,Uint8 a);
	void SetUnicodeText_Blended(SDLGame*,const Uint16*,
		Uint8 r,Uint8 g,Uint8 b,Uint8 a);
	#define SetText SetUTF8Text_Blended
private:
	TTF_Font* m_font;
};

class SDLGameEffectSoundBase
{
public:
	SDLGameEffectSoundBase(int frequency,Uint16 format,int channels,int chunksize);
	~SDLGameEffectSoundBase(void);
private:
	static int num_mix;
};

class SDLGameEffectSound : public SDLGameEffectSoundBase
{
public:
	SDLGameEffectSound(int frequency = MIX_DEFAULT_FREQUENCY,
		Uint16 format = AUDIO_S16MSB,int channels = MIX_DEFAULT_CHANNELS,
		int chunksize = 1024);
	~SDLGameEffectSound(void);
	void OpenWavFile(string name,const char *filepath);
	void OpenWavFile(string name,SDLGameFileBuffer* pfb);
	void PlayWav(string name,int loops = 0,int timed = -1);
	void SetVolume(Uint8 volume)const;
	void SetVolume(string name,Uint8 volume);
	Uint8 GetVolume(string name);
	void CloseWavFile(string name);
private:
	map<string,Mix_Chunk*> m_chunkmap;
};

class SDLGameModel
{
public:
	SDLGameModel(SDLGame* p,const char *filepath);//SDLGM
	SDLGameModel(SDLGame* p,SDLGameFileBuffer *pfb);
	~SDLGameModel(void);
	void Play(SDLGame* p,string name,bool bloop = true);
	void SetPos(int,int);
	void GetPos(int&,int&);
	void SetAlpha(Uint8);
	void GetSpriteWH(Uint32& cx,Uint32& cy);
	void Visible(bool visible);
private:
	bool m_visible;
	Uint32 m_spritecx;
	Uint32 m_spritecy;
	SDLGameSprite* m_sprite;
	SDLGameEffectSound* m_es;
	vector<UIntArray> m_arrdata;
	friend bool SDLGameHC(SDLGameModel* gm1,SDLGameModel* gm2);
};

class SDLGameModelUIButton : public SDLGameUIBase
{
public:
	SDLGameModelUIButton(SDLGame* p,const char *filepath);
	~SDLGameModelUIButton(void);
	void Render(SDLGame* p);
	void Visible(bool visible);
	void SetLoop(bool bloop);
	void SetUIPos(int x,int y);
	void GetUIPos(int& x,int& y);
	void SetAlpha(Uint8);
private:
	bool m_loop;
	SDLGameModel *m_model;
};

class SDLGameUIScroll : public SDLGameUIBase
{
public:
	SDLGameUIScroll(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,Uint32 sliderw,
		bool usecolorkey,Uint8 r = 0,Uint8 g = 0,Uint8 b = 0);
	SDLGameUIScroll(SDLGame* p,const char *UIfilepath);
	~SDLGameUIScroll(void);
	void Render(SDLGame* p);
	void SetUIPos(int x,int y);
	void SetPos(Uint32 pos);
	Uint32 GetPos(void);
	void SetAlpha(Uint8);
private:
	SDL_Texture *m_texture;
	Uint32 m_trackcx,m_cy;
	Uint32 m_slidercx;
	Uint32 m_pos;//0 ~ 10000
};

class SDLGameUITrackBar : public SDLGameUIBase
{
public:
	SDLGameUITrackBar(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,Uint32 sliderw,
		bool usecolorkey,Uint8 r = 0,Uint8 g = 0,Uint8 b = 0);
	SDLGameUITrackBar(SDLGame* p,const char *UIfilepath);
	~SDLGameUITrackBar(void);
	void Render(SDLGame* p);
	void SetUIPos(int x,int y);
	void SetPos(Uint32 pos);
	Uint32 GetPos(void);
	void SetAlpha(Uint8);
private:
	SDL_Texture *m_texture;
	Uint32 m_trackcx,m_cy;
	Uint32 m_slidercx;
	Uint32 m_pos;//0 ~ 10000
};

class SDLGameUIProcessBar : public SDLGameUIBase
{
public:
	SDLGameUIProcessBar(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,
		bool usecolorkey,Uint8 r = 0,Uint8 g = 0,Uint8 b = 0);
	SDLGameUIProcessBar(SDLGame* p,const char *UIfilepath);
	~SDLGameUIProcessBar(void);
	void Render(SDLGame* p);
	void SetUIPos(int x,int y);
	void SetPos(Uint32 pos);
	Uint32 GetPos(void);
	void SetAlpha(Uint8);
private:
	SDL_Texture *m_texture;
	Uint32 m_trackcx,m_cy;
	Uint32 m_pos;//0 ~ 10000
};

class Vector2f
{
public:
	Vector2f(const Vector2f&);
	Vector2f(void);
	Vector2f(float x,float y);
	void operator+=(Vector2f&);
	void operator*=(float);
	void Set(float x,float y);
	void Get(float& x,float& y)const;
	float GetLength(void)const;
private:
	float m_x,m_y;
};

class Particle
{
public:
	Particle(void);
	Particle(const Particle&);
	Particle(const Vector2f& pos,const Vector2f& v,float persenta,Uint32 lifetimes = 120);
	void Update(void);
	void GetPos(float& x,float& y)const;
	void SetPos(float x,float y);
	void SetVelocity(float x,float y);
	void GetVelocity(float& x,float& y)const;
	void SetLifeTimes(Uint32);
	bool Visible(void);
	void Visible(bool visible);
	
private:
	Vector2f m_pos;
	Vector2f m_v;
	float m_persenta;
	bool m_visible;
	Uint32 m_lifetimes;
};

class SDLGameParticleSet
{
public:
	SDLGameParticleSet(const Vector2f& pos,Uint32 count,
		Uint32 lifetimes = 120,Uint32 persentrange = 4,
		Uint32 pastyle = PAS_LD);
	void Reset(const Vector2f& pos,
		Uint32 lifetimes = 120,Uint32 persentrange = 4,
		Uint32 pastyle = PAS_LD);
	~SDLGameParticleSet(void);
	void Draw(SDLGame* p);
	void GetColor(Uint8& r,Uint8& g,Uint8& b,Uint8& a)const;
	void SetColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a);
private:
	Uint8 m_r,m_g,m_b,m_a;
	Uint32 m_count;
	SDL_Point* m_ppoint;
	vector<Particle> m_particle;
};

bool SDLGameHC(SDLGameSprite* s1,SDLGameSprite* s2);
bool SDLGameHC(SDLGameModel* gm1,SDLGameModel* gm2);

#endif
