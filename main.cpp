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
#define PORT 10001

SDL_Surface *screen;
SDL_Event event;

Board * chessboard;
std::vector<int> fds;
bool done;
int fd, max;
int owner;

bool cancel()
{
	while(SDL_PollEvent(&event))
	{
		std::cout << "\t\t\t\tEVENT!" <<std::endl;
		if (event.type == SDL_KEYDOWN)
			std::cout << " \t\t\t\tKEYEVENT!" <<std::endl;
		if ( event.type == SDL_QUIT )
			return true;
		if ((event.type == SDL_KEYDOWN)&&
				((event.key.keysym.sym == SDLK_ESCAPE)
				 || event.key.keysym.sym == SDLK_q))
			return true;
	}
//	std::cout << " \t\t\t\t\tnist"<<std::endl;
	return false;
}

int server()
{
	int port, new_fd, client_address_sz;
	//struct servent *sp;
	struct sockaddr_in server_address, client_address;

	port = PORT;//TODO opravit na najblizsi volny port
	
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
	if (listen(fd, 3) == -1) { perror("listen()"); return -1;}

	printf("Listening\n");

	fcntl(fd, F_SETFL, O_NONBLOCK);
	max = 0;
	while (fds.size()<2) 
	{
		if ((new_fd = accept(fd, (sockaddr *) &client_address, (socklen_t *)&client_address_sz))!=-1){
			printf("Acceptnul jsem klienta...\n");
			if (max < new_fd)
			{
				max = new_fd;
			}
			fds.push_back(new_fd);
			int x = htonl(fds.size());
			write(new_fd,&x,sizeof(int)); //TODO checkovat chybove stavy
		}
		if (cancel())
		{
			printf("Koncim hostovanie");
			for(size_t i =0; i< fds.size(); i++)
			       close(fds[i]);	
			return -1;
		}
	}
	std::cout << max <<std::endl;
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
				width = 1040;
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
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					case SDLK_q:
						return false;
						break;

					case SDLK_r:
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
	timeval delay;
	FD_ZERO(&rfdset);
	for (size_t i = 0; i< fds.size(); i++)
		FD_SET(fds[i], &rfdset);
	efdset = rfdset;
	std::cout << "Begin waiting" <<std::endl;
	delay.tv_sec = 1;
	delay.tv_usec = 0;
	sock = select (max + 1, &rfdset, NULL, &efdset, &delay);
	std::cout << sock << " ";
	std::cout << "End waiting" <<std::endl;
	if (sock == -1)
	{
		std::cout << strerror(errno) <<std::endl;
		return -1;
	}
	int state = 0;
	if (sock > 0)//mame odpoved
	{
		
		bool ex = false;
		std::cout << "IN" << std::endl;
		for (size_t i =0; i< fds.size(); i++)
			if (FD_ISSET(fds[i],&rfdset))
			{
				int buf[10];
				sz = read(fds[i],buf,sizeof(int)*6);
				if (sz == 0)
				{
					ex = true;
					std::cout << "vypadol user" << i << std::endl; //TODO zneplatnit usera
				}
				if (sz==sizeof(int)*6)//mame triple
				{
					printf("mam");
					(*t1) = Triple(ntohl(buf[0]),ntohl(buf[1]),ntohl(buf[2]));
					(*t2) = Triple(ntohl(buf[3]),ntohl(buf[4]),ntohl(buf[5]));
					state = 1;
				}
			}
		for (size_t i =0; i< fds.size(); i++)
			if (FD_ISSET(fds[i],&efdset))
			{
				ex = true;
				std::cout << "Asi spatny prenos od usera" << i << std::endl; //TODO dorobit pre nestandartnu siet
				exit(2);
			}
		if (ex)
			return -1;
	}
	return state; //TODO nejake vynimky
}

bool ProcessEventHost()
{
	if (cancel())
	{
		std::cout <<"exitted"<<std::endl;
		return false;
	}
	if (chessboard->on_turn.val()!=0)//prijimac sockety a rozposielal dalej
	{
		int bufxxx[10];
		Triple t1,t2;
	       	int res	= talk(&t1, &t2);
		if (res == -1) //chyba kecania
			return false;
		if (res == 0)
		{
			return true;
		}
		std::cerr << "picking up figure" <<std::endl;
		int iter = chessboard->on_turn.val()%2;//na rade je 0-> bola 1 a posle sa 1, 2-? na rade bola 1 a posle sa 0 alias 2:)
		chessboard->pick_up_figure(t1);
		chessboard->pick_up_figure(t2);
		bufxxx[0] = htonl(t1.x);bufxxx[1] = htonl(t1.y);bufxxx[2] = htonl(t1.z);
		bufxxx[3] = htonl(t2.x);bufxxx[4] = htonl(t2.y);bufxxx[5] = htonl(t2.z);
		res = write(fds[iter], bufxxx, sizeof(int)*6);
		std::cout << "velkost " << res<<"posielam cislu:"<<iter <<std::endl;
		if (res == -1)
		{
			std::cerr<<"write bad"<< std::endl;
			exit(3);
		}
	}
	else 
	{
		int buf[10];
//		std::cout << "Not waiting" <<chessboard->on_turn.val()<<std::endl;
		bool res = ProcessEvent(); //a sendni ostatnym
		if (chessboard->moved)
		{
			chessboard->moved = false;
			buf[0] = htonl(chessboard->last_move.first.x);
			buf[1] = htonl(chessboard->last_move.first.y);
			buf[2] = htonl(chessboard->last_move.first.z);
			buf[3] = htonl(chessboard->last_move.second.x);
			buf[4] = htonl(chessboard->last_move.second.y);
			buf[5] = htonl(chessboard->last_move.second.z);
//			std::cout << chessboard->last_move.first.x <<" " << chessboard->last_move.first.y << " " << chessboard->last_move.first.z << "\t" <<  chessboard->last_move.second.x <<" " << chessboard->last_move.second.y << " " << chessboard->last_move.second.z;
			for (size_t i = 0; i < fds.size();i++)
				std::cout << write(fds[i],buf,sizeof(int)*6) << "posielam tolkoto bajtov"<<std::endl;
		}
		return res;//TODO zobecnit pre booly a pre inty
	}
	return true;
}

bool end_with(std::string s1, std::string s2)
{
	int end = s1.length()-1;
	for (size_t i = s2.length()-1 ; i>=0; i--)
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
	port = PORT;
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
	return true;
}
bool ProcessSetJoin()
{
	//bool done = false;
	std::string user_port;
	std::string server_name;
	bool ok1=false;
	while(true)
	{
		SDL_WaitEvent(&event);
		switch(event.type)
		{
			case SDL_KEYDOWN:
				{
					switch (event.key.keysym.sym)
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
								user_port+=event.key.keysym.sym;
							else
								server_name+=event.key.keysym.sym;
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

	efdset = rfdset;
	owner = -1;
	int buf[10];
	if (select(fd+1, &rfdset, NULL, &efdset, NULL)==-1)
	       perror("select()");
	if (FD_ISSET(fd, &rfdset))
	{
		int sz = read(fd,buf,sizeof(int));
		owner = ntohl(buf[0]);
		std::cerr << "mam ownera c."<<owner <<  std::endl;
	}
	int res;
	while (true)
	{
		std::cerr <<"owner:"<< owner << "\t";
		if (cancel())
			return true;
		if (chessboard->on_turn.val() == owner)
		{
			std::cout << "Not waiting" << chessboard->on_turn.val()<<std::endl;
			bool res = ProcessEvent();
			if (chessboard->moved)
			{
				int buf2[10];
				chessboard->moved = false;
				buf2[0]=htonl(chessboard->last_move.first.x);
				buf2[1]=htonl(chessboard->last_move.first.y);
				buf2[2]=htonl(chessboard->last_move.first.z);
				buf2[3]=htonl(chessboard->last_move.second.x);
				buf2[4]=htonl(chessboard->last_move.second.y);
				buf2[5]=htonl(chessboard->last_move.second.z);
				int sz = write(fd,buf2,sizeof(int)*6);
				std::cerr << sz << "__" <<std::endl;
			}
			continue;
		}
		FD_ZERO(&rfdset);
		FD_SET(0, &rfdset);
		FD_SET(fd, &rfdset);
		efdset = rfdset;
		timeval t;
		int buf4[10];
		t.tv_sec = 1;
		t.tv_usec = 0;
		res = select (fd + 1, &rfdset, NULL, &efdset, &t);
		if (res == -1)
		{
			perror("select()");
			return false;
		}
		if (FD_ISSET(fd, &rfdset))
		{
			std::cout << "echmechm!"<<std::endl;
			int buf3[10];
			int sz = read(fd,buf3,sizeof(int)*10);
			std::cout << sizeof(int)*10 << ":" << sz <<std::endl;
			if (sz == -1)
				return true;//padol na spatnom vstupe
			if (sz == 0)
			{
				std::cerr << "Eof na vstupe" <<std::endl;
				return true;
			}
			Triple t1(ntohl(buf3[0]),ntohl(buf3[1]),ntohl(buf3[2]));
			Triple t2(ntohl(buf3[3]),ntohl(buf3[4]),ntohl(buf3[5]));
			std::cout << t1.x << " "<< t1.y << " "<< t1.z << " "<< t2.x << " "<< t2.y << " " << t2.z << std::endl;
			chessboard->pick_up_figure(t1);
			chessboard->pick_up_figure(t2);
			chessboard->moved = false;
		}
		std::cout <<"waiting" <<chessboard->on_turn.val()<< std::endl;
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
		done = false;	
		m= menu();
		while (m < 0)
			m = menu();
		// Hlavni smycka programu
		if (m == Play)
		{
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
			std::cerr << "OUT" <<std::endl;
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
				continue;
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

