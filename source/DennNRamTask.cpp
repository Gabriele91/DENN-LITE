#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include <iterator>

namespace Denn
{
namespace NRam
{

	Task::Task(size_t batch_size, size_t max_int, size_t n_regs, size_t timesteps, size_t min_difficulty, size_t max_difficulty, size_t step_gen_change_difficulty, Random& random)
	: m_batch_size(batch_size)
	, m_max_int(max_int)
	, m_n_regs(n_regs)
	, m_timesteps(timesteps)
	, m_random(random)
	, m_max_difficulty(max_difficulty)
	, m_min_difficulty(min_difficulty)
	, m_current_difficulty(min_difficulty)
	, m_step_gen_change_difficulty(step_gen_change_difficulty)
	{
		m_difficulty_grades = {};
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

	TaskTuple Task::create_batch(const size_t current_generation) {
		if (current_generation != 0 && (current_generation + 1) % m_step_gen_change_difficulty == 0)
		{
			size_t number_e = m_random.geometric(0.5);

			size_t max_difficulty = m_max_difficulty <= m_difficulty_grades.size() ? m_max_difficulty : m_difficulty_grades.size();
			size_t D_plus_e_difficulty = (m_current_difficulty + number_e) < max_difficulty ? m_current_difficulty + number_e : max_difficulty;
			Scalar random_number = m_random.uniform(0, 1);
			if (random_number <= 0.1)
			{
				m_current_difficulty = m_random.index_rand(max_difficulty);
			}
			else if (0.1 < random_number && random_number <= 0.35)
			{
				m_current_difficulty = m_random.uniform(m_min_difficulty, D_plus_e_difficulty);
			}
			else
			{
				m_current_difficulty = D_plus_e_difficulty;
			}
		}

		// Set task difficulty parameters
		auto& difficulty_params = m_difficulty_grades[m_current_difficulty - 1];
		m_max_int = std::get<0>(difficulty_params);
		m_timesteps = std::get<1>(difficulty_params);

		const auto& mems = (*this)();
		Matrix in_mem = std::get<0>(mems);
		Matrix out_mem = std::get<1>(mems);
		Matrix mask = std::get<2>(mems);
		Matrix regs = std::get<3>(mems);
		return std::make_tuple(in_mem, out_mem, mask, regs, m_max_int, m_timesteps);
	}

	//info
	size_t  Task::get_batch_size()    const { return m_batch_size; }
	size_t  Task::get_max_int()       const { return m_max_int; }
	size_t  Task::get_num_regs()      const { return m_n_regs; }
	size_t  Task::get_min_difficulty()  const { return m_min_difficulty; }
	size_t  Task::get_max_difficulty()  const { return m_max_difficulty; }
	size_t  Task::get_current_difficulty()  const { return m_current_difficulty; }
	size_t  Task::get_step_gen_change_difficulty()  const { return m_step_gen_change_difficulty; }

	std::tuple<size_t, size_t> get_difficulty_grade() { return {}; }

	Random& Task::get_random_engine() const { return m_random; }

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