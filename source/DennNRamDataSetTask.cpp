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

		m_train.features_vector().resize(1);
		m_train.features() = std::get<0>(train);
		m_train.labels()   = std::get<1>(train);
		m_train.mask()     = std::get<2>(train);

		m_validation.features_vector().resize(1);
		m_validation.features() = std::get<0>(validation);
		m_validation.labels()   = std::get<1>(validation);
		m_validation.mask()     = std::get<2>(validation);

		m_test.features_vector().resize(1);
		m_test.features() = std::get<0>(test);
		m_test.labels()   = std::get<1>(test);
		m_test.mask()     = std::get<2>(test);
		
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

	const DataSetTrainHeaderV2& DataSetTask::get_last_batch_info() const 
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
