/****************************************************************
 * Copyright (c)2005, by Beijing TeleStar Network Technology Company Ltd.(MT2) 
 * All rights reserved.
 *      The copyright notice above does not evidence any
 *      actual or intended publication of such source code

 * FileName:	defs.h $
 * Version:		1.0
 * Modifier:    hxp
 * Date    :    2005.8.15 

 * Description:

   2005-11-29 v1.2 李静林 修订了格式，保证在Dev-C++中也能编译通过		

****************************************************************/

#ifndef __DEFS_
#define __DEFS_

//==========================================================================
#define _DATATYPEDEF(type,name) \
            typedef type name; \
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _ENUMTYPEDEF(name) \
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _ARRAYTYPEDEF(type,name,size) \
            typedef type name[size];\
            typedef name* P##name;\
            typedef name& R##name;\
            typedef name*& RP##name;

#define _CLASSDEF(name) \
            class name; \
            typedef name  * P##name;\
            typedef name  & R##name;\
            typedef name  *  & RP##name;\
            typedef const name  * PC##name;\
            typedef const name  & RC##name;

//==========================================================================
//内核taskid设置
#define __FRAMEMNG_TASKID 	1
#define __FRAMECONTROL_TASKID  	2	
#define __TASKSELECTOR_TASKID 	3
#define __TASKCONTAINER_TASKID  4
//系统预留task数量小于100（不含）
#define __USERDEFINED_TASKID_START	 100

//host 和 Container的最大取值，合起来作为一个处理实例的标识
#define __MAX_HOSTID			0xFFFFF		//默认hostid共计主机1048575个（主机数量）
#define __MAX_TASKCONTAINERID   0xFFF		//默认TaskContainerID共计4096个（主机上的线程数量）
#define __MAX_TASKCONTAINERID_BIT   12		//默认TaskContainerID占用12bit/0xFFF

//MPtask的起始inst取值（不含），小于100的是AbstractTask、PSATask的对象
#define __MPTASKINSTID_START 100

#endif

