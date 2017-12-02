#include "DennParameterInfo.h"

namespace Denn
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ParameterDomain
	ParameterDomain::ParameterDomain() {}

	ParameterDomain::ParameterDomain(const std::string& domain)
	: m_type(MONO)
	, m_domain(domain)
	{
	}

	ParameterDomain::ParameterDomain(const std::string& domain, const std::vector< std::string >& choises)
	: m_type(CHOISE)
	, m_domain(domain)
	, m_choises(choises)
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ParameterOwner::ParameterOwner()
	: m_filter_type(NONE)
	, m_owner_variable(nullptr)
	{
	}
	bool ParameterOwner::test(const ParameterInfo& owner) const
	{
		//ignore test
		if (!owner.has_an_associated_variable() && !m_owner_variable) return true;
		//associated to other owner
		if (owner.m_associated_variable != m_owner_variable) return false;
		//test
		switch (m_filter_type)
		{
		//all type
		default:
		case NONE:
		case ALL: return true;
		//not in
		case EXCEPT:
			for (const Variant& value : m_filter_values)
			{
				if (value.equal(owner.m_associated_variable->variant(), false)) return false;
			}
			return true;
		//in
		case ONLY:
			for (const Variant& value : m_filter_values)
			{
				if (value.equal(owner.m_associated_variable->variant(), false)) return true;
			}
			return false;
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ParameterInfo::ParameterInfo() : m_associated_variable(nullptr) { }

	ParameterInfo::ParameterInfo
	(
		  const std::string&				   	description
		, const std::vector< std::string  >& 	arg_key
		, std::function< bool(Arguments&) >     action
		, const ParameterDomain&                domain
	)
	: m_associated_variable(nullptr)
	, m_description(description)
	, m_arg_key(arg_key)
	, m_action(action)
	, m_domain(domain)
	{
	}

	ParameterInfo::ParameterInfo
	(
		  const std::string&				  description
		, const ParameterOwner			      owener
		, const std::vector< std::string  >&  arg_key
		, std::function< bool(Arguments&) >   action
		, const ParameterDomain&              domain
	)
	: m_associated_variable(nullptr)
	, m_owener(owener)
	, m_description(description)
	, m_arg_key(arg_key)
	, m_action(action)
	, m_domain(domain)
	{
	}

	bool ParameterInfo::has_an_associated_variable() const
	{
		return m_associated_variable != nullptr;
	}

	bool ParameterInfo::serializable() const
	{
		return has_an_associated_variable() && m_associated_variable->serializable();
	}
	//////////////////////////////////////////////
	//CPP type to str param
	template<> const char* ParameterInfo::cpp_type_to_arg_type<bool>() { return "bool"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type<char>() { return "string"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< const char* >() { return "string"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::string >() { return "string"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector< std::string > >() { return "list(string)"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type<short>() { return "int"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<int>() { return "int"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<long>() { return "int"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<long long>() { return "int"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type<unsigned short>() { return "uint"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<unsigned int>() { return "uint"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<unsigned long>() { return "uint"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<unsigned long long>() { return "uint"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type<float>() { return "float"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<double>() { return "float"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<long double>() { return "float"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type<MatrixF>() { return "matrix"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<MatrixD>() { return "matrix"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type<MatrixLD>() { return "matrix"; }

	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<short> >() { return "list(int)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<int> >() { return "list(int)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long> >() { return "list(int)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long long> >() { return "list(int)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned short> >() { return "list(uint)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned int> >() { return "list(uint)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long> >() { return "list(uint)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long long> >() { return "list(uint)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<float> >() { return "list(float)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<double> >() { return "list(float)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long double> >() { return "list(float)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixF> >() { return "list(matrix)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixD> >() { return "list(matrix)"; }
	template<> const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixLD> >() { return "list(matrix)"; }
}