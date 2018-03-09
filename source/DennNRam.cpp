//
// Created by Valerio Belli on 20/11/17.
//
#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include "DennDump.h"
#include  <cctype>
#include  <sstream>

namespace Denn
{
namespace NRam
{
	////////////////////////////////////////////////////////////////////////////////////////
    NRamLayout::NRamLayout()
    {
    }

    void NRamLayout::init
    (
        const size_t batch_size,
        const size_t max_int,
        const size_t n_regs,
        const size_t timesteps,
        const size_t registers_values_extraction_type,
        const Scalar entropy_term,
        const Scalar entropy_decay,
        const Scalar cost_regularization_term,
        const GateList& gates
    )
    {
        // values init
	    m_batch_size = batch_size;
        m_max_int = max_int;
        m_n_regs = n_regs;
        m_timesteps = timesteps;
        m_registers_values_extraction_type = registers_values_extraction_type;
        m_entropy_term = entropy_term;
        m_entropy_decay = entropy_decay;
        m_cost_regularization_term = cost_regularization_term;
	    m_gates = gates;
        // Past cardinality
        size_t i = 0;
        m_nn_output = 0;
        for (auto &gate : m_gates) 
		{
            m_nn_output += (n_regs + i++) * gate->arity();
        }

        // Same size for every c_i
        for (size_t j = 0; j < n_regs; ++j) 
		{
            m_nn_output += n_regs + i;
        }

        // Size for f_t
        m_nn_output += 1;
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
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& m, const Matrix& circuit_configuration)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ m, circuit_configuration }));
    }
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& m, const Matrix& circuit_configuration)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, m, circuit_configuration }));
    }
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& b, const Matrix& in_b, const Matrix& m, const Matrix& circuit_configuration)
    {           
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, b, in_b, m, circuit_configuration }));
    }
    void ExecutionDebug::push_update(size_t r, const Matrix& c, const Matrix& in_c)
    {          
        m_steps[m_steps.size()-1].push_up(UpNode( r , Values{ c, in_c }));
    }
	//serialize help
    Gate::Arity ExecutionDebug::get_arity(size_t value_size)
    {
        switch(value_size)
        {
            default:
            case 2: return Gate::Arity::CONST;  
            case 4: return Gate::Arity::UNARY;   
            case 6: return Gate::Arity::BINARY; 
        }
    }
    //gate name
    static std::string get_gate_or_register_name_from_id(const GateList& gates,size_t regs,size_t value)
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
    std::string ExecutionDebug::shell() const
    {
        std::stringstream output;            
        for(size_t s = 0; s != m_steps.size(); ++s)
        {
            #define get_name_input_from(out_id)\
                get_gate_or_register_name_from_id( m_layout.m_gates, m_layout.m_n_regs,  values[out_id](0,0) )
            //get step
            auto& step = m_steps[s];
            //print step
            output << "Timestep [" << s << "]:" << std::endl;
            //get op
            for(size_t p = 0; p != step.m_ops.size(); ++p)
            {
                //operation
                auto& operation = step.m_ops[p];
                //name, value
                auto& name     = std::get<0>(operation);
                auto& values   = std::get<1>(operation);
                auto  arity    = get_arity(values.size());
                //gate name
                std::string  gate_name= name; 
                gate_name[0] = std::toupper(gate_name[0]);
                //output by type
                switch(arity)
                {
                    case Gate::Arity::CONST:
                        output << u8"\t• "
                               << gate_name << " => "
                               << values.back()(0,0)
                               << std::endl;
                    break;
                    case Gate::Arity::UNARY:
                        output << u8"\t• "
                               << gate_name 
                               << "(" 
                               << get_name_input_from(0)
                               << " : "
                               << values[1](0,0)
                               << ") => "
                               << values.back()(0,0)
                               << std::endl;
                    break;
                    case Gate::Arity::BINARY:
                        output << u8"\t• "
                               << gate_name 
                               << "(" 
                               << get_name_input_from(0)
                               << " : "
                               << values[1](0,0)
                               << ", "
                               << get_name_input_from(2)
                               << " : "
                               << values[3](0,0)
                               << ") => ";
                        //print memory (write)
                        if(gate_name == "Write")
                        {
                            output 
                            << values.back()(0,0) 
                            << ", mem["<<values[1](0,0)<<"] : "
                            << values[4](0,values[1](0,0)) 
                            << std::endl;
                        }
                        else 
                        {
                            output << values.back()(0,0) << std::endl;
                        }
                    break;
                    //none
                    default: break;
                }
            }
            //get update
            for(size_t p = 0; p != step.m_ups.size(); ++p)
            {
                //operation
                auto& update =  step.m_ups[p];
                //name, value
                auto& r      = std::get<0>(update);
                auto& values = std::get<1>(update);
                //update
                output << u8"\t• R" << r << " <= " << get_name_input_from(0) << " : " <<  values.back()(0,0) << std::endl;
            }
            // Print mem at last timestep
            auto& last_gate_op = step.m_ops.back();
            auto& values       = std::get<1>(last_gate_op);
            output << u8"\t• Mem " << Dump::json_matrix(values[values.size() - 2]) << std::endl;
            //undef get_value_from
            #undef get_name_input_from
       
        }
        return output.str();
    }
	//json
    Json ExecutionDebug::json() const
	{
		//Array of steps
		JsonArray jsteps;
		//for each step
		for (size_t s = 0; s != m_steps.size(); ++s)
		{
			//get step
			auto& step = m_steps[s];
			//Json array of ops
			JsonArray jops;
			//get op
			for (size_t p = 0; p != step.m_ops.size(); ++p)
			{
				//Json op
				JsonObject jop;
				//operation
				auto& operation = step.m_ops[p];
				//name, value
				auto& name = std::get<0>(operation);
				auto& values = std::get<1>(operation);
				auto  arity = get_arity(values.size());
				//info json
				jop["type"]  = "gate";
				jop["name"]  = name;
				jop["arity"] = arity == Gate::Arity::CONST
							? "CONST" 
							: arity == Gate::Arity::UNARY
							? "UNARY"
							: "BINARY";
				//values
				JsonArray jvalues;
				for (auto& value : values)
				{
					//save matrix
					jvalues.push_back(json_array_from_matrix(value));
				}
				//save values
				jop["values"] = std::move(jvalues);
				//save jop
				jops.emplace_back(jop);
			}
			//get update
			for (size_t p = 0; p != step.m_ups.size(); ++p)
			{
				//Json op
				JsonObject jup;
				//operation
				auto& update = step.m_ups[p];
				//name, value
				auto& r = std::get<0>(update);
				auto& values = std::get<1>(update);
				//info update
				jup["type"] = "update";
				jup["register"] = int(r);
				//values
				JsonArray jvalues;
				for (auto& value : values)
				{
					//save matrix
					jvalues.push_back(json_array_from_matrix(value));
				}
				//save values
				jup["values"] = std::move(jvalues);
				//save jop
				jops.emplace_back(jup);
			}
			//save jops in steps
			jsteps.emplace_back(jops);
		}
		//return 
        return Json(jsteps);
    }
	////////////////////////////////////////////////////////////////////////////////////////
	// NRam
	Matrix fuzzy_encode(const Matrix& M)
    {
        Matrix sample = Matrix::Zero(M.cols(), M.cols());
        for (Matrix::Index n = 0; n < M.cols(); n++) 
        {
            sample(n, Matrix::Index(M(0,n))) = Scalar(1.0);
        }
        return sample;
    }

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

    Matrix defuzzy_mem_cols(const Matrix &M)
    {
        Matrix m(1, M.cols());
        for (Matrix::Index c = 0; c < M.cols(); c++)
        {
            Scalar        max_value = M(c, 0);
            Matrix::Index max_index = 0;
            for (Matrix::Index r = 1; r < M.rows(); ++r)
            {
                if (max_value < M(r, c))
                {
                    max_value = M(r, c);
                    max_index = r;
                }
            }
            m(0, c) = max_index;
        }
        return m;
    }

    Matrix get_registers_values(const Matrix &M, size_t extraction_type)
    {
        switch (extraction_type)
        {
            default:
            case NRamLayout::RegisterExtaction::P_ZERO:
            {
                return M.col(0).transpose();
            }
            case NRamLayout::RegisterExtaction::P_DEFUZZYED:
            {
                return defuzzy_mem(M);
            }
        }
    }

    Scalar calculate_sample_cost(const Matrix &M, const RowVector &desired_mem, const Matrix& linear_mask)
    {
        Scalar s_cost(0);
        for (size_t idx = 0; idx < M.rows(); ++idx)
            s_cost += Denn::CostFunction::safe_log(M(idx, Matrix::Index(desired_mem(idx)))) * linear_mask(0, idx);
        return s_cost;
    }

    static Matrix avg(const Matrix& regs, const Matrix& in)
    {
        return regs.transpose() * in;
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

        //Entropy
        const Scalar entropy(context.m_entropy_term);
        const Scalar entropy_decay(context.m_entropy_decay);

        // Run the circuit
        for (Matrix::Index s = 0; s < linear_test_in_mem.rows(); ++s)
        {
            // Alloc regs and initialize
            regs = Matrix::Zero(context.m_n_regs,max_int);
            regs.col(0).fill(1);

            // In mem fuzzy
			in_mem = fuzzy_encode(linear_test_in_mem.row(s));

            // Cost helper
            Scalar p_t = Scalar(0.0);
            Scalar prob_incomplete = Scalar(1.0);
            Scalar cum_prob_complete = Scalar(0.0);
            Scalar sample_cost = Scalar(0.0);
            // Execute sa sample for all timestep
            for (size_t timestep = 0; timestep < loop_timesteps; timestep++)
            {
				// NN
				circuit_configuration = network.apply(get_registers_values(regs, context.m_registers_values_extraction_type)).transpose();

                // Run circuit
                Scalar fi = run_circuit(context, circuit_configuration, regs, in_mem);

                // Calculate p_t
                if (timestep == loop_timesteps - 1)  p_t = 1 - cum_prob_complete;
                else                                 p_t = fi * prob_incomplete;

                // Calculate the probability of not complete 
                prob_incomplete *= (Scalar(1.0) - fi);
                cum_prob_complete += p_t;
				
                // Entropy
                const Scalar entropy_weight = entropy * std::pow(entropy_decay, timestep);
                const Matrix copy_mem = in_mem;
                const Scalar entropy_cost(copy_mem.unaryExpr([&] (Scalar v) -> Scalar {
                    return v * Denn::CostFunction::safe_log(v);
                }).sum() * entropy_weight);
                
				// Compute the "sample" cost                
                sample_cost -= (p_t * calculate_sample_cost(in_mem, linear_test_desired_mem.row(s), linear_mask)) - entropy_cost;
            }
            // Add to "batch" cost the "sample" cost
            full_cost += sample_cost;
        }

        // Cost regularization calculation
        Scalar cost_regularization(0.0);
        const Scalar regularization_term(context.m_cost_regularization_term);
        for (NeuralNetwork::LayerConstIterator layer = network.begin(); layer != network.end(); ++layer)
        {
            for (Layer::Iterator l_part = (*layer)->begin(); l_part != (*layer)->end(); ++l_part)
            {
                Matrix params_copy = (*l_part);
                cost_regularization += params_copy.unaryExpr([&](Scalar v) -> Scalar { return v * v; }).sum();
            }
        }
        cost_regularization *= regularization_term;

        return full_cost + cost_regularization;
    }

    Scalar run_circuit
    (   
      const NRamLayout &context
    , const Matrix& nn_out_decision
    , Matrix& regs
    , Matrix& in_mem
    )
    {
		Matrix C;
		ColVector a, b, c;
		Matrix coeff_a, coeff_b, coeff_c;
		//start col
        size_t ptr_col = 0, coefficient_size = context.m_n_regs;
        // Execute circuit
        size_t i = 0;
        for (; i != context.m_gates.size(); ++i, ++coefficient_size)
        {
            auto &gate = *context.m_gates[i];
            switch (gate.arity())
            {
                case Gate::CONST:
                {
                    C = gate(in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::UNARY:
                {
                    a = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    coeff_a = CostFunction::softmax_col(a) /* / CostFunction::softmax_col(a).sum() */;
                    ptr_col += coefficient_size;

                    C = gate(avg(regs, coeff_a), in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::BINARY:
                {
                    a = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    coeff_a = CostFunction::softmax_col(a) /* / CostFunction::softmax_col(a).sum() */;
                    ptr_col += coefficient_size;

                    b = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    coeff_b = CostFunction::softmax_col(b) /* / CostFunction::softmax_col(b).sum() */;
                    ptr_col += coefficient_size;

                    C = gate(avg(regs, coeff_a), avg(regs, coeff_b), in_mem);
                    //append output
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                default:
                break;
            }
        }

        // Update regs after circuit execution
        for (size_t r = 0; i < context.m_gates.size() + context.m_n_regs; ++i, ++r)
        {
			// get row
            c = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
			// softmax
            coeff_c = CostFunction::softmax_col(c) /* / CostFunction::softmax_col(c).sum() */;
			// update
			regs.row(r) = avg(regs, coeff_c).transpose();
			// next
			ptr_col += coefficient_size;
        }
		//resize regs
		regs.conservativeResize(context.m_n_regs, regs.cols());
        //return fi

        return PointFunction::sigmoid(nn_out_decision.col(nn_out_decision.cols() - 1)(0));
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    // Calculate error rate
    Scalar calculate_error_rate(
            const NRamLayout&       context 
        ,   const NeuralNetwork&    network
        ,   const Matrix&           linear_test_in_mem
        ,   const Matrix&           linear_test_desired_mem
        ,   const Matrix&           linear_mask
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
        const Scalar m(linear_test_in_mem.rows() * linear_mask.sum());

        // Run the circuit
        for (Matrix::Index s = 0; s < linear_test_in_mem.rows(); ++s)
        {
            // Alloc regs and initialize
            regs = Matrix::Zero(context.m_n_regs,max_int);
            regs.col(0).fill(1);

            // In mem fuzzy
			in_mem = fuzzy_encode(linear_test_in_mem.row(s));

            // Execute sa sample for all timestep
            for (size_t timestep = 0; timestep < timesteps; timestep++)
            {
				// NN
				circuit_configuration = network.apply(get_registers_values(regs, 
                    context.m_registers_values_extraction_type)).transpose();

                // Run circuit
                run_circuit(context, circuit_configuration, regs, in_mem);
            }

            // Calculate error rate for the sample
            in_mem = defuzzy_mem(in_mem);
            for (size_t col = 0; col < in_mem.cols(); ++col)
                if (linear_mask(0, col) 
                    && Matrix::Index(in_mem(0, col)) == Matrix::Index(linear_test_desired_mem(s, col))) 
                    c += 1;
        }

        return (1 - (c / m)); // Error rate
    }

	////////////////////////////////////////////////////////////////////////////////////////
	//Execute
    ResultAndExecutionDebug execute
    (
      const NRamLayout &context
    , const NeuralNetwork& network
    , const Matrix& linear_test_in_mem
    )
    {		
        // Ouput
        Matrix output;
        //list of execution debugger
        ListExecutionDebug samples_debug;
        // Run the circuit
        for (Matrix::Index s = 0; s < linear_test_in_mem.rows(); ++s)
        {
            //Alloc regs
            Matrix regs = Matrix::Zero(context.m_n_regs,context.m_max_int); 
            //P(X=0)
            regs.col(0).fill(1);
            //In mem fazzy
            Matrix in_mem = fuzzy_encode(linear_test_in_mem.row(s));
            //debugger
            ExecutionDebug execution_debug(context);
            //for all timestep
            for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
            {
                //new step
                execution_debug.push_step();
                //execute nn
                Matrix circuit_configuration = network.apply(get_registers_values(regs, context.m_registers_values_extraction_type)).transpose();
                //run
                run_circuit(context, circuit_configuration, regs, in_mem, execution_debug);
            }
            // Add to connections sample history
            samples_debug.push_back(execution_debug);
            //save ouput
            output.conservativeResize(output.rows()+1, in_mem.cols());
            output.row(output.rows()-1) = defuzzy_mem(in_mem).row(0);
        }
        return ResultAndExecutionDebug( output, samples_debug );
    }

    void run_circuit
    (
      const NRamLayout &context
    , const Matrix& nn_out_decision
    , Matrix& regs
    , Matrix& in_mem
    , ExecutionDebug& debug
    )
    {
        //start col
        size_t ptr_col = 0, coefficient_size = context.m_n_regs;
        // Execute circuit
        size_t i = 0;
        for (; i != context.m_gates.size(); ++i, ++coefficient_size)
        {
            auto &gate = *context.m_gates[i];
            switch (gate.arity())
            {
                case Gate::CONST:
                {
                    Matrix C = gate(in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                    // Add to history execution
                    debug.push_op(gate, defuzzy_mem(in_mem), defuzzy_mem(C));
                }
                break;
                case Gate::UNARY:
                {
                    ColVector a    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_a = CostFunction::softmax_col(a) /* / CostFunction::softmax_col(a).sum() */;
                    ptr_col += coefficient_size;

                    ColVector selected_value_a = avg(regs, coeff_a);
                    Matrix C = gate(selected_value_a, in_mem);
                    //append output
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                    // Add to history execution
                    debug.push_op(
                          gate
                        , defuzzy_mem_cols(coeff_a)
                        , defuzzy_mem_cols(selected_value_a)
                        , defuzzy_mem(in_mem)
                        , defuzzy_mem(C)
                    );
                }
                break;
                case Gate::BINARY:
                {
                    ColVector a    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_a = CostFunction::softmax_col(a)  /* / CostFunction::softmax_col(a).sum() */;
                    ptr_col += coefficient_size;

                    ColVector b    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_b = CostFunction::softmax_col(b)  /* / CostFunction::softmax_col(b).sum() */;
                    ptr_col += coefficient_size;

                    Matrix selected_value_a = avg(regs, coeff_a);
                    Matrix selected_value_b = avg(regs, coeff_b);
                    Matrix C = gate(selected_value_a, selected_value_b, in_mem);
                    //append output
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                    // Add to history execution
                    debug.push_op(
                          gate
                        , defuzzy_mem_cols(coeff_a)
                        , defuzzy_mem_cols(selected_value_a)
                        , defuzzy_mem_cols(coeff_b)
                        , defuzzy_mem_cols(selected_value_b)
                        , defuzzy_mem(in_mem)
                        , defuzzy_mem(C)
                    );
                }
                break;
                default:
                break;
            }
        }

        // Update regs after circuit execution
        for (size_t r = 0; i < context.m_gates.size() + context.m_n_regs; ++i, ++r)
        {
			// get row
            ColVector c = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
			// softmax
            Matrix coeff_c = CostFunction::softmax_col(c) /* / CostFunction::softmax_col(c).sum() */;
			// update
			regs.row(r) = avg(regs, coeff_c).transpose();
			// next
			ptr_col += coefficient_size;
            // Add to history execution
            debug.push_update(r, defuzzy_mem_cols(coeff_c), defuzzy_mem_cols(regs.row(r).transpose()));
        }
		//resize regs
		regs.conservativeResize(context.m_n_regs, regs.cols());
    }
	////////////////////////////////////////////////////////////////////////////////////////
}
}