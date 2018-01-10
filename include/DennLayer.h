#pragma once
#include "Config.h"
#include "DennActivationFunction.h"

namespace Denn
{
class Layer : public std::enable_shared_from_this< Layer >
{ 
public:
	//ref to Layer
	using Scalar = Denn::Scalar;
	using SPtr   = std::shared_ptr<Layer>;
	//return ptr
	SPtr get_ptr();
	///////////////////////////////////////////////////////////////////////////
	//EIGEN_MAKE_ALIGNED_OPERATOR_NEW	
	///////////////////////////////////////////////////////////////////////////
	virtual Layer::SPtr copy() const  				                                                 = 0;
	virtual Matrix apply(const Matrix& input) const								                     = 0;
	virtual size_t size() const											                             = 0;
	virtual Matrix& operator[](size_t i)								                             = 0;
	virtual const Matrix& operator[](size_t i) const						                         = 0;		
	virtual ActivationFunction get_activation_function()											 = 0;
	virtual void           set_activation_function(ActivationFunction active_function)				 = 0;
	///////////////////////////////////////////////////////////////////////////
	//Backpropagation stuff
	virtual Matrix              feedforward(const Matrix& input, Matrix& linear_out)				  							   = 0;
	virtual Matrix              backpropagate_delta(const Matrix& loss)     							                           = 0;		
	virtual Matrix              backpropagate_derive(const Matrix& delta, const Matrix& linear_out)       			               = 0;
	virtual std::vector<Matrix> backpropagate_gradient(const Matrix& delta, const Matrix& linear_input, Scalar regular=Scalar(0.0))= 0;
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

template <>
inline Layer::Scalar distance_pow2<Layer>(const Layer& a, const Layer& b)
{
	//bad case
	if(a.size()!=b.size()) return std::numeric_limits<Scalar>::infinity();
	//value
	Scalar dpow2 = 0.0;
	//sum
	for(size_t i = 0; i!=a.size() ; ++i) dpow2 += distance_pow2(a[i],b[i]);
	//return 
	return dpow2;
} 

}