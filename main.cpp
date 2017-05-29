#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

extern "C" {
//#ifdef BIT64
//#include"./sdl64/include/SDL.h"
//#include"./sdl64/include/SDL_main.h"
//#else
#include"./sdl/include/SDL.h"
#include"./sdl/include/SDL_main.h"
//#endif
}

#define SCREEN_WIDTH				840	//wymiary okna w pikselach
#define SCREEN_HEIGHT				720
#define SCREEN_BPP					32	//glebia obrazu, bity na piksel
#define GAME_WIDTH					576	//wymiary gry w pikselach
#define GAME_HEIGHT					720
#define WYM_KWADRATU				24	//piksele
#define CZAS_POZIOMU				200	//sekundy
#define ILOSC_POZIOMOW				7	//sekundy
#define PREDKOSC_SZYBKIEGO_OPADANIA	8	//pikseli na rendering
#define PREDKOSC_OPADANIA			1	//		(odswiezenie ekranu)
#define WSPOLCZYNNIK_UTRUDNIENIA	0.1
#define LICZBA_NAJLEPSZYCH			20
#define PREDKOSC_OBROTU	(double)	1	//obrotow na sekunde
#define CZAS_SPADANIA (double)		0.6	//sekundy (co jaki czas odbywa siê ruch klocka o jeden kwadrat w dol)
#define BIALY						0xFFFFFF
#define CZARNY						0x000000


enum rodzaje {
	zero,
	czerwony,
	siwy,
	blekitny,
	zolty,
	rozowy,
	niebieski,
	zielony
}rodzajKlocka;

struct hiscore {
	int lp;
	char imie[21];
	int iloscPunktow;
}tabHisc[LICZBA_NAJLEPSZYCH];

int max(int a, int b) {
	if (a > b)return a;
	else return b;
}

// -----------	funkcje rysujace	-----------------

// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawierajaca znaki
void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};

// narysowanie na ekranie screen prostok¹ta klocek w punkcie (x, y)
// (x, y) to punkt srodka prostokata sprite na ekranie
void DrawBlock(SDL_Surface *screen, SDL_Surface *klocki, rodzaje rodzajKlocka, SDL_Rect *klocek, SDL_Rect *dest, SDL_Rect *none, int x, int y, int degrees) {
	switch (rodzajKlocka) {
	case czerwony:
		if (degrees == 0 || degrees == 180) {
			klocek->x = 0;
			klocek->y = 0;
			klocek->h = 4*WYM_KWADRATU;
			klocek->w = 1*WYM_KWADRATU;
			dest->x = x - klocek->w/2;
			dest->y = y;
		}
		else if (degrees == 90 || degrees == 270) {
			klocek->x = 0;
			klocek->y = 120;
			klocek->h = 1*WYM_KWADRATU;
			klocek->w = 4*WYM_KWADRATU;
			dest->x = x - klocek->w+ 5*WYM_KWADRATU/2;
			dest->y = y;
		}
		
		break;

	case siwy:
		if (degrees == 0) {
			klocek->x = 48;
			klocek->y = 24;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
			dest->x = x - klocek->w/2;
		}
		else if (degrees == 90) {
			klocek->x = 120;
			klocek->y = 96;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
			dest->x = x - klocek->w/2 + WYM_KWADRATU/2;
		}
		else if (degrees == 180) {
			klocek->x = 0;
			klocek->y = 192;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
			dest->x = x - klocek->w/2;
		}
		else if (degrees == 270) {
			klocek->x = 240;
			klocek->y = 192;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
			dest->x = x - klocek->w/2 + WYM_KWADRATU/2;
		}
		break;

	case blekitny:
		klocek->x = 144;
		klocek->y = 24;
		klocek->h = 2*WYM_KWADRATU;
		klocek->w = 2*WYM_KWADRATU;
		dest->x = x - klocek->w/4;
		dest->y = y - klocek->h/2 + WYM_KWADRATU;
		break;

	case zolty:
		if (degrees == 0) {
			klocek->x = 216;
			klocek->y = 0;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/4;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		else if (degrees == 90) {
			klocek->x = 192;
			klocek->y = 96;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->x = x - klocek->w/3 - WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		else if (degrees == 180) {
			klocek->x = 96;
			klocek->y = 192;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/4;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		else if (degrees == 270) {
			klocek->x = 312;
			klocek->y = 192;
			klocek->h = 2 * WYM_KWADRATU;
			klocek->w = 3 * WYM_KWADRATU;
			dest->x = x - klocek->w/3 - WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		break;

	case rozowy:
		if (degrees == 0) {
			klocek->x = 288;
			klocek->y = 0;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/4;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		else if (degrees == 90) {
			klocek->x = 288;
			klocek->y = 96;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->x = x - klocek->w/3 - WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		else if (degrees == 180) {
			klocek->x = 168;
			klocek->y = 192;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/4;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		else if (degrees == 270) {
			klocek->x = 408;
			klocek->y = 192;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->x = x - klocek->w/3 - WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		break;

	case niebieski:
		if (degrees == 0 || degrees == 180) {
			klocek->x = 360;
			klocek->y = 24;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->x = x - klocek->w/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		else if (degrees == 90 || degrees == 270) {
			klocek->x = 384;
			klocek->y = 96;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/2 + WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		break;
	case zielony:
		if (degrees == 0 || degrees == 180) {
			klocek->x = 456;
			klocek->y = 24;
			klocek->h = 2*WYM_KWADRATU;
			klocek->w = 3*WYM_KWADRATU;
			dest->x = x - klocek->w/2;
			dest->y = y - klocek->h/2 + WYM_KWADRATU;
		}
		else if (degrees == 90 || degrees == 270) {
			klocek->x = 456;
			klocek->y = 96;
			klocek->h = 3*WYM_KWADRATU;
			klocek->w = 2*WYM_KWADRATU;
			dest->x = x - klocek->w/2 + WYM_KWADRATU/2;
			dest->y = y - klocek->h/2 + 3*WYM_KWADRATU/2;
		}
		break;
	}
	dest->w = klocek->w;
	dest->h = klocek->h;
	SDL_BlitSurface(klocki, klocek, screen, dest);
};

// narysowanie pojedynczego kwadratu klocka w punkcie (x, y)
void DrawSquare(SDL_Surface *screen, SDL_Surface *klocki, rodzaje rodzajKlocka, int x, int y) {
	SDL_Rect src, dest;
	dest.h = dest.w = src.h = src.w = WYM_KWADRATU;
	switch (rodzajKlocka) {
	case czerwony:
		src.x = 504;
		src.y = 120;
		break;

	case siwy:
		src.x = 504;
		src.y = 144;
		break;

	case blekitny:
		src.x = 504;
		src.y = 168;
		break;

	case zolty:
		src.x = 504;
		src.y = 192;
		break;

	case rozowy:
		src.x = 504;
		src.y = 216;
		break;

	case niebieski:
		src.x = 504;
		src.y = 240;
		break;

	case zielony:
		src.x = 504;
		src.y = 96;
		break;
	}
	dest.x = x;
	dest.y = y;
	SDL_BlitSurface(klocki, &src, screen, &dest);
}

void DrawFromTab(SDL_Surface *screen, SDL_Surface *klocki, int tab[][GAME_HEIGHT / WYM_KWADRATU + 1]) {
	rodzaje tmp;
	for (int i = 0; i < GAME_WIDTH/WYM_KWADRATU; i++)
		for (int j = 0; j < GAME_HEIGHT/WYM_KWADRATU; j++) {
			tmp = (rodzaje)tab[i][j];
			if (tab[i][j] != 0) DrawSquare(screen, klocki, tmp, i*WYM_KWADRATU, j*WYM_KWADRATU);
		}
}

// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt srodka obrazka sprite na ekranie
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w/2;
	dest.y = y - sprite->h/2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};

// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};

// rysowanie linii o dlugoœci l w pionie (gdy dx = 0, dy = 1) 
// badz poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};

// rysowanie prostok¹ta o d³ugoœci boków l i k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

// -----------	funkcje inicjujace	----------------------
bool init(SDL_Surface **screen, SDL_Texture **scrtex, SDL_Window **window, SDL_Renderer **renderer) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return false;
	}
	int rc;
//	tryb pe³noekranowy
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return false;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(*window, "Tetris Karol Kuppe 160726");

	*screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	*scrtex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	SDL_Surface *icon = SDL_LoadBMP("./tetris.bmp");
	if (icon == NULL) {
		printf("SDL_LoadBMP(tetris.bmp) error: %s\n", SDL_GetError());
	}
	else SDL_SetWindowIcon(*window, icon);
	SDL_FreeSurface(icon);

	return true;
}

bool loadBMP(SDL_Surface **charset, SDL_Surface **klocki, SDL_Surface **nic, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer) {
	// wczytanie obrazka cs8x8.bmp
	*charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return false;
	};
	SDL_SetColorKey(*charset, true, CZARNY);

	*klocki = SDL_LoadBMP("./klocki.bmp");
	if (klocki == NULL) {
		printf("SDL_LoadBMP(klocki.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(*charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return false;
	};
	SDL_SetColorKey(*klocki, true, BIALY);

	*nic = SDL_LoadBMP("./klocki1.bmp");
	if (klocki == NULL) {
		printf("SDL_LoadBMP(klocki.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(*charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return false;
	};
	SDL_SetColorKey(*nic, true, BIALY);

	return true;
}

// ------------	reszta funkcji
bool czyOpuscic(int tab[][GAME_HEIGHT/WYM_KWADRATU+1], rodzaje rodzajKlocka, SDL_Point punkt, int degrees) {
	switch (rodzajKlocka) {
	case czerwony:
		if (degrees == 0 || degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-4*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+4] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90 || degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1] == 0)return 1;
			else return 0;
		}
		break;

	case siwy:
		if (degrees == 0) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		else if (degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		break;

	case blekitny:
		if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
		else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
		else return 0;
		break;

	case zolty:
		if (degrees == 0) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		else if (degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		break;

	case rozowy:
		if (degrees == 0) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		else if (degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		break;

	case niebieski:
		if (degrees == 0 || degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90 || degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		break;

	case zielony:
		if (degrees == 0 || degrees == 180) {
			if (punkt.y > SCREEN_HEIGHT-2*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1] == 0
				&& tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0)return 1;
			else return 0;
		}
		else if (degrees == 90 || degrees == 270) {
			if (punkt.y > SCREEN_HEIGHT-3*WYM_KWADRATU) return 0;
			else if(tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2] == 0
				&& tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2] == 0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3] == 0)return 1;
			else return 0;
		}
		break;
	}
	return false;
}

bool czyObrocic(SDL_Point punkt, int tab[][GAME_HEIGHT/WYM_KWADRATU+1], rodzaje rodzajKlocka, int degrees) {
	switch (rodzajKlocka) {
	case czerwony:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-5*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			return true;
		}
		break;

	case siwy:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		break;

	case blekitny:
		return true;
		break;

	case zolty:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		break;

	case rozowy:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		break;

	case niebieski:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		break;

	case zielony:
		if (degrees == 0 || degrees == 180) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(punkt.x>=3*WYM_KWADRATU/2 && punkt.x<=GAME_WIDTH-3*WYM_KWADRATU/2)
				return true;
		}
		break;
	}
	return false;
}

bool czyPrzesunac(SDL_Point punkt, int tab[][GAME_HEIGHT/WYM_KWADRATU+1], rodzaje rodzajKlocka, int degrees, bool wLewo) {
	switch (rodzajKlocka) {
	case czerwony:
		if (degrees == 0 || degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 &&
				tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+3]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0 &&
				tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+3]==0)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+3][punkt.y/WYM_KWADRATU]==0)
				return true;
		}
		break;

	case siwy:
		if (degrees == 0) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0)
				return true;
		}
		else if (degrees == 90) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		if (degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
		}
		else if (degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		break;

	case blekitny:
		if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0)
			return true;
		else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
			return true;
		break;

	case zolty:
		if (degrees == 0) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		else if (degrees == 90) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			}
		else if (degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		else if (degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			}
		break;

	case rozowy:
		if (degrees == 0) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		else if (degrees == 90) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			}
		else if (degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
		}
		else if (degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			}
		break;

	case niebieski:
		if (degrees == 0 || degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			}
		break;

	case zielony:
		if (degrees == 0 || degrees == 180) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU-2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0)
				return true;
		}
		else if (degrees == 90 || degrees == 270) {
			if(wLewo==1 && tab[punkt.x/WYM_KWADRATU][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU-1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			else if(wLewo==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]==0 && tab[punkt.x/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]==0 && tab[punkt.x/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]==0)
				return true;
			}
		break;
	}
	return false;
}

int czyPelny(int tab[][GAME_HEIGHT/WYM_KWADRATU+1], int y, int wys) {
	bool fl;
	int ilePelnych = 0;
	for (int i = 0; i < wys/WYM_KWADRATU; i++) {
		fl = 1;
		for (int j = 0; j < GAME_WIDTH/WYM_KWADRATU; j++) {
			if(tab[j][y/WYM_KWADRATU+i]==0) {
				fl=0;
				break;
			}
		}
		if (fl == 1) {
			ilePelnych++;
			for (int j = 0; j < GAME_WIDTH/WYM_KWADRATU; j++)
				tab[j][y/WYM_KWADRATU+i] = 0;
			for (int j = y/WYM_KWADRATU+i; j>0; j--)
				for(int k = 0; k<GAME_WIDTH/WYM_KWADRATU; k++)
					tab[k][j] = tab[k][j-1];
		}
	}
	return ilePelnych;
}

void ktoreMiejsce(int *miejsce, int iloscPunktow, hiscore tabHisc[LICZBA_NAJLEPSZYCH]) {
	for (int i = (*miejsce == 0 ? LICZBA_NAJLEPSZYCH : *miejsce - 1); i > 0; i--) {
		if (iloscPunktow > tabHisc[i].iloscPunktow)
			*miejsce = i;
	}
}

void naniesienieNaEkran(SDL_Surface *screen, SDL_Renderer *renderer, SDL_Texture *scrtex) {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void rysujObszar(SDL_Surface *screen, SDL_Surface *klocki, SDL_Surface *charset, SDL_Rect *klocek, SDL_Point punkt, SDL_Rect *dest, SDL_Rect *none, int nastepnyKlocek, int poziom, 
		int tab[][GAME_HEIGHT/WYM_KWADRATU+1], int miejsce, int degrees, int iloscPunktow) {
	int grey = SDL_MapRGB(screen->format, 0x7E, 0x7E, 0x7E);
	int white = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	char text[128];
	SDL_FillRect(screen, NULL, black);
	DrawRectangle(screen, GAME_WIDTH, 0, SCREEN_WIDTH - GAME_WIDTH, SCREEN_HEIGHT-4*WYM_KWADRATU, black, grey);
	DrawBlock(screen, klocki, (rodzaje)nastepnyKlocek, klocek, dest, none, (SCREEN_WIDTH-GAME_WIDTH)/2+GAME_WIDTH,5*WYM_KWADRATU,0);
	DrawLine(screen, GAME_WIDTH, 0, GAME_HEIGHT, 0, 1, white);
	sprintf(text, "Nastepny klocek:");
	DrawString(screen, SCREEN_WIDTH-(SCREEN_WIDTH-GAME_WIDTH)/2 - strlen(text)*8/2, 4*WYM_KWADRATU, text, charset);
	DrawBlock(screen, klocki, rodzajKlocka, klocek, dest, none, punkt.x, punkt.y, degrees);
	DrawFromTab(screen, klocki, tab);
	sprintf(text, "miejsce %d", miejsce);
	if(miejsce>0)DrawString(screen, SCREEN_WIDTH-(SCREEN_WIDTH-GAME_WIDTH)/2 - strlen(text)*8/2, screen->h/2-10, text, charset);
	sprintf(text, "poziom: %d, ilosc punktow: %d", poziom, iloscPunktow);
	DrawString(screen, SCREEN_WIDTH-(SCREEN_WIDTH-GAME_WIDTH)/2 - strlen(text)*8/2, screen->h/2+10, text, charset);
}

void umiescKlocek(int tab[][GAME_HEIGHT/WYM_KWADRATU+1], SDL_Point punkt, rodzaje rodzajKlocka, int degrees) {
	switch (rodzajKlocka) {
	case czerwony:
		if (degrees == 0 || degrees == 180) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=czerwony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=czerwony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=czerwony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+3]=czerwony;
		}
		else {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=czerwony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=czerwony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=czerwony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+3][punkt.y/WYM_KWADRATU]=czerwony;
		}
		break;
	case siwy:
		if (degrees == 0){
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=siwy;
		}
		if (degrees == 90) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]=siwy;
		}
		if (degrees == 180) {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]=siwy;

		}
		if (degrees == 270) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=siwy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=siwy;
		}
		break;
	case blekitny:
		tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=blekitny;
		tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=blekitny;
		tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=blekitny;
		tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=blekitny;
		break;
	case zolty:
		if (degrees == 0){
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]=zolty;
		}
		if (degrees == 90) {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=zolty;
		}
		if (degrees == 180) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=zolty;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]=zolty;

		}
		if (degrees == 270) {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=zolty;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]=zolty;
		}
		break;
		case rozowy:
		if (degrees == 0){
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]=rozowy;
		}
		if (degrees == 90) {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]=rozowy;
		}
		if (degrees == 180) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=rozowy;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=rozowy;

		}
		if (degrees == 270) {
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=rozowy;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]=rozowy;
		}
		break;
		case niebieski:
		if (degrees == 0 || degrees == 180){
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=niebieski;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU]=niebieski;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=niebieski;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=niebieski;
		}
		if (degrees == 90 || degrees == 270) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=niebieski;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=niebieski;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=niebieski;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+2]=niebieski;
		}
		break;
		case zielony:
		if (degrees == 0 || degrees == 180){
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU]=zielony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=zielony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=zielony;
			tab[(punkt.x-3*WYM_KWADRATU/2)/WYM_KWADRATU+2][punkt.y/WYM_KWADRATU+1]=zielony;
		}
		if (degrees == 90 || degrees == 270) {
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+1]=zielony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU][punkt.y/WYM_KWADRATU+2]=zielony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU]=zielony;
			tab[(punkt.x-WYM_KWADRATU/2)/WYM_KWADRATU+1][punkt.y/WYM_KWADRATU+1]=zielony;
		}
		break;
	}
}

void usunPelny(SDL_Surface *screen, SDL_Surface *klocki, rodzaje *rodzajKlocka, int *nastepnyKlocek, int *iloscPunktow, int poziom, int *degrees, SDL_Point *punkt, SDL_Rect klocek, SDL_Rect dest, SDL_Rect none,
		int *ilePelnych, int *ilePoprzednio) {
	if (*ilePelnych == 1)*iloscPunktow += 100*poziom;
	else if (*ilePelnych == 2)*iloscPunktow += 200*poziom;
	else if (*ilePelnych == 3)*iloscPunktow += 400*poziom;
	else if (*ilePelnych == 4 && *ilePoprzednio != 4)*iloscPunktow += 800*poziom;
	else if (*ilePelnych == 4 && *ilePoprzednio == 4)*iloscPunktow += 1200*poziom;
	*ilePoprzednio = *ilePelnych;
	punkt->x = GAME_WIDTH/2 - WYM_KWADRATU/2;
	punkt->y = 0;
	*rodzajKlocka = (rodzaje)*nastepnyKlocek;
	*nastepnyKlocek = (rand()%7 + 1);
	*degrees = 0;
	DrawBlock(screen, klocki, *rodzajKlocka, &klocek, &dest, &none, punkt->x, punkt->y, *degrees);
}

void aktualizujCzas(int *t1, int *t2, double *worldTime, double *levelTime, double *drawTime, double *delta) {
	*t2 = SDL_GetTicks();
	*delta = (*t2 - *t1) * 0.001;
	*t1 = *t2;
	*worldTime += *delta;
	*levelTime += *delta;
	*drawTime += *delta;
}

void wyswietlHisc(hiscore tabHisc[LICZBA_NAJLEPSZYCH], SDL_Surface *screen, SDL_Surface *charset) {
	char text[128];
	for (int i = 0; i < LICZBA_NAJLEPSZYCH;i++) {
		sprintf(text, "%2d    %7d    %s",tabHisc[i].lp,tabHisc[i].iloscPunktow,tabHisc[i].imie);
		DrawString(screen, GAME_WIDTH/3, screen->h/5+10+12*(i+1), text, charset);
	}
}

void updateHisc(hiscore tabHisc[LICZBA_NAJLEPSZYCH], int miejsce, char imie[21], int iloscPunktow) {
	for (int i = LICZBA_NAJLEPSZYCH; i > miejsce-1; i--) {
		tabHisc[i].iloscPunktow = tabHisc[i-1].iloscPunktow;
		for (int j = 0; j < (int)strlen(tabHisc[i-1].imie) && j<20; j++) {
			tabHisc[i].imie[j] = tabHisc[i-1].imie[j];
			tabHisc[i].imie[j+1]='\0';
		}
	}
	tabHisc[miejsce].iloscPunktow = iloscPunktow;
	for (int i = 0; i < (int)strlen(imie);i++)
		tabHisc[miejsce].imie[i] = imie[i];
	tabHisc[miejsce].imie[strlen(imie)]=0;
	FILE *zapis = fopen("hiscore.tet", "w");
		for (int i = 0; i < LICZBA_NAJLEPSZYCH; i++) {
			fprintf(zapis, "%d ", tabHisc[i].lp);
			fprintf(zapis, "%d ", tabHisc[i].iloscPunktow);
			fprintf(zapis, "%s\n", tabHisc[i].imie);
		}
	fclose(zapis);
}

void loadHiscore(hiscore tabHisc[LICZBA_NAJLEPSZYCH]) {
	FILE *odczyt = fopen("hiscore.tet", "r");
	for (int i = 0; i < LICZBA_NAJLEPSZYCH; i++) {
		int a, b;
		char c[21];
		fscanf(odczyt, "%d ", &a);
		fscanf(odczyt, "%d ", &b);
		fscanf(odczyt, "%20s", c);
		tabHisc[i].lp = a;
		tabHisc[i].iloscPunktow = b;
		for (int j = 0; j < (int)strlen(c); j++)
			tabHisc[i].imie[j] = c[j];
	}
	fclose(odczyt);
}

void closeGame(SDL_Surface *charset, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Window *window) {
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	exit(0);
}

void koniecGry(SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset, bool *wpisanieImienia, int white, int black) {
	char text[128];
	DrawRectangle(screen, GAME_WIDTH/4, GAME_HEIGHT/8, GAME_WIDTH/2, LICZBA_NAJLEPSZYCH*20, white, black);
	sprintf(text, "Koniec gry");
	DrawString(screen, GAME_WIDTH/2- strlen(text)*8/2, screen->h/5-20, text, charset);
	sprintf(text, "Wpisz swoje imie i wcisnij Enter");
	DrawString(screen, GAME_WIDTH/2- strlen(text)*8/2, screen->h/5+20, text, charset);
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_StartTextInput();
	*wpisanieImienia = 1;
}

void obroc(int *degrees, double *worldTime, SDL_Point *center, int *t1, int *t2, double *delta, double *drawTime, double *levelTime, SDL_Renderer *renderer, SDL_Texture *scrtex, SDL_Rect *none, SDL_Rect *dest, 
	SDL_Surface *screen, SDL_Surface *nic, SDL_Surface *klocki, SDL_Rect *klocek, SDL_Point punkt) {
	int	temp_degrees = *degrees;
	while (++(*degrees) < temp_degrees + 90) {
		double tempTime = *worldTime;
		while (*worldTime - tempTime < (1 / (PREDKOSC_OBROTU*1000))) {
			aktualizujCzas(t1, t2, worldTime, levelTime, drawTime, delta);
		}
		if ((*degrees > 0 && *degrees < 90) || (*degrees > 180 && *degrees < 270)) {
			if (rodzajKlocka == czerwony) {
				center->x = 32;
				center->y = 32;
			}
			else if (rodzajKlocka == siwy || rodzajKlocka == niebieski || rodzajKlocka == zielony) {
				center->x = 36;
				center->y = 36;
			}
			else if (rodzajKlocka == blekitny || rodzajKlocka == zolty || rodzajKlocka == rozowy) {
				center->x = 24;
				center->y = 24;
			}
		}
		if ((*degrees > 90 && *degrees < 180) || (*degrees > 270 && *degrees < 360)) {
			if (rodzajKlocka == czerwony || rodzajKlocka == siwy || rodzajKlocka == blekitny || rodzajKlocka == niebieski || rodzajKlocka == zielony) {
				center->x = 24;
				center->y = 24;
			}
			else if (rodzajKlocka == zolty || rodzajKlocka == rozowy) {
				center->x = 36;
				center->y = 36;
			}
		}
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderCopy(renderer, scrtex, none, dest);
		SDL_RenderCopyEx(renderer, scrtex, dest, dest, *degrees%90, center, SDL_FLIP_NONE);
		SDL_RenderPresent(renderer);
	}
	if (*degrees == 360)*degrees = 0;
	DrawBlock(screen, nic, rodzajKlocka, klocek, dest, none, punkt.x, punkt.y, temp_degrees);
	DrawBlock(screen, klocki, rodzajKlocka, klocek, dest, none, punkt.x, punkt.y, *degrees%360);
	naniesienieNaEkran(screen, renderer, scrtex);
}

void zapisz(int* ileZapisano, SDL_Point punkt, int degrees, int iloscPunktow, int poziom, double levelTime, int tab[][GAME_HEIGHT/WYM_KWADRATU+1]) {
	(*ileZapisano)++;
	FILE *zapis = fopen("zapis.tet", "w");
	fprintf(zapis, "%d %d %d %d %d %d %d %lf\n", punkt.x, punkt.y, rodzajKlocka, degrees, iloscPunktow, poziom, *ileZapisano, levelTime);
	for (int i = 0;i<GAME_WIDTH/WYM_KWADRATU;i++){
		for(int j=0;j<GAME_HEIGHT/WYM_KWADRATU;j++)
			fprintf(zapis, "%d ", tab[i][j]);
		fprintf(zapis, "\n");
		}
	fclose(zapis);
}

void wczytaj(SDL_Point *punkt, rodzaje *rodzajKlocka, int *degrees, int *iloscPunktow, int *poziom, int *ileZapisano, double *levelTime,  int tab[][GAME_HEIGHT/WYM_KWADRATU+1], bool *endGame) {
	FILE *odczyt = fopen("zapis.tet", "r");
	if (odczyt != NULL) {
		fscanf(odczyt, "%d %d %d %d %d %d %d %lf\n", &(punkt->x), &(punkt->y), rodzajKlocka, degrees, iloscPunktow, poziom, ileZapisano, levelTime);
		for (int i = 0; i < GAME_WIDTH / WYM_KWADRATU; i++) {
			for (int j = 0; j < GAME_HEIGHT / WYM_KWADRATU; j++)
				fscanf(odczyt, "%d ", &tab[i][j]);
		}
		fclose(odczyt);
		*endGame = 0;
	}
}

void pauza(bool *quitPause, bool endGame, bool wpisanieImienia, int *t1, int *t2, double *delta, SDL_Surface *screen, SDL_Renderer *renderer, SDL_Texture *scrtex, SDL_Surface *charset) {
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	if (*quitPause == 0 && endGame == 0 && wpisanieImienia == 0) {
		char text[128];
		SDL_FillRect(screen, NULL, black);
		sprintf(text, "Pauza");
		DrawString(screen, screen->w/2 - strlen(text)*8/2, screen->h/2, text, charset);
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		*quitPause = 1;
	}
	else if (endGame == 0 && wpisanieImienia == 0) {
		*quitPause = 0;
		*t2 = SDL_GetTicks();
		*delta = (*t2-*t1)*0.001;
		*t1 = *t2;
	}
}

void rysujKoniecGry(SDL_Surface *screen, SDL_Renderer *renderer, SDL_Texture *scrtex, SDL_Surface *charset) {
	char text[128];
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int white = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	DrawRectangle(screen, GAME_WIDTH/4, GAME_HEIGHT/8, GAME_WIDTH/2, LICZBA_NAJLEPSZYCH*20, white, black);
	sprintf(text, "Koniec gry");
	DrawString(screen, GAME_WIDTH/2- strlen(text)*8/2, screen->h/5-20, text, charset);
	wyswietlHisc(tabHisc, screen, charset);
	naniesienieNaEkran(screen, renderer, scrtex);
}

void ruchWdol(SDL_Surface *screen, SDL_Surface *nic, SDL_Surface *klocki, SDL_Rect klocek, SDL_Rect *dest, SDL_Rect *none, SDL_Point *punkt, int degrees, int predkosc) {
	DrawBlock(screen, nic, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
	punkt->y += predkosc;
	DrawBlock(screen, klocki, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
}

void ruchWprawo(SDL_Surface *screen, SDL_Surface *nic, SDL_Surface *klocki, SDL_Rect klocek, SDL_Rect *dest, SDL_Rect *none, SDL_Point *punkt, int degrees, SDL_Renderer *renderer, SDL_Texture *scrtex){
	int temp_x = punkt->x;
	while (punkt->x < temp_x + WYM_KWADRATU) {
		DrawBlock(screen, nic, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
		punkt->x += 2;
		DrawBlock(screen, klocki, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
		naniesienieNaEkran(screen, renderer, scrtex);
	}
}

void ruchWlewo(SDL_Surface *screen, SDL_Surface *nic, SDL_Surface *klocki, SDL_Rect klocek, SDL_Rect *dest, SDL_Rect *none, SDL_Point *punkt, int degrees, SDL_Renderer *renderer, SDL_Texture *scrtex) {
	int temp_x = punkt->x;
	while (punkt->x > temp_x - WYM_KWADRATU) {
		DrawBlock(screen, nic, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
		punkt->x -= 2;
		DrawBlock(screen, klocki, rodzajKlocka, &klocek, dest, none, punkt->x, punkt->y, degrees);
		naniesienieNaEkran(screen, renderer, scrtex);
	};
}

void game(SDL_Surface *charset, SDL_Surface *klocki, SDL_Surface *nic, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Window *window, SDL_Renderer *renderer) {

	srand((unsigned int)time(NULL));
	SDL_Event event;
	SDL_Rect klocek, dest, none;
	SDL_RendererFlip flipType = SDL_FLIP_NONE;
	SDL_Point punkt, center;
	punkt.x = GAME_WIDTH/2 - WYM_KWADRATU/2;
	punkt.y = 0;
	none.x = GAME_WIDTH+5;
	none.y = GAME_HEIGHT-4*WYM_KWADRATU;
	none.h = none.w = WYM_KWADRATU;

	int degrees = 0, t1, t2, quit = 0, nastepnyKlocek, poziom = 1, predkosc=PREDKOSC_OPADANIA, ilePelnych=0, ilePoprzednio=0, iloscPunktow=0, ileZapisano=0, miejsce=0;
	double delta, worldTime = 0, levelTime = 0, drawTime = 0;
	char imie[21] = {};
	int tab[GAME_WIDTH/WYM_KWADRATU+1][GAME_HEIGHT/WYM_KWADRATU+1] = {};
	bool quitPause=0, endGame=0, wpisanieImienia=0, strzalkaWdol=0;
	int black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int white = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);

	t1 = SDL_GetTicks();
	rodzajKlocka = (rodzaje)(rand()%7+1);
	nastepnyKlocek = rand()%7+1;
	loadHiscore(tabHisc);
	ktoreMiejsce(&miejsce, iloscPunktow, tabHisc);
	rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);

	while (!quit) {
		if (endGame == 1 && wpisanieImienia==0)
			rysujKoniecGry(screen, renderer, scrtex, charset);

		else if(quitPause==0){
			aktualizujCzas(&t1, &t2, &worldTime, &levelTime, &drawTime, &delta);
			
			if (drawTime > ((CZAS_SPADANIA - (poziom-1)*WSPOLCZYNNIK_UTRUDNIENIA)/WYM_KWADRATU) || strzalkaWdol == 1)
			{
				drawTime = 0;

				if (levelTime > CZAS_POZIOMU) {
					if (poziom < ILOSC_POZIOMOW) poziom++;
					levelTime = 0;
					rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);
				}

				if (punkt.y < SCREEN_HEIGHT - klocek.h && czyOpuscic(tab, rodzajKlocka, punkt, degrees)) {
					ruchWdol(screen, nic, klocki, klocek, &dest, &none, &punkt, degrees, predkosc);
				}

				else if (punkt.y == 0 && czyOpuscic(tab, rodzajKlocka, punkt, degrees) == 0) {
					if (miejsce != 0) koniecGry(screen, scrtex, renderer, charset, &wpisanieImienia, white, black);
					endGame = 1;
				}

				else {
					umiescKlocek(tab, punkt, rodzajKlocka, degrees);
					ilePelnych = czyPelny(tab, punkt.y, klocek.h);
					usunPelny(screen, klocki, &rodzajKlocka, &nastepnyKlocek, &iloscPunktow, poziom, &degrees, &punkt, klocek, dest, none, &ilePelnych, &ilePoprzednio);
					ktoreMiejsce(&miejsce, iloscPunktow, tabHisc);
					rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);
				}
			}

			if(wpisanieImienia == 0) {
				naniesienieNaEkran(screen, renderer, scrtex);
			}
		}

		// obsluga zdarzen (o ile jakies zaszly)
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
				case SDLK_UP:
					if (endGame == 0 && quitPause == 0 && wpisanieImienia == 0 && czyObrocic(punkt, tab, rodzajKlocka, degrees) == 1)
						obroc(&degrees, &worldTime, &center, &t1, &t2, &delta, &drawTime, &levelTime, renderer, scrtex, &none, &dest, screen, nic, klocki, &klocek, punkt);
					break;

				case SDLK_ESCAPE:
					quit = 1;
					break;

				case SDLK_DOWN:
					predkosc = PREDKOSC_SZYBKIEGO_OPADANIA;
					strzalkaWdol = 1;
					break;

				case SDLK_LEFT:
					if (punkt.x - WYM_KWADRATU/2 >= klocek.w/2 && endGame==0 && quitPause==0 && wpisanieImienia==0 && czyPrzesunac(punkt, tab, rodzajKlocka, degrees,1)==1)
						ruchWlewo(screen, nic, klocki, klocek, &dest, &none, &punkt, degrees, renderer, scrtex);
					break;
				
				case SDLK_RIGHT:
					if (punkt.x + WYM_KWADRATU/2 < GAME_WIDTH-(klocek.w/2) && endGame==0 && quitPause==0 && wpisanieImienia==0 && czyPrzesunac(punkt, tab, rodzajKlocka, degrees, 0)==1)
						ruchWprawo(screen, nic, klocki, klocek, &dest, &none, &punkt, degrees, renderer, scrtex);
					break;

				case SDLK_u:
					if(endGame==0 && quitPause==0 && wpisanieImienia==0) {
						if (poziom < ILOSC_POZIOMOW)poziom++;
						levelTime = 0;
						rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);
					}
					break;

				case SDLK_s:
					if (ileZapisano < 3 && endGame==0 && quitPause==0 && wpisanieImienia==0)
						zapisz(&ileZapisano, punkt, degrees, iloscPunktow, poziom, levelTime, tab);
					break;

				case SDLK_n:
					if (quitPause == 0 && wpisanieImienia==0) {
						game(charset, klocki, nic, screen, scrtex, window, renderer);
						exit(0);
					}
					break;

				case SDLK_l:
					if (quitPause == 0 && wpisanieImienia==0) 
						wczytaj(&punkt, &rodzajKlocka, &degrees, &iloscPunktow, &poziom, &ileZapisano, &levelTime, tab, &endGame);
						rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);
					break;
									
				case SDLK_p:
					pauza(&quitPause, endGame, wpisanieImienia, &t1, &t2, &delta, screen, renderer, scrtex, charset);
					if(quitPause==0)rysujObszar(screen, klocki, charset, &klocek, punkt, &dest, &none, nastepnyKlocek, poziom, tab, miejsce, degrees, iloscPunktow);
					break;

				case SDLK_RETURN:
					if (wpisanieImienia == 1 && endGame==1 && quitPause == 0)
					{
						SDL_StopTextInput();
						updateHisc(tabHisc, miejsce, imie, iloscPunktow);
						wpisanieImienia = 0;
					}
					break;

				}
				break;
			case SDL_TEXTINPUT:
				if(wpisanieImienia==1 && endGame==1 && strlen(event.text.text)<=20) {
					strcat(imie, event.text.text);
				}
				break;
				
			case SDL_KEYUP:
				predkosc = PREDKOSC_OPADANIA;
				strzalkaWdol = 0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
	};

	//zwolnienie powierzchni
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

// -----------	main	----------------------
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	SDL_Surface *screen, *charset;
	SDL_Surface *klocki, *nic;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;

	init(&screen, &scrtex, &window, &renderer);
	loadBMP(&charset, &klocki, &nic, screen, scrtex, window, renderer);
	game(charset, klocki, nic, screen, scrtex, window, renderer);
	return 0;
	};