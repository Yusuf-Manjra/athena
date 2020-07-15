/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**

 * @author Anthony Morley, Christos Anastopoulos
 * @brief  Dynamic array fullfilling alignment requirements
 */

#ifndef GSFUtils_AlignedDynArray_H
#define GSFUtils_AlignedDynArray_H
#include <cstdlib>
#include <memory>
namespace GSFUtils {
/*
 * GCC and Clang provide) the the attribute
 * if we have a std implementing ideas from
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0886r0.pdf
 * prb we do not need this
 * Can be used to express that we return aligned ouputs
 */
#if defined(__GNUC__) && !defined(__CLING__) && !defined(__ICC)
#define GSF_ALIGN_RETURN(X) __attribute__((assume_aligned(X)))
#else
#define GSF_ALIGN_RETURN(X)
#endif

/**
 * A wrapper around std::aligned_alloc
 *
 * The main usage is to create an alligned buffer
 * array to be used with vector instructions
 *
 * Provides
 * - Additional RAII functionality
 * - Default initialization of elements
 * - Value initialization of elements.
 */
template<typename T, size_t ALIGNMENT>
struct AlignedDynArray
{

  ///@{
  // Standard typedefs
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  /// @}
  /// Deleted default constructor
  AlignedDynArray() = delete;
  /// Deleted default copy constructor
  AlignedDynArray(AlignedDynArray const&) = delete;
  /// Deleted default assignment operator
  AlignedDynArray& operator=(AlignedDynArray const&) = delete;

  /// Constructor default initializing elements
  explicit AlignedDynArray(size_type n);
  /// Constructor initializing elements to value
  explicit AlignedDynArray(size_type n, const T& value);

  /// Move copy constructor
  AlignedDynArray(AlignedDynArray&&) noexcept;
  /// Move assignment operator
  AlignedDynArray& operator=(AlignedDynArray&&) noexcept;
  /// Destructor
  ~AlignedDynArray();

  /// Get the underlying buffer
  pointer buffer() noexcept GSF_ALIGN_RETURN(ALIGNMENT);
  /// Get the underlying buffer
  const_pointer buffer() const noexcept GSF_ALIGN_RETURN(ALIGNMENT);

  /// index array operators
  reference operator[](size_type pos) noexcept;
  const_reference operator[](size_type pos) const noexcept;

  /// iterator pointing to the first element
  iterator begin() noexcept GSF_ALIGN_RETURN(ALIGNMENT);

  /// const iterator pointing to the first element
  const_iterator begin() const noexcept GSF_ALIGN_RETURN(ALIGNMENT);

  /// iterator pointing to the past-the-end  element
  iterator end() noexcept;
 
  /// const iterator pointing to the past-the-end  element
  const_iterator end() const noexcept;

  ///  number of elements/size
  size_type size() const noexcept;

  /// returns true is size == 0
  bool empty() const noexcept;


private:
  void cleanup();
  pointer m_buffer = nullptr;
  size_type m_size = 0;
};

} // namespace GSFUtils

// impl .icc
#include "TrkGaussianSumFilter/AlignedDynArray.icc"

#endif
