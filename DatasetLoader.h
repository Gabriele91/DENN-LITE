#pragma once
#pragma once 
#include "Config.h"
#include "IOFileWrapper.h"

namespace Denn
{
	enum DataSetType
	{
		DS_UNKNOWN = -1,
		DS_FLOAT   = 1,
		DS_DOUBLE  = 2
	};

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

	template < typename ScalarType >
	struct DataSetRaw
	{
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_features;
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_labels;
		//get type
		static int get_type_id() { return DataSetType::DS_UNKNOWN;  }
	};
	template<> inline int DataSetRaw< float >::get_type_id()  { return DataSetType::DS_FLOAT; };
	template<> inline int DataSetRaw< double >::get_type_id() { return DataSetType::DS_DOUBLE; };

	template < class IO >
	class DataSetLoader
	{

	public:

		DataSetLoader()
		{
		}

		DataSetLoader(const std::string& path_file)
		{
			open(path_file);
		}

		bool open(const std::string& path_file)
		{
			if (m_file.open(path_file, "rb"))
			{
				m_file.read(&m_header, sizeof(DataSetHeader), 1);
				return true;
			}
			return false;
		}

		bool is_open() const
		{
			return m_file.is_open();
		}

		const DataSetHeader& get_main_header_info() const
		{
			return m_header;
		}

		const DataSetTrainHeader& get_last_batch_info() const
		{
			return m_train_header;
		}

		///////////////////////////////////////////////////////////////////
		// READ TEST SET
		template < typename ScalarType >
		bool read_test(DataSetRaw< ScalarType > & t_out)
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
				bool status = read_raw(t_out, m_test_header.m_n_row);
				//return back
				m_file.seek_set(cur_pos);
				//return
				return status;
			}
			return false;
		}

		///////////////////////////////////////////////////////////////////
		// READ VALIDATION SET
		template < typename ScalarType >
		bool read_validation(DataSetRaw< ScalarType >& t_out)
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
				bool status = read_raw(t_out, m_val_header.m_n_row);
				//return back
				m_file.seek_set(cur_pos);
				//return
				return status;
			}
			return false;
		}

		///////////////////////////////////////////////////////////////////
		// READ TRAINING SET
		bool start_read_batch()
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

		template < typename ScalarType >
		bool read_batch(DataSetRaw< ScalarType >& t_out, bool loop = true)
		{
			if (is_open())
			{
				//read header
				m_file.read(&m_train_header, sizeof(DataSetTrainHeader), 1);
				//read data
				bool status = read_raw(t_out, m_train_header.m_n_row);
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
		///////////////////////////////////////////////////////////////////

	protected:

		template < typename ScalarType >
		bool read_raw(DataSetRaw< ScalarType >& t_out, const unsigned int size)
		{
			//equal type?
			if (t_out.get_type_id() != m_header.m_type) return false;
			//alloc output 
			//data are in row-major layour then the shape is traspose
			t_out.m_features.resize(m_header.m_n_features, size);
			//read features
			m_file.read
			(
				 (void*)(t_out.m_features.data())
				, t_out.m_features.rows()*t_out.m_features.cols() * sizeof(ScalarType)
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
				  (void*)(t_out.m_labels.data())
				, t_out.m_labels.rows()*t_out.m_labels.cols() * sizeof(ScalarType)
				, 1
			);
			//to column-major
			t_out.m_labels.transposeInPlace();
			//
			return true;
		}

		IO m_file;
		DataSetHeader           m_header;
		DataSetTestHeader       m_test_header;
		DataSetValidationHeader m_val_header;
		DataSetTrainHeader      m_train_header;
	};
}