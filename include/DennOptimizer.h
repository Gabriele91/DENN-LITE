#pragma once
#include "Config.h"

namespace Denn
{

class Optimizer
{
public:
	virtual void update(ColVector::ConstAlignedMapType&,ColVector::AlignedMapType&) const= 0;
};

class SGD : public Optimizer
{
protected:

	Scalar m_lrate;
	Scalar m_decay;

public:
	
	SGD(Scalar lrate = Scalar(0.01), Scalar decay = Scalar(0))
	: m_lrate(lrate)
	, m_decay(decay)
	{
	}

	virtual void update
	(
	  ColVector::ConstAlignedMapType& dvec
	, ColVector::AlignedMapType& vec
	) const override
	{
		vec.noalias() -= m_lrate * (dvec + m_decay * vec);
	}
};

}