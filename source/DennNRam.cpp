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

    NRamLayout::NRamLayout(
        const size_t            batch_size,
        const size_t            max_int,
        const size_t            n_regs,
        const size_t            timesteps,
        const Denn::Parameters  &parameters
    ) : m_batch_size(batch_size),
        m_max_int(max_int),
        m_n_regs(n_regs),
        m_timesteps(timesteps)
    {
        m_task = new TaskCopy(m_batch_size, m_max_int, m_n_regs);

        m_gates = {
                GateFactory::create("read"),
                GateFactory::create("inc"),
                GateFactory::create("add"),
                GateFactory::create("dec"),
                GateFactory::create("min"),
                GateFactory::create("write")
        };

        // Past cardinality
        size_t i = 0;
        m_nn_output = 0;
        for (auto &gate : m_gates) {
            m_nn_output += (n_regs + i++) * gate->arity();
        }

        // Same size for every c_i
        for (size_t j = 0; j < n_regs; ++j) {
            m_nn_output += n_regs + i;
        }

        // Size for f_t
        m_nn_output += 1;

    }

    MatrixList fuzzy_encode(const Matrix &M) {
        MatrixList encoded_mem;
        for (size_t s = 0; s < M.rows(); s++) {
            Matrix sample = Matrix::Zero(M.cols(), M.cols());
            for (size_t n = 0; n < M.cols(); n++) {
                sample(n, MatrixT<int>::Index(M(s, n))) = Scalar(1.0);
            }
            encoded_mem.push_back(sample);
        }

        return encoded_mem;
    }

    MatrixList fuzzy_regs(const size_t m_batch_size, const size_t m_n_regs, const size_t m_max_int)
    {
        // Generate fuzzy registers and set the P(x = 0) = 1.0
        MatrixList fuzzy_regs(m_max_int, Matrix::Zero(m_batch_size, m_n_regs));
        fuzzy_regs[0].fill(1);

        return fuzzy_regs;
    }

    Matrix fuzzy_reg_at(const NRamLayout& context, const MatrixList& fuzzy_regs, size_t c)
    {
        Matrix out(context.m_n_regs, context.m_max_int);
        // Iterate through numbers of distribution
        for(size_t p = 0; p!=fuzzy_regs.size(); ++p)
        {
            // Iterate through registers
            for(size_t r = 0; r!=context.m_n_regs; ++r)
            {
                out(r, p) = fuzzy_regs[p](c, r);
            }
        }
        return out;
    }

    /**
     * Set the new value of a register for a sample.
     *
     * @param fuzzy_regs
     * @param new_reg Register new distribution
     * @param idx_reg
     * @param sample
     * @return
     */
    MatrixList& set_fuzzy_reg_at(MatrixList& fuzzy_regs, const Matrix& new_reg, size_t idx_reg, size_t sample)
    {
        for (size_t idx_value = 0; idx_value < fuzzy_regs.size(); ++idx_value)
        {
            fuzzy_regs[idx_value](sample, idx_reg) = new_reg(0, idx_value);
        }
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

    Matrix avg(const Matrix &regs, const Matrix &in)
    {
        return in * regs;
    }

    MatrixT<size_t> defuzzy_mem(const Matrix &M)
    {
        MatrixT<size_t> m(1, M.rows());
        for (size_t r = 0; r < M.rows(); r++)
        {
            Scalar max_value = Scalar(0.0);
            size_t max_index = 0;
            for (size_t c = 0; c < M.cols(); ++c)
            {
                if (max_value < M(r, c))
                {
                    max_value = M(r, c);
                    max_index = c;
                }
            }
            m(0, r) = max_index;
        }

        return m;
    }

    Scalar run_circuit(NRamLayout &context, Matrix &nn_out_decision, MatrixList &fuzzy_regs, MatrixList &in_mem, size_t s)
    {
        //get regs
        Matrix regs = fuzzy_reg_at(context, fuzzy_regs, s);
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
                    auto C = gate(in_mem[s]);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                    break;
                case Gate::UNARY:
                {
                    Matrix a = nn_out_decision.block(s, ptr_col, 1, coefficient_size);
                    Matrix A = CostFunction::softmax(a);
                    ptr_col += coefficient_size;

                    auto C = gate(avg(regs, A), in_mem[s]);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                    break;
                case Gate::BINARY:
                {
                    Matrix a = nn_out_decision.block(s, ptr_col, 1, coefficient_size);
                    Matrix A = CostFunction::softmax(a);
                    ptr_col += coefficient_size;

                    Matrix b = nn_out_decision.block(s, ptr_col, 1, coefficient_size);
                    Matrix B = CostFunction::softmax(b);
                    ptr_col += coefficient_size;

                    Matrix C = gate(avg(regs, A), avg(regs, B), in_mem[s]);
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
            auto c = nn_out_decision.block(s, ptr_col, 1, coefficient_size);
            Matrix C = CostFunction::softmax(c);
            ptr_col += coefficient_size;
            set_fuzzy_reg_at(fuzzy_regs, avg(regs, C), i - context.m_gates.size(), s);
        }

        //return fi
        return PointFunction::sigmoid(nn_out_decision.col(nn_out_decision.cols() - 1)(s));
    }

    Scalar train(
            NRamLayout &context, const NeuralNetwork &nn, MatrixList &fuzzy_regs, MatrixList &in_mem, const Matrix &out_mem
    )
    {
        //execute network
        std::vector<Scalar> p_t(in_mem.size(),
                                Scalar(1.0));               // Probability that execution is finished at timestep t
        std::vector<Scalar> prob_incomplete(in_mem.size(),
                                            Scalar(1.0));   // Probability that the execution is not finished before timestep t
        std::vector<Scalar> cum_prob_complete(in_mem.size(), Scalar(0.0)); // Cumulative probability of p_i

        //time step
        Matrix cost = Matrix::Zero(1, in_mem.size());
        for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
        {
            Matrix out = nn.apply(fuzzy_regs[0]);

            // Run the circuit
            for (size_t s = 0; s != in_mem.size(); ++s)
            {
                Scalar fi = run_circuit(context, out, fuzzy_regs, in_mem, s);

                if (timestep == context.m_timesteps - 1)
                    p_t[s] = 1 - cum_prob_complete[s];
                else
                    p_t[s] = fi * prob_incomplete[s];

                cum_prob_complete[s] += p_t[s];
                prob_incomplete[s] *= 1 - fi;
                //
                cost(0, s) -= p_t[s] * calculate_sample_cost(in_mem[s], out_mem.row(0));
            }
        }

        return cost.sum();
    }
}
}