#ifndef __SIGTEST_H
#define __SIGTEST_H


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/ipc.h>      //shmget
#include <sys/msg.h>      //msgget
#include <sys/shm.h>      //shmget
#include <arpa/inet.h>    //inet_addr(); inet_ntoa()
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <errno.h>
#include "typedef.h"
#include "mgr_common.h"


/**
 * @def  DL
 * @brief 下行
 */
#define DL                       "-d"
/**
 * @def  UL
 * @brief 上行
 */
#define UL                       "-u"

/** @defgroup NAS测试项id */
/** @{ */
#define SHORT_MESSAGE            0      ///< 短消息
#define NAS_GPS_REPORT           1      ///< NAS GPS 上拉
#define MS_GPS_REPORT            2      ///< MS GPS 上拉
#define NEARID_INFO_QUERY        3      ///< 临点信息查询
#define NAS_DISABLE_IDT          4      ///< NAS 遥毙指示
#define MS_DISABLE_IDT           5      ///< MS 遥毙指示
#define NAS_ENABLE_IDT           6      ///< NAS 激活指示
#define MS_ENABLE_IDT            7      ///< MS 激活指示
#define NAS_STUN_IDT             8      ///< NAS 遥晕指示

#if 0
typedef  enum  _SMS_DATA_CALL_TYPE
{
    //短消息
    MESSAGE_PRIVATE_CALL    =0x00,    //单呼
    MESSAGE_GROUP_CALL      =0x01,    //组呼
    STATUS_PRIVATE_CALL     =0x02,    //状态单呼
    STATUS_GROUP_CALL       =0x03,    //状态组呼

    //命令
    STUN_REQ_MS             =0x07,    //遥晕MS
    STUN_REQ_NAS            =0x08,    //遥晕NAS
    KILL_REQ_NAS            =0x09,    //遥毙NAS
    GPS_REPORT_MS           =0x0a,    //拉GPS
    GPS_REPORT_NAS          =0x0b,
    REVIVE_REQ_NAS          =0x0c,
    REVIVE_REQ_MS           =0x0d,
    NEIGHBOR_QUERY          =0x0e,     //邻点查询

    //响应
    NEGHR_QUERY_ACK         =0x20,     //邻点查询相应   sms_send 格式回复
    GPS_REPORT_MS_ACK       =0x21,     //相应拉MS  GPS       CCL 先回ACK标识收到命令           然后  sms_send 格式回复数据信息
    GPS_REPORT_NAS_ACK      =0x22,     //相应拉NAS  GPS      CCL 先回ACK标识收到命令     sms_send 格式回复
    STUN_REQ_MS_ACK         =0x23,
    STUN_REQ_NAS_ACK        =0x24,
    KILL_REQ_NAS_ACK        =0X25,
    REVIVE_REQ_NAS_ACK      =0x26,     //激活NAS
    REVIVE_REQ_MS_ACK       =0x27,     //激活MS

    //告警
    DISCON_ALARM            =0x30,     //激活NAS    //DISCON_ALARM            =0x13,                       //断连告警
    DISCON_ALARM_CLEAR      =0x31,     //断连告警清除
    //增加临点上报

}SMS_DATA_CALL_TYPE;
#endif


/**
 * @struct MSG_DATA_DL_T
 * @brief 中心下行有效数据结构体
 */
typedef struct _MSG_DATA_DL_T
{
    UINT32 call_id;
    UINT32 voice_id;
    UINT8 source_stat;
    UINT8 sms_type;
    UINT16 valid_length;
    UINT8 sms_format;
    UINT8 sender_num[30];
    UINT8 receiver_num[30];
    UINT8 sms_data[200];
    UINT8 reserved[3];
} MSG_DATA_DL_T;


/**
 * @struct CC_CCL_DL_T
 * @brief 中心下行到呼叫控制层结构体
 */
typedef struct _CC_CCL_DL_T
{
    UINT16 Head;
    UINT16 Type;
    UINT16 Datalength;
    UINT16 Source_ID;
    UINT16 Dest_ID;
    UINT8 Source_Type;
    UINT8 Sig_Version;
    UINT8 Path_Num;
    UINT8 Ex_head[11];
    UINT32 Check_Val;
    MSG_DATA_DL_T MsgData;

} CC_CCL_DL_T;

#endif
