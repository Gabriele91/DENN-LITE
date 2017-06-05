#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"

#define STR_PRINT_ATTRIBUTES(...)  #__VA_ARGS__; __VA_ARGS__

int main()
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	//to file
	std::ofstream out("logs.out");
	std::cout.rdbuf(out.rdbuf());
	////////////////////////////////////////////////////////////////////////////////////////////////
	//std::srand(std::time(NULL));
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel (OpenMP)
	#ifdef D_OPEN_MP_SUPPORTED_
	int    n_openMP_threads = 4;
	omp_set_num_threads(n_openMP_threads);
	Eigen::setNbThreads(n_openMP_threads);
	Eigen::initParallel();
	#endif
	//parallel (Thread Pool)
	size_t n_denn_threads = 8;
	ThreadPool thpool(n_denn_threads);
	////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
	auto str_attributes =
	STR_PRINT_ATTRIBUTES(
	using Scalar   = float;
	using MLP	   = PerceptronNetworkF;
	using LP	   = PerceptronLayerF;
	using DSLoader = Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> >;
	using DennAlgo = DennAlgoritem< MLP, DSLoader >;
	DSLoader dataset("assets/ARTICLE_MNIST_5_12x4000_1s.gz");
	size_t gen_tot    = 7000;
	size_t gen_step   = 1000;
	size_t pop_size   = 96;
	Scalar f_default  = 0.9;
	Scalar cr_default = 0.9;
	Scalar clamp_max  = 30.0;
	Scalar clamp_min  = -30.0;
	Scalar range_max  = 2.0;
	Scalar range_min  = -2.0;
	auto   cost_function = CostFunction::softmax_cross_entropy_with_logit<MLP::MatrixType>;
	)
#else
	auto str_attributes =
	STR_PRINT_ATTRIBUTES(
	using Scalar   = double;
	using MLP      = PerceptronNetworkD;
	using LP	   = PerceptronLayerD;
	using DSLoader = Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> >;
	using DennAlgo = DennAlgoritem< MLP, DSLoader >;
	//DATASET
	DSLoader dataset("assets/iris_105x6_5s.gz");
	//info
	size_t gen_tot		= 5000;
	size_t gen_step		= 40;
	size_t pop_size		= 30;
	Scalar f_default	= 0.6;
	Scalar cr_default	= 0.8;
	Scalar clamp_max     =  15.0;
	Scalar clamp_min     = -15.0;
	Scalar range_max     =  2.0;
	Scalar range_min     = -2.0;
	auto   cost_function = CostFunction::softmax_cross_entropy_with_logit<MLP::MatrixType>;
	)
#endif	
	// NETWORK
	size_t n_features = dataset.get_main_header_info().m_n_features;
	size_t n_class = dataset.get_main_header_info().m_n_classes;
	MLP nn0
	(
		LP(n_features, n_class) //Input layer
	);
	//DENN
	DennAlgo denn
	(
		  &dataset
		, pop_size
		, nn0
		, { Scalar(0.1), Scalar(0.1) }	// jde f, cr
		, { f_default, cr_default    }	// crf_default  //default f,cr
		, { clamp_min, clamp_max     }
		, { 2, Scalar(0.02)			 }  //restart
		, { range_min, range_max     }  //random
		, cost_function
	);
	//Init population
	denn.init();
	//Execute DENN
	double time = Time::get_time();
	auto result = denn.execute(gen_tot, gen_step, &thpool);
	time = Time::get_time() - time;
	//output
	{
		/////////////////////////////////////////////////////////
		std::cout << "Time: " << time << "s" << std::endl;
		std::cout << "Result: " << denn.execute_test(result) << " accuracy" << std::endl;
		std::cout << "Attributes:\n{\n " << str_replace(str_attributes, ";", ";\n") << "}" << std::endl;
		std::cout << "Individual JSON: " << std::endl;
		std::cout << "{" << std::endl;
		std::cout << "\t\"f\" : "    << result.m_f    << "," << std::endl;
		std::cout << "\t\"cr\" : "   << result.m_cr   << "," << std::endl;
		std::cout << "\t\"eval\" : " << result.m_eval << "," << std::endl;
		std::cout << "\t\"network\" : [" << std::endl;
		Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
		for (size_t i = 0; i!=result.m_network.size(); ++i)
		{
			std::cout
				<< "\t\t[" 
				<< result.m_network[i].weights().format(matrix_to_json_array)
				<< "," 
				<< result.m_network[i].baias().format(matrix_to_json_array)
				<< ((i != result.m_network.size() - 1) ? "]," : "]")
				<< std::endl;
		}
		std::cout << "\t]" << std::endl;
		std::cout << "}" << std::endl;
		/////////////////////////////////////////////////////////
	}

	return 0;
}

#if 0
int main()
{
	//to file
	std::ofstream out("logs.out");
	std::cout.rdbuf(out.rdbuf());
	/////////////////////////////////////////
	size_t n_features = 28 * 28;
	size_t n_hidden   = 200;
	size_t n_class    = 10;
	PerceptronNetworkF nn0
	(
		PerceptronLayerF(sigmoidF, n_features, n_class) //Input layer
	);
	/////////////////////////////////////////
	Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> > dataset("assets/ARTICLE_MNIST_5_12x4000_1s.gz");
	/////////////////////////////////////////
	DataSetRaw< float > test_set;
	if (!dataset.read_test(test_set)) return -1;
	/////////////////////////////////////////
	double performance = Time::get_ms_time();
	auto  y	           = nn0.apply(test_set.m_features);
	auto& labels       = test_set.m_labels;
	performance        = Time::get_ms_time() - performance;
	//
	std::cout << "--------labels----------" << std::endl;
	std::cout << labels << std::endl;
	std::cout << "-----------y------------" << std::endl;
	std::cout << y << std::endl;
	/*
	std::cout << "------cross_entropy-----" << std::endl;
	std::cout << CostFunction::cross_entropy(labels, y) << std::endl;
	*/
	std::cout << "--reduce_cross_entropy--" << std::endl;
	std::cout << CostFunction::reduce_cross_entropy(labels, y) << std::endl;
	/*
	std::cout << "---correct_prediction---" << std::endl;
	std::cout << CostFunction::correct_prediction(labels, y) << std::endl;
	*/
	std::cout << "--------accuracy--------"        << std::endl;
	std::cout << CostFunction::accuracy(labels, y) << std::endl;
	std::cout << "--------performance--------"     << std::endl;
	std::cout << performance << std::endl;
	return 0;
}
#endif