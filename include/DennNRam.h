//
// Created by Valerio Belli on 20/11/17.
//
#pragma once
#include "Config.h"
#include "Denn.h"
#include "DennDump.h"
#include "DennNRamGate.h"
#include "DennNRamTask.h"

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

    using ConnectionValue             = std::map< std::string, int >;
    using ConnectionsHistory          = std::map< std::string, ConnectionValue >;
    using ListConnectionsHistory      = std::vector< ConnectionsHistory >;
    using ListListConnectionsHistory  = std::vector< ListConnectionsHistory >;
    using ResultAndConnectionsHistory = std::tuple<Matrix, ListListConnectionsHistory>;

    Matrix fuzzy_encode(const Matrix& M);

    Matrix defuzzy_mem(const Matrix &M);

    Matrix defuzzy_mem_cols(const Matrix &M);

    std::string register_or_gate(const NRamLayout& context, Scalar idx);

    void print_sample_execution(const NRamLayout& context, const ListConnectionsHistory& connections, std::ostream& output);

    Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem);

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem);

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem, ConnectionsHistory& history);

    Scalar train(const NRamLayout &context, const NeuralNetwork &nn,  const Matrix& in_mem, const Matrix &out_mem);

    ResultAndConnectionsHistory execute(const NRamLayout &context, const NeuralNetwork& network, const Matrix& linear_in_mem);

}
}