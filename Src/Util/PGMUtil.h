#pragma once
#include <stdio.h>
#include <vector>
#include <string>
#include <limits.h>
#include <algorithm>


inline bool SavePixelDoubleToPGM(const std::string &filepath,
				 const std::vector<double> &pixel,
				 const int sizeU,
				 const int sizeV,
				 const double gain)
{
  FILE* fp;
  if((fp = fopen(filepath.c_str(), "w"))==NULL){
    printf("file open error: %s\n", filepath.c_str());
    return false;
  }

  fprintf(fp, "P2\n");
  fprintf(fp, "%d %d\n", sizeU, sizeV);

  double max = *std::max_element(pixel.begin(), pixel.end());
  fprintf(fp, "%d\n", (int)(max *gain));

  for(int i=0; i<pixel.size(); i++){
    fprintf(fp, "%d ", (int)(pixel[i]*gain) );
    if(i%sizeU == sizeU-1){
      fprintf(fp, "\n");
    }
  }

  fclose(fp);
  return true;
};


inline bool SaveCropPixelDoubleToPGM(const std::string &filepath,
				     const std::vector<double> &pixel,
				     const int sizeU,
				     const int sizeV,
				     const double gain,
				     const int max)
{
  FILE* fp;
  if((fp = fopen(filepath.c_str(), "w"))==NULL){
    printf("file open error: %s\n", filepath.c_str());
    return false;
  }

  fprintf(fp, "P2\n");
  fprintf(fp, "%d %d\n", sizeU, sizeV);

  fprintf(fp, "%d\n", max);

  for(int i=0; i<pixel.size(); i++){
    fprintf(fp, "%d ", std::min( (int)(pixel[i]*gain), max) );
    if(i%sizeU == sizeU-1){
      fprintf(fp, "\n");
    }
  }

  fclose(fp);
  return true;
};
