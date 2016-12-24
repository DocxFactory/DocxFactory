
#ifndef __DOCXFACTORY_MATH_FUNC_H__
#define __DOCXFACTORY_MATH_FUNC_H__



namespace DocxFactory
{	
	using namespace std;

	class MathFunc
	{
	public:
		static const double m_epsilon;

		static double round( double p_val, unsigned short p_precision = 0 );

		static bool cmpEq( double p_a, double p_b );
		static bool cmpGt( double p_a, double p_b );
		static bool cmpGe( double p_a, double p_b );
		static bool cmpLt( double p_a, double p_b );
		static bool cmpLe( double p_a, double p_b );

	protected:

	private:
		MathFunc();
		MathFunc( const MathFunc& p_other );
		MathFunc& operator = ( const MathFunc& p_other );

	};
};

#endif
