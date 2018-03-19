#include "Config.h"
#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include "DennDump.h"

namespace Denn
{
namespace NRam
{


	template < class T >
	class TaskImplement : public Task
	{
	protected:

		TaskImplement() : Task()
		{
			//none
		}

		TaskImplement
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		:
		Task
		(
		  batch_size
		, max_int
		, n_regs
		, timesteps
		, sequence_size
		, min_difficulty
		, max_difficulty
		, step_gen_change_difficulty
		, change_difficulty_lambda		
		, random
		)
		{
			//none
		}

		// Clone a this task
		virtual Task::SPtr clone() const override
		{
			return std::dynamic_pointer_cast<Task>(std::make_shared< T >(*((T*)this)));
		} 

		//destructor
		virtual ~TaskImplement()
		{
			//none
		}
	};


	/**
	 * [Access] 
	 * Given a value k and an array A, return A[k]. Input is given as k, A[0], .., A[n −
   * 1], NULL and the network should replace the first memory cell with A[k].
	 */
	class TaskAccess : public TaskImplement < TaskAccess >
	{
	public:
		TaskAccess
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(12, 2, 10),
				std::make_tuple(12, 3, 10),
				std::make_tuple(12, 4, 10),
				std::make_tuple(12, 5, 10),
				std::make_tuple(12, 6, 10),
				std::make_tuple(12, 7, 10)
			};
		}

		MemoryTuple operator()() override
		{
			// Create and initialize the starting memory
			const size_t memory_size(m_sequence_size + 2);

			Matrix in_mem(m_batch_size, memory_size);
			in_mem = in_mem.unaryExpr(
				[&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, memory_size)); }
			);
			in_mem.col(in_mem.cols() - 1) = ColVector::Zero(in_mem.rows());
			
			// Initialize the pointers to the elements to be accessed
			in_mem.col(0) = in_mem.col(0).unaryExpr(
				[&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, memory_size - 1)); }
			);

			// Initialize the desired memory
			Matrix out_mem = in_mem;
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)  
				out_mem(r, 0) = out_mem(r, Matrix::Index(out_mem(r, 0)));

			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.col(0) = ColVector::Ones(m_batch_size);
			
			return std::make_tuple(in_mem, out_mem, Task::init_mask(), error_mask, Task::init_regs());
		};
	}; 
	REGISTERED_TASK(TaskAccess, "access")

  /**
	 * [Increment] 
	 * Given an array A, increment all its elements by 1. Input is given as
   * A[0], ..., A[n − 1], NULL and the expected output is A[0] + 1, ..., A[n − 1] + 1.
	 */
	class TaskIncrement : public TaskImplement < TaskIncrement >
	{
	public:
		TaskIncrement
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(8, 7, 4),
				std::make_tuple(9, 8, 5),
				std::make_tuple(10, 9, 6),
				std::make_tuple(11, 10, 7),
				std::make_tuple(12, 11, 8)
			};
		}

		MemoryTuple operator()() override
		{
			// Create and initialize the starting memory
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			in_mem.block(0, 0, in_mem.rows(), m_sequence_size) = in_mem
				.block(0, 0, in_mem.rows(), m_max_int - 1)
					.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(0, m_max_int - 1)); });

			// Initialize desired memory
			Matrix out_mem = in_mem;
			out_mem.block(0, 0, out_mem.rows(), m_sequence_size) \
				= out_mem.block(0, 0, out_mem.rows(), m_sequence_size)
						.unaryExpr([&](Scalar x) -> Scalar { return Scalar(x + 1); });

			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.block(0, 0, error_mask.rows(), std::max(m_timesteps, m_max_int)) = Matrix::Ones(m_batch_size, std::max(m_timesteps, m_max_int));
			
			return std::make_tuple(in_mem, out_mem, Task::init_mask(), error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskIncrement, "increment")

	/**
	 * [Copy] 
	 * Given an array and a pointer to the destination, copy all elements from the array to
   * the given location. Input is given as p, A[0], ..., A[n−1] where p points to one element after
   * A[n−1]. The expected output is A[0], ..., A[n−1] at positions p, ..., p+n−1 respectively.
	 */
	class TaskCopy : public TaskImplement < TaskCopy >
	{
	public:
		TaskCopy
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(10, 5, 	1),
				std::make_tuple(10, 6, 	1),
				std::make_tuple(10, 7, 	1),
				std::make_tuple(10, 8, 	1),

				std::make_tuple(10, 7, 	2),
				std::make_tuple(10, 8, 	2),
				std::make_tuple(10, 9, 	2),
				std::make_tuple(10, 10, 2),

				std::make_tuple(10, 9, 	3),
				std::make_tuple(10, 10, 3),
				std::make_tuple(10, 11, 3),
				std::make_tuple(10, 12, 3),

				std::make_tuple(10, 11, 4),
				std::make_tuple(10, 12, 4),
				std::make_tuple(10, 13, 4),
				std::make_tuple(10, 14, 4)
			};
		}

		MemoryTuple operator()() override
		{
			// Create and initialize the starting memory
			Matrix::Index remaining_space(m_max_int - 2);
			Matrix::Index offset(m_max_int / 2);
			
			m_sequence_size = std::min(m_sequence_size, int(remaining_space / 2));

			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			in_mem.col(0) = ColVector::Constant(in_mem.rows(), offset);
			in_mem.block(0, 1, in_mem.rows(), m_sequence_size) \
				= in_mem
						.block(0, 1, in_mem.rows(), m_sequence_size)
							.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int)); });

			// Create the desired mem
			Matrix out_mem = in_mem;
			out_mem.block(0, offset, in_mem.rows(), m_sequence_size) \
				= in_mem.block(0, 1, in_mem.rows(), m_sequence_size);

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[1, max_int]
			mask.leftCols(1) = RowVector::Zero(1);

			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.block(0, offset, error_mask.rows(), m_sequence_size) = Matrix::Ones(m_batch_size, m_sequence_size);

			return std::make_tuple(in_mem, out_mem, mask, error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskCopy, "copy")

  /**
	 * [Reverse] 
	 * Given an array and a pointer to the destination, copy all elements from the array
	 * in reversed order. Input is given as p, A[0], ..., A[n − 1] where p points one element after
   * A[n−1]. The expected output is A[n−1], ..., A[0] at positions p, ..., p+n−1 respectively.
	 */
	class TaskReverse : public TaskImplement < TaskReverse >
	{
	public:
		TaskReverse
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(10, 5, 	1),
				std::make_tuple(10, 6, 	1),
				std::make_tuple(10, 7, 	1),
				std::make_tuple(10, 8, 	1),

				std::make_tuple(10, 7, 	2),
				std::make_tuple(10, 8, 	2),
				std::make_tuple(10, 9, 	2),
				std::make_tuple(10, 10, 2),

				std::make_tuple(10, 9, 	3),
				std::make_tuple(10, 10, 3),
				std::make_tuple(10, 11, 3),
				std::make_tuple(10, 12, 3),

				std::make_tuple(10, 11, 4),
				std::make_tuple(10, 12, 4),
				std::make_tuple(10, 13, 4),
				std::make_tuple(10, 14, 4)
			};
		}

		MemoryTuple operator()() override
		{
			// Init some things
			Matrix::Index remaining_space(m_max_int - 2); // Remaining space without pointers and null terminators
			Matrix::Index offset(m_max_int / 2); // Pointer to the part of the memory where the NRAM will be reverse the vector A
			m_sequence_size = std::min(m_sequence_size, int(remaining_space / 2));

			// Initialize the starting memory
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			in_mem.block(0, 1, in_mem.rows(), m_sequence_size) \
				= in_mem
						.block(0, 1, in_mem.rows(), m_sequence_size)
						.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int)); });

			// Create the desired memory
			Matrix out_mem = in_mem;
			out_mem.block(0, m_max_int - 1 - m_sequence_size, in_mem.rows(), m_sequence_size) \
				= out_mem.block(0, 1, in_mem.rows(), m_sequence_size).rowwise().reverse();

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[1, max_int]
			mask.leftCols(1) = RowVector::Zero(1);

			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.block(0, m_max_int - 1 - m_sequence_size, error_mask.rows(), m_sequence_size) = Matrix::Ones(m_batch_size, m_sequence_size);

			return std::make_tuple(in_mem, out_mem, mask, error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskReverse, "reverse")

  /**
	 * [Swap] 
	 * Given two pointers p, q and an array A, swap elements A[p] and A[q]. Input is
   * given as p, q, A[0], .., A[p], ..., A[q], ..., A[n − 1], 0. The expected modified array A is:
   * A[0], ..., A[q], ..., A[p], ..., A[n − 1].
	 */
	class TaskSwap : public TaskImplement< TaskSwap >
	{
	public:
		TaskSwap
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(8, 3, 7),
				std::make_tuple(8, 4, 7),
				std::make_tuple(8, 6, 7),
				std::make_tuple(8, 7, 7),
				std::make_tuple(8, 8, 7),

				std::make_tuple(10, 3, 9),
				std::make_tuple(10, 4, 9),
				std::make_tuple(10, 6, 9),
				std::make_tuple(10, 7, 9),
				std::make_tuple(10, 8, 9)
			};
		}

		MemoryTuple operator()() override
		{
			// Initialize starting memory
			const Matrix::Index memory_size(m_sequence_size + 3);
			Matrix in_mem = Matrix::Zero(m_batch_size, memory_size);
			in_mem.block(0, 0, in_mem.rows(), memory_size - 1) = 
				in_mem.block(0, 0, in_mem.rows(), memory_size - 1)
					.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, memory_size - 1)); });

			// Set pointers of elements to swap
			for (Matrix::Index r = 0; r < in_mem.rows(); ++r)
			{
				Matrix::Index value_one = m_random->uirand(2, memory_size - 3);
				Matrix::Index value_two = m_random->uirand(value_one + 1, memory_size - 1);
				Matrix::Index index_one = m_random->uirand(2, memory_size - 2);
				Matrix::Index index_two = m_random->uirand(index_one + 1, memory_size - 1);
				in_mem(r, 0) = index_one;
				in_mem(r, 1) = index_two;

				in_mem(r, index_one) = value_one;
				in_mem(r, index_two) = value_two;
			}

			// Init desired memory and swap elements for each example
			Matrix out_mem = in_mem;
			Matrix error_mask = Matrix::Zero(m_batch_size, memory_size);
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
			{
				error_mask(r, Matrix::Index(in_mem(r, 1))) = 1;
				if (m_sequence_size == 1)
					out_mem(r, Matrix::Index(in_mem(r, 1))) = out_mem(r, Matrix::Index(in_mem(r, 0)));
				else
				{
					std::swap(
						out_mem(r, Matrix::Index(in_mem(r, 0))), 
						out_mem(r, Matrix::Index(in_mem(r, 1)))
					);
					error_mask(r, Matrix::Index(in_mem(r, 0))) = 1;
				}	
			}

			// Cut out the the memory parts that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[1, max_int]
			mask.leftCols(2)  = RowVector::Zero(2);
			
			return std::make_tuple(in_mem, out_mem, mask, error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskSwap, "swap")

	/**
	 * [Permutation] 
	 * Given two arrays of n elements: P (contains a permutation of numbers
	 * 0, . . . , n − 1) and A (contains random elements), permutate A according to P. Input is
   * given as a, P[0], ..., P[n − 1], A[0], ..., A[n − 1], where a is a pointer to the array A. The
   * expected output is A[P[0]], ..., A[P[n − 1]], which should override the array P.
	 */
	class TaskPermutation : public TaskImplement< TaskPermutation >
	{
	public:
		TaskPermutation
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(8, 6, 	1),
				std::make_tuple(8, 7, 	1),
				std::make_tuple(8, 8, 	1),
				std::make_tuple(8, 9, 	1),

				std::make_tuple(8, 7, 	2),
				std::make_tuple(8, 8, 	2),
				std::make_tuple(8, 9, 	2),
				std::make_tuple(8, 10, 	2),

				std::make_tuple(8, 9, 	3),
				std::make_tuple(8, 10, 	3),
				std::make_tuple(8, 11, 	3),
				std::make_tuple(8, 12, 	3),

				std::make_tuple(10, 6, 	1),
				std::make_tuple(10, 7, 	1),
				std::make_tuple(10, 8, 	1),
				std::make_tuple(10, 9, 	1),

				std::make_tuple(10, 7, 	2),
				std::make_tuple(10, 8, 	2),
				std::make_tuple(10, 9, 	2),
				std::make_tuple(10, 10, 2),

				std::make_tuple(10, 9, 	3),
				std::make_tuple(10, 10, 3),
				std::make_tuple(10, 11, 3),
				std::make_tuple(10, 12, 3),

				std::make_tuple(10, 11, 4),
				std::make_tuple(10, 12, 4),
				std::make_tuple(10, 13, 4),
				std::make_tuple(10, 14, 4),
			}; 
		}

		MemoryTuple operator()() override
		{
			using namespace Eigen;

			//alloc
			const size_t remaining_space = (m_timesteps - 2);
			const size_t l_bound_values = (m_max_int - 1) / 2;
			Matrix::Index offset(std::floor(m_max_int / 2) + 1);
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);

			//init in mem
			in_mem.block(0, offset, in_mem.rows(), m_sequence_size) = 
				in_mem.block(0, offset, in_mem.rows(), m_sequence_size)
					.unaryExpr([&](Scalar x) -> Scalar { 
						return std::floor(m_random->uniform(l_bound_values - 1, m_max_int - 1)); 
					});

			// Initialize the permutation for all the examples
			for (size_t r = 0; r < in_mem.rows(); ++r)
			{ 
				PermutationMatrix< Dynamic, Dynamic > perm(m_sequence_size);
				perm.setIdentity();
				std::random_device rd;
				std::mt19937 g(rd());
				std::shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size(), g);
				in_mem.block(r, 1, 1, m_sequence_size) = perm
					.indices()
					.cast<Scalar>()
					.transpose();
			}
			
			// Create and initialize the desired memory
			Matrix out_mem = in_mem;
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
			{
				// Select the permutation P
				Matrix perm = fuzzy_encode(out_mem.block(r, 1, 1, m_sequence_size));

				// Select the not already permutated A
				Matrix A = out_mem.block(r, offset, 1, m_sequence_size);

				// Permutate the elements of A according to the permutation P
				out_mem.block(r, 1, 1, m_sequence_size) = A * perm.transpose();
			}

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[1, max_int]
			mask.leftCols(1) = RowVector::Zero(1);

			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.block(0, 1, in_mem.rows(), m_sequence_size) = Matrix::Ones(m_batch_size, m_sequence_size);

			//return
			return std::make_tuple(in_mem, out_mem, mask, error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskPermutation, "permutation")

  /**
	 * [ListK] 
	 * Given a pointer to the head of a linked list and a number k, find the value of the 
	 * k-th element on the list. List nodes are represented as two adjacent memory cells: a pointer
	 * to the next node and a value. Elements are in random locations in the memory, so that
	 * the network needs to follow the pointers to find the correct element. Input is given as:
	 * head, k, out, ... where head is a pointer to the first node on the list, k indicates how many
	 * hops are needed and out is a cell where the output should be put.
	 */
	class TaskListK : public TaskImplement< TaskListK >
	{
	public:
		TaskListK
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(7, 6, 1),
				std::make_tuple(9, 8, 1),
				std::make_tuple(11, 10, 1),
				std::make_tuple(13, 12, 1),
				std::make_tuple(15, 14, 1)
			}; 
		}

		MemoryTuple operator()() override
		{
			using namespace Eigen;

			//alloc
			Matrix::Index list_size = (m_max_int - 4) / 2;
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			Matrix list_elements(m_batch_size, list_size);

			//init in mem
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });

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
					while (pointer_to_the_next < m_permutation.cols() &&
						Matrix::Index(m_permutation(0, pointer_to_the_next)) != (current_element_pointer + 1))
						++pointer_to_the_next;

					// If the current element is the first element in the list then update the head pointer in the memory
					if (current_element_pointer == 0)
						in_mem(r, 0) = 3 + 2 * e;

					// Set the pointer to the next elements in the list
					if (pointer_to_the_next != m_permutation.size())
						in_mem(r, 3 + (2 * e)) = 3 + (2 * pointer_to_the_next);
					else
						in_mem(r, 3 + (2 * e)) = Scalar(0);

					// Set the value of the node of the current list element
					in_mem(r, 3 + (2 * e) + 1) = list_elements(r, e);
				}
			}
			in_mem.col(2) = ColVector::Constant(in_mem.rows(), 2); // Set pointer to vector memory position where the searched element will be writed
			in_mem.col(1) = in_mem.col(1).unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, list_size - 1)); }); // Set the position in the list to read

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
			Matrix mask = Task::init_mask(); //[1, max_int - 1]
			mask.leftCols(1)  = RowVector::Zero(1);

			//return
			return std::make_tuple(in_mem, out_mem, mask, Matrix::Zero(m_batch_size, m_max_int), Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskListK, "listk")

	/**
	 * [ListSearch] 
	 * Given a pointer to the head of a linked list and a value `v` to find return a pointer
	 * to the first node on the list with the value `v`. The list is placed in memory in the same way
	 * as in the task ListK. We fill empty memory with “trash” values to prevent the network from
   * “cheating” and just iterating over the whole memory. 
	 */
	class TaskListSearch : public TaskImplement< TaskListSearch >
	{
	public:
		TaskListSearch
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(7, 6, 1),
				std::make_tuple(9, 8, 1),
				std::make_tuple(11, 10, 1),
				std::make_tuple(13, 12, 1),
				std::make_tuple(15, 14, 1)
			};
		}

		MemoryTuple operator()() override
		{
			using namespace Eigen;

			//alloc
			Matrix::Index list_size = (m_max_int - 3) / 2;
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			Matrix list_elements(m_batch_size, list_size);

			//init in mem
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });

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
					while (pointer_to_the_next < m_permutation.cols() &&
						Matrix::Index(m_permutation(0, pointer_to_the_next)) != (current_element_pointer + 1))
						++pointer_to_the_next;


					// If the current element is the first element in the list then update the head pointer in the memory
					if (current_element_pointer == 0)
						in_mem(r, 0) = 2 + 2 * e;

					// Set the pointer to the next elements in the list
					if (pointer_to_the_next != m_permutation.size())
						in_mem(r, 2 + (2 * e)) = 2 + (2 * pointer_to_the_next);
					else
						in_mem(r, 2 + (2 * e)) = 0;

					// Set the value of the node of the current list element
					in_mem(r, 2 + (2 * e) + 1) = list_elements(r, e);
				}
			}
			in_mem.col(1) = list_elements.col(0); // Set the position in the list to read to read

			//init out mem
			Matrix out_mem = in_mem;
			for (size_t r = 0; r < out_mem.rows(); ++r)
			{
				// Search the element to read and substitute to the memory section indicated at the 2^nd position of the memory itself
				bool found = false;
				Matrix::Index pointer = Matrix::Index(out_mem(r, 0));
				while (!found && pointer != 0) // i.e. the searched element is found or the search has not produced results
				{
					// If the values is found stop the search
					if (Matrix::Index(out_mem(r, Matrix::Index(pointer) + 1)) == Matrix::Index(out_mem(r, 1))) 
					{
						out_mem(r, 0) = pointer;
						found = true;
					} 
					else // Go to the next element
						pointer = Matrix::Index(out_mem(r, pointer));
				}
			}

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[3, max_int - 1]
			mask.leftCols(1)  = RowVector::Zero(1);

			//return
			return std::make_tuple(in_mem, out_mem, mask, Matrix::Zero(m_batch_size, m_max_int), Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskListSearch, "listsearch")

	/**
	 * [Merge]
	 * Given pointers to 2 sorted arrays A and B, and the pointer to the output o,
	 * merge the two arrays into one sorted array. The input is given as: a, b, o, A[0], .., A[n −
	 * 1], G, B[0], ..., B[m − 1], G, where G is a special guardian value, a and b point to the first
	 * elements of arrays A and B respectively, and o points to the address after the second G.
	 * The n + m element should be written in correct order starting from position o
	 */ 
	class TaskMerge : public TaskImplement< TaskMerge >
	{
	public:
		TaskMerge
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(9, 6, 1),
				std::make_tuple(12, 9, 1),
				std::make_tuple(15, 12, 1),
				std::make_tuple(18, 15, 1),
				std::make_tuple(21, 18, 1)
			};
		}

		MemoryTuple operator()() override
		{
			// Initialize some parameters and create the memory
			Matrix::Index offset = 3;
			Matrix::Index remaining_space = m_max_int - 6;
			bool odd_subvector_space = !(remaining_space % 4 == 0);

			Matrix::Index list_size_a = remaining_space / 4, 
										list_size_b = (odd_subvector_space ? list_size_a + 1 : list_size_a);
			
			Matrix::Index list_size_a_plus_b = list_size_a + list_size_b;
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			Matrix list_elements_a = Matrix::Zero(m_batch_size, list_size_a);
			Matrix list_elements_b = Matrix::Zero(m_batch_size, list_size_b);
			Matrix list_elements_a_plus_b = Matrix::Zero(m_batch_size, list_size_a_plus_b);

			// Initilize memories and sort them
			list_elements_a = list_elements_a.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });
			list_elements_b = list_elements_b.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });
			sort_rows_ascending(list_elements_a);
			sort_rows_ascending(list_elements_b);

			// Initialize the output memory and sort it
			list_elements_a_plus_b.block(0, 0, list_elements_a_plus_b.rows(), list_size_a) = list_elements_a;
			list_elements_a_plus_b.block(0, list_size_a, list_elements_a_plus_b.rows(), list_size_b) = list_elements_b;
			sort_rows_ascending(list_elements_a_plus_b);

			// Add data to starting NRAM memory
			in_mem.col(0) = ColVector::Constant(in_mem.rows(), offset); // Starting point of list A
			in_mem.col(1) = ColVector::Constant(in_mem.rows(), offset + list_size_a + 1); // Starting point of list B
			in_mem.col(2) = ColVector::Constant(in_mem.rows(), offset + (2 * list_size_a) + (odd_subvector_space ? 3 : 2)); // Starting point of list A + B
			in_mem.block(0, offset, in_mem.rows(), list_size_a) = list_elements_a; // Copy of A
			in_mem.block(0, offset + list_size_a + 1, in_mem.rows(), list_size_b) = list_elements_b; // Copy B

			// Create and initialize desired memory
			Matrix out_mem = in_mem;
			out_mem.block(0, offset + list_size_a_plus_b + 2, out_mem.rows(), list_size_a_plus_b) = list_elements_a_plus_b;

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[3, max_int - 1]
			mask.leftCols(3) = RowVector::Zero(3);

			return std::make_tuple(in_mem, out_mem, mask, Matrix::Zero(m_batch_size, m_max_int), Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskMerge, "merge")

	/**
	 * [WalkBST]
   * Given a pointer to the root of a Binary Search Tree, and a path to be traversed,
   * return the element at the end of the path. The BST nodes are represented as triples (v, l,
   * r), where v is the value, and l, r are pointers to the left/right child. The triples are placed
   * randomly in the memory. Input is given as root, out, d1, d2, ..., dk, NULL, ..., where root
   * points to the root node and out is a slot for the output. The sequence d1...dk, di ∈ {0, 1}
   * represents the path to be traversed: di = 0 means that the network should go to the left
   * child, di = 1 represents going to the right child.
	 */
	class TaskWalkBST : public TaskImplement< TaskWalkBST >
	{
	public:
		TaskWalkBST
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(7, 5, 1),
				std::make_tuple(11, 7, 1),
				std::make_tuple(15, 9, 1),
				std::make_tuple(19, 11, 1),
				std::make_tuple(23, 13, 1)
			};
		}

		MemoryTuple operator()() override
		{
			using namespace Eigen;

			// Create memory and initialize others data
			Matrix::Index remaining_space(m_max_int - 3);
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			Matrix out_mem = in_mem;
			Matrix::Index num_elements(remaining_space / 4);
			Matrix::Index offset(2);
			Matrix::Index divider_position(offset + num_elements);

			// Create and initialize elements of bsts
			Matrix list_elements(m_batch_size, num_elements);
			list_elements = list_elements.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });

			//Create and initialize the walk of bst
			Matrix walks_bst(m_batch_size, num_elements - 1);
			walks_bst = walks_bst.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(0, 2)); });

			// Initialize the permutation for all the examples and the memories
			for (size_t r = 0; r < in_mem.rows(); ++r)
			{
				// Create the elements order in memory
				PermutationMatrix< Dynamic, Dynamic > perm(num_elements);
				perm.setIdentity();
				std::random_device rd;
				std::mt19937 g(rd());
				std::shuffle(perm.indices().data(), perm.indices().data() + perm.indices().size(), g);
				Matrix m_permutation = perm
						.indices()
						.cast<Scalar>()
						.transpose();
				Matrix permutated_bst_example_elements = list_elements.block(r, 0, 1, list_elements.cols()) * perm;

				// Create temporary matrix that contain BST for an example
				RowVector example_bst = RowVector::Constant(list_elements.cols() * 3, -1);
				Matrix::Index root_pointer = -1;
				for (Matrix::Index elidx = 0; elidx < list_elements.cols(); ++elidx)
				{
					Matrix::Index pointer = get_element_index(m_permutation, elidx);
					example_bst(pointer * 3) = list_elements(r, elidx);
					if (elidx == 0) root_pointer = pointer * 3;
				}

				// Initialize BST
				for (Matrix::Index elidx = 1; elidx < list_elements.cols(); ++elidx)
					insert_in_bst(example_bst, root_pointer, list_elements(r, elidx), get_element_index(m_permutation, elidx) * 3);

				in_mem(r, 0) = Scalar(divider_position + 1 + root_pointer);
				in_mem.block(r, offset, 1, num_elements - 1) = walks_bst.block(r, 0, 1, walks_bst.cols());
				
				// Walk the bst before the normalization
				Scalar value_found = walk_bst(example_bst, walks_bst.block(r, 0, 1, walks_bst.cols()), -1, root_pointer);

				// Normalize bst pointers for the memory and then save it
				for (Matrix::Index c = 0; c < example_bst.cols(); ++c)
					if (c % 3 != 0)
					{
						if (Matrix::Index(example_bst(c)) != -1) example_bst(c) += divider_position + 1;
						else example_bst(c) = Scalar(0);
					}
					
				in_mem.block(r, divider_position + 1, 1, num_elements * 3) = example_bst;

				// Initialize out mem example
				out_mem.block(r, 0, 1, out_mem.cols()) = in_mem.block(r, 0, 1, in_mem.cols());
				out_mem(r, 1) = value_found;
			}

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask();
			mask.leftCols(1)  = RowVector::Zero(1);

			return std::make_tuple(in_mem, out_mem, Task::init_mask(), Matrix::Zero(m_batch_size, m_max_int), Task::init_regs());
		}

	private:
		void insert_in_bst(RowVector& bst, Matrix::Index pointer, const Scalar& element, const Matrix::Index& element_pointer_in_memory)
		{
			if (bst(pointer) > element)
			{
				if (bst(pointer + 1) == -1) bst(pointer + 1) = element_pointer_in_memory;
				else insert_in_bst(bst, bst(pointer + 1), element, element_pointer_in_memory);
			}
			else
			{
				if (bst(pointer + 2) == -1) bst(pointer + 2) = element_pointer_in_memory;
				else insert_in_bst(bst, bst(pointer + 2), element, element_pointer_in_memory);
			}
		}

		Scalar walk_bst(const RowVector& bst, const RowVector& walk, Matrix::Index previous_pointer, Matrix::Index pointer)
		{
			if (pointer == -1) return bst(previous_pointer);
			else if (walk.size() == 0) return bst(pointer);
			else {
				if (walk(0) == 0) return walk_bst(bst, walk.block(0, 1, 1, walk.size() - 1), pointer, bst(pointer + 1));
				else return walk_bst(bst, walk.block(0, 1, 1, walk.size() - 1), pointer, bst(pointer + 2));
			}
		}

		Matrix::Index get_element_index(const Matrix& permutation, const Matrix::Index idx)
		{
			for (Matrix::Index i = 0; i < permutation.cols(); ++i)
				if (Matrix::Index(permutation(0, i)) == idx) return i;
			return Matrix::Index(-1);
		}
	};
	REGISTERED_TASK(TaskWalkBST, "walkbst")

	/**
	 * [Sum] 
	 * Given pointers to 2 arrays A and B, and the pointer to the output o,
	 * sum the two arrays into one array. The input is given as: a, b, o, A[0], .., A[n −
	 * 1], G, B[0], ..., B[m − 1], G, where G is a special guardian value, a and b point to the first
	 * elements of arrays A and B respectively, and o points to the address after the second G.
	 * The A + B array should be written starting from position o.
	 */
	class TaskSum : public TaskImplement< TaskSum >
	{
	public:
		TaskSum
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(9, 12, 1),
				std::make_tuple(9, 13, 1),
				std::make_tuple(9, 14, 1),
				std::make_tuple(9, 15, 1),
				std::make_tuple(9, 16, 1),
				std::make_tuple(9, 17, 1),

				std::make_tuple(12, 12, 1),
				std::make_tuple(12, 13, 1),
				std::make_tuple(12, 14, 1),
				std::make_tuple(12, 15, 1),
				std::make_tuple(12, 16, 1),
				std::make_tuple(12, 17, 1),

				std::make_tuple(12, 12, 2),
				std::make_tuple(12, 13, 2),
				std::make_tuple(12, 14, 2),
				std::make_tuple(12, 15, 2),
				std::make_tuple(12, 16, 2),
				std::make_tuple(12, 17, 2)
			};
		}

		MemoryTuple operator()() override
		{
			// Initialize some parameters and create the memory
			Matrix::Index remaining_space(m_max_int - 6);
			Matrix::Index space_size(remaining_space / 3);

			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			Matrix list_elements_a = Matrix::Zero(m_batch_size, m_sequence_size);
			Matrix list_elements_b = Matrix::Zero(m_batch_size, m_sequence_size);
			Matrix list_elements_a_plus_b = Matrix::Zero(m_batch_size, m_sequence_size);

			// Initialize arrays
			list_elements_a = list_elements_a.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });
			list_elements_b = list_elements_b.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); });
			list_elements_a_plus_b = list_elements_a + list_elements_b;
			for (Matrix::Index r = 0; r < list_elements_a_plus_b.rows(); ++r)
				for (Matrix::Index c = 0; c < list_elements_a_plus_b.cols(); ++c)
					list_elements_a_plus_b(r, c) = Scalar(positive_mod<size_t>((size_t)list_elements_a_plus_b(r, c), m_max_int));

			// Add data to starting NRAM memory
			in_mem.col(0) = ColVector::Constant(in_mem.rows(), 3); // Starting point of list A
			in_mem.col(1) = ColVector::Constant(in_mem.rows(), 3 + space_size + 1); // Starting point of list B
			in_mem.col(2) = ColVector::Constant(in_mem.rows(), 3 + (2 * space_size) +  2); // Starting point of list A + B
			in_mem.block(0, 3, in_mem.rows(), m_sequence_size) = list_elements_a; // Copy of A
			in_mem.block(0, 3 + space_size + 1, in_mem.rows(), m_sequence_size) = list_elements_b; // Copy B
			
			// Create and initialize desired memory
			Matrix out_mem = in_mem;
			out_mem.block(0, 3 + (2 * space_size) + 2, out_mem.rows(), m_sequence_size) = list_elements_a_plus_b;

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[3, max_int - 1]
			mask.leftCols(3) = RowVector::Zero(3);

			// Create the error mask
			Matrix::Index index_o(in_mem(0, 2));
			Matrix error_mask = Matrix::Zero(m_batch_size, m_max_int);
			error_mask.block(0, index_o, error_mask.rows(), m_sequence_size) = Matrix::Ones(m_batch_size, m_sequence_size);

			return std::make_tuple(in_mem, out_mem, mask, error_mask, Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskSum, "sum")

	/**
	 * [Product] 
	 * Given pointers to 2 arrays A and B, and the pointer to the output o,
	 * make the product of A and B. The input is given as: a, b, o, A[0], .., A[n −
	 * 1], G, B[0], ..., B[m − 1], G, where G is a special guardian value, a and b point to the first
	 * elements of arrays A and B respectively, and o is a slot for the output.
	 */
	class TaskProduct : public TaskImplement< TaskProduct >
	{
	public:
		TaskProduct
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, int 	 sequence_size
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda		
		, Random& random
		)
		: TaskImplement(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random)
		{
			m_difficulty_grades = {
				std::make_tuple(7, 5, 1),
				std::make_tuple(9, 7, 1),
				std::make_tuple(11, 9, 1),
				std::make_tuple(13, 11, 1),
				std::make_tuple(15, 13, 1)
			};
		}

		MemoryTuple operator()() override
		{
			// Initialize some parameters and create the memory
			Matrix::Index remaining_space = m_max_int - 5;
			Matrix::Index arrays_memory_size = remaining_space / 2;

			// Create and initialize the starting memory
			Matrix in_mem = Matrix::Zero(m_batch_size, m_max_int);
			in_mem.col(0) = ColVector::Constant(in_mem.rows(), 3); // Starting point of list A
			in_mem.col(1) = ColVector::Constant(in_mem.rows(), 3 + arrays_memory_size + 1); // Starting point of list B
			in_mem.block(0, 3, in_mem.rows(), arrays_memory_size) = in_mem.block(0, 3, in_mem.rows(), arrays_memory_size)
				.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); }); // Inizialize A
			in_mem.block(0, 3 + arrays_memory_size + 1, in_mem.rows(), arrays_memory_size) = in_mem
				.block(0, 3 + arrays_memory_size + 1, in_mem.rows(), arrays_memory_size)
					.unaryExpr([&](Scalar x) -> Scalar { return std::floor(m_random->uniform(1, m_max_int - 1)); }); // Initialize B

			// Create and initialize desired memory
			Matrix out_mem = in_mem;
			out_mem.col(2) = in_mem.block(0, 3, in_mem.rows(), arrays_memory_size) * 
				in_mem.block(0, 3 + arrays_memory_size + 1, in_mem.rows(), arrays_memory_size).transpose();
			for (Matrix::Index r = 0; r < out_mem.rows(); ++r)
				out_mem(r, 2) = Scalar(positive_mod<size_t>((size_t)out_mem(r, 2), m_max_int));

			// Cut out from the cost calculation the memory part that does not make part of the expected output
			Matrix mask = Task::init_mask(); //[3, max_int - 1]
			mask.leftCols(3)  = RowVector::Zero(3);
			
			return std::make_tuple(in_mem, out_mem, mask, Matrix::Zero(m_batch_size, m_max_int), Task::init_regs());
		}
	};
	REGISTERED_TASK(TaskProduct, "product")
}
}