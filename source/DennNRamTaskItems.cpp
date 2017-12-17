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
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform() * m_max_int); });
			in_mem.col(m_max_int - 1) = ColVector::Zero(in_mem.rows());
			in_mem.col(0)             = ColVector::Ones(in_mem.rows()) * 2;
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
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform() * m_max_int); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			in_mem.col(0) = ColVector::Ones(in_mem.rows()) * offset;
			//out mem
			Matrix out_mem = in_mem;
			out_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset - 1) = in_mem.block(0, 1, in_mem.rows(), offset - 1);
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
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(m_max_int - 1)); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			//init out mem
			Matrix out_mem = in_mem;
			for (size_t c = 0; c < m_max_int / 2; c++)  out_mem.col(c) += ColVector::Ones(out_mem.rows());
			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskIncrement, "increment")

	class TaskSwap : public Task
	{
	public:
		TaskSwap
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
			Matrix::Index idx_1(m_max_int / 3);
			Matrix::Index idx_2((m_max_int / 2) + 1);
			Matrix in_mem(m_batch_size, m_max_int);
			//init in mem
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(m_max_int - 1)); });

            // Set pointers
            in_mem.col(0) = ColVector::Ones(in_mem.rows()) * idx_1;
            in_mem.col(1) = ColVector::Ones(in_mem.rows()) * idx_2;
            in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows());

			//init out mem
			Matrix out_mem = in_mem;

            // Swap
            ColVector col_idx_1 = out_mem.col(idx_1);
            out_mem.col(idx_1) = out_mem.col(idx_2);
            out_mem.col(idx_2) = col_idx_1;

            // Cut out the the memory parts that does not make part of the expected output
            out_mem.block(0, 0, out_mem.rows(), 2) = Matrix::Ones(out_mem.rows(), 2) * -1;
            out_mem.col(in_mem.cols() - 1) = ColVector::Ones(in_mem.rows()) * -1;

			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskSwap, "swap")

	class TaskReverse : public Task
	{
	public:
		TaskReverse
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
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(m_max_int - 1)); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			//init out mem
			Matrix out_mem = in_mem;
            out_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset - 1) \
                    = out_mem.block(0, 1, in_mem.rows(), offset - 1).rowwise().reverse();

            // Cut out from the cost calculation the memory part that does not make part of the expected output
            out_mem.block(0, 0, in_mem.rows(), offset) = Matrix::Ones(out_mem.rows(), offset) * -1;

            //return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskReverse, "reverse")

    class TaskPermutation : public Task
    {
    public:
        TaskPermutation
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
            using namespace Eigen;

            //alloc
            Matrix::Index offset(m_max_int / 2);
            Matrix in_mem(m_batch_size, m_max_int);

            //init in mem
            in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(m_max_int - 1)); });

            // Initialize the permutation for all the examples
            for (size_t r = 0; r < in_mem.rows(); ++r)
            {
                PermutationMatrix< Dynamic, Dynamic > perm(offset - 1);
                perm.setIdentity();

				std::random_device rd;
				std::mt19937 g(rd());

				std::shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size(), g);
                in_mem.block(r, 1, 1, offset - 1) = perm
                        .indices()
                        .cast<Scalar>()
                        .transpose()
                        .row(0);
            }
            in_mem.col(0) = ColVector::Ones(in_mem.rows()) * offset; // Set pointer to vector A position in memory
            in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows()); // Set NULL value in memory

            //init out mem
            Matrix out_mem = in_mem;
            for (size_t r = 0; r < out_mem.rows(); ++r)
            {
                // Select the permutation P
                Matrix perm = fuzzy_encode(out_mem.block(r, 1, 1, offset - 1));

                // Select the not already permutated A
                Matrix A = out_mem.block(r, offset, 1, out_mem.cols() - offset - 1);

                // Permutate the elements of A according to the permutation P
                out_mem.block(r, 1, 1, offset - 1) = A * perm;
			}

            // Cut out from the cost calculation the memory part that does not make part of the expected output
            out_mem.col(0) = ColVector::Ones(out_mem.rows()) * -1;
            out_mem.block(0, offset, out_mem.rows(), out_mem.cols() - offset)
					= Matrix::Ones(out_mem.rows(), offset) * -1;

            //return
            return std::make_tuple(in_mem, out_mem, Task::init_regs());
        }
    };
    REGISTERED_TASK(TaskPermutation, "permutation")

	class TaskListK : public Task
	{
	public:
		TaskListK
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
			using namespace Eigen;

			//alloc
			Matrix::Index list_size = (m_max_int - 4) / 2;
			Matrix in_mem(m_batch_size, m_max_int);
			Matrix list_elements(m_batch_size, list_size);

			//init in mem
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(m_max_int - 1)); });

			// Initialize the permutation for all the examples
			for (size_t r = 0; r < in_mem.rows(); ++r)
			{
				PermutationMatrix< Dynamic, Dynamic > perm(list_size);
				perm.setIdentity();

				std::random_device rd;
				std::mt19937 g(rd());

				std::shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size(), g);

				Matrix m_permutation = perm
						.indices()
						.cast<Scalar>()
						.transpose();

				for (Matrix::Index e = 0; e < m_permutation.cols(); ++e)
				{
					Matrix::Index current_element_pointer = Matrix::Index(m_permutation(0, e)); // Pointer inside the list of the current element

					// Search the next element in the list m_permutation
					Matrix::Index pointer_to_the_next = 0; // Pointer of the next element in the list respect to the current
					for (; pointer_to_the_next < m_permutation.cols(); ++pointer_to_the_next)
					{
						if (Matrix::Index(m_permutation(0, pointer_to_the_next)) == (current_element_pointer + 1))
							break;
					}

					// If the current element is the first element in the list then update the head pointer in the memory
					if (current_element_pointer == 0)
						in_mem(r, 0) = 3 + 2 * e;

					// Set the pointer to the next elements in the list
					if (pointer_to_the_next != m_permutation.size())
						in_mem(r, 3 + 2 * e) = 3 + 2 * pointer_to_the_next;
					else
						in_mem(r, 3 + 2 * e) = in_mem.cols() - 1;

					// Set the value of the node of the current list element
					in_mem(r, 3 + 2 * e + 1) = list_elements(r, e);
				}
			}
			in_mem.col(2) = ColVector::Ones(in_mem.rows()) * 2; // Set pointer to vector memory position where the searched element will be writed
			in_mem.col(1) = in_mem.col(1).unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.index_rand(list_size - 1)); }); // Set the position in the list to read to read
			in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows()); // Set NULL value in memory

			//init out mem
			Matrix out_mem = in_mem;
			for (size_t r = 0; r < out_mem.rows(); ++r)
			{
				// Search the element to read and substitute to the memory section indicated at the 2^nd position of the memory itself
				Scalar output = 0.0;
				Matrix::Index pointer = Matrix::Index(out_mem(r, 0));
				for (Matrix::Index hop = 0; hop <= Matrix::Index(out_mem(r, 1)); ++hop)
				{
					output = out_mem(r, pointer + 1);
					pointer = out_mem(r, pointer);
				}
				out_mem(r, Matrix::Index(out_mem(r, 2))) = output;
			}

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			out_mem.block(0, 0, out_mem.rows(), 2) = Matrix::Ones(out_mem.rows(), 2) * -1;
			out_mem.block(0, 3, out_mem.rows(), out_mem.cols() - 3)
					= Matrix::Ones(out_mem.rows(), out_mem.cols() - 3) * -1;

			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskListK, "listk")
}
}