#include <iostream>
#include <SDL/SDL.h>
#include "board.h"

#define SDL_SUBSYSTEMS SDL_INIT_VIDEO
#define WIN_FLAGS SDL_HWSURFACE|SDL_DOUBLEBUF
#define WIN_WIDTH BOARD_WIDTH 
#define WIN_HEIGHT BOARD_HEIGHT + 20
#define WIN_BPP 32

#define WIN_TITLE "3 - player chess!"

// Pro snadnejsi vypisy flagy "zformatovane" do retezce
#define STR_FLAGS "SDL_HWSURFACE"


/*
 * Globalni promenne
 */

SDL_Surface *screen;
Board * chessboard;
bool done;
/*
 * Inicializacni funkce
 */

int Init(int width, int height, int bpp, Uint32 flags)
{
	// Inicializace SDL
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)
	{
		std::cout << "baaad";
		return false;
	}

	bpp = SDL_VideoModeOK(width, height, bpp, flags);

	if(bpp == 0)// Predane parametry nejsou v poradku
	{
		// Nejvhodnejsi barevna hloubka podle SDL
		bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

		bpp = SDL_VideoModeOK(width, height, bpp, flags);

		if(bpp == 0)
		{
			std::cout << "SDL_VideoModeOK(): Parameters are not ";

			// Hodnota bpp byla prepsana volanim SDL_VideoModeOK()
			bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

			// Zkusi ziskat dostupna rozliseni podle flagu
			SDL_Rect **modes;
			modes = SDL_ListModes(NULL, flags);

			if(modes == (SDL_Rect **)0)// Zadne dostupne mody
			{
				std::cout << "SDL_ListModes(): No modes available\n";
				return false;
			}
			else if(modes == (SDL_Rect **)-1)// Zadna omezeni
			{
				width = 640;
				height = 480;
			}
			else// Pouzije maximalni dostupne rozmery
			{
				width = modes[0]->w;
				height = modes[0]->h;
			}

			bpp = SDL_VideoModeOK(width, height, bpp, flags);

			if(bpp == 0)
			{
				return false;
			}
		}
	}

	screen = SDL_SetVideoMode(width, height, bpp, flags);

	if(screen == NULL)
	{
		return false;
	}

	SDL_WM_SetCaption(WIN_TITLE, NULL);

	return true;
}


/*
 * Deinicializacni funkce
 */

void Destroy()
{
	SDL_Quit();
}


/*
 * Osetruje udalosti (bude probrano nekdy v budoucnu)
 * V tomto pripade se ceka na klavesu ESC, ktera ukonci program
 */

int ProcessEvent()
{
	SDL_Event event;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return false;
						break;

					default:
						break;
				}
				break;

			case SDL_QUIT:
				return false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					if(!done)
						done = chessboard->pick_up_figure(event.button.x, event.button.y);
					break;
				}

			default:
				break;
		}
	}

	return true;
}


/*
 * Vstup do programu
 */

int main(int argc, char *argv[])
{
	printf(WIN_TITLE);
	printf("\nPress ESC key to quit (after window is created).\n");

	// Inicializace
	if(!Init(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS))
	{
		Destroy();
		return 1;
	}

	done = false;
	// Hlavni smycka programu
	int done = false;
	/*SDL_Surface * obr = SDL_LoadBMP("./chessboard.bmp");
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	SDL_BlitSurface( obr, NULL, screen, &rect);
	SDL_Flip(screen);*/
	chessboard = new Board();
	SDL_SetColorKey(screen,SDL_SRCCOLORKEY, SDL_MapRGB(screen->format,125,125,125));
	chessboard->draw_board();
	while(!done)
	{
		done = !ProcessEvent();
	/*	SDL_BlitSurface( screen, NULL, screen, NULL);
		SDL_Flip(screen);*/
	}

	// Deinicializace a konec
	Destroy();
	return 0;
}

