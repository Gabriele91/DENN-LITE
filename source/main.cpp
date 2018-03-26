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

int main(int argc,const char** argv)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Denn;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Parameters parameters;
    //parsing
    if (!parameters.get_params(argc, argv))
    {
        std::cout << "Parameter error" << std::endl;
        return 64;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Instance::SPtr instance = InstanceFactory::create(parameters.m_instance, parameters);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //execute test
    if (!instance)            return 128;
    if (!instance->execute()) return 255;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    // Random				  m_random_engine;
    // Denn::NRam::Task::SPtr task = Denn::NRam::TaskFactory::create(parameters.m_task, 2, parameters.m_max_int, parameters.m_n_registers, 
    //      parameters.m_time_steps, parameters.m_sequence_size, 0, 0, 
    //         100, 0.05, m_random_engine
    //     );
    
    // auto batch = task->create_batch(0, Scalar(0.0));
    // auto& in_mem = std::get<0>(batch);
    // auto& out_mem = std::get<1>(batch);
    // auto& mask = std::get<2>(batch);
    // auto& error_mask = std::get<5>(batch);
    // MESSAGE(Dump::json_matrix(in_mem))
    // MESSAGE(Dump::json_matrix(out_mem))
    // MESSAGE(Dump::json_matrix(mask))
    // MESSAGE(Dump::json_matrix(error_mask))
    //lol 1 2 3 4
    return 0;
}
