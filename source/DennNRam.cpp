//
// Created by Valerio Belli on 20/11/17.
//

#include "Denn.h"
#include "DennNRam.h"
#include "DennNRamTask.h"

namespace Denn
{
namespace NRam
{

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

    std::string register_or_gate(const NRamLayout& context, ConnectionValue cv)
    {
        int idx = cv["connection"], value = cv["value"];

        // From 0 to R - 1 is a Register
        if (idx < context.m_n_regs)
        {
            return "R" + std::to_string((int)idx)
                   + "(" +std::to_string(value) + ")"; // idx + 1 for coerence with the paper
        }
        // From R to O - 1 is a Gate
        else if (idx >= context.m_n_regs && idx < context.m_n_regs + context.m_gates.size())
        {
            auto& gate = *context.m_gates[(int)idx - context.m_n_regs];
            std::string gate_name = gate.name();
            gate_name[0] = std::toupper(gate_name[0]);

            return gate_name + "(" +std::to_string(value) + ")";;
        }
        else
        {
            return "Unknown input!";
        }
    }

    void print_sample_execution(const NRamLayout& context,
                                const ListConnectionsHistory& connections,
                                std::ostream& output
    )
    {
        for(size_t t = 0; t < context.m_timesteps; ++t)
        {
            output << "Timestep [" << t << "]:" <<std::endl;
            ConnectionsHistory connections_history = connections[t];
            for(size_t g = 0; g < context.m_gates.size(); ++g)
            {
                auto& gate = *context.m_gates[g];

                // Print for constant gate
                if (gate.arity() == 0)
                {
                    std::string gate_name = gate.name();
                    gate_name[0] = std::toupper(gate_name[0]);
                    output << "\t• " << gate_name << " => "
                           << connections_history[gate.name()]["result"]
                           << std::endl;
                }

                // Print for the other gates and them values
                for(size_t a = 0; a < gate.arity(); ++a)
                {
                    std::string gate_name = gate.name();
                    gate_name[0] = std::toupper(gate_name[0]);
                    output << "\t• " << gate_name
                           << "." << a + 1 << " = "
                           << register_or_gate(context, connections_history[gate.name() + "(" + std::to_string(a) + ")"])
                           << " => "
                           << connections_history[gate.name() + "(" + std::to_string(a) + ")"]["result"]
                           << std::endl;
                }
            }

            // Print the updated register and from what
            for(size_t r = 1; r < context.m_n_regs + 1; ++r)
                output << "\t• R" << r << "' = "
                       << register_or_gate(context, connections_history["reg(" + std::to_string(r - 1) + ")"])
                       << " => " << connections_history["reg(" + std::to_string(r - 1) + ")"]["value"]
                       << std::endl;
        }
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

    Scalar run_circuit(const NRamLayout &context
            , const Matrix& nn_out_decision
            , Matrix& regs
            , Matrix& in_mem)
    {
        ConnectionsHistory timestep_connection;
        return run_circuit(context, nn_out_decision, regs, in_mem, timestep_connection);
    }

    Scalar run_circuit(const NRamLayout &context, const Matrix& nn_out_decision, Matrix& regs, Matrix& in_mem,
                       ConnectionsHistory& timestep_connection)
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
                    ConnectionValue cv;
                    cv["connection"] = -1;
                    cv["value"]      = -1;
                    cv["result"]     = static_cast<int>(defuzzy_mem(C)(0, 0));
                    timestep_connection[gate.name()] = cv;
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
                    ConnectionValue cv;
                    cv["connection"] = static_cast<int>(defuzzy_mem_cols(coeff_a)(0, 0));
                    cv["value"]      = static_cast<int>(defuzzy_mem_cols(selected_value_a)(0, 0));
                    cv["result"]     = static_cast<int>(defuzzy_mem(C)(0, 0));
                    timestep_connection[gate.name() + "(0)"] = cv;
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
                    ConnectionValue cv_a;
                    cv_a["connection"] = static_cast<int>(defuzzy_mem_cols(coeff_a)(0, 0));
                    cv_a["value"]      = static_cast<int>(defuzzy_mem_cols(selected_value_a)(0, 0));
                    cv_a["result"]     = static_cast<int>(defuzzy_mem(C)(0, 0));
                    timestep_connection[gate.name() + "(0)"] = cv_a;


                    ConnectionValue cv_b;
                    cv_b["connection"] = static_cast<int>(defuzzy_mem_cols(coeff_b)(0, 0));
                    cv_b["value"]      = static_cast<int>(defuzzy_mem_cols(selected_value_b)(0, 0));
                    cv_b["result"]     = static_cast<int>(defuzzy_mem(C)(0, 0));
                    timestep_connection[gate.name() + "(1)"] = cv_b;
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


            ConnectionValue cv;
            cv["connection"] = static_cast<int>(defuzzy_mem_cols(coeff_c)(0, 0));
            cv["value"]      = static_cast<int>(defuzzy_mem_cols(regs.row(r).transpose())(0, 0));
            timestep_connection["reg(" + std::to_string(r) + ")"] = cv;
        }
		//resize regs
		regs.conservativeResize(context.m_n_regs, regs.cols());
        //return fi
        return PointFunction::sigmoid(nn_out_decision.col(nn_out_decision.cols() - 1)(0));
    }

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

    ResultAndConnectionsHistory execute
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
        // Run the circuit
        ListListConnectionsHistory samples_timesteps_connection(1);
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
            //for all timestep, run on s

            ListConnectionsHistory sample_timesteps_connection(context.m_timesteps);
            for (size_t timestep = 0; timestep < context.m_timesteps; timestep++)
            {
                ConnectionsHistory timestep_connection;
                //execute nn
                Matrix out = network.apply(regs.col(0).transpose()).transpose();
                //execute circuit
                Scalar fi = run_circuit(context, out, regs, in_mem, timestep_connection);
                sample_timesteps_connection[timestep] = timestep_connection;
            }

            // Add to connections sample history
            samples_timesteps_connection[s] = sample_timesteps_connection;
            //save ouput
            output.conservativeResize(output.rows()+1, in_mem.cols());
            output.row(output.rows()-1) = defuzzy_mem(in_mem).row(0);
        }

        return std::tuple< Matrix, ListListConnectionsHistory >( output, samples_timesteps_connection );
    }
}
}