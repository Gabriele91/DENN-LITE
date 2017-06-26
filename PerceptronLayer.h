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
		);

		PerceptronLayer
		(
			ActiveFunction::Ptr<Matrix> active_function
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
		virtual Layer* copy() const override;
		//////////////////////////////////////////////////
		virtual Matrix apply(const Matrix& input) override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;		
		virtual Matrix& operator[](size_t i) override;
		virtual const Matrix& operator[](size_t i) const;
		//////////////////////////////////////////////////

	protected:

		Matrix m_weights;
		Matrix m_baias;
		ActiveFunction::Ptr<Matrix> m_active_function{ nullptr };
	};
}