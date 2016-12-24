
#ifndef __DOCXFACTORY_OPC_IMAGE_FILE_H__
#define __DOCXFACTORY_OPC_IMAGE_FILE_H__

#include "DocxFactory/util/DocxFactoryDefs.h"

#include <string>



namespace DocxFactory
{
	using namespace std;

	class OpcPackage;
	class OpcPart;
	class OpcImageCache;
	class OpcImageFile
	{
	public:
		static const unsigned int m_emuPerInch	= 914400;
		static const unsigned int m_emuPerCm	= 360000;
		static const unsigned int m_emuPerPixel	= 9525;

		enum ImageSource
		{
			SOURCE_EXTERNAL_FILE,
			SOURCE_IMAGE_OBJECT
		};

		enum TargetMode
		{
			INTERNAL_MODE,
			EXTERNAL_MODE
		};

		OpcImageFile(
			OpcImageCache*	p_imageCache,
			const string&	p_sourceFullPath,
			const string&	p_fileExt );

		virtual ~OpcImageFile();

		void save( const string& p_partFullPath );

		OpcPackage*		getPackage() const;
		OpcImageCache*	getImageCache() const;
		string			getSourceFullPath()	const;
		string			getExt() const;
		bool			isConverted() const;

		unsigned int	getEmuWidth() const;
		unsigned int	getEmuHeight() const;

	protected:

	private:
		OpcImageFile( const OpcImageFile& p_other );
		OpcImageFile operator = ( const OpcImageFile& p_other );

		void loadImage( bool p_convert, const string& p_convertTo = "" );

		OpcPackage*		m_package;
		OpcImageCache*	m_imageCache;
		string			m_sourceFullPath;
		string			m_ext;
		bool			m_converted;

		// the part full path and *not* the object pointer is saved
		// because the part maybe deleted in package manipulations but the image files
		// in the image cache are never deleted and could point to a deleted/non-existing part
		// so by saving the key it loosely couples the image file and part.

		ImageSource		m_imageSource;
		byte*			m_imageBuf;
		size_t			m_imageBufSize;

		unsigned int	m_emuWidth;
		unsigned int	m_emuHeight;

	};
};

#endif
