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

    MatrixList fuzzy_encode(const Matrix &M) {
        MatrixList encoded_mem;
        for (size_t s = 0; s < M.rows(); s++) 
		{
            Matrix sample = Matrix::Zero(M.cols(), M.cols());
            for (size_t n = 0; n < M.cols(); n++) 
			{
                sample(n, MatrixT<int>::Index(M(s, n))) = Scalar(1.0);
            }
            encoded_mem.push_back(sample);
        }
        return encoded_mem;
    }

    MatrixList fuzzy_regs(const size_t m_batch_size, const size_t m_n_regs, const size_t m_max_int)
    {
        // Generate list of fuzzy registers
        MatrixList fuzzy_regs(m_batch_size, Matrix::Zero(m_n_regs, m_max_int));
		// Set the P(x = 0) = 1.0
		for (auto& mem : fuzzy_regs) mem.col(0).fill(1);
		// return
		return fuzzy_regs;
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

    static Matrix avg(const Matrix &regs, const Matrix &in)
    {
        return in * regs;
    }

    Scalar run_circuit(const NRamLayout &context, Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem)
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
                    auto C = gate(in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::UNARY:
                {
                    Matrix a = nn_out_decision.block(0, ptr_col, 1, coefficient_size);
                    Matrix A = CostFunction::softmax(a);
                    ptr_col += coefficient_size;

                    auto C = gate(avg(regs, A), in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::BINARY:
                {
                    Matrix a = nn_out_decision.block(0, ptr_col, 1, coefficient_size);
                    Matrix A = CostFunction::softmax(a);
                    ptr_col += coefficient_size;

                    Matrix b = nn_out_decision.block(0, ptr_col, 1, coefficient_size);
                    Matrix B = CostFunction::softmax(b);
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
        for (; i < context.m_gates.size() + context.m_n_regs; ++i)
        {
			// get row
            auto   c   = nn_out_decision.block(0, ptr_col, 1, coefficient_size);
			// softmax
            Matrix C   = CostFunction::softmax(c);
			// update
			regs.row(i - context.m_gates.size()) = avg(regs, C);
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
		//regs													
		MatrixList regs = fuzzy_regs(context.m_batch_size, context.m_n_regs, context.m_max_int);
		//to list
		MatrixList in_mem = NRam::fuzzy_encode(linear_in_mem);
        //execute network
        std::vector<Scalar> p_t(in_mem.size(),Scalar(1.0));                // Probability that execution is finished at timestep t
        std::vector<Scalar> prob_incomplete(in_mem.size(), Scalar(1.0));   // Probability that the execution is not finished before timestep t
        std::vector<Scalar> cum_prob_complete(in_mem.size(), Scalar(0.0)); // Cumulative probability of p_i
		//tanspose
		#define MEMORY_IS_TRANSPOSE
        //time step
        Matrix cost = Matrix::Zero(1, in_mem.size());
        for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
        {
            // Run the circuit
            for (size_t s = 0; s != in_mem.size(); ++s)
            {
				//execute nn
				Matrix out = network.apply(regs[s].col(0).transpose());
//fix?
#ifdef MEMORY_IS_TRANSPOSE
				//transpose
				Matrix t_in_mem = in_mem[s].transpose();
				//execute circuit
				Scalar fi = run_circuit(context, out, regs[s], t_in_mem);
#else
				//execute circuit
				Scalar fi = run_circuit(context, out, regs[s], in_mem[s]);
#endif
				//compute exit state
                if (timestep == context.m_timesteps - 1)
                    p_t[s] = 1 - cum_prob_complete[s];
                else
                    p_t[s] = fi * prob_incomplete[s];
				//
                cum_prob_complete[s] += p_t[s];
                prob_incomplete[s] *= 1 - fi;
//fix?
#ifdef MEMORY_IS_TRANSPOSE
				//transpose
				in_mem[s] = t_in_mem.transpose();
				//compute cost
				cost(0, s) -= p_t[s] * calculate_sample_cost(in_mem[s], linear_out_mem.row(s));
#else
				//compute cost
				cost(0, s) -= p_t[s] * calculate_sample_cost(in_mem[s], linear_out_mem.row(s));
#endif
            }
        }

        return cost.sum();
    }
}
}