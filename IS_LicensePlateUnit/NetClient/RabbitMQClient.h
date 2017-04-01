/*
依赖rabbitmq.4.dll、rabbitmq_send_client.cfg
需包含./amqp目录{amqp.h、amqp_framing.h、amqp_tcp_socket.h、msinttypes/stdint.h}和utils目录{utils.h}
需包含./amqp/rabbitmq.4.lib

不能多线程调用RMQSC_Init 即单例
可以多线程调用RMQSC_SendData
*/

#ifndef __rabbitmq_send_client_h__
#define __rabbitmq_send_client_h__


#ifdef __cplusplus
extern "C"{
#endif

	//日志等级定义
#define		RMQSC_DEBUG_MODE			0
#define		RMQSC_INFO_MODE				1
#define		RMQSC_WARNING_MODE			2
#define		RMQSC_ERROR_MODE			3

	//返回值定义
#define		RMQSC_OK					0
#define		RMQSC_ERROR					1
#define		RMQSC_NOINIT				2
#define		RMQSC_PARAMETER				3
#define		RMQSC_NETWORK				4
#define		RMQSC_NODENOTEXISTS			5

	//初始化函数参数结构
	typedef struct {
		TCHAR		szIP[16];				//Rabbitmq服务器IP
		UINT16		nPort;					//Rabbitmq服务器端口
		TCHAR		szHostname[32];			//Rabbitmq服务器登录名
		TCHAR		szHostpwd[32];			//Rabbitmq服务器登录密码
		TCHAR		szExchangeName[32];		//交换机名
		TCHAR		szRoutingName[32];		//路由键 当在广播模式下该值无效
	} RMQSendServerConfig;

	typedef struct {
		RMQSendServerConfig		rmqServer[16];	//其中服务器可以有重复
		UINT32		nServerCount;			//配置的服务器数量
		BOOL		bBroadcast;				//广播模式
		BOOL		bDurable;				//持久化
	} RMQSendInitConfig;

	//初始化
	UINT32	__stdcall	RMQSC_Init(RMQSendInitConfig *pInitCfg, int nLogLevel);

	//反初始化
	void	__stdcall	RMQSC_Uninit();

	//发送数据
	UINT32	__stdcall	RMQSC_SendData(BYTE *pbyData, UINT32 nDataSize);

	//测试RabbitMQ节点是否可连接
	BOOL	__stdcall	RMQSC_TestConnection(TCHAR *pszIP, UINT32 nPort, TCHAR *pszHostName, TCHAR *pszHostPwd);

#ifdef __cplusplus
}
#endif


#endif