#pragma once
#include "Config.h"
#include "DennIOFileWrapper.h"
#include "DennDataset.h"
#include "DennFilesystem.h"

namespace Denn
{

	ASPACKED(struct DataSetHeader
	{
		unsigned short m_version;
		int m_n_batch;
		int m_n_features;
		int m_n_classes;
		int m_type;
		int m_seed;
		float m_train_percentage;
		unsigned int m_test_offset;
		unsigned int m_validation_offset;
		unsigned int m_train_offset;

		DataType get_data_type() const
		{
			switch(m_type)
			{
				case 1:  return DataType::DT_FLOAT;
				case 2:  return DataType::DT_DOUBLE;
				default: return DataType::DT_UNKNOWN;
			}
		}
	});

	ASPACKED(struct DataSetTestHeader
	{
		unsigned int m_n_row;
	});

	ASPACKED(struct DataSetValidationHeader
	{
		unsigned int m_n_row;
	});

	ASPACKED(struct DataSetTrainHeader
	{
		unsigned int m_batch_id;
		unsigned int m_n_row;
	});


	class DataSetLoader
	{
	public:
		///////////////////////////////////////////////////////////////////
		using SPtr = std::shared_ptr< DataSetLoader >;
		///////////////////////////////////////////////////////////////////
		virtual bool open(const std::string& path_file) = 0;

		virtual bool is_open() const = 0;

		virtual const DataSetHeader& get_main_header_info() const  = 0;

		virtual const DataSetTrainHeader& get_last_batch_info() const  = 0;

		///////////////////////////////////////////////////////////////////
		// READ TEST SET
		virtual bool read_test(DataSet& t_out) = 0;

		///////////////////////////////////////////////////////////////////
		// READ VALIDATION SET
		virtual bool read_validation(DataSet& t_out) = 0;

		///////////////////////////////////////////////////////////////////
		// READ TRAINING SET
		virtual bool start_read_batch() = 0;

		virtual bool read_batch(DataSet& t_out, bool loop = true) = 0;

		virtual size_t number_of_batch_read() const = 0;

		virtual void clear_batch_counter() = 0;

	};


	template < class IO >
	class DataSetLoaderT : public DataSetLoader
	{
	public:

		DataSetLoaderT()
		{
		}

		DataSetLoaderT(const std::string& path_file)
		{
			open(path_file);
		}

		bool open(const std::string& path_file) override
		{
			if (m_file.open(path_file, "rb"))
			{
				//read header
				m_file.read(&m_header, sizeof(DataSetHeader), 1);
				//start to read batch
				m_n_batch_read = 0;
				//ok
				return true;
			}
			return false;
		}

		bool is_open() const override
		{
			return m_file.is_open();
		}

		const DataSetHeader& get_main_header_info() const override
		{
			return m_header;
		}

		const DataSetTrainHeader& get_last_batch_info() const override
		{
			return m_train_header;
		}

		///////////////////////////////////////////////////////////////////
		// READ TEST SET
		bool read_test(DataSet& t_out) override
		{
			if (is_open())
			{
				//save file pos
				size_t cur_pos = m_file.tell();
				//set file to test offset 
				m_file.seek_set(m_header.m_test_offset);
				//read header
				m_file.read(&m_test_header, sizeof(DataSetTestHeader), 1);
				//read data
				bool status = read(t_out, m_test_header.m_n_row);
				//return back
				m_file.seek_set(cur_pos);
				//return
				return status;
			}
			return false;
		}

		///////////////////////////////////////////////////////////////////
		// READ VALIDATION SET
		bool read_validation(DataSet& t_out) override
		{
			if (is_open())
			{
				//save file pos
				size_t cur_pos = m_file.tell();
				//set file to validation offset 
				m_file.seek_set(m_header.m_validation_offset);
				//read header
				m_file.read(&m_val_header, sizeof(DataSetValidationHeader), 1);
				//read data
				bool status = read(t_out, m_val_header.m_n_row);
				//return back
				m_file.seek_set(cur_pos);
				//return
				return status;
			}
			return false;
		}

		///////////////////////////////////////////////////////////////////
		// READ TRAINING SET
		bool start_read_batch() override
		{
			if (is_open())
			{
				//set file to train offset 
				m_file.seek_set(m_header.m_train_offset);
				//ok
				return true;
			}
			return false;
		}

		bool read_batch(DataSet& t_out, bool loop = true) override
		{
			if (is_open())
			{
				//read header
				m_file.read(&m_train_header, sizeof(DataSetTrainHeader), 1);
				//read data
				bool status = read(t_out, m_train_header.m_n_row);
				//inc count 
				++m_n_batch_read;
				//if loop enable and batch is the last
				if (loop
					&& int(m_train_header.m_batch_id + 1) == m_header.m_n_batch)
				{
					//restart
					start_read_batch();
				}
				return status;
			}
			return false;
		}
		//number of batch read
		size_t number_of_batch_read() const override
		{
			return m_n_batch_read;
		}

		void clear_batch_counter() override
		{
			m_n_batch_read = 0;
		}
		///////////////////////////////////////////////////////////////////

	protected:

		bool read(DataSet& t_out, const unsigned int size)
		{
			if (t_out.get_data_type() == m_header.get_data_type())
			{
				switch(t_out.get_data_type())
				{
					case DataType::DT_FLOAT:  return template_read(*((DataSetX<float>*)(&t_out)),size);
					case DataType::DT_DOUBLE: return template_read(*((DataSetX<double>*)(&t_out)),size);
					case DataType::DT_LONG_DOUBLE: return template_read(*((DataSetX<long double>*)(&t_out)),size);
					default: return false;
				}
			}
			else 
			{
				switch(m_header.get_data_type())
				{
					case DataType::DT_FLOAT:
					{
						DataSetX<float> t_float;
						bool success = template_read(t_float,size);
						switch(t_out.get_data_type())
						{
							case DataType::DT_DOUBLE:
							{
								t_out.features_as_type<double>() = t_float.features_as_type<float>().cast<double>();
								t_out.labels_as_type<double>()   = t_float.labels_as_type<float>().cast<double>();
							}
							break;
							case DataType::DT_LONG_DOUBLE: 
							{
								t_out.features_as_type<long double>() = t_float.features_as_type<float>().cast<long double>();
								t_out.labels_as_type<long double>()   = t_float.labels_as_type<float>().cast<long double>();
							}
							break;
							default: return false;
						}
						return success;
					}
					break;
					case DataType::DT_DOUBLE:
					{
						DataSetX<double> t_double;
						bool success = template_read(t_double,size);
						switch(t_out.get_data_type())
						{
							case DataType::DT_FLOAT:
							{
								t_out.features_as_type<float>() = t_double.features_as_type<double>().cast<float>();
								t_out.labels_as_type<float>()   = t_double.labels_as_type<double>().cast<float>();
							}
							break;
							case DataType::DT_LONG_DOUBLE: 
							{
								t_out.features_as_type<long double>() = t_double.features_as_type<double>().cast<long double>();
								t_out.labels_as_type<long double>()   = t_double.labels_as_type<double>().cast<long double>();
							}
							break;
							default: return false;
						}
						return success;
					}
					break;
					case DataType::DT_LONG_DOUBLE:
					{
						DataSetX<long double> t_long_double;
						bool success = template_read(t_long_double,size);
						switch(t_out.get_data_type())
						{
							case DataType::DT_FLOAT:
							{
								t_out.features_as_type<float>() = t_long_double.features_as_type<long double>().cast<float>();
								t_out.labels_as_type<float>()   = t_long_double.labels_as_type<long double>().cast<float>();
							}
							break;
							case DataType::DT_DOUBLE: 
							{
								t_out.features_as_type<double>() = t_long_double.features_as_type<long double>().cast<double>();
								t_out.labels_as_type<double>()   = t_long_double.labels_as_type<long double>().cast<double>();
							}
							break;
							default: return false;
						}
						return success;
					}
					break;
					default: return false;
				}
			}
		}

		template < typename ScalarType >
		bool template_read(DataSetX<ScalarType>& t_out, const unsigned int size)
		{
			//equal type?
			if (t_out.get_data_type() != m_header.get_data_type()) return false;
			//alloc output 
			//data are in row-major layour then the shape is traspose
			t_out.m_features.resize(m_header.m_n_features, size);
			//read features
			m_file.read
			(
				 (void*)(t_out.data_features())
				, t_out.features_rows()*t_out.features_cols() * sizeof(ScalarType)
				, 1
			);
			//to column-major
			t_out.m_features.transposeInPlace();
			//alloc output
			//data are in row-major layour then the shape is traspose
			t_out.m_labels.resize(m_header.m_n_classes, size);
			//read labels
			m_file.read
			(
				  (void*)(t_out.data_labels())
				, t_out.labels_rows()*t_out.labels_cols() * sizeof(ScalarType)
				, 1
			);
			//to column-major
			t_out.m_labels.transposeInPlace();
			//
			return true;
		}

		IO m_file;
		size_t 					m_n_batch_read;
		DataSetHeader           m_header;
		DataSetTestHeader       m_test_header;
		DataSetValidationHeader m_val_header;
		DataSetTrainHeader      m_train_header;
	};

	using DataSetLoaderSTD = DataSetLoaderT< IOFileWrapper::std_file    >;
	using DataSetLoaderGZ  = DataSetLoaderT< IOFileWrapper::zlib_file<> >;

	inline static DataSetLoader::SPtr get_datase_loader(const std::string& path)
	{
		//ptr out
		DataSetLoader::SPtr dbloader(nullptr);
		//out put
		std::string extension = Filesystem::get_extension(path);
		     if (extension == ".gz")    dbloader = std::dynamic_pointer_cast<DataSetLoader>(std::make_shared<DataSetLoaderGZ>(path));
		else if (extension == ".data")  dbloader = std::dynamic_pointer_cast<DataSetLoader>(std::make_shared<DataSetLoaderSTD>(path));
		//return
		return dbloader;
	}
}