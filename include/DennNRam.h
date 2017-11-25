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

    struct NRamLayout
    {
        NRamLayout();
        
		NRamLayout
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

    Matrix fuzzy_reg_at(const NRamLayout& context, const MatrixList& fuzzy_regs, size_t c);

    /**
     * Set the new value of a register for a sample.
     *
     * @param fuzzy_regs
     * @param new_reg Register new distribution
     * @param idx_reg
     * @param sample
     * @return
     */
    MatrixList& set_fuzzy_reg_at(MatrixList& fuzzy_regs, const Matrix& new_reg, size_t idx_reg, size_t sample);

    Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem);

    Matrix avg(const Matrix &regs, const Matrix &in);

    MatrixT<size_t> defuzzy_mem(const Matrix &M);

    Scalar run_circuit(NRamLayout &context, Matrix &nn_out_decision, MatrixList &fuzzy_regs, MatrixList &in_mem,
                       size_t s);

    Scalar train(NRamLayout &context, const NeuralNetwork &nn, MatrixList &fuzzy_regs, MatrixList &in_mem, const Matrix &out_mem);
}
}