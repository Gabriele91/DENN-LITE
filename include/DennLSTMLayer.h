#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
    //RNN
	class LSTMLayer : public Layer
	{
	public:
		struct GateType{ enum 
		{
			I,    //INPUT
			F,    //FORGET
			C,    //CANDIDATE
			O,    //OUTPUT
			NGATE //NGATE
		};};
		struct FuncType{ enum 
		{
			G,    //INPUT/FORGET/OUTPUT
			C,    //CANDIDATE
			H,	  //LAST
			NFUC
		};};
		///////////////////////////////////////
		LSTMLayer
		(
			  size_t features
			, size_t weight
		);

		LSTMLayer
		(
			  ActivationFunction active_function_c_h
			, size_t features
			, size_t weight
		);

		LSTMLayer
		(
			  ActivationFunction active_function_g
			, ActivationFunction active_function_c_h
			, size_t features
			, size_t weight
		);
		LSTMLayer
		(
			  ActivationFunction active_function_g
			, ActivationFunction active_function_c
			, ActivationFunction active_function_h
			, size_t features
			, size_t weight
		);

		LSTMLayer
		(
			  const std::vector< ActivationFunction >& active_function
			, const std::vector< size_t >& input_output
		);
		//////////////////////////////////////////////////
		Matrix& U(GateType);
		Matrix& W(GateType);
		Matrix& B(GateType);
		Matrix& V();
		//////////////////////////////////////////////////
		const Matrix& U(GateType)   const;
		const Matrix& W(GateType)   const;
		const Matrix& B(GateType)   const;
		const Matrix& V()   const;
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
		Matrix             m_U[GateType::NGATE];
		Matrix             m_W[GateType::NGATE];
		Matrix             m_B[GateType::NGATE];
        //function
		ActivationFunction m_activation_function[FuncType::NFUC]{ nullptr,nullptr };
	};
	REGISTERED_LAYER(LSTMLayer, "lstm", LayerMinMax(1), LayerMinMax(1, 3), LayerMinMax(0))
}