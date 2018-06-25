#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
    //RNN
	class RecurrentLayer : public Layer
	{
	public:
		///////////////////////////////////////
		RecurrentLayer
		(
			  int features
			, int clazz
		);

		RecurrentLayer
		(
			  ActivationFunction active_function
			, size_t features
			, size_t clazz
		);

		RecurrentLayer
		(
			  ActivationFunction active_function
			, const std::vector< size_t >& input_output
		)
		: RecurrentLayer(active_function, input_output[0], input_output[1])
		{

		}
		//////////////////////////////////////////////////
		Matrix& U();
		Matrix& W();
		Matrix& B();
		Matrix& V();
		Matrix& C();
		//////////////////////////////////////////////////
		const Matrix& U()   const;
		const Matrix& W()   const;
		const Matrix& B()   const;
		const Matrix& V()   const;
		const Matrix& C()   const;
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
		virtual ActivationFunction get_activation_function()							       override;
		virtual void               set_activation_function(ActivationFunction active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const														  			   override;
		virtual VMatrix  feedforward(const VMatrix& input, VMatrix& linear_out)				  							       override;
		virtual VMatrix  backpropagate_delta(const VMatrix& loss)     							                               override;		
		virtual VMatrix  backpropagate_derive(const VMatrix& delta, const VMatrix& linear_out)       			               override;
		virtual VVMatrix backpropagate_gradient(const VMatrix& delta, const VMatrix& linear_input, Scalar regular=Scalar(0.0)) override;
		//////////////////////////////////////////////////
		virtual size_t size() const operator_override;		
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:  
        //parameters
		Matrix             m_U;
		Matrix             m_W;
		Matrix             m_B;
		Matrix             m_V;
		Matrix             m_C;
        //function
		ActivationFunction m_activation_function{ nullptr };
	};
	REGISTERED_LAYER(RecurrentLayer, "recurrent")
}