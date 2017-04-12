// $Log: cregexp.C,v $
// Revision 1.1  2005/12/05 02:20:09  jlli
// no message
//
// Revision 1.1.1.1  2004/10/19 08:26:04  cvs
//
//
// Revision 1.1.1.1  2004/10/17 14:08:08  cvs
//
//
// Revision 1.2  2004/05/28 12:07:17  yl
// Re-form again with indent.
//
// Revision 1.1  2004/05/24 13:36:56  yl
// Created. For regular-expression.
//

#include <iostream>
#include "cregexp.h"

using namespace std;

//########################################################################################

namespace xframe
{

//########################################################################################

CRegExp::CRegExp( const std::string & rule ) throw( bad_rule ):m_rule( rule ), parsed( false )
{
	if ( rule.empty() )
		throw bad_rule();
}

//########################################################################################

void CRegExp::initialize( const std::string & rule ) throw( bad_rule )
{
	if ( rule.empty() )
		throw bad_rule();

	m_rule = rule;
}

//########################################################################################

void CRegExp::parse() throw( not_initialized, bad_rule )
{
	// generate the m_element_list from m_rule.
	if ( m_rule.empty() )
		throw not_initialized();

	try
	{
		std::string::const_iterator beg = m_rule.begin();
		root_element.parse( beg, m_rule.end() );
		if ( m_rule.end() != beg )
			throw bad_rule();
	}
	catch( CRegExpEleParser::parse_error & )
	{
		throw bad_rule();
	}
}

//########################################################################################

bool CRegExp::match( const std::string & object ) throw( not_initialized, bad_rule )
{
	// parse the 'rule' just before first match
	if ( !parsed )
	{
		parse();
		parsed = true;
	}

	return root_element.match( object );
}

//########################################################################################

}

//########################################################################################
