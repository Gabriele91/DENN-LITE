//
//  main.cpp
//  DENN
//
//  Created by Gabriele Di Bari on 14/11/17.
//  Copyright © 2017 Gabriele. All rights reserved.
//
#include <iostream>
#include <ctime>
#include <fstream>
#include <utility>
#include "Denn.h"
#include "DennDump.h"
#include "DennMainUtils.h"

namespace Denn
{
    using MatrixList = std::vector < Matrix >;
    using MMatrixList = std::vector < MatrixList >;
    using GateList = std::vector < Gate::SPtr >;

#if 0
    Matrix& softmax_nram(Matrix& inout_matrix)
    {
        auto block = inout_matrix.topRows(inout_matrix.rows()-1);
        block = Denn::CostFunction::softmax(block);
        for(Matrix::Index i = 0; i!= inout_matrix.cols(); ++i)
        {
            Scalar& value = inout_matrix(i,inout_matrix.rows()-1);
            value = Denn::PointFunction::sigmoid(value);
        }
        return inout_matrix;
    }
    REGISTERED_ACTIVE_FUNCTION("softmax_nram",softmax_nram)
#endif

    struct Task
    {
        Task(size_t m_batch_size, size_t m_max_int) : m_batch_size(m_batch_size), m_max_int(m_max_int) {}

        size_t m_batch_size;
        size_t m_max_int;

        std::tuple< MatrixList, Matrix > generate_batch()
        {
            Matrix in_mem(m_batch_size, m_max_int);
            // TODO: initialize init mem
            Matrix out_mem(m_batch_size, m_max_int);
            // TODO: Initialize out mem
            return {encode(in_mem), out_mem};
        }

        MatrixList encode(Matrix& M)
        {
            MatrixList encoded_mem(m_batch_size);
            for(size_t s = 0; s < M.rows(); s++)
            {
                Matrix sample = Matrix::Zero(m_max_int, m_max_int);
                for (size_t n = 0; n < M.cols(); n++)
                {
                    sample(n, Matrix::Index(M(s, n))) = 1;
                }
                encoded_mem.push_back(sample);
            }
            return encoded_mem;
        }
    };

    class NRam
    {
    public:

        NRam(  const size_t batch_size
             , const size_t max_int
             , const size_t n_regs
             , const size_t timesteps
             , const GateList& gates
             , const Denn::Parameters& parameters
             )
        : m_batch_size(batch_size)
        , m_max_int(max_int)
        , m_n_regs(n_regs)
        , m_timesteps(timesteps)
        , m_gates(gates)
        {
            // Past cardinality
            size_t i = 0;
            size_t output_colums = 0;
            for(auto& gate : gates)
            {
                output_colums += (n_regs + i++) * gate->type();
            }

            // Same size for every c_i
            for(size_t j = 0; j < n_regs; ++j)
            {
                output_colums += n_regs + i;
            }

            // Size for f_t
            output_colums += 1;

            m_network = build_mlp_network(m_n_regs, output_colums, parameters);
        }

        void execute(Random& random_engine)
        {
            // Init memory
            Task t(m_batch_size, m_max_int);
            auto mems = t.generate_batch();
            MatrixList in_mem = std::get<0>(mems);
            Matrix out_mem = std::get<1>(mems);

            //Init
            for (auto& layer  : m_network)
            for (auto& matrix : *layer)
            {
                matrix = matrix.unaryExpr([&](Scalar x)->Scalar{ return 1.0; });
                //matrix = matrix.unaryExpr([&](Scalar x)->Scalar{ return random_engine.uniform(); });
            }
            //init regs
            MatrixList fuzzy_regs(m_max_int, Matrix::Zero(m_batch_size, m_n_regs));
            //all to 0 (fuzzy)
            fuzzy_regs[0].fill(1);

            // TODO: While not converged
            Matrix step_cost = train(fuzzy_regs, in_mem, out_mem);
        }

        Matrix train(MatrixList& fuzzy_regs, MatrixList& in_mem, Matrix& out_mem)
        {
            //execute network
            std::vector< Scalar > p_t(m_batch_size, Scalar(1.0));             // Probability that execution is finished at timestep t
            std::vector< Scalar > prob_incomplete(m_batch_size, Scalar(1.0)); // Probability that the execution is not finished before timestep t
            std::vector< Scalar > cum_prob_complete(m_batch_size, Scalar(0.0)); // Comulative probability of p_i

            //time step
            Matrix cost = Matrix::Zero(1, m_batch_size);
            for(size_t timestep = 0; timestep < m_timesteps; timestep++)
            {
                // TODO: execute for the task timesteps
                Matrix out = m_network.apply(fuzzy_regs[0]);
                //..
                //applay to gates
                for(size_t s = 0; s != m_batch_size; ++s)
                {
                    Scalar fi = run_circuit(out, fuzzy_regs, in_mem, s);
                    //
                    if(timestep == m_timesteps-1)
                        p_t[s] =  1 - cum_prob_complete[s];
                    else
                        p_t[s] = fi * prob_incomplete[s];

                    cum_prob_complete[s] += p_t[s];
                    prob_incomplete[s] *= 1-fi;
                    //
                    cost(0, s) += p_t[s] * calculate_sample_cost(in_mem[s], out_mem.row(0));
                }
            }

            return cost;
        }

        Matrix fuzzy_reg_at(const std::vector< Matrix >& fuzzy_regs, size_t c)
        {
            Matrix out(m_n_regs, m_max_int);
            //
            for(size_t p = 0; p!=fuzzy_regs.size(); ++p)
            {
                //
                for(size_t r = 0; r!=m_n_regs; ++r)
                {
                    out(r, p) = fuzzy_regs[p](c, r);
                }
            }
            return out;
        }

        MatrixList& set_fuzzy_reg_at(MatrixList& fuzzy_regs, const Matrix& new_reg, size_t idx_reg, size_t c)
        {
            for (size_t idx_value = 0; idx_value < fuzzy_regs.size(); ++idx_value)
                fuzzy_regs[idx_value](c, idx_reg) = new_reg(0, idx_reg);
            return fuzzy_regs;
        }

        Scalar calculate_sample_cost(Matrix& M, const RowVector& desired_mem)
        {
            Scalar s_cost = 0;
            for (size_t idx = 0; idx < M.rows(); ++idx)
            {
                s_cost += Denn::CostFunction::safe_log(M(idx, desired_mem(idx)));
            }
            return s_cost;
        }

        Matrix avg(const Matrix& regs, const Matrix& in)
        {
            return (regs.transpose() * in.transpose()).transpose();
        }

        Scalar run_circuit(Matrix& out, MatrixList& fuzzy_regs, MatrixList& in_mem,  size_t s)
        {
            //get regs
            Matrix regs = fuzzy_reg_at(fuzzy_regs, s);
            //start col
            size_t ptr_col = 0;
            // Execute circuit
            for (size_t i, pos = 0; i != m_gates.size(); ++i)
            {
                auto &gate = *m_gates[i];
                switch (gate.type())
                {
                    case Gate::CONST:
                    {
                        auto C = gate( in_mem[s] );
                        regs.conservativeResize(regs.rows()+1, regs.cols());
                        regs.row(regs.rows()-1) = C;
                    }
                        break;
                    case Gate::UNARY:
                    {
                        Matrix a = out.block(s,ptr_col, 1, m_n_regs + i);
                        Matrix A = Denn::CostFunction::softmax(a);
                        ptr_col += m_n_regs + i;

                        auto C = gate(avg(regs, A), in_mem[s]);
                        regs.conservativeResize(regs.rows()+1, regs.cols());
                        regs.row(regs.rows()-1) = C;
                    }
                        break;
                    case Gate::BINARY:
                    {
                        Matrix a = out.block(s,ptr_col, 1, m_n_regs + i);
                        Matrix A = Denn::CostFunction::softmax(a);
                        ptr_col += m_n_regs + i;

                        Matrix b = out.block(s,ptr_col, 1, m_n_regs + i);
                        Matrix B = Denn::CostFunction::softmax(b);
                        ptr_col += m_n_regs + i;

                        auto C = gate(avg(regs, A), avg(regs, B), in_mem[s]);
                        //append output
                        regs.conservativeResize(regs.rows()+1, regs.cols());
                        regs.row(regs.rows()-1) = C;
                    }
                        break;
                    default:  break;
                }
            }
            // Update regs after circuit execution
            for(size_t idx_reg = 0; idx_reg < m_n_regs; idx_reg++)
            {
                auto   c = out.block(s, ptr_col, 1, m_n_regs + idx_reg);
                Matrix C = Denn::CostFunction::softmax(c);
                ptr_col += m_n_regs + idx_reg;
                set_fuzzy_reg_at(fuzzy_regs, avg(regs, C), idx_reg, s);
            }

            //return fi
            Scalar fi = Denn::PointFunction::sigmoid(out.col(out.cols()-1)(s));
            return fi;
        }

    protected:

        //build a mlp network from parameters
        NeuralNetwork build_mlp_network
        (
              size_t n_features
            , size_t n_classes
            , const Denn::Parameters& parameters
        )
        {
            //mlp network
            NeuralNetwork mlp_nn;
            //hidden layer list
            const auto& hidden_layers = (*parameters.m_hidden_layers);
            //push all hidden layers
            if (hidden_layers.size())
            {
                //add first layer
                mlp_nn.add_layer(PerceptronLayer(
                          ActiveFunctionFactory::get("relu")
                        , n_features
                        , hidden_layers[0]
                ));
                //add next layers
                for (size_t i = 0; i != hidden_layers.size() - 1; ++i)
                {
                    mlp_nn.add_layer(PerceptronLayer(
                              ActiveFunctionFactory::get("relu")
                            , hidden_layers[i]
                            , hidden_layers[i + 1]
                    ));
                }

                //add last layer
                mlp_nn.add_layer(PerceptronLayer(
                        ActiveFunctionFactory::get("linear")
                        , hidden_layers[hidden_layers.size() - 1]
                        , n_classes
                ));
            }
            //else add only input layer
            else
            {
                //add last layer
                mlp_nn.add_layer(PerceptronLayer(
                        ActiveFunctionFactory::get("linear")
                        , n_features
                        , n_classes
                ));
            }
            //return NeuralNetwork
            return mlp_nn;
        }

        size_t        m_batch_size;
        size_t        m_max_int;
        size_t        m_n_regs;
        size_t        m_timesteps;
        GateList      m_gates;
        NeuralNetwork m_network;
    };
}
int main(int argc, const char** argv)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Denn;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Parameters arguments(argc, argv);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    NRam test(
        1, // Samples
        10, // Max int
        4, // Registers
        1, // Timesteps
        {  /*GateFactory::create("read"),   GateFactory::create("two"), */ GateFactory::create("read")},
        arguments
    );
    // Execute network
    Random random_engine /*(16112017)*/;
    test.execute(random_engine);
    return 0;
}
