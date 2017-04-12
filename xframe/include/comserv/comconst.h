/*********************************************************************

 * Copyright (c)2005-2008, by Beijing TeleStar Network Technology Company Ltd.(MT2)
 * All rights reserved.

 * FileName：       comconst.h
 * System：         UniFrame 
 * SubSystem：      Common
 * Author：         Long Xiangming
 * Date：           2005-2008.04.19
 * Version：        1.0
 * Description：
     公共常量定义。定义了所有模块将要用到的大部分常量，包括：
     
 *
 * Last Modified:
    2005.08.23, 从软交换系统继承到UniFrame. 完成初始版本.
        By Long Xiangming
	  2006.9.5 李静林 添加用户自定义协议栈标识

**********************************************************************/
#ifndef __COMCONST_H_
#define __COMCONST_H_

#include "comtypedef.h"

const INT INFO_BUF_SIZE          = 8096;//输出信息缓冲区大小。决定了输出信息的最大长度。 
const INT INFO_PRE_SIZE          = 96;  //输出信息前缀的长度。
const INT MAX_INFO_SIZE          = (INFO_BUF_SIZE-INFO_PRE_SIZE);
                               //传入print(),errorLog等方法的字符串应该
                                        //大小应该<(INFO_BUF_SIZE-INFO_PRE_SIZE)
//对于CVarChar，应该设定一个最大长度，否则如果socket传送有误，length出错，则可能解码时造成越界，从而core dump.
const INT MAX_VCHAR_LENGTH       = 8192;//4096; modified by lxm. 2008.12.31
const INT MAX_FCHAR_LENGTH       = 256; //added by lxm. 2009-02-26
//added by Long Xiangming. 2008.04.02
const INT MAX_ARRAY_SIZE         = 1024;  //消息定义中，数组的最大长度。（CVarChar则使用MAX_VCHAR_LENGTH限制）
                                          //当socket传输失败时，解码数组时，length越界可能导致段错误。因此规定一个最大长度限制。

#ifdef WIN32
#define MAXINT 65535  //used in func.h, Unix already pre-defined
#endif

//added by Long Xiangming. 2007.10.20
//系统定时器：1～9
const INT T_SYS_MON_CPU          = 1;
const INT T_SYS_MON_MEM          = 2;   
const INT T_MONITOR_SERVERSTATE  = 3;
const INT T_MONITOR_PROCESSSTATE = 4;
const INT T_MONITOR_DBSTATE      = 5;

//for amehandler(command processing)
const I32  MaxArgC               = 20;
const I32  MaxArgVLen            = 255;
const I32  MaxKeyBufferLength    = 512;
const I32  MaxCmdNum             = 100 ;
const I32  MaxCmdLen             = 8000;//255; modified by lxm.2008.12.31 
const I32  MaxCmdResponseLen     = 8000;

//for socket msg
const BYTE COM_MSG_LENGTH       = 4;      /* Bytes */ //靠靠靠靠靠?const BYTE COM_MSG_LENGTH_LENGTH = COM_MSG_LENGTH;
const I32  MaxMsgLength          = 81920;    // 80K. (8192*10). Modified by Long Xiangming. 2007.04.25.
const I32  MAX_MSG_BUF_SIZE      = MaxMsgLength; //靠socket縝uffer靠?                                                 // 靠MSRP靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠Buffer靠靠靠靠靠靠靠靠
const I32  MaxResendLength       =  81920; //80K.
const I32  MaxRecvLength           =  81920; //80K.
const I32  MaxEmergencyLength   = 40960;//40K

#endif
