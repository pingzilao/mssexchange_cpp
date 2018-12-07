/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/
// DataDispose.cpp: implementation of the CDataDispose class.
//
//////////////////////////////////////////////////////////////////////

#include "DataDispose.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//���캯��
CDataDispose::CDataDispose()
{
	m_listEntrustBuy.clear();
	m_listEntrustSell.clear();
	m_currentPrice = 0.00;
	memset(m_errMsg,0,sizeof(m_errMsg));

	//��ʼ��redis����
	struct timeval timeout = {5, 0}; //5s�ĳ�ʱʱ��
	m_redisClient = (redisContext*)redisConnectWithTimeout(g_cfgfile.ServerIp, g_cfgfile.Port, timeout);
	if (m_redisClient == NULL || m_redisClient->err)
	{
		if (m_redisClient)
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"connect redis error:%s",m_redisClient->errstr);
		}
		else
		{
			g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"connect error: can't allocate redis context.");
		}
		return ;
	}
	else
	{
		g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"��ϻر���������redis�ɹ�!");
	}
}

//��������
CDataDispose::~CDataDispose()
{
	if (m_redisClient != NULL)
	{
		redisFree(m_redisClient);
	}
}

//��������ַ�����
int CDataDispose::Dispose(CJsonPack &reqJson)
{
	//��������
	char TradeCode[8] = {0};
	int ret = -1;
	//cout << "��ӡ��������json��" << endl;
	//cout << readJson.ToFormattedString() << endl;
	//��һ������ȡ��ͷ��Ϣ
	memset(TradeCode,0,sizeof(TradeCode));
	reqJson["head"].Get("tradecode", TradeCode);
	//�ڶ��������ɸñ������Ψһ��ˮ��,д������json������
	reqJson["body"].Add("GlobalSerial", GetGlobalSerialNo());
	
	switch (atoi(TradeCode))
	{
	case 10001:
		ret = EntrustHandle(reqJson);
		break;
	default:
		sprintf(m_errMsg, "�޴˹���%d", atoi(TradeCode));
		g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "MSS����:%s", m_errMsg);
		return -1;
	}
	return ret;
}

//�����յ���ί������
int CDataDispose::EntrustHandle(CJsonPack &reqJson)
{
	S_ENTRUSTFIELD EntrustField;
	int ret = -1;
	//�ڶ�������ȡ����
	memset(&EntrustField, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson,&EntrustField);
	//��ʼ����ǰ��
	if ((m_currentPrice < -0.001) && (m_currentPrice > 0.001))  //double�������ݸ�0.00�Ƚϵķ���
	{
		m_currentPrice = EntrustField.Price; //��ǰ���¼�Ӧ�ð��յ�һ�ʳɽ���ȷ����������Ϊ�г���һ��ί��û�гɽ����԰Ѹ�ί�м۸���Ϊ���¼�
	}
	g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "����%s", m_errMsg);
	//�յ�ί�е�����ʼ��ϴ���
	ret = -1; //��ʼ��ret
	memset(m_errMsg, 0, sizeof(m_errMsg));
	ret = PreMatch(reqJson);
	if (ret < 0)
	{
		g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, m_errMsg);
		return -1;
	}
	g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "������洦������[%s]���", EntrustField.EntrustNo);
	return 0;
}

int CDataDispose::PreMatch(CJsonPack &reqJson)
{
	S_ENTRUSTFIELD newEntrust;
	//int ret = -1;
	//�ڶ�������ȡ����
	memset(&newEntrust, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson, &newEntrust);
	//��ί�������ʣ�������ͳɽ���������Ԥ����
	newEntrust.RemainQuantity = newEntrust.Quantity;	//ί�е��ĳ�ʼ״̬ʣ����������ί������
	newEntrust.MatchQuantity = 0;						//ί�е��ĳ�ʼ״̬�ɽ���������0

	//Ԥ����ί�лر�������ί�������ȸ�ί�лر�
	EntrustReturn(reqJson);
	//����Ԥ����
	if (MSS_BUY == newEntrust.BuyOrSell[0])				//�������ί��
	{
		if (m_listEntrustSell.empty())
		{
			int ret = -1;
			ret = AddEntrust2List(reqJson);
			if (ret < 0)
			{
				g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "ί������[%s]����ί���б�ʧ��", newEntrust.EntrustNo);
				reqJson["body"].CreateOrReplace(JSON_BODY_RET_MSG, "ί�д���ʧ��");
				reqJson["body"].CreateOrReplace(JSON_BODY_RET_CODE, MSS_ENTRUST_ERROR);
				return -1;
			}
			return 0;
		}
		if (newEntrust.Price < m_listEntrustSell.front()->Price) //��ǰί�м۸�С�ڵ�ǰ����������۸����ί�в��ᱻ�ɽ���ֱ�Ӽ��������
		{
			int ret = -1;
			ret = AddEntrust2List(reqJson);
			if (ret < 0)
			{
				g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "ί������[%s]����ί���б�ʧ��", newEntrust.EntrustNo);
				reqJson["body"].CreateOrReplace(JSON_BODY_RET_MSG, "ί�д���ʧ��");
				reqJson["body"].CreateOrReplace(JSON_BODY_RET_CODE, MSS_ENTRUST_ERROR);
				return -1;
			}
			else
			{
				//�ݲ�����
			}
		}
		else  //������Ҫ���д��
		{
			MatchEngine(reqJson);
		}
	}
	else if (MSS_SELL == newEntrust.BuyOrSell[0])
	{
		if (m_listEntrustSell.empty())
		{
			int ret = -1;
			ret = AddEntrust2List(reqJson);
			if (ret < 0)
			{
				g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "ί������[%s]����ί���б�ʧ��\n", newEntrust.EntrustNo);
				return -1;
			}
			return 0;
		}
		if (newEntrust.Price > m_listEntrustSell.front()->Price) //��ǰί�м۸�������б��е�ǰ���������۸����ί�в��ᱻ�ɽ���ֱ�Ӽ�����������
		{
			int ret = -1;
			ret = AddEntrust2List(reqJson);
			if (ret < 0)
			{
				g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "ί������[%s]����ί���б�ʧ��\n", newEntrust.EntrustNo);
				return -1;
			}
			else
			{
				//ί���������ί���б�ɹ���Ϊ��Ч�ʳɹ��ľͲ�����־��
			}
		}
		else  //������Ҫ���д��
		{
			MatchEngine(reqJson);
		}
	}
	else
	{
		g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "ί������[%s]������־[%s]У��ʧ��\n", newEntrust.EntrustNo, newEntrust.BuyOrSell);
		return -1;
	}
	return 0;
}


//��ί�м�¼����������б��м۸���ʵ�λ��
//����а��ռ۸�Ӵ�С��˳������m_listEntrustBuy
//�����а��ռ۸��С�����˳������m_listEntrustSell
int CDataDispose::AddEntrust2List(CJsonPack &reqJson)
{
	S_ENTRUSTFIELD newEntrust;
	
	//�ڶ�������ȡ����
	memset(&newEntrust, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson, &newEntrust);
	//�Լ۸�����жϣ������ί�м۸�С�ڵ�ǰ��
	if (MSS_BUY == newEntrust.BuyOrSell[0])   //��ί�е�����list�б�
	{
		auto it=m_listEntrustBuy.begin();
		for(; it != m_listEntrustBuy.end() ;it++)
		{
			if(newEntrust.Price > (*it)->Price)  //���ռ۸�Ӵ�С���򣬲��뵽С�ڸ�ί�н���Ԫ��֮ǰ����ȼ۸�֮��
			{
				m_listEntrustBuy.insert(it,&newEntrust);
				break;
			}
		}
	}
	else if(MSS_SELL == newEntrust.BuyOrSell[0])  //��ί�е�����list�б�
	{
		auto it=m_listEntrustSell.begin();
		for(; it != m_listEntrustSell.end() ;it++)
		{
			if(newEntrust.Price < (*it)->Price)  //���ռ۸��С�������򣬲��뵽���ڸ�ί�н���Ԫ��֮ǰ����ȼ۸�֮��
			{
				m_listEntrustSell.insert(it,&newEntrust);
				break;
			}
		}
	}
	else //��������δ֪�쳣����
	{
		g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"ί������[%s]��������δ֪,�޷������б�",newEntrust.EntrustNo);
		reqJson["body"].CreateOrReplace(JSON_BODY_RET_MSG, "ί�д���ʧ��");
		reqJson["body"].CreateOrReplace(JSON_BODY_RET_CODE, MSS_ENTRUST_ERROR);
		return -1;
	}
	return 0;
}
//add by zyao 20181114 ��Ϻ��Ĵ���ģ��
int CDataDispose::MatchEngine(CJsonPack &reqJson)
{
	S_ENTRUSTFIELD newEntrust;
	
	//�ڶ�������ȡ����
	memset(&newEntrust, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson, &newEntrust);
	if (MSS_BUY == newEntrust.BuyOrSell[0])  //�������ί��
	{
		for (auto SellListRecord:m_listEntrustSell) //�������۸��д�С
		{
			if (newEntrust.Price >= SellListRecord->Price)  //��ί�м۸���ڵ��ڵ�ǰ�����ڵ����ί�м۸�����Դ��
			{
				if (newEntrust.Quantity < SellListRecord->RemainQuantity) //���б��м۸����ȵĵ�һ�ʵ��������㹻�ɽ���ǰ��ί��
				{
					newEntrust.RemainQuantity = 0;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.Quantity;		//ί������ȫ���ܳɽ������Գɽ���������ί������
					SellListRecord->RemainQuantity = SellListRecord->RemainQuantity - newEntrust.Quantity;
					SellListRecord->MatchQuantity = SellListRecord->MatchQuantity + newEntrust.Quantity;
					//m_currentPrice = SellListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(newEntrust.Price,SellListRecord->Price);
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
				else if (newEntrust.Quantity == SellListRecord->RemainQuantity) //ί�����������б���׽ڵ���ȫ�ɽ�
				{
					newEntrust.RemainQuantity = 0;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.Quantity;		//ί������ȫ���ܳɽ������Գɽ���������ί������
					SellListRecord->RemainQuantity = SellListRecord->RemainQuantity - newEntrust.Quantity;	//��������ȫ�ɽ�,SellListRecord.RemainQuantity��ֵӦ�õ���0
					SellListRecord->MatchQuantity = SellListRecord->MatchQuantity + newEntrust.Quantity;		//��������ȫ�ɽ�,SellListRecord.MatchQuantity��ֵӦ�õ���SellListRecord.Quantity
					//m_currentPrice = SellListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(newEntrust.Price,SellListRecord->Price);
					//���б���׽ڵ㱻��ȫ�ɽ��ˣ�����Ӧ��ɾ���ýڵ�
					m_listEntrustSell.pop_front();
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
				else if (newEntrust.Quantity > SellListRecord->RemainQuantity)
				{
					newEntrust.RemainQuantity = newEntrust.RemainQuantity - SellListRecord->RemainQuantity;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.MatchQuantity + SellListRecord->RemainQuantity;	//�ɽ�ί����������ԭ�ѳɽ�����+��ǰ�ɽ�����
					SellListRecord->RemainQuantity = 0;					//��������ȫ�ɽ�,SellListRecord->RemainQuantity��ֵӦ�õ���0
					SellListRecord->MatchQuantity = SellListRecord->MatchQuantity + SellListRecord->RemainQuantity;		//�����ѳɽ�������MatchQuantity + ���γɽ�������RemainQuantity
					//m_currentPrice = SellListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(newEntrust.Price,SellListRecord->Price);
					//���б���׽ڵ㱻��ȫ�ɽ��ˣ�����Ӧ��ɾ���ýڵ�
					m_listEntrustSell.pop_front();
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
			}
			else  //��ί�м۸�С�ڵ�ǰ�ڵ�ļ۸�,�޷��ɽ���ʣ���ί�м�¼������ί���б���
			{
				int ret  = -1;
				ret = AddEntrust2List(reqJson);
				if (ret < 0)
				{
					g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"����ί������[%s]���б�ʧ��\n",newEntrust.EntrustNo);
					reqJson["body"].CreateOrReplace(JSON_BODY_RET_MSG, "ί�д���ʧ��");
					reqJson["body"].CreateOrReplace(JSON_BODY_RET_CODE, MSS_ENTRUST_ERROR);
					return -1;
				}
			}
		}
	}
	else if (MSS_SELL == newEntrust.BuyOrSell[0])  //����ί�е�����
	{
		for (auto BuyListRecord:m_listEntrustBuy) //�������۸���С����
		{
			if (newEntrust.Price <= BuyListRecord->Price)					//��ί�м۸�С�ڵ��ڵ�ǰ�����ڵ����ί�м۸�����Դ��
			{
				if (newEntrust.Quantity < BuyListRecord->RemainQuantity) //���б��м۸����ȵĵ�һ�ʵ��������㹻�ɽ���ǰ��ί��
				{
					newEntrust.RemainQuantity = 0;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.Quantity;		//ί������ȫ���ܳɽ������Գɽ���������ί������
					BuyListRecord->RemainQuantity = BuyListRecord->RemainQuantity - newEntrust.Quantity;
					BuyListRecord->MatchQuantity = BuyListRecord->MatchQuantity + newEntrust.Quantity;
					//m_currentPrice = BuyListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(BuyListRecord->Price,newEntrust.Price);
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
				else if (newEntrust.Quantity == BuyListRecord->RemainQuantity) //ί�����������б���׽ڵ���ȫ�ɽ�
				{
					newEntrust.RemainQuantity = 0;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.Quantity;		//ί������ȫ���ܳɽ������Գɽ���������ί������
					BuyListRecord->RemainQuantity = BuyListRecord->RemainQuantity - newEntrust.Quantity;	//��������ȫ�ɽ�,SellListRecord.RemainQuantity��ֵӦ�õ���0
					BuyListRecord->MatchQuantity = BuyListRecord->MatchQuantity + newEntrust.Quantity;		//��������ȫ�ɽ�,SellListRecord.MatchQuantity��ֵӦ�õ���SellListRecord.Quantity
					//m_currentPrice = BuyListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(BuyListRecord->Price,newEntrust.Price);
					//���б���׽ڵ㱻��ȫ�ɽ��ˣ�����Ӧ��ɾ���ýڵ�
					m_listEntrustSell.pop_front();
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
				else if (newEntrust.Quantity > BuyListRecord->RemainQuantity)
				{
					newEntrust.RemainQuantity = newEntrust.RemainQuantity - BuyListRecord->RemainQuantity;						//�㹻��ȫ�ɽ�����ʣ������Ϊ0
					newEntrust.MatchQuantity = newEntrust.MatchQuantity + BuyListRecord->RemainQuantity;	//�ɽ�ί����������ԭ�ѳɽ�����+��ǰ�ɽ�����
					BuyListRecord->RemainQuantity = 0;					//��������ȫ�ɽ�,SellListRecord->RemainQuantity��ֵӦ�õ���0
					BuyListRecord->MatchQuantity = BuyListRecord->MatchQuantity + BuyListRecord->RemainQuantity;		//�����ѳɽ�������MatchQuantity + ���γɽ�������RemainQuantity
					//m_currentPrice = BuyListRecord->Price;				//���õ�ǰ���¼�
					m_currentPrice = GetCurrentMatchPrice(BuyListRecord->Price,newEntrust.Price);
					//���б���׽ڵ㱻��ȫ�ɽ��ˣ�����Ӧ��ɾ���ýڵ�
					m_listEntrustSell.pop_front();
					//���ͳɽ��ر�
					DoneReturn(reqJson);
				}
			}
			else  //��ί�м۸���ڵ�ǰ�ڵ�ļ۸�,�޷��ɽ���ʣ���ί�м�¼������ί���б���
			{
				int ret  = -1;
				ret = AddEntrust2List(reqJson);
				if (ret < 0)
				{
					g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"����ί������[%s]���б�ʧ��",newEntrust.EntrustNo);
					reqJson["body"].CreateOrReplace(JSON_BODY_RET_MSG, "ί�д���ʧ��");
					reqJson["body"].CreateOrReplace(JSON_BODY_RET_CODE, MSS_ENTRUST_ERROR);
					return -1;
				}
			}
		}
	}
	return 0;
}

//���㵱ǰ�ɽ���
double CDataDispose::GetCurrentMatchPrice(double buyPrice,double salePrice)
{
	if(m_currentPrice<=salePrice)
	{
		m_currentPrice = salePrice;
	}
	else if(m_currentPrice>salePrice && m_currentPrice<=buyPrice)
	{
		m_currentPrice= m_currentPrice;
	}
	else if(m_currentPrice>buyPrice)
	{
		m_currentPrice= buyPrice;
	}
	else
	{
		m_currentPrice= m_currentPrice;
	}
	return m_currentPrice;
}

//��ί�лر�������redis������������ģ�鶩��Ȼ��ַ��������ͻ�����ʱ������ַ���
int CDataDispose::EntrustReturn(CJsonPack &reqJson)
{
	//��װί�лر�����
	S_ENTRUSTFIELD newEntrust;
	
	//�ڶ�������ȡ����
	memset(&newEntrust, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson, &newEntrust);
	//��һ��:����һ��json����
	CJsonPack entrustReturnJson;
	entrustReturnJson.AddEmptySubObject("head");
	entrustReturnJson["head"].Add("RetCode",		MSS_SUCESS);
	entrustReturnJson["head"].Add("RetMsg",			"ί�лر����Է�����Ϣ");
	entrustReturnJson.AddEmptySubObject("body");
	entrustReturnJson["body"].Add("CustNo",		newEntrust.CustNo);
	entrustReturnJson["body"].Add("EntrustNo",	newEntrust.EntrustNo);
	entrustReturnJson["body"].Add("TradeDate",	newEntrust.TradeDate);
	entrustReturnJson["body"].Add("TradeTime",	newEntrust.TradeTime);
	//entrustReturnJson["body"].Add("Price",		newEntrust.Price);
	entrustReturnJson["body"].Add("Variety",	newEntrust.Variety);
	redisReply *reply = NULL;
	reply = (redisReply *)redisCommand(m_redisClient, "PUBLISH %s %s", "entrust_return", entrustReturnJson.ToString().c_str());//����ί�лر�entrust_returnͨ������ʶΪ��entrust_return��countΪ���������ݿ����滻Ϊ���鱨��
	freeReplyObject(reply);
	reply = NULL;
	return 0;
}


//���ɽ��ر�������redis������������ģ�鶩��Ȼ��ַ��������ͻ�����ʱ������ַ���
int CDataDispose::DoneReturn(CJsonPack &reqJson)
{
	//��װί�лر�����
	S_ENTRUSTFIELD newEntrust;
	
	//�ڶ�������ȡ����
	memset(&newEntrust, 0, sizeof(S_ENTRUSTFIELD));
	GetValueToBuffer(reqJson, &newEntrust);
	//��һ��:����һ��json����
	CJsonPack entrustReturnJson;
	entrustReturnJson.AddEmptySubObject("head");
	entrustReturnJson["head"].Add("RetCode",		MSS_SUCESS);
	entrustReturnJson["head"].Add("RetMsg",			"�ɽ��ر����Է�����Ϣ");
	entrustReturnJson.AddEmptySubObject("body");
	entrustReturnJson["body"].Add("CustNo",		newEntrust.CustNo);
	entrustReturnJson["body"].Add("EntrustNo",	newEntrust.EntrustNo);
	entrustReturnJson["body"].Add("TradeDate",	newEntrust.TradeDate);
	entrustReturnJson["body"].Add("TradeTime",	newEntrust.TradeTime);
	entrustReturnJson["body"].Add("Price",		newEntrust.Price);//�ɽ��۸������Ǽ򵥴���ʵ�ʲ�һ�������ֵ
	entrustReturnJson["body"].Add("Variety",	newEntrust.Variety);
	//���ɳɽ����
	entrustReturnJson["body"].Add("DoneSerialNo",GetGlobalSerialNo());
	redisReply *reply = NULL;
	reply = (redisReply *)redisCommand(m_redisClient, "PUBLISH %s %s", "entrust_return", entrustReturnJson.ToString().c_str());//����ί�лر�entrust_returnͨ������ʶΪ��entrust_return��countΪ���������ݿ����滻Ϊ���鱨��
	freeReplyObject(reply);
	reply = NULL;
	return 0;
}


//��ȡȫ��Ψһ������ˮ�ţ����׷��Ч�ʿɽ�ί�гɽ��Ƚ��׸�����������ˮ�ŷֿ���������һ��ǰ׺��������
char* CDataDispose::GetGlobalSerialNo()
{
	static char cSerail[MSS_MAX_SERIAL_NO] = { 0 };
	redisReply *reply = NULL;
	reply = (redisReply *)redisCommand(m_redisClient, "INCR serailno");
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
		//g_logfile.Trace("mssexchange",__FILE__,__LINE__,0,"get int data:%lld",reply->integer);
		sprintf(cSerail, "%s%08lld", g_pub.Today(), reply->integer);
	}
	else 
	{
		g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, "��ȡȫ����ˮ������쳣");
	}
	
	freeReplyObject(reply);
	reply = NULL;
	//g_logfile.Trace("mssexchange", __FILE__, __LINE__, 0, cSerail);
	return cSerail;
}


//��ί������Ҫ��д��ṹ����
//2018-12-06����reqJson�����е�ҵ��Ҫ�ظ��Ƶ��ṹ��S_ENTRUSTFIELD��
int CDataDispose::GetValueToBuffer(CJsonPack &reqJson, S_ENTRUSTFIELD *newEntrust)
{
	//��ȡ����Ҫ��ֵ
	reqJson["body"].Get("CustNo", newEntrust->CustNo);
	reqJson["body"].Get("EntrustNo", newEntrust->EntrustNo);
	reqJson["body"].Get("BuyOrSell", newEntrust->BuyOrSell);
	reqJson["body"].Get("Price", newEntrust->Price);
	reqJson["body"].Get("Quantity", newEntrust->Quantity);
	reqJson["body"].Get("TradeTime", newEntrust->TradeTime);
	reqJson["body"].Get("TradeDate", newEntrust->TradeDate);
	reqJson["body"].Get("Variety", newEntrust->Variety);
	//��ֵ����Ч�Խ���У��
	if (strlen(newEntrust->CustNo) < 6)
	{
		reqJson["body"].CreateOrReplace("RetMsg", "�ͻ��ų���У������");
		reqJson["body"].CreateOrReplace("RetCode", MSS_CUSTNO_LENGTH_ERROR);
	}
	return 0;
}