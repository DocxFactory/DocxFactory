
#include "WordProcessingCompiler.h"

#include "dfwc.h"

#include <stdexcept>

using namespace DocxFactory;
using namespace std;



WordProcessingCompiler& WordProcessingCompiler::getInstance()
{
	static WordProcessingCompiler s_instance;
	return s_instance;
} // getInstance

WordProcessingCompiler::WordProcessingCompiler()
{

} // c'tor

WordProcessingCompiler::~WordProcessingCompiler()
{

} // d'tor



void WordProcessingCompiler::compile( const string& p_srcFileName, const string& p_dstFileName )
{
	dfwc_compile( p_srcFileName.c_str(), p_dstFileName.c_str() );

	if ( dfwc_getErrorFlag() )
		throw runtime_error( dfwc_getErrorWhat() );
} // compile



// os functions
void WordProcessingCompiler::setTempDir()
{
	dfwc_setTempDir( "" );

	if ( dfwc_getErrorFlag() )
		throw runtime_error( dfwc_getErrorWhat() );
} // setTempDir()

void WordProcessingCompiler::setTempDir( const string& p_dir )
{
	dfwc_setTempDir( p_dir.c_str() );

	if ( dfwc_getErrorFlag() )
		throw runtime_error( dfwc_getErrorWhat() );
} // setTempDir

string WordProcessingCompiler::getWorkDir() const
{
	return dfwc_getWorkDir();
} // getWorkDir

string WordProcessingCompiler::getTempDir() const
{
	return dfwc_getTempDir();
} // getTempDir
