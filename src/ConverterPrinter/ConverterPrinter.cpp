
#include "DocxFactory/ConverterPrinter/ConverterPrinter.h"
#include "DocxFactory/ConverterPrinter/OpenOfficeUtil.h"

#if defined( _WIN32 ) || defined( _WIN64 )

#include "DocxFactory/ConverterPrinter/MsWordUtil.h"
#include "DocxFactory/ConverterPrinter/MsExcelUtil.h"

#endif

#include "DocxFactory/ConverterPrinter/OdfConverterUtil.h"
#include "DocxFactory/ConverterPrinter/ConversionNotSupportedException.h"
#include "DocxFactory/ConverterPrinter/PrintingNotSupportedException.h"

#include "DocxFactory/os/OsFunc.h"
#include "DocxFactory/os/FileNotFoundException.h"
#include "DocxFactory/os/DirNotFoundException.h"

#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



// static data members definition

bool ConverterPrinter::m_updateToc = true;

void ConverterPrinter::setUpdateToc( bool p_update )
{
	m_updateToc = p_update;
} // setUpdateToc

bool ConverterPrinter::getUpdateToc()
{
	return m_updateToc;
} // getUpdateToc



ConverterPrinter& ConverterPrinter::getInstance()
{
	static ConverterPrinter s_instance;
	return s_instance;
} // getInstance

ConverterPrinter::ConverterPrinter()
{
	initializeUtils();
} // c'tor

ConverterPrinter::~ConverterPrinter()
{
	list<ConverterPrinterUtil*>::iterator l_utilIterator;
	FOR_EACH( l_utilIterator, &m_utils )
	{
		delete *l_utilIterator;
	}
} // d'tor



void ConverterPrinter::saveAs( const string& p_importFile, const string& p_exportFile )
{
	list<ConverterPrinterUtil*>::iterator	l_utilIterator;
	ConverterPrinterUtil*					l_util;
	ConverterPrinterUtil*					l_bestUtil;

	string	l_importFile;
	string	l_importExt;
	string	l_exportFile;
	string	l_exportExt;
	string	l_exportDir;

	double	l_bestGrade;
	double	l_grade;

	l_importFile	= OsFunc::getFullPath( p_importFile );

	if ( !OsFunc::isFileExists( l_importFile ) )
		throw FileNotFoundException( l_importFile, __FILE__, __LINE__ );

	l_exportFile	= OsFunc::getFullPath( p_exportFile );
	l_exportDir		= OsFunc::getSubPath( l_exportFile, OsFunc::SUBPATH_DIR, OsFunc::SUBPATH_DIR );

	if ( !OsFunc::isDirExists( l_exportDir ) )
		throw DirNotFoundException( l_exportDir, __FILE__, __LINE__ );



	l_importExt = OsFunc::getSubPath( l_importFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );
	l_exportExt = OsFunc::getSubPath( l_exportFile, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );

	l_bestUtil	= NULL;
	l_bestGrade	= -1.0f;

	FOR_EACH( l_utilIterator, &m_utils )
	{
		l_util = *l_utilIterator;
		l_grade = l_util ->getConversionGrade( l_importExt, l_exportExt );
		if ( l_grade > 0.0f )
		{
			if( l_bestGrade < 0.0f
			 || l_bestGrade > l_grade )
			{
				l_bestUtil	= l_util;
				l_bestGrade	= l_grade;
			}
		}
	}

	if ( !l_bestUtil )
		throw ConversionNotSupportedException( l_importExt, l_exportExt, __FILE__, __LINE__ );

	l_bestUtil ->saveAs( l_importFile, l_exportFile );
} // saveAs

void ConverterPrinter::print( const string& p_fileName, const string& p_printerName, int p_copyCnt )
{
	list<ConverterPrinterUtil*>::iterator	l_utilIterator;
	ConverterPrinterUtil*					l_util;
	ConverterPrinterUtil*					l_bestUtil;

	string	l_fileName;
	string	l_ext;

	double	l_bestGrade;
	double	l_grade;

	l_fileName	= OsFunc::getFullPath( p_fileName );
	l_ext		= OsFunc::getSubPath( l_fileName, OsFunc::SUBPATH_EXT, OsFunc::SUBPATH_EXT );

	if( !OsFunc::isFileExists( l_fileName ) )
		throw FileNotFoundException( l_fileName, __FILE__, __LINE__ );

	if( p_copyCnt < 0 )
		p_copyCnt = 0;



	l_bestUtil	= NULL;
	l_bestGrade = -1.0f;

	FOR_EACH( l_utilIterator, &m_utils )
	{
		l_util	= *l_utilIterator;
		l_grade	= l_util ->getPrintingGrade( l_ext );
		if ( l_grade > 0.0f )
		{
			if( l_bestGrade < 0.0f
			 || l_bestGrade > l_grade )
			{
				l_bestUtil	= l_util;
				l_bestGrade	= l_grade;
			}
		}
	}

	if ( !l_bestUtil )
		throw PrintingNotSupportedException( l_fileName, __FILE__, __LINE__ );

	l_bestUtil ->print( l_fileName, p_printerName, p_copyCnt );
} // print



void ConverterPrinter::initializeUtils()
{
	ConverterPrinterUtil*	l_util			= NULL;
	OdfConverterUtil*		l_odfConverter	= NULL;

#if defined( _WIN32 ) || defined( _WIN64 )

	try
	{
		l_util = new MsWordUtil();
		m_utils.push_back( l_util );
	}

	catch ( ... )
	{
	}

	try
	{
		l_util = new MsExcelUtil();
		m_utils.push_back( l_util );
	}

	catch ( ... )
	{
	}

#endif

	try
	{
		l_odfConverter = new OdfConverterUtil();
		m_utils.push_back( l_odfConverter );
	}

	catch ( ... )
	{
	}

	try
	{
		l_util = new OpenOfficeUtil( l_odfConverter );
		m_utils.push_back( l_util );
	}

	catch ( ... )
	{
	}
} // initializeUtils
