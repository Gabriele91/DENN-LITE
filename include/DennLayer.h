#pragma once
#include "Config.h"
#include "DennShape.h"
#include "DennOptimizer.h"
#include "DennActivationFunction.h"

namespace Denn
{
	using Inputs			  = std::vector< int >;
	using ActivationFunctions = std::vector< ActivationFunction >;

	class Layer : public std::enable_shared_from_this< Layer >
	{
	public:


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

		//alias
		using Matrix = Denn::Matrix;
		using Scalar = Denn::Scalar;
		using SPtr = std::shared_ptr<Layer>;
		//Costructor
		Layer(const std::string& name,Shape in, Shape out): m_name(name), m_in_size(in), m_out_size(out) {}
		//info layer (serialization)
		virtual const std::string& name() const { return m_name; }
		virtual const Inputs inputs() const = 0;
		virtual const std::vector< std::string > activations_names() const;
		//info shape
		virtual Shape in_size() const { return m_in_size; }
		virtual Shape out_size() const { return m_out_size; }
		///////////////////////////////////////////////////////////////////////////
		virtual void activations(ActivationFunctions) = 0;
		virtual ActivationFunctions activations() const = 0;
		///////////////////////////////////////////////////////////////////////////
		//return ptr
		SPtr get_ptr();
		virtual SPtr copy()   const = 0;
		///////////////////////////////////////////////////////////////////////////
		virtual const Matrix& feedforward(const Matrix& prev_layer_data)								  = 0;
		virtual const Matrix& backpropagate(const Matrix& prev_layer_data, const Matrix& next_layer_data) = 0;
		///////////////////////////////////////////////////////////////////////////
		virtual void update(const Optimizer& optimize) = 0;
		///////////////////////////////////////////////////////////////////////////
		virtual const Matrix& ff_output() = 0;
		virtual const Matrix& bp_output() = 0;
		///////////////////////////////////////////////////////////////////////////
		virtual size_t        size()  const              = 0;
		virtual Matrix&       operator[](size_t i)       = 0;
		virtual const Matrix& operator[](size_t i) const = 0;
		///////////////////////////////////////////////////////////////////////////
		Iterator begin();
		Iterator end();
		const Iterator begin() const;
		const Iterator end()   const;
		///////////////////////////////////////////////////////////////////////////

	protected:

		const std::string m_name; // layer name
		const Shape m_in_size;    // Size of input units
		const Shape m_out_size;   // Size of output units 

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

	enum LayerShapeType
	{
		SHAPE_1D = 0b001,
		SHAPE_2D = 0b010,
		SHAPE_3D = 0b100,
		SHAPE_1D_2D = 0b011,
		SHAPE_1D_3D = 0b101,
		SHAPE_2D_3D = 0b110,
		SHAPE_1D_2D_3D = 0b111
	};

	struct LayerDescription
	{
		struct MinMax
		{
			int m_min;
			int m_max;
			MinMax(int v) :m_min(v), m_max(v) {}
			MinMax(int min,int max) : m_min(min), m_max(max) {}

			const int min() const { return m_min; }
			const int max() const { return m_max; }
		};

		LayerShapeType m_shape_type{ SHAPE_1D };
		MinMax		   m_intpus{ 0 };
		MinMax		   m_funcs{ 0 };

		LayerDescription(const LayerShapeType& shape, MinMax inputs, MinMax funcs)
		{
			m_shape_type = shape;
			m_intpus = inputs;
			m_funcs = funcs;
		}

		const LayerShapeType shape_type() const { return m_shape_type; }
		const MinMax& inputs() const { return m_intpus; }
		const MinMax& funcs() const { return m_funcs; }

	protected:

		LayerDescription() = default;
		friend struct LayerInfoLayer;

	};

	//class factory of Evolution methods
	class LayerFactory
	{

	public:
		//LayerItemFactory classes map
		typedef Layer::SPtr(*CreateObject)
		(
			  const ActivationFunctions& active_functions
			, const Shape& shape
			, const Inputs& inputs
		);

		//public
		static Layer::SPtr create
		(
			  const std::string& name
			, const ActivationFunctions& active_functions
			, const Shape& shape
			, const Inputs& inputs
		);

		static void append(const std::vector<std::string>& names, CreateObject fun, const LayerDescription& desc, size_t size);
		static LayerDescription* description(const std::string& name);

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
			  const ActivationFunctions& active_functions
			, const Shape& shape
			, const Inputs& inputs
		)
		{
			return (std::make_shared< T >(active_functions, shape, inputs))->get_ptr();
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
			static LayerItem<T> object_item(std::vector<std::string>{ name }, desc, size);
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
	#define REGISTERED_LAYER(class_, names_ , ... )\
	namespace\
	{\
		static const LayerItem<class_>& _Denn_ ## class_ ## _LayerItem = LayerItem<class_>::instance( names_, LayerDescription{ __VA_ARGS__ }, sizeof(class_) );\
	}
}