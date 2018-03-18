#pragma once
#include "Config.h"
#include "DennVariant.h"

namespace Denn
{
	//Metadata
	using MetaData = std::map< std::string, Variant >;
	//Abstract Dataset
	class DataSet
	{
    public:
		virtual void* ptr_features() const 	 = 0;
		virtual void* ptr_mask()     const 	 = 0;
		virtual void* ptr_mask_error() const = 0;
		virtual void* ptr_labels()   const 	 = 0;

		virtual void* data_features() const  = 0;
		virtual void* data_mask()     const  = 0;
		virtual void* data_mask_error() const  = 0;
		virtual void* data_labels()   const  = 0;

		virtual size_t features_rows() const = 0;
		virtual size_t features_cols() const = 0;

		virtual size_t mask_rows() const     = 0;
		virtual size_t mask_cols() const  	 = 0;

		virtual size_t mask_error_rows() const = 0;
		virtual size_t mask_error_cols() const = 0;

		virtual size_t labels_rows() const   = 0;
		virtual size_t labels_cols() const   = 0;
        
		virtual DataType get_data_type() const { return DataType::DT_UNKNOWN;  }

		//get metadata
		virtual const MetaData& get_metadata() const = 0;
		//get metadata
		virtual VariantRef get_metadata(const std::string& key) const = 0;

		//auto cast
		const Matrix&  features() const
		{
			return *((const Matrix*)(ptr_features()));
		}
		const Matrix&  mask() const
		{
			return *((const Matrix*)(ptr_mask()));
		}
		const Matrix&  mask_error() const
		{
			return *((const Matrix*)(ptr_mask_error()));
		}
		const Matrix&  labels() const
		{
			return *((const Matrix*)(ptr_labels()));
		}
		Matrix&  features()
		{
			return *((Matrix*)(ptr_features()));
		}
		Matrix&  mask()
		{
			return *((Matrix*)(ptr_mask()));
		}
		Matrix&  mask_error()
		{
			return *((Matrix*)(ptr_mask_error()));
		}
		Matrix&  labels()
		{
			return *((Matrix*)(ptr_labels()));
		}

		template < typename ScalarType = Scalar > 
		Denn::MatrixT< ScalarType >&  features_as_type() 
		{
			return *((Denn::MatrixT< ScalarType >*)(ptr_features()));
		}
		template < typename ScalarType = Scalar > 
		Denn::MatrixT< ScalarType >&  labels_as_type()
		{
			return *((Denn::MatrixT< ScalarType >*)(ptr_labels()));
		}

		//delete metadata
		virtual ~DataSet(){}
	};
	//Dataset
	template < typename ScalarType >
	class DataSetX : public DataSet
	{
    public:
		//values
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_features;
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_mask;
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_mask_error;
		Eigen::Matrix< ScalarType, Eigen::Dynamic, Eigen::Dynamic > m_labels;
		//metadata
		MetaData m_metadata; 
		//pure virtual methods
		virtual void* ptr_features() const { return (void*)&m_features;       }
		virtual void* ptr_mask()     const { return (void*)&m_mask;           }
		virtual void* ptr_mask_error()     const { return (void*)&m_mask_error;           }
		virtual void* ptr_labels()	 const { return (void*)&m_labels;         }

		virtual void* data_features() const { return (void*)m_features.data(); }
		virtual void* data_mask()     const { return (void*)m_mask.data();     }
		virtual void* data_mask_error()     const { return (void*)m_mask_error.data();     }
		virtual void* data_labels()   const { return (void*)m_labels.data();   }


		virtual size_t features_rows() const
        {
            return m_features.rows();
        }
		virtual size_t features_cols() const
        {
            return m_features.cols();
        }

		virtual size_t mask_rows() const
        {
            return m_mask.rows();
        }
		virtual size_t mask_cols() const
        {
            return m_mask.cols();
        }

		virtual size_t mask_error_rows() const
        {
            return m_mask_error.rows();
        }
		virtual size_t mask_error_cols() const
        {
            return m_mask_error.cols();
        }
		
		virtual size_t labels_rows() const
        {
            return m_labels.rows();
        }
		virtual size_t labels_cols() const
        {
            return m_labels.cols();
        }

		//get metadata
		virtual const MetaData& get_metadata() const
		{
			return m_metadata;
		}
		//get metadata
		virtual VariantRef get_metadata(const std::string& key) const
		{
			//find (const)
			auto it = m_metadata.find(key);
			//return ref
			return it == m_metadata.end() ? VariantRef(/*none*/) : VariantRef( it->second );
		}

		//delete metadata
		virtual ~DataSetX(){}
		
		//data type
		virtual DataType get_data_type() const { return Denn::get_data_type<ScalarType>();  }
	};

	using DataSetScalar = DataSetX<Scalar>;
	using DataSetF 	    = DataSetX<float>;
	using DataSetD      = DataSetX<double>;
}