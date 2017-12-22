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
	using MemoryTuple = std::tuple<Matrix, Matrix, Matrix, Matrix>;
	
	//Task
    class Task : public std::enable_shared_from_this< Task >
	{
    public:

		//ref to Crossover
		using SPtr = std::shared_ptr<Task>;

		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }

		//init
		Task(size_t batch_size, size_t max_int, size_t n_regs, Random& random);

		//delete
		virtual ~Task();

		//build dataset
		virtual MemoryTuple operator()();

        /**
         * Initialize R registers to zero (i.e. set P(x = 0) = 1.0).
         *
         * @param R Registers number
         * @return MatrixList
         */
		Matrix init_regs() const;

		/**
		 *  Initialize mask of nram evalauation
		 * @param Memory size
		 * @return Matrix of ones
		 */
		Matrix init_mask() const;

		//info
		size_t  get_batch_size()    const;
		size_t  get_max_int()       const;
		size_t  get_num_regs()      const;
		Random& get_random_engine() const;

    protected:

        size_t  m_batch_size;
        size_t  m_max_int;
        size_t  m_n_regs;
		Random& m_random;
    };

	//class factory of tasks
	class TaskFactory
	{

	public:
		//Gate classes map
		typedef Task::SPtr(*CreateObject)(size_t batch_size, size_t max_int, size_t n_regs, Random& random);

		//public
		static Task::SPtr create
		(
			  const std::string& name
			, size_t batch_size
			, size_t max_int
			, size_t n_regs
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
		, Random& random
		)
		{
			return (std::make_shared< T >(batch_size, max_int, n_regs, random))->get_ptr();
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