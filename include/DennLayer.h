#pragma once
#include "Config.h"
#include "DennShape.h"
#include "DennOptimizer.h"
#include "DennActivationFunction.h"

namespace Denn
{

class Layer : public std::enable_shared_from_this< Layer >
{
public:


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

	//alias
	using Matrix = Denn::Matrix;
	using Scalar = Denn::Scalar;
	using SPtr = std::shared_ptr<Layer>;
	//Costructor
	Layer(Shape in, Shape out): m_in_size(in), m_out_size(out) {}
	//info shape
	virtual Shape in_size() const { return m_in_size; }
	virtual Shape out_size() const { return m_in_size; }
	///////////////////////////////////////////////////////////////////////////
	virtual void activation(ActivationFunction) = 0;
	virtual ActivationFunction activation() const = 0;
	///////////////////////////////////////////////////////////////////////////
	//return ptr
	SPtr get_ptr();
	virtual SPtr copy()   const = 0;
	///////////////////////////////////////////////////////////////////////////
	virtual const Matrix& feedforward(const Matrix& prev_layer_data)								  = 0;
	virtual const Matrix& backpropagate(const Matrix& prev_layer_data, const Matrix& next_layer_data) = 0;
	///////////////////////////////////////////////////////////////////////////
	virtual void update(const Optimizer& optimize) = 0;
	///////////////////////////////////////////////////////////////////////////
	virtual const Matrix& ff_output() = 0;
	virtual const Matrix& bp_output() = 0;
	///////////////////////////////////////////////////////////////////////////
	virtual size_t        size()  const              = 0;
	virtual Matrix&       operator[](size_t i)       = 0;
	virtual const Matrix& operator[](size_t i) const = 0;
	///////////////////////////////////////////////////////////////////////////
	Iterator begin();
	Iterator end();
	const Iterator begin() const;
	const Iterator end()   const;
	///////////////////////////////////////////////////////////////////////////

protected:

	const Shape m_in_size;  // Size of input units
	const Shape m_out_size; // Size of output units 

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