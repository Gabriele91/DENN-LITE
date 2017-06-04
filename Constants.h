#pragma once

namespace Denn
{
namespace Constants
{

	template < typename ScalarType = double >
	constexpr inline ScalarType pi()
	{
		return ScalarType(3.14159265358979323846264338327950288);
	}

	template < typename ScalarType = double >
	constexpr inline ScalarType two_pi()
	{
		return ScalarType(6.28318530717958647692528676655900576);
	}

	template < typename ScalarType = double >
	constexpr inline ScalarType e()
	{
		return ScalarType(2.71828182845904523536);
	}

	template < typename ScalarType = double >
	constexpr inline ScalarType epsilon()
	{
		return std::numeric_limits<ScalarType>::epsilon();
	}
}
}