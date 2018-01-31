//
// Created by Valerio Belli on 22/11/17.
//
#pragma once
#include "Config.h"
#include "DennNRamTask.h"
#include "DennDatasetLoader.h"

namespace Denn
{
namespace NRam 
{
	//Task
	class DataSetTask : public DataSetLoader
	{
	public:
		///////////////////////////////////////////////////////////////////
		DataSetTask();
		DataSetTask(NRam::Task& task);
		///////////////////////////////////////////////////////////////////
		bool init(NRam::Task& task);
		///////////////////////////////////////////////////////////////////
		bool open(const std::string& path_file) override;

		bool is_open() const override;

		const DataSetHeader& get_main_header_info() const override;

		const DataSetTrainHeader& get_last_batch_info() const override;

		///////////////////////////////////////////////////////////////////
		// READ TEST SET
		bool read_test(const DennAlgorithm& algorithm, DataSet& t_out) override;

		///////////////////////////////////////////////////////////////////
		// READ VALIDATION SET
		bool read_validation(const DennAlgorithm& algorithm, DataSet& t_out) override;

		///////////////////////////////////////////////////////////////////
		// READ TRAINING SET
		bool start_read_batch() override;

		bool read_batch(const DennAlgorithm& algorithm, DataSet& t_out, bool loop = true) override;

		size_t number_of_batch_read() const override;

		void clear_batch_counter() override;

	protected:
		Task::SPtr 			 m_task;

		DataSetScalar    m_train;
		DataSetScalar    m_validation;
		DataSetScalar    m_test;

		DataSetHeader       m_fake_header;
		DataSetTrainHeader  m_fake_train_header;
	};

}
}
