#include "common.h"
#include <iostream>



//build a func
//find a pat,start from  char *,with length of len
//if find,return the position
//if not,return the original pointer


void find(char *str,int len1,char *pat,char *&match,int &len)
{
  for( int i=0 ; i<len1 ; ++i )
  {
    int k=0;
    int j;
    for( j=i,k=0 ; j<len1 && k<4  ;++j,++k )
    {
      if( str[j]!=pat[k]) break;
    }
    if( k==4 )
    {
       match = str;
       len = i;
       return;
    }
  }
  match = str;
  len = len1;
}

using namespace std;
char *appName = "Hello";
int main()
{
  //CStr buffer;
  char buffer[]="hello\0abcbcd";
  char *pat="\r\n\r\n";
  char *res=NULL;
  int reslen=0;



  char *start =buffer;
  int startlen = sizeof(buffer)/sizeof(char)-1;
  do {
    find(start,startlen,pat,res,reslen);
    if( res!=NULL )
    {
      for( int i=0 ; i<reslen ; ++i )
      {
        printf("%x,",res[i]);
      }
      printf("\n");
    }
    start = res+reslen;
    startlen = startlen-reslen;
    if( startlen!=reslen )
    {
      start += strlen(pat);
      startlen -= strlen(pat);
    }
    /* code */
  } while(startlen>0);


  //


  // find(buffer.c_str(),buffer.length(),pat,res,reslen);

  // CStr result;
  // do {
  //   /* code */
  //   result="";
  //   buffer.cut("\r\n\r\n",result);
  //   if( result.length() )
  //   {
  //     cout << result.c_str() << ":" << result.length() << endl;
  //   }
  //
  // } while(result.length());
  return 0;
}
