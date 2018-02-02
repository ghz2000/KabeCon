#ifndef __ROOTPAGE__
#define __ROOTPAGE__

#define ROOT_HTML F("\
<HTML><HTAD><meta http-equiv='Content-Type' content='text/html; charset=utf-8'>\
</HEAD><BODY>\
<style type='text/css'>\
.square_btn{\
    display: inline-block;\
    padding: 0.5em 1em;\
    text-decoration: none;\
    background: #668ad8;\
    color: #FFF;\
    border-bottom: solid 4px #627295;\
    border-radius: 3px;\
}\
.square_btn:active {\
    -ms-transform: translateY(4px);\
    -webkit-transform: translateY(4px);\
    transform: translateY(4px);\
    border-bottom: none;\
}\
html{font-size: 62.5%;}\
body{font-size: 10em;}\
</style></HEAD>\
<BODY>\
\
<a href='/on' class='square_btn'>ON</a>\
<a href='/off' class='square_btn'>OFF</a><BR>\
<a href='/turn' class='square_btn'>Change</a><BR><BR>\
\
<BR><BR><HR>\
\
<B><BIG><BIG>設定</B></BIG></BIG><BR>\
<HR>\
<A HREF='/ssid'>WiFi 設定</A><BR>\
<A HREF='/ini'>iniファイル直接設定</A><BR>\
<A HREF='/procini'>iniデバッグ用</A><BR>\
<A HREF='/delini'>設定を初期化</A><BR>\
<A HREF='/update'>ファームウェア更新</A><BR>\
Ver.2.01\
</BODY></HTML>")


#endif // __ROOTPAGE__



