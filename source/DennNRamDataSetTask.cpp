#include "DennNRamDataSetTask.h"

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
		//
		m_fake_header.m_n_batch = 1;
		m_fake_header.m_n_classes = task.get_max_int();
		m_fake_header.m_n_features = task.get_max_int();
		//
		m_fake_train_header.m_n_row = task.get_batch_size();
		//init
		auto train = task();
		auto validation = task();
		auto test = task();

		m_train.m_features = std::get<0>(train);
		m_train.m_labels = std::get<1>(train);

		m_validation.m_features = std::get<0>(validation);
		m_validation.m_labels = std::get<1>(validation);

		m_test.m_features = std::get<0>(test);
		m_test.m_labels = std::get<1>(test);
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
	bool DataSetTask::read_test(DataSet& t_out) 
	{
		auto& d_out = (*((DataSetScalar*)& t_out));
		d_out = m_test;
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// READ VALIDATION SET
	bool DataSetTask::read_validation(DataSet& t_out) 
	{
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

	bool DataSetTask::read_batch(DataSet& t_out, bool loop) 
	{
		auto& d_out = (*((DataSetScalar*)& t_out));
		d_out = m_train;
		return true;
	}
} 
}
