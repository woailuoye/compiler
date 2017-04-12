#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASIZE 20
#define EOS '\0'
#define True  1
#define False 0

#define key_num   9    //关键字数量
#define buff_size   1024

char buff[buff_size];
int lineno = 1;       //行数
int tokenval = 1;     //数字
char lexbuf[BASIZE];  //字符缓存区
int error[10];        //错误记录

//关键字
char keyWord[key_num][10]={
"void","int","float","char","if","else",
"while","do","return"
};



//数字判断
int isDigit(char ch)
{
	if(ch>='0'&&ch<='9')
		return True;
	else
		return  False;
}


//字母判断
int isAlpha(char ch)
{
	if((ch>='a' && ch<='z') || (ch>='A' && ch <='Z'))
		return True;
	else
		return False;
}


//操作符
int isOperator(char ch)
{
	if(ch == '+'||ch=='-'||ch=='*'||ch=='/'||ch=='>'||ch=='<'||ch=='!'||ch=='=')
        return True;
  else
        return False;
}


//分隔符
int isDelimater(char ch)
{
   if(ch=='('||ch==')'||ch=='['||ch==']'||ch=='{'||ch=='}'||ch==';'||ch==',')
       return True;
   else
      return False;
}




/×
   词发分析器
   @auth：  finch
   @time：  2017/4/1
 ×/

int lexan()
{
  int error_num = 0;
  FILE *fp_in=NULL;
  FILE *fp_out=NULL;

  if((fp_in=fopen("test.txt","r"))!=NULL)  //读取源码进入缓存buff中
  {
    char ch=NULL;
    int x =0;
      while(ch!=EOF)                       //文件结束符
       {
         printf("%d\n",x);
         ch=fgetc(fp_in);
         buff[x]= ch;
         x++;
         if(x>=buff_size)
         {

            printf("Error");
            exit(-1);
          }
       }
      buff[x]=EOF;
      fclose(fp_in);
  }
  else
  {
  	printf("Error:can't find the file");
  	exit(-1);
  }


  if((fp_out=fopen("output.txt","w+"))==NULL)  //词法分析的输出
  {
     printf("Error:can't create a file");
     exit(-1);
  }


  int t=0;
  while(buff[t]!=EOF)                          //读读入内存的源码进行分析
  {
  	if (buff[t] ==' '||buff[t]=='\t')
  		    ;                          //删除空格
  	else if(buff[t]== '\n')                //换行
  	{

        fprintf(fp_out,"\n");
        printf("\n");       
        lineno = lineno +1;                    //行数
 
  	}
  	else if(isDigit(buff[t]))              //数字
  	{
      tokenval = buff[t] - '0';
      t++;
      while(isDigit(buff[t]))                  //整数
      {
      	tokenval = tokenval *10 + buff[t] -'0';
        t++;
      }
      if(isAlpha(buff[t]))                     //数字+字母 ：报错
      {
          fprintf(fp_out," LexicalError,");
          error[error_num++]=lineno;           //出错行记录
          printf("<error:%d,%c>",tokenval,buff[t]);
      }
      else
      {
        t--;                                  
        fprintf(fp_out,"<%d,->,",2);
        printf("<%d:%d>,",2,tokenval);
      }

  	}
  	else if(True==isAlpha(buff[t])||buff[t]=='_')   //标识符或者关键字
  	{

      int b = 0;
      while(isAlpha(buff[t])||isDigit(buff[t])||buff[t]=='_')   //字母或数字
      {
      	lexbuf[b] = buff[t];
      	t++;
      	b = b +1;
      	if(b >=BASIZE)
            printf("compiler error");
      }
      t--;  
      lexbuf[b] ='\0';
        if(True==isKeyWord(lexbuf))                            //关键字判断
      {
          int code = 2;
          if(0==strcmp(lexbuf,"void"))                         //查找对应代码
					code =25;
				else if(0==strcmp(lexbuf,"int"))
					code =26;
				else if(0==strcmp(lexbuf,"float"))
					code =27;
				else if(!strcmp(lexbuf,"char"))
					code =28;
				else if(!strcmp(lexbuf,"if"))
					code =29;
				else if(!strcmp(lexbuf,"else"))
					code =30;
				else if(!strcmp(lexbuf,"while"))
					code =31;
				else if(!strcmp(lexbuf,"do"))
					code =32;
				else if(!strcmp(lexbuf,"return"))
					code =33;
				else if(!strcmp(lexbuf,"main"))
					code =34;
				else if(!strcmp(lexbuf,"printf"))
					code =35;
          fprintf(fp_out,"<%d,->,",code);
          printf("<%d:%s>,",code,lexbuf);
      }
      else
      {                                                         //标识符输出
         
         fprintf(fp_out,"<%d,%s>,",1,lexbuf);
         printf("<%d:%s>,",1,lexbuf);
      }

  	}
    else if(isDelimater(buff[t]))                              //分隔符
    {
      int code = 0;
      if(buff[t]==',')
				code=24;
			else if(buff[t]==';')
				code=23;
			else if(buff[t]=='{')
				code=21;
			else if(buff[t]=='}')
				code=22;
			else if(buff[t]=='(')
				code=17;
			else if(buff[t]==')')
				code=18;
			else if(buff[t]=='[')
				code=19;
			else if(buff[t]==']')
				code=20;
      fprintf(fp_out,"<%d,->,",code);
      printf("<%d,%c>,",code,buff[t]);

    }
    else if(isOperator(buff[t]))                                  //运算符
  	{
  	    int code = 0;
  	   if(buff[t]=='+')
				code=3;
			else if(buff[t]=='-')
				code=4;
			else if(buff[t]=='*')
				code=5;
			else if(buff[t]=='/')
            {
                t++;
                if(buff[t]=='*')                                  //多行注释判断
                {
                  while(buff[t]!='/')
                  {
                      t++;                                        //忽略注释
                  }
                }
                else if(buff[t]=='/')                             //单行注释
                {
                    while(buff[t]!='\n')
                    {

                        t++;
                    }
                }else{
                  code = 4;
                  t--;
                }

            }

			else if(buff[t]=='<')
            {
                t++;
                if(buff[t]=='=')  //==
                {

                }
                else
                {
                    t--;
                  code = 8;       // <
                }
            }

			else if(buff[t]=='>')
				{
				     t++;
				    if(buff[t]=='=') // >=
                    {
                       code =11;
                    }
                    else{
                     code=10;
                     t--;
                    }
				}

			else if(buff[t]=='=')
            {
                t++; 
                if(buff[t]=='=')                 // ==
                {
                    code =12;
                }
                else{
                   code=16;                      // =
                   t--;
                }
            }

          fprintf(fp_out,"<%d,->,",code);
          printf("<%d,%c>",code,buff[t]);
      }
      t++;
  }

  if(error_num!=0)                               //错误输出
{
  fprintf(fp_out,"LexicalError(s) on line(s) ");
  for(int i=0;i<error_num;i++)
  	if(i!=error_num)
       fprintf(fp_out,"%d,",error[i]);
   else
   	fprintf(fp_out,"%d",error[i]);

}
  fclose(fp_out);
  return 0;
}


int main()                                     //主程序入口
{
	lexan();
	return 0;
}
