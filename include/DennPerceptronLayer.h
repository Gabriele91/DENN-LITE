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

		PerceptronLayer(const PerceptronLayer& lpptr);
		//////////////////////////////////////////////////
		Matrix& weights();
		Matrix& baias();
		//////////////////////////////////////////////////
		const Matrix& weights() const;
		const Matrix& baias()   const;
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual Matrix              apply(const Matrix& input) const override;
		//////////////////////////////////////////////////
		//Backpropagation stuff
		virtual Matrix feedforward(const Matrix& input) override;
		virtual Matrix backpropagate(const Matrix& error, Scalar eta = 0.1, Scalar momentum = 0.5) override;
		//////////////////////////////////////////////////
		virtual ActivationFunction get_activation_function()							       override;
		virtual void               set_activation_function(ActivationFunction active_function) override;
		//////////////////////////////////////////////////
		virtual size_t size() const operator_override;		
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:

		Matrix         m_weights;
		Matrix         m_baias;
		ActivationFunction m_activation_function{ nullptr };
	
		//backpropagation context
		struct BPContext
		{
			Matrix m_input;
			Matrix m_output;
			Matrix m_dweights;
			Matrix m_dbaias;
		};
		//utils
		void free_context();
		BPContext& context();
		Matrix&    output();
		Matrix&    input();
		Matrix&    dW();
		Matrix&    dB();
		const BPContext& context() const;
		const Matrix&    output() const;
		const Matrix&    input() const;
		const Matrix&    dW() const;
		const Matrix&    dB() const;
		//help
		Matrix compute_delta(const Matrix& err) const;
		//context ptr
		mutable std::unique_ptr<BPContext> m_context;

	};
}