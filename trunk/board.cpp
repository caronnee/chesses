#include "./board.h"
#define CENTER_X 213
#define CENTER_Y 200
#define NUM_OF_PLACES 48
#define RADIUS 200

enum Tokens
{
	KING, 
	QUEEN, 
	BISHOP, 
	JUMPER, 
	TOWER, 
	PAWN 
};

int Place::occupied()
{
	return occupied_by;
}
//x.y su aktualne policka v dvojrozmernom poli
Place::Place()
{
	occupied_by = -1;
	for ( int i =0; i < NUM_OF_PLACES; i++)
		attackers[i] = false;
	under_attack = 0;
	coord_x = 0;
	coord_y = 0; 
}

void Place::set(int x, int y)
{
	coord_x = x;
	coord_y = y;
}

void Place::occupy(int ID)
{
	occupied_by = ID;
}

int Place::get_x()
{
	return coord_x;
}

int Place::get_y()
{
	return coord_y;
}

int Figure::move()
{
	return -1;
}

void Figure::choosed()
{
	display_token = choose;
}

void Figure::unchoosed()
{
	display_token = token;
}
bool Figure::moved()
{
	return moved_;
}
void Figure::remove()
{
	active = false;

}
SDL_Surface * Figure::display()
{
	return display_token;
}

SDL_Surface * Figure::display_name()
{
	return name;
}

Figure::Figure()
{
	moved_ = false;
	owner = 0; //este nikto nemam zabookovanu figurku
	active = true;
	display_token = NULL;
	choose = SDL_LoadBMP("./images/selected.bmp");
	//SDL_SetColorKey(choose,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	under_attack = 0;
}

King::King(int o)
{
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/king.bmp");
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
}

int King::move()
{
	return KING;
}
Queen::Queen(int o)
{	
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/queen.bmp");
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
}

int Queen::move()
{
	return QUEEN;
}
Bishop::Bishop(int o)
{	
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/bishop.bmp");
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;

}

int Bishop::move()
{
	return BISHOP;
}
Jumper::Jumper(int o)
{	
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/jumper.bmp");
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
}

int Jumper::move()
{
	return JUMPER;
}

Tower::Tower(int o)
{	
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/tower.bmp");
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
}

int Tower::move()
{
	return TOWER;
}

Pawn::Pawn(int o)
{
	owner = o;
	std::stringstream out;
	out <<o;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/pawn.bmp");
	promotion = PAWN;
	if (name ==NULL) std::cerr << "huups!" <<std::endl;
	if (token ==NULL) std::cerr << "huups2!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
}

int Pawn::move()
{
	return promotion;
}

Board::Board()
{
	float size_x = RADIUS/8;//velkost nasho kroku
	float coord_x, coord_y_l, coord_y_l_last;
	int heigth = RADIUS * 1.73 /2; //nasa vyska
	int base = CENTER_Y + heigth;
	coord_y_l = 1.83*RADIUS / 3;//vyska toho najposlednejsie (najviac vlavo)
	board_img = SDL_LoadBMP("./images/chessboard.bmp");
	choosed_x = -1; //este sme nic nevybrali
	choosed_y = -1;
	choosed_p = -1;
	for (int x =0; x<4; x++)
	{
		coord_x = CENTER_X + (x-4)*size_x;
		coord_y_l_last = coord_y_l;
		coord_y_l = coord_y_l_last + (RADIUS*1.73)/24;
		for (int y = 0; y < 4; y++)
		{
			int pom2 = coord_y_l/4; 
			int pom1 = coord_y_l_last/4; //velkost kroku na oboch stranach
			int coord_y;
			int diff = (pom2 - pom1)/2;//prirastok

			if (x<4)	
				coord_y = base - (pom1*y + pom1/2  + diff); 
			else
				coord_y = base - (pom1*y + pom2/2  + diff);
			int rot_x = coord_x + size_x/2 - CENTER_X;
			int rot_y = coord_y - CENTER_Y;
			int rot_x_2 = rot_x + size_x*(7-2*x);
			for (int player = 0; player < 3; player++)
			{
				board[player][x][y].set(rot_x * cos(2*3.14*(player)/3) - rot_y*sin(2*3.14*(player)/3)+ CENTER_X,
						rot_x * sin(2*3.14*(player)/3) + rot_y*cos(2*3.14*(player)/3)+ CENTER_Y);
				std::cout <<"ro_x1:" << rot_x << std::endl;
				std::cout <<"ro_x2:" << rot_x << std::endl;
				board[player][7-x][y].set(rot_x_2 * cos(2*3.14*(player)/3) - rot_y*sin(2*3.14*(player)/3)+ CENTER_X,
						rot_x_2 * sin(2*3.14*(player)/3) + rot_y*cos(2*3.14*(player)/3)+ CENTER_Y);

			}
		}
	}
	for (int i = 0; i< 48; i+=16)
	{
		figures[i] = new Tower(i/16 + 1 );
		figures[i+1] = new Jumper(i/16 + 1 );
		figures[i+2] = new Bishop(i/16 + 1 );
		figures[i+3] = new King(i/16 + 1 );
		figures[i+4] = new Queen(i/16 + 1 );
		figures[i+5] = new Bishop(i/16 + 1 );
		figures[i+6] = new Jumper(i/16 + 1 );
		figures[i+7] = new Tower(i/16 + 1 );
		for (int j = 0; j < 8; j++)
			figures[i+8+j] = new Pawn(i/16 + 1 );
	}//nacitane vsetky spravne so svojimi tokenmi
	for ( int i = 0; i< 96 ; i++)
		access[i] = false;
	for (int i = 0; i < 3; i++)
		for(int j = 0;j< 8; j++)
			for (int k= 0; k< 2; k++)
			{
//				std::cout << "i,j,k "<< i << j <<k << "cislo figurky:" << 16*i + 8+k +j <<std::endl;
				board[i][j][k].occupy(i*16 + k*8 + j ); //
			}
	for (int i =0; i< NUM_OF_PLACES;i++) //vypocitaj pre vsetky figurky, ake policka pokryvaju
	{
	}
}
void Board::suggest_figure(int ID, int p, int xx, int yy)
{
	SDL_Rect rect;
	rect.x =0; rect.y= 0;
	figures[ID]->unchoosed();
	rect.x = board[p][xx][yy].get_x()-TOKEN_WIDTH/2;
	rect.y = board[p][xx][yy].get_y()-TOKEN_HEIGTH/2;
	printf("%d [%d %d %d] %d %d\n",ID, p,xx,yy,board[p][xx][yy].get_x(),board[p][xx][yy].get_y());
	SDL_SetAlpha(figures[ID]->display(), SDL_SRCALPHA, 100);
	SDL_SetColorKey(figures[ID]->display(),SDL_SRCCOLORKEY, SDL_MapRGB(figures[ID]->display()->format,255,255,255));
	SDL_BlitSurface(figures[ID]->display(),NULL,screen, &rect);
	SDL_SetAlpha(figures[ID]->display_name(), SDL_SRCALPHA, 100);
	SDL_SetColorKey(figures[ID]->display_name(),SDL_SRCCOLORKEY, SDL_MapRGB(figures[ID]->display_name()->format,255,255,255));
	SDL_BlitSurface(figures[ID]->display_name(),NULL,screen, &rect);
	figures[ID]->choosed();
}
void Board::display_figure(int ID, int p, int xx, int yy)
{
	SDL_Rect rect;
	rect.x =0; rect.y= 0;
	rect.x = board[p][xx][yy].get_x()-TOKEN_WIDTH/2;
	rect.y = board[p][xx][yy].get_y()-TOKEN_HEIGTH/2;
	printf("%d [%d %d %d] %d %d\n",ID, p,xx,yy,board[p][xx][yy].get_x(),board[p][xx][yy].get_y());
	SDL_SetAlpha(figures[ID]->display(), 0, 0);
	SDL_SetColorKey(figures[ID]->display(),SDL_SRCCOLORKEY, SDL_MapRGB(figures[ID]->display()->format,255,255,255));
	SDL_BlitSurface(figures[ID]->display(),NULL,screen, &rect);
	SDL_SetAlpha(figures[ID]->display_name(), 0, 0);
	SDL_SetColorKey(figures[ID]->display_name(),SDL_SRCCOLORKEY, SDL_MapRGB(figures[ID]->display_name()->format,255,255,255));
	SDL_BlitSurface(figures[ID]->display_name(),NULL,screen, &rect);
}
void Board::draw_board()
{
	SDL_Rect rect;
	rect.x =0; rect.y= 0;
	SDL_BlitSurface(board_img, NULL,screen, &rect);
	for (int p =0;p<3; p++)
		for(int xx = 0; xx < 8; xx++) 
			for(int y = 0; y< 4; y++)
			{     
				int ID = board[p][xx][y].occupied();
				if ( ID >= 0 )
				{
					//std::cout << ID <<std::endl;
					display_figure(ID, p, xx,y);
				}
			}
	SDL_Flip(screen);
}

void Board::pick_up_figure(int x, int y)
{
	printf("Suradnice %d %d\n", x,y);
	int player, x_coord, y_coord, x_old, y_old, ID, choosed_ID = -1;
	x_coord = x;
	y_coord = y;
	bool found = false;
	for(int i = PLAYERS - 1; i >= 0; i--)
	{
		x_old = x_coord;
		y_old = y_coord;
		x_coord = (x_old -CENTER_X)*( cos(2*3.14/3)) - (y_old - CENTER_Y) * (sin(2*3.14/3)) + CENTER_X; //otacanie v opacnom smere
		y_coord = (x_old -CENTER_X)*( sin(2*3.14/3)) + (y_old - CENTER_Y) * (cos(2*3.14/3)) + CENTER_Y; //Ako prvy bude player 3
		for(int j = 0; j < BOARD_X_MAX; j++)
		{
			int diff_x = board[0][j][0].get_x() - x_coord; 
			//printf("\tdiff : %d, [%d %d 0] %d %d \n",diff_x, i, j, board[i][j][0].get_x(), x_coord);

			if ((diff_x <= TOKEN_WIDTH / 2)&&(diff_x>= -TOKEN_WIDTH/2)) //ak je rozdiel X dostatocne maly, potom sme kde na tejto strane	
				for (int k = 0; k < BOARD_Y_MAX; k++)
				{
					int diff =board[0][j][k].get_y() - y_coord;  
					if (( diff <= TOKEN_HEIGTH/2)&&(diff>= -TOKEN_HEIGTH/2))
					{
						found = true;
						player = i;
						x = j;
						y = k;
						ID = board[player][x][y].occupied();
						if (choosed_p >-1) choosed_ID = board[choosed_p][choosed_x][choosed_y].occupied();
					}
					if (found) break;
				}
			if (found) break;
		}
		if (found) break;
	}
	if (found)
	{
	/*	if ( (choosed_p > 0 ) && attack(board[choosed_p][choosed_x][choosed_y].occupied(),player,x,y) ) //je potrebne sa pohnut;)
		{
			remove(player,x,y);///whatever was on the place
			remove(choosed_p,choosed_x, choosed_y);
			(pla);
		}*/
		if ( ID  ==  choosed_ID)
		{
			printf("unchoosing");
			figures[ID]->unchoosed();
			//display_figure(board[choosed_p][choosed_x][choosed_y].occupied(), choosed_p, choosed_x, choosed_y);
			draw_board();
			choosed_x = choosed_y = choosed_p = -1;
		}
		else 
		{	printf("%d %d %d %d",ID,player, x, y);
			figures[ID]->choosed();
			if(choosed_p !=-1)
			{
				figures[choosed_ID]->unchoosed();
				display_figure(choosed_ID, choosed_p, choosed_x, choosed_y);
				figures[board[choosed_p][choosed_x][choosed_y].occupied()]->unchoosed();
			}
			display_figure(ID , player, x, y);
			display_move(player, x,y);
			choosed_x = x;
			choosed_y = y;
			choosed_p = player;
		}
	}
	SDL_Flip(screen);
}
void Board::display_move(int player_part, int x, int y)
{
	int ID = this->board[player_part][x][y].occupied();
	int type = figures[ID]->move();
	switch (type)
	{
		case KING:
			printf("King\n");
			//suggest_move_king(int player_part, int x, int y);
			break;
		case QUEEN:
			printf("Queen\n");
			//suggest_move_queen(int player_part, int x, int y);
			break;
		case BISHOP:
			printf("Bishop\n");
			//suggest_move_bishop(int player_part, int x, int y);
			break;
		case JUMPER:
			printf("Jumper\n");
			//suggest_move_jumper(int player_part, int x, int y);
			break;
		case TOWER:
			printf("Tower\n");
			//suggest_move_tower(int player_part, int x, int y);
			break;
		case PAWN:
			printf("Pawn\n");
			suggest_move_pawn(player_part, x, y);
			break;
		default:
			printf("Bola mi podhodena neznama figurka!\n");
	}
}
void Board::suggest_move_pawn(int player_part,int x, int y)
{
	int ID = board[player_part][x][y].occupied();
	if (!figures[ID]->moved()) //moze byt este o jedno policko
	{
		suggest_figure(ID,player_part,x,y+2);
	}
	if (player_part == figures[ID]->owner) //ak je na svojom vlastnom hristi, checkuj, ci sa moze pohnut o policko
	{
		y++;
	}
	suggest_figure(ID,player_part,x,y);
}
