//
// Created by Valerio Belli on 22/11/17.
//
#pragma once
#include "Config.h"

namespace Denn
{
    using MemoryTuple = std::tuple<Matrix, Matrix, Matrix>;

namespace NRam {
    class Task : std::enable_shared_from_this< Task > {
    public:
        Task(size_t batch_size, size_t max_int, size_t n_regs) : m_batch_size(batch_size),
                                                                       m_max_int(max_int),
                                                                       m_n_regs(n_regs) {}

        virtual MemoryTuple operator()() { return {}; }

        /**
         * Initialize R registers to zero (i.e. set P(x = 0) = 1.0).
         *
         * @param R Registers number
         * @return MatrixList
         */
        Matrix init_regs() const { return Matrix::Zero(m_batch_size, m_n_regs); }

        size_t get_batch_size() const { return m_batch_size; }
        size_t get_max_int()    const { return m_max_int; }
        size_t get_num_regs()   const { return m_n_regs; }
    protected:

        size_t m_batch_size;
        size_t m_max_int;
        size_t m_n_regs;
    };

    class TaskAccess : public Task {
    public:
        TaskAccess(size_t batch_size, size_t max_int, size_t n_regs) : Task(batch_size, max_int, n_regs) {}

        MemoryTuple operator()() override
        {
            std::srand((unsigned int) time(0));

            Matrix in_mem = ((Matrix::Random(m_batch_size, m_max_int) +
                              Matrix::Ones(m_batch_size, m_max_int)) * (m_max_int / 2))
                    .unaryExpr((Scalar(*)(Scalar)) std::floor);
            in_mem.col(m_max_int - 1) = RowVector::Zero(in_mem.rows());
            in_mem.col(0) = RowVector::Ones(in_mem.rows()) * 2;

            Matrix out_mem = in_mem;
            for (size_t r = 0; r < out_mem.rows(); ++r)
                out_mem(r, 0) = out_mem(r, Matrix::Index(out_mem(r, 0)));

            return {in_mem, out_mem, Task::init_regs()};
        };
    };


    class TaskCopy : public Task {
    public:
        TaskCopy(size_t batch_size, size_t max_int, size_t n_regs) : Task(batch_size, max_int, n_regs) {}

        MemoryTuple operator()() override
        {
            std::srand((unsigned int) time(0));
            Matrix in_mem = ((Matrix::Random(m_batch_size, m_max_int) +
                              Matrix::Ones(m_batch_size, m_max_int)) * (m_max_int / 2))
                    .unaryExpr((Scalar(*)(Scalar)) std::floor);
            in_mem.block(0, m_max_int / 2, in_mem.rows(), in_mem.cols() - m_max_int / 2) =
                    Matrix::Zero(m_batch_size, m_max_int / 2);
            in_mem.col(0) = RowVector::Ones(in_mem.rows()) * Matrix::Index(m_max_int / 2);

            Matrix out_mem = in_mem;
            out_mem.block(0, m_max_int / 2, in_mem.rows(), in_mem.cols() - m_max_int / 2)
                    = in_mem.block(0, 1, in_mem.rows(), m_max_int / 2);
            return {in_mem, out_mem, Task::init_regs()};
        }
    };


    class TaskIncrement : Task {
    public:
        TaskIncrement(size_t batch_size, size_t max_int, size_t n_regs) : Task(batch_size, max_int, n_regs) {}

        MemoryTuple operator()() override
        {
            std::srand((unsigned int) time(0));

            Matrix in_mem = ((Matrix::Random(m_batch_size, m_max_int) +
                              Matrix::Ones(m_batch_size, m_max_int)) * (m_max_int / 2))
                    .unaryExpr((Scalar(*)(Scalar)) std::floor);
            in_mem.block(0, Matrix::Index(m_max_int - m_max_int / 2), in_mem.rows(), Matrix::Index(m_max_int / 2)) =
                    Matrix::Zero(m_batch_size, m_max_int - m_max_int / 2);

            Matrix out_mem = in_mem;
            for (size_t c = 0; c < m_max_int / 2; c++)
                out_mem.col(c) += ColVector::Ones(out_mem.rows());

            return {in_mem, out_mem, Task::init_regs()};
        }
    };
}
}