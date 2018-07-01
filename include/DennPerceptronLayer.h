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

		PerceptronLayer
		(
			  const std::vector< ActivationFunction >& active_function
			, const std::vector< size_t >&			   input_output
		)
		: PerceptronLayer(active_function[0], input_output[0], input_output[1])
		{

		}
		//////////////////////////////////////////////////
		Matrix& weights();
		Matrix& baias();
		//////////////////////////////////////////////////
		const Matrix& weights() const;
		const Matrix& baias()   const;
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual Matrix  apply(const Matrix& input) const override;
		//////////////////////////////////////////////////
		virtual Matrix  feedforward(const Matrix& input, Matrix& linear_out)				                               override;
		virtual Matrix  backpropagate_delta(const Matrix& loss)       			               							   override;
		virtual Matrix  backpropagate_derive(const Matrix& delta, const Matrix& linear_out)       			               override;
		virtual VMatrix backpropagate_gradient(const Matrix& delta, const Matrix& linear_inpu, Scalar regular=Scalar(0.0)) override;
		//////////////////////////////////////////////////
		virtual VActivationFunction get_activation_functions()                                           override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const														  			   override;
		virtual VMatrix  feedforward(const VMatrix& input, VMatrix& linear_out)				  							       override;
		virtual VMatrix  backpropagate_delta(const VMatrix& loss)     							                               override;		
		virtual VMatrix  backpropagate_derive(const VMatrix& delta, const VMatrix& linear_out)       			               override;
		virtual VVMatrix backpropagate_gradient(const VMatrix& delta, const VMatrix& linear_input, Scalar regular=Scalar(0.0)) override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t size_ouput() const override;
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:

		Matrix         m_weights;
		Matrix         m_baias;
		ActivationFunction m_activation_function{ nullptr };
	};
	REGISTERED_LAYER(PerceptronLayer, "perceptron", LayerMinMax(1), LayerMinMax(0))
}