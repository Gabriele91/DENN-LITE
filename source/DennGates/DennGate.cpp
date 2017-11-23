//
// Created by Valerio Belli on 15/11/17.
//
#include <iterator>
#include "DennGate.h"


namespace Denn
{
    //map
    static std::map< std::string, GateFactory::CreateObject >& g_map()
    {
        static std::map< std::string, GateFactory::CreateObject > g_map;
        return g_map;
    }
    //public
    Gate::SPtr GateFactory::create(const std::string& name)
    {
        //find
        auto it = g_map().find(name);
        //return
        return it->second();
    }
    void GateFactory::append(const std::string& name, GateFactory::CreateObject fun, size_t size)
    {
        //add
        g_map()[name] = fun;
    }
    //list of methods
    std::vector< std::string > GateFactory::list_of_gates()
    {
        std::vector< std::string > list;
        for (const auto & pair : g_map()) list.push_back(pair.first);
        return list;
    }
    std::string GateFactory::names_of_gates(const std::string& sep)
    {
        std::stringstream sout;
        auto list = list_of_gates();
        std::copy(list.begin(), list.end()-1, std::ostream_iterator<std::string>(sout, sep.c_str()));
        sout << *(list.end() - 1);
        return sout.str();
    }
    bool GateFactory::exists(const std::string& name)
    {
        //find
        auto it = g_map().find(name);
        //return
        return it != g_map().end();
    }
}