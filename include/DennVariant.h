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
#include "DennIndividual.h"
#include "DennPopulation.h"

namespace Denn
{
	class Variant;
	class VariantRef;
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
			*((Individual *)(m_ptr)) = i;
		}

		Variant(const Population& pop)
		{
			set_type(VR_POPULATION);
			*((Population *)(m_ptr)) = pop;
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

		bool equal (const Variant& right, bool case_sensitive = true) const
		{
			if (get_type() != right.get_type()) return false;
			//all cases
			switch (get_type())
			{
			case VR_NONE:  return true;
			case VR_BOOL:  return get<bool>() == right.get<bool>();
			case VR_CHAR:  return get<char>() == right.get<char>();
			case VR_SHORT: return get<short>() == right.get<short>();
			case VR_INT:  return get<int>() == right.get<int>();
			case VR_LONG:  return get<long>() == right.get<long>();
			case VR_LONGLONG:  return get<long long>() == right.get<long long>();

			case VR_UCHAR:  return get<unsigned char>() == right.get<char>();
			case VR_USHORT:  return get<unsigned short>() == right.get<unsigned short>();
			case VR_UINT:  return get<unsigned int>() == right.get<unsigned int>();
			case VR_ULONG:  return get<unsigned long>() == right.get<unsigned long>();
			case VR_ULONGLONG:  return get<unsigned long>() == right.get<unsigned long>();

			case VR_FLOAT:  return get<float>() == right.get<float>();
			case VR_DOUBLE:  return get<double>() == right.get<double>();
			case VR_LONG_DOUBLE:  return get<long double>() == right.get<long double>();

			case VR_FLOAT_MATRIX: return get<MatrixF>() == right.get<MatrixF>();
			case VR_DOUBLE_MATRIX: return get<MatrixD>() == right.get<MatrixD>();
			case VR_LONG_DOUBLE_MATRIX: return get<MatrixLD>() == right.get<MatrixLD>();
			//not support
			case VR_INDIVIDUAL:
			case VR_POPULATION:
			case VR_STD_VECTOR_SHORT:
			case VR_STD_VECTOR_INT:
			case VR_STD_VECTOR_LONG:
			case VR_STD_VECTOR_LONGLONG:
			case VR_STD_VECTOR_USHORT:
			case VR_STD_VECTOR_UINT:
			case VR_STD_VECTOR_ULONG:
			case VR_STD_VECTOR_ULONGLONG:
			case VR_STD_VECTOR_FLOAT:
			case VR_STD_VECTOR_DOUBLE:
			case VR_STD_VECTOR_LONG_DOUBLE:
			case VR_STD_VECTOR_FLOAT_MATRIX:
			case VR_STD_VECTOR_DOUBLE_MATRIX:
			case VR_STD_VECTOR_LONG_DOUBLE_MATRIX:
			case VR_STD_VECTOR_STRING:
				return false;
			//string
			case VR_C_STRING:
			case VR_STD_STRING:
				if(case_sensitive) return get<std::string>() == right.get<std::string>();
				else return case_insensitive_equal(get<std::string>(),right.get<std::string>());
			//pointer
			case VR_PTR: 
				return m_ptr == right.m_ptr;
			//ok
			default: break;
			}
		}

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

		bool is_heap_value() const
		{
			switch (m_type)
			{
			case VR_FLOAT_MATRIX:
			case VR_DOUBLE_MATRIX:
			case VR_LONG_DOUBLE_MATRIX:
			case VR_INDIVIDUAL:
			case VR_POPULATION:
			case VR_C_STRING:
			case VR_STD_STRING:
			case VR_STD_VECTOR_SHORT:
			case VR_STD_VECTOR_INT:
			case VR_STD_VECTOR_LONG:
			case VR_STD_VECTOR_LONGLONG:
			case VR_STD_VECTOR_USHORT:
			case VR_STD_VECTOR_UINT:
			case VR_STD_VECTOR_ULONG:
			case VR_STD_VECTOR_ULONGLONG:
			case VR_STD_VECTOR_FLOAT:
			case VR_STD_VECTOR_DOUBLE:
			case VR_STD_VECTOR_LONG_DOUBLE:
			case VR_STD_VECTOR_FLOAT_MATRIX:
			case VR_STD_VECTOR_DOUBLE_MATRIX:
			case VR_STD_VECTOR_LONG_DOUBLE_MATRIX:
			case VR_STD_VECTOR_STRING: return true; break;
			default: return false; break;
			}
		}

		//get reference pointer
		void* get_ref() const
		{
			if (is_heap_value()) return m_ptr;
			else				 return (void*)&m_c;
		}

		//copy from other variant
		void copy_from(const Variant& in)
		{
			//alloc
			set_type(in.get_type());
			//copy
			switch (m_type)
			{
			//copy from heap
			case VR_FLOAT_MATRIX:			   get<MatrixF>() = (const  MatrixF&)in; break;
			case VR_DOUBLE_MATRIX:			   get<MatrixD>() = (const  MatrixD&)in; break;
			case VR_LONG_DOUBLE_MATRIX:		   get<MatrixLD>() = (const MatrixLD&)in; break;

			case VR_INDIVIDUAL:		   get<Individual>() = (const Individual&)in; break;
			case VR_POPULATION:		   get<Population>() = (const Population&)in; break;

			case VR_STD_VECTOR_SHORT:
				get< std::vector<short> >() = (const std::vector<short>&)in;
				break;
			case VR_STD_VECTOR_INT:
				get< std::vector<int> >() = (const std::vector<int>&)in;
				break;
			case VR_STD_VECTOR_LONG:
				get< std::vector<long> >() = (const std::vector<long>&)in;
				break;
			case VR_STD_VECTOR_LONGLONG:
				get< std::vector<long long> >() = (const std::vector<long long>&)in;
				break;
			case VR_STD_VECTOR_USHORT:
				get< std::vector<unsigned short> >() = (const std::vector<unsigned short>&)in;
				break;
			case VR_STD_VECTOR_UINT:
				get< std::vector<unsigned int> >() = (const std::vector<unsigned int>&)in;
				break;
			case VR_STD_VECTOR_ULONG:
				get< std::vector<unsigned long> >() = (const std::vector<unsigned long>&)in;
				break;
			case VR_STD_VECTOR_ULONGLONG:
				get< std::vector<unsigned long long> >() = (const std::vector<unsigned long long>&)in;
				break;
			case VR_STD_VECTOR_FLOAT:
				get< std::vector<float> >() = (const std::vector<float>&)in;
				break;
			case VR_STD_VECTOR_DOUBLE:
				get< std::vector<double> >() = (const std::vector<double>&)in;
				break;
			case VR_STD_VECTOR_LONG_DOUBLE:
				get< std::vector<long double> >() = (const std::vector<long double>&)in;
				break;
			case VR_STD_VECTOR_FLOAT_MATRIX:
				get< std::vector<MatrixF> >() = (const std::vector<MatrixF>&)in;
				break;
			case VR_STD_VECTOR_DOUBLE_MATRIX:
				get< std::vector<MatrixD> >() = (const std::vector<MatrixD>&)in;
				break;
			case VR_STD_VECTOR_LONG_DOUBLE_MATRIX:
				get< std::vector<MatrixLD> >() = (const std::vector<MatrixLD>&)in;
				break;

			case VR_C_STRING:
			case VR_STD_STRING:
				get<std::string>() = (const std::string&)in;
				break;
			case VR_STD_VECTOR_STRING:
				get< std::vector<std::string> >() = (const std::vector<std::string>&)in;
				break;
				//copy stack
			default:
				std::memcpy(this, &in, sizeof(Variant));
				break;
			}
		}

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
		void set_type(VariantType type)
		{
			//dealloc
			switch (m_type)
			{
			case VR_FLOAT_MATRIX:			   delete (MatrixF*)(m_ptr);  break;
			case VR_DOUBLE_MATRIX:			   delete (MatrixD*)(m_ptr);  break;
			case VR_LONG_DOUBLE_MATRIX:		   delete (MatrixLD*)(m_ptr); break;

			case VR_INDIVIDUAL:		   delete (Individual*)(m_ptr); break;
			case VR_POPULATION:		   delete (Population*)(m_ptr); break;

			case VR_STD_VECTOR_SHORT:    delete (std::vector<short>*)m_ptr;    break;
			case VR_STD_VECTOR_INT:    delete (std::vector<int>*)m_ptr;    break;
			case VR_STD_VECTOR_LONG:    delete (std::vector<long>*)m_ptr;    break;
			case VR_STD_VECTOR_LONGLONG:    delete (std::vector<long long>*)m_ptr;    break;
			case VR_STD_VECTOR_USHORT:    delete (std::vector<unsigned short>*)m_ptr;    break;
			case VR_STD_VECTOR_UINT:    delete (std::vector<unsigned int>*)m_ptr;    break;
			case VR_STD_VECTOR_ULONG:    delete (std::vector<unsigned long>*)m_ptr;    break;
			case VR_STD_VECTOR_ULONGLONG:    delete (std::vector<unsigned long long>*)m_ptr;    break;
			case VR_STD_VECTOR_FLOAT:  delete (std::vector<float>*)m_ptr;  break;
			case VR_STD_VECTOR_DOUBLE:  delete (std::vector<double>*)m_ptr;  break;
			case VR_STD_VECTOR_LONG_DOUBLE:  delete (std::vector<long double>*)m_ptr;  break;
			case VR_STD_VECTOR_FLOAT_MATRIX:        delete (std::vector<MatrixF>*)m_ptr;   break;
			case VR_STD_VECTOR_DOUBLE_MATRIX:        delete (std::vector<MatrixD>*)m_ptr;   break;
			case VR_STD_VECTOR_LONG_DOUBLE_MATRIX:   delete (std::vector<MatrixLD>*)m_ptr;   break;

			case VR_C_STRING:          delete (std::string*)m_ptr;			     break;
			case VR_STD_STRING:        delete (std::string*)m_ptr;			     break;
			case VR_STD_VECTOR_STRING: delete (std::vector<std::string>*)m_ptr;  break;
			default: break;
			}
			//change type
			m_type = type;
			//alloc
			switch (m_type)
			{
			case VR_FLOAT_MATRIX:			   m_ptr = new MatrixF; break;
			case VR_DOUBLE_MATRIX:			   m_ptr = new MatrixD; break;
			case VR_LONG_DOUBLE_MATRIX:		   m_ptr = new MatrixLD; break;

			case VR_INDIVIDUAL:			        m_ptr = new Individual; break;
			case VR_POPULATION:		            m_ptr = new Population; break;

			case VR_STD_VECTOR_SHORT:           m_ptr = new std::vector<short>;      break;
			case VR_STD_VECTOR_INT:             m_ptr = new std::vector<int>;	     break;
			case VR_STD_VECTOR_LONG:            m_ptr = new std::vector<long>;       break;
			case VR_STD_VECTOR_LONGLONG:        m_ptr = new std::vector<long long>;	 break;
			case VR_STD_VECTOR_USHORT:          m_ptr = new std::vector<unsigned short>;     break;
			case VR_STD_VECTOR_UINT:            m_ptr = new std::vector<unsigned int>;	     break;
			case VR_STD_VECTOR_ULONG:           m_ptr = new std::vector<unsigned long>;      break;
			case VR_STD_VECTOR_ULONGLONG:       m_ptr = new std::vector<unsigned long long>; break;
			case VR_STD_VECTOR_FLOAT:           m_ptr = new std::vector<float>;	     break;
			case VR_STD_VECTOR_DOUBLE:          m_ptr = new std::vector<double>;     break;
			case VR_STD_VECTOR_LONG_DOUBLE:     m_ptr = new std::vector<long double>;break;
			case VR_STD_VECTOR_FLOAT_MATRIX:    m_ptr = new std::vector<MatrixF>;    break;
			case VR_STD_VECTOR_DOUBLE_MATRIX:   m_ptr = new std::vector<MatrixD>;	 break;
			case VR_STD_VECTOR_LONG_DOUBLE_MATRIX: m_ptr = new std::vector<MatrixLD>;break;

			case VR_C_STRING:          m_ptr = new std::string;				 break;
			case VR_STD_STRING:        m_ptr = new std::string;				 break;
			case VR_STD_VECTOR_STRING: m_ptr = new std::vector<std::string>; break;
			default: break;
			}
		}
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

	inline Variant::Variant(const VariantRef& ref)
	{
		switch (ref.get_type())
		{
		case VariantType::VR_NONE:     (*this) = Variant();        break;
		case VariantType::VR_BOOL:     (*this) = ref.get<bool>();  break;
		case VariantType::VR_CHAR:     (*this) = ref.get<char>();  break;
		case VariantType::VR_SHORT:    (*this) = ref.get<short>(); break;
		case VariantType::VR_INT:      (*this) = ref.get<int>();   break;
		case VariantType::VR_LONG:     (*this) = ref.get<long>();  break;
		case VariantType::VR_LONGLONG: (*this) = ref.get<long long>(); break;
		case VariantType::VR_UCHAR:    (*this) = ref.get<unsigned char>(); break;
		case VariantType::VR_USHORT:   (*this) = ref.get<unsigned short>(); break;
		case VariantType::VR_UINT:     (*this) = ref.get<unsigned int>(); break;
		case VariantType::VR_ULONG:    (*this) = ref.get<unsigned long>(); break;
		case VariantType::VR_ULONGLONG:(*this) = ref.get<unsigned long long>(); break;
		case VariantType::VR_FLOAT:    (*this) = ref.get<float>(); break;
		case VariantType::VR_DOUBLE:   (*this) = ref.get<double>();  break;
		case VariantType::VR_LONG_DOUBLE:   (*this) = ref.get<long double>();  break;

		case VariantType::VR_FLOAT_MATRIX:      (*this)  = ref.get<MatrixF>(); break;
		case VariantType::VR_DOUBLE_MATRIX:     (*this)  = ref.get<MatrixD>(); break;
		case VariantType::VR_LONG_DOUBLE_MATRIX: (*this) = ref.get<MatrixLD>(); break;

		case VariantType::VR_INDIVIDUAL:         (*this) = ref.get<Individual>(); break;
		case VariantType::VR_POPULATION:         (*this) = ref.get<Population>(); break;

		case VariantType::VR_STD_VECTOR_SHORT:             (*this) = ref.get< std::vector<short> >(); break;
		case VariantType::VR_STD_VECTOR_INT:               (*this) = ref.get< std::vector<int> >(); break;
		case VariantType::VR_STD_VECTOR_LONG:              (*this) = ref.get< std::vector<long> >(); break;
		case VariantType::VR_STD_VECTOR_LONGLONG:          (*this) = ref.get< std::vector<long long> >(); break;
		case VariantType::VR_STD_VECTOR_USHORT:            (*this) = ref.get< std::vector<unsigned short> >(); break;
		case VariantType::VR_STD_VECTOR_UINT:              (*this) = ref.get< std::vector<unsigned int> >(); break;
		case VariantType::VR_STD_VECTOR_ULONG:             (*this) = ref.get< std::vector<unsigned long> >(); break;
		case VariantType::VR_STD_VECTOR_ULONGLONG:         (*this) = ref.get< std::vector<unsigned long long> >(); break;
		case VariantType::VR_STD_VECTOR_FLOAT:             (*this) = ref.get< std::vector<float> >(); break;
		case VariantType::VR_STD_VECTOR_DOUBLE:            (*this) = ref.get< std::vector<double> >(); break;
		case VariantType::VR_STD_VECTOR_LONG_DOUBLE:       (*this) = ref.get< std::vector<long double> >(); break;
		case VariantType::VR_STD_VECTOR_FLOAT_MATRIX:      (*this) = ref.get< std::vector<MatrixF> >(); break;
		case VariantType::VR_STD_VECTOR_DOUBLE_MATRIX:     (*this) = ref.get< std::vector<MatrixD> >(); break;
		case VariantType::VR_STD_VECTOR_LONG_DOUBLE_MATRIX:(*this) = ref.get< std::vector<MatrixLD> >(); break;

		case VariantType::VR_C_STRING:
		case VariantType::VR_STD_STRING:           (*this) = ref.get< std::string >(); break;
		case VariantType::VR_STD_VECTOR_STRING:    (*this) = ref.get< std::vector<std::string> >(); break;

		case VariantType::VR_PTR:				   (*this) = (void*)ref.get_ptr<void>(); break;
		default: break;
		}
	}
}