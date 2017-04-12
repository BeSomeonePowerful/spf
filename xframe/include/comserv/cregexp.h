#ifndef _CREGEXP_H
#define _CREGEXP_H

// $Log: cregexp.h,v $
// Revision 1.1  2005/12/05 02:20:27  jlli
// no message
//
// Revision 1.1.1.1  2004/10/19 08:26:04  cvs
//
//
// Revision 1.1.1.1  2004/10/17 14:08:08  cvs
//
//
// Revision 1.1  2004/05/24 13:36:56  yl
// Created. For regular-expression.
//

#include "cregexpele.h"

namespace xframe
{

	//########################################################################################
	/**
	 * The CRegExp class provide the functions of a regular-expression.
	 **/
	//########################################################################################
	class CRegExp
	{
		public:
			class bad_rule {};
			class not_initialized {};

		public:
			CRegExp() throw() : parsed(false) {}
			CRegExp( const std::string& rule ) throw ( bad_rule );
			~CRegExp() throw() {}

			void initialize( const std::string& rule ) throw ( bad_rule );
			bool match( const std::string& object ) throw ( not_initialized, bad_rule );

		private:
			void parse() throw ( not_initialized, bad_rule );

		private:
			CRegExpEleRoot root_element;
			std::string m_rule;
			bool parsed;

	};

}
#endif// _CREGEXP_H
