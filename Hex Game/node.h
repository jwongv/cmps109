//Jacky Wong
//Michael Ungerer
//Hang Gao


#ifndef __NODE_H__
#define __NODE_H__


#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>

using namespace std;


class node{
private:
   int row;      //row coordinate
   int col;      //column coordinate
   int color;    //color for path finding
   int owner;    //owner of node
public:
   node();                 //default constructor
   node(int r,int c);      //constructor with coordinates
   int get_row();
   int get_col();
   int get_color();
   int get_owner();
   void change_owner(int player);
   void change_color(int c);
};


#endif
