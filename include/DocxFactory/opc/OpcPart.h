
#ifndef __DOCXFACTORY_OPC_PART_H__
#define __DOCXFACTORY_OPC_PART_H__

#include "DocxFactory/opc/OpcImageFile.h"

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMDocument;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcRelationships;
	class OpcImageFile;
	class OpcPart
	{
	public:
		enum PartChangeStatus
		{
			PART_NOT_CHANGED,
			PART_CHANGED_IN_DOM,
			PART_CHANGED_IN_IMAGE_FILE,
			PART_CHANGED_IN_BUFFER,
			PART_SAVED
		};

		OpcPart( OpcPackage* p_package, const string& p_fullPath );
		virtual ~OpcPart();

		void loadDoc();
		void save();
		void updateRefCnt( int p_refCnt );

		void setDoc			( xercesc::DOMDocument* p_doc );
		void setImageFile	( OpcImageFile* p_imageFile );
		void setBuf			( const char* p_buf, size_t p_bufSize );
		void setChangeStatus( PartChangeStatus p_changeStatus );

		OpcImageFile* insertImageFile(
			OpcImageFile::TargetMode	p_targetMode,
			const string&				p_fileName,
			const string&				p_fileExt,
			const string&				p_fileUrl,
			string&						p_fileRId );

		OpcPackage*				getPackage() const;
		string					getFullPath() const;
		OpcRelationships*		getRelationships() const;

		xercesc::DOMDocument*	getDoc() const;
		OpcImageFile*			getImageFile() const;
		const char*				getBuf( size_t& p_bufSize ) const;
		PartChangeStatus		getChangeStatus() const;

	protected:

	private:
		OpcPart( const OpcPart& p_other );
		OpcPart operator = ( const OpcPart& p_other );

		void destroy();

		OpcPackage*				m_package;
		string					m_fullPath;
		OpcRelationships*		m_relationships;
		int						m_refCnt;

		xercesc::DOMDocument*	m_doc;
		OpcImageFile*			m_imageFile;
		const char*				m_buf;
		size_t					m_bufSize;
		PartChangeStatus		m_changeStatus;

	};
};

#endif
