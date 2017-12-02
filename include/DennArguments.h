#pragma once
#include "Config.h"

namespace Denn
{

class Arguments
{

public:

	Arguments(int nargs, const char** vargs);

	const char* get_string();
	int get_int();
	bool get_bool();
	double get_double();
	
	int remaining() const;

	void back_of_one();
	bool start_with_minus() const;

private:

	//default, none
	template <typename T> struct Helper
	{
		static T get(Arguments& args) { return T(); }
	};
	//vector type
	template <typename T> struct Helper< std::vector<T> >
	{
		static std::vector<T> get(Arguments& args)
		{
			std::vector< T > strs;
			while (args.remaining() && !args.start_with_minus()) strs.push_back(args.get<T>());
			return strs;
		}
	};

public:

	template < class T > T get() { return Helper<T>::get(*this); }

protected:

	int		      m_rem_arg;
	const char**  m_pointer;

};


//CPP type argument
template <> struct Arguments::Helper< bool > { static bool get(Arguments& args) { return args.get_bool(); } };
template <> struct Arguments::Helper< char > { static char get(Arguments& args) { return args.get_string()[0]; } };
template <> struct Arguments::Helper< const char* > { static const char* get(Arguments& args) { return args.get_string(); } };
template <> struct Arguments::Helper< std::string > { static std::string get(Arguments& args) { return args.get_string(); } };

template <> struct Arguments::Helper< short > { static short get(Arguments& args) { return (short)args.get_int(); } };
template <> struct Arguments::Helper< int > { static int get(Arguments& args) { return args.get_int(); } };
template <> struct Arguments::Helper< long > { static long get(Arguments& args) { return args.get_int(); } };
template <> struct Arguments::Helper< long long > { static long long get(Arguments& args) { return args.get_int(); } };

template <> struct Arguments::Helper< unsigned short > { static unsigned short get(Arguments& args) { return (unsigned short)args.get_int(); } };
template <> struct Arguments::Helper< unsigned int > { static unsigned int get(Arguments& args) { return (unsigned int)args.get_int(); } };
template <> struct Arguments::Helper< unsigned long > { static unsigned long get(Arguments& args) { return (unsigned long)args.get_int(); } };
template <> struct Arguments::Helper< unsigned long long > { static unsigned long long get(Arguments& args) { return (unsigned long long)args.get_int(); } };

template <> struct Arguments::Helper< float > { static float get(Arguments& args) { return (float)args.get_double(); } };
template <> struct Arguments::Helper< double > { static double get(Arguments& args) { return args.get_double(); } };
template <> struct Arguments::Helper< long double > { static long double get(Arguments& args) { return args.get_double(); } };

}