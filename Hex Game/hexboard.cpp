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
#include <chrono>
#include "node.h"
#include "hexboard.h"
using namespace chrono;
using namespace std;

inline int rand_hex(){ return rand() % 11;}  //inline function to return a random hex coordinate

//hexboard class definition
hexboard::hexboard(int s){
   size = s;
   winchance = -1;
   matrix.resize(size);               //resize vector containing node vector
   for(int i = 0; i < size; ++i)      //resize node vectors
      matrix[i].resize(size);

   for(int i = 0; i < size; ++i){
      for(int j = 0; j < size; ++j)   //fill in node vectors with nodes containing coordinates
         matrix[i][j] = node(i,j);
   }
}

//function to fill the hexboard randomly
void hexboard::fill_board(){
   int row = rand_hex();
   int col = rand_hex();
   for(int i = 0; i < 61; ++i){
      while(matrix[row][col].get_owner() != 0){    //get random coordinates until an empty one is found
         row = rand_hex();
         col = rand_hex();
      }
      matrix[row][col].change_owner(1);            //change owner to player 1
      if(i == 60) continue;
      while(matrix[row][col].get_owner() != 0){    //get random coordinates until an empty one is found
         row = rand_hex();
         col = rand_hex();
      }
      matrix[row][col].change_owner(2);            //change owner to player 2
   }
}

//function to fill in a turn
void hexboard::fill_turn(int x, int y, int player){
   matrix[y][x].change_owner(player);
}

//function for the player's turn. asks for coordinates for input
void hexboard::player_turn(){
   int col, row;
   cout << "Please enter a coordinate with values of 0-10, separated with a space" << endl << "Anything other than numbers will cause the program to break." << endl << "Entering more than two numbers will apply to your next turn" << endl;
   cin >> col >> row;
   if (col < 0 || col > 10 || row < 0 || row > 10){
      cout << "Invalid input!" << endl;
      return player_turn();                               //check if valid input, if not restart loop
   }
   if (!check_empty(col,row)){
      cout << "That hex is taken" << endl;
      return player_turn();                               //check if valid input, if not restart loop
   }
   cout << "col = " << col << " row = " << row << endl;
   fill_turn(col, row, 1);

}

//function to perform the AI's turn
void hexboard::ai_turn(){
   int randomi = rand()%size;
   int randomj = rand()%size;
   while(!check_empty(randomj, randomi)){
      randomi = rand()%size;
      randomj = rand()%size;
   }
   cout << "Calculating next move. Please don't type anything!" << endl << endl;
   node* nextMove = calc_move();
   if(nextMove == NULL){
      matrix[randomi][randomj].change_owner(2);
      cout << "The computer has chosen the hex at coordinate " << randomi << "," << randomj <<endl << endl;
   }else{
      nextMove->change_owner(2);
      cout << "The computer has chosen the hex at coordinate " << nextMove->get_col() << "," << nextMove->get_row() << endl << endl;
   }
}

//function to calculate the AI's next move. Calls monte carlo method and finds the hex with highest win chance. 
//Performs both threaded and nonthreaded calls and compares the time it takes for each. Uses two threads
node* hexboard::calc_move(){
   pthread_t threads[2];                                               //variables used for threading and time recording
   pthread_attr_t attr;
   void *status;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   int rc;
   high_resolution_clock::time_point before, after;
   milliseconds elapsed_thread (0);
   milliseconds elapsed_nonthread (0);

   float best = -1;
   int bestindex_i = -1;
   int bestindex_j = -1;
   for(int i = 0; i < size; i++){
      for(int j = 0; j < size; j++){
         if(matrix[i][j].get_owner() == 0){
            matrix[i][j].change_owner(2);

            before = high_resolution_clock::now();

            for(int i = 0; i < 2; i++){
              rc = pthread_create(&threads[i], &attr, hexboard::monte_carlo_helper, this);        //create threads
            }
            if( rc) cout << "PROBREMMM" << endl;
            pthread_attr_destroy(&attr);
            for(int i = 0; i < 2; i++){                                                           //joins threads
               rc = pthread_join(threads[i], &status); 
            }

            after = high_resolution_clock::now();
            elapsed_thread += duration_cast<milliseconds>(after - before);                        //calculates time of threaded monte carlo

            before = high_resolution_clock::now();
            monte_carlo_nonthread();
            after = high_resolution_clock::now();
            elapsed_nonthread += duration_cast<milliseconds>(after - before);                     //calculates time of nonthreaded monte carlo

            if(rc) cout << "ANOTHER PROBREMM" << endl;
            matrix[i][j].change_owner(0);
            if (winchance > best){
               best = winchance;
               bestindex_i = i;
               bestindex_j = j;
            }
            winchance = -1;
         }
      }
   }
   cout << "Thread Monte Carlo: " << elapsed_thread.count() << " milliseconds" << endl;
   cout << "Nonthread Monte Carlo: " << elapsed_nonthread.count() << " milliseconds" << endl;

   if(bestindex_i != -1) return &matrix[bestindex_i][bestindex_j];
   else return NULL;
}

//threaded monte carlo function. fills in half the remaining empty hexes and checks if it wins.
void *hexboard::monte_carlo(){
   vector<node*> emptyhexes;
   emptyhexes.clear();
   hexboard hexboardcopy = *this;
   int wins = 0;
   int games = 0;
   for(int i = 0; i < size; i++){
      for(int j = 0; j < size; j++){
         if(matrix[i][j].get_owner() == 0)
            emptyhexes.push_back(&hexboardcopy.matrix[i][j]);
      }
   }

   for(int i = 0; i < 500; i++){
      random_shuffle(emptyhexes.begin(),emptyhexes.end());
      for(int j = 0; j < emptyhexes.size()/2; j++){
         emptyhexes[j]->change_owner(2);
      }
      int result = hexboardcopy.find_ai_winner();
      for(int k = 0; k < emptyhexes.size()/2; k++){
         emptyhexes[k]->change_owner(0);
      }
      if (result == 2){
         ++wins;
      }
      ++games;
   }
   if (((float)(wins))/((float)(games)) >= winchance){
      winchance = ((float)(wins))/((float)(games));
   }
   pthread_exit(NULL);
}

//nonthreaded monte carlo
void *hexboard::monte_carlo_nonthread(){
   vector<node*> emptyhexes;
   emptyhexes.clear();
   hexboard hexboardcopy = *this;
   int wins = 0;
   int games = 0;
   for(int i = 0; i < size; i++){
      for(int j = 0; j < size; j++){
         if(matrix[i][j].get_owner() == 0)
            emptyhexes.push_back(&hexboardcopy.matrix[i][j]);
      }
   }

   for(int i = 0; i < 1000; i++){
      random_shuffle(emptyhexes.begin(),emptyhexes.end());
      for(int j = 0; j < emptyhexes.size()/2; j++){
         emptyhexes[j]->change_owner(2);
      }
      int result = hexboardcopy.find_ai_winner();
      for(int k = 0; k < emptyhexes.size()/2; k++){
         emptyhexes[k]->change_owner(0);
      }
      if (result == 2){
         ++wins;
      }
      ++games;
   }
   if (((float)(wins))/((float)(games)) >= winchance){
      winchance = ((float)(wins))/((float)(games));
   }
}

//checks if a hex is empty
bool hexboard::check_empty(int x, int y){
   if(matrix[y][x].get_owner() == 0)
      return true;
   else return false;
}

//resets the colors of all hexes used for path finding
void hexboard::reset_colors(){
   for(int i =0; i < size; i++){
      for(int j = 0; j < size; j++)
         matrix[i][j].change_color(0);
   }
}

//BFS search used to check if the player won
int hexboard::find_player_winner(){
   int row = 0;
   list <node*> queue;
   queue.clear();
   for(int col = 0; col < size; ++col){       //begin at 0,0 coordinate
      if(matrix[row][col].get_owner() == 1){
         if(matrix[row][col].get_color() == 1 || matrix[row][col].get_color() == 2) continue;
         queue.push_back(&matrix[row][col]);
         matrix[row][col].change_color(1);
         while(queue.size() != 0){           //start BFS loop. Outer if statements check for edge cases. If node is owned by player 1 and unvisited, add to queue.
            node* curr = queue.front();
            if(curr->get_row() != 0){
               if(matrix[curr->get_row()-1][curr->get_col()].get_owner() == 1 && matrix[curr->get_row()-1][curr->get_col()].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()-1][curr->get_col()]);
                  matrix[curr->get_row()-1][curr->get_col()].change_color(1);
               }
            }
            if(curr->get_row() != 0 && curr->get_col() != size-1){
               if(matrix[curr->get_row()-1][curr->get_col()+1].get_owner() == 1 && matrix[curr->get_row()-1][curr->get_col()+1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()-1][curr->get_col()+1]);
                  matrix[curr->get_row()-1][curr->get_col()+1].change_color(1);
               }
            }
            if(curr->get_col() != 0){
               if(matrix[curr->get_row()][curr->get_col()-1].get_owner() == 1 && matrix[curr->get_row()][curr->get_col()-1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()][curr->get_col()-1]);
                  matrix[curr->get_row()][curr->get_col()-1].change_color(1);
               }
            }
            if(curr->get_col() != size-1){
               if(matrix[curr->get_row()][curr->get_col()+1].get_owner() == 1 && matrix[curr->get_row()][curr->get_col()+1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()][curr->get_col()+1]);
                  matrix[curr->get_row()][curr->get_col()+1].change_color(1);
               }
            }
            if(curr->get_row() != size-1){
               if(matrix[curr->get_row()+1][curr->get_col()].get_owner() == 1 && matrix[curr->get_row()+1][curr->get_col()].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()+1][curr->get_col()]);
                  matrix[curr->get_row()+1][curr->get_col()].change_color(1);
               }
            }
            if(curr->get_row() != size-1 && curr->get_col() != 0){
               if(matrix[curr->get_row()+1][curr->get_col()-1].get_owner() == 1 && matrix[curr->get_row()+1][curr->get_col()-1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()+1][curr->get_col()-1]);
                  matrix[curr->get_row()+1][curr->get_col()-1].change_color(1);
               }
            }

         queue.pop_front();         //pop off queue and change node to visited
         curr->change_color(2);

         }
      }
   }
   for(int i = 0; i < size; ++i){                //check opposite side of board for visited node. If found, player 1 won. Else player 2 won
      if(matrix[size-1][i].get_color() == 2){
         reset_colors();
         return 1;
      }
   }
   reset_colors();
   return 0;

}
//BFS search used to check if the AI won
int hexboard::find_ai_winner(){
   int col = 0;
   list <node*> queue;
   queue.clear();
   for(int row = 0; row < size; ++row){       //begin at 0,0 coordinate
      if(matrix[row][col].get_owner() == 2){
         if(matrix[row][col].get_color() == 1 || matrix[row][col].get_color() == 2) continue;
         queue.push_back(&matrix[row][col]);
         matrix[row][col].change_color(1);
         while(queue.size() != 0){           //start BFS loop. Outer if statements check for edge cases. If node is owned by player 1 and unvisited, add to queue.
            node* curr = queue.front();
            if(curr->get_row() != 0){
               if(matrix[curr->get_row()-1][curr->get_col()].get_owner() == 2 && matrix[curr->get_row()-1][curr->get_col()].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()-1][curr->get_col()]);
                  matrix[curr->get_row()-1][curr->get_col()].change_color(1);
               }
            }
            if(curr->get_row() != 0 && curr->get_col() != size-1){
               if(matrix[curr->get_row()-1][curr->get_col()+1].get_owner() == 2 && matrix[curr->get_row()-1][curr->get_col()+1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()-1][curr->get_col()+1]);
                  matrix[curr->get_row()-1][curr->get_col()+1].change_color(1);
               }
            }
            if(curr->get_col() != 0){
               if(matrix[curr->get_row()][curr->get_col()-1].get_owner() == 2 && matrix[curr->get_row()][curr->get_col()-1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()][curr->get_col()-1]);
                  matrix[curr->get_row()][curr->get_col()-1].change_color(1);
               }
            }
            if(curr->get_col() != size-1){
               if(matrix[curr->get_row()][curr->get_col()+1].get_owner() == 2 && matrix[curr->get_row()][curr->get_col()+1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()][curr->get_col()+1]);
                  matrix[curr->get_row()][curr->get_col()+1].change_color(1);
               }
            }
            if(curr->get_row() != size-1){
               if(matrix[curr->get_row()+1][curr->get_col()].get_owner() == 2 && matrix[curr->get_row()+1][curr->get_col()].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()+1][curr->get_col()]);
                  matrix[curr->get_row()+1][curr->get_col()].change_color(1);
               }
            }
            if(curr->get_row() != size-1 && curr->get_col() != 0){
               if(matrix[curr->get_row()+1][curr->get_col()-1].get_owner() == 2 && matrix[curr->get_row()+1][curr->get_col()-1].get_color() == 0){
                  queue.push_back(&matrix[curr->get_row()+1][curr->get_col()-1]);
                  matrix[curr->get_row()+1][curr->get_col()-1].change_color(1);
               }
            }

         queue.pop_front();         //pop off queue and change node to visited
         curr->change_color(2);

         }
      }
   }
   for(int i = 0; i < size; ++i){                //check opposite side of board for visited node. If found, player 1 won. Else player 2 won
      if(matrix[i][size-1].get_color() == 2){
         reset_colors();
         return 2;
      }
   }
   reset_colors();
   return 0;

}

//overloading of << operator to print out the hexboard
ostream& operator<<(ostream& out, hexboard h){
   cout << "      P  L  A  Y  E  R  1  =  #" << endl;   //player 1 header
   cout << "  ";
   for(int i = 0; i < h.size; ++i){
      cout << "/ \\ ";
   }
   cout << endl;
   int q = 0;
   for(int i = 0; i < h.size; ++i){
      ++q;
      for(int z = 0 ; z < q; ++z){
         cout<<" ";
      }

      for(int j = 0; j < h.size; ++j){
         if(h.matrix[i][j].get_owner() == 1)
            cout<<"| # ";
         else if(h.matrix[i][j].get_owner() == 2)
            cout<<"| . ";
         else cout << "|   ";
      }
      cout << "|";
      ++q;
      cout << endl;
      for(int z = 0; z < q; ++z)
         cout<<" ";
      for(int j = 0; j < h.size; ++j){
         if(i + 1 == h.size)
            cout << "\\ / ";
         else if(j + 1 == h.size)
            cout << "\\ / \\";
         else cout << "\\ / ";
      }
      if(i == 0) cout << "     P";         //player 2 header
      if(i == 1) cout << "     L";
      if(i == 2) cout << "     A";
      if(i == 3) cout << "     Y";
      if(i == 4) cout << "     E";
      if(i == 5) cout << "     R";
      if(i == 6) cout << "     2";
      if(i == 7) cout << "     =";
      if(i == 8) cout << "     .";
      cout << endl;
   }
   cout << endl;
   return out;
}

//the game loop for our hex game
void hexboard::game_loop(){
   int winner = 0;
   while(winner == 0){
      player_turn();
      cout << *this;
      winner = find_player_winner();
      if(winner == 1) break;
      ai_turn();
      winner = find_ai_winner();
      cout << *this;
      if(winner == 2) break;
   }
   if (winner == 1)
      cout << "Player 1 wins!" << endl << endl;
   if (winner == 2)
      cout << "Player 2 wins!" << endl << endl;
}

















