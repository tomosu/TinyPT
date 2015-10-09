#pragma once
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>



inline bool SavePixelDoubleToInt(const std::string &filepath,
			  const std::vector<double> &pixel,
			  double gain){

  FILE* fprjb;
  int *pixelInt = (int*)malloc( sizeof(int) *pixel.size() );

  for(long long int i=0; i <pixel.size(); i++){
    if((int)(pixel[i] *gain) < INT_MAX){
      pixelInt[i] = (short)(pixel[i] * gain);
    }else{
      pixelInt[i] = INT_MAX;
    }

  }
  
  if((fprjb = fopen(filepath.c_str(), "w"))==NULL){
    printf("file open error\n");
    return false;
  }

  fwrite(pixelInt, sizeof(int), pixel.size(), fprjb);
  free(pixelInt);
  fclose(fprjb);
  return true;
};
