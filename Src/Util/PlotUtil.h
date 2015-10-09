#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>

class Plotter
{

public:
  Plotter(const int Xgraduation_in, const int Ygraduation_in)
    : Xgraduation(Xgraduation_in), Ygraduation(Ygraduation_in)
  {
    XLabel ="X";
    YLabel ="Y";
  }

  Plotter(const int Xgraduation_in, const int Ygraduation_in, const std::string& XLabel_in, const std::string& YLabel_in)
    : Xgraduation(Xgraduation_in), Ygraduation(Ygraduation_in) ,XLabel(XLabel_in), YLabel(YLabel_in)
  {
    
  }

  void Plot(const std::vector<double> &X, const std::vector<double> &Y, const std::string& mark)
  {
    double Xmax = *std::max_element(X.begin(), X.end());
    double Xmin = *std::min_element(X.begin(), X.end());
    double Ymax = *std::max_element(Y.begin(), Y.end());
    double Ymin = *std::min_element(Y.begin(), Y.end());

    double Xstep =(Xmax-Xmin) /(double)Xgraduation;
    double Ystep =(Ymax-Ymin) /(double)Ygraduation;
    int size =Xgraduation *Ygraduation;
    std::vector<bool> cell(size, false);
    
    for(int i=0; i<X.size(); i++){
      int xidx=std::max((int)(X[i]/Xstep)-1, 0);
      int yidx=std::max((int)(Y[i]/Ystep)-1, 0);
      cell[xidx +Xgraduation*yidx]=true;
    }

    //Ylabel
    std::cout << std::endl << YLabel << std::endl;

    //draw line
    for(int i=Ygraduation-1; i >= 0; i--){
      if(i==Ygraduation-1 || i==Ygraduation/2 || i==0){
	printf("%.2e #", Ymax-(Ygraduation-1-i)*Ystep);
      }else{
	printf("         #");
      }
      PrintLine(cell, i, mark);
    }
    


    //xaxis
    printf("         ");
    for(int i=0; i<Xgraduation+1; i++){
      printf("#");
    }
    printf("\n");


    //xval
    printf("         ");
    printf("%.2e", Xmin);
    for(int i=0; i<Xgraduation/2-8; i++){ printf(" "); }
    printf("%.2e", (Xmin+Xmax)/2.0);
    for(int i=0; i<Xgraduation/2-8; i++){ printf(" "); }
    printf("%.2e", Xmax);

    //Xlabel
    std::cout << std::endl;
    for(int i=0; i<Xgraduation+8; i++){ printf(" "); }
    std::cout << XLabel << std::endl;
  }
  
  void PlotXlog(){}
  void PlotYlog(){}
  void PlotXYlog(){}

private:
  void PrintLine(const std::vector<bool> &cell, const int line, const std::string &mark)
  {
    for(int x=0; x<Xgraduation; x++){
      if(cell[x+line*Xgraduation]){
	std::cout << mark;
      }else{
	std::cout << " ";
      }
    }
    std::cout << std::endl;
  };

  int Xgraduation;
  int Ygraduation;
  std::string XLabel;
  std::string YLabel;
};
