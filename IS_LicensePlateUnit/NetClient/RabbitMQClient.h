/*
����rabbitmq.4.dll��rabbitmq_send_client.cfg
�����./amqpĿ¼{amqp.h��amqp_framing.h��amqp_tcp_socket.h��msinttypes/stdint.h}��utilsĿ¼{utils.h}
�����./amqp/rabbitmq.4.lib

���ܶ��̵߳���RMQSC_Init ������
���Զ��̵߳���RMQSC_SendData
*/

#ifndef __rabbitmq_send_client_h__
#define __rabbitmq_send_client_h__


#ifdef __cplusplus
extern "C"{
#endif

	//��־�ȼ�����
#define		RMQSC_DEBUG_MODE			0
#define		RMQSC_INFO_MODE				1
#define		RMQSC_WARNING_MODE			2
#define		RMQSC_ERROR_MODE			3

	//����ֵ����
#define		RMQSC_OK					0
#define		RMQSC_ERROR					1
#define		RMQSC_NOINIT				2
#define		RMQSC_PARAMETER				3
#define		RMQSC_NETWORK				4
#define		RMQSC_NODENOTEXISTS			5

	//��ʼ�����������ṹ
	typedef struct {
		TCHAR		szIP[16];				//Rabbitmq������IP
		UINT16		nPort;					//Rabbitmq�������˿�
		TCHAR		szHostname[32];			//Rabbitmq��������¼��
		TCHAR		szHostpwd[32];			//Rabbitmq��������¼����
		TCHAR		szExchangeName[32];		//��������
		TCHAR		szRoutingName[32];		//·�ɼ� ���ڹ㲥ģʽ�¸�ֵ��Ч
	} RMQSendServerConfig;

	typedef struct {
		RMQSendServerConfig		rmqServer[16];	//���з������������ظ�
		UINT32		nServerCount;			//���õķ���������
		BOOL		bBroadcast;				//�㲥ģʽ
		BOOL		bDurable;				//�־û�
	} RMQSendInitConfig;

	//��ʼ��
	UINT32	__stdcall	RMQSC_Init(RMQSendInitConfig *pInitCfg, int nLogLevel);

	//����ʼ��
	void	__stdcall	RMQSC_Uninit();

	//��������
	UINT32	__stdcall	RMQSC_SendData(BYTE *pbyData, UINT32 nDataSize);

	//����RabbitMQ�ڵ��Ƿ������
	BOOL	__stdcall	RMQSC_TestConnection(TCHAR *pszIP, UINT32 nPort, TCHAR *pszHostName, TCHAR *pszHostPwd);

#ifdef __cplusplus
}
#endif


#endif