#include "SDLGame.h"

#ifdef _DEBUG
#include <windows.h>
#endif

#define SDLGAMEVERHIWORD 2
#define SDLGAMEVERLOWORD 2


void SDLGameGetInfo(SDLGameVerInfo& ver)
{
	ver.hiword = SDLGAMEVERHIWORD;
	ver.loword = SDLGAMEVERLOWORD;
	strcpy(ver.infostr,"made by YT vc6 x86 release");
}

int SDLGameImageBase::m_imgusecount = 0;
int SDLGameEffectSoundBase::num_mix = 0;
int SDLGameFontBase::num_font = 0;

SDLGame::SDLGame(bool ua,int width,int height,bool busesync)
{
	m_screen = NULL;
	m_gameover = false;
	m_fps = 30;
	m_renderer = NULL;
	m_bkpictex = NULL;
	m_usebkpicscale = false;
	memset(&m_bkpicrect,0,sizeof(SDL_Rect));
	memset(&m_bkscrrect,0,sizeof(SDL_Rect));
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_VIDEO);
	m_window = SDL_CreateWindow("SDL",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_HIDDEN);
	m_renderer = SDL_CreateRenderer(m_window,-1,(busesync ? SDL_RENDERER_PRESENTVSYNC : 0) | (ua ? SDL_RENDERER_ACCELERATED : SDL_RENDERER_SOFTWARE));
		
	m_screen = SDL_GetWindowSurface(m_window);

	m_pkeys = NULL;
	m_rp = NULL;
	m_kbdp = NULL;
	m_kbup = NULL;
	m_mmp = NULL;
	m_mbdp = NULL;
	m_mbup = NULL;
	m_qp = NULL;
	m_mwp = NULL;
	m_dp = NULL;
	m_wp = NULL;
	m_swmp = NULL;
	m_cr = 0;
	m_cg = 0;
	m_cb = 0;
}

SDLGame::~SDLGame(void)
{
	if(m_renderer)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = NULL;
	}
	if(m_bkpictex)
	{
		SDL_DestroyTexture(m_bkpictex);
		m_bkpictex = NULL;
	}
	SDL_DestroyWindow(m_window);
	m_window = NULL;
	SDL_Quit();
}

void SDLGame::DisableScreenSaver(void)const 
{
	SDL_DisableScreenSaver();
}

void SDLGame::EnableScreenSaver(void)const
{
	SDL_EnableScreenSaver();
}

void SDLGame::UpdateScreen(void)const
{
	SDL_RenderPresent(m_renderer);
}

void SDLGame::MainLoop(void)
{
	while(true)
	{
		SDL_PollEvent(&m_evt);
		switch(m_evt.type)
		{
		case SDL_QUIT:
			OnQuit(m_evt.quit);
			break;
		case SDL_KEYDOWN:
			OnKeyDown(m_evt.key);
			break;
		case SDL_KEYUP:
			OnKeyUp(m_evt.key);
			break;
		case SDL_MOUSEMOTION:
			OnMouseMotion(m_evt.motion);
			break;
		case SDL_MOUSEBUTTONDOWN:
			OnMouseButtonDown(m_evt.button);
			break;
		case SDL_MOUSEBUTTONUP:
			OnMouseButtonUp(m_evt.button);
			break;
		case SDL_MOUSEWHEEL:
			OnMouseWheel(m_evt.wheel);
			break;
		case SDL_DROPFILE:
			OnDrop(m_evt.drop);
			break;
		case SDL_WINDOWEVENT:
			OnWindow(m_evt.window);
			break;
		case SDL_SYSWMEVENT:
			OnSysWM(m_evt.syswm);
			break;
		default:
			break;
		}
		OnRender();
		if(m_gameover)
			break;
	}
}

void SDLGame::OnRender(void)
{
	static Uint32 time;
	if(SDL_GetTicks() - time > 1000/m_fps)
	{
		time = SDL_GetTicks();
		m_pkeys = SDL_GetKeyboardState(NULL);
		
		SDL_SetRenderDrawColor(m_renderer,m_cr,m_cg,m_cb,255);
		SDL_RenderClear(m_renderer);
		if(m_bkpictex)
			SDL_RenderCopy(m_renderer,m_bkpictex,&m_bkpicrect,NULL);
	
		if(m_rp)
			m_rp(this);
				
		for(vector<SDLGameUIBase*>::iterator it = m_ui.begin();
			it != m_ui.end(); it++)
			(*it)->Render(this);
		UpdateScreen();
	}
	
}


bool SDLGame::IsKeyDown(SDL_Scancode scancode)const 
{
	return (bool)m_pkeys[scancode];
}

void SDLGame::SetQuitProc(QuitProc qp)
{
	m_qp = qp;
}

void SDLGame::SetRenderProc(RenderProc rp)
{
	m_rp = rp;
}

void SDLGame::SetKeyboardProc(KeyBoardProc kbdp,KeyBoardProc kbup)
{
	m_kbdp = kbdp;
	m_kbup = kbup;
}

void SDLGame::SetMouseMotionProc(MouseMotionProc mmp)
{
	m_mmp = mmp;
}

void SDLGame::SetMouseButtonProc(MouseButtonProc mbdp,MouseButtonProc mbup)
{
	m_mbdp = mbdp;
	m_mbup = mbup;
}

void SDLGame::SetMouseWheelProc(MouseWheelProc mwp)
{
	m_mwp = mwp;
}

void SDLGame::SetDropProc(DropProc dp)
{
	m_dp = dp;
}

void SDLGame::SetWindowProc(WindowProc wp)
{
	m_wp = wp;
}

void SDLGame::SetSysWMProc(SysWMProc swmp)
{
	m_swmp = swmp;
}

void SDLGame::SetTitle(const char *title)const 
{
	SDL_SetWindowTitle(m_window,title);
}

void SDLGame::SetWindowSize(int width,int height)const 
{
	SDL_SetWindowSize(m_window,width,height);
}

void SDLGame::SetClearColor(Uint8 r,Uint8 g,Uint8 b)
{
	m_cr = r;
	m_cg = g;
	m_cb = b;
}

void SDLGame::SetBkPicture(const char* file)
{
	m_bkpictex = IMG_LoadTexture(m_renderer,file);
	SetBkPictureViewPort(0,0);
}


void SDLGame::SetBkPictureViewPort(const int x,const int y,const int w,const int h)
{
	m_bkpicrect.x = x;
	m_bkpicrect.y = y;
	if(w == -1)
	{
		GetWindowSize(m_bkpicrect.w,m_bkpicrect.h);
	}
	else
	{
		m_bkpicrect.w = w;
		m_bkpicrect.h = h;
	}
	m_bkscrrect.x = 0;
	m_bkscrrect.y = 0;
	GetWindowSize(m_bkscrrect.w,m_bkscrrect.h); 
}

void SDLGame::SetBlendMode(SDL_BlendMode bm)const 
{
	SDL_SetRenderDrawBlendMode(m_renderer,bm);
}


void SDLGame::SetTimer(string name,SDL_TimerCallback fn,Uint32 msec,void *param)
{
	m_timermap[name] = SDL_AddTimer(msec,fn,param);
}

void SDLGame::KillTimer(string name)
{
	SDL_RemoveTimer(m_timermap[name]);
	m_timermap.erase(name);
}


void SDLGame::ShowWindow(void)const 
{
	SDL_ShowWindow(m_window);
}

void SDLGame::HideWindow(void)const
{
	SDL_HideWindow(m_window);
}

void SDLGame::ShowCursor(void)const
{
	SDL_ShowCursor(1);
}

void SDLGame::HideCursor(void)const
{
	SDL_ShowCursor(0);
}

void SDLGame::GetWindowSize(int &w,int &h)const 
{
	SDL_GetWindowSize(m_window,&w,&h);
}

int SDLGame::ShowMessageBox(SDL_MessageBoxFlags flag,const char *msg,const char *title)const 
{
	return SDL_ShowSimpleMessageBox(flag,title,msg,m_window);
}

void SDLGame::InitSubSystem(Uint32 flags)const
{
	SDL_InitSubSystem(flags);
}

void SDLGame::SetWindowFullScreen(bool busedesktopsize)
{
	if(busedesktopsize)
		SDL_SetWindowFullscreen(m_window,SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(m_window,SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
}

void SDLGame::MinimizeWindow(void)const 
{
	SDL_MinimizeWindow(m_window);
}

void SDLGame::MaximizeWindow(void)const
{
	SDL_MaximizeWindow(m_window);
}

void SDLGame::RaiseWindow(void)const 
{
	SDL_RaiseWindow(m_window);
}

void SDLGame::AddUI(SDLGameUIBase *p)
{
	m_ui.push_back(p);
}

void SDLGame::DeleteUI(SDLGameUIBase *p)
{
	vector<SDLGameUIBase*>::iterator it = m_ui.begin();
	for(; it != m_ui.end(); it++)
		if(*it == p)
			break;
	if(it != m_ui.end())
		m_ui.erase(it);
}

void SDLGame::OnKeyDown(SDL_KeyboardEvent keybdevt)
{
	switch(keybdevt.keysym.sym)
	{
		case SDLK_ESCAPE:
			OnQuit(m_evt.quit);
			break;
		default:
			if(m_kbdp)
				m_kbdp(keybdevt);
			break;
	}
	
}

void SDLGame::OnKeyUp(SDL_KeyboardEvent keybdevt)
{
	if(m_kbup)
		m_kbup(keybdevt);
}

void SDLGame::OnMouseMotion(SDL_MouseMotionEvent mmevt)
{
	int posx,posy;
	for(vector<SDLGameUIBase*>::iterator it = m_ui.begin();
		it != m_ui.end(); it++)
	{
		SDLGameUIBase *p = *it;
		if(!p->m_visible)
			continue;
		p->m_mousex = mmevt.x;
		p->m_mousey = mmevt.y;
		p->GetUIPos(posx,posy);
		if(mmevt.x > posx && mmevt.x < posx + p->m_uicx &&
			mmevt.y > posy && mmevt.y < posy + p->m_uicy)
			p->OnMouseEnter();
		else
			p->OnMouseLeave();
	}
	
	if(m_mmp)
		m_mmp(mmevt);
}

void SDLGame::OnMouseButtonDown(SDL_MouseButtonEvent mbevt)
{
	int posx,posy;
	vector<SDLGameUIBase*> ui(m_ui);
	for(vector<SDLGameUIBase*>::iterator it = ui.begin();
		it != ui.end(); it++)
	{
		SDLGameUIBase *p = *it;
		if(!p->m_visible)
			continue;
		p->m_mousex = mbevt.x;
		p->m_mousey = mbevt.y;
		p->GetUIPos(posx,posy);
		if(mbevt.x > posx && mbevt.x < posx + p->m_uicx &&
			mbevt.y > posy && mbevt.y < posy + p->m_uicy)
			p->OnMouseButtonDown();
	}
	
	if(m_mbdp)
		m_mbdp(mbevt);
}

void SDLGame::OnMouseButtonUp(SDL_MouseButtonEvent mbevt)
{
	int posx,posy;
	for(vector<SDLGameUIBase*>::iterator it = m_ui.begin();
		it != m_ui.end(); it++)
	{
		SDLGameUIBase *p = *it;
		if(!p->m_visible)
			continue;
		p->m_mousex = mbevt.x;
		p->m_mousey = mbevt.y;
		p->GetUIPos(posx,posy);
		if(mbevt.x > posx && mbevt.x < posx + p->m_uicx &&
			mbevt.y > posy && mbevt.y < posy + p->m_uicy)
			p->OnMouseButtonUp();
	}
	if(m_mbup)
		m_mbup(mbevt);
}

void SDLGame::OnMouseWheel(SDL_MouseWheelEvent mwevt)
{
	if(m_mwp)
		m_mwp(mwevt);
}

void SDLGame::OnDrop(SDL_DropEvent devt)
{
	if(m_dp)
		m_dp(devt);
}

void SDLGame::OnWindow(SDL_WindowEvent wevt)
{
	if(m_wp)
		m_wp(wevt);
}

void SDLGame::OnSysWM(SDL_SysWMEvent swmevt)
{
	if(m_swmp)
		m_swmp(swmevt);
}

void SDLGame::OnQuit(SDL_QuitEvent quitevt)
{
	if(m_qp)
		m_gameover = m_qp(quitevt);
	else
		m_gameover = true;
}

void SDLGame::SetFps(Uint32 fps)
{
	m_fps = fps;
}

Uint32 SDLGame::GetFps(void)const 
{
	return m_fps;
}




UIntArray::UIntArray(const UIntArray& o)
{
	if(o.m_size == 0)
	{
		m_size = 0;
		m_parray = NULL;
		return;
	}
	m_size = o.m_size;
	m_parray = new Uint32[m_size];
	memcpy(m_parray,o.m_parray,sizeof(Uint32) * m_size);
}

UIntArray::UIntArray(void)
{
	m_size = 0;
	m_parray = NULL;
}

UIntArray::~UIntArray(void)
{
	if(m_parray)
		delete[] m_parray;
}

Uint32* UIntArray::GetPointer(void)const
{
	return m_parray;
}

Uint32 UIntArray::GetSize(void)const 
{
	return m_size;
}

void UIntArray::CopyFromUIntArray(Uint32 size,Uint32* p)
{
	if(size == m_size)
		memcpy(m_parray, p, sizeof(Uint32) * m_size);
	else
	{
		if(m_parray)
			delete [] m_parray;
		m_size = size;
		m_parray = new Uint32[size];
		memcpy(m_parray, p, sizeof(Uint32) * size);
	}
}


SDLGameUIBase::SDLGameUIBase(void)
{
	m_uicx = 0;
	m_uicy = 0;
	m_uistate = UIS_NONE;
	m_visible = true;
	m_mep = NULL;
	m_mlp = NULL;
	m_mbcp = NULL;
	m_x = 0;
	m_y = 0;
}

SDLGameUIBase::~SDLGameUIBase(void)
{
	
}


void SDLGameUIBase::SetMouseEnterProc(MouseEnterProc mep)
{
	m_mep = mep;
}

void SDLGameUIBase::SetMouseLeaveProc(MouseLeaveProc mlp)
{
	m_mlp = mlp;
}

void SDLGameUIBase::SetMouseButtonClickProc(MouseButtonClickProc mbcp)
{
	m_mbcp = mbcp;
}

void SDLGameUIBase::OnMouseEnter(void)
{
	if(m_uistate & UIS_MOUSEENTER)
		return;
	m_uistate |= UIS_MOUSEENTER;
	if(m_mep)
		m_mep(this);
}

void SDLGameUIBase::OnMouseLeave(void)
{
	if(!(m_uistate & UIS_MOUSEENTER))
		return;
	m_uistate &= ~UIS_MOUSEENTER;
	m_uistate &= ~UIS_MOUSELBUTTONDOWN;
	if(m_mlp)
		m_mlp(this);
}

void SDLGameUIBase::OnMouseButtonDown(void)
{
	if(m_uistate & UIS_MOUSELBUTTONDOWN)
		return;
	m_uistate |= UIS_MOUSELBUTTONDOWN;
	if(m_mbcp)
		m_mbcp(this);
}

void SDLGameUIBase::OnMouseButtonUp(void)
{
	m_uistate &= ~UIS_MOUSELBUTTONDOWN;
}

void SDLGameUIBase::Visible(bool visible)
{
	m_visible  = visible;
}


void SDLGameUIBase::SetUIPos(int x,int y)
{
	m_x = x;
	m_y = y;
}

void SDLGameUIBase::GetUIPos(int& x,int& y)const
{
	x = m_x;
	y = m_y;
}




SDLGameFileBuffer::SDLGameFileBuffer(const char *filepath)
{
	m_nofree = false;
	m_p = NULL;
	m_size = 0;
	FILE *fp = fopen(filepath,"rb");
	fseek(fp,0,SEEK_END);
    m_size = ftell(fp);
    m_p = new Uint8[m_size];
    rewind(fp);
    fread(m_p,m_size,1,fp);
    fclose(fp);
}

SDLGameFileBuffer::SDLGameFileBuffer(Uint8* pdata,int size)
{
	m_nofree = true;
	m_p = pdata;
	m_size = size;
}

SDLGameFileBuffer::~SDLGameFileBuffer(void)
{
	if(m_nofree)
		return;
	if(m_p)
		delete m_p;
}

const void* SDLGameFileBuffer::GetPointer(void)const 
{
	return m_p;
}

int SDLGameFileBuffer::GetSize(void)const 
{
	return m_size;
}



SDLGameImageBase::SDLGameImageBase(void)
{
	if(!m_imgusecount)
		IMG_Init(IMG_INIT_TIF | IMG_INIT_PNG | IMG_INIT_JPG);
	m_imgusecount++;
}

SDLGameImageBase::~SDLGameImageBase(void)
{
	m_imgusecount--;
	if(!m_imgusecount)
		IMG_Quit();
}




SDLGameImage::SDLGameImage(SDLGame *p,const char *filepath,bool usecolorkey,Uint8 r,Uint8 g,Uint8 b)
{
	m_texture = NULL;

	m_angle = 0.0;
	m_flip = SDL_FLIP_NONE;
	memset(&m_point,0,sizeof(SDL_Point));
	memset(&m_srcrect,0,sizeof(SDL_Rect));
	memset(&m_dstrect,0,sizeof(SDL_Rect));
	if(usecolorkey)
	{
		SDL_Surface *temp = IMG_Load(filepath);
		SDL_Surface* surface = SDL_ConvertSurface(temp,p->m_screen->format,p->m_screen->flags);
		SDL_FreeSurface(temp);
		SDL_SetColorKey(surface,SDL_TRUE,SDL_MapRGB(surface->format,r,g,b));
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		m_cx = surface->w;
		m_cy = surface->h;
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture(p->m_renderer,filepath);
		SDL_QueryTexture(m_texture,NULL,NULL,&m_cx,&m_cy);
	}
	
	m_srcrect.w = m_cx;
	m_srcrect.h = m_cy;
	m_dstrect.w = m_cx;
	m_dstrect.h = m_cy;
}

SDLGameImage::SDLGameImage(SDLGame *p,SDLGameFileBuffer *pfb,bool usecolorkey,Uint8 r,Uint8 g,Uint8 b)
{
	SDL_RWops *rwops = SDL_RWFromConstMem(pfb->GetPointer(),pfb->GetSize());
	m_texture = NULL;
	
	m_angle = 0.0;
	m_flip = SDL_FLIP_NONE;
	memset(&m_point,0,sizeof(SDL_Point));
	memset(&m_srcrect,0,sizeof(SDL_Rect));
	memset(&m_dstrect,0,sizeof(SDL_Rect));
	
	if(usecolorkey)
	{
		SDL_Surface *temp = IMG_Load_RW(rwops,0);
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,p->m_screen->flags);
		SDL_FreeSurface(temp);
		SDL_SetColorKey(surface,SDL_TRUE,SDL_MapRGB(surface->format,r,g,b));
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		m_cx = surface->w;
		m_cy = surface->h;
		SDL_FreeSurface(surface);
		
	}
	else
	{
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
		SDL_QueryTexture(m_texture,NULL,NULL,&m_cx,&m_cy);
	}
	SDL_FreeRW(rwops);
	m_srcrect.w = m_cx;
	m_srcrect.h = m_cy;
	m_dstrect.w = m_cx;
	m_dstrect.h = m_cy;
}

SDLGameImage::SDLGameImage(void)
{
	m_texture = NULL;
	m_cx = 0;
	m_cy = 0;
	m_angle = 0.0;
	m_flip = SDL_FLIP_NONE;
	memset(&m_point,0,sizeof(SDL_Point));
	memset(&m_srcrect,0,sizeof(SDL_Rect));
	memset(&m_dstrect,0,sizeof(SDL_Rect));
}

SDLGameImage::~SDLGameImage(void)
{
	if(m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}
}

void SDLGameImage::SetPos(int x,int y)
{
	m_dstrect.x = x;
	m_dstrect.y = y;
}

void SDLGameImage::GetPos(int& x,int& y)const
{
	x = m_dstrect.x;
	y = m_dstrect.y;
}

void SDLGameImage::GetSize(int& cx,int& cy)const 
{
	cx = m_cx;
	cy = m_cy;
}

void SDLGameImage::SetAngle(double angle,int centerx,int centery)
{
	m_angle = angle;
	m_point.x = centerx;
	m_point.y = centery;
}


void SDLGameImage::SetFlip(SDL_RendererFlip flip)
{
	m_flip = flip;
}

void SDLGameImage::SetSrcRect(int x,int y,int w,int h)
{
	m_srcrect.x = x;
	m_srcrect.y = y;
	if(w + h == -2)
		return;
	m_srcrect.w = w;
	m_srcrect.h = h;
}

void SDLGameImage::SetDstRect(int x,int y,int w,int h)
{
	m_dstrect.x = x;
	m_dstrect.y = y;
	if(w + h == -2)
	{
		m_dstrect.w = m_cx;
		m_dstrect.h = m_cy;
		return;
	}
	m_dstrect.w = w;
	m_dstrect.h = h;
}

void SDLGameImage::Draw(SDLGame *p)
{
	if(m_point.x == -1)
		SDL_RenderCopyEx(p->m_renderer,m_texture,&m_srcrect,&m_dstrect,m_angle,NULL,m_flip);
	else
		SDL_RenderCopyEx(p->m_renderer,m_texture,&m_srcrect,&m_dstrect,m_angle,&m_point,m_flip);
}

void SDLGameImage::SetAlpha(Uint8 alpha)
{
	if(m_texture)
		SDL_SetTextureAlphaMod(m_texture,alpha);
}

Uint8 SDLGameImage::GetAlpha(void)const
{
	Uint8 alpha = 255;
	if(m_texture)
		SDL_GetTextureAlphaMod(m_texture,&alpha);
	return alpha;
}




SDLGameSprite::SDLGameSprite(SDLGame *p, const char* filepath,Uint32 spritecx,Uint32 spritecy,
	Uint32 x_num,Uint32 y_num,bool busecolorkey,
	Uint8 colorkeyr,Uint8 colorkeyg,Uint8 colorkeyb):
	m_busecolorkey(busecolorkey),m_colorkeyr(colorkeyr),
	m_colorkeyg(colorkeyg),m_colorkeyb(colorkeyb),
	m_spritecx(spritecx),m_spritecy(spritecy),m_alpha(0)
{
	SDL_Surface *temp = IMG_Load(filepath);
	SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,p->m_screen->flags);
	if(busecolorkey)
		SDL_SetColorKey(surface,SDL_TRUE,SDL_MapRGB(surface->format,colorkeyr,colorkeyg,colorkeyb));
	SDL_FreeSurface(temp);
	
	m_surfacecx = surface->w;
	m_surfacecy = surface->h;
	m_texture = NULL;
	m_nowframeindex = 0;
	m_fps = 5;
	m_x_num = x_num;
	m_y_num = y_num;
	m_spritex = 0;
	m_spritey = 0;
	m_time = 0;
	m_visible = true;
	
	
	if(!busecolorkey)
		m_texture = IMG_LoadTexture(p->m_renderer,filepath);
	else
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_FreeSurface(surface);
}

SDLGameSprite::SDLGameSprite(SDLGame *p,SDLGameFileBuffer *pfb,SDLGameSprite* psgs):
	m_busecolorkey(psgs->m_busecolorkey),m_colorkeyr(psgs->m_colorkeyr),
	m_colorkeyg(psgs->m_colorkeyg),m_colorkeyb(psgs->m_colorkeyb),
	m_spritecx(psgs->m_spritecx),m_spritecy(psgs->m_spritecy),m_alpha(0)
{
	SDL_RWops *rwops = SDL_RWFromConstMem(pfb->GetPointer(),pfb->GetSize());

	m_surfacecx = psgs->m_surfacecx;
	m_surfacecy = psgs->m_surfacecy;
	m_texture = NULL;
	m_nowframeindex = 0;
	m_fps = psgs->m_fps;
	m_x_num = psgs->m_x_num;
	m_y_num = psgs->m_y_num;
	m_spritex = psgs->m_spritex;
	m_spritey = psgs->m_spritey;
	m_time = 0;
	
	m_mapping = psgs->m_mapping;
	m_visible = psgs->m_visible;
	
	SDL_Surface *temp = IMG_Load_RW(rwops,0);
	SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,p->m_screen->flags);
	if(m_busecolorkey)
	{
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,m_colorkeyr,m_colorkeyg,m_colorkeyb));
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	}
	else
	{ 
		SDL_RWseek(rwops,0,RW_SEEK_SET);
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
	} 
	SDL_FreeSurface(temp);
	SDL_FreeSurface(surface);
	
	SDL_FreeRW(rwops);
	
}

SDLGameSprite::SDLGameSprite(SDLGame *p,SDLGameFileBuffer *pfb,Uint32 spritecx,Uint32 spritecy,
	Uint32 x_num,Uint32 y_num,bool busecolorkey,
	Uint8 colorkeyr,Uint8 colorkeyg,Uint8 colorkeyb):
	m_busecolorkey(busecolorkey),m_colorkeyr(colorkeyr),
	m_colorkeyg(colorkeyg),m_colorkeyb(colorkeyb),
	m_spritecx(spritecx),m_spritecy(spritecy),m_alpha(0)
{
	SDL_RWops *rwops = SDL_RWFromConstMem(pfb->GetPointer(),pfb->GetSize());
	SDL_Surface *temp = IMG_Load_RW(rwops,0);
	SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,p->m_screen->flags);
	if(busecolorkey)
		SDL_SetColorKey(surface,SDL_TRUE,SDL_MapRGB(surface->format,colorkeyr,colorkeyg,colorkeyb));
	
	SDL_FreeSurface(temp);
	
	m_surfacecx = surface->w;
	m_surfacecy = surface->h;
	m_texture = NULL;
	m_nowframeindex = 0;
	m_fps = 5;
	m_x_num = x_num;
	m_y_num = y_num;
	m_spritex = 0;
	m_spritey = 0;
	m_time = 0;
	m_visible = true;
	
	
	if(!busecolorkey)
	{ 
		SDL_RWseek(rwops,0,RW_SEEK_SET);
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
	} 
	else
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_FreeSurface(surface);
	
	SDL_FreeRW(rwops);
}

SDLGameSprite::~SDLGameSprite(void)
{
	if(m_texture)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
	}
	for(map<string,UIntArray*>::iterator it = m_mapping.begin();
		it != m_mapping.end(); it++)
	{
		delete it->second;
	}
}

void SDLGameSprite::SetGroup(string name,const UIntArray& ua)
{
	m_mapping[name] = new UIntArray(ua);
}

void SDLGameSprite::SetFps(Uint32 fps)
{
	m_fps = fps;
}

Uint32 SDLGameSprite::GetFps(void)const
{
	return m_fps;
}

void SDLGameSprite::SetAlpha(Uint8 alpha)
{
	m_alpha = alpha;
	if(m_texture)
		SDL_SetTextureAlphaMod(m_texture,alpha);
}

Uint8 SDLGameSprite::GetAlpha(void)
{
	if(m_texture)
		SDL_GetTextureAlphaMod(m_texture,&m_alpha);
	return m_alpha;
}

void SDLGameSprite::Visible(bool visible)
{
	m_visible = visible;
}

bool SDLGameSprite::IsNewFrame(void)
{
	return m_isnewframe;
}

void SDLGameSprite::DrawSubset(SDLGame* g,string subsetname,bool bloop)
{
	if(!m_visible)
		return;
	
	bool bredrew = false;
	if(SDL_GetTicks() - m_time < 1000 / m_fps)
		bredrew = true;
	else
		m_time = SDL_GetTicks();
		
	Uint32 _size,*parray;
	int sheetindex;
	SDL_Rect srcrect,dstrect;
	if(subsetname == m_nowplaysubset)
	{
		
		parray = m_mapping[subsetname]->GetPointer();
		_size = m_mapping[subsetname]->GetSize();
		
		m_isnewframe = !bredrew && _size != 1 && !m_nowframeindex;
		
		
		sheetindex = parray[m_nowframeindex];
		srcrect.x = m_spritecx * (sheetindex % m_x_num);
		srcrect.y = m_spritecy * (sheetindex / m_x_num);
		srcrect.w = m_spritecx;
		srcrect.h = m_spritecy;
		dstrect.x = m_spritex;
		dstrect.y = m_spritey;
		dstrect.w = m_spritecx;
		dstrect.h = m_spritecy;
		
		SDL_RenderCopy(g->m_renderer,m_texture,&srcrect,&dstrect);
		
		if(!bredrew) m_nowframeindex++;
		if(m_nowframeindex == _size)
			m_nowframeindex = bloop ? 0 : m_nowframeindex - 1;
		
	}
	else
	{
		m_nowplaysubset = subsetname;
		m_nowframeindex = 0;
		parray = m_mapping[subsetname]->GetPointer();
		_size = m_mapping[subsetname]->GetSize();

		sheetindex = parray[m_nowframeindex];
		
		srcrect.x = m_spritecx * (sheetindex % m_x_num);
		srcrect.y = m_spritecy * (sheetindex / m_x_num);
		srcrect.w = m_spritecx;
		srcrect.h = m_spritecy;
		
		dstrect.x = m_spritex;
		dstrect.y = m_spritey;
		dstrect.w = m_spritecx;
		dstrect.h = m_spritecy;
		
		SDL_RenderCopy(g->m_renderer,m_texture,&srcrect,&dstrect);
		
		if(_size == 1)
			m_nowframeindex = 0;
		else
			m_nowframeindex = 1;
			
		m_isnewframe = true;
	}
}

void SDLGameSprite::SetPos(int x,int y)
{
	m_spritex = x;
	m_spritey = y;
}

void SDLGameSprite::GetPos(int& x,int& y)const
{
	x = m_spritex;
	y = m_spritey;
}



SDLGameSpriteUIButton::SDLGameSpriteUIButton(SDLGame *p, const char *filepath, Uint32 UIcx, Uint32 UIcy,
	Uint32 x_num, Uint32 y_num, bool busecolorkey,
	Uint8 colorkeyr, Uint8 colorkeyg, Uint8 colorkeyb)
{
	SDLGameUIBase::m_uicx = UIcx;
	SDLGameUIBase::m_uicy = UIcy;
	m_sprite = new SDLGameSprite(p,filepath,UIcx,UIcy,x_num,y_num,
	busecolorkey,colorkeyr,colorkeyg,colorkeyb);
	m_loop = true;
}

SDLGameSpriteUIButton::~SDLGameSpriteUIButton(void)
{
	delete m_sprite;
}


void SDLGameSpriteUIButton::Render(SDLGame *p)
{
	if(!SDLGameUIBase::m_visible)
		return;
	if(SDLGameUIBase::m_uistate & UIS_MOUSELBUTTONDOWN)
		m_sprite->DrawSubset(p,"press",m_loop);
	else if(SDLGameUIBase::m_uistate & UIS_MOUSEENTER)
		m_sprite->DrawSubset(p,"enter",m_loop);
	else
		m_sprite->DrawSubset(p,"leave",m_loop);
	
}

void SDLGameSpriteUIButton::SetLoop(bool bloop)
{
	m_loop = bloop;
}


void SDLGameSpriteUIButton::SetPressGroup(const UIntArray& ua)
{
	m_sprite->SetGroup("press",ua);
}

void SDLGameSpriteUIButton::SetEnterGroup(const UIntArray& ua)
{
	m_sprite->SetGroup("enter",ua);
}

void SDLGameSpriteUIButton::SetLeaveGroup(const UIntArray& ua)
{
	m_sprite->SetGroup("leave",ua);
}

void SDLGameSpriteUIButton::SetFps(Uint32 fps)
{
	m_sprite->SetFps(fps);
}

Uint32 SDLGameSpriteUIButton::GetFps(void)const
{
	return m_sprite->GetFps();
}

void SDLGameSpriteUIButton::SetUIPos(int x,int y)
{
	m_sprite->SetPos(x,y);
	SDLGameUIBase::SetUIPos(x,y);
}

void SDLGameSpriteUIButton::GetUIPos(int& x,int& y)
{
	m_sprite->GetPos(x,y);
}

void SDLGameSpriteUIButton::Visible(bool visible)
{
	m_sprite->Visible(visible);
	SDLGameUIBase::Visible(visible);
}

void SDLGameSpriteUIButton::SetAlpha(Uint8 alpha)
{
	m_sprite->SetAlpha(alpha);
}




SDLGameFontBase::SDLGameFontBase(void)
{
	if(num_font == 0)
		TTF_Init();
	num_font++;
}

SDLGameFontBase::~SDLGameFontBase(void)
{
	num_font++;
	if(num_font == 0)
		TTF_Quit();
}



SDLGameFont::SDLGameFont(const char* file,int ptsize)
{
	m_font = TTF_OpenFont(file,ptsize);
}

SDLGameFont::~SDLGameFont(void)
{
	TTF_CloseFont(m_font);
}



SDLGameText::SDLGameText(SDLGameFont* font)
{
	m_font = font->m_font;
}

SDLGameText::~SDLGameText(void)
{
	
}


int SDLGameText::GetFontStyle(void)
{
	return TTF_GetFontStyle(m_font);
}

void SDLGameText::SetFontStyle(int style)
{
	TTF_SetFontStyle(m_font,style);
}

int SDLGameText::GetFontHinting(void)
{
	return TTF_GetFontHinting(m_font);
}

void SDLGameText::SetFontHinting(int hinting)
{
	TTF_SetFontHinting(m_font,hinting);
}

int SDLGameText::GetFontHeight(void)
{
	return TTF_FontHeight(m_font);
}

void SDLGameText::GetUTF8TextSize(const char *text,int& w,int& h)
{
	TTF_SizeUTF8(m_font,text,&w,&h);
}

void SDLGameText::GetUnicodeTextSize(const Uint16 *text,int& w,int& h)
{
	TTF_SizeUNICODE(m_font,text,&w,&h);
}

void SDLGameText::SetUTF8Text_Solid(SDLGame *p,const char *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUTF8_Solid(m_font,text,c);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);

	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}

void SDLGameText::SetUnicodeText_Solid(SDLGame *p,const Uint16 *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUNICODE_Solid(m_font,text,c);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);
	
	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}

void SDLGameText::SetUTF8Text_Shaded(SDLGame *p,const char *text,Uint8 fr,Uint8 fg,Uint8 fb,
	Uint8 br,Uint8 bg,Uint8 bb,Uint8 a)
{
	SDL_Color fc;
	fc.r = fr;
	fc.g = fg;
	fc.b = fb;
	SDL_Color bc;
	bc.r = br;
	bc.g = bg;
	bc.b = bb;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUTF8_Shaded(m_font,text,fc,bc);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);
	
	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}

void SDLGameText::SetUnicodeText_Shaded(SDLGame *p,const Uint16 *text,Uint8 fr,Uint8 fg,Uint8 fb,
	Uint8 br,Uint8 bg,Uint8 bb,Uint8 a)
{
	SDL_Color fc;
	fc.r = fr;
	fc.g = fg;
	fc.b = fb;
	SDL_Color bc;
	bc.r = br;
	bc.g = bg;
	bc.b = bb;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUNICODE_Shaded(m_font,text,fc,bc);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);
	
	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}

void SDLGameText::SetUTF8Text_Blended(SDLGame *p,const char *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUTF8_Blended(m_font,text,c);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);
	
	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}

void SDLGameText::SetUnicodeText_Blended(SDLGame *p,const Uint16 *text,Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	SDL_Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	if(m_texture)
		SDL_DestroyTexture(m_texture);
		
	SDL_Surface* surface = TTF_RenderUNICODE_Solid(m_font,text,c);
	m_cx = surface->w;
	m_cy = surface->h;
	
	m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
	SDL_SetTextureAlphaMod(m_texture,a);
	SDL_FreeSurface(surface);
	
	m_srcrect.h = m_cy;
	m_srcrect.w = m_cx;
	m_dstrect.h = m_cy;
	m_dstrect.w = m_cx;
}




SDLGameEffectSoundBase::SDLGameEffectSoundBase(int frequency,Uint16 format,int channels,int chunksize)
{
	if(num_mix == 0)
	{
		SDL_InitSubSystem(SDL_INIT_AUDIO);
		Mix_OpenAudio(frequency,format,channels,chunksize);
	}
	num_mix++;
}

SDLGameEffectSoundBase::~SDLGameEffectSoundBase(void)
{
	num_mix--;
	if(num_mix == 0)
	{
		Mix_CloseAudio();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}



SDLGameEffectSound::SDLGameEffectSound(int frequency,Uint16 format,int channels,int chunksize):
	SDLGameEffectSoundBase(frequency,format,channels,chunksize)
{
	
}

SDLGameEffectSound::~SDLGameEffectSound(void)
{
	for(map<string,Mix_Chunk*>::iterator it = m_chunkmap.begin();it != m_chunkmap.end(); it++)
		Mix_FreeChunk(it->second);
}

void SDLGameEffectSound::OpenWavFile(string name,const char *filepath)
{
	m_chunkmap[name] = Mix_LoadWAV(filepath);
}

void SDLGameEffectSound::OpenWavFile(string name,SDLGameFileBuffer *pfb)
{
	SDL_RWops *rwops = SDL_RWFromConstMem(pfb->GetPointer(),pfb->GetSize());
	m_chunkmap[name] = Mix_LoadWAV_RW(rwops,0);
	SDL_FreeRW(rwops);
}

void SDLGameEffectSound::PlayWav(string name,int loops,int timed)
{
	if(m_chunkmap.end() == m_chunkmap.find(name))
		return;
	Mix_PlayChannelTimed(-1,m_chunkmap[name],loops,timed);
}

void SDLGameEffectSound::SetVolume(Uint8 volume)const 
{
	Mix_Volume(-1,volume);
}

void SDLGameEffectSound::SetVolume(string name,Uint8 volume)
{
	Mix_VolumeChunk(m_chunkmap[name],volume);
}

Uint8 SDLGameEffectSound::GetVolume(string name)
{
	return Mix_VolumeChunk(m_chunkmap[name],-1);
}

void SDLGameEffectSound::CloseWavFile(string name)
{
	Mix_FreeChunk(m_chunkmap[name]);
}



SDLGameModel::SDLGameModel(SDLGame* p,const char *filepath)
{
	m_visible = true;
	m_es = new SDLGameEffectSound();
	FILE* fp = fopen(filepath,"rb");
	SDLGameModelFile smf;

	fread(&smf,sizeof(SDLGameModelFile),1,fp);
	Uint8* ppic = new Uint8[smf.picsize];
	fread(ppic,smf.picsize,1,fp);
	m_spritecy = smf.sprite_cy;
	m_spritecx = smf.sprite_cx;
	SDLGameFileBuffer fb(ppic,smf.picsize);
	m_sprite = new SDLGameSprite(p,&fb,smf.sprite_cx,smf.sprite_cy,
		smf.x_num,smf.y_num,smf.usecolorkey,smf.r,smf.g,smf.b);
	m_sprite->SetFps(smf.fps);
	ModelFrame mf;
	
	fread(&mf,sizeof(ModelFrame),1,fp);
	while(mf.used)
	{
		UIntArray arr;
		Uint32* parr = new Uint32[mf.arrsize / sizeof(Uint32)];
		fread(parr,mf.arrsize,1,fp);
		if(mf.wavsize)
		{
			Uint8* pwav = new Uint8[mf.wavsize];
			fread(pwav,mf.wavsize,1,fp);
			
			SDLGameFileBuffer wavfb(pwav,mf.wavsize);
			m_es->OpenWavFile(mf.name,&wavfb);
			
			delete pwav;
		}
		
		arr.CopyFromUIntArray(mf.arrsize / sizeof(Uint32),parr);
		
		m_sprite->SetGroup(mf.name,arr);
		m_arrdata.push_back(arr);
		
		delete parr;
		fread(&mf,sizeof(ModelFrame),1,fp);
	} 
}

SDLGameModel::SDLGameModel(SDLGame* p,SDLGameFileBuffer *pfb)
{
	const void* pfile = pfb->GetPointer();
	m_visible = true;
	m_es = new SDLGameEffectSound();
	SDLGameModelFile smf;
	memcpy(&smf,pfile,sizeof(SDLGameModelFile));
	pfile = (void*)((Uint32)pfile + sizeof(SDLGameModelFile));
	Uint8* ppic = new Uint8[smf.picsize];
	
	memcpy(ppic,pfile,smf.picsize);
	pfile = (void*)((Uint32)pfile + smf.picsize);
	m_spritecy = smf.sprite_cy;
	m_spritecx = smf.sprite_cx;
	SDLGameFileBuffer fb(ppic,smf.picsize);
	m_sprite = new SDLGameSprite(p,&fb,smf.sprite_cx,smf.sprite_cy,
		smf.x_num,smf.y_num,smf.usecolorkey,smf.r,smf.g,smf.b);
	m_sprite->SetFps(smf.fps);
	
	ModelFrame mf;
	
	memcpy(&mf,pfile,sizeof(ModelFrame));
	pfile = (void*)((Uint32)pfile + sizeof(ModelFrame));
	while(mf.used)
	{
		UIntArray arr;
		Uint32* parr = new Uint32[mf.arrsize / sizeof(Uint32)];
		memcpy(parr,pfile,mf.arrsize);
		pfile = (void*)((Uint32)pfile + mf.arrsize);
		if(mf.wavsize)
		{
			Uint8* pwav = new Uint8[mf.wavsize];
		
			memcpy(pwav,pfile,mf.wavsize);
			pfile = (void*)((Uint32)pfile + mf.wavsize);
			
			SDLGameFileBuffer wavfb(pwav,mf.wavsize);
			m_es->OpenWavFile(mf.name,&wavfb);
			delete pwav;
		}
		
		arr.CopyFromUIntArray(mf.arrsize / sizeof(Uint32),parr);
		
		m_sprite->SetGroup(mf.name,arr);
		m_arrdata.push_back(arr);
		
		delete parr;
		memcpy(&mf,pfile,sizeof(ModelFrame));
		pfile = (void*)((Uint32)pfile + sizeof(ModelFrame));
	} 
}

SDLGameModel::~SDLGameModel(void)
{
	if(m_sprite)
		delete m_sprite;
	if(m_es)
		delete m_es;
}

void SDLGameModel::Play(SDLGame* p,string name,bool bloop)
{
	m_sprite->DrawSubset(p,name,bloop);
	if(m_sprite->IsNewFrame())
		m_es->PlayWav(name);
}

void SDLGameModel::SetPos(int x,int y)
{
	m_sprite->SetPos(x,y);
}

void SDLGameModel::GetPos(int& x,int& y)
{
	m_sprite->GetPos(x,y);
}

void SDLGameModel::SetAlpha(Uint8 a)
{
	m_sprite->SetAlpha(a);
}

void SDLGameModel::Visible(bool visible)
{
	m_visible = visible;
	m_sprite->Visible(visible);
}

void SDLGameModel::GetSpriteWH(Uint32& cx,Uint32& cy)
{
	cx = m_spritecx;
	cy = m_spritecy;
}



SDLGameModelUIButton::SDLGameModelUIButton(SDLGame *p,const char *filepath)
{
	m_model = new SDLGameModel(p,filepath);

	m_model->GetSpriteWH(SDLGameUIBase::m_uicx,SDLGameUIBase::m_uicy);

	m_loop = true;
}

SDLGameModelUIButton::~SDLGameModelUIButton(void)
{
	delete m_model;
}

void SDLGameModelUIButton::Render(SDLGame *p)
{
	if(!SDLGameUIBase::m_visible)
		return;
	if(SDLGameUIBase::m_uistate & UIS_MOUSELBUTTONDOWN)
		m_model->Play(p,"press",m_loop);
	else if(SDLGameUIBase::m_uistate & UIS_MOUSEENTER)
		m_model->Play(p,"enter",m_loop);
	else
		m_model->Play(p,"leave",m_loop);
	
}

void SDLGameModelUIButton::Visible(bool visible)
{
	m_model->Visible(visible);
	SDLGameUIBase::Visible(visible);
}

void SDLGameModelUIButton::SetLoop(bool bloop)
{
	m_loop = bloop;
}

void SDLGameModelUIButton::SetUIPos(int x,int y)
{
	m_model->SetPos(x,y);
	SDLGameUIBase::SetUIPos(x,y);
}

void SDLGameModelUIButton::GetUIPos(int& x,int& y)
{
	m_model->GetPos(x,y);
}

void SDLGameModelUIButton::SetAlpha(Uint8 a)
{
	m_model->SetAlpha(a);
}




SDLGameUIScroll::SDLGameUIScroll(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,Uint32 sliderw,
	bool usecolorkey,Uint8 r /* = 0 */,Uint8 g /* = 0 */,Uint8 b /* = 0 */)
{
	m_uicx = trackw;
	m_uicy = h;
	m_cy = h;
	m_trackcx = trackw;
	m_slidercx = sliderw;
	m_pos = 0;
	if(usecolorkey)
	{
		SDL_Surface *temp = IMG_Load(filepath);
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,r,g,b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture(p->m_renderer,filepath);
	}
}

SDLGameUIScroll::SDLGameUIScroll(SDLGame* p,const char *UIfilepath)
{
	FILE *fp = fopen(UIfilepath,"rb");
	ScrollModelFile smf;
	fread(&smf,sizeof(ScrollModelFile),1,fp);
	Uint8 *pic = new Uint8[smf.picsize];
	fread(pic,smf.picsize,1,fp);
	fclose(fp);
	
	m_uicx = smf.trackcx;
	m_uicy = smf.cy;
	m_cy = smf.cy;
	m_trackcx = smf.trackcx;
	m_slidercx = smf.slidercx;
	m_pos = 0;
	SDL_RWops *rwops = SDL_RWFromConstMem(pic,smf.picsize);
	if(smf.usecolorkey)
	{
		SDL_Surface *temp = IMG_Load_RW(rwops,0);
		
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,smf.r,smf.g,smf.b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
	}
	SDL_FreeRW(rwops);
	delete pic;
}

SDLGameUIScroll::~SDLGameUIScroll(void)
{
	SDL_DestroyTexture(m_texture);
}

void SDLGameUIScroll::SetAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(m_texture,alpha);
}

void SDLGameUIScroll::SetPos(Uint32 pos)
{
	m_pos = pos;
}

Uint32 SDLGameUIScroll::GetPos(void)
{
	return m_pos;
}

void SDLGameUIScroll::Render(SDLGame* p)
{
	if(!m_visible)
		return;
	SDL_Rect srcrect,dstrect;
	if(m_uistate & UIS_MOUSELBUTTONDOWN)
	{
		int tmp = (m_mousex - m_x) * 10000 / m_trackcx;
		m_pos = tmp > 10000 ? 10000 : tmp;
	}
	if(!m_uistate)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = 0;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
	if(m_uistate & UIS_MOUSEENTER)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = m_cy;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
	if(m_uistate & UIS_MOUSELBUTTONDOWN)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = m_cy * 2;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
}

void SDLGameUIScroll::SetUIPos(int x,int y)
{
	m_x = x;
	m_y = y;
}



SDLGameUITrackBar::SDLGameUITrackBar(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,Uint32 sliderw,
	bool usecolorkey,Uint8 r /* = 0 */,Uint8 g /* = 0 */,Uint8 b /* = 0 */)
{
	m_uicx = trackw;
	m_uicy = h;
	m_cy = h;
	m_trackcx = trackw;
	m_slidercx = sliderw;
	m_pos = 0;
	if(usecolorkey)
	{
		SDL_Surface *temp = IMG_Load(filepath);
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,r,g,b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture(p->m_renderer,filepath);
	}
}

SDLGameUITrackBar::SDLGameUITrackBar(SDLGame* p,const char *UIfilepath)
{
	FILE *fp = fopen(UIfilepath,"rb");
	ScrollModelFile smf;
	fread(&smf,sizeof(ScrollModelFile),1,fp);
	Uint8 *pic = new Uint8[smf.picsize];
	fread(pic,smf.picsize,1,fp);
	fclose(fp);
	
	m_uicx = smf.trackcx;
	m_uicy = smf.cy;
	m_cy = smf.cy;
	m_trackcx = smf.trackcx;
	m_slidercx = smf.slidercx;
	m_pos = 0;
	SDL_RWops *rwops = SDL_RWFromConstMem(pic,smf.picsize);
	if(smf.usecolorkey)
	{
		SDL_Surface *temp = IMG_Load_RW(rwops,0);
		
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,smf.r,smf.g,smf.b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
	}
	SDL_FreeRW(rwops);
	delete pic;
}

SDLGameUITrackBar::~SDLGameUITrackBar(void)
{
	SDL_DestroyTexture(m_texture);
}

void SDLGameUITrackBar::SetAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(m_texture,alpha);
}

void SDLGameUITrackBar::SetPos(Uint32 pos)
{
	m_pos = pos;
}

Uint32 SDLGameUITrackBar::GetPos(void)
{
	return m_pos;
}

void SDLGameUITrackBar::SetUIPos(int x,int y)
{
	m_x = x;
	m_y = y;
}

void SDLGameUITrackBar::Render(SDLGame* p)
{
	if(!m_visible)
		return;
	SDL_Rect srcrect,dstrect;
	if(m_uistate & UIS_MOUSELBUTTONDOWN)
	{
		int tmp = (m_mousex - m_x) * 10000 / m_trackcx;
		m_pos = tmp > 10000 ? 10000 : tmp;
	}
	if(!m_uistate)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = 0;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_trackcx;
		dstrect.x = m_x;
		dstrect.w = m_pos * m_trackcx / 10000;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx * 2;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
	if(m_uistate & UIS_MOUSEENTER)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = m_cy;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_trackcx;
		dstrect.x = m_x;
		dstrect.w = m_pos * m_trackcx / 10000;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x += m_trackcx;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
	if(m_uistate & UIS_MOUSELBUTTONDOWN)
	{
		srcrect.h = m_cy;
		srcrect.w = m_trackcx;
		srcrect.x = 0;
		srcrect.y = m_cy * 2;
		dstrect.x = m_x;
		dstrect.y = m_y;
		dstrect.w = m_trackcx;
		dstrect.h = m_cy;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x = m_trackcx;
		srcrect.w = m_trackcx;
		dstrect.x = m_x;
		dstrect.w = m_pos * m_trackcx / 10000;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
		srcrect.x += m_trackcx;
		srcrect.w = m_slidercx;
		dstrect.x = -((int)m_slidercx / 2) + m_x + m_pos * m_trackcx / 10000;
		dstrect.w = m_slidercx;
		SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	}
}




SDLGameUIProcessBar::SDLGameUIProcessBar(SDLGame* p,const char *filepath,Uint32 h,Uint32 trackw,
	bool usecolorkey,Uint8 r /* = 0 */,Uint8 g /* = 0 */,Uint8 b /* = 0 */)
{
	m_uicx = trackw;
	m_uicy = h;
	m_cy = h;
	m_trackcx = trackw;
	m_pos = 0;
	if(usecolorkey)
	{
		SDL_Surface *temp = IMG_Load(filepath);
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,r,g,b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture(p->m_renderer,filepath);
	}
}

SDLGameUIProcessBar::SDLGameUIProcessBar(SDLGame* p,const char *UIfilepath)
{
	FILE *fp = fopen(UIfilepath,"rb");
	ScrollModelFile smf;
	fread(&smf,sizeof(ScrollModelFile),1,fp);
	Uint8 *pic = new Uint8[smf.picsize];
	fread(pic,smf.picsize,1,fp);
	fclose(fp);
	
	m_uicx = smf.trackcx;
	m_uicy = smf.cy;
	m_cy = smf.cy;
	m_trackcx = smf.trackcx;
	
	m_pos = 0;
	SDL_RWops *rwops = SDL_RWFromConstMem(pic,smf.picsize);
	if(smf.usecolorkey)
	{
		SDL_Surface *temp = IMG_Load_RW(rwops,0);
		
		SDL_Surface *surface = SDL_ConvertSurface(temp,p->m_screen->format,0);
		SDL_SetColorKey(surface,1,SDL_MapRGB(surface->format,smf.r,smf.g,smf.b));
		SDL_FreeSurface(temp);
		m_texture = SDL_CreateTextureFromSurface(p->m_renderer,surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		m_texture = IMG_LoadTexture_RW(p->m_renderer,rwops,0);
	}
	SDL_FreeRW(rwops);
	delete pic;
}

SDLGameUIProcessBar::~SDLGameUIProcessBar(void)
{
	SDL_DestroyTexture(m_texture);
}

void SDLGameUIProcessBar::SetAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(m_texture,alpha);
}

void SDLGameUIProcessBar::SetPos(Uint32 pos)
{
	m_pos = pos;
}

Uint32 SDLGameUIProcessBar::GetPos(void)
{
	return m_pos;
}

void SDLGameUIProcessBar::SetUIPos(int x,int y)
{
	m_x = x;
	m_y = y;
}

void SDLGameUIProcessBar::Render(SDLGame* p)
{
	if(!m_visible)
		return;
	SDL_Rect srcrect,dstrect;

	srcrect.h = m_cy;
	srcrect.w = m_trackcx;
	srcrect.x = 0;
	srcrect.y = 0;
	dstrect.x = m_x;
	dstrect.y = m_y;
	dstrect.w = m_trackcx;
	dstrect.h = m_cy;
	SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
	srcrect.x = m_trackcx;
	srcrect.w = m_trackcx;
	dstrect.x = m_x;
	dstrect.w = m_pos * m_trackcx / 10000;
	SDL_RenderCopy(p->m_renderer,m_texture,&srcrect,&dstrect);
}





Vector2f::Vector2f(float x,float y)
{
	m_x = x;
	m_y = y;
}

Vector2f::Vector2f(void)
{
	m_x = 0;
	m_y = 0;
}

Vector2f::Vector2f(const Vector2f& o)
{
	m_x = o.m_x;
	m_y = o.m_y;
}

void Vector2f::operator+=(Vector2f& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
}

void Vector2f::operator*=(float f)
{
	m_x *= f;
	m_y *= f;
}

void Vector2f::Set(float x,float y)
{
	m_x = x;
	m_y = y;
}

void Vector2f::Get(float& x,float& y)const
{
	x = m_x;
	y = m_y;
}

float Vector2f::GetLength(void)const
{
	return SDL_sqrt(m_x * m_x + m_y * m_y);
}



Particle::Particle(void)
{
	m_v.Set(0.0f,0.0f);
	m_pos.Set(0.0f,0.0f);
	m_visible = false;
	m_persenta = 1.0f;
	m_lifetimes = 120;
}

Particle::Particle(const Particle& o)
{
	m_v = o.m_v;
	m_pos = o.m_pos;
	m_persenta = o.m_persenta;
	m_visible = o.m_visible;
	m_lifetimes = o.m_lifetimes;
}

Particle::Particle(const Vector2f& pos,const Vector2f& v,float persenta,Uint32 lifetimes)
{
	m_pos = pos;
	m_v = v;
	m_persenta = persenta;
	m_lifetimes = lifetimes;
	m_visible = true;
}

void Particle::SetLifeTimes(Uint32 lifetimes)
{
	m_lifetimes = lifetimes;
}

void Particle::Update(void)
{
	if(!m_visible)
		return;
	m_v *= m_persenta;
	m_pos += m_v;
	if(--m_lifetimes == 0)
		m_visible = false;
}

void Particle::GetPos(float& x,float& y)const
{
	m_pos.Get(x,y);
}

void Particle::SetPos(float x,float y)
{
	m_pos.Set(x,y);
	m_visible = true;
}

void Particle::SetVelocity(float x,float y)
{
	m_v.Set(x,y);
}

void Particle::GetVelocity(float& x,float& y)const
{
	m_v.Get(x,y);
}

bool Particle::Visible(void)
{
	return m_visible;
}

void Particle::Visible(bool visible)
{
	m_visible = visible;
}




SDLGameParticleSet::SDLGameParticleSet(const Vector2f& pos,Uint32 count,
	Uint32 lifetimes,Uint32 persentrange,Uint32 pastyle)
{
	srand(SDL_GetTicks());
	m_r = 0;
	m_g = 0;
	m_b = 0;
	m_a = 255;
	m_ppoint = new SDL_Point[count];
	m_count = count;
	Reset(pos,lifetimes,persentrange,pastyle);
}

void SDLGameParticleSet::Reset(const Vector2f& pos,
	Uint32 lifetimes,Uint32 persentrange,Uint32 pastyle)
{
	m_particle.clear();
	float t = 360.0f / m_count;
	for(Uint32 i = 0;i < m_count; i++)
	{
		float persenta = 0.9f + (rand() % 10) / 100.0f;
		Vector2f v(persentrange * SDL_sinf(t * i),persentrange * SDL_cosf(t * i));
		Particle particle(pos,v,persenta);
		
		if(pastyle == PAS_LD)
			particle.SetLifeTimes((int)(lifetimes * persenta));
		if(pastyle == PAS_FD)
			particle.SetLifeTimes((int)(lifetimes / persenta));
		if(pastyle == PAS_RD)
			particle.SetLifeTimes((int)(lifetimes / 2 + rand() % lifetimes / 2));
		m_particle.push_back(particle);
	}
}

SDLGameParticleSet::~SDLGameParticleSet(void)
{
	vector<Particle> v;
	m_particle.swap(v);
	delete m_ppoint;
}

void SDLGameParticleSet::GetColor(Uint8& r,Uint8& g,Uint8& b,Uint8& a)const
{
	r = m_r;
	g = m_g;
	b = m_b;
	a = m_a;
}

void SDLGameParticleSet::SetColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}


void SDLGameParticleSet::Draw(SDLGame* p)
{
	float x,y;
	memset(m_ppoint,0,m_count * sizeof(SDL_Point));
	int posindex = 0;
	SDL_SetRenderDrawColor(p->m_renderer,m_r,m_g,m_b,m_a);
	
	for(int i = 0;i < m_particle.size();i++)
	{
		if(m_particle[i].Visible())
		{
			m_particle[i].GetPos(x,y);
			(m_ppoint + posindex)->x = (int)x;
			(m_ppoint + posindex++)->y = (int)y;
			m_particle[i].Update();
		}
	}
	SDL_RenderDrawPoints(p->m_renderer,m_ppoint,posindex);
}

bool SDLGameHC(SDLGameSprite* s1,SDLGameSprite* s2)
{
	if(s1->m_spritex >= s2->m_spritex &&
		s1->m_spritey >= s2->m_spritey &&
		s1->m_spritex <= s2->m_spritex + s2->m_spritecx &&
		s1->m_spritey <= s2->m_spritey + s2->m_spritecy)
		return true;
	if(s2->m_spritex >= s1->m_spritex &&
		s2->m_spritey >= s1->m_spritey &&
		s2->m_spritex <= s1->m_spritex + s1->m_spritecx &&
		s2->m_spritey <= s1->m_spritey + s1->m_spritecy)
		return true;
	return false;
}

bool SDLGameHC(SDLGameModel* gm1,SDLGameModel* gm2)
{
	return SDLGameHC(gm1->m_sprite,gm2->m_sprite);
}
