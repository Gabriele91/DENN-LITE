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

        enum RegisterExtaction
        {
            P_ZERO      = 0,
			P_DEFUZZYED = 1
        };

        NRamLayout();

        void init
        (
            const size_t batch_size,
			const size_t max_int,
			const size_t n_regs,
			const size_t timesteps,
            const size_t registers_values_extraction_type,
            const Scalar entropy_term,
            const Scalar entropy_decay,
            const Scalar cost_regularization_term,
			const GateList& gates
        );

        size_t      m_batch_size;
        size_t      m_max_int;
        size_t      m_n_regs;
        size_t      m_timesteps;
        size_t      m_nn_output;
        size_t      m_registers_values_extraction_type;
        Scalar      m_entropy_term;
        Scalar      m_entropy_decay;
        Scalar      m_cost_regularization_term;
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
        
        ExecutionDebug(const NRamLayout& layout);
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
        //layout
        NRamLayout m_layout;
    };

    using ListExecutionDebug      = std::vector< ExecutionDebug >;
    using ResultAndExecutionDebug = std::tuple<Matrix, ListExecutionDebug >;

	//Evalutation of nram
	class NRamEval : public Evaluation
	{
	public:
		//methods
		virtual bool minimize() const;
		virtual Scalar operator () (const Individual& individual, const DataSet& ds);
		//layout
		const NRam::NRamLayout* m_context{ nullptr };
		//set context
		Evaluation::SPtr set_context(const NRamLayout& context);
	};

    Matrix fuzzy_encode(const Matrix& M);

    Matrix defuzzy_mem(const Matrix &M);

    Matrix defuzzy_mem_cols(const Matrix &M);

    Matrix get_registers_values(const Matrix &M, size_t extraction_type);

    Scalar calculate_sample_cost(const Matrix& M, const RowVector& desired_mem,  const Matrix& mask);

    Scalar calculate_error_rate(const NRamLayout& context, const NeuralNetwork& network, const Matrix& linear_test_in_mem, const Matrix& linear_test_desired_mem, const Matrix& linear_mask, const size_t& max_int, const size_t& timesteps);

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem);

    void run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem, ExecutionDebug& debug);

    Scalar train(const NRamLayout &context, const NeuralNetwork &nn,  const Matrix& in_mem, const Matrix &out_mem, const Matrix& mask, const size_t& max_int, const size_t& timesteps);

    ResultAndExecutionDebug execute(const NRamLayout &context, const NeuralNetwork& network, const Matrix& linear_in_mem);

}
}