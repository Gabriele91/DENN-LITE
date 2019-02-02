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
			  const Layer::Shape&			    shape
			, const Layer::Input&			    input
			, const Layer::VActivationFunction& active_functions
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
		virtual VActivationFunction get_activation_functions()										     override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t ouput_paramters() const override;
		virtual size_t input_shape_dims() const override;
		virtual Layer::Shape output_shape() const override;
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