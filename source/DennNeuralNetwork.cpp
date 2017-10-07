#include "DennNeuralNetwork.h"

namespace Denn
{	
	////////////////////////////////////////////////////////////////
	//  default constructor 
	NeuralNetwork::NeuralNetwork()
	{
	}
	//  default copy constructor  and assignment operator
	NeuralNetwork::NeuralNetwork(const NeuralNetwork& nn)
	{
		//alloc
		m_layers.clear();
		//copy all layers
		for (size_t i = 0; i != nn.size(); ++i)
		{
			m_layers.push_back(nn[i].copy()->get_ptr());
		}
	}
	NeuralNetwork& NeuralNetwork::operator= (const NeuralNetwork & nn)
	{
		//alloc
		m_layers.clear();
		//copy all layers
		for (size_t i = 0; i != nn.size(); ++i)
		{
			m_layers.push_back(nn[i].copy()->get_ptr());
		}
		//self return
		return *this;
	}
	/////////////////////////////////////////////////////////////////////////
	Matrix NeuralNetwork::apply(const Matrix& input)
	{
		//no layer?
		assert(m_layers.size());
		//input layer
		Matrix output = m_layers[0]->apply(input);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = m_layers[i]->apply(output);
		}
		//return
		return output;
	}

    struct SGD_BackpropagationContext : public NeuralNetwork::BackpropagationContext
    {
        std::vector< Matrix >                 m_deltas;
        std::vector< std::vector < Matrix > > m_gradients;
        
        SGD_BackpropagationContext(size_t size)
        : m_deltas(size)
        , m_gradients(size)
        {
        }
        
        virtual void clear() override
        {
            //all delta to 0
            for(auto& delta : m_deltas) delta.fill(0);
            //all gradient to 0
            for(auto& gradient : m_gradients)
            for(auto& matrix   : gradient   )
            {
                matrix.fill(0);
            }
        }
    };
    //SGD BackpropagationContext pointer
    using SGD_BackpropagationContext_sptr = std::shared_ptr<SGD_BackpropagationContext>;
    //util
    std::vector < Matrix > operator + (  const std::vector< Matrix >& left
                                       , const std::vector< Matrix >& right)
    {
        assert(left.size()==right.size());
        std::vector< Matrix > output(left.size());
        for(size_t i = 0; i != left.size(); ++i) output[i] = left[i] + right[i];
        return output;
    }
    
	//execute 
    NeuralNetwork::BackpropagationContext_sptr NeuralNetwork::backpropagation_with_sgd(
		  std::function<Matrix(const Matrix& x, const Matrix& y)> loss_function
		, const Matrix& input
		, const Matrix& y
		, const Scalar learn_rate
		, const Scalar regular_param
        , NeuralNetwork::BackpropagationContext_sptr context
	)
	{		
		//no layer?
		assert(m_layers.size());
        //input size
        size_t input_size = input.rows();
		//output of layer
		std::vector< Matrix > z_outputs(m_layers.size());
		std::vector< Matrix > s_outputs(m_layers.size());
		//////////////////////////////////////////////////////////////////////
		//FEED FORWAORD PROPAGATION
		//output
		z_outputs[0] = m_layers[0]->feedforward(input, s_outputs[0]);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			z_outputs[i] = m_layers[i]->feedforward(z_outputs[i-1], s_outputs[i]);
		}
		//////////////////////////////////////////////////////////////////////
        bool first_pass  = false;
        auto sgd_context = std::dynamic_pointer_cast<SGD_BackpropagationContext>(context);
        //test
        if(!sgd_context)
        {
            sgd_context = std::make_shared<SGD_BackpropagationContext>(m_layers.size());
            context     = sgd_context;
            first_pass  = true;
        }
        auto& deltas    = sgd_context->m_deltas;
        auto& gradients = sgd_context->m_gradients;
		//////////////////////////////////////////////////////////////////////
		#define S_LAST s_outputs.back()
		#define Z_LAST z_outputs.back()

		#define S(idx) s_outputs[idx]
		#define Z(idx) z_outputs[idx]
		#define W(idx) (*m_layers[idx])[0]
		#define D(idx) deltas[idx]
		#define G(idx) gradients[idx]

        #define L(idx) (*m_layers[idx])
        if(first_pass)
        {
            // BACK PROPAGATION (COMPUTE DELTA)
            deltas.back() = loss_function(Z_LAST, y).transpose();
            
            for (long i = m_layers.size() - 2; i>-1; --i)
                D(i) = L(i + 1).backpropagate_delta(D(i + 1), S(i));
            
            // BACK PROPAGATION (COMPUTE GRADIENT)
            for (long i = m_layers.size() - 1; i>0; --i)
                G(i) = L(i).backpropagate_gradient(D(i), Z(i-1), input_size, regular_param);
            
            //input layer
            G(0) = L(0).backpropagate_gradient(D(0), input, input_size, regular_param);
        }
        else
        {
            // BACK PROPAGATION (COMPUTE DELTA)
            deltas.back() += loss_function(Z_LAST, y).transpose();
            
            for (long i = m_layers.size() - 2; i>-1; --i)
                D(i) += L(i + 1).backpropagate_delta(D(i + 1), S(i));
            
            // BACK PROPAGATION (COMPUTE GRADIENT)
            for (long i = m_layers.size() - 1; i>0; --i)
                G(i) = G(i) + L(i).backpropagate_gradient(D(i), Z(i-1), input_size, regular_param);
            
            //input layer
            G(0) = G(0) + L(0).backpropagate_gradient(D(0), input, input.rows(), regular_param);
        }
		//[S]GD
		for (size_t i = 0; i < m_layers.size(); ++i)
		{
			for (size_t m = 0; m < m_layers[i]->size(); ++m)
			{
				L(i)[m] += (Scalar(-1.0) * learn_rate) * G(i)[m];
			}
		}
		//return
		return context;

	}
	/////////////////////////////////////////////////////////////////////////
	size_t NeuralNetwork::size() const
	{
		return m_layers.size();
	}

	Layer& NeuralNetwork::operator [] (size_t i)
	{
		return *(m_layers[i].get());
	}

	const Layer& NeuralNetwork::operator [] (size_t i) const
	{
		return *(m_layers[i].get());
	}

	NeuralNetwork::LayerIterator NeuralNetwork::begin()
	{
		return m_layers.begin();
	}

	NeuralNetwork::LayerIterator NeuralNetwork::end()
	{
		return m_layers.end();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::begin() const
	{
		return m_layers.begin();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::end() const
	{
		return m_layers.end();
	}
}