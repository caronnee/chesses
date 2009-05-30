#include "./board.h"
#define CENTER_X 213 //TODO spravit to ako image->w
#define CENTER_Y 200 //	stejne
#define NUM_OF_PLACES 48
#define RADIUS 200


Triple::Triple()
{
	x= y =z;
}
Triple::Triple(int x_,int y_,int z_)
{
	x = x_;
	y = y_;
	z = z_;
}
bool Triple::operator==(const Triple t)
{
	if ((t.x==x)&&(t.y == y)&&(t.z == z))
		return true;
	return false;
}
bool Triple::operator==(const int i)
{
	if ((x==i)||(y==i)||(z==i))
		return true;
	return false;
}
Triple& Triple::operator=(const int i)
{
	x=y=z=i;
	return *this;
}

Place& Gameboard::operator[](const Triple t)
{
	return board[t.z][t.x][t.y];
}

std::vector<Triple> Figure::moves()
{
	return legal_positions;
}

std::vector<Triple> Figure::threats()
{
	return legal_positions;
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
	under_attack = false;
	owner = -1; //este nikto nemam zabookovanu figurku
	active = true;
	display_token = NULL;
	choose = SDL_LoadBMP("./images/selected.bmp");
	//SDL_SetColorKey(choose,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
}

Figure * Place::occupied()
{
	return occupied_by;
}
bool Figure::move(Gameboard *g,Triple NewPosition) // checkne, ci sa tam da ist, spravi check
{
	//vymaz vsekty occupy, TODO
	//vsetkym, co sa da dane a cielove policko byt pozreli, sprav na ne check
	bool found = false;
	for (unsigned int i =0; i< legal_positions.size(); i++)
		if ( NewPosition == legal_positions[i])
		{
			found = true;
			break;
		}
	if (!found) return false;
	std::cout <<"dound!"<<std::endl;
	(*g)[legal_positions[0]].occupy(NULL);
	(*g)[NewPosition].occupy(this);
	legal_positions.clear();
	legal_positions.push_back(NewPosition);
	check(g);
	moved_ = true;
	return true;
}

//x.y su aktualne policka v dvojrozmernom poli
Place::Place()
{
	occupied_by = NULL;
	coord_x = 0;
	coord_y = 0; 
}
void Place::set(int x, int y)
{
	coord_x = x;
	coord_y = y;
}

void Place::occupy(Figure * ID)
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

King::King(Triple t)
{
	owner = t.z;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/king.bmp");
	if (name == NULL) std::cerr << "King not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
	legal_positions.push_back(t);
}

void King::check(Gameboard *g)
{
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut
Queen::Queen( Triple t)
{	
	owner = t.z;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/queen.bmp");
	if (name ==NULL) std::cerr << "Queen image not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
	legal_positions.push_back(t);
}

void Queen::check(Gameboard *g)
{
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut
Bishop::Bishop( Triple t)
{	
	owner = t.z;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/bishop.bmp");
	if (name ==NULL) std::cerr << "Bishop Image not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
	legal_positions.push_back(t);
}
void Bishop::check(Gameboard *g)
{
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut

Jumper::Jumper( Triple t)
{	
	owner = t.z;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/jumper.bmp");
	if (name ==NULL) std::cerr << "Jumper image not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
	legal_positions.push_back(t);
}

void Jumper::check(Gameboard *g)
{
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut
Tower::Tower( Triple t)
{	
	owner = t.z;
	std::stringstream out;
	out << owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/tower.bmp");
	if (name ==NULL) std::cerr << "Tower image not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	display_token = token;
	legal_positions.push_back(t);
}

void Tower::check(Gameboard *g)
{
	//zvysujeme x az kym figura,0 alebo 3
	Triple pos =  legal_positions[0];
	Triple n = pos;
	n.x++;
	while((*g)[n].occupied()==NULL)
	{
		if (n.x +1 == BOARD_X_MAX)
			break;
		n.x++;
		legal_positions.push_back(n);
	}
	Figure * f = (*g)[n].occupied();
	if ((f!=NULL)&&(f->owner!=owner))
		legal_positions.push_back(n);

	n = pos;
	n.x--;
	while((*g)[n].occupied()==NULL)
	{
		if (n.x +1 == BOARD_X_MAX)
			break;
		n.x++;
		legal_positions.push_back(n);
	}
	if ((f!=NULL)&&(f->owner!=owner))
		legal_positions.push_back(n);
}
Pawn::Pawn( Triple t)
{
	owner = t.z;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/pawn.bmp");
	if (name ==NULL) std::cerr << "Pawn image not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	//SDL_SetColorKey(token,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	//SDL_SetColorKey(name,SDL_SRCCOLORKEY, SDL_MapRGB(choose->format,125,125,125));
	legal_positions.push_back(t);
	display_token = token;
}

void Pawn::check(Gameboard *g)
{
	//do gameboardu sa napisu iba threats
//	std::cout << legal_positions[0].x << " " << legal_positions[0].y <<" "<< legal_positions[0].z <<std::endl;
	Triple pos = legal_positions[0];
	//TODO zlikvidovat attackers!
	threats_.clear();
	Triple n = pos;
	if (!moved_)
	{
		n.y +=2;
		if((*g)[n].occupied()== NULL)
			legal_positions.push_back(n);
	}
	if (pos.z == owner)
		n.y = pos.y+1;
	else 
		n.y = pos.y -1;
	if (pos.y == 0)//cas na promociu
		{std::cout << "Promotion! "<<std::endl;return;}
	if(n.y > 3)
	{
		n.y = 3;
		if (n.x<4)
		{
			n.z = (n.z + 1)%PLAYERS;
		}
		else
		{
			n.z = (n.z + 2)%PLAYERS;
		}
		n.x= 7 - n.x;

	}
	if((*g)[n].occupied() == NULL) legal_positions.push_back(n);
	Triple threat=n; //to posledne, nie je to specialne, iba normalny pohyb, tke ok
	if ((n.x +1)< BOARD_X_MAX)
	{
		threat.x++;
		threats_.push_back(threat);
	//	(*g)[threat].attackers.push_back(this);
	}
	threat = n;
	if ((n.x -1)> 0)
	{
		threat.x--;
		threats_.push_back(threat);
	//	(*g)[threat].attackers.push_back(this);
	}
	for (unsigned int i =0; i< threats_.size(); i++)
	{
		Figure * f = (*g)[threats_[i]].occupied();
		if ((f!=NULL) && (owner != f->owner))
			legal_positions.push_back(threats_[i]);
	}
}

std::vector<Triple> Pawn::threats()
{
	return threats_;
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut
Board::Board()
{
	float size_x = RADIUS/8;//velkost nasho kroku
	float coord_x, coord_y_l, coord_y_l_last;
	int heigth = RADIUS * 1.73 /2; //nasa vyska
	int base = CENTER_Y + heigth;
	coord_y_l = 1.83*RADIUS / 3;//vyska toho najposlednejsie (najviac vlavo)
	board_img = SDL_LoadBMP("./images/chessboard.bmp");
	choosed = -1;
	for (int x = 0; x<4; x++)
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
				board.board[player][x][y].set(rot_x * cos(2*3.14*(player)/3) - rot_y*sin(2*3.14*(player)/3)+ CENTER_X,
						rot_x * sin(2*3.14*(player)/3) + rot_y*cos(2*3.14*(player)/3)+ CENTER_Y);
				board.board[player][7-x][y].set(rot_x_2 * cos(2*3.14*(player)/3) - rot_y*sin(2*3.14*(player)/3)+ CENTER_X,
						rot_x_2 * sin(2*3.14*(player)/3) + rot_y*cos(2*3.14*(player)/3)+ CENTER_Y);

			}
		}
	}
	for (int i = 0; i< 48; i+=16)
	{
		int owner = i/16;
		figures[i] = new Tower(Triple(0,0,owner) );
		figures[i+1] = new Jumper(Triple(1,0,owner) );
		figures[i+2] = new Bishop(Triple(2,0,owner) );
		figures[i+3] = new King(Triple(3,0,owner) );
		figures[i+4] = new Queen(Triple(4,0,owner) );
		figures[i+5] = new Bishop(Triple(5,0,owner) );
		figures[i+6] = new Jumper(Triple(6,0,owner) );
		figures[i+7] = new Tower(Triple(7,0,owner) );
		for (int j = 0; j < 8; j++)
			figures[i+8+j] = new Pawn( Triple(j,1,owner));
	}//nacitane vsetky spravne so svojimi tokenmi
	for(unsigned int i = 0; i< 48; i++)
	{
		Triple pos = figures[i]->moves()[0];
		figures[i]->move(&board,pos);
	}
//	board.board[0][2][2].occupy( new Jumper(Triple(0,2,2)));
	for (int i =0; i< 48;i++) //vypocitaj pre vsetky figurky, ake policka pokryvaju
	{
		figures[i]->check(&board); //nastavi aj figuram
	}
}
void Board::suggest_figure(Figure *f,Triple t)
{
	SDL_Rect rect;
	f->unchoosed();//potrebuje povodne cosi
	rect.x =0; rect.y= 0;
	rect.x = board[t].get_x()-TOKEN_WIDTH/2;
	rect.y = board[t].get_y()-TOKEN_HEIGTH/2;
//	printf("%d [%d %d %d] %d %d\n",ID, p,xx,yy,board[p][xx][yy].get_x(),board[p][xx][yy].get_y());
	SDL_SetAlpha(f->display(), SDL_SRCALPHA, 100);
	SDL_SetColorKey(f->display(),SDL_SRCCOLORKEY, SDL_MapRGB(f->display()->format,255,255,255));
	SDL_BlitSurface(f->display(),NULL,screen, &rect);
	SDL_SetAlpha(f->display_name(), SDL_SRCALPHA, 100);
	SDL_SetColorKey(f->display_name(),SDL_SRCCOLORKEY, SDL_MapRGB(f->display_name()->format,255,255,255));
	SDL_BlitSurface(f->display_name(),NULL,screen, &rect);
	f->choosed();
}
void Board::display_figure(Triple t)
{
	SDL_Rect rect;
	Figure * f = board[t].occupied();
	rect.x =0; rect.y= 0;
	rect.x = board[t].get_x()-TOKEN_WIDTH/2;
	rect.y = board[t].get_y()-TOKEN_HEIGTH/2;
//	printf("%d [%d %d %d] %d %d\n",ID, p,xx,yy,board[p][xx][yy].get_x(),board[p][xx][yy].get_y());
	SDL_SetAlpha(f->display(), 0, 0);
	SDL_SetColorKey(f->display(),SDL_SRCCOLORKEY, SDL_MapRGB(f->display()->format,255,255,255));
	SDL_BlitSurface(f->display(),NULL,screen, &rect);
	SDL_SetAlpha(f->display_name(), 0, 0);
	SDL_SetColorKey(f->display_name(),SDL_SRCCOLORKEY, SDL_MapRGB(f->display_name()->format,255,255,255));
	SDL_BlitSurface(f->display_name(),NULL,screen, &rect);
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
				if ( board.board[p][xx][y].occupied()!=NULL )
				{
					//std::cout << ID <<std::endl;
					display_figure(Triple(xx,y,p));
				}
			}
	SDL_Flip(screen);
}

bool Board::pick_up_figure(int x, int y)
{
	printf("Suradnice %d %d\n", x,y);
	Triple new_choose;
	int x_coord, y_coord, x_old, y_old;
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
			int diff_x = board.board[0][j][0].get_x() - x_coord; 
			//printf("\tdiff : %d, [%d %d 0] %d %d \n",diff_x, i, j, board[i][j][0].get_x(), x_coord);
			if ((diff_x <= TOKEN_WIDTH / 2)&&(diff_x>= -TOKEN_WIDTH/2)) //ak je rozdiel X dostatocne maly, potom sme kde na tejto strane	
				for (int k = 0; k < BOARD_Y_MAX; k++)
				{
					int diff =board.board[0][j][k].get_y() - y_coord;  
					if (( diff <= TOKEN_HEIGTH/2)&&(diff>= -TOKEN_HEIGTH/2))
					{
						found = true;
						new_choose.z = i;
						new_choose.x = j;
						new_choose.y = k;
	//					if (board[new_choose].occupied() == NULL) new_choose = choosed;
	//					std::cout << "found!" << i <<" "<< j << " "<< k <<std::endl;
					}
					if (found) break;
				}
			if (found) break;
		}
		if (found) break;
	}

	if (!found) return false;
	if (choosed.z == -1) //este sme nic nevybrali
	{
		Figure * f = board[new_choose].occupied();
		if ( f == NULL)
			return false;
		f->choosed();
		std::cout << "xxx";
		display_figure(new_choose);
		display_move(new_choose);
		choosed = new_choose;
	}
	else if(choosed == new_choose)
	{
		board[choosed].occupied()->unchoosed();
		choosed = -1;
		draw_board();	
	}
	else 
	{
		Figure * f = board[choosed].occupied();
		f->unchoosed();
		if (!f->move(&board,new_choose)) //ak sa nepodarilo presunut,
		{
			draw_board();
			Figure * f2 = board[new_choose].occupied();
			if (f2!=NULL) //a sucasne je owner na tahu
			{
				f2->choosed();
				display_figure(new_choose);
				display_move(new_choose);
				choosed = new_choose;
			}
			else choosed = -1;
		}
		else 
		{
			choosed = -1;
			draw_board();
		}
	}


	/*if ((found)&&(new_choose.z !=-1))
	{
		if ( choosed  ==  new_choose)
		{
			board[choosed].occupied()->unchoosed();
			choosed = -1;
			draw_board();
		}
		else 
		{
			Figure * f = board[choosed].occupied();
			if(!f->move(&board,new_choose))
			{
				board[new_choose].occupied()->choosed();
				board[choosed].occupied()->unchoosed();
				draw_board();
				display_figure(new_choose);
				display_move(new_choose);
				choosed = new_choose;
			}
			else
			{
				choosed = -1;
				draw_board();
			}
		}
	}*/
	SDL_Flip(screen);
	return false;
}
void Board::display_move(Triple pos)
{
	Figure * f = board[pos].occupied();
	std::vector<Triple> move = f->moves();
	for (unsigned int i =1; i< move.size(); i++) //na nultej pozicii ma ulozene svoje veci
	{
		std::cout << "suggesting!" <<std::endl;
		suggest_figure(f, move[i]);
	}
}

