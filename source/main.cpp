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
#include "Denn.h"
#include "DennMainUtils.h"

namespace Denn
{
    struct Gate
    {
        using GateOp = std::function < Matrix (const std::vector< Matrix& >& input) > ;
        enum Type
        {
            CONST = 0,
            ONE   = 1,
            DOUBLE= 2
        };
        Type   m_type;
        GateOp m_function;
        
        Gate(Type type):m_type(type){}
    };
    
    using GateList = std::vector < Gate >;
    
    class NRam
    {
    public:
        NRam(  const size_t max_int
             , const size_t n_regs
             , const GateList& gates
             , const Denn::Parameters& parameters
             )
        : m_max_int(max_int)
        , m_n_regs(n_regs)
        , m_gates(gates)
        {
            //
            size_t i = 0;
            size_t output_size = 0;
            //a+b
            for(auto& gate : gates)
            {
                output_size += (n_regs + i++) * gate.m_type;
            }
            //c
            output_size += n_regs * (n_regs + gates.size());
            //f
            output_size += 1;
            //todo
            m_network = build_mlp_network(m_n_regs, output_size, parameters);
            //
            std::cout << "W: " << m_network[0][0].array().size() << std::endl;
            std::cout << "b: " << m_network[0][1].array().size() << std::endl;
            std::cout << "all: " << m_network[0][0].array().size() + m_network[0][1].array().size() << std::endl;
        }
        
    protected:
        
        size_t        m_max_int;
        size_t        m_n_regs;
        GateList      m_gates;
        NeuralNetwork m_network;
    };
}
int main(int argc, const char** argv)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    using namespace Denn;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    Parameters arguments(argc, argv);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    NRam test(
        10,
        4,
        { Gate(Gate::ONE), Gate(Gate::CONST), Gate(Gate::DOUBLE) },
        arguments
    );
    return 0;
}
