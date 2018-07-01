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
			  size_t features
			, size_t iweight
			, size_t output
		);

		RecurrentLayer
		(     
			  ActivationFunction active_function_output
			, size_t features
			, size_t iweight
			, size_t output
		);

		RecurrentLayer
		(
			  ActivationFunction active_function_inside
			, ActivationFunction active_function_output
			, size_t features
			, size_t iweight
			, size_t output
		);

		RecurrentLayer
		(
			  const std::vector< ActivationFunction >& active_function
			, const std::vector< size_t >& input_output
		);
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
		virtual VActivationFunction get_activation_functions()										     override;
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
        //parameters
		Matrix             m_U;
		Matrix             m_W;
		Matrix             m_B;
		Matrix             m_V;
		Matrix             m_C;
        //function
		ActivationFunction m_activation_function_inside{ nullptr };
		ActivationFunction m_activation_function_output{ nullptr };
	};
	REGISTERED_LAYER(RecurrentLayer, "recurrent", LayerMinMax(1, 2), LayerMinMax(1, 2), LayerMinMax(1, 1))
}