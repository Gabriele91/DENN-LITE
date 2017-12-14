#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

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
			  ActivationFunction active_function
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
		virtual Matrix              apply(const Matrix& input) const                                                                          override;
		virtual Matrix              feedforward(const Matrix& input, Matrix& ff_out)			                                              override;
		virtual Matrix              backpropagate_delta   (const Matrix& bp_delta, const Matrix& ff_out)                                      override;
		virtual std::vector<Matrix> backpropagate_gradient(const Matrix& bp_delta, const Matrix& ff_out, size_t input_samples, Scalar lambda) override;
		//////////////////////////////////////////////////
		virtual ActivationFunction get_activation_function()							   override;
		virtual void           set_activation_function(ActivationFunction active_function) override;
		//////////////////////////////////////////////////
		virtual size_t size() const operator_override;		
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:

		Matrix         m_weights;
		Matrix         m_baias;
		ActivationFunction m_activation_function{ nullptr };
	};
}