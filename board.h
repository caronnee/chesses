#include <SDL/SDL.h>
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
class Place
{
	//Jedno policko moze byt jednych hracom pokryte mac 16 figurakmi, tx 1<<4-1 je prvy hrac 1<<8-1&1<<4-1 druhy atd
	int coord_x,coord_y; //suradnice stredu policka, kde sa to moze napasovat + kde sa bue vyznacovat krizik a pod.
	int occupied_by; //ci na nom nejaka figurka stoji alebo niei a jej ID
public:
	bool attackers[16*3];//pre kazde Place je zoznam toho, co ho ohrozuje, stac ID
	int under_attack; //kolko figurok utoci na dane policko
	int get_x();
	int get_y();
	void occupy(int ID);
	int occupied();
	void set(int x, int y);//coordinates spocitane boardom
	Place();
};

class Figure
{
protected:
	bool active;
	bool moved_;
	int under_attack;
	SDL_Surface * token;
	SDL_Surface * display_token;
	SDL_Surface * name;
	SDL_Surface * choose;
public:
	int owner; //vlastnik figurky vzhadom na hraca pri pocitaci (1 aktualny hrac, 2 hrac druhy v poradi atd)
	virtual int move() = 0; //Ktory pohyb ma vykonavat board
	void choosed(); // ak vybrana, zmeni sa jej vyzor..trosku
	void unchoosed();
	bool moved();
	void remove(); //stane sa nej neaktivna, TODO ak by sa mali aj ukazovat - vlastnost boardu
	SDL_Surface * display();
	SDL_Surface * display_name();
	Figure();
};

class King : public Figure
{
public:
	King(int o);
	virtual int move();
};

class Queen : public Figure
{
public:
	Queen(int o);
	virtual int move();
};

class Bishop : public Figure
{
public:
	Bishop(int o);
	virtual int move();
};

class Jumper : public Figure
{
public:
	Jumper(int o);
	virtual int move();
};

class Tower : public Figure
{
public:
	Tower(int o);
	virtual int move();
};

class Pawn : public Figure
{
	int promotion; //ked sa changne, potom iba zmeni zposob, akym sa pohybuje a svoje oismeno
public:
	Pawn(int o);
	virtual int move();
};

class Board
{
	int player_on_turn;
	SDL_Surface * board_img;
	int choosed_x, choosed_y, choosed_p;
	Place board[PLAYERS][BOARD_X_MAX][BOARD_Y_MAX]; // pri prenasani sa bude menit prva suradnica podla toho, aky hrac to ma obdrzat
	Figure* figures[48];
	bool access[96];//vsetky policka, na ktore moze dana figurka, co je rpave na taku, skocit, budem potrebovat stale
	int turn; //kto je prave na rade
	int find_place(int x, int y); //vrati ID cislo figurky na policku so suradnicami x,y alebo -1, ak sa nenasla
	void display_figure(int ID, int p, int x, int y);
	void suggest_figure(int ID, int p, int x, int y);
	void suggest_move_king(int p, int y, int x);
	void suggest_move_queen(int p, int y, int x);
	void suggest_move_bishop(int p, int y, int x);
	void suggest_move_jumper(int p, int y, int x);
	void suggest_move_tower(int p, int y, int x);
	void suggest_move_pawn(int p,int y, int x); //samostany pohyb uz spravi chessboarda
	void display_move(int player, int x, int y);
public:
	Board();	
	void draw_board(); //prejde cez vsetky figurky, co su na boarde a splitne ich na spravne miesto
	void pick_up_figure(int x, int y); //vyberie danu figurku a zisti, kde vsade sa moze hybat, x, su mysove suradnice
};
