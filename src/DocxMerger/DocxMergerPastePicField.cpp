
#include "DocxFactory/DocxMerger/DocxMergerField.h"
#include "DocxFactory/DocxMerger/DocxMergerPastePicField.h"

using namespace DocxFactory;
using namespace std;



DocxMergerPastePicField::DocxMergerPastePicField(
	DocxMergerField*			p_field,
	OpcImageFile::TargetMode	p_targetMode,
	const string&				p_fileName,
	const string&				p_fileExt,
	const string&				p_fileUrl ) : DocxMergerPasteField( p_field)
{
	m_targetMode	= p_targetMode;
	m_fileName		= p_fileName;
	m_fileExt		= p_fileExt;
	m_fileUrl		= p_fileUrl;
} // c'tor

DocxMergerPastePicField::~DocxMergerPastePicField()
{

} // d'tor



OpcImageFile::TargetMode DocxMergerPastePicField::getTargetMode() const
{
	return m_targetMode;
} // getTargetMode

const string* DocxMergerPastePicField::getFileName() const
{
	return &m_fileName;
} // getFileName

const string* DocxMergerPastePicField::getFileExt() const
{
	return &m_fileExt;
} // getFileExt

const string* DocxMergerPastePicField::getFileUrl() const
{
	return &m_fileUrl;
} // getFileUrl
