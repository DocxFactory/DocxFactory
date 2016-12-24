
#include "DocxFactory/WordProcessingMerger/dfw.h"

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
#include "DocxFactory/str/StrEntries.h"

#include "DocxFactory/util/DocxFactoryDefs.h"
#include "DocxFactory/util/DocxFactoryException.h"
#include "DocxFactory/util/tchar.h"

#include "boost/scoped_array.hpp"

#include <cstdlib>
#include <stdexcept>
#include <exception>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>

using namespace DocxFactory;
using namespace std;



class Merger
{
public:
	static void	initialize( int p_argc, TCHAR* p_argv[] );
	static void	load();
	static void	merge();
	static void	save();
	static void	print();
	static void list();
	static void	showUsage();

	static bool	isHelp();
	static bool isList();
	static bool	isPrint();

private:
	Merger( const Merger& p_other );
	Merger &operator = ( const Merger &p_other );

	static string			m_templateFile;
	static string			m_dataFile;
	static string			m_saveFile;

	static string			m_printer;
	static unsigned short	m_copyCnt;
	static bool				m_stdin;
	static bool				m_help;
	static bool				m_list;
};



// tmain and tchar macros are defined in util/tchar.h
// and will compile under unix/linux as well.
int _tmain( int argc, TCHAR* argv[] )
{
	try
	{
		Merger::initialize( argc, argv );

		if( Merger::isHelp() )
		{
			Merger::showUsage();
			return EXIT_SUCCESS;
		}

		if( Merger::isList() )
		{
			Merger::load();
			Merger::list();
			return EXIT_SUCCESS;
		}

		time_t l_start = clock();

		Merger::load();
		Merger::merge();

		if( Merger::isPrint() )
			Merger::print();
		else
			Merger::save();

		cout<< "Completed (in "
			<< LocaleFunc::numToStr( ( double ) ( clock() - l_start ) / ( double ) CLOCKS_PER_SEC, 3, 0, true )
			<< " seconds)."
			<< endl;

		return EXIT_SUCCESS;
	}

	catch ( GetOptException )
	{
		Merger::showUsage();
		exit( EXIT_FAILURE );
	}

	catch ( const ConsoleUsageException& e )
	{
		cout << e.what() << endl;
		cout << endl;

		Merger::showUsage();
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

string			Merger::m_templateFile	= "";
string			Merger::m_dataFile		= "";
string			Merger::m_saveFile		= "";

string			Merger::m_printer		= "";
unsigned short	Merger::m_copyCnt		= 0;
bool			Merger::m_stdin			= false;
bool			Merger::m_help			= false;
bool			Merger::m_list			= false;



// public

void Merger::initialize( int p_argc, TCHAR* p_argv[] )
{
	struct option l_optList[] =
	{
		{ _T( "stdin" ),		no_argument,		0, _T( 's' ) },
		{ _T( "fr" ),			required_argument,	0, _T( 'f' ) },
		{ _T( "th" ),			required_argument,	0, _T( 't' ) },
		{ _T( "d" ),			required_argument,	0, _T( 'd' ) },
		{ _T( "firstweekday" ),	required_argument,	0, _T( 'i' ) },
		{ _T( "yy" ),			required_argument,	0, _T( 'y' ) },
		{ _T( "w" ),			required_argument,	0, _T( 'w' ) },
		{ _T( "m" ),			required_argument,	0, _T( 'm' ) },
		{ _T( "p" ),			required_argument,	0, _T( 'p' ) },
		{ _T( "copies" ),		required_argument,	0, _T( 'c' ) },
		{ _T( "T" ),			required_argument,	0, _T( 'T' ) },
		{ _T( "list" ),			no_argument,		0, _T( 'l' ) },
		{ _T( "help" ),			no_argument,		0, _T( 'h' ) },
		{ _T( "h" ),			no_argument,		0, _T( 'h' ) },
		{ 0, 0, 0, 0 }
	};

	string	l_optArgUtf8;
	int		l_optIdx = 0;
	int		l_opt;
	string	l_file;

	opterr = 1;

	while ( ( l_opt = getopt_long_only( p_argc, p_argv, _T( "" ), l_optList, &l_optIdx) ) != -1 )
	{
		if ( optarg )
			l_optArgUtf8 = LocaleFunc::convertToUtf8( optarg );
		else
			l_optArgUtf8 = "";

		switch ( l_opt )
		{
		case _T( 's' ):

			m_stdin = true;
			break;

		case _T( 'f' ):

			if ( l_optArgUtf8.length() != 1 )
				throw InvalidOptException( "-fr", l_optArgUtf8, __FILE__, __LINE__ );

			dfw_setNumFracSep( l_optArgUtf8[0] );

			if ( dfw_getErrorFlag() )
				throw runtime_error( dfw_getErrorWhat() );

			break;

		case _T( 't' ):

			if ( l_optArgUtf8.length() != 1 )
				throw InvalidOptException( "-th", l_optArgUtf8, __FILE__, __LINE__ );

			dfw_setNumThSep( l_optArgUtf8[0] );

			if ( dfw_getErrorFlag() )
				throw runtime_error( dfw_getErrorWhat() );

			break;

		case _T( 'd' ):

			try
			{
				LocaleFunc::setDateFormat( l_optArgUtf8 );
			}

			catch ( ... )
			{
				throw InvalidOptException( "-d", l_optArgUtf8, __FILE__, __LINE__ );
			}

			break;

		case _T( 'i' ):

			if ( StrFunc::lc( l_optArgUtf8 ) == "sunday" || l_optArgUtf8 == "1" )
			{
				dfw_setFirstWeekDay(1);

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else 
			if ( StrFunc::lc( l_optArgUtf8 ) == "monday" || l_optArgUtf8 == "2" )
			{
				dfw_setFirstWeekDay(2);

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else
				throw InvalidOptException( "-firstweekday", l_optArgUtf8, __FILE__, __LINE__ );

			break;

		case _T( 'w' ):
		{
			StrEntries	l_weekDayNames( l_optArgUtf8 );

			if ( l_weekDayNames.getNumEntries() == 7 )
			{
				dfw_setWeekDayNames7(
					l_weekDayNames.getEntry(0).c_str(),
					l_weekDayNames.getEntry(1).c_str(),
					l_weekDayNames.getEntry(2).c_str(),
					l_weekDayNames.getEntry(3).c_str(),
					l_weekDayNames.getEntry(4).c_str(),
					l_weekDayNames.getEntry(5).c_str(),
					l_weekDayNames.getEntry(6).c_str() );

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else
			if ( l_weekDayNames.getNumEntries() == 14 )
			{
				dfw_setWeekDayNames14(
					l_weekDayNames.getEntry(0).c_str(),
					l_weekDayNames.getEntry(1).c_str(),
					l_weekDayNames.getEntry(2).c_str(),
					l_weekDayNames.getEntry(3).c_str(),
					l_weekDayNames.getEntry(4).c_str(),
					l_weekDayNames.getEntry(5).c_str(),
					l_weekDayNames.getEntry(6).c_str(),

					l_weekDayNames.getEntry(7).c_str(),
					l_weekDayNames.getEntry(8).c_str(),
					l_weekDayNames.getEntry(9).c_str(),
					l_weekDayNames.getEntry(10).c_str(),
					l_weekDayNames.getEntry(11).c_str(),
					l_weekDayNames.getEntry(12).c_str(),
					l_weekDayNames.getEntry(13).c_str() );

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else
				throw InvalidOptException( "-w", l_optArgUtf8, __FILE__, __LINE__ );

			break;
		}

		case _T( 'm' ):
		{
			StrEntries l_monthNames( l_optArgUtf8 );

			if ( l_monthNames.getNumEntries() == 12 )
			{
				dfw_setMonthNames12(
					l_monthNames.getEntry(0).c_str(),
					l_monthNames.getEntry(1).c_str(),
					l_monthNames.getEntry(2).c_str(),
					l_monthNames.getEntry(3).c_str(),
					l_monthNames.getEntry(4).c_str(),
					l_monthNames.getEntry(5).c_str(),
					l_monthNames.getEntry(6).c_str(),
					l_monthNames.getEntry(7).c_str(),
					l_monthNames.getEntry(8).c_str(),
					l_monthNames.getEntry(9).c_str(),
					l_monthNames.getEntry(10).c_str(),
					l_monthNames.getEntry(11).c_str() );

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else
			if ( l_monthNames.getNumEntries() == 24 )
			{
				dfw_setMonthNames24(
					l_monthNames.getEntry(0).c_str(),
					l_monthNames.getEntry(1).c_str(),
					l_monthNames.getEntry(2).c_str(),
					l_monthNames.getEntry(3).c_str(),
					l_monthNames.getEntry(4).c_str(),
					l_monthNames.getEntry(5).c_str(),
					l_monthNames.getEntry(6).c_str(),
					l_monthNames.getEntry(7).c_str(),
					l_monthNames.getEntry(8).c_str(),
					l_monthNames.getEntry(9).c_str(),
					l_monthNames.getEntry(10).c_str(),
					l_monthNames.getEntry(11).c_str(),

					l_monthNames.getEntry(12).c_str(),
					l_monthNames.getEntry(13).c_str(),
					l_monthNames.getEntry(14).c_str(),
					l_monthNames.getEntry(15).c_str(),
					l_monthNames.getEntry(16).c_str(),
					l_monthNames.getEntry(17).c_str(),
					l_monthNames.getEntry(18).c_str(),
					l_monthNames.getEntry(19).c_str(),
					l_monthNames.getEntry(20).c_str(),
					l_monthNames.getEntry(21).c_str(),
					l_monthNames.getEntry(22).c_str(),
					l_monthNames.getEntry(23).c_str() );

				if ( dfw_getErrorFlag() )
					throw runtime_error( dfw_getErrorWhat() );
			}

			else
				throw InvalidOptException( "-m", l_optArgUtf8, __FILE__, __LINE__ );

			break;
		}

		case _T( 'p' ):

			m_printer = l_optArgUtf8;
			break;

		case _T( 'c' ):

			if ( !StrFunc::isNum( l_optArgUtf8 ) )
				throw InvalidOptException( "-copies", l_optArgUtf8, __FILE__, __LINE__ );

			m_copyCnt = StrFunc::strToInt<unsigned short>( l_optArgUtf8 );

			if ( m_copyCnt <= 0 )
				throw InvalidOptException( "-copies", l_optArgUtf8, __FILE__, __LINE__ );

			break;

		case _T( 'T' ):

			dfw_setTempDir( l_optArgUtf8.c_str() );

			if ( dfw_getErrorFlag() )
				throw runtime_error( dfw_getErrorWhat() );

			break;

		case _T( 'l' ):

			m_list = true;
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
		throw MissingArgException( "<template file>", __FILE__, __LINE__ );

	l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
	m_templateFile = l_optArgUtf8;



	if ( !m_list )
	{
		if ( !m_stdin )
		{
			if ( optind >= p_argc )
				throw MissingArgException( "<data file>", __FILE__, __LINE__ );

			l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );

			m_dataFile = OsFunc::normalizePath( l_optArgUtf8 );
			m_dataFile = OsFunc::getFullPath( m_dataFile );

			/*
			if ( OsFunc::isRelativePath( m_dataFile ) )
			{
				l_file = OsFunc::normalizePath( m_templateFile );
				l_file = OsFunc::getFullPath( l_file );

				m_dataFile = OsFunc::normalizePath(
					  OsFunc::getSubPath( l_file, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_DIR ) + "/"
					+ m_dataFile );
			}
			*/

			if ( !OsFunc::isFileExists( m_dataFile ) )
				throw FileNotFoundException( m_dataFile, __FILE__, __LINE__ );
		}

		if ( m_printer.empty() )
		{
			if ( optind < p_argc ) 
			{
				l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
				m_saveFile = l_optArgUtf8;
			}
		}
	}

	if ( optind < p_argc )
	{
		l_optArgUtf8 = LocaleFunc::convertToUtf8( p_argv[ optind++ ] );
		throw UnexpectedArgException( l_optArgUtf8, __FILE__, __LINE__ );
	}
} // init

void Merger::load()
{
	dfw_load( m_templateFile.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // load

void Merger::merge()
{
	string l_data;
	string l_line;

	if ( m_stdin )
	{
		while ( !cin.eof() && !cin.fail() )
		{
			getline( cin, l_line );
			l_data += ( l_data.empty() ? "" : "\n" ) + l_line;
		}

	}

	else
	{
		size_t						l_bufSize;
		boost::scoped_array<byte>	l_buf( OsFunc::readFile( m_dataFile, l_bufSize ) );

		l_data = string( ( char* ) l_buf.get(), l_bufSize );
	}

	dfw_setCodePage( "UTF-8" );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );

	dfw_merge( l_data.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // merge

void Merger::save()
{
	dfw_save( m_saveFile.c_str() );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // save

void Merger::print()
{
	dfw_print( m_printer.c_str(), m_copyCnt );

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // print

void Merger::list()
{
	StrEntries	l_items( dfw_getItems() );
	string		l_item;
	string		l_parent;
	string		l_fields;

	size_t		len	= l_items.getNumEntries();
	size_t		pos;

		   /*12345678901234567890 12345678901234567890*/
	cout << "Item                 Parent               Fields" << endl;
	cout << "----                 ------               ------" << endl;

	for ( pos = 0; pos < len; pos++ )
	{
		l_item		= l_items.getEntry( pos );
		l_parent	= dfw_getItemParent( l_item.c_str() );
		l_fields	= dfw_getItemFields( l_item.c_str() );

		cout << StrFunc::padRight( l_item, 20 ) << " "
			 << StrFunc::padRight( l_parent, 20 ) << " "
			 << l_fields << endl;
	}

	dfw_close();

	if ( dfw_getErrorFlag() )
		throw runtime_error( dfw_getErrorWhat() );
} // list

void Merger::showUsage()
{
		  /* 12345678901234567890123456789012345678901234567890123456789012345678901234567890 */
	cout << COPYRIGHT << endl;
	cout << "Word Processing Merger " << VERSION << ". Usage:" << endl;
	cout << "word-processing-merger [options] <template file> <data file>" << endl;
	cout << "   [<save file>|-p <printer>]" << endl;
	cout << "   Generates a word processing file by merging a template and a data file." << endl;
	cout << endl;
	cout << "   <template file>" << endl;
	cout << "   .dfw DocxFactory compiled template file." << endl;
	cout << endl;
	cout << "   <data file>" << endl;
	cout << "   .xml or .json data file to merge with the template." << endl;
	cout << "   The merger automatically maps items and fields names" << endl;
	cout << "   to XML node or JSON member names." << endl;
	cout << endl;
	cout << "   <save file>" << endl;
	cout << "   .docx file to save the merged document." << endl;
	cout << "   If a .pdf, .html, .odt etc. file is passed," << endl;
	cout << "   a .docx file is first saved and then converted." << endl;
	cout << endl;
	cout << "  -stdin" << endl;
	cout << "   XML or JSON data to merge with the template" << endl;
	cout << "   is taken from stdin instead of a file." << endl;
	cout << endl;
	cout << "  -fr \"<numeric fractional separator>\"" << endl;
	cout << "   Numeric fractional separator e.g. \".\"." << endl;
	cout << endl;
	cout << "  -th \"<numeric thousand separator>\"" << endl;
	cout << "   Numeric thousand separator e.g. \",\"." << endl;
	cout << endl;
	cout << "  -d dmy|mdy|ymd" << endl;
	cout << "   Date format day, month and year order." << endl;
	cout << endl;
	cout << "  -yy 1950-1999" << endl;
	cout << "   Year offset for 2 digit year dates." << endl;
	cout << "   Years from  the 2 digit year offset are in 1900." << endl;
	cout << "   Years below the 2 digit year offset are in 2000." << endl;
	cout << "   For example:" << endl;
	cout << "     -yy 1950 date: 01/01/50 equals 01/01/1950" << endl;
	cout << "     -yy 1950 date: 01/01/20 equals 01/01/2020" << endl;
	cout << endl;
	cout << "  -firstweekday sunday|monday" << endl;
	cout << "   First day of the week. Used to calculate week number for dates." << endl;
	cout << endl;
	cout << "  -w \"<weekday names>\"" << endl;
	cout << "    7 entry comma separated list of full weekday names." << endl;
	cout << "      The short weekday names are taken from the full names first 3 letters." << endl; 
	cout << "   14 entry comma separated list of 7 full and 7 short weekday names" << endl;
	cout << "      can be used instead to enter the short weekday names explicitly." << endl;
	cout << endl;
	cout << "  -m \"<month names>\"" << endl;
	cout << "   12 entry comma separated list of full month names." << endl;
	cout << "      The short month names are taken from the full names first 3 letters." << endl; 
	cout << "   24 entry comma separated list of 12 full and 12 short month names" << endl;
	cout << "      can be used instead to enter the short month names explicitly." << endl;
	cout << endl;
	cout << "  -p <printer>" << endl;
	cout << "   Print document." << endl;
	cout << "   If this option is used, no file is saved and <save file> is not read." << endl;
	cout << endl;
	cout << "  -copies <n>" << endl;
	cout << "   Number of copies to print." << endl;
	cout << endl;
	cout << "  -T <temp dir>" << endl;
	cout << "   Set a directory for saving temporary files." << endl;
	cout << "   If no directory is set, the operating system temp dir is used." << endl;
	cout << endl;
	cout << "  -list" << endl;
	cout << "   Lists the items and fields in the template." << endl;
	cout << endl;
	cout << "  -help, -h" << endl;
	cout << "   Display help." << endl;
} // showUsage



bool Merger::isHelp()
{
	return m_help;
} // isHelp

bool Merger::isList()
{
	return m_list;
} // isList

bool Merger::isPrint()
{
	return ( !m_printer.empty() );
} // isHelp
