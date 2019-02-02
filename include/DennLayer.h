#pragma once
#include "Config.h"
#include "DennActivationFunction.h"

namespace Denn
{

	class BPOptimizer
	{
	public:
	
		virtual void operator () (const Matrix& gradient, Matrix& matrix) = 0;
	
	};

	class GDOptimizer : public BPOptimizer
	{

		Scalar m_alpha;

	public:

		GDOptimizer(Scalar alpha = 0.01) : m_alpha(alpha) {}

		void operator () (const Matrix& gradient, Matrix& matrix) override
		{
			matrix += (matrix*gradient)*m_alpha;
		}
	};

	class Layer : public std::enable_shared_from_this< Layer >
	{ 
	public:
		//ref to Layer
		using Scalar   = Denn::Scalar;
		using SPtr     = std::shared_ptr<Layer>;
		using Shape    = std::vector< long >;
		using Input	   = std::vector< long >;
		using VMatrix  = std::vector<Matrix>;
		using VVMatrix = std::vector<VMatrix>;
		using VActivationFunction = std::vector< ActivationFunction >;
		//return ptr
		SPtr get_ptr();
		///////////////////////////////////////////////////////////////////////////
		//EIGEN_MAKE_ALIGNED_OPERATOR_NEW	
		///////////////////////////////////////////////////////////////////////////
		virtual Layer::SPtr copy() const = 0;
		virtual size_t size() const  = 0;
		virtual size_t ouput_paramters() const = 0;
		virtual size_t input_shape_dims() const = 0;
		virtual Shape  output_shape() const = 0;
		virtual Matrix& operator[](size_t i) = 0;
		virtual const Matrix& operator[](size_t i) const = 0;
		virtual VActivationFunction get_activation_functions() = 0;
		virtual void                set_activation_functions(const VActivationFunction& active_function) = 0;
		///////////////////////////////////////////////////////////////////////////
		virtual VMatrix   apply(const std::vector<Matrix>& input) const { return {}; }
		///////////////////////////////////////////////////////////////////////////
		//Backpropagation stuff
		virtual VMatrix   feedforward(const VMatrix& input)   { assert(0); return {}; }
		virtual VMatrix   backpropagate(const VMatrix& input) { assert(0); return {}; }
		virtual void      optimizer(BPOptimizer& optimizer)   { assert(0); }
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
		int  m_min{ 0 };
		int  m_max{ 0 };

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

	enum LayerDefaultArguments
	{
		DENN_CAN_BE_AN_INPUT_LAYER	= 0b0001,
		DENN_CAN_BE_AN_OUTPUT_LAYER	= 0b0010,
		DENN_CAN_BE_AN_HIDDEN_LAYER	= 0b0100,
		DENN_CAN_BE_ALL_TYPE_LAYER  = 0b0111,
	};

	struct LayerDescription
	{
		LayerMinMax  m_shape;
		LayerMinMax  m_input;
		LayerMinMax  m_function;
		LayerMinMax  m_output;
		unsigned int m_flags { DENN_CAN_BE_ALL_TYPE_LAYER };

		LayerDescription() = default;

		LayerDescription
		(
			  LayerMinMax shape
			, unsigned int flags = DENN_CAN_BE_ALL_TYPE_LAYER
		)
		{
			m_shape = shape;
			m_flags = flags;
		}

		LayerDescription
		(
			  LayerMinMax shape
			, LayerMinMax min_max
			, unsigned int flags = DENN_CAN_BE_ALL_TYPE_LAYER
		)
		{
			m_shape    = shape;
			m_input    = min_max;
			m_function = min_max;
			m_output   = min_max;
			m_flags    = flags;
		}

		LayerDescription
		(
			  LayerMinMax shape
			, LayerMinMax input
			, LayerMinMax output
			, unsigned int flags = DENN_CAN_BE_ALL_TYPE_LAYER
		)
		{
			m_input    = input;
			m_function = input;
			m_output   = output;
			m_flags    = flags;
		}
		LayerDescription
		(
			  LayerMinMax shape
			, LayerMinMax input
			, LayerMinMax function
			, LayerMinMax output
			, unsigned int flags = DENN_CAN_BE_ALL_TYPE_LAYER
		)
		{
			m_shape    = shape;
			m_input    = input;
			m_function = function;
			m_output   = output;
			m_flags    = flags;
		}

	};
	
	//class factory of Evolution methods
	class LayerFactory
	{

	public:
		//LayerItemFactory classes map
		typedef Layer::SPtr(*CreateObject)(
			  const Layer::Shape& shape
			, const Layer::Input& inputs
			, const Layer::VActivationFunction& active_functions
		);

		//public
		static Layer::SPtr create(
			  const std::string& name
			, const Layer::Shape& shape
			, const Layer::Input& inputs
			, const Layer::VActivationFunction& active_functions
		);
		static void append(const std::string& name, CreateObject fun, const LayerDescription& desc, size_t size);
		
		static void append(const std::vector<std::string>& names, CreateObject fun, const LayerDescription& desc, size_t size);

		static LayerDescription* layer_description(const std::string& name);

		static int min_shape_size(const std::string& name);
		static int max_shape_size(const std::string& name);

		static int min_input_size(const std::string& name);
		static int max_input_size(const std::string& name);

		static int min_output_size(const std::string& name);
		static int max_output_size(const std::string& name);
		
		static int min_activation_size(const std::string& name);
		static int max_activation_size(const std::string& name);

		static unsigned int flags(const std::string& name);

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

		static Layer::SPtr create
		(
			  const Layer::Shape& shape
			, const Layer::Input& inputs
			, const Layer::VActivationFunction& active_functions
		)
		{
			return (std::make_shared< T >(shape, inputs, active_functions))->get_ptr();
		}

		LayerItem(const std::string& name, const LayerDescription& desc, size_t size)
		{
			LayerFactory::append(name, LayerItem<T>::create, desc, size);
		}

		LayerItem(const std::vector<std::string>& names, const LayerDescription& desc, size_t size)
		{
			LayerFactory::append(names, LayerItem<T>::create, desc, size);
		}

	public:


		static LayerItem<T>& instance
		(
			  const std::string& name
			, const LayerDescription& desc
			, size_t size
		)
		{
			static LayerItem<T> object_item(name, desc, size);
			return object_item;
		}

		static LayerItem<T>& instance
		(
			  const std::vector<std::string>& names
			, const LayerDescription& desc
			, size_t size
		)
		{
			static LayerItem<T> object_item(names, desc, size);
			return object_item;
		}

	};

    #define LAYER_NAMES(...) (std::vector<std::string>{ __VA_ARGS__ })
	#define REGISTERED_LAYER(class_, name_ , ... )\
	namespace\
	{\
		static const LayerItem<class_>& _Denn_ ## class_ ## _LayerItem = LayerItem<class_>::instance( name_, { __VA_ARGS__ }, sizeof(class_) );\
	}

}