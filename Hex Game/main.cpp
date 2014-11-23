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
#include "hexboard.h"
using namespace std;

  
int main(){
   srand(time(0));                 //seed random generator
   hexboard test1 = hexboard(11);  //create a 11x11 hexboard
   test1.game_loop();
   return 0;
}


 

