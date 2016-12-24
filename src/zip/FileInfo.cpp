
#include "DocxFactory/zip/FileInfo.h"

using namespace DocxFactory;
using namespace std;



FileInfo::FileInfo( unz_file_info p_unzFileInfo )
{
	m_unzFileInfo = p_unzFileInfo;

	m_zipFileInfo.external_fa		= m_unzFileInfo.external_fa;
	m_zipFileInfo.internal_fa		= m_unzFileInfo.internal_fa;
	m_zipFileInfo.dosDate			= m_unzFileInfo.dosDate;

	m_zipFileInfo.tmz_date.tm_hour	= m_unzFileInfo.tmu_date.tm_hour;
	m_zipFileInfo.tmz_date.tm_mday	= m_unzFileInfo.tmu_date.tm_mday;
	m_zipFileInfo.tmz_date.tm_min	= m_unzFileInfo.tmu_date.tm_min;
	m_zipFileInfo.tmz_date.tm_mon	= m_unzFileInfo.tmu_date.tm_mon;
	m_zipFileInfo.tmz_date.tm_sec	= m_unzFileInfo.tmu_date.tm_sec;
	m_zipFileInfo.tmz_date.tm_year	= m_unzFileInfo.tmu_date.tm_year;
} // c'tor

FileInfo::~FileInfo()
{

} // d'tor



const unz_file_info* FileInfo::getUnzFileInfo() const
{
	return &m_unzFileInfo;
} // getUnzFileInfo

const zip_fileinfo* FileInfo::getZipFileInfo() const
{
	return &m_zipFileInfo;
} // getZipFileInfo
