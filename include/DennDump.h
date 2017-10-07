#pragma once
#include "Config.h"
#include "DennVariant.h"

namespace Denn
{
namespace Dump
{
	inline static std::string json_bool(bool b)
	{
		return b ? "true" : "false";
	}
	
	inline static std::string json_string(const std::string& str)
	{
		//copy ptr
		const char *tmp = str.c_str();
		//start '"'
		std::string out = "\"";
		//push all chars
		while (*tmp)
		{
			switch (*tmp)
			{
			case '\n':
				out += "\\n";
				break;
			case '\t':
				out += "\\t";
				break;
			case '\b':
				out += "\\b";
				break;
			case '\r':
				out += "\\r";
				break;
			case '\f':
				out += "\\f";
				break;
			case '\a':
				out += "\\a";
				break;
			case '\\':
				out += "\\\\";
				break;
			case '\?':
				out += "\\?";
				break;
			case '\'':
				out += "\\\'";
				break;
			case '\"':
				out += "\\\"";
				break;
			default:
				out += *tmp;
				break;
			}
			++tmp;
		}
		return out + "\"";
	}

	template < typename T > static std::string json_matrix(const T& matrix)
	{
		Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
		std::ostringstream ssout; ssout << matrix.format(matrix_to_json_array); return ssout.str();
	}

	template < typename T > inline static std::string json_array(const std::vector< T >& t_list)
	{
		if (!t_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != t_list.size() - 1; ++i) s_out << std::to_string(t_list[i]) << ", ";
		s_out << std::to_string(t_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}

	template<> inline static std::string json_array<bool>(const std::vector< bool >& b_list)
	{
		if (!b_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != b_list.size() - 1; ++i) s_out << json_bool(b_list[i]) << ", ";
		s_out << json_bool(b_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}

	template<> inline static std::string json_array<std::string>(const std::vector< std::string >& str_list)
	{
		if (!str_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != str_list.size() - 1; ++i) s_out << json_string(str_list[i]) << ", ";
		s_out << json_string(str_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}

	template<> inline static std::string json_array<MatrixF>(const std::vector< MatrixF >& m_list)
	{
		if (!m_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != m_list.size() - 1; ++i) s_out << json_matrix(m_list[i]) << ", ";
		s_out << json_matrix(m_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}
	
	template<> inline static std::string json_array<MatrixD>(const std::vector< MatrixD >& m_list)
	{
		if (!m_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != m_list.size() - 1; ++i) s_out << json_matrix(m_list[i]) << ", ";
		s_out << json_matrix(m_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}

	template<> inline static std::string json_array<MatrixLD>(const std::vector< MatrixLD >& m_list)
	{
		if (!m_list.size()) return "[]";
		//concanate names
		std::stringstream s_out;
		s_out << "[";
		for (size_t i = 0; i != m_list.size() - 1; ++i) s_out << json_matrix(m_list[i]) << ", ";
		s_out << json_matrix(m_list.back());
		s_out << "]";
		//end
		return s_out.str();
	}

	static std::string json_variant(const Variant& value)
	{
		Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
		switch (value.get_type())
		{
		case VariantType::VR_NONE:     return "null"; break;
		case VariantType::VR_BOOL:     return value.get<bool>() ? "true" : "false"; break;
		case VariantType::VR_CHAR:     return std::to_string(value.get<char>());  break;
		case VariantType::VR_SHORT:    return std::to_string(value.get<short>()); break;
		case VariantType::VR_INT:      return std::to_string(value.get<int>());   break;
		case VariantType::VR_LONG:     return std::to_string(value.get<long>());  break;
		case VariantType::VR_LONGLONG: return std::to_string(value.get<long long>()); break;
		case VariantType::VR_UCHAR:    return std::to_string(value.get<unsigned char>());  break;
		case VariantType::VR_USHORT:   return std::to_string(value.get<unsigned short>()); break;
		case VariantType::VR_UINT:     return std::to_string(value.get<unsigned int>()); break;
		case VariantType::VR_ULONG:    return std::to_string(value.get<unsigned long>()); break;
		case VariantType::VR_ULONGLONG:return std::to_string(value.get<unsigned long long>()); break;
		case VariantType::VR_FLOAT:    return std::to_string(value.get<float>());  break;
		case VariantType::VR_DOUBLE:   return std::to_string(value.get<double>());  break;
		case VariantType::VR_LONG_DOUBLE:   return std::to_string(value.get<long double>());  break;

		case VariantType::VR_FLOAT_MATRIX:        return json_matrix<MatrixF>(value.get<MatrixF>()); break;
		case VariantType::VR_DOUBLE_MATRIX:       return json_matrix<MatrixD>(value.get<MatrixD>()); break;
		case VariantType::VR_LONG_DOUBLE_MATRIX:  return json_matrix<MatrixLD>(value.get<MatrixLD>()); break;

		//case VariantType::VR_INDIVIDUAL:         (*this) = ref.get<Individual>(); break;
		//case VariantType::VR_POPULATION:         (*this) = ref.get<Population>(); break;

		case VariantType::VR_STD_VECTOR_SHORT:             return json_array<short>(value.get< std::vector<short> >()); break;
		case VariantType::VR_STD_VECTOR_INT:               return json_array<int>(value.get< std::vector<int> >()); break;
		case VariantType::VR_STD_VECTOR_LONG:              return json_array<long>(value.get< std::vector<long> >()); break;
		case VariantType::VR_STD_VECTOR_LONGLONG:          return json_array<long long>(value.get< std::vector<long long> >()); break;

		case VariantType::VR_STD_VECTOR_USHORT:             return json_array<unsigned short>(value.get< std::vector<unsigned short> >()); break;
		case VariantType::VR_STD_VECTOR_UINT:               return json_array<unsigned int>(value.get< std::vector<unsigned int> >()); break;
		case VariantType::VR_STD_VECTOR_ULONG:              return json_array<unsigned long>(value.get< std::vector<unsigned long> >()); break;
		case VariantType::VR_STD_VECTOR_ULONGLONG:          return json_array<unsigned long long>(value.get< std::vector<unsigned long long> >()); break;

		case VariantType::VR_STD_VECTOR_FLOAT:             return json_array<float>(value.get< std::vector<float> >()); break;
		case VariantType::VR_STD_VECTOR_DOUBLE:            return json_array<double>(value.get< std::vector<double> >()); break;
		case VariantType::VR_STD_VECTOR_LONG_DOUBLE:       return json_array<long double>(value.get< std::vector<long double> >()); break;
		case VariantType::VR_STD_VECTOR_FLOAT_MATRIX:      return json_array<MatrixF>(value.get< std::vector<MatrixF> >()); break;
		case VariantType::VR_STD_VECTOR_DOUBLE_MATRIX:     return json_array<MatrixD>(value.get< std::vector<MatrixD> >()); break;
		case VariantType::VR_STD_VECTOR_LONG_DOUBLE_MATRIX:return json_array<MatrixLD>(value.get< std::vector<MatrixLD> >()); break;

		case VariantType::VR_C_STRING:
		case VariantType::VR_STD_STRING:           return json_string(value.get< std::string >()); break;
		case VariantType::VR_STD_VECTOR_STRING:    return json_array(value.get< std::vector<std::string> >()); break;

		case VariantType::VR_PTR:				   return std::to_string(value.get<size_t>()); break;
		default: return ""; break;
		}
	}
}
}