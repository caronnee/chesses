#include "./board.h"
#define CENTER_X 213 //TODO spravit to ako image->w
#define CENTER_Y 200 //	stejne
#define NUM_OF_PLACES 48
#define RADIUS 200

DoubleLinkItem::DoubleLinkItem(int val)
{ value = val;next = this; prev = this;}
DoubleLink::DoubleLink()
{
	head = NULL;
}
void DoubleLink::add(int val)
{
	DoubleLinkItem * it = new DoubleLinkItem(val);
	if (head == NULL)
	{
		head = it;
		return;
	}
	DoubleLinkItem *last = head->prev;
	last->next = it;
	it->next = head;
	head->prev = it;
	it->prev = last;
}

void DoubleLink::remove()
{
	if (head->next == head)
	{
		delete head;
		head = NULL;
		return;
	}
	DoubleLinkItem * it = head;
	next();
	it->prev->next = it->next;
	it->next->prev = it->prev;
	delete it;
}
void DoubleLink::next()
{
	head = head->next;
}
int DoubleLink::val()
{
	return head->value;
}

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

SDL_Surface * Figure::display()
{
	return display_token;
}

SDL_Surface * Figure::display_name()
{
	if (owner>=0)
		return name;
	return display_token;
}

Figure::Figure()
{
	moved = -1;
	under_attack = false;
	owner = ~0; //este nikto nemam zabookovanu figurku
	active = true;
	display_token = NULL;
	choose = SDL_LoadBMP("./images/selected.bmp");
	public_owner = -1;
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
	Triple old_position = legal_positions[0];
//	std::cout << old_position.x <<","<<old_position.y <<"," << old_position.z <<std::endl;
	(*g)[old_position].occupy(NULL);
	{
		Figure *f =(*g)[NewPosition].occupied();
		if (f!=NULL) 
		{
			f->clear(g);
			f->active = false;
		}
	}
	legal_positions[0] = NewPosition;
	(*g)[NewPosition].occupy(this);
	moved++; //neviem, kto utoci, ale policko je pokryte, ak je suma >0
	return true;
}
void Figure::clear(Gameboard * g)
{
	Triple old_position = legal_positions[0];
	std::vector<Triple> t = threats();
	for (unsigned int i =0; i< t.size(); i++)
	{
		(*g)[t[i]].sum--;
		(*g)[t[i]].players_attack[owner]--;
	} //moze tam byt aj viac krat, vzdy sa to odcita
}
void Figure::recheck(Gameboard * g)
{
	clear(g);
	Triple pos = legal_positions[0];
	legal_positions.clear();
	legal_positions.push_back(pos);
	check(g);
	std::vector<Triple> t = threats();
	for (unsigned int i =0; i< t.size(); i++)
	{
		(*g)[t[i]].sum++;
		(*g)[t[i]].players_attack[owner]++;
	}
}

Place::Place()
{
	occupied_by = NULL;
	coord_x = 0;
	coord_y = 0; 
	for (unsigned int i =0; i< PLAYERS; i++)
		players_attack[i] = 0;
	sum = 0;
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
	public_owner = 1 << owner;
	std::stringstream out;
	out <<owner;
	std::string s = "./images/token";
	s+=out.str();
	s+=".bmp";
	token = SDL_LoadBMP(s.c_str());
	name =  SDL_LoadBMP("./images/king.bmp");
	if (name == NULL) std::cerr << "King not found!" <<std::endl;
	if (token ==NULL) std::cerr << "Token image not loaded!" <<std::endl;
	display_token = token;
	legal_positions.push_back(t);
}
void King::check(Gameboard *g)
{
	Triple pos =  legal_positions[0];
	Figure * f = NULL;
	if (pos.y == 3)
	{
		Triple n = pos; //moze na kazdu stranu
		if (pos.x < 4)
		{
			n.x = 7-pos.x;
			n.z = (n.z+1)%3;
			for (unsigned int i =1; i<=2; i++)
			{
				if (n.x<0) continue;
				f = (*g)[n].occupied();
				if ((f == NULL) || (f->public_owner & ~public_owner))
					legal_positions.push_back(n);
				n.x++;
			}
			n.x = 7-pos.x-1;
			f = (*g)[n].occupied();
			if ((f == NULL) || (f->public_owner & ~public_owner))
				legal_positions.push_back(n);
		}
		else
		{
			n.x = 7-pos.x;
			n.z = (pos.z+2)%3;
			for (unsigned int i =1; i<=2; i++)
			{
				if (n.x<0) continue;
				f = (*g)[n].occupied();
				if ((f == NULL) || (f->public_owner & ~public_owner))
					legal_positions.push_back(n);
				n.x--;
			}
			n.x = 7-pos.x+1;
			f = (*g)[n].occupied();
			if ((f == NULL) || (f->public_owner & ~public_owner))
				legal_positions.push_back(n);
		}
		if ((pos.x == 3)||(pos.x==4))
		{
			n = pos;
			n.z++;
			if (pos.x == 3)
				n.z++;
			n.z%=3;
			for (n.x =3; n.x<=4; n.x++)
			{
				f = (*g)[n].occupied();
				if ((f == NULL) || (f->public_owner & ~public_owner))
					legal_positions.push_back(n);
			}
		}
	}
	for (int i = -1; i<2; i++)
		for (int j = -1; j<2; j++)
		{
			Triple n(pos.x+i, pos.y +j, pos.z);
			if ((n.x <0) || (n.y < 0)|| (n.y >= BOARD_Y_MAX)||( n.x >= BOARD_X_MAX))
				continue;
			f = (*g)[n].occupied();
			if((f== NULL) || (f->public_owner & ~public_owner))
				legal_positions.push_back(n);
		}
	unsigned int iter = 1;
//	std::cout << "\t"<<owner<<"\t" <<std::endl;
	while (iter < legal_positions.size())
	{
//		std::cerr <<(*g)[legal_positions[iter]].sum<<":"<<(*g)[legal_positions[iter]].players_attack[owner]<<"("
//			<<legal_positions[iter].x<<","<< legal_positions[iter].y<<","<< legal_positions[iter].z<<std::endl;
/*		std::cerr <<" === "<<(*g)[legal_positions[iter]].players_attack[0];
		std::cerr <<" === "<<(*g)[legal_positions[iter]].players_attack[1];
		std::cerr <<" === "<<(*g)[legal_positions[iter]].players_attack[2]<<std::endl;*/
		if ((*g)[legal_positions[iter]].sum > (*g)[legal_positions[iter]].players_attack[owner])
		{
			legal_positions.erase(legal_positions.begin()+iter);
		}
		else iter++;
	}
//	std::cout << "!!!!!!" <<std::endl<<std::endl;
}
Queen::Queen( Triple t)
{	
	owner = t.z;
	public_owner = 1<< owner;
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
	bishop(g);
	tower (g);
}//TODO tu sa naplna, co kral moze ohrozit a kam sa pohnut
Bishop::Bishop( Triple t)
{	
	owner = t.z;
	public_owner = 1<< owner;
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
	bishop(g);
}
void Figure::bishop(Gameboard *g)
{
	Triple pos = legal_positions[0];
	Triple n = pos;
	Figure * f = NULL;
	if (pos.y == 3)
	{
		if (pos.x == 3)
		{
			n.z = (n.z+2)%3;
		}
		if (pos.x == 4)
		{
			n.z = (n.z+1)%3;
		}
		f = (*g)[n].occupied();
		if ((f==NULL)||(f->public_owner & ~public_owner))
			legal_positions.push_back(n);
	}
	n = pos;
	while(true)
	{
		if (n.x+1 == BOARD_X_MAX)
			break;
		if (n.y+1 == BOARD_Y_MAX)
		{
			if (n.x < 4)
			{
				n.z = (n.z +1)%3;
				n.x = 7-n.x +1;
				if (n.x >= BOARD_X_MAX)
					break;
			}
			else 
			{
				n.z = (n.z +2)%3;
				n.x = 7-n.x +1;
				if (n.x < 0)
					break;
			}
			f = (*g)[n].occupied();
			if((f==NULL)||(f->public_owner & ~public_owner))
				legal_positions.push_back(n);
			break;
		}
		n.x++; n.y++;
		f = (*g)[n].occupied();
		if(f == NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if(f->public_owner & ~public_owner)
		{
			legal_positions.push_back(n);
		}
		break;
	}
	n = pos;
	while(true)
	{
		if (n.x == 0)
			break;
		if (n.y+1 == BOARD_Y_MAX)
		{
			if (n.x < 4)
			{
				n.z = (n.z +1)%3;
				n.x = 7-n.x -1;
				if (n.x < 0)
					break;
			}
			else 
			{
				n.z = (n.z +2)%3;
				n.x = 7-n.x +1;
				if (n.x >= BOARD_X_MAX)
					break;
			}
			f = (*g)[n].occupied();
			if((f==NULL)||(f->public_owner & ~public_owner))
				legal_positions.push_back(n);
			break;
		}
		n.x--; n.y++;
		f = (*g)[n].occupied();
		if(f == NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if(f->public_owner& ~public_owner)
		{
			legal_positions.push_back(n);
		}
		break;
	}
	n = pos;
	while(true)
	{
		if ((n.x+1 == BOARD_X_MAX)||((n.y-1)< 0))
			break;
		n.x++; n.y--;
		f = (*g)[n].occupied();
		if(f == NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if (f->public_owner & ~public_owner)
		{
			legal_positions.push_back(n);
		}
		break;
	}
	n = pos;
	while(true)
	{
		if ((n.x-1< 0)||(n.y-1< 0))
			break;
		n.x--; n.y--;
		f = (*g)[n].occupied();
		if(f == NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if (f->public_owner & ~public_owner)
		{
			legal_positions.push_back(n);
		}
		break;
	}
}

Jumper::Jumper( Triple t)
{	
	owner = t.z;
	public_owner = 1<< owner;
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
	Triple n = legal_positions[0];
	Figure * f =NULL;
	std::vector<Triple> v;
	int odd = 2;
	for (int i = -2; i<=2; i++)
	{
		if (i==0) continue;

		odd = (4+i)%2+1; //u jednicky bude zase dvojka, ako bola predtym
		v.push_back(Triple(n.x+i,n.y+odd, n.z));
		v.push_back(Triple(n.x+i,n.y-odd, n.z));
		v.push_back(Triple(n.x+odd,n.y+i, n.z));
		v.push_back(Triple(n.x-odd,n.y+i, n.z));
	}
	for (unsigned int i =0; i< v.size(); i++)
	{
		if((v[i].x < 0) || (v[i].y<0) || (v[i].x >= BOARD_X_MAX)) 
			continue;
		if (v[i].y>3)
		{
			int pom;
			if(v[i].x <4 )
			{
				v[i].z = (v[i].z +1)%3;
				pom = 1;
			}
			else
			{
				v[i].z = (v[i].z +2)%3;
				pom =2;
			}
			v[i].y = 7 - v[i].y;
			v[i].x = 7 - v[i].x;
			if ((n.y == 3)&&(n.x >=2)) //je najvyssie mozne
			{
				v.push_back(v[i]);
				v[i].z = (v[i].z+pom)%3;
				if (((n.x>3)&&(pom == 2))
					|| ((n.x <4)&&(pom == 1)))
				{
					if ((n.x == 3) || (n.x == 4))
					{
						v[i].x =7- n.x;
						v[i].y = 3;
					}
					else continue;
				}
			}
		}
		f = (*g)[v[i]].occupied();
		if ((f==NULL)||(f->public_owner & ~public_owner))
			legal_positions.push_back(v[i]);
	}
}
Tower::Tower( Triple t)
{	
	owner = t.z;
	public_owner = 1<< owner;
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
	tower(g);
}
void Figure::tower(Gameboard * g)
{
	//zvysujeme x az kym figura,0 alebo 3
	Triple pos =  legal_positions[0];
	Triple n = pos;
	Figure * f = NULL; 
	while (true)
	{
		if (n.x +1 == BOARD_X_MAX)
			break;
		n.x++;
		f = (*g)[n].occupied();
		if (f ==NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if (f->public_owner & ~public_owner)
			legal_positions.push_back(n);
		break;
	}

	n = pos;
	while (true)
	{
		if (n.x -1< 0)
			break;
		n.x--;
		f = (*g)[n].occupied();
		if (f ==NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if (f->public_owner & ~public_owner)
			legal_positions.push_back(n);
		break;

	}
	n = pos;
	while (true)
	{
		if (n.y -1< 0)
			break;
		n.y--;
		f = (*g)[n].occupied();
		if (f ==NULL)
		{
			legal_positions.push_back(n);
			continue;
		}
		if (f->public_owner & ~public_owner)
			legal_positions.push_back(n);
		break;
	}
	n = pos;
	if (pos.y == 3)//moze aj do dalsej
	{
		int pom = (pos.x <4)?2:1;
		n.z = (n.z+pom)%3;
		n.x =5-pom;
		f = (*g)[n].occupied();
		if ((f == NULL) || (f->public_owner & ~public_owner))
			legal_positions.push_back(n);
	}
	n = pos;
	while (true)
	{
		if (n.y + 1 == BOARD_Y_MAX)
		{
			n.x = 7-n.x;
			if (n.x < 4 )
			{
				n.z = (n.z +2)%3 ;//jednym smerom
			}
			else
			{
				n.z = (n.z +1)%3 ;//jednym smerom
			}
			f = (*g)[n].occupied();
			if ((f==NULL)||(f->public_owner & ~public_owner))
				legal_positions.push_back(n);
			break;
		}
		n.y++;
		f = (*g)[n].occupied();
		if (f ==NULL)
		{
		legal_positions.push_back(n);
		continue;
		}
		if (f->public_owner & ~public_owner)
			legal_positions.push_back(n);
		break;
	}
}
Pawn::Pawn( Triple t)
{
	owner = t.z;
	public_owner = 1<< owner;
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
	Triple pos = legal_positions[0];
	threats_.clear();
	Triple n = pos;

	n = pos;
	if (pos.z == owner)
		n.y = pos.y + 1;
	else 
		n.y = pos.y - 1;
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
	if((*g)[n].occupied() == NULL) 
	{
		legal_positions.push_back(n);
		if (moved <= 0)
		{
			n.y++;
			if((*g)[n].occupied()== NULL)
				legal_positions.push_back(n);
			n.y--;
		}
	}

	Triple threat=n; //to posledne, nie je to specialne, iba normalny pohyb, tke ok
	if ((n.x +1)< BOARD_X_MAX)
	{
		threat.x++;
		threats_.push_back(threat);
		if (threat.x == 4)
		{
		//	threat.x = 7-threat.x;
			if ((pos.y ==3)&&(pos.z==owner)) threat.z = (threat.z +2)%3;
			threats_.push_back(threat);
		}
	}
	threat = n;
	if ((n.x -1)> 0)
	{
		threat.x--;
		threats_.push_back(threat);
		if (threat.x ==3)
		{
			if ((pos.y == 3)&&(pos.z == owner)) threat.z = (threat.z +1)%3;
			threats_.push_back(threat);
		}
	}
	for (unsigned int i =0; i< threats_.size(); i++)
	{
		Figure * f = (*g)[threats_[i]].occupied();
		if ((f!=NULL) && (public_owner & ~f->public_owner))
			legal_positions.push_back(threats_[i]);
	}
}

std::vector<Triple> Pawn::threats()
{
	return threats_;
}

Board::Board()
{
	win = -1;
	moved = false;
	float size_x = RADIUS/8;//velkost nasho kroku
	float coord_x, coord_y_l, coord_y_l_last;
	int heigth = RADIUS * 1.73 /2; //nasa vyska
	int base = CENTER_Y + heigth;
	coord_y_l = 1.83*RADIUS / 3;//vyska toho najposlednejsie (najviac vlavo)
	board_img = SDL_LoadBMP("./images/chessboard.bmp");
	choosed = -1;
	for (int i =0; i< 3; i++)
		on_turn.add(i);
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
		{
			figures[i+8+j] = new Pawn( Triple(j,1,owner));
		}
		
	}//nacitane vsetky spravne so svojimi tokenmi
	for (int i =0; i< 48; i++)
	{
		board[figures[i]->moves()[0]].occupy(figures[i]);
	}
		
/*	for(int i = 47; i>=0; i--)
	{
		Triple pos = figures[i]->moves()[0];
		figures[i]->move(&board,pos);
//		std::cout << i << ":moved" << figures[i]->moved <<std::endl;
	}*/
/*	Figure * f = new Pawn(Triple(3,1,0));
	Triple pos = f->moves()[0];
	f->move(&board,pos);
	*/
	for (int i =0; i< 48;i++) //vypocitaj pre vsetky figurky, ake policka pokryvaju
	{
		figures[i]->check(&board); //nastavi aj figuram
	}
}
void Board:: reset()
{
	std::cerr <<"boma";
	for (int i =0; i< 48; i++)
	{
		delete figures[i];
	}
	for (int i =0; i< PLAYERS; i++)
	{
		for (int j =0; j< 4; j++)
			for (int x = 0; x < 8; x++)
				board.board[i][x][j].reset();
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
		{
			figures[i+8+j] = new Pawn( Triple(j,1,owner));
		}

	}//nacitane vsetky spravne so svojimi tokenmi
	for (int i =0; i< 48; i++)
	{
		board[figures[i]->moves()[0]].occupy(figures[i]);
	}
	for (int i =0; i< 48;i++) //vypocitaj pre vsetky figurky, ake policka pokryvaju
	{
		figures[i]->check(&board); //nastavi aj figuram
	}
	draw_board();
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
					display_figure(Triple(xx,y,p));
				}
			}
	SDL_Flip(screen);
}

bool Board::pick_up_figure(int x, int y)
{
//	printf("Suradnice %d %d\n", x,y);
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
					}
					if (found) break;
				}
			if (found) break;
		}
		if (found) break;
	}

	if (!found) return false;
	return pick_up_figure(new_choose);
}

bool Board::pick_up_figure(Triple new_choose)
{	
	if (choosed.z == -1) //este sme nic nevybrali
	{
		Figure * f = board[new_choose].occupied();
		if (( f == NULL)||(f->public_owner & ~(1<< on_turn.val()))||(!f->active))
			return false;
		f->choosed();
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
			moved = true;
			last_move.first = choosed;
			last_move.second = new_choose;
			draw_board();
			Figure * f2 = board[new_choose].occupied();
			if (f2!=NULL) //a sucasne je owner na tahu
			{
				if (!f2->active) return false;
				if (f2->public_owner & ~f->public_owner) return false;
				f2->choosed();
				display_figure(new_choose);
				display_move(new_choose);
				choosed = new_choose;
			}
			else choosed = -1;
		}
		else 
		{
			if (f->moves().size() == 0)
			{
				//promote!
				bool promoted = false;
				int ID = 0;
				for (ID = 0; ID< 48; ID++)
					if (figures[ID]==f)
						break;
				while (!promoted)
				{
				SDL_Event e;
				SDL_WaitEvent(&e);
				switch (e.type)
				{
					case SDL_KEYDOWN:
						switch(e.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								return false;
								break;

							case SDLK_q:
								promoted = true;
								f->clear(&board);
								delete f;
								figures[ID] = new Queen(new_choose);
								break;
							case SDLK_t:
								promoted = true;
								f->clear(&board);
								delete f;
								figures[ID] = new Tower(new_choose);
								break;
							case SDLK_j:
								promoted = true;
								f->clear(&board);
								delete f;
								figures[ID] = new Jumper(new_choose);
								break;			
							case SDLK_b:
								promoted = true;
								f->clear(&board);
								delete f;
								figures[ID] = new Bishop(new_choose);
								break;
							default:
								break;
						}
						break;
					case SDL_QUIT:
						return false;
						break;

				}
				}
			}
			choosed = -1;
			for (int i =0; i< 48; i++)
			{
				figures[i]->recheck(&board);
			}
			for (int i = 0; i<PLAYERS; i++)
			{
				figures[16*i + 3]->recheck(&board);
			}
			Triple ooo(4,1,0);
			std::cerr <<" \t "<<board[ooo].sum<<std::endl;
			std::cerr <<" === "<<board[ooo].players_attack[0];
			std::cerr <<" === "<<board[ooo].players_attack[1];
			std::cerr <<" === "<<board[ooo].players_attack[2]<<std::endl;
			
			on_turn.next();
			Triple attacked = figures[16*on_turn.val() +3]->moves()[0];
			if (board[attacked].sum - board[attacked].players_attack[on_turn.val()] > 0)
				if (!possible_avoid(attacked))
				{
					for (int i = 0; i<16; i++)
					{
						figures[16*on_turn.val() + i]->remove(&board);
					}
				}
			draw_board();
		}
	}
	SDL_Flip(screen);
	return false;
}
void Figure::remove(Gameboard * g)
{
	std::cout << "removing!" <<std::endl;
	clear(g);
	public_owner = ~0;
	owner = -1;
}
void Board::display_move(Triple pos)
{
	Figure * f = board[pos].occupied();
	std::vector<Triple> move = f->moves();
	for (unsigned int i =1; i< move.size(); i++) //na nultej pozicii ma ulozene svoje veci
	{
		suggest_figure(f, move[i]);
	}
}

bool Board::possible_avoid(Triple t)
{
	std::cout << "tu nenam zatial co robit"<<std::endl;
	//ak sa ma kde pohnut kral
	Figure *f = board[t].occupied();
	if (f->moves().size()>0)
		return true;
	//ak policko ohrozuje iba jedna figurka a niekto moze blokovat
	int o;
	for (o =0; o< PLAYERS; o++)
		if ( (1<o) & f->public_owner)
			break;
	int at = board[t].sum - board[t].players_attack[o];
	if (at == 2)
		return false;
	//ak to niek to moze zblokovat
	for(unsigned int i = 0; i< 16; i++)
	{
		if (!figures[on_turn.val()*16 + i]->active)
			continue;
		std::vector<Triple> m = figures[on_turn.val()*16 + i]->moves();
		Triple old_position = m[0];
		for (unsigned int a = 1; a < m.size(); a++)
		{
			f->move(&board,m[a]);
			for (unsigned int j =0; j< 48; j++)
			{
				figures[j]->recheck(&board);
			}
			if(board[t].sum == board[t].players_attack[o])
			{
				f->moved--;
				f->move(&board,old_position);
				for (unsigned int j =0; j< 48; j++)
				{
					figures[j]->recheck(&board);
				}
				return true;
			}
			f->moved--;
		}
		f->move(&board,old_position);
		for (unsigned int j =0; j< 48; j++)
		{
			figures[j]->recheck(&board);
		}
	}
	return false;
}
Board::~Board()
{
	for(int i =0; i< 48; i++)
		delete figures[i];
	SDL_FreeSurface(board_img);
}
Figure::~Figure()
{
	SDL_FreeSurface(name);
	SDL_FreeSurface(token);
//	SDL_FreeSurface(display_token);
	SDL_FreeSurface(choose);
}
Pawn::~Pawn(){}
King::~King(){}
Queen::~Queen(){}
Bishop::~Bishop(){}
Jumper::~Jumper(){}
Tower::~Tower(){}

void Place::reset()
{
	occupied_by = NULL;
	sum = NULL;
	for (int i =0; i < PLAYERS; i++)
		players_attack[i] = 0;
}
