/*
2018-12-07:
����ؽ�������̳��ַ�� http://kingstargold.cn:8080/goldblog/c/%E9%87%91%E5%B7%B4%E7%89%B9%E4%B8%AD%E5%BF%83%E5%8C%96%E4%BA%A4%E6%98%93%E6%89%80
����ؽ��������ߣ� ��ҫ
����ؽ������ֻ��� 15216631375
����ؽ�����qq����Ⱥ�ţ� 959372362
����ؽ������������䣺 myzhaoyao@126.com
*/

//���ļ���������������ݽṹ����ͨ�ú궨��
#ifndef __BASEDEFINE_H__
#define __BASEDEFINE_H__

//δ֪���� 
#define MSS_UNKNOW		"1"     //δ֪״̬������״̬����ֵ�ڲ�ȷ�������޷���ȡ��ʱ�򶼿���ʹ��δ֪����
//�������� ������:BuyOrSell
#define MSS_BUY			'b'     //��
#define MSS_SELL		's'     //��


//�г�״̬ ������:
#define MSS_MARKET_OPEN			'1'     //��ʼ
#define MSS_MARKET_PAUSE        '2'		//��ͣ����
#define MSS_MARKET_CLOSE		'3'     //����

//Ʒ�ִ��� 
#define MSS_VARIETY1             'autd'		//Au(T+D)��Լ
#define MSS_VARIETY2             'agtd'		//Ag(T+D)��Լ

//�����뷵����Ϣ���ձ�
#define MSS_SUCESS				"0000"			//���״���ɹ�
#define MSS_ERR_UNDEFINE    	"0001"  		//δ�������
#define MSS_CUSTNO_LENGTH_ERROR "0002"			//�ͻ��ų�������
#define MSS_ENTRUST_ERROR		"0003"			//ί�д���ʧ��

//�������Ҫ�صĳ���
#define MSS_CUSTNO_LEN          31+1			//���ͻ��ų���Ϊ31
#define MSS_ENTRUSTNO_LEN		31+1			//���ί�е��ų���Ϊ31
#define MSS_VATIETY_LEN			15+1			//ί��������Ʒ�ֳ���
#define MSS_MAX_SERIAL_NO       31+1			//��ˮ����󳤶�

//����ϵͳ�е����buf��С
#define MSS_MAX_BUF_LEN			8191+1			//ϵͳ���buf��СΪ8192

//���屾ϵͳ��json����key�ַ���ֵ��ֻ�б������ж����keyֵ�ſ�����
#define JSON_HEAD_STRING			"head"			//json�����head��ǩֵ
#define JSON_BODY_STRING			"body"			//json�����body��ǩֵ
#define JSON_BODY_RET_CODE			"RetCode"		//json�����з�����
#define JSON_BODY_RET_MSG			"RetMsg"		//json�����еķ�����Ϣ



//add by zyao 20181114 begin ��������ṹ�� 


///ί�е�Ҫ��
typedef struct _MSSEntrustField
{
	char CustNo[MSS_CUSTNO_LEN];		//�ͻ���	
	char EntrustNo[MSS_ENTRUSTNO_LEN];	//ί�е���
	char TradeDate[12];					//��������
	char TradeTime[12];					//����ʱ��
	char BuyOrSell[4];					//������־
	double Price;						//�۸�
	int Quantity;						//�������� Quantity = RemainQuantity + MatchQuantity
	int RemainQuantity;					//ʣ����������ί�б����ֳɽ�ʱʹ��RemainQuantity <= Quantity
	int MatchQuantity;					//�ѳɽ�����
	char Variety[MSS_VATIETY_LEN];		//ί��������Ʒ��
}S_ENTRUSTFIELD;

//ί�лر�Ҫ��
typedef struct _MSSEntrustReturn
{
	char CustNo[MSS_CUSTNO_LEN];		//�ͻ���	
	char EntrustNo[MSS_ENTRUSTNO_LEN];	//ί�е���
	char TradeDate[12];					//��������
	char TradeTime[12];					//����ʱ��
	char BuyOrSell[4];					//������־
	double Price;						//�۸�
	int Quantity;						//�������� Quantity = RemainQuantity + MatchQuantity
	int RemainQuantity;					//ʣ����������ί�б����ֳɽ�ʱʹ��RemainQuantity <= Quantity
	int MatchQuantity;					//�ѳɽ�����
	char Variety[MSS_VATIETY_LEN];		//ί��������Ʒ��
	char RetCode[8];					//������
	char RetMsg[2048];					//������Ϣ
}S_ENTRUST_RETURN;

//�ɽ��ر�Ҫ��
typedef struct _MSSDoneReturn
{
	char CustNo[MSS_CUSTNO_LEN];		//�ͻ���	
	char EntrustNo[MSS_ENTRUSTNO_LEN];	//ί�е���
	char TradeDate[12];					//��������
	char TradeTime[12];					//����ʱ��
	char BuyOrSell[4];					//������־
	double Price;						//�۸�
	int Quantity;						//�������� Quantity = RemainQuantity + MatchQuantity
	int RemainQuantity;					//ʣ����������ί�б����ֳɽ�ʱʹ��RemainQuantity <= Quantity
	int MatchQuantity;					//�ѳɽ�����
	char Variety[MSS_VATIETY_LEN];		//ί��������Ʒ��
	char DoneSerialNo[MSS_MAX_SERIAL_NO];//�������ɽ�����
}S_DONE_RETURN;


//�����ļ�Ҫ��
typedef struct _MSSCfgFile
{
	char ServerIp[64];
	int Port;
}S_CFG_FILE;


//end by zyao 20181114 begin ��������ṹ�� 
#endif //__BASEDEFINE_H__
