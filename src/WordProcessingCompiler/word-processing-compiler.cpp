
#include "DocxFactory/WordProcessingCompiler/dfwc.h"

#include "DocxFactory/console/getopt.h"
#include "DocxFactory/console/ConsoleUsageException.h"
#include "DocxFactory/console/GetOptException.h"
#include "DocxFactory/console/InvalidArgException.h"
#include "DocxFactory/console/InvalidOptException.h"
#include "DocxFactory/console/MissingArgException.h"
#include "DocxFactory/console/MissingOptException.h"
#include "DocxFactory/console/UnexpectedArgException.h"
#include "DocxFactory/console/UnexpectedOptException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/UnsupportedFileTypeException.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/DirNotFoundException.h"

#include "DocxFactory/locale/LocaleFunc.h"
#include "DocxFactory/str/StrFunc.h"

#include "DocxFactory/util/DocxFactoryDefs.h"
#include "DocxFactory/util/DocxFactoryException.h"
#include "DocxFactory/util/tchar.h"

#include <cstdlib>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

using namespace DocxFactory;
using namespace std;



class Compiler
{
public:
	static void	initialize( int p_argc, TCHAR* p_argv[] );
	static void	compile();
	static void	showUsage();

	static bool	isHelp();

private:
	Compiler( const Compiler& p_other );
	Compiler &operator = ( const Compiler& p_other );

	static string	m_sourceFile;
	static string	m_targetFile;
	static bool		m_help;
};



// tmain and tchar macros are defined in util/tchar.h
// and will compile under unix/linux as well.
int _tmain( int argc, TCHAR* argv[] )
{
	try
	{
		time_t	l_start = clock();

		Compiler::initialize( argc, argv );

		if ( Compiler::isHelp() )
		{
			Compiler::showUsage();
			return EXIT_SUCCESS;
		}

		Compiler::compile();

		cout<< "Completed (in "
			<< LocaleFunc::numToStr( ( double ) ( clock() - l_start ) / ( double ) CLOCKS_PER_SEC, 3, 0, true )
			<< " seconds)."
			<< endl;

		return EXIT_SUCCESS;
	}

	catch ( GetOptException )
	{
		Compiler::showUsage();
		exit( EXIT_FAILURE );
	}

	catch ( const ConsoleUsageException& e )
	{
		cout << e.what() << endl;
		cout << endl;

		Compiler::showUsage();
		exit( EXIT_FAILURE );
	}

	catch ( const exception& e )
	{
		cout << e.what() << endl;
		cout << endl;
		cout << CONTACTUS << endl;

		exit( EXIT_FAILURE );
	}
} // main



// static data members definition

string	Compiler::m_sourceFile	= "";
string	Compiler::m_targetFile	= "";
bool	Compiler::m_help		= false;



// public

void Compiler::initialize( int p_argc, TCHAR* p_argv[] )
{
	struct option l_optList[] =
	{
		{ _T( "T" ),	required_argument,	0, _T( 'T' ) },
		{ _T( "help" ),	no_argument,		0, _T( 'h' ) },
		{ _T( "h" ),	no_argument,		0, _T( 'h' ) },
		{ 0, 0, 0, 0 }
	};

	string	l_optArgUtf8;
	int		l_optIdx = 0;
	int		l_opt;

	string	l_dir;
	string	l_file;
	string	l_ext;

	opterr = 1;

	while ( ( l_opt = getopt_long_only( p_argc, p_argv, _T( "" ), l_optList, &l_optIdx) ) != -1 )
	{
		if ( optarg )
			l_optArgUtf8 = LocaleFunc::convertToUtf8( optarg );
		else
			l_optArgUtf8 = "";

		switch ( l_opt )
		{
		case _T( 'T' ):

			dfwc_setTempDir( l_optArgUtf8.c_str() );

			if ( dfwc_getErrorFlag() )
				throw runtime_error( dfwc_getErrorWhat() );

			break;

		case _T( 'h' ):

			m_help = true;
			return;

		case '?':

			throw GetOptException( __FILE__, __LINE__ );
			break;
		}
	}

	if ( optind >= p_argc )
		throw MissingArgException( "<source file>", __FILE__, __LINE__ );

	l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
	m_sourceFile = l_optArgUtf8;



	if ( optind < p_argc ) 
	{
		l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
		m_targetFile = l_optArgUtf8;
	}

	else
		m_targetFile = "";

	if ( optind < p_argc )
	{
		l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
		throw UnexpectedArgException( l_optArgUtf8, __FILE__, __LINE__ );
	}
} // initialize

void Compiler::compile()
{
	dfwc_compile( m_sourceFile.c_str(), m_targetFile.c_str() );

	if ( dfwc_getErrorFlag() )
		throw runtime_error( dfwc_getErrorWhat() );
} // compile

void Compiler::showUsage()
{
		  /* 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
	cout << COPYRIGHT << endl;
	cout << "Word Processing Compiler " << VERSION << ". Usage:" << endl;
	cout << "word-processing-compiler [options] <source file> <target file>" << endl;
	cout << "   Compiles a word processing file to a compiled template file." << endl;
	cout << endl;
	cout << "   <source file>" << endl;
	cout << "   .docx Microsoft Word file." << endl;
	cout << endl;
	cout << "   <target file>" << endl;
	cout << "   .dfw DocxFactory compiled template file." << endl;
	cout << endl;
	cout << "  -T <temp dir>" << endl;
	cout << "   Set a directory for saving temporary files." << endl;
	cout << "   If no directory is set the operating system temp dir is used." << endl;
	cout << endl;
	cout << "  -help, -h" << endl;
	cout << "   Display help." << endl;
} // showUsage



bool Compiler::isHelp()
{
	return m_help;
} // isHelp
