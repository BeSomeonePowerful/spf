xframe v1.1   20160914
-----------------------------------------------------------------------------------------------
r375
test psatask ok! work well 
---------------------
r374
修改测试代码
---------------------
r373
add test code 
---------------------
r372
compile ok! makefile support install to self-dir 
---------------------
r371
增加psatask，调整framemng，支持psa类型
---------------------
r370
增加psatask分支
---------------------


-----------------------------------------------------------------------------------------------
xframe v1.0   20160719
-----------------------------------------------------------------------------------------------
r269
fix a bug  
---------------------
r266
fix a bug
---------------------
r265
更新所有文件的内码到 utf8
---------------------
r264
compile ok
---------------------
r262
支持对话开始的时候，设置与inst的绑定关系，后继消息如果不携带instid，只要携带了sessionid，也能正确发送到第一次分发的inst
---------------------
r193
更新编译说明 更新makefile，如果用git，不能提交空文件夹，会导致编译出错
---------------------
r192
更新文档
---------------------
r176
支持用户上下线状态统计
---------------------
r143
同步RTC-BUPT r927 于java互通null的初步改动,加好友逻辑，删除联系人 --------------------- r926 应该是提交错的文件 --------------------- 
---------------------
r107
delete xframe/_lib/hiredis-x64/libhiredis.so.0.11 and xframe/_lib/hiredis-x32/libhiredis.so.0.13 
---------------------
r96
refine build.sh 
---------------------
r95
refine xframe/makefile for copy openssl so 
---------------------
r94
delete xframe/_lib/openssl1.0.2e-x32, xframe/_lib/openssl1.0.1e-x64, xframe/_lib/openssl1.0.1i-x64 compile xframe using /usr/lib/libssl.so and /usr/lib/libcrypto.so 
---------------------
r92
删掉了getHostbyname，现在改成了读取缓存，不用了
---------------------
r85
同步到RTC-BUPT r797
---------------------
r77
同步RTC-BUPT 786  r786 fix a bug  --------------------- r785 增加域名支持，topRoute检查的时候，域名是本地同样会删除 dialog内消息和Dialog外消息，同时重用sendRequest --------------------- r784 fix a bug  --------------------- r783 重新检查并修订 发送给终端或LB或其他路由的时候，RequestURI或Route要添加tran...
---------------------
r73
refine build.sh, for 32bit, default ssl ver is openssl1.0.2e 
---------------------
r71
add hiredis-x32/libhiredis.so.0.13 as link file 
---------------------
r70
delete hiredis-x32/libhiredis.so.0.13 not link file 
---------------------
r69
add openssl1.0.1e-x64/libcrypto.so.10 openssl1.0.1e-x64/libssl.so.10 as link file 
---------------------
r68
delete openssl1.0.1e-x64/libcrypto.so.10 openssl1.0.1e-x64/libssl.so.10 not link file 
---------------------
r64
同步RTC-BUPT修改 r770 增加正则表达式 修改使用了C++11正则表达式的地方，兼容旧版本 ---------------------
---------------------
r35
refine sql trace function 
---------------------
r34
add sql trace 
---------------------
r13
delete xframe/_lib/hiredis-x64/libhiredis.so.0.11 and xframe/_lib/mysql5.1-x64/libmysqlclient_r.so.16, do ln in xframe/makefile 
---------------------
r11
mv xframe/_lib/openssl1.0.1i-x64/libcrypto.so.1.0.1 xframe/_lib/openssl1.0.1i-x64/libcrypto.so mv xframe/_lib/openssl1.0.1i-x64/libssl.so.1.0.1 xframe/_lib/openssl1.0.1i-x64/libssl.so 
---------------------
r7
some compile problem 
---------------------
r6
提交缺失的链接库
---------------------
r1
根据v2.0版本初始化版本库
---------------------
