#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
namespace Denn
{
	class NoneMethod : public EvolutionMethod
	{
	public:

		NoneMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& population
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//none
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			//none
		}

	private:

	};
	REGISTERED_EVOLUTION_METHOD(NoneMethod,"NONE")


	class PHistoryMethod : public EvolutionMethod
	{
	public:
		//PHistoryMethod
		PHistoryMethod(const DennAlgorithm& algorithm): EvolutionMethod(algorithm)
		{
			//get size
			m_max_size   = parameters().m_history_size;
			//sub method
			m_sub_method = EvolutionMethodFactory::create(parameters().m_sub_evolution_type, m_algorithm);
		}
		//..		
		virtual void start() override
		{ 
			//init 
			m_k = 0;
			m_history.clear();
			//start
			m_sub_method->start(); 
		};
		virtual void start_a_gen_pass(DoubleBufferPopulation& g_population) override
		{ 
			///////////////////////////////////////////////////////////////////////////////
			// compere history with new population
			for(Population& population_tmp : m_history)
			{
				m_algorithm.execute_fitness_on(population_tmp);
				for(size_t i=0; i!=current_np(); ++i)
				{
					if( population_tmp[i]->m_eval < g_population.parents()[i]->m_eval )
					{
						g_population.parents()[i] = population_tmp[i]->copy();
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			//copy
			if(m_history.size() < m_max_size)
			{
				m_history.push_back(g_population.parents().copy());
			}  
			else
			{
				m_history[m_k] = g_population.parents().copy(); 
				m_k = (m_k + 1) % m_max_size;
			} 
			///////////////////////////////////////////////////////////////////////////////
			//start the pass
			m_sub_method->start_a_gen_pass(g_population);
		};
		virtual void start_a_subgen_pass(DoubleBufferPopulation& population) override { m_sub_method->start_a_subgen_pass(population); };
		virtual void create_a_individual(DoubleBufferPopulation& population, size_t target, Individual& i_output) override { m_sub_method->create_a_individual(population, target, i_output); };
		virtual	void selection(DoubleBufferPopulation& population) override           { m_sub_method->selection(population); };
		virtual void end_a_subgen_pass(DoubleBufferPopulation& population) override   { m_sub_method->end_a_subgen_pass(population); };
		virtual void end_a_gen_pass(DoubleBufferPopulation& population) override      { m_sub_method->end_a_gen_pass(population); };
		virtual const VariantRef get_context_data() const override                    { return m_sub_method->get_context_data(); }

	protected:
	
		size_t					  m_k;
		size_t					  m_max_size;
		std::vector< Population > m_history;
		EvolutionMethod::SPtr     m_sub_method;

	};
	REGISTERED_EVOLUTION_METHOD(PHistoryMethod,"PHISTORY")


	class P2HistoryMethod : public PHistoryMethod
	{
	public:

		//PHistoryMethod
		P2HistoryMethod(const DennAlgorithm& algorithm): PHistoryMethod(algorithm) {}

		virtual void start_a_gen_pass(DoubleBufferPopulation& g_population) override
		{ 
			///////////////////////////////////////////////////////////////////////////////
			// compere history with new population
			for(Population& population_tmp : m_history)
			{
				m_algorithm.execute_fitness_on(population_tmp);
			}				
			//best of the best
			for(const Population& population_tmp : m_history)
			{
				for(const Individual::SPtr& individual_tmp : population_tmp)
				for(size_t u=0; u!=current_np(); ++u)				
				{
					if( individual_tmp->m_eval < g_population.parents()[u]->m_eval )
					{
						g_population.parents()[u] = individual_tmp->copy();
						break;
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			//copy
			if(m_history.size() < m_max_size)
			{
				m_history.push_back(g_population.parents().copy());
			}  
			else
			{
				m_history[m_k] = g_population.parents().copy(); 
				m_k = (m_k + 1) % m_max_size;
			} 
			///////////////////////////////////////////////////////////////////////////////
			//start the pass
			m_sub_method->start_a_gen_pass(g_population);
		};

	};
	REGISTERED_EVOLUTION_METHOD(P2HistoryMethod,"P2HISTORY")

	class DEMethod : public EvolutionMethod
	{
	public:

		DEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void start() override
		{
			//create mutation/crossover
			m_mutation = MutationFactory::create(m_algorithm.parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& population
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			const Population& parents= population.parents();
			const Individual& target = *parents[i_target];
			//copy
			i_output.m_f  = target.m_f;
			i_output.m_cr = target.m_cr;
			//call muation
			(*m_mutation) (parents, i_target, i_output);
			//call crossover
			(*m_crossover)(parents, i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			population.the_best_sons_become_parents();
		}

	private:

		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(DEMethod,"DE")

	class JDEMethod : public EvolutionMethod
	{
	public:

		JDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}
	
		virtual void start() override
		{
			//create mutation/crossover
			m_mutation = MutationFactory::create(parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}

		virtual void create_a_individual
		(     
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		) 
		override
		{
			//vectors
			const Population& parents    = dpopulation.parents();
			const Individual& target     = *parents[i_target];
			//f JDE
			if (random(i_target).uniform() < Scalar(parameters().m_jde_f))
				i_output.m_f = Scalar(random(i_target).uniform(0.0, 2.0));
			else
				i_output.m_f = target.m_f;
			//cr JDE
			if (random(i_target).uniform() < Scalar(parameters().m_jde_cr))
				i_output.m_cr = Scalar(random(i_target).uniform());
			else
				i_output.m_cr = target.m_cr;
			//call muation
			(*m_mutation) (parents, i_target, i_output);
			//call crossover
			(*m_crossover)(parents, i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			population.the_best_sons_become_parents();
		}

	protected:

		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(JDEMethod, "JDE")


	class JADEMethod : public EvolutionMethod
	{
	public:

		JADEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) 
		{
			m_archive_max_size = parameters().m_archive_size;
			m_c_adapt          = parameters().m_f_cr_adapt;
			m_mu_f       = Scalar(0.5);
			m_mu_cr      = Scalar(0.5);
		}
		
		virtual void start() override
		{
			//reinit
			m_mu_f = Scalar(0.5);
			m_mu_cr = Scalar(0.5);
			//clear
			m_archive.clear();
			//create mutation/crossover
			m_mutation = MutationFactory::create(parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			if(m_mutation->required_sort()) dpopulation.parents().sort();
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//Compute F
			//JADE REF:  Cauchy  distribution  with  location  parameter μF and scale parameter 0.1
			//           Fi=randci(μF,0.1) and  then  truncated  to  be  1  if Fi≥1  or  regenerated  if Fi ≤ 0
			Scalar v;
			do v = random(i_target).cauchy(m_mu_f, 0.1); while (v <= 0);
			i_output.m_f = Denn::sature(v);
			//Cr
			i_output.m_cr = Denn::sature(random(i_target).normal(m_mu_cr, 0.1));
			//call muation
			(*m_mutation) (dpopulation.parents(), i_target, i_output);
			//call crossover
			(*m_crossover)(dpopulation.parents(), i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			Scalar sum_f = 0;
			Scalar sum_f2 = 0;
			Scalar sum_cr = 0;
			size_t n_discarded = 0;
			size_t np = current_np();

			for (size_t i = 0; i != np; ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (son->m_eval < father->m_eval)
				{
					if (m_archive_max_size) m_archive.push_back(father->copy());
					//else if (main_random().uniform() < Scalar(m_archive_max_size) / Scalar(m_archive_max_size + n_discarded))
					sum_f += son->m_f;
					sum_f2 += son->m_f * son->m_f;
					sum_cr += son->m_cr;
					++n_discarded;
					dpopulation.swap(i);
				}
			}
			//safe compute muF and muCR 
			if (n_discarded)
			{
				m_mu_cr = Denn::lerp(m_mu_cr, sum_cr / n_discarded, m_c_adapt);
				m_mu_f = Denn::lerp(m_mu_f, sum_f2 / sum_f, m_c_adapt);
			}
			//reduce A
			while (m_archive_max_size < m_archive.size())
			{
				m_archive[main_random().index_rand(m_archive.size())] = m_archive.last();
				m_archive.pop_back();
			}
		}

		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:		

		size_t          m_archive_max_size{ false };
		Scalar          m_c_adapt         { Scalar(1.0) };
		Scalar          m_mu_f      { Scalar(0.5) };
		Scalar          m_mu_cr     { Scalar(0.5) };
		Population	    m_archive;
		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(JADEMethod, "JADE")

	class SHADEMethod : public EvolutionMethod
	{
	public:

		SHADEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			m_archive_max_size = parameters().m_archive_size;
			m_h = parameters().m_shade_h;
		}

		virtual void start() override
		{
			//H size
			m_mu_f = std::vector<Scalar>(m_h, Scalar(0.5));
			m_mu_cr = std::vector<Scalar>(m_h, Scalar(0.5));
			m_k = 0;
			m_pmin = Scalar(2) / Scalar(current_np());
			//clear
			m_archive.clear();
			//create mutation/crossover
			m_mutation = MutationFactory::create(parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			if (m_mutation->required_sort()) dpopulation.parents().sort();
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//take tou
			size_t tou_i = random(i_target).index_rand(m_mu_f.size());
			//Compute F
			Scalar v;
			do v = random(i_target).cauchy(m_mu_f[tou_i], 0.1); while (v <= 0);
			i_output.m_f = Denn::sature(v);
			//Cr
			i_output.m_cr = Denn::sature(random(i_target).normal(m_mu_cr[tou_i], 0.1));
			//P
			i_output.m_p = random(i_target).uniform(m_pmin, 0.2);
			//call muation
			(*m_mutation) (dpopulation.parents(), i_target, i_output);
			//call crossover
			(*m_crossover)(dpopulation.parents(), i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			//F
			Scalar sum_f = 0;
			Scalar sum_f2 = 0;
			//CR
			std::vector<Scalar> s_cr;
			std::vector<Scalar> s_delta_f;
			Scalar delta_f = 0;
			Scalar sum_delta_f = 0;
			size_t n_discarded = 0;
			//pop
			size_t np = current_np();

			for (size_t i = 0; i != np; ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (son->m_eval < father->m_eval)
				{
					if (m_archive_max_size) m_archive.push_back(father->copy());
					//else if (main_random().uniform() < Scalar(m_archive_max_size) / Scalar(m_archive_max_size + n_discarded))
					//F
					sum_f += son->m_f;
					sum_f2 += son->m_f * son->m_f;
					//w_k (for mean of Scr)
					delta_f = std::abs(son->m_eval - father->m_eval);
					s_delta_f.push_back(delta_f);
					sum_delta_f += delta_f;
					//Scr
					s_cr.push_back(son->m_cr);
					++n_discarded;
					//SWAP
					dpopulation.swap(i);
				}
			}
			//safe compute muF and muCR 
			if (n_discarded)
			{
				Scalar mean_w_scr = 0;
				for (size_t k = 0; k != n_discarded; ++k)
				{
					//mean_ca(Scr) = sum_0-k( Scr_k   * w_k )
					mean_w_scr += s_cr[k] * (s_delta_f[k] / sum_delta_f);
				}
				m_mu_cr[m_k] = mean_w_scr;
				m_mu_f[m_k] = sum_f2 / sum_f;
				m_k = (m_k + 1) % m_mu_f.size();
			}
			//reduce A
			while (m_archive_max_size < m_archive.size())
			{
				m_archive[main_random().index_rand(m_archive.size())] = m_archive.last();
				m_archive.pop_back();
			}
		}
		
		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:

		size_t				m_h{ 0 };
		size_t				m_k{ 0 };
		size_t              m_archive_max_size{ false };
		Scalar				m_pmin   { Scalar(0.0) };
		std::vector<Scalar> m_mu_f;
		std::vector<Scalar> m_mu_cr;
		Population	        m_archive;
		Mutation::SPtr      m_mutation;
		Crossover::SPtr     m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(SHADEMethod, "SHADE")
	
	class L_SHADEMethod : public EvolutionMethod
	{
	public:

		L_SHADEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			m_archive_max_size = parameters().m_archive_size;
			m_h = parameters().m_shade_h;
		}

		virtual void start() override
		{
			//H size
			m_mu_f = std::vector<Scalar>(m_h, Scalar(0.5));
			m_mu_cr = std::vector<Scalar>(m_h, Scalar(0.5));
			m_k = 0;
			m_pmin = Scalar(2) / Scalar(current_np());
			//clear
			m_archive.clear();
			//NFE to 0
			m_curr_nfe = 0;
			//create mutation/crossover
			m_mutation = MutationFactory::create(parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			if (m_mutation->required_sort()) dpopulation.parents().sort();
		}

		virtual void end_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//compute NFE
			m_curr_nfe += current_np();
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//take tou
			size_t tou_i = random(i_target).index_rand(m_mu_f.size());
			//Compute F
			Scalar v;
			do v = random(i_target).cauchy(m_mu_f[tou_i], 0.1); while (v <= 0);
			i_output.m_f = Denn::sature(v);
			//Cr
			i_output.m_cr = 
			  m_mu_cr[tou_i] == *parameters().m_mu_cr_terminal_value
			? Scalar(0.0) 
			: Denn::sature(random(i_target).normal(m_mu_cr[tou_i], 0.1));
			//P
			i_output.m_p = random(i_target).uniform(m_pmin, 0.2);
			//call muation
			(*m_mutation) (dpopulation.parents(), i_target, i_output);
			//call crossover
			(*m_crossover)(dpopulation.parents(), i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			//F
			Scalar sum_f = 0;
			Scalar sum_f2 = 0;
			//CR
			std::vector<Scalar> s_cr;
			std::vector<Scalar> s_delta_f;
			Scalar delta_f = 0;
			Scalar sum_delta_f = 0;
			size_t n_discarded = 0;
			//pop
			size_t np = current_np();
			for (size_t i = 0; i != np; ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (son->m_eval < father->m_eval)
				{
					if (m_archive_max_size) m_archive.push_back(father->copy());
					//F
					sum_f += son->m_f;
					sum_f2 += son->m_f * son->m_f;
					//w_k (for mean of Scr)
					delta_f = std::abs(son->m_eval - father->m_eval);
					s_delta_f.push_back(delta_f);
					sum_delta_f += delta_f;
					//Scr
					s_cr.push_back(son->m_cr);
					++n_discarded;
					//SWAP
					dpopulation.swap(i);
				}
			}
			//safe compute muF and muCR 
			if (n_discarded)
			{
				//compute mu
				Scalar mu_cr = *parameters().m_mu_cr_terminal_value;
				//isn't terminale?
				if ( m_mu_cr[m_k] != (*parameters().m_mu_cr_terminal_value)  &&
					(*std::max_element(s_cr.begin(), s_cr.end())) != Scalar(0.0))
				{
					//compute mu cr from formula
					Scalar w_k = 0;
					Scalar mean_w_scr = 0;
					Scalar mean_w_scr_pow2 = 0;
					//compute Mcr
					for (size_t k = 0; k != n_discarded; ++k)
					{
						//mean_ca(Scr) = sum_0-k( Scr_k   * w_k )
						w_k = (s_delta_f[k] / sum_delta_f);
						mean_w_scr_pow2 += s_cr[k] * s_cr[k] * w_k;
						mean_w_scr += s_cr[k] * w_k;
					}
					mu_cr = mean_w_scr_pow2 / mean_w_scr;
				}
				//
				m_mu_cr[m_k] = mu_cr;
				m_mu_f[m_k] = sum_f2 / sum_f;
				m_k = (m_k + 1) % m_mu_f.size();
			}
			//reduce A
			reduce_archive();
			//reduce population
			reduce_population(dpopulation);
		}

		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:

		size_t				m_h{ 0 };
		size_t				m_k{ 0 };
		/////////////////////////////////////////////////////
		size_t              m_archive_max_size{ false };
		Scalar				m_pmin{ Scalar(0.0) };
		//////////////////////////////////////////////////////
		size_t              m_curr_nfe{ size_t(0) };
		//////////////////////////////////////////////////////
		std::vector<Scalar> m_mu_f;
		std::vector<Scalar> m_mu_cr;
		Population	        m_archive;
		Mutation::SPtr      m_mutation;
		Crossover::SPtr     m_crossover;

		//reduce archive
		void reduce_archive()
		{
			while (m_archive_max_size < m_archive.size())
			{
				m_archive[main_random().index_rand(m_archive.size())] = m_archive.last();
				m_archive.pop_back();
			}
		}
		//reduce population
		void reduce_population(DoubleBufferPopulation& dpopulation)
		{
			using FPSize_t = double;
			//compute new np
			size_t new_np = size_t(std::round(
				((FPSize_t(*parameters().m_min_np) - FPSize_t(*parameters().m_np)) / FPSize_t(*parameters().m_max_nfe)) 
				* FPSize_t(m_curr_nfe) 
				+ FPSize_t(*parameters().m_np)
			));
			//max/min safe
			new_np = clamp<size_t>(new_np, *parameters().m_min_np, *parameters().m_np);
			//reduce
			if (new_np < current_np())
			{
				//sort population
				dpopulation.parents().sort();
				//reduce
				dpopulation.resize(new_np);
				//test
				assert(current_np() == new_np);
				//MESSAGE("np: " << new_np);
			}
		}
	};
	REGISTERED_EVOLUTION_METHOD(L_SHADEMethod, "L-SHADE")

	#if 0
	class SaDEMethod : public EvolutionMethod
	{
	public:

		SaDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			//m_epoct = parameters().m_epoct;
		}

		virtual void start() override
		{
			//reinit
			for (size_t i = 0; i != 2; ++i)
			{
				m_nfalse[i] = 0;
				m_ntesut[i] = 0;
				m_p[i]	    = Scalar(0.5);
			}
			//restart counter
			m_curr_epoct = m_epoct;
			//create mutation/crossover
			m_mutation[0] = MutationFactory::create("rand/1", m_algorithm);
			m_mutation[1] = MutationFactory::create("current_to_best/1", m_algorithm);
			m_crossover   = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//none
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t target
			, Individual& i_output
		)
		override
		{
			//todo
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			//todo
			for (size_t i = 0; i != dpopulation.sons().size(); ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (father->m_eval >= son->m_eval)
				{
					//todo
					dpopulation.swap(i);
				}
			}
			//todo
		}

		virtual const VariantRef get_context_data() const override
		{
			//todo
			return VariantRef();
		}

	protected:

		size_t			   m_epoct     { 50   };
		size_t			   m_curr_epoct{ 50   };
		size_t             m_nfalse[2] { 0, 0 };
		size_t             m_ntesut[2] { 0, 0 };
		Scalar			   m_p[2]      { Scalar(0.5), Scalar(0.5) };
		Population	       m_archive;
		Mutation::SPtr     m_mutation[2];
		Crossover::SPtr    m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(SaDEMethod, "SADE")
	#endif 

}