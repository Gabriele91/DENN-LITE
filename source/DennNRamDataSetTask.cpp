#include "DennAlgorithm.h"
#include "DennNRamDatasetTask.h"

namespace Denn
{
namespace NRam 
{
	///////////////////////////////////////////////////////////////////
	DataSetTask::DataSetTask() {}
	DataSetTask::DataSetTask(NRam::Task& task) { init(task); }
	///////////////////////////////////////////////////////////////////
	bool DataSetTask::init(NRam::Task& task)
	{
		m_task = (Task::SPtr) &task;
		
		auto train = m_task->create_batch(0);
		auto validation = m_task->create_batch(0);
		auto test = m_task->create_batch(0);

		m_train.m_features 					= std::get<0>(train);
		m_train.m_labels   					= std::get<1>(train);
		m_train.m_mask     					= std::get<2>(train);
		m_train.m_meta["max_int"]   = int(std::get<4>(train));
		m_train.m_meta["timesteps"] = int(std::get<5>(train));

		m_validation.m_features 				 = std::get<0>(validation);
		m_validation.m_labels    				 = std::get<1>(validation);
		m_validation.m_mask     				 = std::get<2>(validation);
		m_validation.m_meta["max_int"]   = int(std::get<4>(validation));
		m_validation.m_meta["timesteps"] = int(std::get<5>(validation));

		m_test.m_features 				 = std::get<0>(test);
		m_test.m_labels   				 = std::get<1>(test);
		m_test.m_mask     				 = std::get<2>(test);
		m_test.m_meta["max_int"]   = int(std::get<4>(test));
		m_test.m_meta["timesteps"] = int(std::get<5>(test));

		//
		m_fake_header.m_n_batch = 1;
		m_fake_header.m_n_classes = std::get<4>(train);
		m_fake_header.m_n_features = std::get<4>(train);
		//
		m_fake_train_header.m_n_row = m_task->get_batch_size();

		return true;
	}
	///////////////////////////////////////////////////////////////////
	bool DataSetTask::open(const std::string& path_file) 
	{
		return true;
	}

	bool DataSetTask::is_open() const 
	{
		return true;
	}

	const DataSetHeader& DataSetTask::get_main_header_info() const 
	{
		return m_fake_header;
	}

	const DataSetTrainHeader& DataSetTask::get_last_batch_info() const 
	{
		return m_fake_train_header;
	}

	///////////////////////////////////////////////////////////////////
	// READ TEST SET
	bool DataSetTask::read_test(const DennAlgorithm& algorithm, DataSet& t_out) 
	{
		const auto& test 		   = m_task->create_batch(algorithm.current_generation());
		m_test.m_features 		 = std::get<0>(test);
		m_test.m_labels   		 = std::get<1>(test);
		m_test.m_mask     		 = std::get<2>(test);
		m_test.m_meta["max_int"] = int(std::get<4>(test));
		m_test.m_meta["timesteps"] = int(std::get<5>(test));

		m_fake_header.m_n_classes = std::get<4>(test);
		m_fake_header.m_n_features = std::get<4>(test);

		auto& d_out = (*((DataSetScalar*)& t_out));
		d_out = m_test;
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// READ VALIDATION SET
	bool DataSetTask::read_validation(const DennAlgorithm& algorithm, DataSet& t_out) 
	{
		const auto& validation 			= m_task->create_batch(algorithm.current_generation());
		m_validation.m_features 		= std::get<0>(validation);
		m_validation.m_labels   		= std::get<1>(validation);
		m_validation.m_mask 			  = std::get<2>(validation);
		m_validation.m_meta["max_int"] = int(std::get<4>(validation));
		m_validation.m_meta["timesteps"] = int(std::get<5>(validation));

		m_fake_header.m_n_classes = std::get<4>(validation);
		m_fake_header.m_n_features = std::get<4>(validation);

		auto& d_out = (*((DataSetScalar*)& t_out));
		d_out = m_validation;
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// READ TRAINING SET
	bool DataSetTask::start_read_batch() 
	{
		return true;
	}

	bool DataSetTask::read_batch(const DennAlgorithm& algorithm, DataSet& t_out, bool loop) 
	{
		const auto& train 			= m_task->create_batch(algorithm.current_generation());
		m_train.m_features 			= std::get<0>(train);
		m_train.m_labels   			= std::get<1>(train);
		m_train.m_mask     			= std::get<2>(train);
		m_train.m_meta["max_int"] = int(std::get<4>(train));
		m_train.m_meta["timesteps"] = int(std::get<5>(train));

		m_fake_header.m_n_classes = std::get<4>(train);
		m_fake_header.m_n_features = std::get<4>(train);

		auto& d_out = (*((DataSetScalar*)& t_out));
		d_out = m_train;
		return true;
	}

	size_t DataSetTask::number_of_batch_read() const 
	{
		return 1;
	}

	void DataSetTask::clear_batch_counter()
	{
		//none
	}
} 
}
