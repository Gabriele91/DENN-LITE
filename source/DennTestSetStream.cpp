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
		//allac cache (depth size 1 is need)
		m_cache_batch.resize_depth(1);
		//alloc
		m_batch.features_conservative_resize(
			  m_batch_size
			, m_dataset->get_main_header_info().m_n_features
			, m_cache_batch.features_depth()
		);
		//mask
		m_batch.mask().conservativeResize
		(
			  1
			, m_dataset->get_main_header_info().m_n_features
		);
		//init labels
		m_batch.labels().conservativeResize
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
		denn_assert(n_rows <= m_batch.features().size());
		//olready read
		const size_t c_features = m_dataset->get_main_header_info().m_n_features;
		const size_t c_labels = m_dataset->get_main_header_info().m_n_classes;
		size_t offset = 0;
		size_t n_read = 0;
		
		if (n_rows < m_batch.features().rows())
		{
			//put the last N values on the top
			for(int i = 0; i != m_batch.features_depth(); ++i)
			{
				m_batch.features(i) = Denn::shift_top(m_batch.features(i), n_rows);
			}
			m_batch.labels() = Denn::shift_top(m_batch.labels(), n_rows);
			//start to n_rows
			offset = m_batch.features().rows() - n_rows;
		}
		//copy last
		if (m_cache_rows_read < m_cache_batch.features().rows())
		{
			//remaning
			size_t remaning = m_cache_batch.features().rows() - m_cache_rows_read;
			size_t to_read = std::min(remaning, n_rows);
			//read			
			for(int i = 0; i != m_cache_batch.features_depth(); ++i)
			{
				m_batch.features(i).block(offset, 0, to_read, c_features) = m_cache_batch.features(i).block(m_cache_rows_read, 0, to_read, c_features);
			}
			m_batch.labels().block(offset, 0, to_read, c_labels) = m_cache_batch.labels().block(m_cache_rows_read, 0, to_read, c_labels);
			//move
			n_read += to_read;
			offset += to_read;
			m_cache_rows_read += to_read;
		}
		//first read?
		bool is_first_read = !m_cache_batch.features().rows();
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
				m_batch.mask() = m_cache_batch.mask();
			}
			//compute n rows to read
			size_t to_read = std::min<size_t>(m_cache_batch.features().rows(), read_remaning);
			//alloc if need
			if(m_batch.features_depth()!=m_cache_batch.features_depth())
			{
				m_batch.features_conservative_resize(
					  m_batch_size
					, m_cache_batch.features_cols()
					, m_cache_batch.features_depth()
				);
			}
			//read
			for(int i = 0; i != m_cache_batch.features_depth(); ++i)
			{
				m_batch.features(i).block(offset, 0, to_read, c_features) = m_cache_batch.features(i).topRows(to_read);
			}
			m_batch.labels().block(offset, 0, to_read, c_labels) = m_cache_batch.labels().topRows(to_read);
			//move
			n_read += to_read;
			offset += to_read;
			m_cache_rows_read += to_read;
		}
		//get
		return m_batch;
	}
}