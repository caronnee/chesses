#include <iostream>
#include <SDL/SDL.h>
#include "board.h"
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <sys/time.h>

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
std::vector<int> fds;
bool done;
int fd, max;
int owner;

bool cancel()
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
	if ((e.type == SDL_KEYDOWN)&&
		((e.key.keysym.sym == SDLK_ESCAPE)
		 || e.key.keysym.sym == SDLK_q))
		return true;
	}
	return false;
}

int server()
{
	int port, new_fd, client_address_sz;
	//struct servent *sp;
	struct sockaddr_in server_address, client_address;

	port = 10000;//TODO opravit na najblizsi volny port
	
	//TODO ako funguje getservbyname???
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket()");
		return -1;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) 
	{
		perror("socket()");
		return -1;
	}

	client_address_sz = sizeof(client_address);
	if (listen(fd, 1) == -1) { perror("listen()"); return -1;}

	printf("Listening\n");

	fcntl(fd, F_SETFL, O_NONBLOCK);
	max = 0;
	while (fds.size()<2) 
	{
		if ((new_fd = accept(fd, (sockaddr *) &client_address, (socklen_t *)&client_address_sz))!=-1){
			printf("Acceptnul jsem klienta...\n");
			if (max < new_fd)
			{
				std::cerr << "zvysujem"<<std::endl;
				max = new_fd;
			}
			fds.push_back(new_fd);
			int x = htonl(fds.size());
			std::cerr <<"posielam" <<x <<std::endl;
			std::cerr <<"zapisujem " <<write(new_fd,&x,sizeof(x))<< " do bufferu"<<std::endl; //TODO checkovat chybove stavy
		}
		if (cancel())
		{
			printf("koncim hostovanie");
			return -1;
		}
	}
	std::cerr<< "mmmm";
	return 0;
}

/*
 * Inicializacni funkce
 */

int Init(int width, int height, int bpp, Uint32 flags)
{
	// Inicializace SDL
	if(SDL_Init(SDL_SUBSYSTEMS) == -1)
	{
		std::cerr << "baaad";
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
			std::cerr << "SDL_VideoModeOK(): Parameters are not ";

			// Hodnota bpp byla prepsana volanim SDL_VideoModeOK()
			bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;

			// Zkusi ziskat dostupna rozliseni podle flagu
			SDL_Rect **modes;
			modes = SDL_ListModes(NULL, flags);

			if(modes == (SDL_Rect **)0)// Zadne dostupne mody
			{
				std::cerr << "SDL_ListModes(): No modes available\n";
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

					case SDLK_r:
						std::cerr << " ha!";
						chessboard->reset();
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
enum Menu
{
	Host = 0,
	Load,
	Join,
	Play,
	Exit,

};
int menu()
{
	SDL_Event event;

	while(SDL_WaitEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						return Exit;
						break;

					case SDLK_h:
						return Host;
						break;
					case SDLK_j:
						return Join;
					case SDLK_p:
						return Play;
					case SDLK_l:
						return Load;
					default:
						break;
				}
				break;

			case SDL_QUIT:
				return Exit;
				break;
			default:
				return -1;
				break;
		}
	}
	return true;
}

int talk(Triple * t1, Triple * t2)
{
	int sock;
	int sz;
	fd_set rfdset,efdset;
	char buf[6];
	timeval delay;
	delay.tv_sec = 1;
	FD_ZERO(&rfdset);
	for (unsigned int i = 0; i< fds.size(); i++)
		FD_SET(fds[i], &rfdset);
	efdset = rfdset;
	sock = select (max + 1, &rfdset, NULL, &efdset, &delay);
	if (sock > 0)//mame odpoved
	{
		for (unsigned int i =0; i< fds.size(); i++)
			if (FD_ISSET(fds[i],&rfdset))
			{
				sz = read(fds[i],buf,6);
				if (sz==6)//mame triple
				{
					printf("mam");
					(*t1) = Triple(buf[0],buf[1],buf[2]);
					(*t2) = Triple(buf[3],buf[4],buf[5]);
				}
			}
	}
	return false; //TODO nejake vynimky
}

//OK
bool ProcessEventHost()
{
	int buf[6];
	if (chessboard->on_turn.val()!=0)//prijimac sockety a rozposielal dalej
	{
		if (cancel())
			return true;
		Triple t1,t2;
	       	int res	= talk(&t1, &t2);
		if (res == -1)
			return true;
		chessboard->pick_up_figure(t1);
		chessboard->pick_up_figure(t2);
		int iter = chessboard->on_turn.val()%2;//na rade je 0-> bola 1 a posle sa 1, 2-? na rade bola 1 a posle sa 0 alias 2:)
		buf[0] = t1.x;buf[1] = t1.y;buf[2] = t1.z;
		buf[3] = t1.x;buf[4] = t1.y;buf[5] = t1.z;
		res = write(fds[iter], buf, 6);
		if (res == -1)
		{
			std::cerr<<"write bad"<< std::endl;
		}
		return false;
	}
	else 
	{
		bool res = ProcessEvent(); //a sendni ostatnym
		if (chessboard->moved)
		{
			chessboard->moved = false;
			buf[0] = chessboard->last_move.first.x;
			buf[1] = chessboard->last_move.first.y;
			buf[2] = chessboard->last_move.first.z;
			buf[3] = chessboard->last_move.second.x;
			buf[4] = chessboard->last_move.second.y;
			buf[5] = chessboard->last_move.second.z;
			for (unsigned int i = 0; i < fds.size();i++)
				write(fd,buf,6);
		}
		return res;//TODO zobecnit pre booly a pre inty
	}
	return false;
}

bool end_with(std::string s1, std::string s2)
{
	int end = s1.length()-1;
	for (unsigned int i = s2.length()-1 ; i>=0; i--)
	{
		if ( s1[end] != s2[i] )
			return false;
		end--;
		if (end <0)
			return false;
	}
	return true;
}
bool client()
{
	char server_name[] = "localhost";
	struct hostent *serverova_obcanka;
	//struct servent *sp;
	struct sockaddr_in server_address;
	int port;

	std::cerr << "snazim sa pripojit"<<std::endl;
	port = 10000;
	if (!(serverova_obcanka = gethostbyname(server_name))) {
		fprintf(stderr, "Cannot resolve server address\n");
		return false;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	memcpy(&server_address.sin_addr.s_addr, serverova_obcanka->h_addr_list[0], serverova_obcanka->h_length);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket()");
		return false;
	}
	if (connect(fd, (struct sockaddr *)&server_address,sizeof(server_address)) < 0) {
		perror("connect()");
		return false;
	};
	std::cerr << "Returning from client" <<std::endl;
	return true;
}
bool ProcessSetJoin()
{
	//bool done = false;
	std::string user_port;
	std::string server_name;
	SDL_Event e;
	bool ok1=false;
	while(true)
	{
		SDL_WaitEvent(&e);
		switch(e.type)
		{
			case SDL_KEYDOWN:
				{
					switch (e.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							return false;
						case SDLK_RETURN:
							if (ok1)
								return true;//TODO este prevod do portu
							ok1 = true;
							break;
						case SDLK_BACKSPACE:
							if (ok1) user_port.erase(user_port.length()-1,1);
							else server_name.erase(server_name.length()-1,1);
						default:
							if (ok1)
								user_port+=e.key.keysym.sym;
							else
								server_name+=e.key.keysym.sym;
					}
				}
		}
	}
}
bool ProcessEventJoin()
{
	fd_set rfdset,efdset;

	FD_ZERO(&rfdset);
	FD_SET(0, &rfdset);
	FD_SET(fd, &rfdset);

//	rfdset = efdset;
//	buf[0] =123;
	owner = -1;
	int buf[6];
	std::cerr << "pred\t";
	std::cerr << "select vyplul" << select(fd+1, &rfdset, NULL, &efdset, NULL) << std::endl;
	std::cerr << "\tpo";
	if (FD_ISSET(fd, &rfdset))
	{
		int sz = read(fd,buf,sizeof(buf));
		owner = ntohl(buf[0]);//TODO check!
		std::cerr << "mam ownera c."<<owner <<  std::endl;
	}
	int res;
	while (true)
	{
		std::cerr << owner << "\t";
		std::cerr << owner <<std::endl;
		if (cancel())
			return true;
		if (chessboard->on_turn.val() == owner)
		{
			
			std::cerr<<"he??:";
			bool res = ProcessEvent();
			if (chessboard->moved)
			{
				int buf2[6];
				buf2[0]=chessboard->last_move.first.x;
				buf2[1]=chessboard->last_move.first.y;
				buf2[2]=chessboard->last_move.first.z;
				buf2[3]=chessboard->last_move.second.x;
				buf2[4]=chessboard->last_move.second.y;
				buf2[5]=chessboard->last_move.second.z;
				int sz = write(fd,buf2,6);
			}
			continue;
		}
		FD_ZERO(&rfdset);
		FD_SET(0, &rfdset);
		FD_SET(fd, &rfdset);
		timeval t;
		int buf4[6];
		t.tv_sec = 1;
		res = select (fd + 1, &rfdset, NULL, &efdset, &t);
		if (res == -1)
		{
			perror("select()");
			exit(1);
		}
		if (FD_ISSET(fd, &rfdset))
		{
			int buf3[6];
			int sz = read(fd,buf3,6);
			if (sz == -1)
				return true;//padol na spatnom vstupe
			if (sz == 0)
			{
				std::cerr << "Eof na vstupe" <<std::endl;
				return true;
			}
			Triple t1(buf3[0],buf3[1],buf3[2]);
			Triple t2(buf3[3],buf3[4],buf3[5]);
			chessboard->pick_up_figure(t1);
			chessboard->pick_up_figure(t2);
		}
		std::cout <<"waiting" << std::endl;
	}
	return true;
}

int main(int argc, char *argv[])
{
	printf(WIN_TITLE);
	printf("\nWelcome to 3D-chess board.\n");

	// Inicializace
	if(!Init(WIN_WIDTH, WIN_HEIGHT, WIN_BPP, WIN_FLAGS))
	{
		Destroy();
		return 1;
	}

	int m = -1;
	while (m!= Exit)
	{
		m= menu();
		while (m < 0)
			m = menu();
		// Hlavni smycka programu
		if (m == Play)
		{
			int done = 0;
			chessboard = new Board();
			//			SDL_SetColorKey(screen,SDL_SRCCOLORKEY, SDL_MapRGB(screen->format,125,125,125));
			chessboard->draw_board();
			while(!done)
			{
				done = !ProcessEvent();
			}
			delete chessboard;
		}
		if (m == Load)
		{
			DIR * dir;
			dir = opendir(".");
			struct dirent *adr = readdir(dir);
			while (adr!=NULL)
			{
				std::string name = adr->d_name;
				if(end_with(name,".chs"))
					std::cerr <<name<<std::endl;
				adr = readdir(dir);
			}
			chessboard = new Board();
			chessboard->draw_board();
			while(!done)
			{
				//		done = !ProcessEventLoad();
				done = true;
			}
			delete chessboard;
		}
		if(m == Host)
		{
			int done = 0;
			fds.clear();
			int ret = server();
			if (ret!=0) {std::cerr << "Niekde nastala chyba" << std::endl; continue;}
			chessboard = new Board();
			chessboard->draw_board();
			while(!done)
			{
				done = !ProcessEventHost();
			}
			close(fd);
			delete chessboard;
		}
		if(m == Join)
		{
			int done = 0;
			fds.clear();
		/*	bool ok = ProcessSetJoin();
			if (!ok)
			{
				std::cerr <<"Nieco spatne parametre"<<std::endl;
			       continue;	
			}*/
			bool ok = client();
			std::cerr << ok <<std::endl;
			if(!ok)
			{
				printf("Nepodarilo sa pripojit");
				break;
			}
			chessboard = new Board();
			chessboard->draw_board();
			ProcessEventJoin();
			close(fd);
			delete chessboard;
		}
	}
	// Deinicializace a konec
	Destroy();
	return 0;
}

