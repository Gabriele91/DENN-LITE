//
// Created by Valerio Belli on 20/11/17.
//
#pragma once
#include "Config.h"
#include "Denn.h"
#include "DennDump.h"
#include "DennNRamGate.h"
#include "DennNRamTask.h"
#include "DennJson.h"

namespace Denn
{
namespace NRam
{

    class NRamLayout
    {
    public:

        NRamLayout();

        void init
        (
            const size_t batch_size,
			const size_t max_int,
			const size_t n_regs,
			const size_t timesteps,
			const GateList& gates
        );

        size_t      m_batch_size;
        size_t      m_max_int;
        size_t      m_n_regs;
        size_t      m_timesteps;
        size_t      m_nn_output;
        GateList    m_gates;
    };


    class ExecutionDebug
    {
    public:
        //values
        using Values     = std::vector< Matrix >;
        using OpNode     = std::tuple < std::string, Values >;
        using Operations = std::vector< OpNode >;
        using UpNode     = std::tuple < size_t, Values >;
        using Updates    = std::vector < UpNode >;
        //structs
        struct Step  
        {
            Operations  m_ops;
            Updates     m_ups;
            void push_op(const OpNode& op){ m_ops.push_back(std::move(op)); }
            void push_up(const UpNode& up){ m_ups.push_back(std::move(up)); }
        };
        using Steps  = std::vector< Step >;
        
        ExecutionDebug();
        ExecutionDebug(const ExecutionDebug& debug);

        void push_step();
        void push_op(const Gate& gate, const Matrix& m, const Matrix& out);
        void push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& m, const Matrix& out);
        void push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& b, const Matrix& in_b, const Matrix& m, const Matrix& out);
        void push_update(size_t r, const Matrix& c, const Matrix& in_c);
        
        std::string shell() const;
        Json json() const;


    protected:
        //save all step
        Steps m_steps;
        //help
        static Gate::Arity get_arity(size_t value_size);

    };
    using ListExecutionDebug      = std::vector< ExecutionDebug >;
    using ResultAndExecutionDebug = std::tuple<Matrix, ListExecutionDebug >;


    Matrix fuzzy_encode(const Matrix& M);

    Matrix defuzzy_mem(const Matrix &M);

    Matrix defuzzy_mem_cols(const Matrix &M);

    std::string register_or_gate(const NRamLayout& context, Scalar idx);

    Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem);

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem);

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem, ExecutionDebug& debug);

    Scalar train(const NRamLayout &context, const NeuralNetwork &nn,  const Matrix& in_mem, const Matrix &out_mem);

    ResultAndExecutionDebug execute(const NRamLayout &context, const NeuralNetwork& network, const Matrix& linear_in_mem);

}
}