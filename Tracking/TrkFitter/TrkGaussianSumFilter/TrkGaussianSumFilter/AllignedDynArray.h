/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
  AllignedDynArray.h  -  description
  -------------------------------------------------
 begin                : 26th November 2019
 author               : amorley, Christos
 decription           : Dynamic array fullfilling alignment requirements
 *********************************************************************************/

#ifndef GSFUtils_AllignedDynArray_H
#define GSFUtils_AllignedDynArray_H

#include <stdlib.h>
namespace GSFUtils{

template<typename T,int Alignment>
/*
 * Dynamically allocate un-initialized storage where the storage is 
 * aligned according to Alignment i.e the storage starts at an adrress 
 * that should be divisible with Alignment 
 *
 * This is usefull for arrays of simple types like 
 *
 *  
 * Notes :
 * For fixed size buffers i.e when the size is known at runtime prefer : 
 * std::aligned_storage/std::align
 *
 *  In principle in C++17 the call to posix_memalign and the placement new can be avoided 
 *  in favour of the "new operator" alignment aware overloads. Needs some checking on library support.
 */

class AlignedDynArray
{
public:
  AlignedDynArray() = delete;
  AlignedDynArray(AlignedDynArray const&) = delete;
  AlignedDynArray& operator=(AlignedDynArray const&) = delete;
  AlignedDynArray(AlignedDynArray&&) = delete;
  AlignedDynArray& operator=(AlignedDynArray&&) = delete;

  explicit AlignedDynArray(size_t n);
  ~AlignedDynArray();

  ///conversions to T*
  operator T*(); 
  operator const T*() const; 
 
  /// index array operators  
  T& operator [](const std::size_t pos);
  const T& operator [](const std::size_t pos) const ;
 
  ///size of allocated buffer
  std::size_t size() const;

private:
  T*  m_buffer=nullptr;
  void* m_raw=nullptr;
  size_t const m_size=0;
}; 


template<typename T, int Alignment>
inline
AlignedDynArray<T,Alignment>::AlignedDynArray(size_t n): m_buffer(nullptr), 
                                            m_raw(nullptr),
                                            m_size(n){
  size_t const size = n * sizeof(T) +alignof(T) ;
  //create buffer of right size,properly aligned
  posix_memalign(&m_raw, Alignment, size);
  //placement new of elements to the buffer
  m_buffer = new (m_raw) T[n];
}

template<typename T, int Alignment>
inline  
AlignedDynArray<T,Alignment>::~AlignedDynArray(){
  for (std::size_t pos = 0; pos < m_size; ++pos) {
    m_buffer[pos].~T();
  }
  free(m_buffer);
}

template<typename T, int Alignment>
inline   
AlignedDynArray<T,Alignment>::operator T*() {return m_buffer;} 

template<typename T, int Alignment>
inline   
AlignedDynArray<T,Alignment>::operator const T*() const {return m_buffer;}

template<typename T, int Alignment>
inline  
T& AlignedDynArray<T,Alignment>::operator[] (const std::size_t pos) { return m_buffer[pos];}

template<typename T, int Alignment>
inline  
const T& AlignedDynArray<T,Alignment>::operator[] (const std::size_t pos) const { return m_buffer[pos];}

template<typename T, int Alignment>
inline  
std::size_t AlignedDynArray<T,Alignment>::size() const {return m_size;}

}//namespace GSFUtils

#endif
