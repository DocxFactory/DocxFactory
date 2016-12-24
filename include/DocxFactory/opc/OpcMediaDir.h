
#ifndef __DOCXFACTORY_OPC_MEDIA_DIR_H__
#define __DOCXFACTORY_OPC_MEDIA_DIR_H__

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcImageFile;
	class OpcMediaDir
	{
	public:
		OpcMediaDir( OpcPackage* p_package );
		virtual ~OpcMediaDir();

		void load();

		OpcImageFile*				insertImageFile( const string& p_fileName );
		OpcPackage*					getPackage() const;
		map<string, OpcImageFile*>* getImageFileMap() const;

	protected:

	private:
		OpcMediaDir( const OpcMediaDir& p_other );
		OpcMediaDir operator = ( const OpcMediaDir& p_other );

		OpcPackage*					m_package;
		string						m_fullPath;
		map<string, OpcImageFile*>*	m_imageFileMap;
		unsigned int				m_imageFileSeq;

	};
};

#endif
