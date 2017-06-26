#pragma once
#include "Config.h"

namespace Denn
{
	class Layer 
	{
	public:		
		///////////////////////////////////////////////////////////////////////////
		virtual Layer* copy() const  					= 0;
		virtual Matrix apply(const Matrix& input)       = 0;
		virtual size_t size() const                     = 0;
		virtual Matrix& operator[](size_t i)            = 0;
		virtual const Matrix& operator[](size_t i) const= 0;
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
}