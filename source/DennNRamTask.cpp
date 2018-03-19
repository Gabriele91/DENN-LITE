#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include <iterator>

namespace Denn
{
namespace NRam
{

	Task::Task()
	: m_batch_size(0)
	, m_max_int(0)
	, m_n_regs(0)
	, m_timesteps(0)
	, m_sequence_size(-1)
	, m_random(nullptr)
	, m_use_difficulty(false)
	, m_max_difficulty(0)
	, m_min_difficulty(0)
	, m_current_difficulty(0)
	, m_step_gen_change_difficulty(0)
	, m_change_difficulty_lambda(0)
	, m_previous_generation(0)
	, m_stall_generations(0)
	{}
	Task::Task(size_t batch_size, size_t max_int, size_t n_regs, size_t timesteps, int sequence_size, size_t min_difficulty, size_t max_difficulty, size_t step_gen_change_difficulty, Scalar change_difficulty_lambda, Random& random)
	: m_batch_size(batch_size)
	, m_max_int(max_int)
	, m_n_regs(n_regs)
	, m_timesteps(timesteps)
	, m_sequence_size(sequence_size)
	, m_random(&random)
	, m_use_difficulty(max_int == 0 && timesteps == 0 && min_difficulty > 0)
	, m_max_difficulty(std::max(min_difficulty,max_difficulty))
	, m_min_difficulty(std::min(min_difficulty, max_difficulty))
	, m_current_difficulty(m_min_difficulty)
	, m_step_gen_change_difficulty(step_gen_change_difficulty)
	, m_change_difficulty_lambda(change_difficulty_lambda)
	, m_previous_generation(0)
	, m_stall_generations(0)
	{
	}

	//delete
	Task::~Task()
	{
	}

	//build dataset
	MemoryTuple Task::operator()() 
	{ 
		return {};
	}
	
	//Initialize R registers to zero (i.e. set P(x = 0) = 1.0).
	Matrix Task::init_regs() const { return Matrix::Zero(m_batch_size, m_n_regs); }
	
	//init mask
	Matrix Task::init_mask() const { return Matrix::Ones(1, m_max_int); }

	TaskTuple Task::create_batch(const size_t& current_generation, const Scalar& error_rate) 
	{
		const bool force_change_difficulty = error_rate <= m_change_difficulty_lambda;
		
		m_stall_generations += current_generation - m_previous_generation;
		m_previous_generation = current_generation;

		if (m_use_difficulty &&
					(current_generation == 0 
					 || (force_change_difficulty 
							 && m_stall_generations >= m_step_gen_change_difficulty)
					)
		)
		{
			// Select update type
			Scalar random_number = m_random->uniform(0, 1);

			// cases
			if (random_number <= 0.1)
			{
				m_current_difficulty = m_random->irand(m_min_difficulty, m_max_difficulty + 1);
			}
			else
			{
				// Geometric value
				int number_e = m_random->geometric(0.5);

				// Value D
				int D_plus_e_difficulty = clamp<int>(m_current_difficulty + number_e, m_min_difficulty, m_max_difficulty);

				//cases
				if (random_number <= 0.35)
				{
					m_current_difficulty = m_random->irand(m_min_difficulty, D_plus_e_difficulty + 1);
				}
				else
				{
					m_current_difficulty = D_plus_e_difficulty;
				}
			}
			
			// Set task difficulty parameters
			int current_difficulty = clamp<int>(m_current_difficulty - 1,0, m_difficulty_grades.size() - 1);
			DifficultyGrade difficulty_params =  m_difficulty_grades[current_difficulty];
			m_max_int = std::get<0>(difficulty_params);
			m_timesteps = std::get<1>(difficulty_params);
			m_sequence_size = std::get<2>(difficulty_params);

			// Generate memories
			const auto& mems = (*this)();
			m_in_mem = std::get<0>(mems);
			m_out_mem = std::get<1>(mems);
			m_mask = std::get<2>(mems);
			m_error_m = std::get<3>(mems);
			m_regs = std::get<4>(mems);

			// Reset the generation counter which indicates how many generation is used the same difficulty
			m_stall_generations = 0; 
		}
		else if(!m_use_difficulty && (current_generation == 0 || m_stall_generations >= m_step_gen_change_difficulty))
		{
			// Generate memories
			const auto& mems = (*this)();
			m_in_mem = std::get<0>(mems);
			m_out_mem = std::get<1>(mems);
			m_mask = std::get<2>(mems);
			m_error_m = std::get<3>(mems);
			m_regs = std::get<4>(mems);

			// Reset the generation counter which indicates how many generation is used the same difficulty
			m_stall_generations = 0;
		}

		return std::make_tuple(m_in_mem, m_out_mem, m_mask, m_max_int, m_timesteps, m_error_m);
	}

	//info
	size_t  Task::get_batch_size()    									const { return m_batch_size; }
	size_t  Task::get_max_int()       									const { return m_max_int; }
	size_t  Task::get_num_regs()      									const { return m_n_regs; }
	size_t  Task::get_min_difficulty()  								const { return m_min_difficulty; }
	size_t  Task::get_max_difficulty()  								const { return m_max_difficulty; }
	size_t  Task::get_timestemps()  										const { return m_timesteps; }
	int  		Task::get_sequence_size()  									const { return m_sequence_size; }
	size_t  Task::get_current_difficulty()  						const { return m_current_difficulty; }
	size_t  Task::get_step_gen_change_difficulty()  		const { return m_step_gen_change_difficulty; }
	Task::DifficultyGrade Task::get_difficulty_grade() 				{ return {}; }
	Random* Task::get_random_engine() 									const { return m_random; }

	// Clone a this task
	Task::SPtr Task::clone() const
	{
		return std::make_shared<Task>(*this);
	}

	//map
	static std::map< std::string, NRam::TaskFactory::CreateObject >& t_map()
	{
		static std::map< std::string, NRam::TaskFactory::CreateObject > t_map;
		return t_map;
	}
	//public
	Task::SPtr TaskFactory::create
	(
		  const std::string& name
		, size_t batch_size
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
	{
		//find
		auto it = t_map().find(name);
		//return
		return it == t_map().end() ? nullptr : it->second(batch_size, max_int, n_regs, timesteps, sequence_size, min_difficulty, max_difficulty, step_gen_change_difficulty, change_difficulty_lambda, random);
	}
	void TaskFactory::append(const std::string& name, TaskFactory::CreateObject fun, size_t size)
	{
		//add
		t_map()[name] = fun;
	}
	//list of methods
	std::vector< std::string > TaskFactory::list_of_tasks()
	{
		std::vector< std::string > list;
		for (const auto & pair : t_map()) list.push_back(pair.first);
		return list;
	}
	std::string TaskFactory::names_of_tasks(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_tasks();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	bool TaskFactory::exists(const std::string& name)
	{
		//find
		auto it = t_map().find(name);
		//return
		return it != t_map().end();
	}
}
}