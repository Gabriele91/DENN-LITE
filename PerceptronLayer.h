#pragma once
#include "Config.h"
#include "ActiveFunction.h"

namespace Denn
{
	template < class Matrix = Eigen::MatrixXd >
	class PerceptronLayer
	{
	public:
		///////////////////////////////////////
		using MatrixType    = typename Matrix;
		using ScalarType    = typename Matrix::Scalar;
		using RowVectorType = typename Eigen::Matrix<ScalarType, 1, Eigen::Dynamic>;
		///////////////////////////////////////
		PerceptronLayer
		(
			  int features
			, int clazz
		)
		{
			m_weights.resize(features, clazz);
			m_baias.resize(clazz);
		}

		PerceptronLayer
		(
			  ActiveFunction< ScalarType > active_function
			, size_t features
			, size_t clazz
		)
		{
			m_active_function = active_function;
			m_weights.resize(features, clazz);
			m_baias.resize(clazz);
		}

		//////////////////////////////////////////////////
		Matrix&		   weights() { return m_weights; }
		RowVectorType& baias()   { return m_baias;   }
		//////////////////////////////////////////////////
		const Matrix&		 weights() const  { return m_weights; }
		const RowVectorType& baias()   const  { return m_baias;   }
		//////////////////////////////////////////////////
		Matrix apply(const Matrix& input)
		{
			if (m_active_function) return apply_function(apply_input(input));
			else                  return apply_input(input);
		}
		//////////////////////////////////////////////////
		struct WrapperArray
		{
			WrapperArray(ScalarType* ptr_array, size_t size)
			{
				m_data = ptr_array;
				m_size = size;
			}

			size_t size() const
			{
				return m_size;
			}

			ScalarType& operator()(size_t i)
			{
				return m_data[i];
			}

			ScalarType operator()(size_t i) const
			{
				return m_data[i];
			}

			ScalarType& operator[](size_t i)
			{
				return m_data[i];
			}

			ScalarType operator[](size_t i) const
			{
				return m_data[i];
			}

		protected:

			ScalarType* m_data{ nullptr };
			size_t		m_size{ 0       };
		};

		constexpr size_t size() const
		{
			return 2;
		}
		
		WrapperArray operator[](size_t i)
		{
			if (i & 0x1) return  WrapperArray(weights().array().data(), weights().array().size());
			else		 return  WrapperArray(baias().array().data(), baias().array().size());
		}

		const WrapperArray operator[](size_t i) const
		{
			if (i & 0x1) return  WrapperArray(weights().array().data(), weights().array().size());
			else		 return  WrapperArray(baias().array().data(), baias().array().size());
		}
		//////////////////////////////////////////////////

	protected:

		inline Matrix apply_input(const Matrix& input)
		{
			return (input * m_weights).rowwise() + m_baias;
		}

		inline Matrix apply_function(const Matrix& to_eval)
		{
			return to_eval.unaryExpr(m_active_function);
		}

		MatrixType    m_weights;
		RowVectorType m_baias;
		ActiveFunction< ScalarType > m_active_function{ nullptr };
	};
	//////////////////////////////////////////////////////ALIAS
	using PerceptronLayerD = PerceptronLayer< MatrixD >;
	using PerceptronLayerF = PerceptronLayer< MatrixF >;
}