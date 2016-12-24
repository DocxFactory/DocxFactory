
#include "DocxFactory/math/MathFunc.h"

#include <cmath>

using namespace DocxFactory;
using namespace std;



const double MathFunc::m_epsilon = 0.0000001f;



double MathFunc::round( double p_val, unsigned short p_precision )
{
	bool	l_neg = false;
	double	l_frac;
	double	l_val;

	if ( p_val < 0 )
	{
		l_neg = true;
		p_val = -p_val;
	}

	if ( p_precision > 10 )
		p_precision = 10;

	if ( p_precision > 0 )
	{
		double l_pow = pow( 10.0f, p_precision );
		p_val = p_val * l_pow;

		l_frac = modf( p_val, &l_val );
		if ( l_frac >= 0.5 ) l_val++;

		p_val = l_val / l_pow;
	}

	else
	{
		l_frac = modf( p_val, &l_val );
		if ( l_frac >= 0.5 ) l_val++;

		p_val = l_val;
	}

	if ( l_neg )
		p_val = -p_val;

	return p_val;
} // round



bool MathFunc::cmpEq( double p_a, double p_b )
{
	return ( fabs( p_a - p_b ) <= m_epsilon );
} // cmpEq

bool MathFunc::cmpGt( double p_a, double p_b )
{
	return ( p_a > p_b && !cmpEq( p_a, p_b ) );
} // cmpGt

bool MathFunc::cmpGe( double p_a, double p_b )
{
	return ( p_a > p_b || cmpEq( p_a, p_b ) );
} // cmpGe

bool MathFunc::cmpLt( double p_a, double p_b )
{
	return ( p_a < p_b && !cmpEq( p_a, p_b ) );
} // cmpLt

bool MathFunc::cmpLe( double p_a, double p_b )
{
	return ( p_a < p_b || cmpEq( p_a, p_b ) );
} // cmpLe
