#include "DennRuntimeOutput.h"
#include "DennAlgorithm.h"

namespace Denn
{
    class FullOutput : public Denn::RuntimeOutput
    {
    public:

		FullOutput(std::ostream& stream,const DennAlgorithm& algorithm):RuntimeOutput(stream, algorithm) {}
        
        virtual void start() override
        { 
            output() << "Denn start" << std::endl;
            m_start_time = Denn::Time::get_time();
            m_n_sub_pass = 0;
            m_n_pass     = 0;      
            //clean line
            clean_line();
            //output
            write_output(); 
            output() << std::endl;     
        }

        virtual void start_a_pass() override 
        { 
            //reset
            m_sub_pass_time  = Denn::Time::get_time(); 
            m_n_sub_pass     = 0;
        }

        virtual void end_a_pass() override 
        { 
            //clean line
            clean_line();
            //output
            write_output(); 
            output() << std::endl;
            //count
            ++m_n_pass;
        }

        virtual void end_a_sub_pass() override 
        { 
            ++m_n_sub_pass;
            //compute pass time
            double pass_per_sec = (double(m_n_sub_pass) / (Denn::Time::get_time() - m_sub_pass_time));
            //clean line
            clean_line();
            //write output
            output() << double(long(pass_per_sec*10.))/10.0 << " [it/s], ";
            write_output(); 
            output() << "\r";
        }

        virtual void end() override
        { 
            double time_of_execution = Denn::Time::get_time() - m_start_time ;
            Scalar test_result = m_algorithm.execute_test(*m_algorithm.best_context().m_best);
            output() 
            << "Denn end [ test: " 
            << test_result
            << ", time: " 
            << time_of_execution
            << " ]" 
            << std::endl;
        }


    protected:

        double m_start_time;
        double m_sub_pass_time;
        long   m_n_pass;
        long   m_n_sub_pass;

        virtual void write_output()
        {
            write_local_pass();
            output() << " -> on population: ";
            write_pass_best("[ id: ",", cross: ");
            output() << ", best: ";
            write_global_best("[ acc: ",", cross: ");
        }

        virtual void clean_line()
        {
            //clean line
            for(short i=0;i!=11;++i) output() << "          ";
            //end row
            output() << "\r";
        }

		virtual void write_local_pass() 
		{
            size_t n_sub_pass = *parameters().m_sub_gens;
			output() << (n_sub_pass * m_n_pass + m_n_sub_pass);
		}

        virtual void write_global_best
        (
			const std::string& open="[ ", 
			const std::string& separetor=", ", 
			const std::string& closer=" ]"
		) 
		{
			output() 
			<< open 
			<< m_algorithm.best_context().m_eval 
            << separetor 
            << m_algorithm.best_context().m_best->m_eval 
            << closer;
		}

		virtual void write_pass_best
        (
			const std::string& open="[ ", 
			const std::string& separetor=", ", 
			const std::string& closer=" ]"
		) 
		{
            ///////////////////////////////////////
            size_t id; 
            Scalar eval;
            m_algorithm.population().best(id,eval);
            ///////////////////////////////////////
			output()
            << open 
            << id 
            << separetor 
            << eval 
            << closer;
		}

    };
    REGISTERED_RUNTIME_OUTPUT(FullOutput,"full")

    class BenchOutput : public Denn::RuntimeOutput
    {
    public:

		BenchOutput(std::ostream& stream,const DennAlgorithm& algorithm):RuntimeOutput(stream, algorithm) {}
        
        virtual void start() override
        { 
            output() << "=== START ===" << std::endl;
            m_start_time = Denn::Time::get_time();
            m_n_pass = 0;
            m_n_restart = 0;
            //clean line
            clean_line();
            //output
            write_output(); 
            output() << std::endl;
        }

        virtual void start_a_pass() override 
        { 
            //reset
            m_sub_pass_time = Denn::Time::get_time(); 
            m_n_sub_pass    = 0;
        }

        virtual void end_a_pass() override 
        { 
            //count
            ++m_n_pass;
            //clean line
            clean_line();
            //output
            write_output(); 
            output() << std::endl;
        }

        virtual void restart()
        {
            ++m_n_restart;
        }

        virtual void start_a_sub_pass() override 
        { 
            ++m_n_sub_pass;
            //compute pass time
            double pass_per_sec = (double(m_n_sub_pass) / (Denn::Time::get_time() - m_sub_pass_time));
            //clean line
            clean_line();
            //write output
            write_output();
            output() << " ...\t" << double(long(pass_per_sec*10.))/10.0 << " [it/s] ";
            output() << "\r";
        }

        virtual void end() override
        { 
            double time_of_execution = Denn::Time::get_time() - m_start_time ;
            Scalar test_result = m_algorithm.execute_test(*m_algorithm.best_context().m_best);
            output() << "+ TEST\t" << test_result       << std::endl;
            output() << "+ TIME\t" << time_of_execution << std::endl;
            output() << "=== END ===" << std::endl;
        }

    protected:

        double m_start_time;
        double m_sub_pass_time;
        long   m_n_pass;
        long   m_n_sub_pass;
        long   m_n_restart;


        virtual void write_output()
        {
            //////////////////////////////////////////////////////////
            size_t n_sub_pass = *parameters().m_sub_gens;
            Scalar validation =  m_algorithm.best_context().m_eval;
            ///////////////////////////////////////////////////////////
            output() << "|-[" << (m_n_pass) * n_sub_pass; 
            output() << "]->[ACC_VAL:" << cut_digits(validation) << "]";
            if(*parameters().m_compute_test_per_pass)
            {
                Scalar test_result = m_algorithm.execute_test(*m_algorithm.best_context().m_best);
                output() << "[ACC_TEST:" << cut_digits(test_result) << "]";
            }
            output() << "[N_RESET:" << cut_digits(m_n_restart) << "]";
        }

        virtual void clean_line()
        {
            //clean line
            for(short i=0;i!=11;++i) output() << "          ";
            //end row
            output() << "\r";
        }

        static double cut_digits(double val)
        {
            return double(long(std::round(val * 10000))) / 10000;
        }

    };
    REGISTERED_RUNTIME_OUTPUT(BenchOutput,"bench")
}