#pragma once
#include "DennUtilitiesNetworks.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
	NNFromStringOut  get_network_from_string
	(
		const std::string& network_string,
		const int in_shape1D,
		const int out_shape1D
	)
	{
		//parser state
		enum State
		{
			S_READ_TYPE,
			S_READ_SHAPE,
			S_READ_INPUTS,
			S_READ_ACTIVATIONS,
			S_FINALIZE
		};
		//source ptr
		const char* ptr = network_string.data();
		//name layer
		State state{ S_READ_TYPE };
		//net
		NeuralNetwork nn;
		std::string err;
		//layer info
		bool first = true;
		std::string type;
		std::vector<int> shape;
		std::vector<int> inputs;
		std::string function;
		std::vector < std::string > functions;
		//temp layer input
		Shape in_shape;
		Inputs in_inputs;
		ActivationFunctions in_functions;
		//ptr
		while (*ptr || state == S_FINALIZE)
		{
			//jump space
			while (std::isspace(*ptr)) ++ptr;
			//change parse state
			if (state == S_READ_TYPE)
			{
				if ((!type.size() && std::isalpha(*ptr)) ||
					(type.size() && (std::isalnum(*ptr) || *ptr == '_')))
				{
					type += *ptr;
				}
				else if (*ptr == '(')
				{
					state = S_READ_SHAPE;
					//all lower case
					std::transform(type.begin(), type.end(), type.begin(), ::tolower);
					//test
					if (!LayerFactory::exists(type))
					{
						err += type + " is an invalid layer name";
						return std::make_tuple(nn, err, false);
					}
				}
				//next
				++ptr; continue;
			}
			else if (state == S_READ_SHAPE)
			{
				if (std::isdigit(*ptr))
				{
					//parse
					char *outptr = nullptr;
					shape.push_back(std::strtol(ptr, &outptr, 10));
					if (ptr == outptr)
					{
						err += std::string(ptr-2, 5) + " is not valid shape";
						return std::make_tuple(nn, err, false);
					}
					//next
					ptr = outptr; continue;
				}
				else if (',')
				{
					state = S_READ_INPUTS;
					//next
					++ptr; continue;
				}
				else if (*ptr == ')')
				{
					state = S_FINALIZE;
					//next
					++ptr; continue;
				}
			}
			else if (state == S_READ_INPUTS)
			{
				if (std::isdigit(*ptr))
				{
					//parse
					char *outptr = nullptr;
					inputs.push_back(std::strtol(ptr, &outptr, 10));
					if (ptr == outptr)
					{
						err += std::string(ptr - 2, 5) + " is not valid input";
						return std::make_tuple(nn, err, false);
					}
					//next
					ptr = outptr; continue;
				}
				else if (',')
				{
					state = S_READ_ACTIVATIONS;
					//next
					++ptr; continue;
				}
				else if (*ptr == ')')
				{
					state = S_FINALIZE;
					//next
					++ptr; continue;
				}
			}
			else if (state == S_READ_ACTIVATIONS)
			{
				if ((!type.size() && std::isalpha(*ptr)) ||
					(type.size() && (std::isalnum(*ptr) || *ptr == '_')))
				{
					function += *ptr;
				}
				else if (*ptr == ')')
				{
					state = S_FINALIZE;
					//next
					++ptr; continue;
				}
				//next
				++ptr;
				//is a space/)?
				if (std::isspace(*ptr) || *ptr == ')')
				{
					//all lower case
					std::transform(function.begin(), function.end(), function.begin(), ::tolower);
					//test
					if (!ActivationFunctionFactory::exists(function))
					{
						err += type + " is an invalid activation function name";
						return std::make_tuple(nn, err, false);
					}
					//push
					functions.push_back(function);
					//clean
					function.clear();
				}
				//end state
				continue;
			}
			else if (state == S_FINALIZE)
			{
				//get if is the last
				while (std::isspace(*ptr)) ++ptr;
				//get layer info
				auto* description = LayerFactory::description(type);
				//test shape
				if (first)
				{
					switch (shape.size())
					{
					case 1: 
						if (!(description->shape_type() & SHAPE_1D))
						{
							err += "Shape for this layer must to be 1D";
							return std::make_tuple(nn, err, false);
						}
						in_shape = Shape(shape[0]);
					break;
					case 2:
						if (!(description->shape_type() & SHAPE_2D))
						{
							err += "Shape for this layer must to be 2D";
							return std::make_tuple(nn, err, false);
						}
						in_shape = Shape(shape[0],shape[1]);
					break;
					case 3:
						if (!(description->shape_type() & SHAPE_3D))
						{
							err += "Shape for this layer must to be 3D";
							return std::make_tuple(nn, err, false);
						}
						in_shape = Shape(shape[0], shape[1], shape[2]);
					break;
					//0 input, default
					case 0:
						if ((0 < in_shape1D) 
						&&  first 
						&&  (description->shape_type() & SHAPE_1D))
						{
							in_shape = Shape(in_shape1D);
							break;
						}
					default:
						err += "Shape must to be 1D/2D/3D";
						return std::make_tuple(nn, err, false);
					break;
					}
				}
				else
				{
					in_shape = nn[nn.size() - 1].out_size();
				}
				//test inputs
				if (inputs.size() < description->inputs().min()
				||  inputs.size() > description->inputs().max())
				{
					err += "Inputs for this layer are not invalid"
						   " (min: " + std::to_string(description->inputs().min()) + 
						   ", max: " + std::to_string(description->inputs().max()) + ")";
					return std::make_tuple(nn, err, false);
				}
				in_inputs = inputs;
				//test functions
				if (functions.size() < description->funcs().min()
				||  functions.size() > description->funcs().max())
				{
					err += "Functions for this layer are not invalid"
						   " (min: " + std::to_string(description->funcs().min()) +
						   ", max: " + std::to_string(description->funcs().max()) + ")";
					return std::make_tuple(nn, err, false);
				}
				for (auto& name_afunc : functions)
				{
					in_functions.push_back(ActivationFunctionFactory::get(name_afunc));
				}
				//new layer
				auto layer = LayerFactory::create(type, in_functions, in_shape, in_inputs);
				//add
				nn.add_layer(layer);
				//ok
				first = false;
				type.clear();
				function.clear();
				inputs.clear();
				shape.clear();
				functions.clear();
				//in clear
				in_shape = Shape();
				in_inputs.clear();
				in_functions.clear();
				//restart
				state = S_READ_TYPE;
			}
		}
		//ok
		return std::make_tuple(nn, err, true);
	}


	TestNNStringOut  get_network_from_string_test(const std::string& network_string)
	{
		auto value = get_network_from_string(network_string, 1);
		return { std::get<1>(value), std::get<2>(value) };
	}

	std::string  get_string_from_network
	(
		const NeuralNetwork& nn
	)
	{
		//out string
		std::string out;
		//for each layers
		for (size_t i = 0; i < nn.size(); ++i)
		{
			const Layer& layer = nn[i];
			//type shape
			std::string type = layer.name();
			std::string shape;
			std::string inputs;
			std::string functions;
			//get layer info
			auto* description = LayerFactory::description(type);
			//shape
			if (!i)
			{
				if (description->shape_type() & SHAPE_3D)
					shape =
					std::to_string(layer.in_size().width()) + " " +
					std::to_string(layer.in_size().height()) + " " +
					std::to_string(layer.in_size().channels());
				else if (description->shape_type() & SHAPE_2D)
					shape =
					std::to_string(layer.in_size().width()) + " " +
					std::to_string(layer.in_size().height());
				else
					shape = std::to_string(layer.in_size().width());

			}
			//inputs
			for (int in : layer.inputs()) inputs += " " + std::to_string(in);
			//activations
			for (auto& afname : layer.activations_names()) functions += " " + afname;
			//out string
			out += (i ? " " : "") + type + "(" + shape + "," + inputs + ", " + functions + ")";
		}
		return out;
	}
}
