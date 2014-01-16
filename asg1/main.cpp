//Convert this program to C++
//Change to C++ io
//Change to one line comments
//Change defines of constants to const
//Change array to vector<>
//Inline any short function

//Convert to a C++ version of monte carlo integration for pi
//Copyright Ira Pohl

//Jacky Wong
//ID: 1281439

#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
using namespace std;

const int MAXINT = 32767;
inline double f(double x){return sqrt(1-x*x);}
inline double r_num(){ return ((rand() % MAXINT)/(double)MAXINT);}

double mcint(double a, double b, double max, int n){
   int below = 0, trials = 0;
   double x, y;
   for(trials = 0; trials < n; ++trials){
      x = r_num()*(b - a) + a;
      y = r_num();
      if(f(x) > y) ++ below;
   }
   return ((double)below/trials * (b - a) * max);
}

int main(){
   double pi, x, y;
   int below, trials, mtrial, i, n_experiment = 5;
   vector<double> pi_approx(n_experiment);
   cout << "TRIALS = \n";
   cin >> mtrial;
   cout << setprecision(5) << fixed; //set decimal precision to 5

   for(i = 0; i < n_experiment; ++i){
      pi_approx[i] = 4 * mcint(0.0, 1.0, 1.0, mtrial);
      mtrial = 10 * mtrial;
   }
   
   for(i = 0; i < n_experiment; ++i)
      cout << "pi trial " << i << " = " << pi_approx[i] << ", \n";

   return 0;

}
