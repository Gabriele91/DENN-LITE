#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include <iterator>

namespace Denn
{
namespace NRam
{

	Task::Task(size_t batch_size, size_t max_int, size_t n_regs, Random& random)
	: m_batch_size(batch_size)
	, m_max_int(max_int)
	, m_n_regs(n_regs)
	, m_random(random)
	{
	}
	
	//build dataset
	MemoryTuple Task::operator()() 
	{ 
		return {};
	}
	
	//Initialize R registers to zero (i.e. set P(x = 0) = 1.0).
	Matrix Task::init_regs() const { return Matrix::Zero(m_batch_size, m_n_regs); }
	
	//info
	size_t  Task::get_batch_size()    const { return m_batch_size; }
	size_t  Task::get_max_int()       const { return m_max_int; }
	size_t  Task::get_num_regs()      const { return m_n_regs; }
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
		, Random& random
	)
	{
		//find
		auto it = t_map().find(name);
		//return
		return it == t_map().end() ? nullptr : it->second(batch_size, max_int, n_regs, random);
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