/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/
// DataDispose.h: interface for the CDataDispose class.
// Create by zyao 20181114 for mss exchange ��ϴ�������
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATADISPOSE_H__CCDC35C7_562B_477D_BA48_A3DB4B042D3F__INCLUDED_)
#define AFX_DATADISPOSE_H__CCDC35C7_562B_477D_BA48_A3DB4B042D3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "jsonpack.h"  //��׼mssjson��ʽ���ݰ�������
#include "BaseDefine.h"  //�����ṹ�嶨���ļ�
#include "msslog.h"
#include "hiredis.h"
#include "MssPubfunc.h"
#include <iostream>
#include <string.h>
#include <list>
#include <vector>
#include <time.h>
#ifdef WIN32
#include<windows.h>
#endif
using namespace std;

extern CMssLog g_logfile;
extern S_CFG_FILE g_cfgfile; //ȫ�������ļ�
extern CMssPubfunc g_pub;	//����ȫ�ֵ��õĻ�����


class CDataDispose  
{
public:
	CDataDispose();
	virtual ~CDataDispose();
public:
	list<S_ENTRUSTFIELD*> m_listEntrustSell;	//��ί�е��б�
	list<S_ENTRUSTFIELD*> m_listEntrustBuy;		//��ί�е��б�
	double m_currentPrice;						//��ǰ���¼۸�
	char m_errMsg[8192];						//������Ϣ
	redisContext *m_redisClient;				//redis���
	//char m_tradeDate[16];						//��ŵ�ǰ��Ȼ����,������Ϊ��ˮ�ŵ�ǰ׺
public:
	//�����յ���mssjson��ʽ����
	int Dispose(CJsonPack &reqJson);

	//��ί�м�¼����������б��м۸���ʵ�λ��
	int AddEntrust2List(CJsonPack &reqJson);
	
	//�յ�һ��ί�е���ʼ���д��ǰ����
	int PreMatch(CJsonPack &reqJson);

	//�յ�һ��ί�е���ʼ���д�ϴ���
	int MatchEngine(CJsonPack &reqJson);

	//���㵱ǰ���³ɽ�����Ϊ���鵱ǰ�м�
	double GetCurrentMatchPrice(double buyPrice,double salePrice);

	//ί�лر�
	int EntrustReturn(CJsonPack &reqJson);

	//�ɽ��ر�
	int DoneReturn(CJsonPack &reqJson);

	//��ȡȫ��Ψһ������ˮ�ţ����׷��Ч�ʿɽ�ί�гɽ��Ƚ��׸�����������ˮ�ŷֿ����ɣ�
	//int64 GetGlobalSerialNo();
	char *GetGlobalSerialNo(); //����Ǿ������ַ�����

	//ί�д������
	int EntrustHandle(CJsonPack &reqJson);

	//��ί������Ҫ��д��ṹ����
	int GetValueToBuffer(CJsonPack &reqJson, S_ENTRUSTFIELD *newEntrust);
};

#endif // !defined(AFX_DATADISPOSE_H__CCDC35C7_562B_477D_BA48_A3DB4B042D3F__INCLUDED_)
