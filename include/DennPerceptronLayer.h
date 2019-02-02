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
			  const Layer::Shape&			    shape
			, const Layer::Input&			    input
			, const Layer::VActivationFunction& active_function
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
		virtual VActivationFunction get_activation_functions()                                           override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t ouput_paramters() const override;
		virtual size_t input_shape_dims() const override;
		virtual Layer::Shape output_shape() const override;
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////
		virtual VMatrix   apply(const std::vector<Matrix>& input) const;
		///////////////////////////////////////////////////////////////////////////
		//Backpropagation stuff
		virtual VMatrix   feedforward(const VMatrix& input);
		virtual VMatrix   backpropagate(const VMatrix& input);
		virtual void      optimizer(BPOptimizer& optimizer);
		///////////////////////////////////////////////////////////////////////////

	protected:

		Matrix         m_weights;
		Matrix         m_baias;
		ActivationFunction m_activation_function{ nullptr };
	};
	REGISTERED_LAYER
	(
		  PerceptronLayer
		, LAYER_NAMES(
		      "perceptron"
			, "lp"
			, "fullconnected"
		    , "fc"
	      )
		, LayerMinMax(0,1)
		, LayerMinMax(1)
		, LayerMinMax(0)
	)
}