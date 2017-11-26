#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"

namespace Denn
{
namespace NRam
{
	class TaskAccess : public Task
	{
	public:
		TaskAccess
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs, Random& random
		)
		: Task(batch_size, max_int, n_regs, random)
		{
		}

		MemoryTuple operator()() override
		{
			//alloc
			Matrix::Index offset(m_max_int / 2);
			Matrix in_mem(m_batch_size, m_max_int);
			//init
			in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform() * m_max_int); });
			in_mem.col(m_max_int - 1) = RowVector::Zero(in_mem.rows());
			in_mem.col(0) = ColVector::Ones(in_mem.rows()) * 2;
			//out mem
			Matrix out_mem = in_mem;
			for (size_t r = 0; r < out_mem.rows(); ++r)  out_mem(r, 0) = out_mem(r, Matrix::Index(out_mem(r, 0)));
			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		};
	}; 
	REGISTERED_TASK(TaskAccess,"access")

	class TaskCopy : public Task
	{
	public:
		TaskCopy
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, Random& random
		)
		: Task(batch_size, max_int, n_regs, random)
		{
		}

		MemoryTuple operator()() override
		{
			//alloc
			Matrix::Index offset(m_max_int / 2);
			Matrix in_mem(m_batch_size, m_max_int);
			//init in mem
			in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform() * m_max_int); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			in_mem.col(0) = ColVector::Ones(in_mem.rows()) * offset;
			//out mem
			Matrix out_mem = in_mem;
			out_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = in_mem.block(0, 1, in_mem.rows(), offset);
			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskCopy, "copy")

	class TaskIncrement : public Task
	{
	public:
		TaskIncrement
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, Random& random
		)
		: Task(batch_size, max_int, n_regs, random)
		{
		}

		MemoryTuple operator()() override
		{
			//alloc
			Matrix::Index offset(m_max_int / 2);
			Matrix in_mem(m_batch_size, m_max_int);
			//init in mem
			in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform() * m_max_int); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			//init out mem
			Matrix out_mem = in_mem;
			for (size_t c = 0; c < m_max_int / 2; c++)  out_mem.col(c) += ColVector::Ones(out_mem.rows());
			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskIncrement, "increment")
}
}