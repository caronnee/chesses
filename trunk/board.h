#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <math.h>

#define BOARD_HEIGHT 430
#define BOARD_WIDTH 430
#define TOKEN_WIDTH 26
#define TOKEN_HEIGTH 26
#define PLAYERS 3
#define BOARD_X_MAX 8
#define BOARD_Y_MAX 4

extern SDL_Surface * screen;

struct DoubleLinkItem
{
	DoubleLinkItem * next;
	DoubleLinkItem * prev;
	int value;
	DoubleLinkItem(int val);
};
struct DoubleLink
{
	DoubleLinkItem *head;
	DoubleLink();
	void add(int item);
	void remove(DoubleLinkItem *item);
};
struct Triple
{
	int x, y, z;
	Triple();
	Triple(int x,int y,int z);
	bool operator==(const Triple t);
	bool operator==(const int i);
	Triple& operator=(const int i);
};

class Figure;

class Place
{
	//Jedno policko moze byt jednych hracom pokryte mac 16 figurakmi, tx 1<<4-1 je prvy hrac 1<<8-1&1<<4-1 druhy atd
	int coord_x,coord_y; //suradnice stredu policka, kde sa to moze napasovat + kde sa bue vyznacovat krizik a pod.
	Figure * occupied_by; //ci na nom nejaka figurka stoji alebo niei a jej ID
public:
	int sum;
	int players_attack[3];
//	std::vector<Figure *> attackers;//pre kazde Place je zoznam toho, co ho ohrozuje, podla ID
	int get_x();
	int get_y();
	void occupy(Figure * ID);
	Figure * occupied();
	void set(int x, int y);//coordinates spocitane boardom
	Place();
	void reset();
};

struct Gameboard
{
	Place board[PLAYERS][BOARD_X_MAX][BOARD_Y_MAX]; // pri prenasani sa bude menit prva suradnica podla toho, aky hrac to ma obdrzat
	Place& operator[](const Triple t);
};

class Figure
{
protected:
	int owner; //vlastnik figurky vzhadom na hraca pri pocitaci (1 aktualny hrac, 2 hrac druhy v poradi atd, -1 = figurka je vyradena)
	bool under_attack; //ci je fugurka ohrozena, pomocne pri debugu
	std::vector<Triple> legal_positions;
	SDL_Surface * token;
	SDL_Surface * display_token;
	SDL_Surface * name;
	SDL_Surface * choose;
	void bishop(Gameboard *g);
	void tower(Gameboard *g);
public:
	void clear(Gameboard *g);
	void recheck(Gameboard * g);

	enum Tokens
	{
		KING, 
		QUEEN, 
		BISHOP, 
		JUMPER, 
		TOWER, 
		PAWN,
		NONE	
	};

	int public_owner;
	bool active; //ktora figurka to vlastne je
	std::vector<Triple> moves();
	bool move(Gameboard *g,Triple NewPosition); // checkne, ci sa tam da ist, spravi check
	virtual void check(Gameboard * g) = 0; //Kam vsade sa moze hybat
	virtual std::vector<Triple> threats(); //co vsetko ohrozuje
	void choosed(); // ak vybrana, zmeni sa jej vyzor..trosku
	void unchoosed();
	int moved; //figurka s pohla, dobre pre rosadu, pohyb pawna
	void remove(); //stane sa nej neaktivna, TODO ak by sa mali aj ukazovat - vlastnost boardu
	SDL_Surface * display();
	SDL_Surface * display_name();
	Figure();
	virtual ~Figure();
};


class King : public Figure
{
public:
	King( Triple t);
	virtual void check(Gameboard * g);
	virtual ~King();
};

class Queen : public Figure
{
public:
	Queen( Triple t);
	virtual void check(Gameboard * g);
	virtual ~Queen();
};

class Bishop : public Figure
{
public:
	Bishop( Triple t);
	virtual void check(Gameboard * g);
	virtual ~Bishop();
};

class Jumper : public Figure
{
public:
	Jumper( Triple t);
	virtual void check(Gameboard * g);
	virtual ~Jumper();
};

class Tower : public Figure
{
public:
	Tower( Triple t);
	virtual void check(Gameboard * g);
	virtual ~Tower();
};

class Pawn : public Figure
{
	std::vector<Triple> threats_;
public:
	Pawn( Triple t);
	virtual void check(Gameboard * g);
	virtual std::vector<Triple> threats();
	virtual ~Pawn();
};

class Board
{
	int win;
	SDL_Surface * board_img;
	Triple choosed;
	Gameboard board;
	Figure* figures[48];
	int turn; //kto je prave na rade
	void display_figure(Triple t);
	void suggest_figure(Figure *f, Triple t);
	void display_move(Triple t);
	bool possible_avoid(Triple t);
public:
	int player_on_turn;
	Board();	
	int get_player();
	void draw_board(); //prejde cez vsetky figurky, co su na boarde a splitne ich na spravne miesto
	bool pick_up_figure(int x, int y); //vyberie danu figurku a zisti, kde vsade sa moze hybat, x, su mysove suradnice
	bool pick_up_figure(Triple t); //vyberie danu figurku a zisti, kde vsade sa moze hybat, x, su mysove suradnice
	void reset();
	~Board();
};
