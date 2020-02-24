/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaMonitoringKernel_MonitoredCollection_h
#define AthenaMonitoringKernel_MonitoredCollection_h

#include <functional>
#include <type_traits>
#include <vector>

#include "AthenaMonitoringKernel/IMonitoredVariable.h"

namespace Monitored {

  // Forward declares
  template <class T> class ValuesCollection;
  template <class T, class R> class ObjectsCollection;

  /**
   * Declare a monitored (double-convertable) collection
   *
   * Any iterable container with elements covertable to double can be monitored.
   *
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   *
   * \code
   *   std::vector<float> eta( {0.2, 0.1} );
   *   auto m = Monitored::Collection("Eta", eta);
   * \endcode
   */
  template <class T> ValuesCollection<T> Collection(std::string name, const T& collection) {
    return ValuesCollection<T>(std::move(name), collection);
  }

  /**
   * Disallow temporaries
   */
  template <class T> ValuesCollection<T> Collection(std::string name, T const&& collection) = delete;

  /**
   * Declare a monitored collection of objects
   *
   * A converter function/accessor needs to be provided to extract the relevant quantity.
   *
   * @param name        Name of monitored quantity
   * @param collection  Collection to be monitored (e.g. STL container or array)
   * @param converterToDouble  Function taking an element and returning a double
   *
   * \code
   *   std::vector<Track> tracks;
   *   auto phi = Monitored::Collection("Phi", tracks, [](const Track& t) {return t.phi();});
   * \endcode
   */
  template <class T>
  ObjectsCollection<T, double>
  Collection(std::string name, const T& collection,
	     std::function<double(const typename ObjectsCollection<T, double>::const_value_type&)> converterToDouble) {
    return ObjectsCollection<T, double>(std::move(name), collection, std::move(converterToDouble));
  }

  template <class T>
  ObjectsCollection<T, double>
  Collection(std::string name, const T&& collection,
             std::function<double(const typename ObjectsCollection<T, double>::const_value_type&)> converterToDouble) = delete;


  template <class T>
  ObjectsCollection<T, std::string>
  Collection(std::string name, const T& collection,
	     std::function<std::string(const typename ObjectsCollection<T, std::string>::const_value_type&)> converterToDouble) {
    return ObjectsCollection<T, std::string>(std::move(name), collection, std::move(converterToDouble));
  }

  template <class T>
  ObjectsCollection<T, std::string>
  Collection(std::string name, const T&& collection,
             std::function<std::string(const typename ObjectsCollection<T, std::string>::const_value_type&)> converterToDouble) = delete;


  namespace detail {
    /// Get element type for containers
    template <typename T> struct get_value_type { typedef typename T::value_type value_type; };
    /// Get element type for arrays
    template <typename T, size_t N> struct get_value_type<T[N]> { typedef T value_type; };

    template <typename T> struct make_pointer_const { typedef T type; };
    template <typename T> struct make_pointer_const<T*> { typedef const T* type; };
  } // namespace detail

  /**
   * Monitoring of collections
   *
   * This class is not supposed to be used by the end user.
   */
  template <class T> class ValuesCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;

    static_assert(std::is_convertible<value_type, double>::value or std::is_constructible<std::string, value_type>::value, "Conversion of collection values to double or string is impossible");

    /// @brief .     \if empty doc string required due to doxygen bug 787131 \endif
    friend ValuesCollection<T> Collection<T>(std::string name, const T& collection);
    friend ValuesCollection<T> Collection<T>(std::string name, const T&& collection);

    ValuesCollection(ValuesCollection&&) = default;

    std::vector<double> getVectorRepresentation() const override {
      return convertToDouble( m_collection );
    }

    virtual std::vector<std::string> getStringVectorRepresentation() const override {
      return convertToString( m_collection );
    }
    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, value_type>::value;
    }
    virtual size_t size() const override {
      return std::size(m_collection);
    }

  private:
    const T& m_collection;


    ValuesCollection(std::string vname, const T& collection)
      : IMonitoredVariable(std::move(vname)), m_collection{collection} {
    }

    ValuesCollection(ValuesCollection const&) = delete;
    ValuesCollection& operator=(ValuesCollection const&) = delete;

    template< typename U, typename = typename std::enable_if< std::is_constructible<std::string, typename detail::get_value_type<U>::value_type>::value >::type >
    std::vector<std::string> convertToString( const U& collection ) const {
      return std::vector<std::string>(std::begin(collection), std::end(collection));
    }

    template< typename U, typename = typename std::enable_if< ! std::is_constructible<std::string, typename detail::get_value_type<U>::value_type>::value >::type, typename = void >
    std::vector<std::string> convertToString( const U& ) const {
      return {};
    }

    template< typename U, std::enable_if_t< !std::is_convertible<typename detail::get_value_type<U>::value_type, double>::value, int> = 0>
    std::vector<double> convertToDouble( const U&  ) const {
      return {};
    }

    template< typename U, std::enable_if_t< std::is_convertible<typename detail::get_value_type<U>::value_type, double>::value, int> = 0>
    std::vector<double> convertToDouble(const U& collection ) const {
      return std::vector<double>(std::begin(collection), std::end(collection));
    }
  };

  /**
   * Monitoring of object collections
   *
   * Template types are: T - collection, R - result type of the converter function (either convertible to double or to string)
   * This class is not supposed to be used by the end user.
   */
  template <class T, class R> class ObjectsCollection : public IMonitoredVariable {
  public:
    /// Type of the collection elements
    using value_type = typename detail::get_value_type<T>::value_type;
    using const_value_type = typename detail::make_pointer_const<value_type>::type;

    static_assert(std::is_convertible<R, double>::value or std::is_constructible<std::string, R>::value, "Conversion from type returned by the converter/accessor to double or string is impossible");

    /// @brief .     \if empty doc string required due to doxygen bug 787131 \endif
    // With a non-template friend declaration, clang 4.0.1
    // fails to match the friend.
    template <class U> friend ObjectsCollection<U, double>
    Collection(std::string name, const U& collection,
               std::function<double(const typename ObjectsCollection<U, double>::const_value_type&)> converterToDouble);

    template <class U> friend ObjectsCollection<U, std::string>
    Collection(std::string name, const U& collection,
               std::function<std::string(const typename ObjectsCollection<U, std::string>::const_value_type&)> converterToString);

    ObjectsCollection(ObjectsCollection&&) = default;

    std::vector<double> getVectorRepresentation() const override {
      return convertToDouble<R>();
    }

    virtual std::vector<std::string> getStringVectorRepresentation() const override {
      return convertToString<R>();
    };
    virtual bool hasStringRepresentation() const override {
      return std::is_constructible<std::string, R>::value;
    };
    virtual size_t size() const override {
      return std::size(m_collection);
    }

  private:
    const T& m_collection;
    std::function<R(const const_value_type&)> m_converterToR;

    ObjectsCollection(std::string name, const T& collection,
                      std::function<R(const const_value_type&)> converterToR)
        : IMonitoredVariable(std::move(name)),
	  m_collection(std::move(collection)),
          m_converterToR(std::move(converterToR)) {}

    ObjectsCollection(ObjectsCollection const&) = delete;

    ObjectsCollection& operator=(ObjectsCollection const&) = delete;

    template< typename U, std::enable_if_t< std::is_convertible<U, double>::value, int> = 0>
    inline std::vector<double> convertToDouble() const {
      // Reserve space and fill vector
      std::vector<double> result;
      result.reserve(std::size(m_collection));      
      for (const auto& value : m_collection) result.push_back(m_converterToR(value));
      return result;
    }

    template< typename U, std::enable_if_t< !std::is_convertible<U, double>::value, int> = 0>
    inline std::vector<double> convertToDouble() const {
      return {};
    }

    template< typename U, std::enable_if_t< std::is_constructible<U, std::string>::value, int> = 0>
    inline std::vector<std::string> convertToString() const {
      // Reserve space and fill vector
      std::vector<std::string> result;
      result.reserve(std::size(m_collection));
      for (const auto& value : m_collection) result.push_back(m_converterToR(value));
      return result;
    }

    template< typename U, std::enable_if_t< !std::is_constructible<U, std::string>::value, int> = 0>
    inline std::vector<std::string> convertToString() const {
      return {};
    }



  };


} // namespace Monitored

#endif /* AthenaMonitoringKernel_MonitoredCollection_h */
