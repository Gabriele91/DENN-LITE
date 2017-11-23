//
// Created by Valerio Belli on 15/11/17.
//
#pragma once
#include "Config.h"

namespace Denn
{
    class Gate : public std::enable_shared_from_this< Gate >
    {
    public:
        enum Arity
        {
            CONST = 0,
            UNARY = 1,
            BINARY= 2
        };

        //ref to Crossover
        using SPtr = std::shared_ptr<Gate>;

        //return ptr
        SPtr get_ptr() { return this->shared_from_this(); }

        //contructor
        Gate() { /* void */ }

        virtual Matrix operator () (Matrix& M)
        {
            return Matrix();
        }

        virtual Matrix operator () (const Matrix& A, Matrix& M)
        {
            return Matrix();
        }

        virtual Matrix operator () (const Matrix& A, const Matrix& B, Matrix& M)
        {
            return Matrix();
        }

        const std::string& name() const
        {
            return m_name;
        }

        Arity arity() const
        {
            return m_arity;
        }

    protected:

        Gate(const std::string& name, Arity arity) : m_name(name), m_arity(arity) {}

        std::string m_name;

        Arity m_arity;

    };

    //class factory of gate methods
    class GateFactory
    {

    public:
        //Gate classes map
        typedef Gate::SPtr(*CreateObject)();

        //public
        static Gate::SPtr create(const std::string& name);
        static void append(const std::string& name, CreateObject fun, size_t size);

        //list of methods
        static std::vector< std::string > list_of_gates();
        static std::string names_of_gates(const std::string& sep = ", ");

        //info
        static bool exists(const std::string& name);

    };

    //class used for static registration of a object class
    template<class T>
    class GateItem
    {

        static Gate::SPtr create()
        {
            return (std::make_shared< T >())->get_ptr();
        }

        GateItem(const std::string& name, size_t size)
        {
            GateFactory::append(name, GateItem<T>::create, size);
        }

    public:


        static GateItem<T>& instance(const std::string& name, size_t size)
        {
            static GateItem<T> objectItem(name, size);
            return objectItem;
        }

    };


    #define REGISTERED_GATE(class_,name_)\
    namespace\
    {\
        static const auto& _Denn_ ## class_ ## _GateItem= GateItem<class_>::instance( name_, sizeof(class_) );\
    }
}