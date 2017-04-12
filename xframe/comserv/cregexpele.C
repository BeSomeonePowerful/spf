/*********************************************************************
* Copyright (c)2005, by MT2
* All rights reserved.
* FileName：				cregexpele.C
* System：           		SoftSwitch
* SubSystem：	        	Common
* Author：					李静林
* Description：

* Last Modified:
	2005-12-05 v1.1 李静林 将list模版类替换为clist.h，用我们自己编写的CList代替
	2005-12-05 v1.2 李静林 将clear替换为clist定义的remove

// $Log: cregexpele.C,v $
// Revision 1.3  2005/12/07 03:00:20  jcdu
// no message
//
// Revision 1.2  2005/12/05 06:55:45  jlli
// *** empty log message ***
//
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
*********************************************************************/

#include <memory>

#include "cregexpele.h"

//########################################################################################

namespace xframe
{

//########################################################################################

CRegExpEleParser::general_char CRegExpEleParser::get_next_char( std::string::const_iterator & obj_b,
																std::string::const_iterator obj_e ) throw( get_char_failed )
{
	if ( obj_e == obj_b )
		throw get_char_failed();

	general_char ret;

	// match escape character
	if ( '\\' == ( *obj_b ) )
	{
		// no successor
		if ( obj_e == obj_b + 1 )
			throw get_char_failed();

		ret.escaped = true;
		obj_b++;
		ret.character = *obj_b;
		obj_b++;
	}
	else
	{
		ret.escaped = false;
		ret.character = *obj_b;
		obj_b++;
	}

	return ret;
}

//########################################################################################

CRegExpElement::MResult CRegExpEleExact::match_front( std::string::const_iterator & begin,
													  std::string::const_iterator end,
													  std::string::const_iterator target ) throw()
{
	int passed = 0;
	if ( ( target - begin <= m_modifier.maximum ) && ( end - begin >= m_modifier.minimum ) )
	{
		while ( ( ( target > begin ) || ( passed < m_modifier.minimum ) ) && ( passed < m_modifier.maximum ) )
		{
			if ( *begin == m_c )
			{
				begin++;
				passed++;
			}
			else
			{
				return certain_not;
			}
		}
		if ( ( passed == m_modifier.maximum ) && ( target > begin ) )
			return certain_not;
		else if ( ( passed == m_modifier.maximum ) && ( target == begin ) )
			return certain;
		else
			return un_certain;
	}
	else
	{
		return certain_not;
	}
}

//########################################################################################

CRegExpElement::MResult CRegExpEleAnyOne::match_front( std::string::const_iterator & begin,
													   std::string::const_iterator end,
													   std::string::const_iterator target ) throw()
{
	if ( target - begin == m_modifier.maximum )
	{
		begin = target;
		return certain;
	}
	else if ( ( target - begin < m_modifier.maximum ) && ( end - begin >= m_modifier.minimum ) )
	{
		if ( begin + m_modifier.minimum > target )
		{
			begin = begin + m_modifier.minimum;
		}
		else
		{
			begin = target;
		}
		return un_certain;
	}
	else
		return certain_not;
}

//########################################################################################

CRegExpEleGroup::~CRegExpEleGroup()
{
	for ( CList < CGroupItem * >::iterator it = m_componentList.begin(); it != m_componentList.end(); it++ )
	{
		delete( *it );
		( *it ) = 0;
	}
}

//########################################################################################

CRegExpElement::MResult CRegExpEleGroup::match_front( std::string::const_iterator & begin,
													  std::string::const_iterator end,
													  std::string::const_iterator target ) throw()
{
	int passed = 0;
	if ( ( target - begin <= m_modifier.maximum ) && ( end - begin >= m_modifier.minimum ) )
	{
		while ( ( ( target > begin ) || ( passed < m_modifier.minimum ) ) && ( passed < m_modifier.maximum ) )
		{
			if ( match_component( *begin ) )
			{
				begin++;
				passed++;
			}
			else
			{
				return certain_not;
			}
		}
		if ( ( passed == m_modifier.maximum ) && ( target > begin ) )
			return certain_not;
		else if ( ( passed == m_modifier.maximum ) && ( target == begin ) )
			return certain;
		else
			return un_certain;
	}
	else
	{
		return certain_not;
	}
}

//########################################################################################

bool CRegExpEleGroup::match_component( char object )
{
	bool match = false;
	CList < CGroupItem * >::iterator it = m_componentList.begin();
	for ( ; it != m_componentList.end(); it++ )
	{
		if ( ( *it )->match( object ) )
		{
			match = true;
			break;
		}
	}
	if ( m_reversed )
		match = !match;

	return match;
}

//########################################################################################

void CRegExpEleGroup::parse( std::string::const_iterator & begin,
							 std::string::const_iterator end ) throw( parse_error )
{
	general_char gc;
	std::string::const_iterator it = begin;
	bool finish = false;
	const general_char fin_char( ']', false );
	const general_char splite_char( '-', false );

	char b_char;

	enum status
	{ search_reverse_tag, reading_begin_char, search_spliter, reading_end_char };

	status s = search_reverse_tag;

	// match '['
	try
	{
		gc = get_next_char( it, end );
	}
	catch( get_char_failed & )
	{
		throw parse_error();
	}
	if ( !( general_char( '[', false ) == gc ) )
	{
		throw parse_error();
	}

	try
	{
		while ( ( !finish ) && ( end != begin ) )
		{
			// get next general character.
			try
			{
				gc = get_next_char( it, end );
			}
			catch( get_char_failed & )
			{
				throw parse_error();
			}

			if ( fin_char == gc )
			{
				finish = true;
				if ( search_spliter == s )
				{
					m_componentList.push_back( new CGroupItem( b_char, b_char ) );
				}
				break;
			}

			switch ( s )
			{
				case search_reverse_tag:
				{
					if ( general_char( '^', false ) == gc )
					{
						m_reversed = true;
						s = reading_begin_char;
					}
					else
					{
						b_char = gc.character;
						s = search_spliter;
					}
					break;
				}
				case reading_begin_char:
				{
					if ( splite_char == gc )
					{
						throw parse_error();
					}
					b_char = gc.character;
					s = search_spliter;
					break;
				}
				case search_spliter:
				{
					if ( splite_char == gc )
					{
						s = reading_end_char;
					}
					else
					{
						m_componentList.push_back( new CGroupItem( b_char, b_char ) );
						b_char = gc.character;
					}
					break;
				}
				case reading_end_char:
				{
					if ( splite_char == gc )
					{
						throw parse_error();
					}
					m_componentList.push_back( new CGroupItem( b_char, gc.character ) );
					s = reading_begin_char;
					break;
				}
			}
		}

		if ( ( !finish ) ||
			 ( ( s != search_spliter ) && ( s != reading_begin_char ) ) || ( m_componentList.empty() ) )
		{
			throw parse_error();
		}

		begin = it;
	}
	catch( parse_error & e )
	{
		for ( CList < CGroupItem * >::iterator it = m_componentList.begin(); it != m_componentList.end();
			  it++ )
		{
			delete( *it );
			( *it ) = 0;
		}
		m_componentList.remove();
		throw e;
	}
}

//########################################################################################

CRegExpEleComplex::~CRegExpEleComplex()
{
	for ( CList < CRegExpElement * >::iterator it = m_element_list.begin(); it != m_element_list.end();
		  it++ )
	{
		delete( *it );
		( *it ) = 0;
	}
	m_element_list.remove();
}

//########################################################################################

CRegExpElement::MResult CRegExpEleComplex::match_front( std::string::const_iterator & begin,
														std::string::const_iterator end,
														std::string::const_iterator target ) throw()
{
	assert( 0 );
	return certain_not;
}

//########################################################################################

bool CRegExpEleComplex::match_impl( std::string::const_iterator obj_b, std::string::const_iterator obj_e,
									CList < CRegExpElement * >::iterator ele_b,
									CList < CRegExpElement * >::iterator ele_e ) throw()
{
	std::string::const_iterator obj_b_cur = obj_b;
	CList < CRegExpElement * >::iterator ele_b_cur = ele_b;

	// match from left -> right.
	std::string::const_iterator obj_it_temp = obj_b;
	std::string::const_iterator target = obj_b_cur;
	while ( ele_b_cur != ele_e )
	{
		CRegExpElement::MResult result = ( *ele_b_cur )->match_front( obj_it_temp, obj_e, target );
		switch ( result )
		{
			case CRegExpElement::certain:
			{
				obj_b_cur = obj_it_temp;
				target = obj_b_cur;
				ele_b_cur++;
				break;
			}
			case CRegExpElement::certain_not:
			{
				return false;
			}
			case CRegExpElement::un_certain:
			{
				CList < CRegExpElement * >::iterator next_ele = ele_b_cur;
				next_ele++;
				if ( match_impl( obj_it_temp, obj_e, next_ele, ele_e ) )
				{
					return true;
				}
				else
				{
					obj_it_temp = obj_b_cur;
					target++;
				}
				break;
			}
		}
	}

	return ( ( ele_b_cur == ele_e ) && ( obj_b_cur == obj_e ) );
}

//########################################################################################

CRegExpElement *CRegExpEleComplex::produce_element( std::string::const_iterator & begin,
													std::string::const_iterator end ) throw( parse_error )
{
	// search for element
	CRegExpElement *pNewElement = 0;
	std::auto_ptr < CRegExpElement > ptrEle( pNewElement = produce_element_body( begin, end ) );

	// search for modifier
	ele_modifier modifier;
	try
	{
		produce_element_modifier( modifier, begin, end );
	}
	catch( invalid_modifier & )
	{
		throw parse_error();
	}

	ptrEle->set_modifier( modifier );

	ptrEle.release();

	return pNewElement;
}

//########################################################################################

CRegExpElement *CRegExpEleComplex::produce_element_body( std::string::const_iterator & begin,
														 std::string::const_iterator end ) throw( parse_error )
{
	general_char gc;
	std::string::const_iterator it = begin;

	// get next general character.
	try
	{
		gc = get_next_char( it, end );
	}
	catch( get_char_failed & )
	{
		throw parse_error();
	}

	// deal with escaped characters.
	if ( gc.escaped )
	{
		switch ( gc.character )
		{
			default:
			{
				begin = it;
				return new CRegExpEleExact( gc.character );
			}
		}					// switch ( gc.character )
	}
	// deal with normal characters.
	else
	{
		switch ( gc.character )
		{
			case '.':
			{
				begin = it;
				return new CRegExpEleAnyOne();
			}
			case '[':
			{
				CRegExpEleParser *pElement = 0;
				std::auto_ptr < CRegExpElement > ptrEle( pElement = new CRegExpEleGroup() );
				pElement->parse( begin, end );
				ptrEle.release();
				return pElement;
			}
			default:
			{
				begin = it;
				return new CRegExpEleExact( gc.character );
			}
		}					// switch ( gc.character )
	}
}

//########################################################################################

void CRegExpEleComplex::produce_element_modifier( ele_modifier & em, std::string::const_iterator & begin,
												  std::string::const_iterator end ) throw( invalid_modifier )
{
	if ( end == begin )
	{
		em.minimum = 1;
		em.maximum = 1;
		return;
	}

	general_char gc;
	std::string::const_iterator it = begin;

	// get next general character.
	try
	{
		gc = get_next_char( it, end );
	}
	catch( get_char_failed & )
	{
		throw invalid_modifier();
	}

	if ( gc.escaped )
	{
		if ( '{' == gc.character )
		{
			begin = it;
			return produce_modifier_range( em, begin, end );
		}
		else
		{
			em.minimum = 1;
			em.maximum = 1;
			return;
		}
	}
	else
	{
		switch ( gc.character )
		{
			case '?':
			{
				begin = it;
				em.minimum = 0;
				em.maximum = 1;
				break;
			}
			case '+':
			{
				begin = it;
				em.minimum = 1;
				em.maximum = MAX_COUNT;
				break;
			}
			case '*':
			{
				begin = it;
				em.minimum = 0;
				em.maximum = MAX_COUNT;
				break;
			}
			default:
			{
				em.minimum = 1;
				em.maximum = 1;
				return;
			}
		}
	}
}

//########################################################################################

void CRegExpEleComplex::produce_modifier_range( ele_modifier & em, std::string::const_iterator & begin,
												std::string::const_iterator end ) throw( invalid_modifier )
{
	general_char gc;
	std::string::const_iterator it = begin;
	bool finish = false;

	enum status
	{ reading_min, reading_max };
	status s = reading_min;

	int min = 0;
	int max = 0;
	bool min_read = false;
	bool max_read = false;
	bool comma_read = false;

	while ( !finish )
	{
		// get next general character.
		try
		{
			gc = get_next_char( it, end );
		}
		catch( get_char_failed & )
		{
			throw invalid_modifier();
		}

		switch ( s )
		{
			case reading_min:
			{
				if ( gc.escaped )
				{
					if ( '}' == gc.character )
					{
						finish = true;
						break;
					}
					else
					{
						throw invalid_modifier();
					}
				}

				if ( ( '0' <= gc.character ) && ( '9' >= gc.character ) )
				{
					min_read = true;
					min *= 10;
					min += gc.character - '0';
				}
				else if ( ',' == gc.character )
				{
					// no number before the ',' character.
					if ( !min_read )
					{
						throw invalid_modifier();
					}
					comma_read = true;
					s = reading_max;
				}
				else
				{
					throw invalid_modifier();
				}
				break;
			}
			case reading_max:
			{
				if ( gc.escaped )
				{
					if ( '}' == gc.character )
					{
						finish = true;
						break;
					}
					else
					{
						throw invalid_modifier();
					}
				}

				if ( ( '0' <= gc.character ) && ( '9' >= gc.character ) )
				{
					max_read = true;
					max *= 10;
					max += gc.character - '0';
				}
				else
				{
					throw invalid_modifier();
				}
				break;
			}
		}					// switch ( s )
	}						// while ( !finish )

	em.minimum = min;
	if ( max_read )
	{
		if ( max < min )
			throw invalid_modifier();
	}
	else if ( comma_read )
	{
		max = MAX_COUNT;
	}
	else
	{
		max = min;
	}

	em.maximum = max;
	begin = it;
}

//########################################################################################

void CRegExpEleRoot::parse( std::string::const_iterator & begin,
							std::string::const_iterator end ) throw( parse_error )
{
	while ( end != begin )
	{
		m_element_list.push_back( produce_element( begin, end ) );
	}
}

//########################################################################################

bool CRegExpEleRoot::match( const std::string & object ) throw()
{
	// perform the match algorithm
	return match_impl( object.begin(), object.end(), m_element_list.begin(), m_element_list.end() );
}

//########################################################################################

}

//########################################################################################
