//
//  main.cpp
//  DENN
//
//  Created by Gabriele Di Bari on 14/11/17.
//  Copyright © 2017 Gabriele. All rights reserved.
//
#include <iostream>
#include <ctime>
#include <fstream>
#include <utility>
#include "Denn.h"
#include "DennDump.h"
#include "DennNRam.h"
#include "DennMainUtils.h"

namespace Denn
{
    using MatrixList  = std::vector<Matrix>;
    using GateList    = std::vector< Gate::SPtr>;
    using MemoryTuple = std::tuple<Matrix, Matrix, Matrix>;

    class DataSetNRAM : public DataSetScalar
    {
    public:
        Eigen::Matrix< Scalar, Eigen::Dynamic, Eigen::Dynamic > m_in_mem;

        virtual void* ptr_in_mem() const { return (void*)&m_in_mem; }

        const Matrix& in_mem() const
        {
            return *((const Matrix*)(ptr_in_mem()));
        }

        Matrix& in_mem()
        {
            return *((Matrix*)(ptr_in_mem()));
        }
    };

    class DataSetTask : public DataSetLoader
    {
    public:

        ///////////////////////////////////////////////////////////////////
        DataSetTask(NRam::Task* task)
        {
            //
            m_fake_header.m_n_batch    = 1;
            m_fake_header.m_n_classes  = task->get_max_int();
            m_fake_header.m_n_features = task->get_max_int();
            //
            m_fake_train_header.m_n_row= task->get_batch_size();
            //init
            auto train       = (*task)();
            auto test        = (*task)();
            auto validation  = (*task)();

            m_train.m_features      = std::get<0>(train);
            m_train.m_labels        = std::get<1>(train);
//            m_train.m_in_mem        = std::get<0>(train);

            m_test.m_features       = std::get<0>(test);
            m_test.m_labels         = std::get<1>(test);
//            m_test.m_in_mem         = std::get<0>(test);

            m_validation.m_features = std::get<0>(validation);
            m_validation.m_labels   = std::get<1>(validation);
//            m_validation.m_in_mem   = std::get<0>(validation);

        }
        ///////////////////////////////////////////////////////////////////
        bool open(const std::string& path_file) override
        {
            return true;
        }

        bool is_open() const override
        {
            return true;
        }

        const DataSetHeader& get_main_header_info() const override
        {
            return m_fake_header;
        }

        const DataSetTrainHeader& get_last_batch_info() const override
        {
            return m_fake_train_header;
        }

        ///////////////////////////////////////////////////////////////////
        // READ TEST SET
        bool read_test(DataSet& t_out) override
        {
            auto& d_out = (*((DataSetScalar*)& t_out));
            d_out = m_test;
            return true;
        }

        ///////////////////////////////////////////////////////////////////
        // READ VALIDATION SET
        bool read_validation(DataSet& t_out) override
        {
            auto& d_out =(*((DataSetScalar*)& t_out));
            d_out = m_validation;
            return true;
        }

        ///////////////////////////////////////////////////////////////////
        // READ TRAINING SET
        bool start_read_batch() override
        {
            return true;
        }

        bool read_batch(DataSet& t_out, bool loop = true) override
        {
            auto& d_out =(*((DataSetScalar*)& t_out));
            d_out = m_train;
            return true;
        }

    protected:

        DataSetScalar    m_test;
        DataSetScalar    m_validation;
        DataSetScalar    m_train;

        DataSetHeader       m_fake_header;
        DataSetTrainHeader  m_fake_train_header;
    };
}

void execute
(
    const Denn::Parameters&       parameters
    , Denn::DataSetLoader&        dataset
    , Denn::ThreadPool*           ptr_thpool
    , std::ostream&               output
    , Denn::SerializeOutput::SPtr serialize_output
)
{
    using namespace Denn;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Create context
    NRam::NRamLayout context(1000, 10, 4, 9, parameters);
    //Dataset
    DataSetTask db_copy(context.m_task);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    NeuralNetwork nn0 = build_mlp_network(context.m_n_regs, context.m_nn_output, parameters);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //Loss function
    auto loss_function =
    [&](const Individual& individual, const DataSet& ds) -> Scalar
    {
        //network
        auto& nn      = individual.m_network;

        auto task     = context.m_task;

        //Dataset
        auto& in_mem  = ds.features();
        auto& out_mem = ds.labels();

        auto regs    = NRam::fuzzy_regs(context.m_batch_size, context.m_n_regs, context.m_max_int);

        //to list
        auto list_in_mem = NRam::fuzzy_encode(in_mem);
        //execute
        return NRam::train(context, nn, regs, list_in_mem, out_mem);
    };
    //DENN
    DennAlgorithm denn
    (
          &db_copy
        , parameters
        , nn0
        , loss_function
        //output
        , output
        //thread pool
        , ptr_thpool
    );
    //execute
    double execute_time = Time::get_time();
    auto result = denn.execute();
    execute_time = Time::get_time() - execute_time;
    //test
    DataSetScalar test_set;
    db_copy.read_test(test_set);
    auto test = loss_function(*result, test_set);
    //output
    serialize_output->serialize_parameters(parameters);
    serialize_output->serialize_best
            (
                    execute_time
                    , test //denn.execute_test(*result)
                    , result->m_f
                    , result->m_cr
                    , result->m_network
            );
    //output
    nn0 = result->m_network;
    ////////////////////////////////////////////////////////////////////////////////////////////////
}

int main(int argc,const char** argv)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Denn;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Parameters arguments(argc, argv);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //parallel (OpenMP)
#ifdef EIGEN_HAS_OPENMP
    if (*arguments.m_threads_omp)
	{
		omp_set_num_threads((int)*arguments.m_threads_omp);
		Eigen::setNbThreads((int)*arguments.m_threads_omp);
		Eigen::initParallel();
	}
#endif
    //parallel (Thread Pool)
    //ptr
    std::unique_ptr<ThreadPool> uptr_thpool;
    //alloc new ThreadPool
    if(*arguments.m_threads_pop)
    {
        uptr_thpool = std::make_unique<ThreadPool>(*arguments.m_threads_pop);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
    //test file name
    if(!(*arguments.m_dataset_filename).size()) return -1;
    //test file
    if(!Denn::Filesystem::exists((const std::string&)arguments.m_dataset_filename))
    {
        std::cerr << "input file: \"" << *arguments.m_dataset_filename << "\" not exists!" << std::endl;
        return 1; //exit
    }
    //get loader
    DataSetLoader::SPtr dataset = get_datase_loader((const std::string&)arguments.m_dataset_filename);
    //test loader
    if(!dataset)
    {
        std::cerr << "input file: \"" << *arguments.m_dataset_filename << "\" not supported!" << std::endl;
        return 1; //exit
    }
#else
    DataSetLoader::SPtr dataset = nullptr;
#endif
    ////////////////////////////////////////////////////////////////////////////////////////////////
    std::ostream   runtime_output_stream(nullptr);
    std::ofstream  runtime_output_file_stream;
    std::string    runtime_output_arg = (*arguments.m_runtime_output_file);
    //output
    if(runtime_output_arg.size())
    {
        if(runtime_output_arg == "::cout")
        {
            runtime_output_stream.rdbuf(std::cout.rdbuf());
        }
        else if(runtime_output_arg == "::cerr")
        {
            runtime_output_stream.rdbuf(std::cerr.rdbuf());
        }
        else if(!Denn::Filesystem::exists((const std::string&)arguments.m_runtime_output_file) ||
                Denn::Filesystem::is_writable((const std::string&)arguments.m_runtime_output_file))
        {
            runtime_output_file_stream.open(*arguments.m_runtime_output_file);
            runtime_output_stream.rdbuf(runtime_output_file_stream.rdbuf());
        }
        else
        {
            std::cerr << "can't write into the file: \"" << *arguments.m_runtime_output_file << "\"" << std::endl;
            return 1; //exit
        }
    }
    else
    {
        runtime_output_stream.rdbuf(std::cout.rdbuf());
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    if(Denn::Filesystem::exists((const std::string&)arguments.m_output_filename) &&
       !Denn::Filesystem::is_writable((const std::string&)arguments.m_output_filename))
    {
        std::cerr << "can't write into the file: \"" << *arguments.m_output_filename << "\"" << std::endl;
        return 1; //exit
    }
    std::ofstream	ofile((const std::string&)arguments.m_output_filename);
    //extension
    std::string ext = Denn::Filesystem::get_extension(*arguments.m_output_filename);
    std::transform(ext.begin(),ext.end(), ext.begin(), ::tolower);
    if (!SerializeOutputFactory::exists(ext))
    {
        std::cerr << "can't serialize a file with extension \"" << ext << "\"" << std::endl;
        return 1; //exit
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    SerializeOutput::SPtr serialize_output = SerializeOutputFactory::create(ext, ofile, arguments);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //execute test
    execute(arguments, *dataset, uptr_thpool.get(), runtime_output_stream, serialize_output);
    ////////////////////////////////////////////////////////////////////////////////////////////////

    return 0;
}

