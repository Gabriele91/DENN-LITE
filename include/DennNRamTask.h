//
// Created by Valerio Belli on 22/11/17.
//
#pragma once
#include "Config.h"
#include "DennRandom.h"

namespace Denn
{
namespace NRam 
{
	//memory struct
	using TaskTuple = std::tuple<Matrix, Matrix, Matrix, Matrix, size_t, size_t>;
	using MemoryTuple = std::tuple<Matrix, Matrix, Matrix, Matrix>;

	//Task
	class Task : public std::enable_shared_from_this< Task >
	{
    public:

		//ref to Crossover
		using SPtr = std::shared_ptr<Task>;
		using DifficultyGrade = std::tuple<size_t, size_t>;

		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }

		//void init
		Task();

		//init
		Task(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda
		, Random& random
		);

		//delete
		virtual ~Task();

		//build dataset
		TaskTuple create_batch(const size_t current_generation, const Scalar& best_m_eval);

		/**
		* Initialize R registers to zero (i.e. set P(x = 0) = 1.0).
		*
		* @param R Registers number
		* @return MatrixList
		*/
		Matrix init_regs() const;

		/**
		* Initialize mask of nram evalauation
		* 
		* @param Memory size
		* @return Matrix of ones
		*/
		Matrix init_mask() const;

		//info
		size_t  get_batch_size()    const;
		size_t  get_max_int()       const;
		size_t  get_num_regs()      const;
		size_t  get_timestemps()    const;
		size_t  get_min_difficulty() const;
		size_t  get_max_difficulty() const;
		size_t  get_current_difficulty() const;
		size_t  get_step_gen_change_difficulty() const;
		DifficultyGrade get_difficulty_grade();
		Random* get_random_engine() const;

		// Clone a this task
		virtual Task::SPtr clone() const;


    protected:

		virtual MemoryTuple operator()();

		size_t  m_batch_size;
		size_t  m_max_int;
		size_t  m_n_regs;
		size_t  m_timesteps;
		size_t 	previous_generation;
		size_t  stall_generations;

		bool    m_use_difficulty;
		size_t  m_min_difficulty;
		size_t  m_max_difficulty;
		size_t  m_current_difficulty;
		size_t 	m_step_gen_change_difficulty;
		Scalar  m_old_best_context_eval;
		Scalar  m_change_difficulty_lambda;

		Matrix  m_in_mem;
		Matrix  m_out_mem;
		Matrix  m_mask;
		Matrix  m_regs;

		std::vector<DifficultyGrade> m_difficulty_grades;

		Random* m_random;
	};

	//class factory of tasks
	class TaskFactory
	{

	public:
		//Gate classes map
		typedef Task::SPtr(*CreateObject)(size_t batch_size, size_t max_int, size_t n_regs, size_t timesteps, size_t min_difficulty, size_t max_difficulty, size_t step_gen_change_difficulty, Scalar change_difficulty_lambda, Random& random);

		//public
		static Task::SPtr create
		(
			  const std::string& name
			, size_t batch_size
			, size_t max_int
			, size_t n_regs
			, size_t timesteps
			, size_t min_difficulty
			, size_t max_difficulty
			, size_t step_gen_change_difficulty
			, Scalar change_difficulty_lambda
			, Random& random
		);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_tasks();
		static std::string names_of_tasks(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};
	
	//class used for static registration of a object class
	template<class T>
	class TaskItem
	{

		static Task::SPtr create
		(
		  size_t batch_size
		, size_t max_int
		, size_t n_regs
		, size_t timesteps
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Scalar change_difficulty_lambda
		, Random& random
		)
		{
			return (std::make_shared< T >(batch_size, max_int, n_regs, timesteps, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random))->get_ptr();
		}

		TaskItem(const std::string& name, size_t size)
		{
			TaskFactory::append(name, TaskItem<T>::create, size);
		}

	public:


		static TaskItem<T>& instance(const std::string& name, size_t size)
		{
			static TaskItem<T> objectItem(name, size);
			return objectItem;
		}

	};

	#define REGISTERED_TASK(class_,name_)\
    namespace\
    {\
        static const auto& _Denn_ ## class_ ## _TaskItem= Denn::NRam::TaskItem<class_>::instance( name_, sizeof(class_) );\
    }
}
}