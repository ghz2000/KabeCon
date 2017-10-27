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
body{font-size: 12em;}\
</style></HEAD>\
<BODY>\
\
<a href='/on' class='square_btn'>ON</a>\
<a href='/off' class='square_btn'>OFF</a><BR>\
<a href='/turn' class='square_btn'>Change</a><BR><BR>\
\
now value=\
<BR><HR>\
\
maximum =\
<BR>\
minimum =\
<BR><BR><HR>\
\
<FORM method='GET' action='/'>\
<TABLE>\
<TR><TD>\
<INPUT TYPE='value' name='servo' value=''\
'></TD><TD>\
</TD></TR>\
</TABLE>\
\
<INPUT type='submit' value='set'>\
</FORM><BR><BR>\
\
<B><BIG><BIG>ini ファイルが簡単に使えるパーサー サンプル</B></BIG></BIG><BR><BR>\
<HR>\
<A HREF='/ini'>iniファイルパーサー サンプル</A><BR>\
<A HREF='/ssid'>WiFi SSID設定用ページ</A><BR>\
<A HREF='/procini'>デバッグ用ページ</A><BR>\
<A HREF='/delini'>iniファイルを削除する(デバッグ用)</A><BR>\
</BODY></HTML>")


#endif // __ROOTPAGE__


// クラスインスタンスを作成
