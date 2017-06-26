#pragma once
#include "Config.h"
#include "Layer.h"
#include "ActiveFunction.h"

namespace Denn
{

	class PerceptronLayer : public Layer
	{
	public:
		///////////////////////////////////////
		PerceptronLayer
		(
			  int features
			, int clazz
		)
		{
			m_weights.resize(features, clazz);
			m_baias.resize(1, clazz);
		}

		PerceptronLayer
		(
			  ActiveFunction::Ptr<Matrix> active_function
			, size_t features
			, size_t clazz
		)
		{
			m_active_function = active_function;
			m_weights.resize(features, clazz);
			m_baias.resize(1, clazz);
		}

		//////////////////////////////////////////////////
		Matrix& weights() { return m_weights; }
		Matrix& baias()   { return m_baias;   }
		//////////////////////////////////////////////////
		const Matrix& weights() const  { return m_weights; }
		const Matrix& baias()   const  { return m_baias;   }
		//////////////////////////////////////////////////
		virtual Layer* copy() const override
		{
			return (Layer*)new PerceptronLayer(*this);
		}
		//////////////////////////////////////////////////
		virtual Matrix apply(const Matrix& input) override
		{
			//get output
			Matrix layer_output = (input * m_weights).rowwise() 
								+ RowVector(Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows()));
			//activation function?
			if (m_active_function) return m_active_function(layer_output);
			else                   return layer_output;
		}
		//////////////////////////////////////////////////
		virtual size_t size() const override
		{
			return 2;
		}
		
		virtual Matrix& operator[](size_t i) override
		{
			if (i & 0x1) return  m_weights;
			else		 return  m_baias;
		}

		virtual const Matrix& operator[](size_t i) const override
		{
			if (i & 0x1) return  m_weights;
			else		 return  m_baias;
		}
		//////////////////////////////////////////////////

	protected:

		Matrix m_weights;
		Matrix m_baias;
		ActiveFunction::Ptr<Matrix> m_active_function{ nullptr };
	};
}