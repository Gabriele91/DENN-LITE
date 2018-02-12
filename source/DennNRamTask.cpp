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
	, m_random(nullptr)
	, m_use_difficulty(false)
	, m_max_difficulty(0)
	, m_min_difficulty(0)
	, m_current_difficulty(0)
	, m_step_gen_change_difficulty(0)
	{}
	Task::Task(size_t batch_size, size_t max_int, size_t n_regs, size_t timesteps, size_t min_difficulty, size_t max_difficulty, size_t step_gen_change_difficulty, Random& random)
	: m_batch_size(batch_size)
	, m_max_int(max_int)
	, m_n_regs(n_regs)
	, m_timesteps(timesteps)
	, m_random(&random)
	, m_use_difficulty(max_int == 0 && timesteps == 0 && min_difficulty > 0)
	, m_max_difficulty(std::max(min_difficulty,max_difficulty))
	, m_min_difficulty(std::min(min_difficulty, max_difficulty))
	, m_current_difficulty(min_difficulty)
	, m_step_gen_change_difficulty(step_gen_change_difficulty)
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

	TaskTuple Task::create_batch(const size_t current_generation) 
	{
		if (m_use_difficulty
          && (current_generation == 0 || !((current_generation + 1) % m_step_gen_change_difficulty))
		)
		{
			// Min/Max index
			int min_difficulty = clamp<int>(1, m_difficulty_grades.size(), m_min_difficulty);
			int max_difficulty = clamp<int>(min_difficulty, m_difficulty_grades.size(), m_max_difficulty);

			// Geometric value
			int number_e = m_random->geometric(0.5);
			
			// Value D
			int D_plus_e_difficulty = clamp<int>(m_current_difficulty + number_e, min_difficulty, max_difficulty);
			
			// Select update type
			volatile Scalar random_number = m_random->uniform(0, 1);
			
			// cases
			if (random_number <= 0.1)
			{
				m_current_difficulty = m_random->irand(m_min_difficulty, max_difficulty + 1);
			}
			else if (0.1 < random_number && random_number <= 0.35)
			{
				m_current_difficulty = m_random->irand(m_min_difficulty, D_plus_e_difficulty + 1);
			}
			else
			{
				m_current_difficulty = D_plus_e_difficulty;
			}

			// Set task difficulty parameters
			int current_difficulty = clamp<int>(m_current_difficulty - 1,0, m_difficulty_grades.size() - 1);
			DifficultyGrade difficulty_params =  m_difficulty_grades[current_difficulty];
			m_max_int = std::get<0>(difficulty_params);
			m_timesteps = std::get<1>(difficulty_params);

			// Generate memories
			const auto& mems = (*this)();
			m_in_mem = std::get<0>(mems);
			m_out_mem = std::get<1>(mems);
			m_mask = std::get<2>(mems);
			m_regs = std::get<3>(mems);
		}
		else if(!m_use_difficulty && current_generation == 0)
		{
				// Generate memories
				const auto& mems = (*this)();
				m_in_mem = std::get<0>(mems);
				m_out_mem = std::get<1>(mems);
				m_mask = std::get<2>(mems);
				m_regs = std::get<3>(mems);
		}

		return std::make_tuple(
			m_in_mem, 
			m_out_mem,
			m_mask, 
			m_regs, 
			m_max_int, 
			m_timesteps
		);
	}

	//info
	size_t  Task::get_batch_size()    const { return m_batch_size; }
	size_t  Task::get_max_int()       const { return m_max_int; }
	size_t  Task::get_num_regs()      const { return m_n_regs; }
	size_t  Task::get_min_difficulty()  const { return m_min_difficulty; }
	size_t  Task::get_max_difficulty()  const { return m_max_difficulty; }
	size_t  Task::get_timestemps()  const { return m_timesteps; }
	size_t  Task::get_current_difficulty()  const { return m_current_difficulty; }
	size_t  Task::get_step_gen_change_difficulty()  const { return m_step_gen_change_difficulty; }
	Task::DifficultyGrade Task::get_difficulty_grade() { return {}; }
	Random* Task::get_random_engine() const { return m_random; }

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
		, size_t min_difficulty
		, size_t max_difficulty
		, size_t step_gen_change_difficulty
		, Random& random
	)
	{
		//find
		auto it = t_map().find(name);
		//return
		return it == t_map().end() ? nullptr : it->second(batch_size, max_int, n_regs, timesteps, min_difficulty, max_difficulty, step_gen_change_difficulty, random);
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