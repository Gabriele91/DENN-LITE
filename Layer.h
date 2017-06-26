#pragma once
#include "Config.h"

namespace Denn
{
	class Layer 
	{
	public:		
		virtual Layer* copy() const  					= 0;
		virtual Matrix apply(const Matrix& input)       = 0;
		virtual size_t size() const                     = 0;
		virtual Matrix& operator[](size_t i)            = 0;
		virtual const Matrix& operator[](size_t i) const= 0;

		class Iterator 
		{
		public:

			Iterator(const Iterator& it) :m_layer(it.m_layer), m_index(it.m_index) {}			
			Iterator(      Layer& layer,size_t index) :m_layer((Layer*)&layer), m_index(index) {}			
			Iterator(const Layer& layer,size_t index) :m_layer((Layer*)&layer), m_index(index) {}	
	
			Iterator& operator++()   { ++m_index; return *this;                     }
			Iterator operator++(int) { Iterator it(*this); operator++(); return it; }
			
			bool operator==(const Iterator& rhs) const {return m_index==rhs.m_index;}
			bool operator!=(const Iterator& rhs) const {return m_index!=rhs.m_index;}

			      Matrix& operator*() 	    { return (*m_layer)[m_index];                 }
			const Matrix& operator*() const { return (*((const Layer*)m_layer))[m_index]; }

		protected:
			friend class Denn::Layer;
			Layer* m_layer;
			size_t m_index;
		};

		Iterator begin()		     { return Iterator(*this,0); }
		Iterator end()   			 { return Iterator(*this,size()); }
		const Iterator begin() const { return Iterator(*this,0); }
		const Iterator end()   const { return Iterator(*this,size()); }
	};
}