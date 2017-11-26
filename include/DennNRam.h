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

    MatrixList fuzzy_encode(const Matrix& M);

    MatrixList fuzzy_regs(const size_t batch_size, const size_t n_regs, const size_t max_int);
	
	Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem);

    Scalar run_circuit(const NRamLayout &context, Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem);

    Scalar train(const NRamLayout &context, const NeuralNetwork &nn,  const Matrix& in_mem, const Matrix &out_mem);
}
}