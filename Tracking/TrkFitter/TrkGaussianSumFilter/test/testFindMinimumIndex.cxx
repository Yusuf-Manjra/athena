/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include <iostream>

int main(){

  alignas(32) float array[64] = {
    2.21929, 8.51508, 9.71981, 2.98931, 3.7735,  5.92499, 2.69544, 9.93593, 9.96815, 9.70925, 7.53255, 9.82999, 1.98876,
    8.18295, 3.67326, 1.04305, 2.01218, 6.75787, 8.90588, 5.53296, 8.18136, 4.25165, 2.90732, 7.13224, 4.58865, 7.66583,
    5.27283, 4.79879, 2.56479, 3.71722, 8.17552, 3.84895, 8.85186, 2.34203, 9.94662, 8.39713, 2.12664, 7.87375, 5.4153,
    6.97245, 2.13307, 2.89188, 1.46095, 1.32797, 4.67858, 5.1219,  5.38812, 8.14577, 9.28787, 8.26778, 7.35197, 1.02537,
    7.39633, 6.79565, 5.1043,  7.96525, 6.16379, 8.89082, 8.27358, 1.15997, 8.39121, 8.38757, 9.46067, 4.714};

  std::pair<int32_t,float> minIndex=GSFUtils::findMinimumIndex(array,64);
  std::cout << "FindMinimumIndex Index = " <<  minIndex.first <<  " with value = " <<  minIndex.second <<'\n'; 

  return 0;
}
