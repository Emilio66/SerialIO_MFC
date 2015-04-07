#include "stdafx.h"
#include "CommonUtil.h"
#include <string.h>

void CString2Char(CString str,char * buffer){
	int len=str.GetLength();
	memset(buffer,0,len+1);

	WideCharToMultiByte(CP_OEMCP, NULL, (LPCWSTR)str, -1,NULL, 0, NULL, FALSE);
	WideCharToMultiByte(CP_OEMCP, NULL, (LPCWSTR)str, -1,(LPSTR)buffer, len, NULL, FALSE);
    
	buffer[len]='\0';
}

void Char2CString(char * inputbuffer,CString &content){
	//Ԥת�����õ�����ռ�Ĵ�С
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, inputbuffer, strlen(inputbuffer), NULL, 0);
	//����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ�
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	//ת��
	::MultiByteToWideChar(CP_ACP, NULL, inputbuffer, strlen(inputbuffer), wszString, wcsLen);
	//������'\0'
	wszString[wcsLen] = '\0';
	//���ӵ�CString������
	content.Append(wszString);
}

void Char2Hex(unsigned char n,char* res){
	short digit = n & 0xf;		//n%16
	short decimal = (n-digit) >> 4;	//n/16
	res[0] = digit>9?('A'-10)+digit : '0'+digit;
	res[1] = decimal>9?('A'-10)+decimal : '0'+decimal;
}
void HexDisplay(unsigned char* data, char* str,int length){
	int i=0;
	for(;i<length;i++){
		char res[2]={0};
		Char2Hex(data[i],res);
		str[i*2]=res[1];
		str[i*2+1]=res[0];
	}
	str[i*2]='\0';
}
