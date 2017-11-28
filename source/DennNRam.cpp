//
// Created by Valerio Belli on 20/11/17.
//

#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"

namespace Denn
{
namespace NRam
{

    NRamLayout::NRamLayout()
    {
    }

    void NRamLayout::init
    (
        const size_t batch_size,
        const size_t max_int,
        const size_t n_regs,
        const size_t timesteps,
        const GateList& gates
    )
    {
        // values init
	    m_batch_size = batch_size;
        m_max_int = max_int;
        m_n_regs = n_regs;
        m_timesteps = timesteps;
	    m_gates = gates;
        // Past cardinality
        size_t i = 0;
        m_nn_output = 0;
        for (auto &gate : m_gates) 
		{
            m_nn_output += (n_regs + i++) * gate->arity();
        }

        // Same size for every c_i
        for (size_t j = 0; j < n_regs; ++j) 
		{
            m_nn_output += n_regs + i;
        }

        // Size for f_t
        m_nn_output += 1;
    }

    Matrix fuzzy_encode(const Matrix& M)
    {
        Matrix sample = Matrix::Zero(M.cols(), M.cols());
        for (Matrix::Index n = 0; n < M.cols(); n++) 
        {
            sample(n, Matrix::Index(M(0,n))) = Scalar(1.0);
        }
        return sample;
    }

    Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem)
    {
        Scalar s_cost = 0;
        for (size_t idx = 0; idx < M.rows(); ++idx)
        {
            s_cost += Denn::CostFunction::safe_log(M(idx, Matrix::Index(desired_mem(idx))));
        }
        return s_cost;
    }

    static Matrix avg(const Matrix& regs, const Matrix& in)
    {
        return regs.transpose() * in;
    }

    Scalar run_circuit(const NRamLayout& context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem)
    {
        //start col
        size_t ptr_col = 0, coefficient_size = context.m_n_regs;
        // Execute circuit
        size_t i = 0;
        for (; i != context.m_gates.size(); ++i, ++coefficient_size)
        {
            auto &gate = *context.m_gates[i];
            switch (gate.arity())
            {
                case Gate::CONST:
                {
                    Matrix C = gate(in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::UNARY:
                {
                    ColVector a = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix A = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    Matrix C = gate(avg(regs, A), in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::BINARY:
                {
                    ColVector a = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix A = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    ColVector b = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix B = CostFunction::softmax_col(b);
                    ptr_col += coefficient_size;

                    Matrix C = gate(avg(regs, A), avg(regs, B), in_mem);
                    //append output
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                default:
                break;
            }
        }

        // Update regs after circuit execution
        for (size_t r = 0; i < context.m_gates.size() + context.m_n_regs; ++i, ++r)
        {
			// get row
            ColVector c = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
			// softmax
            Matrix C = CostFunction::softmax_col(c);
			// update
			regs.row(r) = avg(regs, C).transpose();
			// next
			ptr_col += coefficient_size;
        }
		//resize regs
		regs.conservativeResize(context.m_n_regs, regs.cols());
        //return fi
        return PointFunction::sigmoid(nn_out_decision.col(nn_out_decision.cols() - 1)(0));
    }

    Scalar train
    (
      const NRamLayout &context
    , const NeuralNetwork& network
    , const Matrix& linear_in_mem
    , const Matrix& linear_out_mem
    )
    {		
        //time step
        Scalar full_cost = Scalar(0.0);
        // Run the circuit
        for (Matrix::Index s = 0; s < linear_in_mem.rows(); ++s)
        {
            //Alloc regs
            Matrix regs = Matrix::Zero(context.m_n_regs,context.m_max_int); 
            //P(X=0)
            regs.col(0).fill(1);
            //In mem fazzy
            Matrix in_mem = fuzzy_encode(linear_in_mem.row(s));
            //Cost helper
            Scalar p_t = Scalar(1.0);
            Scalar prob_incomplete = Scalar(1.0);
            Scalar cum_prob_complete = Scalar(0.0);
            Scalar sample_cost = Scalar(0.0);
            //for all timestep, run on s
            for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
            {
                //execute nn
                Matrix out = network.apply(regs.col(0).transpose()).transpose();
                //execute circuit
                Scalar fi = run_circuit(context, out, regs, in_mem);
                //compute exit state
                if (timestep == context.m_timesteps - 1)
                    p_t = 1 - cum_prob_complete;
                else
                    p_t = fi * prob_incomplete;
                //
                cum_prob_complete += p_t;
                prob_incomplete *= 1 - fi;
                //compute cost
                sample_cost -= p_t * calculate_sample_cost(in_mem, linear_out_mem.row(s));
            }
            //add to full "cost"
            full_cost += sample_cost;
        }

        return full_cost;
    }
}
}