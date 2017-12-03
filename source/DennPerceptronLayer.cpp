#include "DennActiveFunction.h"
#include "DennPerceptronLayer.h"

namespace Denn
{
	///////////////////////////////////////	
	PerceptronLayer::PerceptronLayer
	(
		  int features
		, int clazz
	)
	{
		m_weights.resize(features, clazz);
		m_baias.resize(1, clazz);
	}

	PerceptronLayer::PerceptronLayer
	(
		  ActiveFunction active_function
		, size_t features
		, size_t clazz
	)
	{
		set_active_function(active_function);
		m_weights.resize(features, clazz);
		m_baias.resize(1, clazz);
	}
	//////////////////////////////////////////////////
	Matrix& PerceptronLayer::weights() { return m_weights; }
	Matrix& PerceptronLayer::baias() { return m_baias; }
	//////////////////////////////////////////////////
	const Matrix& PerceptronLayer::weights() const { return m_weights; }
	const Matrix& PerceptronLayer::baias()   const { return m_baias; }
	//////////////////////////////////////////////////
	Layer::SPtr PerceptronLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<PerceptronLayer>(*this));
	}
	//////////////////////////////////////////////////
	Matrix PerceptronLayer::apply(const Matrix& input) const
	{
		//get output
		Matrix layer_output = (input * m_weights).rowwise() + Eigen::Map<RowVector>((Scalar*)m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_active_function) return m_active_function(layer_output);
		else                   return layer_output;
	}
	Matrix PerceptronLayer::feedforward(const Matrix& input, Matrix& ff_out)
	{		
		//get output
		ff_out = (input * m_weights).rowwise() + Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_active_function)
		{
			Matrix out_matrix(ff_out);
            m_active_function(out_matrix);
			return out_matrix;
		} 
		else
		{ 
			return ff_out; 
		}
	}
    Matrix PerceptronLayer::backpropagate_delta(const Matrix& bp_delta, const Matrix& ff_out)
    {
        //////////////////////////////////////////////////////////////////////
        if (m_active_function.exists_function_derivate())
        {
            //derivate of active function
            Matrix d_f(ff_out);
            d_f = m_active_function.derive(d_f); // g := D_f(x))
            return d_f.transpose().cwiseProduct(m_weights * bp_delta);
        }
        else
        {
            //f(x) = x, g := D_f(x) => 1.0
            return m_weights * bp_delta;
        } 
        //////////////////////////////////////////////////////////////////////
    }
    std::vector<Matrix> PerceptronLayer::backpropagate_gradient(
          const Matrix& delta
        , const Matrix& lout
        , size_t input_samples
        , Scalar regular_param
    )
    {
        Scalar inv_input_samples = Scalar(1.0) / Scalar(input_samples);
        // add regularization to weights, bias weights are not regularized
        Matrix dEdW = ((delta * lout).transpose()) * inv_input_samples;
        Matrix dEdb = (delta.transpose().colwise().sum()) * inv_input_samples;
        //add regular factor
        if (regular_param != Scalar(0.0)) dEdW += (regular_param * inv_input_samples)*m_weights;
        //return
        return std::vector<Matrix>{dEdW /* this[0] = w */, dEdb /* this[1] = b */};
        //J = Scalar(0.5) * lambda * m_weights.array().square().sum() / Scalar(input_samples);
    }
	//////////////////////////////////////////////////
	ActiveFunction PerceptronLayer::get_active_function() 
	{
		return m_active_function;
	}
	void PerceptronLayer::set_active_function(ActiveFunction active_function)
	{
		m_active_function = ActiveFunctionFactory::name_of(active_function) == "linear" ? nullptr : active_function;
	}
    //////////////////////////////////////////////////
	size_t PerceptronLayer::size() const
	{
		return 2;
	}
	Matrix& PerceptronLayer::operator[](size_t i)
	{
		denn_assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	const Matrix& PerceptronLayer::operator[](size_t i) const
	{
		denn_assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	//////////////////////////////////////////////////
}
