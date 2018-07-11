#pragma once
#include "Config.h"
#include "DennActivationFunction.h"

namespace Denn
{

	class Layer : public std::enable_shared_from_this< Layer >
	{ 
	public:
		//ref to Layer
		using VActivationFunction = std::vector< ActivationFunction >;
		using VMatrix  = std::vector<Matrix>;
		using VVMatrix = std::vector<VMatrix>;
		using Scalar   = Denn::Scalar;
		using SPtr     = std::shared_ptr<Layer>;
		//return ptr
		SPtr get_ptr();
		///////////////////////////////////////////////////////////////////////////
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW	
		///////////////////////////////////////////////////////////////////////////
		virtual Layer::SPtr copy() const  				                                                 = 0;
		virtual Matrix apply(const Matrix& input) const								                     = 0;
		virtual size_t size() const                                                                      = 0;
		virtual size_t size_ouput() const											                     = 0;
		virtual Matrix& operator[](size_t i)								                             = 0;
		virtual const Matrix& operator[](size_t i) const						                         = 0;
		virtual VActivationFunction get_activation_functions()										     = 0;
		virtual void                set_activation_functions(const VActivationFunction& active_function) = 0;
		///////////////////////////////////////////////////////////////////////////
		//Backpropagation stuff
		virtual Matrix    feedforward(const Matrix& input, Matrix& linear_out)				  							     = 0;
		virtual Matrix    backpropagate_delta(const Matrix& loss)     							                             = 0;		
		virtual Matrix    backpropagate_derive(const Matrix& delta, const Matrix& linear_out)       			             = 0;
		virtual VMatrix   backpropagate_gradient(const Matrix& delta, const Matrix& linear_input, Scalar regular=Scalar(0.0))= 0;
		///////////////////////////////////////////////////////////////////////////
		//Recurrent network
		virtual VMatrix apply(const std::vector<Matrix>& input) const					     							     = 0;
		virtual VMatrix feedforward(const VMatrix& input, VMatrix& linear_out)				  							     = 0;
		virtual VMatrix backpropagate_delta(const VMatrix& loss)     							                             = 0;		
		virtual VMatrix backpropagate_derive(const VMatrix& delta, const VMatrix& linear_out)       			             = 0;
		virtual VVMatrix backpropagate_gradient(const VMatrix& delta, const VMatrix& linear_input, Scalar regular=Scalar(0.0))= 0;
		///////////////////////////////////////////////////////////////////////////
		class Iterator 
		{
		public:

			Iterator(const Iterator& it);
			Iterator(Layer& layer, size_t index);
			Iterator(const Layer& layer, size_t index);

			Iterator& operator++();
			Iterator operator++(int);
			
			bool operator==(const Iterator& rhs) const;
			bool operator!=(const Iterator& rhs) const;

			Matrix& operator*();
			const Matrix& operator*() const;

		protected:

			friend class Denn::Layer;
			Layer* m_layer;
			size_t m_index;
		};

		Iterator begin();
		Iterator end();
		const Iterator begin() const;
		const Iterator end()   const;
		///////////////////////////////////////////////////////////////////////////
	};

	template <>
	inline Layer::Scalar distance_pow2<Layer>(const Layer& a, const Layer& b)
	{
		//bad case
		if(a.size()!=b.size()) return std::numeric_limits<Scalar>::infinity();
		//value
		Scalar dpow2 = 0.0;
		//sum
		for(size_t i = 0; i!=a.size() ; ++i) dpow2 += distance_pow2(a[i],b[i]);
		//return 
		return dpow2;
	} 

	struct LayerMinMax
	{
		int  m_min{ 1 };
		int  m_max{ 1 };

		LayerMinMax() 
		{

		}

		LayerMinMax(int min_max)
		{
			m_min = min_max;
			m_max = min_max;
		}

		LayerMinMax(int min, int max)
		{
			m_min = min;
			m_max = max;
		}
	};

	struct LayerDescription
	{
		LayerMinMax m_input;
		LayerMinMax m_function;
		LayerMinMax m_output;
		bool m_can_be_output{ true };

		LayerDescription()
		{
		}

		LayerDescription(LayerMinMax min_max, bool can_be_output = true)
		{
			m_input    = min_max;
			m_function = min_max;
			m_output   = min_max;
			m_can_be_output = can_be_output;
		}

		LayerDescription(LayerMinMax input, LayerMinMax output, bool can_be_output = true)
		{
			m_input    = input;
			m_function = input;
			m_output   = output;
			m_can_be_output = can_be_output;
		}
		LayerDescription(LayerMinMax input, LayerMinMax function, LayerMinMax output, bool can_be_output = true)
		{
			m_input    = input;
			m_function = function;
			m_output   = output;
			m_can_be_output = can_be_output;
		}

	};
	
	//class factory of Evolution methods
	class LayerFactory
	{

	public:
		//LayerItemFactory classes map
		typedef Layer::SPtr(*CreateObject)(const std::vector<ActivationFunction>& active_function, const std::vector<size_t>& input_output);

		//public
		static Layer::SPtr create(const std::string& name, const std::vector<ActivationFunction>& active_functions, const std::vector<size_t>& inputs);
		static void append(const std::string& name, CreateObject fun, const LayerDescription& desc, size_t size);
		
		static int min_input_size(const std::string& name);
		static int max_input_size(const std::string& name);

		static int min_output_size(const std::string& name);
		static int max_output_size(const std::string& name);
		
		static int min_activation_size(const std::string& name);
		static int max_activation_size(const std::string& name);

		static bool can_be_output(const std::string& name);

		//list of methods
		static std::vector< std::string > list_of_layers();
		static std::string names_of_layers(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};

	//class used for static registration of a object class
	template<class T>
	class LayerItem
	{

		static Layer::SPtr create(const std::vector<ActivationFunction>&  active_functions, const std::vector<size_t>& inputs)
		{
			return (std::make_shared< T >(active_functions, inputs))->get_ptr();
		}

		LayerItem(const std::string& name, const LayerDescription& desc, size_t size)
		{
			LayerFactory::append(name, LayerItem<T>::create, desc, size);
		}

	public:


		static LayerItem<T>& instance(const std::string& name, const LayerDescription& desc, size_t size)
		{
			static LayerItem<T> objectItem(name, desc, size);
			return objectItem;
		}

	};

	#define REGISTERED_LAYER(class_, name_ , ... )\
	namespace\
	{\
		static const LayerItem<class_>& _Denn_ ## class_ ## _LayerItem = LayerItem<class_>::instance( name_, { __VA_ARGS__ }, sizeof(class_) );\
	}

}