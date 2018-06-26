#pragma once
#include "Config.h"


namespace Denn
{
	class DataSet
	{
    public:
		virtual void* ptr_features_vector() const = 0;
		virtual void* ptr_features(size_t depth = 0) const = 0;
		virtual void* ptr_mask()     const = 0;
		virtual void* ptr_labels()   const  = 0;

		virtual void* data_features_vector() const = 0;
		virtual void* data_features(size_t depth = 0) const = 0;
		virtual void* data_mask()     const = 0;
		virtual void* data_labels()   const = 0;

		virtual size_t features_rows(size_t depth = 0) const = 0;
		virtual size_t features_cols(size_t depth = 0) const = 0;
		virtual size_t features_depth() const = 0;

		virtual size_t mask_rows() const = 0;
		virtual size_t mask_cols() const  = 0;

		virtual size_t labels_rows() const  = 0;
		virtual size_t labels_cols() const  = 0;
        
		virtual DataType get_data_type() const { return DataType::DT_UNKNOWN;  }

		//auto cast
		virtual const Matrix&  features(size_t depth = 0) const
		{
			return *((const Matrix*)(ptr_features(depth)));
		}
		virtual const std::vector < Matrix >&  features_vector(size_t depth = 0) const
		{
			return *((const std::vector < Matrix >*)(ptr_features_vector()));
		}
		virtual const Matrix&  mask() const
		{
			return *((const Matrix*)(ptr_mask()));
		}
		virtual const Matrix&  labels() const
		{
			return *((const Matrix*)(ptr_labels()));
		}
		virtual std::vector < Matrix >&  features_vector()
		{
			return *((std::vector < Matrix >*)(ptr_features_vector()));
		}
		virtual Matrix&  features(size_t depth = 0)
		{
			return *((Matrix*)(ptr_features(depth)));
		}
		virtual Matrix&  mask()
		{
			return *((Matrix*)(ptr_mask()));
		}
		virtual Matrix&  labels()
		{
			return *((Matrix*)(ptr_labels()));
		}

		template < typename ScalarType = Scalar > 
		std::vector < Denn::MatrixT< ScalarType > >&  features_as_vector_type() 
		{
			return *((std::vector < Denn::MatrixT< ScalarType > >*)(ptr_features_vector()));
		}

		template < typename ScalarType = Scalar > 
		Denn::MatrixT< ScalarType >&  features_as_type(size_t depth=0) 
		{
			return *((Denn::MatrixT< ScalarType >*)(ptr_features(depth)));
		}
		template < typename ScalarType = Scalar > 
		Denn::MatrixT< ScalarType >&  labels_as_type()
		{
			return *((Denn::MatrixT< ScalarType >*)(ptr_labels()));
		}

		void resize_depth(size_t size)
		{
			features_vector().resize(size);
		}
		void features_conservative_resize(size_t rows, size_t cols, size_t depth)
		{
			resize_depth(depth);
			for(auto& matrix : features_vector())
			{
				matrix.conservativeResize(rows,cols);
			}
		}
	};

	template < typename ScalarType >
	class DataSetX : public DataSet
	{
    public:
		std::vector < Denn::MatrixT< ScalarType > > m_features;
		Denn::MatrixT< ScalarType > m_mask;
		Denn::MatrixT< ScalarType > m_labels;

		virtual void* ptr_features_vector()           const override { return (void*)&m_features;         }
		virtual void* ptr_features(size_t depth = 0)  const override { return (void*)&m_features[depth];  }
		virtual void* ptr_mask()                      const override { return (void*)&m_mask;             }
		virtual void* ptr_labels()	                  const override { return (void*)&m_labels;           }

		virtual void* data_features_vector()          const override { return (void*)m_features.data();         }
		virtual void* data_features(size_t depth = 0) const override { return (void*)m_features[depth].data(); }
		virtual void* data_mask()                     const override { return (void*)m_mask.data();             }
		virtual void* data_labels()                   const override { return (void*)m_labels.data();           }


		virtual size_t features_rows(size_t depth = 0) const override
        {
            return m_features[depth].rows();
        }
		virtual size_t features_cols(size_t depth = 0) const override
        {
            return m_features[depth].cols();
        }
		virtual size_t features_depth() const override
        {
            return m_features.size();
        }

		virtual size_t mask_rows() const override
        {
            return m_mask.rows();
        }
		virtual size_t mask_cols() const override
        {
            return m_mask.cols();
        }
		
		virtual size_t labels_rows() const override
        {
            return m_labels.rows();
        }
		virtual size_t labels_cols() const override
        {
            return m_labels.cols();
        }

		virtual DataType get_data_type() const override { return Denn::get_data_type<ScalarType>();  }
	};

	using DataSetScalar = DataSetX<Scalar>;
	using DataSetF 	    = DataSetX<float>;
	using DataSetD      = DataSetX<double>;
}