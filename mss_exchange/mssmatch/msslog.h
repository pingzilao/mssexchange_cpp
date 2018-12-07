/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/
// CMssLog.h: interface for the CMssLog class.
//create by zyao 20150630 for ICBC DSR log
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSRLOG_H__D37D5809_0158_44ED_A000_672922647684__INCLUDED_)
#define AFX_DSRLOG_H__D37D5809_0158_44ED_A000_672922647684__INCLUDED_
#include <sys/timeb.h>
#include "stdarg.h"
#include <string.h>

#ifndef WIN32
#include <time.h>
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMssLog  
{
public:
	CMssLog();
	virtual ~CMssLog();
	void Trace(const char *obj,const char *file,int line,int code,const char* szcmf,...);
	void Trace(char *dst,const char *obj,const char *file,int line,int dstLen,const char* szcmf,...);
	char m_file[128];
	char m_filepath[128];

	void Today(char *date);
	//YYYY�� MM�� DD�� HHʱ MM�� SS�� MS����
	void GetTime(char* time);
	unsigned char  AscToHex(unsigned char hex);
};

#endif // !defined(AFX_DSRLOG_H__D37D5809_0158_44ED_A000_672922647684__INCLUDED_)
