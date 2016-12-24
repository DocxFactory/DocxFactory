
#include "DocxFactory/parser/ParserFunc.h"
#include "DocxFactory/parser/UnmatchedQuoteException.h"
#include "DocxFactory/parser/UnclosedBracketException.h"
#include "DocxFactory/parser/UnexpectedBracketException.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

using namespace DocxFactory;
using namespace std;



vector<string> ParserFunc::parseWordList( const string& p_str, SpaceHandle p_space, bool p_drill ) 
{
	vector<string>	l_wordList;
	string			l_word;
	string			l_separator;

	char			l_quote;
	unsigned short	l_openCmmt;
	unsigned short	l_openBracket;
	unsigned short	l_openBracketSquare;
	unsigned short	l_openBracketCurly;

	size_t			l_len = p_str.length();
	size_t			l_pos = 0;
	string			l_str;
	char			l_ch;

	vector<string>	l_result;
	size_t			i;

	while ( l_pos < l_len )
	{
		l_ch = p_str[ l_pos ];

		if ( l_ch == '/' && l_pos + 1 < l_len && p_str[ l_pos + 1 ] == '*' && l_word.empty() )
		{
			l_openCmmt	= 1;
			l_pos		= l_pos + 2;

			while ( l_pos < l_len && l_openCmmt > 0 )
			{
				l_ch = p_str[ l_pos ];

				if ( l_ch == '/' && l_pos + 1 < l_len && p_str[ l_pos + 1 ] == '*' )
				{
					l_openCmmt++;
					l_pos += 2;
				}

				else if ( l_ch == '*' && l_pos + 1 < l_len && p_str[ l_pos + 1 ] == '/' )
				{
					l_openCmmt--;
					l_pos += 2;
				}

				else
					l_pos += 1;
			}
		}

		else if ( strchr( " \n\r\t*/,:;=<>!|\"'()[]{}", l_ch )  

			|| ( l_ch == '+' || l_ch == '-' || l_ch == '.' ) && ( l_pos + 1 >= l_len || !IS_DIGIT( p_str[ l_pos + 1 ] ) ) )

		{
			if ( l_ch == ' ' || l_ch == '\n' || l_ch == '\r' || l_ch == '\t' )
			{
				switch ( p_space )
				{
					case SPACE_REMOVE	: l_separator = "";		break;
					case SPACE_TRIM		: l_separator = " ";	break;
					case SPACE_KEEP		: l_separator = l_ch;	break;
				}

				l_pos += 1;

				while ( l_pos < l_len )
				{
					l_ch = p_str[ l_pos ];

					if ( l_ch !=  ' ' && l_ch != '\n' && l_ch != '\r' && l_ch != '\t' )
						break;

					if ( p_space == SPACE_KEEP )
						l_separator = l_separator + l_ch;

					l_pos += 1;
				}
			}
		
			else if ( strchr( "+-.*/,:;=<>!|", l_ch ) )
			{
				l_separator = l_ch;
				l_pos		+= 1;
			}

			else if ( l_ch == '"' || l_ch == '\'' )
			{
				l_quote		= l_ch;
				l_separator	= l_ch;
				l_pos		+= 1;

				while ( true )
				{
					if ( l_pos >= l_len )
						throw UnmatchedQuoteException( p_str, __FILE__, __LINE__);

					l_ch = p_str[ l_pos ];

					if ( l_ch == l_quote )
					{
						if ( l_pos + 1 < l_len && p_str[ l_pos + 1 ] == l_quote ) // escaped quote inside the quote e.g. " 15"" "
						{
							l_separator	+= p_str.substr( l_pos, 2 );
							l_pos		+= 2;
						}

						else
						{
							l_separator += l_ch;
							l_pos		+= 1;

							break;
						}
					}

					else if ( l_ch == '\\' )
					{
						l_separator  += p_str.substr ( l_pos, 2 );
						l_pos        += 2;
					}

					else
					{
						l_separator	+= l_ch;
						l_pos		+= 1;
					}
				}
			}

			else if ( l_ch == '(' || l_ch == '[' || l_ch == '{' ) 
			{
				l_openBracket		= 0;
				l_openBracketCurly	= 0;
				l_openBracketSquare	= 0;

				l_separator = "";

				while ( true )
				{
					if ( strchr( "(){}[]", l_ch ) )
					{
						l_separator += l_ch;
						l_pos       += 1;

						switch ( l_ch )
						{
						case '(':	l_openBracket++;		break;
						case ')':	l_openBracket--;		break;
						case '[':	l_openBracketSquare++;	break;
						case ']':	l_openBracketSquare--;	break;
						case '{':	l_openBracketCurly++;	break;
						case '}':	l_openBracketCurly--;	break;
						}

						if ( l_openBracket < 0 || l_openBracketSquare < 0 || l_openBracketCurly < 0 )
							throw UnexpectedBracketException( p_str, __FILE__, __LINE__ );
					
						if ( l_openBracket == 0 && l_openBracketSquare == 0 && l_openBracketCurly == 0 )
							break;				
					}

					else if ( l_ch == '"' || l_ch == '\'' )
					{
						l_quote		= l_ch;
						l_separator	+= l_ch;
						l_pos       += 1;

						while ( true )
						{
							if ( l_pos >= l_len )
								throw UnmatchedQuoteException( p_str, __FILE__, __LINE__ );
							
							l_ch = p_str[ l_pos ];

							if ( l_ch == l_quote )
							{
								if ( l_pos + 1 < l_len && p_str[ l_pos + 1 ] == l_quote )
								{
									l_separator  += p_str.substr ( l_pos, 2 );
									l_pos        += 2;
								}

								else 
								{
									l_separator	+= l_ch;
									l_pos		+= 1;

									break;
								}
							}

							else if ( l_ch == '\\' )
							{
								l_separator += p_str.substr( l_pos, 2 );
								l_pos       += 2;
							}

							else
							{
								l_separator  += l_ch;
								l_pos        += 1;
							}
						}
					}

					else if ( l_ch == '\\' )
					{
						l_separator  += p_str.substr( l_pos, 2 );
						l_pos        += 2;
					}

					else
					{
						l_separator  += l_ch;
						l_pos        += 1;
					}

					if ( l_pos >= l_len ) 
						break;

					l_ch = p_str[ l_pos ];
				}
				
				if ( l_openBracket > 0 || l_openBracketSquare > 0 || l_openBracketCurly > 0 )
					throw UnclosedBracketException( p_str, __FILE__, __LINE__ );
			}

			if ( !l_word.empty() )
				l_wordList.push_back( l_word );

			if ( !l_separator.empty() )
			{
				if ( p_drill 
				  && l_separator.length() >= 3
				  && ( l_separator[0] == '(' || l_separator[0] == '{' || l_separator[0] == '[' ) )
				{
					l_wordList.push_back( l_separator.substr( 0, 1 ) );

					l_result = parseWordList( l_separator.substr( 1, l_separator.length() - 2 ), p_space, p_drill );	
					
					for ( i = 0; i < l_result.size(); ++i )	
					{
						l_wordList.push_back( l_result.at(i) );
					}
					
					l_result.clear();

					l_wordList.push_back( l_separator.substr( l_separator.length() - 1, 1 ) ); 
				}

				else
					l_wordList.push_back( l_separator );  		
			}
			l_word		= "";
			l_separator	= "";
		}

		else if ( l_ch == '\\' )
		{
			l_word	+= p_str.substr( l_pos, 2 );
			l_pos	+= l_pos + 2;
		}

		else
		{
			l_word	+= l_ch;
			l_pos	+= 1;
		}	
	}

	if ( !l_word.empty() )
		l_wordList.push_back( l_word ); 

	return l_wordList; 
}

string ParserFunc::unquote( const string& p_str )
{
	string	l_wordList;
	string	l_str;
	char	l_quote;
	char	l_ch;

	size_t	l_len;
	size_t	l_pos;

	l_str = StrFunc::trim( p_str );

	if ( l_str.empty() )
		return "";

	if ( l_str[0] != '"' && l_str[0] != '\'' )
		return l_str;

	l_quote	= l_str[0];
	l_len	= l_str.length();
	l_pos   = 1;

	while ( l_pos < l_len )
	{
		l_ch = l_str[ l_pos ];

		if ( l_ch == l_quote )
		{
			if ( l_pos + 1 < l_len && l_str[ l_pos + 1 ] == l_quote )
			{
				l_wordList	+= l_ch;
				l_pos		+= 2;
			}

			else
			{
				l_pos		+= 1;
				break;
			}
		}

		else if ( l_ch == '\\' )
		{
			if ( l_pos + 1 < l_len )
				l_wordList += l_str[ l_pos + 1 ];

			l_pos += 2;
		}

		else
		{
			l_wordList	+= l_ch;
			l_pos		+= 1;
		}
	}

	return l_wordList;
} // unquote
