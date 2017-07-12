#include "DennRuntimeOutput.h"
#include "DennParameters.h"

namespace Denn
{
    class FullOutput : public Denn::RuntimeOutput
    {
    public:

		FullOutput(std::ostream& stream,const Parameters& params):RuntimeOutput(stream,params) {}
        
        virtual bool is_enable() override             { return true;  }
		virtual bool is_enable_compute_test() override { return false; }

        virtual void start() override
        { 
            output() << "Denn start" << std::endl;
            m_start_time = Denn::Time::get_time();
            m_pass_time  = Denn::Time::get_time(); 
            m_n_pass = 0;
        }

        virtual void update_best() override 
        { 
            //reset
            m_pass_time  = Denn::Time::get_time(); 
            m_n_pass 	 = 0;
            //clean line
            clean_line();
            //output
            write_output(); 
            output() << std::endl;
        }

        virtual void update_pass() override 
        { 
            ++m_n_pass;
            //compute pass time
            double pass_per_sec = (double(m_n_pass) / (Denn::Time::get_time() - m_pass_time));
            //clean line
            clean_line();
            //write output
            output() << double(long(pass_per_sec*10.))/10.0 << " [it/s], ";
            write_output(); 
            output() << "\r";
        }

        virtual void end() override
        { 
            output() 
            << "Denn end [ test: " 
            << m_end_of_iterations.m_test_result 
            << ", time: " 
            << Denn::Time::get_time() - m_start_time 
            << " ]" 
            << std::endl;
        }

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
            for(short i=0;i!=11;++i) 
                output() << "          ";
            //end row
            output() << "\r";
        }

    protected:

        double m_start_time;
        double m_pass_time;
        long   m_n_pass;

    };
    REGISTERED_RUNTIME_OUTPUT(FullOutput,"full")

    class BenchOutput : public Denn::RuntimeOutput
    {
    public:

		BenchOutput(std::ostream& stream,const Parameters& params):RuntimeOutput(stream,params) {}
        
        bool is_enable() override              { return true;  }
        bool is_enable_pass() override         { return false; }
        bool is_enable_compute_test() override { return *parameters().m_compute_test_per_pass;  }


        virtual void start() override
        { 
            output() << "=== START ===" << std::endl;
            m_start_time = Denn::Time::get_time();
            m_pass_time  = Denn::Time::get_time(); 
            m_n_pass = 0;
        }

        virtual void update_best() override 
        { 
            //reset
            m_pass_time  = Denn::Time::get_time(); 
            m_n_pass 	 = 0;
            //clean line
            // clean_line();
            //output
            write_output(); 
            output() << std::endl;
        }

        virtual void update_pass() override 
        { 
            ++m_n_pass;
            //compute pass time
            double pass_per_sec = (double(m_n_pass) / (Denn::Time::get_time() - m_pass_time));
            //clean line
            clean_line();
            //write output
            write_output();
            output() << " ...\t" << double(long(pass_per_sec*10.))/10.0 << " [it/s] ";
            output() << "\r";
        }

        virtual void end() override
        { 
            output() << "+ TEST\t" << m_end_of_iterations.m_test_result << std::endl;
            output() << "+ TIME\t" << Denn::Time::get_time() - m_start_time << std::endl;
            output() << "=== END ===" << std::endl;
        }

        virtual void write_output()
        {
            output() << "|-[" << (m_global_pass.m_g_pass+1) * m_initialization.m_n_s_pass; 
            output() << "]->[ACC_VAL:" << cut_digits(m_global_pass.m_validation_eval) << "]";
            if(*parameters().m_compute_test_per_pass)
                output() << "[ACC_TEST:" << cut_digits(m_global_pass.m_test_eval) << "]";
            output() << "[N_RESET:" << cut_digits(m_global_pass.m_n_restart) << "]";
        }

        virtual void clean_line()
        {
            //clean line
            for(short i=0;i!=11;++i) 
                output() << "          ";
            //end row
            output() << "\r";
        }

        static double cut_digits(double val)
        {
            return double(long(val * 1000)) / 1000;
        }

    protected:

        double m_start_time;
        double m_pass_time;
        long   m_n_pass;

    };
    REGISTERED_RUNTIME_OUTPUT(BenchOutput,"bench")
}