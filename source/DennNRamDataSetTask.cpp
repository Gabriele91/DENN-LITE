#include "DennAlgorithm.h"
#include "DennNRamDatasetTask.h"
#include "DennDump.h"

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
		m_task_train = task.clone();
		m_task_validation = task.clone();
		m_task_test = task.clone();
		//|FAKE|
		m_fake_header.m_n_batch     = 1;
		m_fake_header.m_n_classes   = 1;
		m_fake_header.m_n_features  = 1;
		//|X|
		m_fake_train_header.m_n_row = m_task_train->get_batch_size();

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

	bool DataSetTask::can_stream() const
	{ 
		return false; 
	}
	///////////////////////////////////////////////////////////////////
	// READ TEST SET
	bool DataSetTask::read_test(const DennAlgorithm& algorithm, DataSet& t_out) 
	{
		auto& dataset          = *((DataSetScalar*)&t_out);
		VariantRef error_rate = dataset.get_metadata("error_rate");
		const auto& data		   = m_task_train->create_batch(algorithm,
			error_rate.get_type() == VR_NONE ? Scalar(0.0) : error_rate.get<Scalar>(), "test");
		dataset.m_features 		 = std::get<0>(data);
		dataset.m_labels   		 = std::get<1>(data);
		dataset.m_mask     		 = std::get<2>(data);
		dataset.m_metadata["max_int"] = int(std::get<3>(data));
		dataset.m_metadata["time_steps"] = int(std::get<4>(data));
        dataset.m_mask_error     = std::get<5>(data);
        m_fake_header.m_n_classes = std::get<3>(data);
		m_fake_header.m_n_features = std::get<3>(data);
		return true;
	}

	///////////////////////////////////////////////////////////////////
	// READ VALIDATION SET
	bool DataSetTask::read_validation(const DennAlgorithm& algorithm, DataSet& t_out) 
	{
		auto& dataset          = *((DataSetScalar*)&t_out);
		VariantRef error_rate = dataset.get_metadata("error_rate");
		const auto& data		   = m_task_train->create_batch(algorithm,
			error_rate.get_type() == VR_NONE ? Scalar(0.0) : error_rate.get<Scalar>(), "validation");
		dataset.m_features 		 = std::get<0>(data);
		dataset.m_labels   		 = std::get<1>(data);
		dataset.m_mask     		 = std::get<2>(data);
		dataset.m_metadata["max_int"] = int(std::get<3>(data));
		dataset.m_metadata["time_steps"] = int(std::get<4>(data));
        dataset.m_mask_error     = std::get<5>(data);
		m_fake_header.m_n_classes = std::get<3>(data);
		m_fake_header.m_n_features = std::get<3>(data);
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
		auto& dataset            = *((DataSetScalar*)&t_out);
		VariantRef error_rate = dataset.get_metadata("error_rate");
		const auto& data		   = m_task_train->create_batch(algorithm,
			error_rate.get_type() == VR_NONE ? Scalar(0.0) : error_rate.get<Scalar>(), "train");
		dataset.m_features 		 = std::get<0>(data);
		dataset.m_labels   		 = std::get<1>(data);
		dataset.m_mask     		 = std::get<2>(data);
		dataset.m_metadata["max_int"] = int(std::get<3>(data));
		dataset.m_metadata["time_steps"] = int(std::get<4>(data));
		dataset.m_mask_error          = std::get<5>(data);
		m_fake_header.m_n_classes = std::get<3>(data);
		m_fake_header.m_n_features = std::get<3>(data);
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
