//
// Created by Valerio Belli on 20/11/17.
//

#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
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
        const GateList& gates
    )
    {
        // values init
	    m_batch_size = batch_size;
        m_max_int = max_int;
        m_n_regs = n_regs;
        m_timesteps = timesteps;
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
	ExecutionDebug::ExecutionDebug(){}
    ExecutionDebug::ExecutionDebug(const ExecutionDebug& debug){ m_steps = debug.m_steps; }
    void ExecutionDebug::push_step()
    {
        m_steps.resize(m_steps.size()+1);
    }
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& m, const Matrix& out)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ m, out }));
    }
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& m, const Matrix& out)
    {
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, m, out }));
    }
    void ExecutionDebug::push_op(const Gate& gate, const Matrix& a, const Matrix& in_a, const Matrix& b, const Matrix& in_b, const Matrix& m, const Matrix& out)
    {           
        m_steps[m_steps.size()-1].push_op(OpNode( gate.name() , Values{ a, in_a, b, in_b, m, out }));
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
	//shell
    std::string ExecutionDebug::shell() const
    {
        std::stringstream output;            
        for(size_t s = 0; s != m_steps.size(); ++s)
        {
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

                switch(arity)
                {
                    case Gate::Arity::CONST:
                        output << "\t• " 
                               << gate_name << " => "
                               << values.back()(0,0)
                               << std::endl;
                    break;
                    case Gate::Arity::UNARY:
                        output << "\t• " 
                               << gate_name 
                               << "(" 
                               << values[0](0,0)
                               << ") => "
                               << values.back()(0,0)
                               << std::endl;
                    break;
                    case Gate::Arity::BINARY:
                        output << "\t• " 
                               << gate_name 
                               << "(" 
                               << values[0](0,0)
                               << ","
                               << values[2](0,0)
                               << ") => "
                               << values.back()(0,0)
                               << std::endl;
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
                output << "\t• R" << r << " => " << values.back()(0,0) << std::endl;
            }

            // Print mem at last timestep
            auto& last_gate_op = step.m_ops.back();
            auto& values       = std::get<1>(last_gate_op);
            output << "\t• Mem " << Dump::json_matrix(values[values.size() - 2]) << std::endl;
       
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
					//json array of matrix
					JsonArray jmatrix;
					//matrix to json array
					for (Matrix::Index r = 0; r != value.rows(); ++r)
					{
						//row of matrix
						JsonArray jrow_matrix;
						//cols
						for (Matrix::Index c = 0; c != value.cols(); ++c)
						{
							jrow_matrix.push_back(value(r, c));
						}
						//save row
						jmatrix.push_back(jrow_matrix);
					}
					//save matrix
					jvalues.push_back(jmatrix);
				}
				//save values
				jop["values"] = jvalues;
				//save jop
				jops.push_back(jop);
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
					//json array of matrix
					JsonArray jmatrix;
					//matrix to json array
					for (Matrix::Index r = 0; r != value.rows(); ++r)
					{
						//row of matrix
						JsonArray jrow_matrix;
						//cols
						for (Matrix::Index c = 0; c != value.cols(); ++c)
						{
							jrow_matrix.push_back(value(r, c));
						}
						//save row
						jmatrix.push_back(jrow_matrix);
					}
					//save matrix
					jvalues.push_back(jmatrix);
				}
				//save values
				jup["values"] = jvalues;
				//save jop
				jops.push_back(jup);
			}
			//save jops in steps
			jsteps.push_back(jops);
		}
		//return 
        return Json(jsteps);
    }
	////////////////////////////////////////////////////////////////////////////////////////
	//Evaluetor of nram
	REGISTERED_EVALUATION(NRamEval, "nram")
	//methods
	bool NRamEval::minimize() const { return true; }
	Scalar NRamEval::operator () (const Individual& individual, const DataSet& ds)
	{
		assert(m_context);
		//network
		auto& nn = individual.m_network;
		//Dataset
		auto& in_mem = ds.features();
		auto& out_mem = ds.labels();
		//execute
		return NRam::train(*m_context, nn, in_mem, out_mem);
	}
	//set context
	Evaluation::SPtr NRamEval::set_context(const NRamLayout& context)
	{
		m_context = &context;
		return this->get_ptr();
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

    Scalar calculate_sample_cost(Matrix &M, const RowVector &desired_mem)
    {
        Scalar s_cost = 0;
        for (size_t idx = 0; idx < M.rows(); ++idx)
        {
            s_cost += Denn::CostFunction::safe_log(M(idx, Matrix::Index(desired_mem(idx))));
        }
        return s_cost;
    }

    static Matrix avg(const Matrix& regs, const Matrix& in)
    {
        return regs.transpose() * in;
    }
	////////////////////////////////////////////////////////////////////////////////////////
	//Train
    Scalar train
    (
      const NRamLayout &context
    , const NeuralNetwork& network
    , const Matrix& linear_in_mem
    , const Matrix& linear_out_mem
    )
    {		
        //time step
        Scalar full_cost = Scalar(0.0);
        // Run the circuit
        for (Matrix::Index s = 0; s < linear_in_mem.rows(); ++s)
        {
            //Alloc regs
            Matrix regs = Matrix::Zero(context.m_n_regs,context.m_max_int); 
            //P(X=0)
            regs.col(0).fill(1);
            //In mem fazzy
            Matrix in_mem = fuzzy_encode(linear_in_mem.row(s));
            //Cost helper
            Scalar p_t = Scalar(1.0);
            Scalar prob_incomplete = Scalar(1.0);
            Scalar cum_prob_complete = Scalar(0.0);
            Scalar sample_cost = Scalar(0.0);

            //for all timestep, run on s
            for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
            {
                //execute nn
                Matrix out = network.apply(regs.col(0).transpose()).transpose();
                //execute circuit
                Scalar fi = run_circuit(context, out, regs, in_mem);

                //compute exit state
                if (timestep == context.m_timesteps - 1)
                    p_t = 1 - cum_prob_complete;
                else
                    p_t = fi * prob_incomplete;
                //
                cum_prob_complete += p_t;
                prob_incomplete *= 1 - fi;
                //compute cost
                sample_cost -= p_t * calculate_sample_cost(in_mem, linear_out_mem.row(s));

            }
            //add to full "cost"
            full_cost += sample_cost;
        }

        return full_cost;
    }

    Scalar run_circuit
    (   
      const NRamLayout &context
    , const Matrix& nn_out_decision
    , Matrix& regs
    , Matrix& in_mem
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
                }
                break;
                case Gate::UNARY:
                {
                    ColVector a    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_a = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    Matrix C = gate(avg(regs, coeff_a), in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                }
                break;
                case Gate::BINARY:
                {
                    ColVector a    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_a = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    ColVector b    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_b = CostFunction::softmax_col(b);
                    ptr_col += coefficient_size;

                    Matrix C = gate(avg(regs, coeff_a), avg(regs, coeff_b), in_mem);
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
            ColVector c = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
			// softmax
            Matrix coeff_c = CostFunction::softmax_col(c);
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
	//Execute
    ResultAndExecutionDebug execute
    (
      const NRamLayout &context
    , const NeuralNetwork& network
    , const Matrix& linear_in_mem
    )
    {		
        //time step
        Scalar full_cost = Scalar(0.0);
        // Ouput
        Matrix output;
        //list of execution debugger
        ListExecutionDebug samples_debug;
        // Run the circuit
        for (Matrix::Index s = 0; s < linear_in_mem.rows(); ++s)
        {
            //Alloc regs
            Matrix regs = Matrix::Zero(context.m_n_regs,context.m_max_int); 
            //P(X=0)
            regs.col(0).fill(1);
            //In mem fazzy
            Matrix in_mem = fuzzy_encode(linear_in_mem.row(s));
            //Cost helper
            Scalar p_t = Scalar(1.0);
            //debugger
            ExecutionDebug execution_debug;
            //for all timestep, run on s
            for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
            {
                //new step
                execution_debug.push_step();
                //execute nn
                Matrix out = network.apply(regs.col(0).transpose()).transpose();
                //execute circuit
                Scalar fi = run_circuit(context, out, regs, in_mem, execution_debug);
            }
            // Add to connections sample history
            samples_debug.push_back(execution_debug);
            //save ouput
            output.conservativeResize(output.rows()+1, in_mem.cols());
            output.row(output.rows()-1) = defuzzy_mem(in_mem).row(0);
        }
        return ResultAndExecutionDebug( output, samples_debug );
    }

    Scalar run_circuit
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
                    Matrix coeff_a = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    ColVector selected_value_a = avg(regs, coeff_a);
                    Matrix C = gate(selected_value_a, in_mem);
                    regs.conservativeResize(regs.rows() + 1, regs.cols());
                    regs.row(regs.rows() - 1) = C;
                    // Add to history execution
                    debug.push_op(
                          gate
                        , defuzzy_mem(coeff_a)
                        , defuzzy_mem(selected_value_a)
                        , defuzzy_mem(in_mem)
                        , defuzzy_mem(C)
                    );
                }
                break;
                case Gate::BINARY:
                {
                    ColVector a    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_a = CostFunction::softmax_col(a);
                    ptr_col += coefficient_size;

                    ColVector b    = nn_out_decision.block(ptr_col, 0, coefficient_size, 1);
                    Matrix coeff_b = CostFunction::softmax_col(b);
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
                        , defuzzy_mem(coeff_a)
                        , defuzzy_mem(selected_value_a)
                        , defuzzy_mem(coeff_b)
                        , defuzzy_mem(selected_value_b)
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
            Matrix coeff_c = CostFunction::softmax_col(c);
			// update
			regs.row(r) = avg(regs, coeff_c).transpose();
			// next
			ptr_col += coefficient_size;
            // Add to history execution
            debug.push_update(r, defuzzy_mem_cols(coeff_c), defuzzy_mem_cols(regs.row(r).transpose()));
        }
		//resize regs
		regs.conservativeResize(context.m_n_regs, regs.cols());
        //return fi
        return PointFunction::sigmoid(nn_out_decision.col(nn_out_decision.cols() - 1)(0));
    }
	////////////////////////////////////////////////////////////////////////////////////////
}
}