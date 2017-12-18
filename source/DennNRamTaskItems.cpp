#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include "DennDump.h"

namespace Denn
{
namespace NRam
{

	/**
	 * [Access] Given a value k and an array A, return A[k]. Input is given as k, A[0], .., A[n −
   * 1], NULL and the network should replace the first memory cell with A[k].
	 */
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
			// Create and initialize the starting memory
			Matrix in_mem(m_batch_size, m_max_int);
			in_mem = in_mem.unaryExpr(
				[&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int)); }
			);
			in_mem.col(m_max_int - 1) = ColVector::Zero(in_mem.rows());
			
			// Initialize the pointers to the elements to be accessed
			in_mem.col(0) = in_mem.col(0).unaryExpr(
				[&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int - 1)); }
			);

			// Initialize the desired memory
			Matrix out_mem = in_mem;
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)  
				out_mem(r, 0) = out_mem(r, Matrix::Index(out_mem(r, 0)));

			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		};
	}; 
	REGISTERED_TASK(TaskAccess, "access")

  /**
	 * [Increment] Given an array A, increment all its elements by 1. Input is given as
   * A[0], ..., A[n − 1], NULL and the expected output is A[0] + 1, ..., A[n − 1] + 1.
	 */
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
			// Create and initialize the starting memory
			Matrix in_mem(m_batch_size, m_max_int);
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int - 1)); });
			in_mem.col(in_mem.cols() - 1) = ColVector::Zero(m_batch_size);

			// Initialize desired memory
			Matrix out_mem = in_mem;
			for (Matrix::Index c = 0; c < in_mem.cols() - 1; ++c)  
				out_mem.col(c) += ColVector::Ones(out_mem.rows());
			
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskIncrement, "increment")

	/**
	 * [Copy] Given an array and a pointer to the destination, copy all elements from the array to
   * the given location. Input is given as p, A[0], ..., A[n−1] where p points to one element after
   * A[n−1]. The expected output is A[0], ..., A[n−1] at positions p, ..., p+n−1 respectively.
	 */
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
			// Create and initialize the starting memory
			Matrix::Index offset(((m_max_int - 2) / 2) + 1);
			Matrix in_mem(m_batch_size, m_max_int);
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int)); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);
			in_mem.col(0) = ColVector::Ones(in_mem.rows()) * offset;

			// Create the desired mem
			Matrix out_mem = in_mem;
			out_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset - 1) = in_mem.block(0, 1, in_mem.rows(), offset - 1);

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			out_mem.block(0, 0, in_mem.rows(), offset) = Matrix::Ones(out_mem.rows(), offset) * -1;
			out_mem.col(out_mem.cols() - 1) = ColVector::Ones(out_mem.rows()) * -1;

			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskCopy, "copy")

  /**
	 * [Reverse] Given an array and a pointer to the destination, copy all elements from the array
	 * in reversed order. Input is given as p, A[0], ..., A[n − 1] where p points one element after
   * A[n−1]. The expected output is A[n−1], ..., A[0] at positions p, ..., p+n−1 respectively.
	 */
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
			// Initialize pointer to parts of the memory where the NRAM will be reverse the vector A
			Matrix::Index offset(((m_max_int - 2) / 2) + 1);

			// Initialize the starting memory
			Matrix in_mem(m_batch_size, m_max_int);
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int)); });
			in_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset) = Matrix::Zero(m_batch_size, in_mem.cols() - offset);

			// Create the desired memory
			Matrix out_mem = in_mem;
			out_mem.block(0, offset, in_mem.rows(), in_mem.cols() - offset - 1) \
				= out_mem.block(0, 1, in_mem.rows(), offset - 1).rowwise().reverse();

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			out_mem.block(0, 0, in_mem.rows(), offset) = Matrix::Ones(out_mem.rows(), offset) * -1;
			out_mem.col(out_mem.cols() - 1) = ColVector::Ones(out_mem.rows()) * -1;

			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskReverse, "reverse")

  /**
	 * [Swap] Given two pointers p, q and an array A, swap elements A[p] and A[q]. Input is
   * given as p, q, A[0], .., A[p], ..., A[q], ..., A[n − 1], 0. The expected modified array A is:
   * A[0], ..., A[q], ..., A[p], ..., A[n − 1].
	 */
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
			// Initialize starting memory
			Matrix in_mem(m_batch_size, m_max_int);
			in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int)); });

			// Set pointers of elements to swap
			in_mem.col(0) = in_mem.block(0, 0, in_mem.rows(), 1).unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(2, m_max_int - 1)); });
			for (Matrix::Index r = 0; r < in_mem.rows(); ++r)
				in_mem(r, 1) = Scalar(std::floor(m_random.uniform(Matrix::Index(in_mem(r, 0)) + 1, m_max_int - 1)));
		
			// Set NULL values to the last column as terminator
			in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows());

			// Init desired memory
			Matrix out_mem = in_mem;

			// Swap elements for each example
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
			{
				// Swap
				Scalar first_value = out_mem(r, Matrix::Index(out_mem(r, 0)));
				out_mem(r, Matrix::Index(out_mem(r, 0))) = out_mem(r, Matrix::Index(out_mem(r, 1)));
				out_mem(r, Matrix::Index(out_mem(r, 1))) = first_value;
			}

			// Cut out the the memory parts that does not make part of the expected output
			out_mem.block(0, 0, out_mem.rows(), 2) = Matrix::Ones(out_mem.rows(), 2) * -1;
			out_mem.col(in_mem.cols() - 1) = ColVector::Ones(in_mem.rows()) * -1;

			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskSwap, "swap")

	/**
	 * [Permutation] Given two arrays of n elements: P (contains a permutation of numbers
	 * 0, . . . , n − 1) and A (contains random elements), permutate A according to P. Input is
   * given as a, P[0], ..., P[n − 1], A[0], ..., A[n − 1], where a is a pointer to the array A. The
   * expected output is A[P[0]], ..., A[P[n − 1]], which should override the array P.
	 */
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
				in_mem = in_mem.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int - 1)); });

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
				for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
				{
					// Select the permutation P
					Matrix perm = fuzzy_encode(out_mem.block(r, 1, 1, offset - 1));

					// Select the not already permutated A
					Matrix A = out_mem.block(r, offset, 1, out_mem.cols() - offset - 1);

					// Permutate the elements of A according to the permutation P
					out_mem.block(r, 1, 1, offset - 1) = A * perm.transpose();
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

  /**
	 * [ListK] Given a pointer to the head of a linked list and a number k, find the value of the
   * k-th element on the list. List nodes are represented as two adjacent memory cells: a pointer
   * to the next node and a value. Elements are in random locations in the memory, so that
   * the network needs to follow the pointers to find the correct element. Input is given as:
   * head, k, out, ... where head is a pointer to the first node on the list, k indicates how many
   * hops are needed and out is a cell where the output should be put.
	 */
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
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int - 1)); });

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
						in_mem(r, 3 + (2 * e)) = 3 + (2 * pointer_to_the_next);
					else
						in_mem(r, 3 + (2 * e)) = Scalar(-1);

					// Set the value of the node of the current list element
					in_mem(r, 3 + (2 * e) + 1) = list_elements(r, e);
				}
			}
			in_mem.col(2) = ColVector::Ones(in_mem.rows()) * 2; // Set pointer to vector memory position where the searched element will be writed
			in_mem.col(1) = in_mem.col(1).unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, list_size - 1)); }); // Set the position in the list to read to read
			in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows()); // Set NULL value in memory

			// Create the desired memory
			Matrix out_mem = in_mem;
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
			{
				// Search the element to read and substitute to the memory section indicated at the 2^nd position of the memory itself
				Scalar output = 0.0;
				Matrix::Index pointer = Matrix::Index(out_mem(r, 0));
				for (Matrix::Index hop = 0; hop <= Matrix::Index(out_mem(r, 1)); ++hop)
				{
					output = out_mem(r, pointer + 1);
					pointer = Matrix::Index(out_mem(r, pointer));
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

	/**
	 * [ListSearch] Given a pointer to the head of a linked list and a value `v` to find return a pointer
	 * to the first node on the list with the value `v`. The list is placed in memory in the same way
	 * as in the task ListK. We fill empty memory with “trash” values to prevent the network from
   * “cheating” and just iterating over the whole memory. 
	 */
	class TaskListSearch : public Task
	{
	public:
		TaskListSearch
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
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random.uniform(1, m_max_int - 1)); });

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
						in_mem(r, 0) = 2 + 2 * e;

					// Set the pointer to the next elements in the list
					if (pointer_to_the_next != m_permutation.size())
						in_mem(r, 2 + (2 * e)) = 2 + (2 * pointer_to_the_next);
					else
						in_mem(r, 2 + (2 * e)) = -1;

					// Set the value of the node of the current list element
					in_mem(r, 2 + (2 * e) + 1) = list_elements(r, e);
				}
			}
			in_mem.col(1) = list_elements.col(0); // Set the position in the list to read to read
			in_mem.block(0, in_mem.cols() - 2, in_mem.rows(), 2) = Matrix::Zero(in_mem.rows(), 2); // Set NULL value in memory

			//init out mem
			Matrix out_mem = in_mem;
			for (size_t r = 0; r < out_mem.rows(); ++r)
			{
				// Search the element to read and substitute to the memory section indicated at the 2^nd position of the memory itself
				bool found = false;
				Matrix::Index pointer = Matrix::Index(out_mem(r, 0));
				while (!found && pointer != -1) // i.e. the searched element is found or the search has not produced results
				{
					if (Matrix::Index(out_mem(r, Matrix::Index(pointer) + 1)) == Matrix::Index(out_mem(r, 1))) // Value if found, Stop the search
					{
						out_mem(r, 0) = pointer;
						found = true;
					} 
					else // Go to the next element
						pointer = Matrix::Index(out_mem(r, pointer));
				}
			}
			out_mem.block(0, 1, out_mem.rows(), out_mem.cols() - 1) =
				Matrix::Ones(out_mem.rows(), out_mem.cols() - 1) * -1;

			//return
			return std::make_tuple(in_mem, out_mem, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskListSearch, "listsearch")

}
}