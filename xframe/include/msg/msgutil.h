/*********************************************************************
 * FileName：       msgutil.h
 * System：         XFrame
 * SubSystem：
 * Author：         Li Jinglin
 * Date：           2011.02.11
 * Version：        4.0
 * Description：
     与消息处理相关的一些工具，包括编解码宏，一些处理函数等.
*********************************************************************/
#ifndef _MSGUTIL_H
#define _MSGUTIL_H

#ifndef WIN32
#include <netinet/in.h> //include htons,htonl,etc.
#else
#include <winsock2.h> //include htons,htonl,etc.
#endif
#include "comtypedef.h"
#include "comconst.h"
#include "comtypedef_vchar.h"

//判断字段的_flag是否设置
#ifndef IS_FIELD_SET
#define IS_FIELD_SET(MSG,MSGCLASS,FIELD)\
	(MSG->optionSet & MSGCLASS::FIELD##_flag)
#endif

//设置optionSet
#ifndef OPTION_SET
#define OPTION_SET(MSG,MSGCLASS,FIELD);\
	MSG->optionSet |= MSGCLASS::FIELD##_flag;
#endif



///////////////////////////////////////////////
//   二进制编解码宏定义
///////////////////////////////////////////////
//   注意，对于INT,UINT,SHORT,USHORT,ENUM,BOOL等类型，需要转换成网络字节顺序
//   但字符和字符串类型不需转换
//   （注：宏中的大括号是必需的，因为可能调用宏之前使用了if语句

#define ENCODE_LONG( BUFFER, VAR ) \
	{\
		ULONG iTemp = htonl(VAR);\
		memcpy( BUFFER, &iTemp, sizeof(VAR) ); \
		BUFFER += sizeof(VAR);\
	}

#define DECODE_LONG( VAR, BUFFER ) \
	{\
		ULONG iTemp = 0;\
		memcpy( &iTemp, BUFFER, sizeof(VAR) ); \
		VAR = ntohl(iTemp);\
		BUFFER += sizeof(VAR);\
	}

#define ENCODE_BOOL( BUFFER, VAR ) \
	ENCODE_INT( BUFFER, VAR )

#define DECODE_BOOL( VAR, BUFFER ) \
	DECODE_INT( VAR, BUFFER )


#define ENCODE_SHORT( BUFFER, VAR ) \
	{\
		USHORT iTemp = htons(VAR);\
		memcpy( BUFFER, &iTemp, sizeof(VAR) ); \
		BUFFER += sizeof(VAR);\
	}

#define DECODE_SHORT( VAR, BUFFER ) \
	{\
		USHORT iTemp = 0;\
		memcpy( &iTemp, BUFFER, sizeof(VAR) ); \
		VAR = ntohs(iTemp);\
		BUFFER += sizeof(VAR);\
	}

#define ENCODE_ENUM( BUFFER, VAR ) \
	{\
		UINT iTemp = 0;\
		memcpy(&iTemp,&VAR,sizeof(VAR));\
		UINT iTemp2 = htonl(iTemp);\
		memcpy( BUFFER, &iTemp2, sizeof(VAR) ); \
		BUFFER += sizeof(VAR);\
	}

#define DECODE_ENUM( VAR, BUFFER ) \
	{\
		UINT iTemp = 0;\
		memcpy( &iTemp, BUFFER, sizeof(VAR) ); \
		UINT  iTemp2= ntohl(iTemp);\
		memcpy(&VAR,&iTemp2,sizeof(VAR));\
		BUFFER += sizeof(VAR);\
	}

#define ENCODE_FLOAT( BUFFER, VAR ) \
	{\
		UINT iTemp = 0;\
		memcpy(&iTemp,&VAR,sizeof(VAR));\
		UINT iTemp2 = htonl(iTemp);\
		memcpy( BUFFER, &iTemp2, sizeof(VAR) ); \
		BUFFER += sizeof(VAR);\
	}

#define DECODE_FLOAT( VAR, BUFFER ) \
	{\
		UINT iTemp = 0;\
		memcpy( &iTemp, BUFFER, sizeof(VAR) ); \
		UINT  iTemp2= ntohl(iTemp);\
		memcpy(&VAR,&iTemp2,sizeof(VAR));\
		BUFFER += sizeof(VAR);\
	}
/*
#define ENCODE_FLOAT( BUFFER, VAR ) \
	{\
		FLOAT fTemp = xfloat(VAR);\
		memcpy( BUFFER, &fTemp, sizeof(VAR) ); \
		BUFFER += sizeof(VAR);\
	}

#define DECODE_FLOAT( VAR, BUFFER ) \
	{\
		FLOAT fTemp = 0;\
		memcpy( &fTemp, BUFFER, sizeof(VAR) ); \
		VAR = xfloat(fTemp);\
		BUFFER += sizeof(VAR);\
	}
*/

#define ENCODE_INT_ARRAY( BUFFER, ARRAY, LENGTH ) \
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				iTemp = htonl(ARRAY[i]);\
				memcpy( BUFFER, &iTemp, sizeof(INT) ); \
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define DECODE_INT_ARRAY( ARRAY, BUFFER, LENGTH )\
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
		if(LENGTH>0)\
		{\
			UINT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(INT) ); \
				ARRAY[i] = ntohl(iTemp);\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define ENCODE_LONG_ARRAY( BUFFER, ARRAY, LENGTH ) \
	{\
		if(LENGTH>0)\
		{\
			ULONG iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				iTemp = htonl(ARRAY[i]);\
				memcpy( BUFFER, &iTemp, sizeof(LONG) ); \
				BUFFER += sizeof(LONG);\
			}\
		}\
	}

#define DECODE_LONG_ARRAY( ARRAY, BUFFER, LENGTH )\
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
		if(LENGTH>0)\
		{\
			ULONG iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(LONG) ); \
				ARRAY[i] = ntohl(iTemp);\
				BUFFER += sizeof(LONG);\
			}\
		}\
	}

#define ENCODE_BOOL_ARRAY( BUFFER, ARRAY, LENGTH ) \
	 ENCODE_INT_ARRAY( BUFFER, ARRAY, LENGTH )


#define DECODE_BOOL_ARRAY( ARRAY, BUFFER, LENGTH )\
	DECODE_INT_ARRAY( ARRAY, BUFFER, LENGTH )


#define ENCODE_SHORT_ARRAY( BUFFER, ARRAY, LENGTH ) \
	{\
		if(LENGTH>0)\
		{\
			USHORT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				iTemp = htons(ARRAY[i]);\
				memcpy( BUFFER, &iTemp, sizeof(SHORT) ); \
				BUFFER += sizeof(SHORT);\
			}\
		}\
	}

#define DECODE_SHORT_ARRAY( ARRAY, BUFFER, LENGTH )\
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
		if(LENGTH>0)\
		{\
			USHORT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(SHORT) ); \
				ARRAY[i] = ntohs(iTemp);\
				BUFFER += sizeof(SHORT);\
			}\
		}\
	}

#define ENCODE_ENUM_ARRAY( BUFFER, ARRAY, LENGTH ) \
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, &ARRAY[i], sizeof(INT) ); \
				iTemp2 = htonl(iTemp);\
				memcpy( BUFFER, &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}


#define DECODE_ENUM_ARRAY( ARRAY, BUFFER, LENGTH )\
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp,BUFFER, sizeof(INT) ); \
				iTemp2 = ntohl(iTemp);\
				memcpy( &ARRAY[i], &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define ENCODE_FLOAT_ARRAY( BUFFER, ARRAY, LENGTH ) \
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, ARRAY[i], sizeof(INT) ); \
				iTemp2 = htonl(iTemp);\
				memcpy( BUFFER, &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define DECODE_FLOAT_ARRAY( ARRAY, BUFFER, LENGTH )\
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAY);\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp,BUFFER, sizeof(INT) ); \
				iTemp2 = ntohl(iTemp);\
				memcpy( ARRAY[i], &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}
/////////////////////////////////////////////////////////
// 以下为不作网络字节顺序转换的宏. 对于字符类型,使用这些宏
/////////////////////////////////////////////////////////

#define ENCODE_NEST_ARRAY( BUFFER, ARRAYNEST, LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYNEST[i].encode(BUFFER);\
		}\
	}

#define DECODE_NEST_ARRAY( ARRAYNEST,BUFFER,LENGTH ) \
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAYNEST);\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYNEST[i].decode(BUFFER);\
		}\
	}

#define ENCODE_VCHAR_ARRAY( BUFFER, ARRAYVCHAR, LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYVCHAR[i].encode(BUFFER);\
		}\
	}


#define DECODE_VCHAR_ARRAY( ARRAYVCHAR,BUFFER,LENGTH ) \
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAYVCHAR);\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYVCHAR[i].decode(BUFFER);\
		}\
	}


#define DECODE_FCHAR_ARRAY( ARRAYFCHAR,BUFFER,LENGTH ) \
	{\
		CHECK_MAX_ARRAY_SIZE(LENGTH,ARRAYFCHAR);\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYFCHAR[i].decode(BUFFER);\
		}\
	}

#define ENCODE_FCHAR_ARRAY( BUFFER, ARRAYFCHAR, LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
					ARRAYFCHAR[i].encode(BUFFER);\
		}\
	}

// LIST ENCODE/DECODE
#define ENCODE_INT_LIST( BUFFER, CLASS, LIST ) \
	{\
		if(LIST.size()>0)\
		{\
			UINT iTemp=0;\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
			{\
				iTemp = htonl(*i);\
				memcpy( BUFFER, &iTemp, sizeof(INT) ); \
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define DECODE_INT_LIST( CLASS, LIST, BUFFER, LENGTH )\
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(INT) ); \
				CLASS value = ntohl(iTemp);\
				LIST.push_back(value);\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define ENCODE_LONG_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{\
			ULONG iTemp=0;\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
			{\
				iTemp = htonl(*i);\
				memcpy( BUFFER, &iTemp, sizeof(LONG) ); \
				BUFFER += sizeof(LONG);\
			}\
		}\
	}

#define DECODE_LONG_LIST( CLASS, LIST, BUFFER, LENGTH )\
	{\
		if(LENGTH>0)\
		{\
			ULONG iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(LONG) ); \
				CLASS value = ntohl(iTemp);\
				LIST.push_back(value);\
				BUFFER += sizeof(LONG);\
			}\
		}\
	}

#define ENCODE_BOOL_LIST( BUFFER, LIST, LENGTH ) \
	 ENCODE_INT_LIST( BUFFER, LIST, LENGTH ) 


#define DECODE_BOOL_LIST( CLASS, LIST, BUFFER, LENGTH )\
	DECODE_INT_LIST( CLASS, LIST, BUFFER, LENGTH )


#define ENCODE_SHORT_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{\
			USHORT iTemp=0;\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
			{\
				iTemp = htons(*i);\
				memcpy( BUFFER, &iTemp, sizeof(SHORT) ); \
				BUFFER += sizeof(SHORT);\
			}\
		}\
	}

#define DECODE_SHORT_LIST( CLASS, LIST, BUFFER, LENGTH )\
	{\
		if(LENGTH>0)\
		{\
			USHORT iTemp=0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp, BUFFER, sizeof(SHORT) ); \
				CLASS value = ntohs(iTemp);\
				LIST.push_back(value);\
				BUFFER += sizeof(SHORT);\
			}\
		}\
	}

#define ENCODE_ENUM_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
			{\
				memcpy( &iTemp, &(*i), sizeof(INT) ); \
				iTemp2 = htonl(iTemp);\
				memcpy( BUFFER, &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}
	

#define DECODE_ENUM_LIST( CLASS, LIST, BUFFER, LENGTH )\
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp,BUFFER, sizeof(INT) ); \
				iTemp2 = ntohl(iTemp);\
				CLASS value;\
				memcpy( &value, &iTemp2, sizeof(INT) );\
				LIST.push_back(value);\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define ENCODE_FLOAT_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
			{\
				memcpy( &iTemp, &(*i), sizeof(INT) ); \
				iTemp2 = htonl(iTemp);\
				memcpy( BUFFER, &iTemp2, sizeof(INT) );\
				BUFFER += sizeof(INT);\
			}\
		}\
	}

#define DECODE_FLOAT_LIST( CLASS, LIST, BUFFER, LENGTH )\
	{\
		if(LENGTH>0)\
		{\
			UINT iTemp = 0;\
			UINT iTemp2 = 0;\
			for(int i=0;i<LENGTH;i++)\
			{\
				memcpy( &iTemp,BUFFER, sizeof(INT) ); \
				iTemp2 = ntohl(iTemp);\
				CLASS value;\
				memcpy( value, &iTemp2, sizeof(INT) );\
				LIST.push_back(value);\
				BUFFER += sizeof(INT);\
			}\
		}\
	}
/////////////////////////////////////////////////////////
// 以下为不作网络字节顺序转换的宏. 对于字符类型,使用这些宏
/////////////////////////////////////////////////////////

#define ENCODE_NEST_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{	\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
				(*i).encode(BUFFER);\
		}\
	}

#define DECODE_NEST_LIST( CLASS, LIST,BUFFER,LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
			{\
				CLASS value;\
				value.decode(BUFFER);\
				LIST.push_back(value);\
			}\
		}\
	}

#define ENCODE_VCHAR_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{	\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
				(*i).encode(BUFFER);\
		}\
	}


#define DECODE_VCHAR_LIST( CLASS, LIST,BUFFER,LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
			{\
				CLASS value;\
				value.decode(BUFFER);\
				LIST.push_back(value);\
			}\
		}\
	}


#define DECODE_FCHAR_LIST( CLASS, LIST,BUFFER,LENGTH ) \
	{\
		if(LENGTH>0)\
		{	\
			for(INT i=0;i<LENGTH;i++)\
			{\
				CLASS value;\
				value.decode(BUFFER);\
				LIST.push_back(value);\
			}\
		}\
	}

#define ENCODE_FCHAR_LIST( BUFFER, CLASS, LIST) \
	{\
		if(LIST.size()>0)\
		{	\
			for(CList<CLASS>::iterator i=LIST.begin();i!=LIST.end();i++)\
				(*i).encode(BUFFER);\
		}\
	}

///////////////////////////////////////////////
//   消息比较宏定义
///////////////////////////////////////////////
#ifndef COMPARE_MSG_BEGIN
#define COMPARE_MSG_BEGIN(thisClassName,MSG)\
	P##thisClassName comparedMsg2 = (P##thisClassName)(&MSG);\
	P##thisClassName comparedMsg1 = this;
#endif
//  <dynamic_cast>在Windows下运行时出错！
//	P##thisClassName comparedMsg2 = dynamic_cast<P##thisClassName>(&MSG);\

//以xml中的消息字段为准。所以，不比较optionSet.
/*
	if(comparedMsg1->optionSet != comparedMsg2->optionSet) \
	{\
		return FALSE;\
	}
*/

#ifndef COMPARE_NEST_BEGIN
#define COMPARE_NEST_BEGIN(thisClassName,MSG)\
    P##thisClassName comparedMsg2 = &MSG;\
	P##thisClassName comparedMsg1 = this;
#endif

#ifndef COMPARE_ARRAY_BEGIN
#define COMPARE_ARRAY_BEGIN(thisClassName,MSG)\
    P##thisClassName comparedMsg2 = &MSG;\
	P##thisClassName comparedMsg1 = this;
#endif

#ifndef COMPARE_FORCE_COM
#define COMPARE_FORCE_COM(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_FORCE_BOOL
#define COMPARE_FORCE_BOOL(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_BOOL
#define COMPARE_OPTION_BOOL(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_BYTE
#define COMPARE_FORCE_BYTE(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_BYTE
#define COMPARE_OPTION_BYTE(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_CHAR
#define COMPARE_FORCE_CHAR(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_CHAR
#define COMPARE_OPTION_CHAR(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_INT
#define COMPARE_FORCE_INT(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_INT
#define COMPARE_OPTION_INT(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_LONG
#define COMPARE_FORCE_LONG(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_LONG
#define COMPARE_OPTION_LONG(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_FLOAT
#define COMPARE_FORCE_FLOAT(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_FLOAT
#define COMPARE_OPTION_FLOAT(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_VCHAR
#define COMPARE_FORCE_VCHAR(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_VCHAR
#define COMPARE_OPTION_VCHAR(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_FCHAR
#define COMPARE_FORCE_FCHAR(thisClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_FCHAR
#define COMPARE_OPTION_FCHAR(thisClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif

#ifndef COMPARE_FORCE_NEST
#define COMPARE_FORCE_NEST(thisClassName,nestClassName,variableName)\
	 COMPARE_FORCE_FIELD(thisClassName,nestClassName,variableName)
#endif

#ifndef COMPARE_OPTION_NEST
#define COMPARE_OPTION_NEST(thisClassName,nestClassName,variableName)\
	 COMPARE_OPTION_FIELD(thisClassName,nestClassName,variableName)
#endif

//注意，最外层的大括号是必需的。因为在同一个函数中同时调用多个ARRAY这样的宏，
//如果没有外面的大括号， 则int i被认为是重复定义。
#ifndef COMPARE_FORCE_BOOL_ARRAY
#define COMPARE_FORCE_BOOL_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_BOOL_ARRAY
#define COMPARE_OPTION_BOOL_ARRAY(thisClassName,variableName,repeatNum)\
		if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		{\
			for(int i=0;i<repeatNum;i++)\
			if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
			{\
				return FALSE;\
			}\
		}
#endif

#ifndef COMPARE_FORCE_CHAR_ARRAY
#define COMPARE_FORCE_CHAR_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_CHAR_ARRAY
#define COMPARE_OPTION_CHAR_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_BYTE_ARRAY
#define COMPARE_FORCE_BYTE_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_BYTE_ARRAY
#define COMPARE_OPTION_BYTE_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_INT_ARRAY
#define COMPARE_FORCE_INT_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_INT_ARRAY
#define COMPARE_OPTION_INT_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_FLOAT_ARRAY
#define COMPARE_FORCE_FLOAT_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_FLOAT_ARRAY
#define COMPARE_OPTION_FLOAT_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(float i=0;i<repeatNum;i++)\
		if(comparedMsg1->variableName[i]!=comparedMsg2->variableName[i]) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_VCHAR_ARRAY
#define COMPARE_FORCE_VCHAR_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->variableName[i]==comparedMsg2->variableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_VCHAR_ARRAY
#define COMPARE_OPTION_VCHAR_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->variableName[i]==comparedMsg2->variableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_FCHAR_ARRAY
#define COMPARE_FORCE_FCHAR_ARRAY(thisClassName,variableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->variableName[i]==comparedMsg2->variableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_FCHAR_ARRAY
#define COMPARE_OPTION_FCHAR_ARRAY(thisClassName,variableName,repeatNum)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->variableName[i]==comparedMsg2->variableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif


#ifndef COMPARE_FORCE_NEST_ARRAY
#define COMPARE_FORCE_NEST_ARRAY(thisClassName,nestClassName,nestVariableName,repeatNum)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->nestVariableName[i]==comparedMsg2->nestVariableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_NEST_ARRAY
#define COMPARE_OPTION_NEST_ARRAY(thisClassName,nestClassName,nestVariableName,repeatNum)\
	if(thisClassName::nestVariableName##_flag & comparedMsg1->optionSet)\
	{\
		for(int i=0;i<repeatNum;i++)\
		if(!(comparedMsg1->nestVariableName[i]==comparedMsg2->nestVariableName[i])) \
		{\
			return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_FORCE_NEST_LIST
#define COMPARE_FORCE_NEST_LIST(thisClassName,nestClassName,nestVariableName)\
	{\
		if(comparedMsg1->nestVariableName.size() != comparedMsg2->nestVariableName.size())\
			return FALSE;\
		for(int i = 0; i < comparedMsg1->nestVariableName.size(); i++)\
		{\
			nestClassName* v1;\
			nestClassName* v2;\
			comparedMsg1->nestVariableName.get_p(i, v1);\
			comparedMsg2->nestVariableName.get_p(i, v2);\
			if(*v1 != *v2)\
				return FALSE;\
		}\
	}
#endif

#ifndef COMPARE_OPTION_NEST_LIST
#define COMPARE_OPTION_NEST_LIST(thisClassName,nestClassName,nestVariableName)\
	if(thisClassName::nestVariableName##_flag & comparedMsg1->optionSet)\
	{\
		COMPARE_FORCE_NEST_LIST(thisClassName,nestClassName,nestVariableName)\
	}
#endif

#ifndef COMPARE_END
#define COMPARE_END\
	return TRUE;
#endif

////////////////////////////////
// 以下仅供内部调用, 不向外开放
////////////////////////////////
#ifndef COMPARE_FORCE_FIELD
#define COMPARE_FORCE_FIELD(thisClassName,fieldClassName,variableName)\
	if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
	{\
		return FALSE;\
	}
#endif

#ifndef COMPARE_OPTION_FIELD
#define COMPARE_OPTION_FIELD(thisClassName,fieldClassName,variableName)\
	if(thisClassName::variableName##_flag & comparedMsg1->optionSet)\
		if(!(comparedMsg1->variableName==comparedMsg2->variableName)) \
		{\
			return FALSE;\
		}
#endif


void getMsgFieldName(const char** p, CStr& fieldName, int& arrayIndex);




#endif

