#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
namespace Denn
{
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
				if (loss_function_compare(son->m_eval,father->m_eval))
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
				denn_assert(current_np() == new_np);
			}
		}
	};
	REGISTERED_EVOLUTION_METHOD(L_SHADEMethod, "L-SHADE")

	template < typename T >
	class MultiArmedBanditsBAIO
	{
	public:
		using Action = T;
		using ActionPtr = typename T::SPtr;

		MultiArmedBanditsBAIO()
			: m_T(1)
			, m_j(0)
		{}

		MultiArmedBanditsBAIO(const std::vector < ActionPtr >& actions)
			: m_T(1)
			, m_j(0)
			, m_actions(actions)
			, m_s(actions.size(), Scalar(1.0))
			, m_n(actions.size(), size_t(1.0))
			, m_cost(actions.size(), Scalar(std::numeric_limits<Scalar>::max()))
		{}

		void init(const std::vector < ActionPtr >& actions)
		{
			m_T = 1;
			m_j = 0;
			m_actions = actions;
			m_s = std::vector < Scalar >(actions.size(), Scalar(1.0));
			m_n = std::vector < size_t >(actions.size(), size_t(1.0));
			m_cost = std::vector < Scalar >(actions.size(), Scalar(std::numeric_limits<Scalar>::max()));
		}

		ActionPtr get()
		{
			return m_actions[m_j];
		}

		Scalar& s()
		{
			return m_s[m_j];
		}

		Scalar& n()
		{
			return m_n[m_j];
		}

		void update(Scalar rewards = 0, size_t nT = 1)
		{
			//update
			m_s[m_j] += rewards;
			//update cost
			m_cost[m_j] = (m_s[m_j] / m_n[m_j]) * std::sqrt(Scalar(2) * std::log(m_T + 1) / m_n[m_j]);
			//update count
			m_n[m_j] += 1;
			m_T += nT;
			//argmin
			//MESSAGE("")
			//MESSAGE("-----------------------------")
			//MESSAGE("J = " << m_j)
			//MESSAGE("rewards: " << rewards)
			//MESSAGE("factor: " << std::sqrt( Scalar(2) * std::log(m_T) / m_n[m_j] ))
			for (size_t j = 0; j != m_actions.size(); ++j)
			{
				if (m_cost[m_j] < m_cost[j]) m_j = j;
				//MESSAGE("[" << j << "] =" << m_cost[j] << ", " << m_s[j] << ", " << m_n[j] )
			}
			//MESSAGE("J -> " << m_j)
		}

	protected:

		size_t					  m_j;
		size_t					  m_T;
		std::vector < ActionPtr > m_actions;
		std::vector < Scalar > 	  m_s;
		std::vector < size_t > 	  m_n;
		std::vector < Scalar > 	  m_cost;
	};

	class MAB_SHADEMethod : public EvolutionMethod
	{

	public:

		MAB_SHADEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			//init
			m_archive_max_size = parameters().m_archive_size;
			m_h = parameters().m_shade_h;
			//get mutations
			for(const std::string& mut_name : parameters().m_mutations_list_type.get()) 
			{
				m_mutations_list.push_back(MutationFactory::create(mut_name, m_algorithm));
			}
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
			m_mutations.init(m_mutations_list);
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			if (m_mutations.get()->required_sort()) dpopulation.parents().sort();
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
			(*m_mutations.get()) (dpopulation.parents(), i_target, i_output);
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
			Scalar rewards = 0.0;
			//
			for (size_t i = 0; i != np; ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (loss_function_compare(son->m_eval, father->m_eval))
				{
					if (m_archive_max_size) m_archive.push_back(father->copy());
					//max
					rewards += std::abs(std::abs(son->m_eval) - std::abs(father->m_eval)) / std::abs(father->m_eval);
					//F
					sum_f += son->m_f;
					sum_f2 += son->m_f * son->m_f;
					//w_k (for mean of Scr)
					delta_f = std::abs(std::abs(son->m_eval) - std::abs(father->m_eval));
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
			//update MAB
			m_mutations.s() += (n_discarded ? rewards / n_discarded : 0.0);
		}

		virtual void end_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//update MAB, chose next
			m_mutations.update();
		}

		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:

		size_t				m_h{ 0 };
		size_t				m_k{ 0 };
		size_t              m_archive_max_size{ false };
		Scalar				m_pmin{ Scalar(0.0) };
		std::vector<Scalar> m_mu_f;
		std::vector<Scalar> m_mu_cr;
		Population	        m_archive;
		std::vector<Mutation::SPtr>      m_mutations_list;
		MultiArmedBanditsBAIO<Mutation>  m_mutations;
		Crossover::SPtr                  m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(MAB_SHADEMethod, "MAB-SHADE")


}