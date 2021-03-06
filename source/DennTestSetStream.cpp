#include "DennTestSetStream.h"

namespace Denn
{
	//start
	void TestSetStream::start_read_batch(size_t batch_size, size_t rows_offset)
	{
		//start
		m_cache_rows_read = 0;
		m_batch_size = batch_size;
		m_batch_offset = rows_offset;
		m_dataset->start_read_batch();
		//init batch
		//int features
		m_batch.m_features.conservativeResize
		(
			m_batch_size
			, m_dataset->get_main_header_info().m_n_features
		);
		//mask
		m_batch.m_mask.conservativeResize
		(
			  1
			, m_dataset->get_main_header_info().m_n_features
		);
		//init labels
		m_batch.m_labels.conservativeResize
		(
			m_batch_size
			, m_dataset->get_main_header_info().m_n_classes
		);
		//read
		read_batch(m_batch_size);
	}

	//read
	DataSetScalar& TestSetStream::read_batch()
	{
		return read_batch(m_batch_offset);
	}

	//get last
	const DataSetScalar& TestSetStream::last_batch() const
	{
		return m_batch;
	}
	//read
	DataSetScalar& TestSetStream::read_batch(size_t n_rows)
	{
		//test
		denn_assert(n_rows <= m_batch.m_features.size());
		//olready read
		const size_t c_features = m_dataset->get_main_header_info().m_n_features;
		const size_t c_labels = m_dataset->get_main_header_info().m_n_classes;
		size_t offset = 0;
		size_t n_read = 0;
		#if 0
		//swift
		if (n_rows < m_batch.m_features.rows())
		{
			//put the last N values on the top
			m_batch.m_features = Denn::shift_by_rows(m_batch.m_features, m_batch.m_features.size() - n_rows);
			m_batch.m_labels = Denn::shift_by_rows(m_batch.m_labels, m_batch.m_labels.size() - n_rows);
			//start to n_rows
			offset = n_rows;
		}
		#else 
		if (n_rows < m_batch.m_features.rows())
		{
			//put the last N values on the top
			m_batch.m_features = Denn::shift_top(m_batch.m_features, n_rows);
			m_batch.m_labels = Denn::shift_top(m_batch.m_labels, n_rows);
			//start to n_rows
			offset = m_batch.m_features.rows() - n_rows;
		}
		#endif
		//copy last
		if (m_cache_rows_read < m_cache_batch.m_features.rows())
		{
			//remaning
			size_t remaning = m_cache_batch.m_features.rows() - m_cache_rows_read;
			size_t to_read = std::min(remaning, n_rows);
			//read
			m_batch.m_features.block(offset, 0, to_read, c_features) = m_cache_batch.m_features.block(m_cache_rows_read, 0, to_read, c_features);
			m_batch.m_labels.block(offset, 0, to_read, c_labels) = m_cache_batch.m_labels.block(m_cache_rows_read, 0, to_read, c_labels);
			//move
			n_read += to_read;
			offset += to_read;
			m_cache_rows_read += to_read;
		}
		//first read?
		bool is_first_read = !m_cache_batch.m_features.rows();
		//copy next
		while (n_read < n_rows)
		{
			//...
			size_t read_remaning = n_rows - n_read;
			//read
			m_cache_rows_read = 0;
			m_dataset->read_batch(m_cache_batch);
			//copy mask
			if(is_first_read)
			{
				m_batch.m_mask = m_cache_batch.m_mask;
			}
			//compute n rows to read
			size_t to_read = std::min<size_t>(m_cache_batch.m_features.rows(), read_remaning);
			//read
			m_batch.m_features.block(offset, 0, to_read, c_features) = m_cache_batch.m_features.topRows(to_read);
			m_batch.m_labels.block(offset, 0, to_read, c_labels) = m_cache_batch.m_labels.topRows(to_read);
			//move
			n_read += to_read;
			offset += to_read;
			m_cache_rows_read += to_read;
		}
		//get
		return m_batch;
	}
}