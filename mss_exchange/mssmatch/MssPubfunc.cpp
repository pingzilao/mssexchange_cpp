/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/

#include "MssPubfunc.h"


CMssPubfunc::CMssPubfunc()
{
}


CMssPubfunc::~CMssPubfunc()
{
}


char* CMssPubfunc::Today()
{
	time_t t;
	static char buf[9];
	time(&t);
#ifdef WIN32
	struct tm *tt;
	tt = localtime(&t);
	sprintf(buf, "%4d%02d%02d", \
		1900 + tt->tm_year, tt->tm_mon + 1, tt->tm_mday);
#else
	struct tm tt;
	localtime_r(&t, &tt);
	sprintf(buf, "%4d%02d%02d", \
		1900 + tt.tm_year, tt.tm_mon + 1, tt.tm_mday);
#endif

	return(buf);
}