#pragma once
#include "Config.h"


namespace Denn
{
	class DataSet
	{
    public:
		virtual void* ptr_features()  = 0;
		virtual void* ptr_labels()    = 0;

		virtual void* data_features() = 0;
		virtual void* data_labels()   = 0;

		virtual size_t features_rows() const = 0;
		virtual size_t features_cols() const  = 0;

		virtual size_t labels_rows() const  = 0;
		virtual size_t labels_cols() const  = 0;
        
		virtual DataType get_data_type() const { return DataType::DT_UNKNOWN;  }

		//auto cast
		template < typename ScalarType > 
		Denn::Matrix< ScalarType >&  features()
		{
			return *((Denn::Matrix< ScalarType >*)(ptr_features()));
		}
		template < typename ScalarType > 
		Denn::Matrix< ScalarType >&  labels()
		{
			return *((Denn::Matrix< ScalarType >*)(ptr_labels()));
		}
	};

	template < typename ScalarType >
	class DataSetX : public DataSet
	{
    public:
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_features;
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_labels;

		virtual void* ptr_features()	 { return (void*)&m_features;       }
		virtual void* ptr_labels()		 { return (void*)&m_labels;         }

		virtual void* data_features()    { return (void*)m_features.data(); }
		virtual void* data_labels()      { return (void*)m_labels.data();   }


		virtual size_t features_rows() const
        {
            return m_features.rows();
        }
		virtual size_t features_cols() const
        {
            return m_features.cols();
        }

		virtual size_t labels_rows() const
        {
            return m_labels.rows();
        }
		virtual size_t labels_cols() const
        {
            return m_labels.cols();
        }

		virtual DataType get_data_type() const { return Denn::GetDataType<ScalarType>();  }
	};

	using DataSetF = DataSetX<float>;
	using DataSetD = DataSetX<double>;
}