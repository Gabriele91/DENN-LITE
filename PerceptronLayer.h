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
		using MatrixType    = Matrix;
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
			  ActiveFunction::Ptr<MatrixType> active_function
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
			//get output
			Matrix layer_output = (input * m_weights).rowwise() + m_baias;
			//activation function?
			if (m_active_function) return m_active_function(layer_output);
			else                   return layer_output;
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
			if (i & 0x1) return  WrapperArray(baias().array().data(), baias().array().size());
			else		 return  WrapperArray(weights().array().data(), weights().array().size());
		}

		const WrapperArray operator[](size_t i) const
		{
			if (i & 0x1) return  WrapperArray(baias().array().data(), baias().array().size());
			else		 return  WrapperArray(weights().array().data(), weights().array().size());
		}
		//////////////////////////////////////////////////

	protected:

		MatrixType    m_weights;
		RowVectorType m_baias;
		ActiveFunction::Ptr<MatrixType> m_active_function{ nullptr };
	};
	//////////////////////////////////////////////////////ALIAS
	using PerceptronLayerD = PerceptronLayer< MatrixD >;
	using PerceptronLayerF = PerceptronLayer< MatrixF >;
}