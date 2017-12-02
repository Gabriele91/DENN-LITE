#pragma once
#include "Config.h"
#include "DennRandom.h"
#include "DennVariant.h"
#include "DennArguments.h"
#include <string>
#include <vector>

namespace Denn
{
	//////////////////////////////////////////////
	class  Parameters;
	class  GenericReadOnly;
	struct ParameterDomain;
	struct ParameterOwner;
	struct ParameterInfo;
	//////////////////////////////////////////////
	class GenericReadOnly
	{
	public:
		virtual std::string name()  const = 0;
		virtual Variant variant()   const = 0;
		virtual bool serializable() const = 0;
	};

	template <class T>
	class ReadOnly : public GenericReadOnly
	{
	public:

		//cast
		template < class X >
		inline operator X() const { return X(m_data); }
		//no cast
		const T& operator *() const { return m_data; }
		//no cast
		const T& get() const { return m_data; }
		//get variant
		virtual Variant variant() const	  override { return get(); }
		//get name
		virtual std::string name() const  override { return m_name; }
		//get if serializable
		virtual bool serializable() const override { return m_serializable; }

	private:


		//init
		ReadOnly() {}
		ReadOnly(const std::string& name) { m_name = name; }
		//init + value
		template < typename I >
		ReadOnly(const std::string& name, const I& arg, bool serializable = true)
		{
			m_name = name;
			m_data = T(arg);
			m_serializable = serializable;
		}
		//set value
		template < typename I >
		T operator=(const I& arg)
		{
			m_data = T(arg);
			return m_data;
		}

		//get (no const) value
		T& get() { return m_data; }

		//name/data/serializable
		std::string m_name;
		T m_data;
		bool m_serializable{ true };

		//friends
		friend class Parameters;
		friend struct ParameterInfo;

	};

	struct ParameterDomain
	{
		enum TypeDomain
		{
			NONE,
			MONO,
			CHOISE,
		};
		TypeDomain  m_type{ NONE };
		std::string m_domain{ "void" };
		std::vector< std::string > m_choises;

		ParameterDomain();
		ParameterDomain(const std::string& domain);
		ParameterDomain(const std::string& domain, const std::vector< std::string >& choises);
	};

	struct ParameterOwner
	{
		enum Filter
		{
			NONE,
			ALL,
			EXCEPT,
			ONLY
		};
		const Filter			     m_filter_type;
		const std::vector< Variant > m_filter_values;
		const GenericReadOnly*       m_owner_variable;

		ParameterOwner();

		bool test(const ParameterInfo& owner) const;

		template< class T >
		ParameterOwner(const ReadOnly< T >& owner)
		: m_filter_type(ALL)
		, m_owner_variable(&owner)
		{

		}

		template< class T >
		ParameterOwner(const ReadOnly< T >& owner, const std::vector< Variant >& value)
		: m_filter_type(ONLY)
		, m_filter_values(value)
		, m_owner_variable(&owner)
		{

		}

		template< class T >
		ParameterOwner(const ReadOnly< T >& owner, const Filter filter, const std::vector< Variant >& value)
		: m_filter_type(filter)
		, m_filter_values(value)
		, m_owner_variable(&owner)
		{

		}
	};

	struct ParameterInfo
	{

		const GenericReadOnly*		      m_associated_variable;
		const ParameterOwner			  m_owener;
		std::string				   		  m_description;
		std::vector< std::string  > 	  m_arg_key;
		std::function< bool(Arguments&) > m_action;
		ParameterDomain                   m_domain;
		
		ParameterInfo();

		template< class T >
		ParameterInfo
		(
			  ReadOnly< T >&			          associated_variable
			, const std::string&				  description
			, const std::vector< std::string  >&  arg_key
			, std::function< bool(Arguments&) >   action = nullptr
			, const ParameterDomain&              domain = ParameterDomain(cpp_type_to_arg_type<T>())
		)
		: m_associated_variable(&associated_variable)
		, m_description(description)
		, m_arg_key(arg_key)
		, m_domain(domain)
		{
			m_action = action ? action : [&](Arguments& args) -> bool { associated_variable = args.get<T>(); return true; };
			m_arg_key.insert(m_arg_key.begin(), "--" + associated_variable.name());
		}

		template< class T >
		ParameterInfo
		(
			  ReadOnly< T >&			          associated_variable
			, const ParameterOwner			      owener
			, const std::string&				  description
			, const std::vector< std::string  >&  arg_key
			, std::function< bool(Arguments&) >   action = nullptr
			, const ParameterDomain&              domain = ParameterDomain(cpp_type_to_arg_type<T>())
		)
			: m_associated_variable(&associated_variable)
			, m_owener(owener)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_domain(domain)
		{
			m_action = action ? action : [&](Arguments& args) -> bool { associated_variable = args.get<T>(); return true; };
			m_arg_key.insert(m_arg_key.begin(), "--" + associated_variable.name());
		}

		ParameterInfo
		(
			  const std::string&				   	description
			, const std::vector< std::string  >& 	arg_key
			, std::function< bool(Arguments&) >     action
			, const ParameterDomain&                domain = ParameterDomain(std::string())
		);

		ParameterInfo
		(
			  const std::string&				  description
			, const ParameterOwner			      owener
			, const std::vector< std::string  >&  arg_key
			, std::function< bool(Arguments&) >   action
			, const ParameterDomain&              domain = ParameterDomain(std::string())
		);

		bool has_an_associated_variable() const;

		bool serializable() const;

	protected:

		template< class T > static const char* cpp_type_to_arg_type() { return "void"; }

	};
	//////////////////////////////////////////////
	//CPP type to str param
	extern template const char* ParameterInfo::cpp_type_to_arg_type<bool>();

	extern template const char* ParameterInfo::cpp_type_to_arg_type<char>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< const char* >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::string >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector< std::string > >();

	extern template const char* ParameterInfo::cpp_type_to_arg_type<short>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<int>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<long>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<long long>();

	extern template const char* ParameterInfo::cpp_type_to_arg_type<unsigned short>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<unsigned int>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<unsigned long>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<unsigned long long>();

	extern template const char* ParameterInfo::cpp_type_to_arg_type<float>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<double>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<long double>();

	extern template const char* ParameterInfo::cpp_type_to_arg_type<MatrixF>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<MatrixD>();
	extern template const char* ParameterInfo::cpp_type_to_arg_type<MatrixLD>();

	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<short> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<int> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long long> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned short> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned int> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long long> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<float> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<double> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<long double> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixF> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixD> >();
	extern template const char* ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixLD> >();
	//////////////////////////////////////////////
}