/*  
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/    
    
#include "hiredis.h"
#include "DataDispose.h"
#include "msslog.h"
#ifdef WIN32
#include<windows.h>
#endif

#include <set>    
#include <vector>    
#include <iostream>
#include "MssPubfunc.h"

using namespace std;    


CMssLog g_logfile;
S_CFG_FILE g_cfgfile;
CMssPubfunc g_pub;   //����ȫ�ֵ��õĻ���������

//create by zyao 2018-11-13 for trade match
//˵������ʱֻʵ�ֻ���redis����Ϣ����list�����ݴ�ϴ���
//����һ������redis����Ϣ���н������ݴ���                         2018-11-13����ʱֻʵ�ָ÷���
//������������socket����Ϣ���н������ݴ���,Ҳ���Ը��콫socket���ڽ������ݺ�push��redis�ڴ�⣬���ɴ�����洦��
//������������redis����Ϣ��������ģʽ��ȡ��������ϴ���
int main(int argc,char *argv[])    
{    
	g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"��ӭʹ��MSS������ϵͳ");
	//��ȡ�����ļ�
	CJsonPack jsonConfig;
	CJsonPack reqJson;
	memset(&g_cfgfile,0,sizeof(S_CFG_FILE));
	jsonConfig.FromFile("mssexchange.json");
	jsonConfig["redis"].Get("server_ip",g_cfgfile.ServerIp);
	jsonConfig["redis"].Get("server_port",g_cfgfile.Port);
	//����һ������redis����Ϣ���н������ݴ���
	int count = 1;
	redisReply *reply = NULL;
	CDataDispose myDispose;
	struct timeval timeout = {5, 0}; //5s�ĳ�ʱʱ��
	//redisContext* redisClient = redisConnect("127.0.0.1", 6379);//ͬredis����������
	redisContext *redisClient = (redisContext*)redisConnectWithTimeout(g_cfgfile.ServerIp, g_cfgfile.Port, timeout);
	if (redisClient == NULL || redisClient->err)
	{
		if (redisClient)
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"connect redis [%s][%d] error:%s",g_cfgfile.ServerIp,g_cfgfile.Port,redisClient->errstr);
		}
		else
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"connect redis [%s][%d] error: can't allocate redis context.",g_cfgfile.ServerIp,g_cfgfile.Port);
		}
		return -1;
	}
	else
	{
		g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"ȡredis������������redis�ɹ�!");
	}
	
	/*
	//���redis���ݿ����������룬��������ȷ���������뷽������ʹ��redis
	reply = (redisReply *)redisCommand(redisClient, "AUTH 123456");//����ͬredis�����õ����ӵ���֤����
	freeReplyObject(reply);
	*/
	
	reply = NULL;
	count = 1;
	while (1)
	{
		g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"�ȴ�redis��������");
		//��redis����list������ʽȡֵ��0��ʾ�ȴ�ֱ������Ϣ����0��ʾ�ȴ�X��Ȼ�󷵻�
		reply = (redisReply *)redisCommand(redisClient, "brpop autd 0");//��redis ����list������ʽ��ȡ����
		g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"�յ�redis�������ݿ�ʼ�����[%d]������",count);
		//reply = (redisReply *)redisCommand(redisClient, "get hello");//getִ�к�õ��Ľ������string����listִ��brpop�õ��Ľ����Ϊ����
		//REDIS_REPLY_STRING   1   //�����ַ������鿴str,len�ֶ�
		//REDIS_REPLY_ARRAY    2    //����һ�����飬�鿴elements��ֵ�������������ͨ��element[index]�ķ�ʽ��������Ԫ�أ�ÿ������Ԫ����һ��redisReply�����ָ��
		//REDIS_REPLY_INTEGER  3  //������������integer�ֶλ�ȡֵ
		//REDIS_REPLY_NIL      4      //û�����ݷ���
		//REDIS_REPLY_STATUS   5   //��ʾ״̬������ͨ��str�ֶβ鿴���ַ���������len�ֶ�
		//REDIS_REPLY_ERROR    6    //��ʾ�����鿴������Ϣ�����ϵ�str,len�ֶ�
		if (reply->type == REDIS_REPLY_ERROR)
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"redisCommand failure");
		}
		else if(reply->type == REDIS_REPLY_STRING)  //�������ַ���
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"get string data:%s\n",reply->str);
		}
		else if(reply->type == REDIS_REPLY_INTEGER) //����������ֵ
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"get int data:%lld",reply->integer);
		}
		else if(reply->type == REDIS_REPLY_ARRAY) //����һ�����飬�鿴elements��ֵ�������������ͨ��element[index]�ķ�ʽ��������Ԫ�أ�ÿ������Ԫ����һ��redisReply�����ָ��
		{
			for(unsigned int i = 0; i < reply->elements; i++)
			{
				if(reply->element[i]->type == REDIS_REPLY_STRING)
				{
					
					//�յ�mssjson��ʽ���Ŀ�ʼ���д�ϴ���
					if (i == 0)  //��i����1��ʱ�򣬵õ����ַ����Ƕ�����
					{
						g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"�յ����ݵ�redis������Ϊ[%s]",reply->element[i]->str);
					}
					else if(i == 1)
					{
						g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"�յ�������Ϊ[%s]",reply->element[i]->str);
						reqJson.Clear();
						string temp;
						bool flag = false;
						flag = reqJson.Parse(reply->element[i]->str);
						if (flag == false)
						{
							g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "json����ʧ��,��������[%s]", reply->element[i]->str);
							return -1;
						}
						int ret = myDispose.Dispose(reqJson);
						if (ret < 0)
						{
							g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "������[%s]����ʧ��", reqJson.ToFormattedString().c_str());
							//��ʱ��Ҫ��ʧ�ܻر�
							char RetCode[8] = {0};
							reqJson[JSON_BODY_STRING].Get(JSON_BODY_RET_CODE, RetCode);
						}
					}
				}
			}
		}
		freeReplyObject(reply);
		count++;
	}
	
	reply = NULL;
	redisFree(redisClient);
	return 0;
//������������socket����Ϣ���н������ݴ���
/*
    printf("pid: %d\n", getpid());    
    BFServer server(3);   //Ĭ�������߳��� 
    server.AddSignalEvent(SIGINT, BFServer::QuitCb);  //ע���˳��ź�   
    timeval tv = {10, 0};    
    server.AddTimerEvent(BFServer::TimeOutCb, tv, false);    
    server.SetPort(9998);    
    server.StartRun();    
    printf("done\n");    
  
    return 0; 
*/	
} 
