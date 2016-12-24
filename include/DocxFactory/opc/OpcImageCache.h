
#ifndef __DOCXFACTORY_OPC_IMAGE_CACHE_H__
#define __DOCXFACTORY_OPC_IMAGE_CACHE_H__

#include <map>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcImageFile;



	// OpcImageCache fulfills 3 purposes -
	// 1. when adding an image, if that image has already been added use that image.
	// 2. when adding an image convert to a supported format, if the image type is not supported.
	// 3. get the dpi and pixel widget/height.
	// all these features are required for inserting new images.
	// at the moment, these features are not required for old images (that were already in the package).
	// therefore OpcImageCache only supports inserting new images.

	class OpcImageCache
	{
	public:
		OpcImageCache( OpcPackage* p_package );
		virtual ~OpcImageCache();

		void load();
		OpcImageFile* insertImageFile( const string& p_srceFileName, const string& p_fileExt );

		OpcPackage*							getPackage() const;
		string								getFullPath() const;
		const map<string, OpcImageFile*>*	getImageFilesByFileName() const;
		unsigned int						nextSeq();

	protected:

	private:
		OpcImageCache( const OpcImageCache& p_other );
		OpcImageCache operator = ( const OpcImageCache& p_other );

		OpcPackage*					m_package;
		string						m_fullPath;
		map<string, OpcImageFile*>	m_imageFilesByFileName;
		unsigned int				m_imageFileSeq;
	};
};

#endif
