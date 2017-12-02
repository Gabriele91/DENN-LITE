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
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ParameterInfo::ParameterInfo() { }

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
}