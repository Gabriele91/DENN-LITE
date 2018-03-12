#pragma once
//default C include
#include <cstring>
#include <cmath>
#include <cassert>
#include <cctype>
//default CPP include
#include <limits>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
//lib include
#include <Eigen/Eigen>
#include <zlib.h>
//defines
#define USE_CROSS_ENTROPY_SAFE
#define RANDOM_SAFE_MUTATION
#define RANDOM_SAFE_CROSSOVER
#define RANDOM_SAFE_EVOLUTION_METHOD
//macro
#define MESSAGE( _msg_ ) std::cout<< _msg_ <<std::endl;
//MSMACRO
#if defined( _MSC_VER )
	#define ASPACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
	#define operator_override
	#define denn_noop __noop 
#else 
	#define ASPACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
	#define operator_override override
	#ifdef __clang__
		#define denn_noop ((void)0)
	#else
		#define denn_noop (__ASSERT_VOID_CAST (0))
	#endif 
#endif
//Debug
#if !defined(  NDEBUG )
	#define debug_message(_msg_) std::cout<< _msg_ <<std::endl;
	#define denn_assert(_exp_) assert(_exp_)
	#define denn_assert_code(_code_)  assert( _code_ );
#else
	#define debug_message(_msg_)
	#define denn_assert(_exp_) denn_noop
	#define denn_assert_code(_code_)  _code_ ;
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

	template < typename T >
	using RowVectorT  = typename Eigen::Matrix<T, 1, Eigen::Dynamic>;
	using RowVectorLD = typename Eigen::Matrix<long double, 1, Eigen::Dynamic>;
	using RowVectorD  = typename Eigen::Matrix<double, 1, Eigen::Dynamic>;
	using RowVectorF  = typename Eigen::Matrix<float,  1, Eigen::Dynamic>;
	using RowVector   = typename Eigen::Matrix<Scalar,  1, Eigen::Dynamic>;

	template < typename T >
	using ColVectorT  = typename Eigen::Matrix<T, Eigen::Dynamic, 1>;
	using ColVectorLD = typename Eigen::Matrix<long double, Eigen::Dynamic, 1>;
	using ColVectorD  = typename Eigen::Matrix<double, Eigen::Dynamic, 1>;
	using ColVectorF  = typename Eigen::Matrix<float,  Eigen::Dynamic, 1>;
	using ColVector   = typename Eigen::Matrix<Scalar,  Eigen::Dynamic, 1>;

	template < typename T >
	using MatrixT	 = typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixLD	 = typename Eigen::Matrix<long double, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixD	 = typename Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
	using MatrixF	 = typename Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
	using Matrix 	 = typename Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic > ;

	template < typename T >
	using MatrixListT  = std::vector < MatrixT< T > >;
	using MatrixListF  = std::vector < MatrixF >;
	using MatrixListD  = std::vector < MatrixD >;
	using MatrixListLD = std::vector < MatrixLD >;
	using MatrixList   = std::vector < Matrix >;


	/* Map Version */
	template < typename T >
	using MapRowVectorT  = typename Eigen::Map< RowVectorT<T> >;
	using MapRowVectorLD = typename Eigen::Map< RowVectorLD >;
	using MapRowVectorD  = typename Eigen::Map< RowVectorD >;
	using MapRowVectorF  = typename Eigen::Map< RowVectorF >;
	using MapRowVector   = typename Eigen::Map< RowVector >;

	template < typename T >
	using MapColVectorT  = typename Eigen::Map< ColVectorT<T> >;
	using MapColVectorLD = typename Eigen::Map< ColVectorLD >;
	using MapColVectorD  = typename Eigen::Map< ColVectorD >;
	using MapColVectorF  = typename Eigen::Map< ColVectorF >;
	using MapColVector   = typename Eigen::Map< ColVector >;

	template < typename T >
	using MapMatrixT	 = typename Eigen::Map< MatrixT<T> >;
	using MapMatrixLD	 = typename Eigen::Map< MatrixLD >;
	using MapMatrixD	 = typename Eigen::Map< MatrixD >;
	using MapMatrixF	 = typename Eigen::Map< MatrixF >;
	using MapMatrix 	 = typename Eigen::Map< Matrix > ;

	template < typename T >
	using MapMatrixListT  = std::vector < MapMatrixT< T > >;
	using MapMatrixListF  = std::vector < MapMatrixF >;
	using MapMatrixListD  = std::vector < MapMatrixD >;
	using MapMatrixListLD = std::vector < MapMatrixLD >;
	using MapMatrixList   = std::vector < MapMatrix >;

	//rows to map
	template < typename T >
	inline MapRowVectorT< T > as_map(RowVectorT< T >& value)
	{ return MapRowVectorT< T >(value.data(), value.size()); }
	inline MapRowVectorLD as_map(RowVectorLD& value)
	{ return MapRowVectorLD(value.data(), value.size()); }
	inline MapRowVectorD as_map(RowVectorD& value)
	{ return MapRowVectorD(value.data(), value.size()); }
	inline MapRowVectorF as_map(RowVectorF& value)
	{ return MapRowVectorF(value.data(), value.size()); }

	//cols to map
	template < typename T >
	inline MapColVectorT< T > as_map(ColVectorT< T >& value)
	{ return MapColVectorT< T >(value.data(), value.size()); }
	inline MapColVectorLD as_map(ColVectorLD& value)
	{ return MapColVectorLD(value.data(), value.size()); }
	inline MapColVectorD as_map(ColVectorD& value)
	{ return MapColVectorD(value.data(), value.size()); }
	inline MapColVectorF as_map(ColVectorF& value)
	{ return MapColVectorF(value.data(), value.size()); }

	//matrix to map
	template < typename T >
	inline MapMatrixT< T > as_map(MatrixT< T >& value)
	{ return MapMatrixT< T >(value.data(), value.rows(), value.cols()); }
	inline MapMatrixLD as_map(MatrixLD& value)
	{ return MapMatrixLD(value.data(), value.rows(), value.cols()); }
	inline MapMatrixD as_map(MatrixD& value)
	{ return MapMatrixD(value.data(), value.rows(), value.cols()); }
	inline MapMatrixF as_map(MatrixF& value)
	{ return MapMatrixF(value.data(), value.rows(), value.cols()); }
}
//types
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
	inline DataType get_data_type(){ return DataType::DT_UNKNOWN; }
	template <>
	inline DataType get_data_type<float>(){ return DataType::DT_FLOAT; }
	template <>
	inline DataType get_data_type<double>(){ return DataType::DT_DOUBLE; }
	template <>
	inline DataType get_data_type<long double>(){ return DataType::DT_LONG_DOUBLE; }

}
//internal include
#include "DennTicksTime.h"
//utilities
#include "DennUtilitiesMath.h"
#include "DennUtilitiesMatrix.h"
#include "DennUtilitiesString.h"
#include "DennUtilitiesVector.h"


