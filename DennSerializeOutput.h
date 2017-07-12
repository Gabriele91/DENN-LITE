#pragma once 
#include "Config.h"
#include "TicksTime.h"
#include "NeuralNetwork.h"
#include <iostream>
#include <sstream>

namespace Denn 
{
	class SerializeOutput  : public std::enable_shared_from_this< SerializeOutput >
	{
	public:

        using SPtr = std::shared_ptr<SerializeOutput>;

        SPtr get_ptr() { return shared_from_this(); }

		SerializeOutput(std::ostream& ostream = std::cout) :m_ostream(ostream) {}

		virtual ~SerializeOutput() {}

		virtual void serialize_parameters(const Denn::Parameters& args) {}
		
		virtual void serialize_best(double time, Denn::Scalar accuracy, Denn::Scalar f, Denn::Scalar cr, const Denn::NeuralNetwork& network)
        {   
        }

	protected:

		std::ostream& m_ostream;
		
	};


    class JSONSerializeOutput : public SerializeOutput
    {
    public:
    	
		JSONSerializeOutput(std::ostream& ostream = std::cout) : SerializeOutput(ostream)
		{
			m_ostream << "{" << std::endl;
		}

		virtual ~JSONSerializeOutput()
		{
			m_ostream << "}" << std::endl;
		}

		virtual void serialize_parameters(const Denn::Parameters& args) override
        {			
            m_ostream << "\t\"arguments\" :" << std::endl;
			m_ostream << "\t{" << std::endl;
            //get only serializables
            std::vector < const Denn::Parameters::ParameterInfo* > params_serializable;
            for(auto& params : args.m_params_info)
            if(params.serializable())  params_serializable.emplace_back(&params);
            //serialize
            for(size_t i=0; i!=params_serializable.size() ;++i)
            {
                auto* variable = params_serializable[i]->m_associated_variable;
                m_ostream << "\t\t\""
                 << variable->name() << "\" : "
                 << variant_to_str(variable->variant()) 
                 << ((i+1)!=params_serializable.size() ? "," : "")
                 << std::endl;
            }

			m_ostream << "\t}," << std::endl;
        }
		
		virtual void serialize_best(double time, Denn::Scalar accuracy, Denn::Scalar f, Denn::Scalar cr, const Denn::NeuralNetwork& network) override
        {   
			m_ostream << "\t\"time\" : "     << time << "," << std::endl;
			m_ostream << "\t\"accuracy\" : " << accuracy << "," << std::endl;
			m_ostream << "\t\"f\" : " << f << "," << std::endl;
			m_ostream << "\t\"cr\" : " << cr << "," << std::endl;
			m_ostream << "\t\"network\" : [" << std::endl;
			for (size_t i = 0; i != network.size(); ++i)
			{
				m_ostream
				<< "\t\t[";
                for (size_t m = 0; m != network[i].size(); ++m)
                {
                    m_ostream
                    << to_string_matrix< Matrix > ( network[i][m] )
                    << ((m != network[i].size() - 1) ? "," : "");
                }
                m_ostream
				<< ((i != network.size() - 1) ? "]," : "]")
				<< std::endl;
			}
			m_ostream << "\t]" << std::endl;
        }

    private:

        template < typename T >
        static std::string to_string_matrix(const T& matrix)
        {
            Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
            std::ostringstream ssout; ssout << matrix.format(matrix_to_json_array); return ssout.str();
        }

        template< typename T >
        static std::string vector_to_str(const std::vector< T >& vec_of_t)
        {
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << std::to_string(vec_of_t[0]);
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << std::to_string(vec_of_t[i]);
            ssout << " ]";
            return ssout.str();
        }

        template< typename T >
        static std::string vector_to_str_matrix(const std::vector< T >& vec_of_t)
        {			
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << to_string_matrix(vec_of_t[0]);
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << to_string_matrix(vec_of_t[i]);
            ssout << " ]";
            return ssout.str();
        }

        static std::string vector_to_str_string(const std::vector< std::string >& vec_of_t)
        {			
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << "\"" << vec_of_t[0] << "\"";
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << "\"" << vec_of_t[i] << "\"";
            ssout << " ]";
            return ssout.str();
        }

        static std::string variant_to_str(const Variant& value)
        {			
            Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
            switch (value.get_type())
            {
            case VariantType::VR_NONE:     return "null";       break;
            case VariantType::VR_BOOL:     return value.get<bool>() ? "true" : "false";break;
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

            case VariantType::VR_FLOAT_MATRIX:        return to_string_matrix<MatrixF>(value.get<MatrixF>()); break;
            case VariantType::VR_DOUBLE_MATRIX:       return to_string_matrix<MatrixD>(value.get<MatrixD>()); break;
            case VariantType::VR_LONG_DOUBLE_MATRIX:  return to_string_matrix<MatrixLD>(value.get<MatrixLD>()); break;

            //case VariantType::VR_INDIVIDUAL:         (*this) = ref.get<Individual>(); break;
            //case VariantType::VR_POPULATION:         (*this) = ref.get<Population>(); break;

            case VariantType::VR_STD_VECTOR_INT:               return vector_to_str<int>(value.get< std::vector<int> >()); break;
            case VariantType::VR_STD_VECTOR_FLOAT:             return vector_to_str<float>(value.get< std::vector<float> >()); break;
            case VariantType::VR_STD_VECTOR_DOUBLE:            return vector_to_str<double>(value.get< std::vector<double> >()); break;
            case VariantType::VR_STD_VECTOR_LONG_DOUBLE:       return vector_to_str<long double>(value.get< std::vector<long double> >()); break;
            case VariantType::VR_STD_VECTOR_FLOAT_MATRIX:      return vector_to_str_matrix<MatrixF>(value.get< std::vector<MatrixF> >()); break;
            case VariantType::VR_STD_VECTOR_DOUBLE_MATRIX:     return vector_to_str_matrix<MatrixD>(value.get< std::vector<MatrixD> >()); break;
            case VariantType::VR_STD_VECTOR_LONG_DOUBLE_MATRIX:return vector_to_str_matrix<MatrixLD>(value.get< std::vector<MatrixLD> >()); break;

            case VariantType::VR_C_STRING:
            case VariantType::VR_STD_STRING:           return "\"" + value.get< std::string >() + "\""; break;
            case VariantType::VR_STD_VECTOR_STRING:    return vector_to_str_string(value.get< std::vector<std::string> >()); break;

            case VariantType::VR_PTR:				   return std::to_string(value.get<size_t>()); break;
            default: return ""; break;
            }
        }

    };

    class CSVSerializeOutput : public SerializeOutput
    {
    public:
    	
		CSVSerializeOutput(std::ostream& ostream = std::cout) : CSVSerializeOutput(false, ostream)
		{

		}
		CSVSerializeOutput(bool full_csv, std::ostream& ostream = std::cout) : SerializeOutput(ostream), m_full_csv(full_csv)
		{
		}

		virtual ~CSVSerializeOutput()
		{
		}

		virtual void serialize_parameters(const Denn::Parameters& args) override
        {			
            //get only serializables
            std::vector < const Denn::Parameters::ParameterInfo* > params_serializable;
            for(auto& params : args.m_params_info)
            if(params.serializable())  params_serializable.emplace_back(&params);
            //print header
            for(size_t i=0; i!=params_serializable.size() ;++i)
                m_ostream << params_serializable[i]->m_associated_variable->name() << "; ";
            m_ostream  << "time; accuracy; f; cr" << (m_full_csv ? "; network" : "");
            m_ostream  << std::endl; 
            //serialize
            for(size_t i=0; i!=params_serializable.size() ;++i)
                m_ostream << variant_to_str(params_serializable[i]->m_associated_variable->variant()) << "; ";       
        }
		
		virtual void serialize_best(double time, Denn::Scalar accuracy, Denn::Scalar f, Denn::Scalar cr, const Denn::NeuralNetwork& network) override
        {   
			m_ostream << time << "; " << accuracy << "; "  << f << "; " << cr;
            //no full?
            if(!m_full_csv)
            {
                m_ostream << std::endl;
                return;
            }
            //print NN
            for (size_t i = 0; i != network.size(); ++i)
			{
				m_ostream
				<< "[";
                for (size_t m = 0; m != network[i].size(); ++m)
                {
                    m_ostream
                    << to_string_matrix< Matrix > ( network[i][m] )
                    << ((m != network[i].size() - 1) ? "," : "");
                }
                m_ostream
				<< ((i != network.size() - 1) ? "]," : "]");
			}
			m_ostream << std::endl;

        }

    private:

        template < typename T >
        static std::string to_string_matrix(const T& matrix)
        {
            Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
            std::ostringstream ssout; ssout << matrix.format(matrix_to_json_array); return ssout.str();
        }

        template< typename T >
        static std::string vector_to_str(const std::vector< T >& vec_of_t)
        {
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << std::to_string(vec_of_t[0]);
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << std::to_string(vec_of_t[i]);
            ssout << " ]";
            return ssout.str();
        }

        template< typename T >
        static std::string vector_to_str_matrix(const std::vector< T >& vec_of_t)
        {			
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << to_string_matrix(vec_of_t[0]);
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << to_string_matrix(vec_of_t[i]);
            ssout << " ]";
            return ssout.str();
        }

        static std::string vector_to_str_string(const std::vector< std::string >& vec_of_t)
        {			
            std::stringstream ssout;
            ssout << "[ ";
            if (vec_of_t.size()) ssout << "\"" << vec_of_t[0] << "\"";
            for(size_t i=1; i < vec_of_t.size(); ++i) ssout << ", " << "\"" << vec_of_t[i] << "\"";
            ssout << " ]";
            return ssout.str();
        }

        static std::string variant_to_str(const Variant& value)
        {			
            Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
            switch (value.get_type())
            {
            case VariantType::VR_NONE:     return "null";                              break;
            case VariantType::VR_BOOL:     return value.get<bool>() ? "true" : "false";break;
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

            case VariantType::VR_FLOAT_MATRIX:        return to_string_matrix<MatrixF>(value.get<MatrixF>()); break;
            case VariantType::VR_DOUBLE_MATRIX:       return to_string_matrix<MatrixD>(value.get<MatrixD>()); break;
            case VariantType::VR_LONG_DOUBLE_MATRIX:  return to_string_matrix<MatrixLD>(value.get<MatrixLD>()); break;

            //case VariantType::VR_INDIVIDUAL:         (*this) = ref.get<Individual>(); break;
            //case VariantType::VR_POPULATION:         (*this) = ref.get<Population>(); break;

            case VariantType::VR_STD_VECTOR_INT:               return vector_to_str<int>(value.get< std::vector<int> >()); break;
            case VariantType::VR_STD_VECTOR_FLOAT:             return vector_to_str<float>(value.get< std::vector<float> >()); break;
            case VariantType::VR_STD_VECTOR_DOUBLE:            return vector_to_str<double>(value.get< std::vector<double> >()); break;
            case VariantType::VR_STD_VECTOR_LONG_DOUBLE:       return vector_to_str<long double>(value.get< std::vector<long double> >()); break;
            case VariantType::VR_STD_VECTOR_FLOAT_MATRIX:      return vector_to_str_matrix<MatrixF>(value.get< std::vector<MatrixF> >()); break;
            case VariantType::VR_STD_VECTOR_DOUBLE_MATRIX:     return vector_to_str_matrix<MatrixD>(value.get< std::vector<MatrixD> >()); break;
            case VariantType::VR_STD_VECTOR_LONG_DOUBLE_MATRIX:return vector_to_str_matrix<MatrixLD>(value.get< std::vector<MatrixLD> >()); break;

            case VariantType::VR_C_STRING:
            case VariantType::VR_STD_STRING:           return "\"" + value.get< std::string >() + "\""; break;
            case VariantType::VR_STD_VECTOR_STRING:    return vector_to_str_string(value.get< std::vector<std::string> >()); break;

            case VariantType::VR_PTR:				   return std::to_string(value.get<size_t>()); break;
            default: return ""; break;
            }
        }

        bool m_full_csv         { false };

    };
}