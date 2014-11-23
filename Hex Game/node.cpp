//Jacky Wong
//Michael Ungerer
//Hang Gao


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>
#include "node.h"
using namespace std;

node::node(){
   row = 0;
   col = 0;
   color = 0;
   owner = 0;
}

node::node(int r, int c){
   row = r;
   col = c;
   color = 0;
   owner = 0;
}

int node::get_row(){
   return row;
}

int node::get_col(){
   return col;
}

int node::get_color(){
   return color;
}

int node::get_owner(){
   return owner;
}

void node::change_owner(int player){
   owner = player;
}

void node::change_color(int c){
   color = c;
}

