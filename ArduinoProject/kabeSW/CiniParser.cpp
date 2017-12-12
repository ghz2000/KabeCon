#include <Arduino.h>
#include <FS.h>
#include "CiniParser.h"

#include "Debug.h"


//iniファイル名を設定｡ 存在しない場合は-1を返す
//ファイル名が長過ぎる場合 1 を返す
int CiniParser::setIniFileName(char *fnm){
  SPIFFS.begin();

  String tmp = fnm;
  String fnmDir = tmp.substring(0, tmp.lastIndexOf('/')+1);
  String fnmExt = tmp.substring(tmp.lastIndexOf('.'));
  String fnmBody = tmp.substring( tmp.lastIndexOf('/')+1, tmp.lastIndexOf('.') );

  if(DIR_LENGTH < tmp.length()){
    //too lage length 
    return 1;
  }

  tmp = fnmDir + fnmBody + fnmExt;
  strcpy(iniFileName, tmp.c_str());

  tmp = fnmDir + "_" + fnmBody + fnmExt;
  strcpy(tmpFileName, tmp.c_str());

  DebugPrint("set Ini fnm:");
  DebugPrintln(iniFileName);
  DebugPrint("set tmp fnm:");
  DebugPrintln(tmpFileName);

  if(!SPIFFS.exists(iniFileName)){
    DebugPrintln("no file in setIniFileName");
    DebugPrint("Create : ");
    DebugPrintln( createIniFile() );
    return -1;
  }
  return 0;
}

//fnm はデフォルト引数が iniFileName
int CiniParser::openFile(File *file, char *openMode){
  char *fnm = iniFileName;
  return openFile(file, openMode, fnm);
}

int CiniParser::openFile(File *file, char *openMode, char *fnm){
  SPIFFS.begin();
  if(!SPIFFS.exists(fnm)){
    return 1;
  }
  *file = SPIFFS.open(fnm, openMode);

  return 0;
}



// iniファイルを作成する｡ 
//  既に存在している場合は1を返す
//  作成成功で0を返す
int CiniParser::createIniFile(){
  if(SPIFFS.exists(iniFileName)){
    return 1;
  }

  File file = SPIFFS.open(iniFileName, "w");
  if(file){
    file.seek(0, SeekSet);    
    file.close();
  }  

  if(!SPIFFS.exists(iniFileName)){
    return 3;
  }
  return 0;
}


// iniファイルを削除する｡ 
//  作成成功で1を返す
int CiniParser::deleteIniFile(){
  return SPIFFS.remove(iniFileName);
}

//ファイルを読み込む
//新規作成はしない
void CiniParser::readIniFile(String *str){
  int len = 1;
  char buf[101];
  File file;
  if(openFile(&file, "r"))return ;

  if(file){
    file.seek(0, SeekSet);
    
    for(int i=0; i<file.size();){
      file.readBytes(buf, len);
      buf[len] = NULL;
      *str += buf;
      i+=len;
      len = (file.size() - i) > 100 ? 100: (file.size() -i);
    }
    file.close();
  }
}

//ファイルを書き込む
//必ず新規作成する
int CiniParser::writeIniFile(String *data){
  int len = 1;
  char buf[101];
  File file;
  if(openFile(&file, "w")) return 0;

  if(file){
    file.println(*data);
    file.close();
  }

  return 0;
}



//ファイルサイズを取得する
//失敗すると0
int CiniParser::getIniFileSize(){
  File file;
  if(openFile(&file, "r")) return 0;
  if(file){
    return file.size();
  }
  return 0;
}

// Section, paraName, paraValue を確認する
// 3: paraName まで一致があれば その行を作り直してコピー
// 2: Section  まで一致があれば そのSectionの最後に追記コピー
// 1: Section  も存在しなければ､ファイルの最後に追記コピー
// 0: ファイルが存在しない
int CiniParser::rwIni(String section, String paraName, String *paraValue, int rw){
  unsigned long startPos = 0;
  unsigned long endPos = 0;
  char openMode[2] = "r";
  int res = 0;
  String writeValue = *paraValue;

  if(rw == WRITE) strcpy(openMode, "r+");
  
  File file;
  if(openFile(&file, "r")) return 0;

  if(file){
    startPos = searchSection(&file, section);
    if(0<startPos){
      //Section は存在した
      getValue(&file, &startPos, &endPos, paraName, paraValue);

      if(0<endPos){
        //SectionもparaName もあった
        res = 3;
      }else{
        //Sectionはあったが,paraName は無かった
        res = 2;
      }
    }else{
      //Section が存在しなかった
      res = 1;
    }
    file.close();
  }

  if(rw == READ) return res;
  //以下は書き込みモードの時だけ


  //dstファイルが存在してはいけない
  if(SPIFFS.exists(tmpFileName)){
    DebugPrintln("dst File exist");
    SPIFFS.remove(tmpFileName);
    DebugPrintln("deleted !!");
  }

  switch(res){
    case 1:   // 1: Section  も存在しなければ､ファイルの最後に追記コピー
      copyIniFile();  //全コピー
      writeLine(tmpFileName, "[" + section + "]" );
      writeLine(tmpFileName, paraName + "=" + writeValue );
      break;
    case 2:   // 2: Section  まで一致があれば そのSectionの最後に追記コピー
      copyFile(iniFileName, tmpFileName, 0, startPos);  //startPos までコピー
      writeLine(tmpFileName, paraName + "=" + writeValue );
      copyFile(iniFileName, tmpFileName, startPos);     //startPos から 最後までコピー
      break;
    case 3:   // 3: paraName まで一致があれば その行を作り直してコピー
      copyFile(iniFileName, tmpFileName, 0, startPos);  //startPos までコピー
      writeLine(tmpFileName, paraName + "=" + writeValue );
      copyFile(iniFileName, tmpFileName, endPos +1);       //endPos から 最後までコピー
      break;
    default:
      DebugPrintln("sonnnakoto okiru?");
      break;
  }

  //元ファイルを削除して tmpからコピー
  SPIFFS.remove(iniFileName);
  copyFile(tmpFileName, iniFileName);
  
  return res;
}

//1行書く
int CiniParser::writeLine(char *dstFnm, String line){
  File dstFile;
  SPIFFS.begin();
  dstFile = SPIFFS.open(dstFnm, "a+");
  
  if(dstFile){
    dstFile.println(line);
  }
  dstFile.close();
  
  return 0;
}




//Section が存在する場合 startPos から paraName を検索する
// 見つかった場合 その行の初めの startPos と その行の終わりの endPos と一緒に paraValue を返す
// paraName が無かった場合 Sectionの最後を startPos にし､ endPos は 0を返す
int CiniParser::getValue(File *file, unsigned long *startPos, unsigned long *endPos, String paraName, String *paraValue){
  char buf[2];
  String tmp = "";
  paraName = paraName + "=";
  DebugPrintln(paraName);

  file->seek(*startPos, SeekSet);
  while(1){
    *startPos = file->position();

    //1行取り出す
    while(file->available()){
      file->read((uint8_t*)buf, 1);
      if(buf[0] == '\n' || buf[0] == '\r') break;
      tmp += buf[0];
    }

    if(tmp.startsWith(paraName)){
      //paraName の一致があった
      DebugPrintln("much Name!!");
      *paraValue = tmp.substring(paraName.length(), tmp.length());
      DebugPrintln(*paraValue);
      *endPos = file->position();
      return file->position();
    }

    if(tmp.startsWith("[")){
      //見つからなかった
      DebugPrintln("over section");
      *endPos = 0;
      return 0;
    }

    if(!file->available()){
      //見つからなかった
      DebugPrintln("no Hit at last");
      *endPos = 0;
      return 0;
    }
    tmp = "";
  }
}

int CiniParser::searchSection(File *file, String section){
  char buf[2];
  String tmp = "";
  section = "[" + section + "]";
  DebugPrintln(section);

  file->seek(0, SeekSet);
  while(1){
    while(file->available()){
      file->read((uint8_t*)buf, 1);
      if(buf[0] == '\n' || buf[0] == '\r') break;
      tmp += buf[0];
    }
  
    DebugPrint("get line :");
    DebugPrintln(tmp);

    if(tmp.compareTo(section) == 0){
      DebugPrintln("much Section!!");
      return file->position();
    }

    if(!file->available()){
      DebugPrintln("no Hit");
      return 0;
    }
    tmp = "";
  }
}


//ファイルをコピーする
//スタート､ エンドを省略すると最初から最後までになる
//書き込み先は新規作成で追記書き込みとなる｡
//startPos, endPos は デフォルト引数 0
//srcファイルが存在しない   2
// 成功 0
int CiniParser::copyFile(char *srcFnm, char *dstFnm, unsigned long startPos, unsigned long endPos){
  int len = 1;
  char buf[2];
  String str = "";
  File srcFile;
  File dstFile;

  SPIFFS.begin();

  //srcファイルが存在しないといけない
  if(!SPIFFS.exists(srcFnm)){
    DebugPrintln("src File no exist");
    return 2;
  }

  srcFile = SPIFFS.open(srcFnm, "r");
  dstFile = SPIFFS.open(dstFnm, "a+");

  if(srcFile && dstFile){
    srcFile.seek(startPos, SeekSet);
    if(endPos == 0) endPos = srcFile.size();

    for(unsigned long i=startPos; i<endPos; i++){
      srcFile.read((uint8_t *)buf, 1);
      DebugPrint(i);
      DebugPrint(":");
      DebugPrint(buf[0]);
      DebugPrint(":");
//      Serial.println(buf[0], HEX);
      dstFile.write(buf[0]);
    }

    srcFile.close();
    dstFile.close();
  }
  return 0;
}

//iniファイルをコピーする
//dstファイルが存在している 1
//srcファイルが存在しない   2
//成功 0
//startPos, endPos は デフォルト引数 0
int CiniParser::copyIniFile(unsigned long startPos, unsigned long endPos){
  File srcFile;
  File dstFile;

  SPIFFS.begin();

  //dstファイルが存在してはいけない
  if(SPIFFS.exists(tmpFileName)){
    DebugPrintln("dst File exist");
    SPIFFS.remove(tmpFileName);
    DebugPrintln("deleted !!");
    return 1;
  }

  return copyFile(iniFileName, tmpFileName, startPos, endPos);
}











