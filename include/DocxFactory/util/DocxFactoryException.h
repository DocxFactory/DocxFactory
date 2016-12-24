
#ifndef __DOCXFACTORY_EXCEPTION_H__
#define __DOCXFACTORY_EXCEPTION_H__

#include <exception>
#include <string>



namespace DocxFactory
{
	using namespace std;

	class DocxFactoryException : public exception
	{
	public:
		virtual ~DocxFactoryException() throw();

		// what() must return char* like exception::what() that it's overriding
		virtual const char* what() const throw();

		// where() also returns char* to be consistent with what()
		virtual const char* where() const throw();

	protected:
		DocxFactoryException( const string& p_file, int p_line );

		string	m_what;
		string	m_where;
		string	m_file;
		int		m_line;

	private:

	};
};

#endif
