//
// Created by Valerio Belli on 20/11/17.
//
#include "Denn.h"
#include "DennNRamEnhanced.h"
#include "DennNRamTask.h"
#include "DennDump.h"
#include  <cctype>
#include  <sstream>

namespace Denn
{
namespace NRamEnhanced
{
	////////////////////////////////////////////////////////////////////////////////////////
    NRamLayout::NRamLayout()
    {
    }

    void NRamLayout::init
    (
            const size_t batch_size
        ,   const size_t max_int
        ,   const size_t n_regs
        ,   const size_t timesteps
        ,   const int    sequence_size
        ,   const size_t registers_values_extraction_type
        ,   const Scalar entropy_term
        ,   const Scalar entropy_decay
        ,   const Scalar cost_regularization_term
        ,   const NRam::GateList& gates
        ,   const bool activate_curriculum_learning
        ,   const bool enhanced
    )
    {
        // values init
	    m_batch_size = batch_size;
        m_max_int = max_int;
        m_n_regs = n_regs;
        m_timesteps = timesteps;
        m_sequence_size = sequence_size;
        m_registers_values_extraction_type = registers_values_extraction_type;
        m_entropy_term = entropy_term;
        m_entropy_decay = entropy_decay;
        m_cost_regularization_term = cost_regularization_term;
	    m_gates = gates;
        m_activate_curriculum_learning = activate_curriculum_learning;
        m_enhanced = enhanced;

        // Structure composed by 6 values: 
        // (Gate to execute, 
        //  First Value, 
        //  Second Value, 
        //  Register update, 
        //  Pointer to the value to use to update the register, 
        //  Willingess)
        m_nn_output = 6; 
    }
	////////////////////////////////////////////////////////////////////////////////////////
	// debugger
	ExecutionDebug::ExecutionDebug(const NRamLayout& layout)
    {
        m_layout = layout;
    }
    ExecutionDebug::ExecutionDebug(const ExecutionDebug& debug)
    { 
        m_steps = debug.m_steps;
        m_layout = debug.m_layout;
    }
    void ExecutionDebug::push_step()
    {
        m_steps.resize(m_steps.size()+1);
    }
    void ExecutionDebug::push_op(const NRam::Gate& gate, const Matrix& m, const Matrix& circuit_configuration)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ m, circuit_configuration }));
    }
    void ExecutionDebug::push_op(const NRam::Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& m, const Matrix& circuit_configuration)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, m, circuit_configuration }));
    }
    void ExecutionDebug::push_op(const NRam::Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& b, const Matrix& in_b, const Matrix& m, const Matrix& circuit_configuration)
    {           
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, b, in_b, m, circuit_configuration }));
    }
    void ExecutionDebug::push_update(size_t r, const Matrix& c, const Matrix& in_c)
    {          
        m_steps[m_steps.size()-1].push_up(UpNode( r , Values{ c, in_c }));
    }
	//serialize help
    NRam::Gate::Arity ExecutionDebug::get_arity(size_t value_size)
    {
        switch(value_size)
        {
            default:
            case 2: return NRam::Gate::Arity::CONST;  
            case 4: return NRam::Gate::Arity::UNARY;   
            case 6: return NRam::Gate::Arity::BINARY; 
        }
    }
    //gate name
    static std::string get_gate_or_register_name_from_id(const NRam::GateList& gates,size_t regs,size_t value)
    {
        if ( value < regs ) return "R(" + std::to_string(value) + ")";
        size_t id = value-regs;
        //get
        if(id < gates.size())
        {
            std::string gate_name = gates[value-regs]->name();
            gate_name[0] = std::toupper(gate_name[0]);
            return gate_name;
        }
        //
        return "Unknow";
    }
	//shell
    // std::string ExecutionDebug::shell() const
    // {
    //     std::stringstream output;            
    //     for(size_t s = 0; s != m_steps.size(); ++s)
    //     {
    //         #define get_name_input_from(out_id)\
    //             get_gate_or_register_name_from_id( m_layout.m_gates, m_layout.m_n_regs,  values[out_id](0,0) )
    //         //get step
    //         auto& step = m_steps[s];
    //         //print step
    //         output << "Timestep [" << s << "]:" << std::endl;
    //         //get op
    //         for(size_t p = 0; p != step.m_ops.size(); ++p)
    //         {
    //             //operation
    //             auto& operation = step.m_ops[p];
    //             //name, value
    //             auto& name     = std::get<0>(operation);
    //             auto& values   = std::get<1>(operation);
    //             auto  arity    = get_arity(values.size());
    //             //gate name
    //             std::string  gate_name= name; 
    //             gate_name[0] = std::toupper(gate_name[0]);
    //             //output by type
    //             switch(arity)
    //             {
    //                 case Gate::Arity::CONST:
    //                     output << u8"\t• "
    //                            << gate_name << " => "
    //                            << values.back()(0,0)
    //                            << std::endl;
    //                 break;
    //                 case Gate::Arity::UNARY:
    //                     output << u8"\t• "
    //                            << gate_name 
    //                            << "(" 
    //                            << get_name_input_from(0)
    //                            << " : "
    //                            << values[1](0,0)
    //                            << ") => "
    //                            << values.back()(0,0)
    //                            << std::endl;
    //                 break;
    //                 case Gate::Arity::BINARY:
    //                     output << u8"\t• "
    //                            << gate_name 
    //                            << "(" 
    //                            << get_name_input_from(0)
    //                            << " : "
    //                            << values[1](0,0)
    //                            << ", "
    //                            << get_name_input_from(2)
    //                            << " : "
    //                            << values[3](0,0)
    //                            << ") => ";
    //                     //print memory (write)
    //                     if(gate_name == "Write")
    //                     {
    //                         output 
    //                         << values.back()(0,0) 
    //                         << ", mem["<<values[1](0,0)<<"] : "
    //                         << values[4](0,values[1](0,0)) 
    //                         << std::endl;
    //                     }
    //                     else 
    //                     {
    //                         output << values.back()(0,0) << std::endl;
    //                     }
    //                 break;
    //                 //none
    //                 default: break;
    //             }
    //         }
    //         //get update
    //         for(size_t p = 0; p != step.m_ups.size(); ++p)
    //         {
    //             //operation
    //             auto& update =  step.m_ups[p];
    //             //name, value
    //             auto& r      = std::get<0>(update);
    //             auto& values = std::get<1>(update);
    //             //update
    //             output << u8"\t• R" << r << " <= " << get_name_input_from(0) << " : " <<  values.back()(0,0) << std::endl;
    //         }
    //         // Print mem at last timestep
    //         auto& last_gate_op = step.m_ops.back();
    //         auto& values       = std::get<1>(last_gate_op);
    //         output << u8"\t• Mem " << Dump::json_matrix(values[values.size() - 2]) << std::endl;
    //         //undef get_value_from
    //         #undef get_name_input_from
       
    //     }
    //     return output.str();
    // }
	// //json
    // Json ExecutionDebug::json() const
	// {
	// 	//Array of steps
	// 	JsonArray jsteps;
	// 	//for each step
	// 	for (size_t s = 0; s != m_steps.size(); ++s)
	// 	{
	// 		//get step
	// 		auto& step = m_steps[s];
	// 		//Json array of ops
	// 		JsonArray jops;
	// 		//get op
	// 		for (size_t p = 0; p != step.m_ops.size(); ++p)
	// 		{
	// 			//Json op
	// 			JsonObject jop;
	// 			//operation
	// 			auto& operation = step.m_ops[p];
	// 			//name, value
	// 			auto& name = std::get<0>(operation);
	// 			auto& values = std::get<1>(operation);
	// 			auto  arity = get_arity(values.size());
	// 			//info json
	// 			jop["type"]  = "gate";
	// 			jop["name"]  = name;
	// 			jop["arity"] = arity == Gate::Arity::CONST
	// 						? "CONST" 
	// 						: arity == Gate::Arity::UNARY
	// 						? "UNARY"
	// 						: "BINARY";
	// 			//values
	// 			JsonArray jvalues;
	// 			for (auto& value : values)
	// 			{
	// 				//save matrix
	// 				jvalues.push_back(json_array_from_matrix(value));
	// 			}
	// 			//save values
	// 			jop["values"] = std::move(jvalues);
	// 			//save jop
	// 			jops.emplace_back(jop);
	// 		}
	// 		//get update
	// 		for (size_t p = 0; p != step.m_ups.size(); ++p)
	// 		{
	// 			//Json op
	// 			JsonObject jup;
	// 			//operation
	// 			auto& update = step.m_ups[p];
	// 			//name, value
	// 			auto& r = std::get<0>(update);
	// 			auto& values = std::get<1>(update);
	// 			//info update
	// 			jup["type"] = "update";
	// 			jup["register"] = int(r);
	// 			//values
	// 			JsonArray jvalues;
	// 			for (auto& value : values)
	// 			{
	// 				//save matrix
	// 				jvalues.push_back(json_array_from_matrix(value));
	// 			}
	// 			//save values
	// 			jup["values"] = std::move(jvalues);
	// 			//save jop
	// 			jops.emplace_back(jup);
	// 		}
	// 		//save jops in steps
	// 		jsteps.emplace_back(jops);
	// 	}
	// 	//return 
    //     return Json(jsteps);
    // }

    std::string ExecutionDebug::shell() const { return ""; }

    Json ExecutionDebug::json() const { return {}; }

	////////////////////////////////////////////////////////////////////////////////////////
	// NRam

    Matrix defuzzy_mem(const Matrix &M)
    {
        Matrix m(1, M.rows());
        for (Matrix::Index r = 0; r < M.rows(); r++)
        {
            Scalar        max_value = M(r, 0);
            Matrix::Index max_index = 0;
            for (Matrix::Index c = 1; c < M.cols(); ++c)
            {
                if (max_value < M(r, c))
                {
                    max_value = M(r, c);
                    max_index = c;
                }
            }
            m(0, r) = max_index;
        }
        return m;
    }

    Scalar calculate_sample_cost(const Matrix &M, const RowVector &desired_mem, const Matrix& linear_mask)
    {
        int s_cost = 0;
        for (size_t idx = 0; idx < M.cols(); ++idx)
            if (int(linear_mask(0, idx)) && int(M(0, idx)) != int(desired_mem(0, idx)))
                s_cost += 1;
        return s_cost;
    }

    Scalar run_gate(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem)
    {
        // Retrieve the gate to execute
        int gate_index = round(PointFunction::sigmoid(nn_out_decision(0, 0)) * (context.m_gates.size() - 1));
        auto& selected_gate = *context.m_gates[gate_index];

        // Retrieve the input(s) of the gate
        int first_input  = round(PointFunction::sigmoid(nn_out_decision(0, 1)) * (context.m_max_int - 1));
        int second_input = round(PointFunction::sigmoid(nn_out_decision(0, 2)) * (context.m_max_int - 1));

        // Prepare to the output of the gate
        Matrix regs_and_output(1, regs.cols() + 1);
        regs_and_output.block(0, 0, 1, regs.cols()) = regs;
        switch (selected_gate.arity())
        {
            case NRam::Gate::CONST:
                regs_and_output(0, regs.cols()) = defuzzy_mem(selected_gate(in_mem))(0, 0);
                break;
            case NRam::Gate::UNARY:
                regs_and_output(0, regs.cols()) = selected_gate(first_input, in_mem);
                break;
            case NRam::Gate::BINARY:
                regs_and_output(0, regs.cols()) = selected_gate(first_input, second_input, in_mem);
                break;
            default:
                break;
        }

        // Update a register with a new content
        int register_index  = round(PointFunction::sigmoid(nn_out_decision(0, 3)) * (context.m_n_regs - 1));
        int pointer         = round(PointFunction::sigmoid(nn_out_decision(0, 4)) * (context.m_n_regs));
        regs(0, register_index) = regs_and_output(0, pointer);

        return PointFunction::sigmoid(nn_out_decision(0, 5));
    }

	////////////////////////////////////////////////////////////////////////////////////////
	//Train
	//tlocal
    Scalar train
    (
      const NRamLayout &context
    , const NeuralNetwork& network
    , const Matrix& linear_test_in_mem
    , const Matrix& linear_test_desired_mem
    , const Matrix& linear_mask
    , const size_t& dataset_max_int
    , const size_t& dataset_timesteps
    )
    {
        //init by threads
		Matrix regs;
		Matrix in_mem;
		Matrix circuit_configuration;
        Scalar full_cost(0.0);

		//max int and steps
		size_t max_int = std::max(context.m_max_int, dataset_max_int);
		size_t loop_timesteps = std::max(context.m_timesteps, dataset_timesteps);
        
        // Run the circuit
        for (Matrix::Index s = 0; s < linear_test_in_mem.rows(); ++s)
        {
            // Alloc regs and initialize
            regs = Matrix::Zero(1, context.m_n_regs);

            // In mem fuzzy
			in_mem = linear_test_in_mem.row(s);

            // Cost helper
            Scalar p_t = Scalar(0.0);
            Scalar prob_incomplete = Scalar(1.0);
            Scalar cum_prob_complete = Scalar(0.0);
            Scalar sample_cost = Scalar(0.0);
            // Execute sa sample for all timestep
            for (size_t timestep = 0; timestep < loop_timesteps; timestep++)
            {
				// NN
                Matrix nn_example(1, context.m_n_regs + 1);
                nn_example.leftCols(context.m_n_regs) = regs;
                nn_example.rightCols(1) = RowVector::Constant(1, timestep);
                circuit_configuration = network.apply(regs);

                Scalar fi = run_gate(context, circuit_configuration, regs, in_mem);

                // Calculate p_t
                if (timestep == loop_timesteps - 1)  p_t = 1 - cum_prob_complete;
                else                                 p_t = fi * prob_incomplete;

                // // Calculate the probability of not complete 
                prob_incomplete *= (Scalar(1.0) - fi);
                cum_prob_complete += p_t;
                
				// // Compute the "sample" cost                
                Scalar cost = calculate_sample_cost(in_mem, linear_test_desired_mem.row(s), linear_mask);
                sample_cost += p_t * cost;

                if (fi >= 1.0) break;
            }
            // Add to "batch" cost the "sample" cost
            full_cost += sample_cost;
        }
        return full_cost;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // Calculate error rate
    Scalar calculate_error_rate(
            const NRamLayout&       context 
        ,   const NeuralNetwork&    network
        ,   const Matrix&           linear_test_in_mem
        ,   const Matrix&           linear_test_desired_mem
        ,   const Matrix&           linear_mask
        ,   const Matrix&           error_m
        ,   const size_t&           d_max_int
        ,   const size_t&           d_timesteps
    )
    {
        //init by threads
		Matrix regs;
		Matrix in_mem;
		Matrix circuit_configuration;

		//max int and steps
		const size_t max_int = std::max(context.m_max_int, d_max_int);
		const size_t timesteps = std::max(context.m_timesteps, d_timesteps);

        // Error rate variables
        Scalar c(0);
        const Scalar m(error_m.sum());

        // Run the circuit
        for (Matrix::Index s = 0; s < linear_test_in_mem.rows(); ++s)
        {
            // Alloc regs and initialize
            regs = Matrix::Zero(1, context.m_n_regs);
            regs.col(0).fill(1);

            // In mem fuzzy
			in_mem = linear_test_in_mem.row(s);

            // Execute sa sample for all timestep
            for (size_t timestep = 0; timestep < timesteps; timestep++)
            {
				// NN
                Matrix nn_example(1, context.m_n_regs + 1);
                nn_example.leftCols(context.m_n_regs) = regs;
                nn_example.rightCols(1) = RowVector::Constant(1, timestep);
				circuit_configuration = network.apply(nn_example).transpose();

                // Run circuit
                if (run_gate(context, circuit_configuration, regs, in_mem) >= 1.0) break;
            }

            // Calculate error rate for the sample
            in_mem = defuzzy_mem(in_mem);
            for (size_t col = 0; col < in_mem.cols(); ++col)
                if (error_m(s, col) 
                    && Matrix::Index(in_mem(0, col)) == Matrix::Index(linear_test_desired_mem(s, col))) 
                    c += 1;
        }

        return (1 - (c / m)); // Error rate
    }
	////////////////////////////////////////////////////////////////////////////////////////
}
}