#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActiveFunction.h"

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
		);

		PerceptronLayer
		(
			  ActiveFunction active_function
			, size_t features
			, size_t clazz
		);
		//////////////////////////////////////////////////
		Matrix& weights();
		Matrix& baias();
		//////////////////////////////////////////////////
		const Matrix& weights() const;
		const Matrix& baias()   const;
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual Matrix apply(const Matrix& input) override;
		virtual Matrix backpropagate(const Matrix& last_output, const Matrix& input) override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;		
		virtual Matrix& operator[](size_t i) override;
		virtual const Matrix& operator[](size_t i) const;
		//////////////////////////////////////////////////

	protected:

		Matrix         m_weights;
		Matrix         m_baias;
		ActiveFunction m_active_function{ nullptr };
	};
}