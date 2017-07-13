#pragma once
//default include
#include <cstring>
#include <cmath>
#include <cassert>
#include <string>
#include <functional>
#include <algorithm>
#include <limits>
#include <vector>
#include <queue>
//lib include
#include <Eigen/Eigen>
#include <zlib.h>
//internal include
#include "TicksTime.h"
#include "DennRandom.h"
//defines
#define USE_CROSS_ENTROPY_SAFE
//macro
#define MESSAGE( _msg_ ) std::cout<< _msg_ <<std::endl;
//MSMACRO
#if defined( _MSC_VER )
	#define ASPACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#else 
	#define ASPACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
//alias
namespace Denn
{
	#if defined(USE_LONG_DOUBLE)
	using Scalar	 = long double;
	#elif defined(USE_DOUBLE)
	using Scalar	 = double;
	#else
	using Scalar	 = float;
	#endif

	using RowVectorLD = typename Eigen::Matrix<long double, 1, Eigen::Dynamic>;
	using RowVectorD = typename Eigen::Matrix<double, 1, Eigen::Dynamic>;
	using RowVectorF = typename Eigen::Matrix<float,  1, Eigen::Dynamic>;
	using RowVector  = typename Eigen::Matrix<Scalar,  1, Eigen::Dynamic>;

	using ColVectorLD = typename Eigen::Matrix<long double, Eigen::Dynamic, 1>;
	using ColVectorD  = typename Eigen::Matrix<double, Eigen::Dynamic, 1>;
	using ColVectorF  = typename Eigen::Matrix<float,  Eigen::Dynamic, 1>;
	using ColVector  = typename Eigen::Matrix<Scalar,  Eigen::Dynamic, 1>;

	template < typename T >
	using MatrixT	 = typename Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixLD	 = typename Eigen::Matrix<long double, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixD	 = typename Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixF	 = typename Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
	using Matrix 	 = typename Eigen::Matrix< Scalar, Eigen::Dynamic, Eigen::Dynamic > ;
}
//utilities
namespace Denn
{
	template<typename T, class Compare>
	inline constexpr T clamp(const T& value, const T& lo, const T& hi, Compare comp)
	{
		return assert(!comp(hi, lo)), comp(value, lo) ? lo : comp(hi, value) ? hi : value;
	}

	template< typename T >
	inline constexpr T clamp(const T& value, const T& lo, const T& hi)
	{
		return clamp(value, lo, hi, std::less<void>());
	}	

	template< typename T >
	inline constexpr T sature(const T& v)
	{
		return clamp< T >(v, 0.0, 1.0);
	}

	template < typename T, typename S >
	inline constexpr T lerp(const T& a, const T& b, const S& alpha)
	{
		return a * ( T(1.0) - alpha ) + b * alpha;
	}

	template < typename T >
	inline constexpr T positive_mod(const T& value,const T& base)
	{
		#if 0
		T value_mod = value % base;
		while  (value_mod < T(0)) value_mod += value;
		return value_mod;
		#else 
		//from JS
		return (((value % base) + base) % base);
		#endif
	}

	inline std::string str_replace(std::string str, const std::string& old_str, const std::string& new_str)
	{
		std::string::size_type pos = 0u;
		while ((pos = str.find(old_str, pos)) != std::string::npos)
		{
			str.replace(pos, old_str.length(), new_str);
			pos += new_str.length();
		}
		return str;
	}
}

//Debug
#ifdef  _DEBUG
	#define MESSAGE_DEBUG( _msg_ ) std::cout<< _msg_ <<std::endl;
	#define ASSERT_DEBUG( _code_ ) assert(_code_);
	#define ASSERT_DEBUG_MSG( _code_, _msg_ )\
			{ if (!(_code_)) { std::cout << _msg_ << std::endl; } assert(_code_); } 
#else
	#define MESSAGE_DEBUG( _msg_ ) 
	#define ASSERT_DEBUG( _code_ ) 
	#define ASSERT_DEBUG_MSG( _code_, _msg_ )\
			{ if (!(_code_)) { std::cout << _msg_ << std::endl; } assert(_code_); } 
#endif

namespace Denn
{
	enum class DataType
	{
		DT_UNKNOWN      =-1,
		DT_FLOAT        = 1,
		DT_DOUBLE       = 2,
		DT_LONG_DOUBLE  = 3 
	};

	template < typename ScalarType >
	inline DataType GetDataType(){ return DataType::DT_UNKNOWN; }
	template <>
	inline DataType GetDataType<float>(){ return DataType::DT_FLOAT; }
	template <>
	inline DataType GetDataType<double>(){ return DataType::DT_DOUBLE; }
	template <>
	inline DataType GetDataType<long double>(){ return DataType::DT_LONG_DOUBLE; }
}