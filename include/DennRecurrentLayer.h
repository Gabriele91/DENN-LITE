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

		RecurrentLayer
		(
			  const Layer::Shape&			    shape
			, const Layer::Input&			    input
			, const Layer::VActivationFunction& active_functions
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
		virtual VActivationFunction get_activation_functions()										     override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const	override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t ouput_paramters() const override;
		size_t input_shape_dims() const  override;
		Layer::Shape output_shape() const override;
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