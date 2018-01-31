//
//  DennVariant.h
//  HCubo
//
//  Created by Gabriele on 09/07/176.
//  Copyright � 2017 Gabriele. All rights reserved.
//
#pragma once
#include "Config.h"
#include <vector>

namespace Denn
{
	//Forward-declare 
	class Individual;
	class Population;
	class Variant;
	class VariantRef;
	//Types
	enum VariantType
	{
		VR_NONE,
		VR_BOOL,
		VR_CHAR,
		VR_SHORT,
		VR_INT,
		VR_LONG,
		VR_LONGLONG,

		VR_UCHAR,
		VR_USHORT,
		VR_UINT,
		VR_ULONG,
		VR_ULONGLONG,

		VR_FLOAT,
		VR_DOUBLE,
		VR_LONG_DOUBLE,



		//HEAP
		VR_FLOAT_MATRIX,
		VR_DOUBLE_MATRIX,
		VR_LONG_DOUBLE_MATRIX,

		VR_INDIVIDUAL,
		VR_POPULATION,

		VR_STD_VECTOR_SHORT,
		VR_STD_VECTOR_INT,
		VR_STD_VECTOR_LONG,
		VR_STD_VECTOR_LONGLONG,
		VR_STD_VECTOR_USHORT,
		VR_STD_VECTOR_UINT,
		VR_STD_VECTOR_ULONG,
		VR_STD_VECTOR_ULONGLONG,
		VR_STD_VECTOR_FLOAT,
		VR_STD_VECTOR_DOUBLE,
		VR_STD_VECTOR_LONG_DOUBLE,
		VR_STD_VECTOR_FLOAT_MATRIX,
		VR_STD_VECTOR_DOUBLE_MATRIX,
		VR_STD_VECTOR_LONG_DOUBLE_MATRIX,

		VR_C_STRING,
		VR_STD_STRING,
		VR_STD_VECTOR_STRING,

		//PTR
		VR_PTR
	};

	//get type
	template < class T > inline VariantType static_variant_type() { return VR_NONE; };
	//template specialization 
	template <> inline VariantType static_variant_type<bool>() { return VR_BOOL; };
	template <> inline VariantType static_variant_type<char>() { return VR_CHAR; };
	template <> inline VariantType static_variant_type<short>() { return VR_SHORT; };
	template <> inline VariantType static_variant_type<int>() { return VR_INT; };
	template <> inline VariantType static_variant_type<long>() { return VR_LONG; };
	template <> inline VariantType static_variant_type<long long>() { return VR_LONGLONG; };

	template <> inline VariantType static_variant_type<unsigned char>() { return VR_UCHAR; };
	template <> inline VariantType static_variant_type<unsigned short>() { return VR_USHORT; };
	template <> inline VariantType static_variant_type<unsigned int>() { return VR_UINT; };
	template <> inline VariantType static_variant_type<unsigned long>() { return VR_ULONG; };
	template <> inline VariantType static_variant_type<unsigned long long>() { return VR_ULONGLONG; };

	template <> inline VariantType static_variant_type<float>() { return VR_FLOAT; };
	template <> inline VariantType static_variant_type<double>() { return VR_DOUBLE; };
	template <> inline VariantType static_variant_type<long double>() { return VR_LONG_DOUBLE; };

	template <> inline VariantType static_variant_type<MatrixF>() { return VR_FLOAT_MATRIX; };
	template <> inline VariantType static_variant_type<MatrixD>() { return VR_DOUBLE_MATRIX; };
	template <> inline VariantType static_variant_type<MatrixLD>() { return VR_LONG_DOUBLE_MATRIX; };

	template <> inline VariantType static_variant_type<Individual>() { return VR_INDIVIDUAL; };
	template <> inline VariantType static_variant_type<Population>() { return VR_POPULATION; };

	template <> inline VariantType static_variant_type< std::vector< short > >() { return VR_STD_VECTOR_SHORT; };
	template <> inline VariantType static_variant_type< std::vector< int > >() { return VR_STD_VECTOR_INT; };
	template <> inline VariantType static_variant_type< std::vector< long > >() { return VR_STD_VECTOR_LONG; };
	template <> inline VariantType static_variant_type< std::vector< long long > >() { return VR_STD_VECTOR_LONGLONG; };

	template <> inline VariantType static_variant_type< std::vector< unsigned short > >() { return VR_STD_VECTOR_USHORT; };
	template <> inline VariantType static_variant_type< std::vector< unsigned int > >() { return VR_STD_VECTOR_UINT; };
	template <> inline VariantType static_variant_type< std::vector< unsigned long > >() { return VR_STD_VECTOR_ULONG; };
	template <> inline VariantType static_variant_type< std::vector< unsigned long long > >() { return VR_STD_VECTOR_ULONGLONG; };

	template <> inline VariantType static_variant_type< std::vector< float > >() { return VR_STD_VECTOR_FLOAT; };
	template <> inline VariantType static_variant_type< std::vector< double > >() { return VR_STD_VECTOR_DOUBLE; };
	template <> inline VariantType static_variant_type< std::vector< long double > >() { return VR_STD_VECTOR_LONG_DOUBLE; };
	template <> inline VariantType static_variant_type< std::vector< MatrixF > >()  { return VR_STD_VECTOR_FLOAT_MATRIX; };
	template <> inline VariantType static_variant_type< std::vector< MatrixD > >()  { return VR_STD_VECTOR_DOUBLE_MATRIX; };
	template <> inline VariantType static_variant_type< std::vector< MatrixLD > >() { return VR_STD_VECTOR_LONG_DOUBLE_MATRIX; }

	template <> inline VariantType static_variant_type<const char*>() { return VR_C_STRING; };
	template <> inline VariantType static_variant_type<std::string>() { return VR_STD_STRING; };
	template <> inline VariantType static_variant_type< std::vector< std::string > >() { return VR_STD_VECTOR_STRING; };

	template <> inline VariantType static_variant_type<void*>() { return VR_PTR; };

	class Variant
	{
	public:

		Variant(VariantType type)
		{
			set_type(type);
		}

		Variant()
		{
			set_type(VR_NONE);
		}

		Variant(const Variant& in)
		{
			copy_from(in);
		}

		~Variant()
		{
			set_type(VR_NONE);
		}

		Variant(bool b)
		{
			set_type(VR_BOOL);
			m_b = b;
		}

		Variant(char c)
		{
			set_type(VR_CHAR);
			m_c = c;
		}

		Variant(short s)
		{
			set_type(VR_SHORT);
			m_s = s;
		}

		Variant(int i)
		{
			set_type(VR_INT);
			m_i = i;
		}

		Variant(long l)
		{
			set_type(VR_LONG);
			m_l = l;
		}

		Variant(long long l)
		{
			set_type(VR_LONGLONG);
			m_ll = l;
		}

		Variant(unsigned char uc)
		{
			set_type(VR_UCHAR);
			m_uc = uc;
		}

		Variant(unsigned short us)
		{
			set_type(VR_USHORT);
			m_us = us;
		}

		Variant(unsigned int ui)
		{
			set_type(VR_UINT);
			m_ui = ui;
		}

		Variant(unsigned long ul)
		{
			set_type(VR_ULONG);
			m_ul = ul;
		}

		Variant(unsigned long long ull)
		{
			set_type(VR_ULONGLONG);
			m_ull = ull;
		}

		Variant(float f)
		{
			set_type(VR_FLOAT);
			m_f = f;
		}

		Variant(double d)
		{
			set_type(VR_DOUBLE);
			m_d = d;
		}
		
		Variant(long double ld)
		{
			set_type(VR_LONG_DOUBLE);
			m_ld = ld;
		}

		Variant(const MatrixF& fm)
		{
			set_type(VR_FLOAT_MATRIX);
			*((MatrixF *)(m_ptr)) = fm;
		}

		Variant(const MatrixD& dm)
		{
			set_type(VR_DOUBLE_MATRIX);
			*((MatrixD *)(m_ptr)) = dm;
		}

		Variant(const MatrixLD& ldm)
		{
			set_type(VR_LONG_DOUBLE_MATRIX);
			*((MatrixLD *)(m_ptr)) = ldm;
		}

		Variant(const Individual& i)
		{
			set_type(VR_INDIVIDUAL);
			set_individual(i);
		}

		Variant(const Population& pop)
		{
			set_type(VR_POPULATION);
			set_population(pop);
		}

		Variant(const std::vector< short > & v_s)
		{
			set_type(VR_STD_VECTOR_SHORT);
			*((std::vector< short >*)(m_ptr)) = v_s;
		}

		Variant(const std::vector< int > & v_i)
		{
			set_type(VR_STD_VECTOR_INT);
			*((std::vector< int >*)(m_ptr)) = v_i;
		}

		Variant(const std::vector< long > & v_l)
		{
			set_type(VR_STD_VECTOR_LONG);
			*((std::vector< long >*)(m_ptr)) = v_l;
		}

		Variant(const std::vector< long long > & v_ll)
		{
			set_type(VR_STD_VECTOR_LONGLONG);
			*((std::vector< long long >*)(m_ptr)) = v_ll;
		}

		Variant(const std::vector< unsigned short > & v_us)
		{
			set_type(VR_STD_VECTOR_USHORT);
			*((std::vector< unsigned short >*)(m_ptr)) = v_us;
		}

		Variant(const std::vector< unsigned int > & v_ui)
		{
			set_type(VR_STD_VECTOR_UINT);
			*((std::vector< unsigned int >*)(m_ptr)) = v_ui;
		}

		Variant(const std::vector< unsigned long > & v_ul)
		{
			set_type(VR_STD_VECTOR_ULONG);
			*((std::vector< unsigned long >*)(m_ptr)) = v_ul;
		}

		Variant(const std::vector< unsigned long long > & v_ull)
		{
			set_type(VR_STD_VECTOR_ULONGLONG);
			*((std::vector< unsigned long long >*)(m_ptr)) = v_ull;
		}

		Variant(const std::vector< float > & v_f)
		{
			set_type(VR_STD_VECTOR_FLOAT);
			*((std::vector< float >*)(m_ptr)) = v_f;
		}

		Variant(const std::vector< double > & v_d)
		{
			set_type(VR_STD_VECTOR_DOUBLE);
			*((std::vector< double >*)(m_ptr)) = v_d;
		}

		Variant(const std::vector< long double > & v_ld)
		{
			set_type(VR_STD_VECTOR_LONG_DOUBLE);
			*((std::vector< long double >*)(m_ptr)) = v_ld;
		}

		Variant(const std::vector< MatrixF > & v_v2)
		{
			set_type(VR_STD_VECTOR_FLOAT_MATRIX);
			*((std::vector< MatrixF >*)(m_ptr)) = v_v2;
		}

		Variant(const std::vector< MatrixD > & v_v3)
		{
			set_type(VR_STD_VECTOR_DOUBLE_MATRIX);
			*((std::vector< MatrixD >*)(m_ptr)) = v_v3;
		}

		Variant(const std::vector< MatrixLD > & v_v4)
		{
			set_type(VR_STD_VECTOR_LONG_DOUBLE_MATRIX);
			*((std::vector< MatrixLD >*)(m_ptr)) = v_v4;
		}

		Variant(const char* c_str)
		{
			set_type(VR_STD_STRING);
			*((std::string*)(m_ptr)) = c_str;
		}

		Variant(const std::string& str)
		{
			set_type(VR_STD_STRING);
			*((std::string*)(m_ptr)) = str;
		}

		Variant(const std::vector< std::string > & v_str)
		{
			set_type(VR_STD_VECTOR_STRING);
			*((std::vector< std::string >*)(m_ptr)) = v_str;
		}

		Variant(void* ptr)
		{
			set_type(VR_PTR);
			m_ptr = ptr;
		}

		Variant(const VariantRef& ref);

		//cast objects
		template < class T >
		T& get()
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				return *((T*)&m_ptr);
		}
		template < class T >
		const T& get() const
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				return *((T*)&m_ptr);
		}
		template < class T >
		explicit  operator const T& () const
		{
			if (is_heap_value()) return *((T*)m_ptr);
			else				 return *((T*)&m_ptr);
		}

		//alloc_cast
		template < class T >
		T& get_alloc()
		{
			if (static_variant_type<T>() != m_type)
			{
				if (!is_heap_value())
				{
					std::memset(this, 0, sizeof(Variant));
					m_type = VR_NONE;
				}
				set_type(static_variant_type<T>());
			}
			return *((T*)&m_ptr);
		}

		//type
		VariantType get_type() const
		{
			return m_type;
		}

		bool equal(const Variant& right, bool case_sensitive = true) const;

		bool operator == (const Variant& right) const
		{
			return equal(right);
		}

		bool operator != (const Variant& right) const
		{
			return !equal(right);
		}
		//query
		bool is_none() const
		{
			return m_type == VR_NONE;
		}

		bool is_null() const
		{
			return m_type == VR_NONE;
		}

		bool is_heap_value() const;

		//get reference pointer
		void* get_ref() const
		{
			if (is_heap_value()) return m_ptr;
			else				 return (void*)&m_c;
		}

		//copy from other variant
		void copy_from(const Variant& in);
		//set help function
		void set_individual(const Individual& individual);
		void set_population(const Population& population);

	private:

		//native type
		union alignas(16)
		{
			bool       m_b;
			char       m_c;
			short      m_s;
			int        m_i;
			long       m_l;
			long long  m_ll;

			unsigned char       m_uc;
			unsigned short      m_us;
			unsigned int        m_ui;
			unsigned long       m_ul;
			unsigned long long  m_ull;

			float  m_f;
			double m_d;
			long double m_ld;

			void* m_ptr;
		};
		//save type
		VariantType m_type{ VR_NONE };
		//set type
		void set_type(VariantType type);
	};

	class VariantRef
	{
	public:

		VariantRef()
		{
			m_type = VR_NONE;
		}


		VariantRef(const bool& b)
		{
			m_ptr = (void*)&b;
			m_type = VR_BOOL;
		}

		VariantRef(const char& c)
		{
			m_ptr = (void*)&c;
			m_type = VR_CHAR;
		}

		VariantRef(const short& s)
		{
			m_ptr = (void*)&s;
			m_type = VR_SHORT;
		}

		VariantRef(const int& i)
		{
			m_ptr = (void*)&i;
			m_type = VR_INT;
		}

		VariantRef(const long& l)
		{
			m_ptr = (void*)&l;
			m_type = VR_LONG;
		}

		VariantRef(const long long& l)
		{
			m_ptr = (void*)&l;
			m_type = VR_LONGLONG;
		}

		VariantRef(const unsigned char& uc)
		{
			m_ptr = (void*)&uc;
			m_type = VR_UCHAR;
		}

		VariantRef(const unsigned short& us)
		{
			m_ptr = (void*)&us;
			m_type = VR_USHORT;
		}

		VariantRef(const unsigned int& ui)
		{
			m_ptr = (void*)&ui;
			m_type = VR_UINT;
		}

		VariantRef(const unsigned long& ul)
		{
			m_ptr = (void*)&ul;
			m_type = VR_ULONG;
		}

		VariantRef(const unsigned long long& ull)
		{
			m_ptr = (void*)&ull;
			m_type = VR_ULONGLONG;
		}

		VariantRef(const float& f)
		{
			m_ptr = (void*)&f;
			m_type = VR_FLOAT;
		}

		VariantRef(const double& d)
		{
			m_ptr = (void*)&d;
			m_type = VR_DOUBLE;
		}

		VariantRef(const long double& ld)
		{
			m_ptr = (void*)&ld;
			m_type = VR_LONG_DOUBLE;
		}

		VariantRef(const MatrixF& fm)
		{
			m_ptr = (void*)&fm;
			m_type = VR_FLOAT_MATRIX;
		}

		VariantRef(const MatrixD& dm)
		{
			m_ptr = (void*)&dm;
			m_type = VR_DOUBLE_MATRIX;
		}

		VariantRef(const MatrixLD& ldm)
		{
			m_ptr = (void*)&ldm;
			m_type = VR_LONG_DOUBLE_MATRIX;
		}

		VariantRef(const Individual& i)
		{
			m_ptr = (void*)&i;
			m_type = VR_INDIVIDUAL;
		}

		VariantRef(const Population& pop)
		{
			m_ptr = (void*)&pop;
			m_type = VR_POPULATION;
		}

		VariantRef(const std::vector< short > & v_s)
		{
			m_ptr = (void*)&v_s;
			m_type = VR_STD_VECTOR_SHORT;
		}

		VariantRef(const std::vector< int > & v_i)
		{
			m_ptr = (void*)&v_i;
			m_type = VR_STD_VECTOR_INT;
		}

		VariantRef(const std::vector< long > & v_l)
		{
			m_ptr = (void*)&v_l;
			m_type = VR_STD_VECTOR_LONG;
		}

		VariantRef(const std::vector< long long > & v_ll)
		{
			m_ptr = (void*)&v_ll;
			m_type = VR_STD_VECTOR_LONGLONG;
		}		
		
		VariantRef(const std::vector< unsigned short > & v_us)
		{
			m_ptr = (void*)&v_us;
			m_type = VR_STD_VECTOR_USHORT;
		}

		VariantRef(const std::vector< unsigned int > & v_ui)
		{
			m_ptr = (void*)&v_ui;
			m_type = VR_STD_VECTOR_UINT;
		}

		VariantRef(const std::vector< unsigned long > & v_ul)
		{
			m_ptr = (void*)&v_ul;
			m_type = VR_STD_VECTOR_ULONG;
		}

		VariantRef(const std::vector< unsigned long long > & v_ull)
		{
			m_ptr = (void*)&v_ull;
			m_type = VR_STD_VECTOR_ULONGLONG;
		}

		VariantRef(const std::vector< float > & v_f)
		{
			m_ptr = (void*)&v_f;
			m_type = VR_STD_VECTOR_FLOAT;
		}

		VariantRef(const std::vector< double > & v_d)
		{
			m_ptr = (void*)&v_d;
			m_type = VR_STD_VECTOR_DOUBLE;
		}

		VariantRef(const std::vector< long double > & v_ld)
		{
			m_ptr = (void*)&v_ld;
			m_type = VR_STD_VECTOR_LONG_DOUBLE;
		}

		VariantRef(const std::vector< MatrixF > & v_fm)
		{
			m_ptr = (void*)&v_fm;
			m_type = VR_STD_VECTOR_FLOAT_MATRIX;
		}

		VariantRef(const std::vector< MatrixD > & v_dm)
		{
			m_ptr = (void*)&v_dm;
			m_type = VR_STD_VECTOR_DOUBLE_MATRIX;
		}

		VariantRef(const std::vector< MatrixLD > & v_dm)
		{
			m_ptr = (void*)&v_dm;
			m_type = VR_STD_VECTOR_LONG_DOUBLE_MATRIX;
		}

		VariantRef(const char*& c_str)
		{
			m_ptr = (void*)&c_str;
			m_type = VR_C_STRING;
		}

		VariantRef(const std::string& str)
		{
			m_ptr = (void*)&str;
			m_type = VR_STD_STRING;
		}

		VariantRef(const std::vector< std::string >& v_str)
		{
			m_ptr = (void*)&v_str;
			m_type = VR_STD_VECTOR_STRING;
		}

		VariantRef(const void* ptr)
		{
			m_ptr = (void*)ptr;
			m_type = VR_PTR;
		}

		//only if is explicit (shadowing the others constructors)
		explicit VariantRef(const Variant& var) 
		{
			m_ptr = (void*)var.get_ref();
			m_type = var.get_type();
		}

		template < class T >
		T& get()
		{
			return *((T*)m_ptr);
		}

		template < class T >
		const T& get() const
		{
			return *((T*)m_ptr);
		}


		template < class T >
		explicit  operator const T& () const
		{
			return *((T*)m_ptr);
		}

		VariantType get_type() const
		{
			return m_type;
		}

		template < class T >
		T* get_ptr()
		{
			return (T*)m_ptr;
		}

		template < class T >
		const T* get_ptr() const
		{
			return (T*)m_ptr;
		}

	private:

		void*		 m_ptr;
		VariantType m_type;

	};

	
}