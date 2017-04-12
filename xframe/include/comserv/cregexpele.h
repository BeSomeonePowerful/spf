/*********************************************************************
* Copyright (c)2005, by MT2
* All rights reserved.
* FileName：				cregexpele.h
* System：           		SoftSwitch
* SubSystem：	        	Common
* Author：					李静林
* Description：

* Last Modified:
	2005-12-05 v1.1 李静林 将list模版类替换为clist.h，用我们自己编写的CList代替

原始记录
// $Log: cregexpele.h,v $
// Revision 1.2  2005/12/07 02:59:21  jcdu
// no message
//
// Revision 1.1  2005/12/05 02:20:27  jlli
// no message
//
// Revision 1.1.1.1  2004/10/19 08:26:04  cvs
//
//
// Revision 1.1.1.1  2004/10/17 14:08:08  cvs
//
//
// Revision 1.2  2004/06/12 01:06:07  yl
// Refactoring on suggestions of Effective C++.
//
// Revision 1.1  2004/05/24 13:36:56  yl
// Created. For regular-expression.
//
*********************************************************************/



#ifndef _REGEXP_ELEMENT_H_
#define _REGEXP_ELEMENT_H_


#include <string>
#include <cassert>
using namespace std;

#include "clist.h"


namespace xframe
{

	class CRegExpEleComplex;

	//########################################################################################
	/**
	 * The CRegExpElement class is the base class of all regular-expression element.
	 **/
	//########################################################################################
	class CRegExpElement
	{
		public:
			/** MResult explained :
			 *	certain     --- matched and can not match more.
			 *	certain_not --- can't be matched.
			 *	un_certain  --- matched and there is possibility to match more.
			 **/
			enum MResult { certain, certain_not, un_certain };

		public:
			CRegExpElement() : m_modifier() {}
			virtual ~CRegExpElement() {}

			// match from front of the object string. set 'begin' to next position after a successful match.
			virtual MResult match_front( std::string::const_iterator& begin, std::string::const_iterator end,
				std::string::const_iterator target ) throw () = 0;

		protected:
			static const int MAX_COUNT = 10000;
			class ele_modifier
			{
				public:
					int minimum;
					int maximum;

					ele_modifier() : minimum(0), maximum(0) {}
			};

		protected:
			void set_modifier( const ele_modifier& em ) throw () { m_modifier = em; }

			friend class CRegExpEleComplex;

		protected:
			ele_modifier m_modifier;

	};

	//########################################################################################
	/**
	 * All elements that need a parse() method to construct itself is derived from this class.
	 **/
	//########################################################################################
	class CRegExpEleParser : public CRegExpElement
	{
		public:
			class parse_error {};

		public:
			virtual void parse( std::string::const_iterator & begin, std::string::const_iterator end ) throw ( parse_error ) = 0;

		protected:
			class get_char_failed {};
			class invalid_modifier {};

		protected:
			class general_char
			{
				public:
					char character;
					bool escaped;

					general_char() : character(0), escaped(false) {}
					general_char( char c, bool esc ) : character(c), escaped(esc) {}

					bool operator == ( const general_char& gc ) const
					{
						return (( gc.character == character )&&( gc.escaped == escaped ));
					}
			};

		protected:

			//####################################################################
			/**
			 * Get the next general character from the given string section.
			 *
			 * If the charater is a escaped character such as '\*', the 'escaped'
			 * attribute of retuened general_char object is setted to true.
			 * And the 'character' attribute contains the character after '\'.
			 *
			 * Parameter 'obj_b' will be pointed to the end of the retrieved
			 * charater.
			 *
			 * On exception, such as 'obj_b' equal to or exceed 'obj_e', escape
			 * character '\' at the end of the section, a object of class
			 * 'get_char_failed' is thrown.
			 **/
			//####################################################################
			static general_char get_next_char( std::string::const_iterator& obj_b,
				std::string::const_iterator obj_e ) throw ( get_char_failed );

	};

	//########################################################################################
	/**
	 * A exact character.
	 **/
	//########################################################################################
	class CRegExpEleExact : public CRegExpElement
	{
		public:
			CRegExpEleExact( char c ) : m_c(c) {}
			~CRegExpEleExact() {}

			MResult match_front( std::string::const_iterator& begin, std::string::const_iterator end,
				std::string::const_iterator target ) throw ();

		private:
			char m_c;
	};

	//########################################################################################
	/**
	 * Any character. @@ rule as '.' @@
	 **/
	//########################################################################################
	class CRegExpEleAnyOne : public CRegExpElement
	{
		public:
			~CRegExpEleAnyOne() {}

			MResult match_front( std::string::const_iterator& begin, std::string::const_iterator end,
				std::string::const_iterator target ) throw ();
	};

	//########################################################################################
	/**
	 * A group of character. @@ rule as [...] @@
	 **/
	//########################################################################################
	class CRegExpEleGroup : public CRegExpEleParser
	{
		public:
			CRegExpEleGroup() : m_reversed(false) {}
			~CRegExpEleGroup();

		protected:
			MResult match_front( std::string::const_iterator& begin, std::string::const_iterator end,
				std::string::const_iterator target ) throw ();

			void parse( std::string::const_iterator & begin, std::string::const_iterator end ) throw ( parse_error );

		protected:
			bool match_component( char object );

		protected:
			class CGroupItem
			{
				public:
					char begin;
					char end;

					CGroupItem() : begin(0), end(0) {}
					CGroupItem( char b, char e ) : begin(b), end(e) {}
					bool match( char object ) { return (( object >= begin )&&( object <= end )); }
			};

		protected:
			bool m_reversed;
			CList<CGroupItem*> m_componentList;

	};

	//########################################################################################
	/**
	 * All element that is a container of a list of elements.
	 **/
	//########################################################################################
	class CRegExpEleComplex : public CRegExpEleParser
	{
		public:
			~CRegExpEleComplex();

		protected:
			MResult match_front( std::string::const_iterator& begin, std::string::const_iterator end,
				std::string::const_iterator target ) throw ();

			bool match_impl( std::string::const_iterator obj_b, std::string::const_iterator obj_e,
				CList<CRegExpElement*>::iterator ele_b, CList<CRegExpElement*>::iterator ele_e ) throw ();

			//####################################################################
			/**
			 * Parse a given rule section into a list of CRegExpElement*.
			 *
			 * The rule is given between 'begin' end 'end'. Note that 'end' will
			 * NOT be exceeded but not surely be reached. It is because the
			 * routine will return immediately when a 'finish_char' character is
			 * met, where a 'finish_char' character will be ']', '}' or ')' for
			 * example.
			 *
			 * Parameter 'begin' will be moved to the position where the parse
			 * action stopped. It is the caller's duty to check whether 'begin'
			 * equals to 'end' after a parsing operation.
			 *
			 * On exception, such as no 'finish_char' found till end, invalid
			 * charater met, a object of class 'parse_error' is thrown.
			 **/
			//####################################################################
			CRegExpElement* produce_element( std::string::const_iterator & begin,
				std::string::const_iterator end ) throw ( parse_error );

			CRegExpElement* produce_element_body( std::string::const_iterator & begin,
				std::string::const_iterator end ) throw ( parse_error );

			void produce_element_modifier( ele_modifier & em, std::string::const_iterator & begin,
				std::string::const_iterator end ) throw ( invalid_modifier );

			void produce_modifier_range( ele_modifier & em, std::string::const_iterator & begin,
				std::string::const_iterator end ) throw ( invalid_modifier );

		protected:
			CList<CRegExpElement*> m_element_list;

	};

	//########################################################################################
	/**
	 * The CRegExpEleRoot element is the whole rule.
	 **/
	//########################################################################################
	class CRegExpEleRoot : public CRegExpEleComplex
	{
		public:
			~CRegExpEleRoot() {}

		public:
			void parse( std::string::const_iterator & begin, std::string::const_iterator end ) throw ( parse_error );
			bool match( const std::string& object ) throw ();

	};

}
#endif// _REGEXP_ELEMENT_H_
