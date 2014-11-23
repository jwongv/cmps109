//Jacky Wong
//Michael Ungerer
//Hang Gao


#ifndef __HEXBOARD_H__
#define __HEXBOARD_H__

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>
#include "node.h"
using namespace std;

//hexboard class containing a 2D vector of hex nodes
class hexboard{
private:
   int size;                              //size of board
   float winchance;
   vector < vector <node> > matrix;       //matrix of the board

public:
   hexboard(int s);
   void print_board();
   void fill_board();
   void fill_turn(int x, int y, int player);
   bool check_empty(int x, int y);
   int find_player_winner();
   int find_ai_winner();
   friend ostream& operator<<(ostream& out, hexboard h);
   void printhex();
   void game_loop();
   void player_turn();
   void reset_colors();
   void ai_turn();
   node* calc_move();
   static void *monte_carlo_helper(void *context){
      return ((hexboard *)context)->monte_carlo();
   };
   void *monte_carlo();
   void *monte_carlo_nonthread();
};


#endif

