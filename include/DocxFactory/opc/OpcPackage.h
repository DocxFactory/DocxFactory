
#ifndef __DOCXFACTORY_OPC_PACKAGE_H__
#define __DOCXFACTORY_OPC_PACKAGE_H__

#include "xercesc/util/XercesDefs.hpp"

#include <map>
#include <string>



XERCES_CPP_NAMESPACE_BEGIN

class DOMDocument;
class DOMLSParser;

XERCES_CPP_NAMESPACE_END

namespace DocxFactory
{
	using namespace std;

	class ZipFile;
	class UnzipFile;

	class OpcPart;
	class OpcRelationships;
	class OpcContentTypes;
	class OpcImageCache;
	class OpcImageFile;
	class OpcPackage
	{
	public:
		OpcPackage();
		virtual ~OpcPackage();

		virtual void load( const string& p_fileName );
		virtual void save( const string& p_fileName );

		OpcPart*	insertPart			( const string& p_fullPath );
		OpcPart*	insertImageFilePart	( OpcImageFile* p_imageFile );
		void		deletePart			( OpcPart* p_part );

		const map<string, OpcPart*>*	getPartsByFullPath() const;
		OpcRelationships*				getRelationships() const;
		OpcContentTypes*				getContentTypes() const;
		OpcImageCache*					getImageCache() const;
		UnzipFile*						getUnzipFile() const;
		ZipFile*						getZipFile() const;
		xercesc::DOMLSParser*			getDomParser() const;

	protected:

	private:
		OpcPackage( const OpcPackage& p_other );
		OpcPackage& operator = ( const OpcPackage& p_other );

		void loadParts();
		void saveParts();

		map<string, OpcPart*>			m_partsByFullPath;
		map<OpcImageFile*, OpcPart*>	m_partsByImageFile;
		OpcRelationships*				m_relationships;
		OpcContentTypes*				m_contentTypes;
		OpcImageCache*					m_imageCache;

		UnzipFile*						m_unzipFile;
		ZipFile*						m_zipFile;

		// docs created by the parser are tied to the scope of the parser.
		// the package has a parser making all docs parsed in the package scoped to the package.
		xercesc::DOMLSParser*			m_domParser;

	};
};

#endif
