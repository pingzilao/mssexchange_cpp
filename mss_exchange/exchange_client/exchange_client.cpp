/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/
#include <iostream>
#include "hiredis.h"
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "msslog.h"
#include "jsonpack.h"

using namespace std;





void RedisSync(char *buf);
void RedisSendList(char *buf);

//CMssLog g_logfile;
int main()
{

	//RedisSync(buf);
	RedisSendList(NULL);
	return 0;
}

//redis���ķ���,ͬ�������ӿ�
void RedisSync(char *buf)
{
	redisContext* rc = redisConnect("140.143.244.108", 9998);//ͬredis����������
	if (rc == NULL || rc->err)
	{
		printf("redis connect error\n");
		return;
	}
	redisReply *reply = NULL;
//	reply = (redisReply *)redisCommand(rc, "AUTH 123456");//����ͬredis�����õ����ӵ���֤����
//	freeReplyObject(reply);
	int count = 1;
	reply = NULL;
	while (1)
	{
		reply = (redisReply *)redisCommand(rc, "PUBLISH %s %d", "quotation", count);//����quotationͨ������ʶΪ��quotation��countΪ���������ݿ����滻Ϊ���鱨��
#ifdef WIN32
		Sleep(3000);
#else
		sleep(3);
#endif // WIN32
		
		printf("����:%d��", count);
		count++;
		freeReplyObject(reply);
	}
	
	reply = NULL;
	redisFree(rc);
}

void RedisSendList(char *req)
{
	//���ļ��ж�ȡ������
	FILE *fp;
	CJsonPack readJson;
	fp = fopen("entrust.req","r");
	if (fp == NULL)
	{
		printf("fopen entrust.req error\n");
		return;
	}
	char buf[8192] = {0};
	
	redisContext* rc = redisConnect("140.143.244.108", 9998);//ͬredis����������
	if (rc == NULL || rc->err)
	{
		printf("redis connect error\n");
		return;
	}
	redisReply *reply = NULL;
	int count = 1;
	reply = NULL;
	char temp[32] = {0};
	while (1)
	{
		memset(buf,0,sizeof(buf));
		if (fgets(buf,8192,fp) == NULL)   //��ȡmssjson��ʽί������
		{
			printf("�ļ���ȡ����,�����˳�\n");
			break;
		}
		bool flag = false;
		readJson.Clear();
		flag = readJson.Parse(buf);
		if (flag == false)
		{
			//g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"json����ʧ��,��������[%s]",buf);
			return ;
		}
		//��ί�����ݽ��д����Ա�֤�ܳɽ�����֤��Ϲ���
		memset(temp,0,sizeof(temp));
		readJson["body"].Get("CustNo",temp);
		sprintf(temp,"%8d",count);
		readJson["body"].Replace("EntrustNo",temp);
		//��������
		reply = (redisReply *)redisCommand(rc, "lpush autd %s", readJson.ToString().c_str());//��list�б��в���ί��������
		
		//sleep(3);
		printf("����:%d��\n", count);
		count++;
		freeReplyObject(reply);
	}

	return ;
}