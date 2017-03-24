#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <signal.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>    //inet_addr(); inet_ntoa()
#include <sys/shm.h>
#include <fcntl.h>

#include "mgr_common.h"
#include "dll_common.h"
/**
 * @var TEST
 * @brief测试
 */
// #define TEST  
/**
 * @var ROUTE_BUSY
 * @brief业务忙
 */
 #define ROUTE_BUSY 1
 /**
 * @var ROUTE_FREE
 * @brief业务空闲
 */
 #define ROUTE_FREE 0
  /**
 * @var ROUTE_DATA_LEN_MAX
 * @brief空口接受数据长度最大值
 */
 #define ROUTE_DATA_LEN_MAX 128
 /**
 * @def DB_NODE_MAX
 * @brief 内存池节点个数
 */
#define DB_NODE_MAX            (100)
/**
 * @def DB_NODE_MAX
 * @brief 内存池节点个数
 */
#define ROUTE_ERROR_NODE_MAX           (20)
/**
 * @struct semun
 * @brief 信号量
 */
union semun 
{
	int val;                ///< value for SETVAL 
	struct semid_ds *buf;   ///< buffer for IPC_STAT, IPC_SET
	unsigned short *array;  ///< array for GETALL, SETALL 
	struct seminfo *__buf;  ///< buffer for IPC_INFO
};
#define ROUTE_TEST_MSG_SERVER 10

/**
 * @ROUTE_PRINTF
 * @brief  链表节点数据结构
 */
typedef struct _ROUTE_PRINTF { 
 UINT8 uAIUp;  
 UINT8 uDown;  
 UINT8 uUpBusy;
 UINT8 uDownIr;
} ROUTE_PRINTF;
/**
 * @ROUTE_MSGBUF
 * @brief  链表节点数据结构
 */
typedef struct _ROUTE_MSGBUF { 
 long mtype;  
 UINT8 mtext[ROUTE_DATA_LEN_MAX];  
} ROUTE_MSGBUF;
/**
 * @ROUTE_DBC_NODE_T
 * @brief  链表节点数据结构
 */
typedef struct _ROUTE_DBC_NODE_T
{
	struct _ROUTE_DBC_NODE_T* ptNext;          ///< 后一节点
	UINT8               auData[ROUTE_DATA_LEN_MAX]; ///< 帧数据
} ROUTE_DBC_NODE_T;

/**
 * @struct INF_DLL_NM_T
 * @brief 网管消息,暂时不用
 */
typedef struct _INF_DLL_NM_T
{
    UINT8 Command;         ///命令字
    UINT8 NmType : 2;         ///< 0x00:s1 0x01:s2
    UINT8 OpCode :6;          ///< 0x00:f1 0x01:f2
    UINT8 SrcId;         ///源节点
    UINT8 DesId:   4;         ///目的节点
    UINT8 uRev[8];			///数据区
    UINT8 Crc;       ///< 预留1
} INF_DLL_NM_T;
/**
 * @struct INF_DLL_ODP_DATA_T
 * @brief 网管消息
 */
typedef struct _INF_DLL_ODP_DATA_T
{    
    void * pServer;			///业务数据链表
    void * pNm;       ///网管数据链表
    void * pAlarmEmergecy;       ///紧急告警
    void * pServerTail;///业务数据链表尾
    void * pNmTail;       ///网管数据链表尾
    void * pAlarmEmergecyTail;       ///网管数据链表尾
    UINT8 Status;         ///是否有业务数据
} INF_DLL_ODP_DATA_T;
/**
 * @struct ROUTE_DEBUG_DATA_T
 * @brief 本模块错误日志
 */
typedef struct _ROUTE_DEBUG_DATA_T
{    
	UINT32 Index;
    	UINT8 ErrorType;
	UINT8 Data[ROUTE_DATA_LEN_MAX];	
} ROUTE_DEBUG_DATA_T;

/**
 * @enum  ROUTE_AI_RECV_TYPE_E空口数据类型
 * @brief  用来获取内部端口号
 */
typedef enum _ROUTE_AI_RECV_TYPE_E
{
    ROUTE_AI_RECV_TYPE_CONFIG    = 0,//网管配置数据
    ROUTE_AI_RECV_TYPE_REPLY       = 1,//网管回复、告警数据
    ROUTE_AI_RECV_TYPE_SERVER        = 2,//业务数据
    ROUTE_AI_RECV_TYPE_CONFIG_SERVER    = 3,//网管配置数据和业务数据
    ROUTE_AI_RECV_TYPE_REPLY_SERVER       = 4,//网管回复、告警数据和业务数据
    ROUTE_AI_RECV_TYPE_CONFIG_REPLY    = 5,//网管配置数据
    ROUTE_AI_RECV_TYPE_MAX       = 6,//其他，不是本机数据抛掉
    ROUTE_AI_RECV_TYPE_ERROR       = 7//其他，数据错误
} ROUTE_AI_RECV_TYPE_E;
/**
 * @enum  ROUTE_SEND_TYPE_E空口发送数据类型
 * @brief  空口发送数据类型
 */
typedef enum _ROUTE_SEND_TYPE_E
{
    ROUTE_SEND_TYPE_SERVER    = 0,//
    ROUTE_SEND_TYPE_ALARM       = 1,//
    ROUTE_SEND_TYPE_NM        = 2,//   
    ROUTE_SEND_TYPE_INVALID        = 3//   
} ROUTE_SEND_TYPE_E;
/**
 * @enum  ROUTE_SEND_PORT_TYPE_E
 * @brief  
 */
typedef enum _ROUTE_SEND_PORT_TYPE_E
{
    ROUTE_SEND_TYPE_PORT_CONFIG    = 0x0001,//
    ROUTE_SEND_TYPE_PORT_REPLY      = 0x0010,//
    ROUTE_SEND_TYPE_PORT_SERVER    = 0x0100,//    
    ROUTE_SEND_TYPE_PORT_INVALID  = 0x1000//   
} ROUTE_SEND_PORT_TYPE_E;
/**
 * @enum  ROUTE_ERROR_DEBUG_TYPE_E
 * @brief  错误日志类型
 */
typedef enum _ROUTE_ERROR_DEBUG_TYPE_E
{
    ROUTE_ERROR_DEBUG_TYPE_RECV_LEN    = 0,//
    ROUTE_ERROR_DEBUG_TYPE_SEND_LEN       = 1,//
    ROUTE_ERROR_DEBUG_TYPE_NO_BUFF        = 2,//   
    ROUTE_ERROR_DEBUG_TYPE_INVALID        = 3,//   
    ROUTE_ERROR_DEBUG_TYPE_ALARM        = 4,//   
} ROUTE_ERROR_DEBUG_TYPE_E;
/**
 * @enum  ROUTE_RECV_F1F2_TYPE_E
 * @brief  F1F2状态
 */
typedef enum _ROUTE_RECV_F1F2_TYPE_E
{
    ROUTE_RECV_F1F2_NO_TYPE    = 0,//
    ROUTE_RECV_F1F2_F1_TYPE       = 1,//
    ROUTE_RECV_F1F2_F2_TYPE        = 2,//
    ROUTE_RECV_F1F2_ALL_TYPE       = 3,//
} ROUTE_RECV_F1F2_TYPE_E;
