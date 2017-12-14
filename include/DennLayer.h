#pragma once
#include "Config.h"
#include "DennActivationFunction.h"

namespace Denn
{
	class Layer : public std::enable_shared_from_this< Layer >
	{ 
	public:
		//ref to Layer
		using SPtr = std::shared_ptr<Layer>;
		//return ptr
		SPtr get_ptr();
		///////////////////////////////////////////////////////////////////////////
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW	
		///////////////////////////////////////////////////////////////////////////
		virtual Layer::SPtr copy() const  				                                                 = 0;
		virtual Matrix apply(const Matrix& input) const								                     = 0;
		virtual Matrix              feedforward(const Matrix& input, Matrix& ff_out)													      = 0;
		virtual Matrix              backpropagate_delta   (const Matrix& bp_delta, const Matrix& ff_out)								      = 0;
		virtual std::vector<Matrix> backpropagate_gradient(const Matrix& bp_delta, const Matrix& ff_out, size_t input_samples, Scalar lambda) = 0;
		virtual size_t size() const											                             = 0;
		virtual Matrix& operator[](size_t i)								                             = 0;
		virtual const Matrix& operator[](size_t i) const						                         = 0;		
		virtual ActivationFunction get_activation_function()													 = 0;
		virtual void           set_activation_function(ActivationFunction active_function)						 = 0;
		///////////////////////////////////////////////////////////////////////////
		class Iterator 
		{
		public:

			Iterator(const Iterator& it);
			Iterator(Layer& layer, size_t index);
			Iterator(const Layer& layer, size_t index);
	
			Iterator& operator++();
			Iterator operator++(int);
			
			bool operator==(const Iterator& rhs) const;
			bool operator!=(const Iterator& rhs) const;

			Matrix& operator*();
			const Matrix& operator*() const;

		protected:

			friend class Denn::Layer;
			Layer* m_layer;
			size_t m_index;
		};

		Iterator begin();
		Iterator end();
		const Iterator begin() const;
		const Iterator end()   const;
		///////////////////////////////////////////////////////////////////////////
	};
}