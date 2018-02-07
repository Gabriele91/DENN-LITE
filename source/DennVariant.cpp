#include "Config.h"
#include "DennVariant.h"
#include "DennIndividual.h"
#include "DennPopulation.h"

namespace Denn
{
	Variant::Variant(const VariantRef& ref)
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

		case VariantType::VR_FLOAT_MATRIX:      (*this) = ref.get<MatrixF>(); break;
		case VariantType::VR_DOUBLE_MATRIX:     (*this) = ref.get<MatrixD>(); break;
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

	void Variant::set_individual(const Individual& individual)
	{
		*((Individual *)(m_ptr)) = individual;
	}

	void Variant::set_population(const Population& population)
	{
		*((Population *)(m_ptr)) = population;
	}

	bool Variant::is_heap_value() const
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

	bool Variant::equal(const Variant& right, bool case_sensitive) const
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
			if (case_sensitive) return get<std::string>() == right.get<std::string>();
			else return case_insensitive_equal(get<std::string>(), right.get<std::string>());
			//pointer
		case VR_PTR:
			return m_ptr == right.m_ptr;
			//ok
		default: break;
		}
	}

	void Variant::copy_from(const Variant& in)
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

	void Variant::set_type(VariantType type)
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
		case VR_STD_VECTOR_LONG_DOUBLE:     m_ptr = new std::vector<long double>; break;
		case VR_STD_VECTOR_FLOAT_MATRIX:    m_ptr = new std::vector<MatrixF>;    break;
		case VR_STD_VECTOR_DOUBLE_MATRIX:   m_ptr = new std::vector<MatrixD>;	 break;
		case VR_STD_VECTOR_LONG_DOUBLE_MATRIX: m_ptr = new std::vector<MatrixLD>; break;

		case VR_C_STRING:          m_ptr = new std::string;				 break;
		case VR_STD_STRING:        m_ptr = new std::string;				 break;
		case VR_STD_VECTOR_STRING: m_ptr = new std::vector<std::string>; break;
		default: break;
		}
	}

}