/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file  KLGaussianMixtureReduction.h
* @author amorley
* @author Anastopoulos
* @date 26th November 2019
* @brief Function that help implement component merging .
*/
#ifndef KLGaussianMixReductionUtils_H
#define KLGaussianMixReductionUtils_H

#include "CxxUtils/restrict.h"
#include "CxxUtils/features.h"
#include <vector>
#include <utility>
#include <cstdint>

namespace GSFUtils {

/**
 * Alignment used  for SIMD
 */
constexpr size_t alignment =32;

/**
 * The main pupropse of the utilities here
 * are to facilitate the calculation of the 
 * divergence between components of the mixture 
 * and the merging of similar componets.
 *
 * For a revies of available methods look 
 * https://arxiv.org/pdf/2001.00727.pdf
 *
 * Here we opt for formula 10. 
 * For an 1D Normal distributions this becomes: 
 *
 * (variance1-variance2) (1/variance1 - 1/variance2) - 
 * (mean1-mean2)(1/variance+1/variance)(mean1-mean2)
 *
 * We use doubles for the intermediate calculations
 * but we store the final distances to short in an array
 * of floats.
 *
*/
struct Component1D{
  double mean=0.;
  double cov=0.;
  double invCov=1e10; 
  double weight=0.;
};

/**
 *   For pairwise distance comparisons assuming 0..... N-1 (N total elements )
 *   The pairwise distance matrix  can be represented in a triangular array:
 *   [ (1,0)
 *   [ (2,0), (2,1),
 *   [ (3,0), (3,1), (3,2)
 *   [ (4,0), (4,1), (4,2) , (4,3)
 *   [.......................
 *   [.............................
 *   [(N-1,0),(N-1,1),(N-1,2),(N-1,3).......(N-1,N-2)]
 *
 *   With size 
 *   1+2+3+ .... (N-1) = N*(N-1)/2
 *
 *   The lexicographical storage allocation function is
 *   Loc(i,j) = i*(i-1)/2 + j 
 *   e.g
 *   (1,0) --> 1 *(1-1)/2 + 0 --> 0 
 *   (2,0) --> 2 *(2-1)/2 + 0 --> 1
 *   (2,1) --> 2 *(2-1)/2 + 1 --> 2
 *   (3,0) --> 3 * (3-1)/2 +0 --> 3
 *   Leading to  
 *   [(1,0),(2,0),(2,1),(3,0),(3,1),(3,2)...... (N-1,N-2)]
 * 
 *
 *   The N-1 Rows  map to value K of the 1st element in the pair.
 *   K=1,2,3,...N-1  and each one has size K. 
 *   Each Row starts at array positions K*(K-1)/2
 *   e.g 
 *   The row for element 1 starts at array position 0.
 *   The row for element 2 starts at array position 1.
 *   The row for element N-1  starts at array positon (N-1)*(N-2)/2
 *   
 *   The N-1 Columns map to the value K of the second  element in the pair 
 *   0,1,2 ... N-2
 *   The array position follow (i-1)*i/2+K;
 *   where i : K+1 .... N-1 [for(i=K+1;i<N;++i)
 *   e.g 
 *   0 appears as 2nd element in the pair at array positions 
 *   [0,1,3,6...]
 *   1 appears as 2nd element in the pair at array positions
 *   [2,4,7...] 
 *   2 appears as 2nd element in the pair at array positions
 *   [5,8,12....] 
 *   N-2 appears as 2nd element once at position [N(N-1)/2-1]
 *
 */
struct triangularToIJ{
  int32_t I=-1;
  int32_t J=-1;
};

/**
 * Some usefull typedefs
 */
typedef float* ATH_RESTRICT floatPtrRestrict;
typedef Component1D* ATH_RESTRICT componentPtrRestrict;

/**
 * Merge the componentsIn and return 
 * which componets got merged
 */
std::vector<std::pair<int32_t, int32_t>>
findMerges(componentPtrRestrict componentsIn, 
           const int32_t inputSize, 
           const int32_t reducedSize);

/**
 * For finding the index of the minumum pairwise distance
 * we opt for SIMD and function multiversioning
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2")))
std::pair<int32_t,float> 
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
__attribute__((target("sse4.1"))) 
std::pair<int32_t,float> 
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
__attribute__((target("sse2"))) 
std::pair<int32_t,float> 
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
#endif //x86_64 specific targets
__attribute__((target("default")))
#endif// function multiversioning
std::pair<int32_t,float> 
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);

} // namespace KLGaussianMixtureReduction

      


#endif
