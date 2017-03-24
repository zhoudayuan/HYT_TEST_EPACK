#ifndef __DLL_COMMON_H__
#define __DLL_COMMON_H__
/**
 * Copyright (C), 2016-2020, Hytera Comm. Co., Ltd.
 * @file    DLL.h
 * @brief   包含数据链路层协议公共全局类型、变量、结构、宏定义
 * @author  陈禹良
 * @version v1.0
 * @date    2016-7-20
 */
/*
 *   修改历史
 *   2016-7-20                 陈禹良                     建立文件
 *   *************************************************************************/

/******************************************************************************
 *   引用头文件声明
 *   *************************************************************************/
#include "typedef.h"


/******************************************************************************
 *   宏定义
 *   *************************************************************************/
// 信道机系统模式
#define PDT_WORK_MODE            ((UINT8)0)       ///< PDT
#define DMR_WORK_MODE            ((UINT8)1)       ///< DMR

/**
 * @def  FPGA_SHM_DEV
 * @brief FPGA共享内存设备驱动
 */
#define FPGA_SHM_DEV           "/dev/axiram-config"

/**
 * @def  FPGA_SHM_ADDR
 * @brief FPGA共享内存首地址
 */
#define FPGA_SHM_ADDR          0xc0011000

/**
 * @def  FPGA_SHM_LEN
 * @brief FPGA共享内存长度
 */
#define FPGA_SHM_LEN           0x1000

/**
 * @def  NAS_KILL_FLAG
 * @brief NAS遥毙标示
 */
#define NAS_KILL_FLAG          0x12de7c53


/******************************************************************************
 *   枚举声明
 *   *************************************************************************/
/**
 * @enum DLL_INF_MSG_TYPE_E
 * @brief 链路层到接口层消息类型
 */
typedef enum _DLL_INF_MSG_TYPE_E
{
    DI_MSG_IDLE  = 0x00,                      ///< 空闲消息
    DI_MSG_VOICE = 0x01,                      ///< 语音消息
    DI_MSG_DATA  = 0x02,                      ///< 数据消息
    DI_MSG_NEAR  = 0x03,                      ///< 邻点消息
    DI_MSG_NM    = 0x04,                      ///< 网管消息
    DI_MSG_WLU   = 0x05,                      ///< WLU数据消息

} DLL_INF_MSG_TYPE_E;

/**
 * @enum SLOT_TYPE_E
 * @brief 时隙类型
 */
typedef enum _SLOT_TYPE_E
{
    S_1  = 0x00,                      ///< 1时隙
    S_2  = 0x01,                      ///< 2时隙

} SLOT_TYPE_E;

/**
 * @enum FREQUECY_TYPE_E
 * @brief 频率类型
 */
typedef enum _FREQUECY_TYPE_E
{
    F_1  = 0x00,                      ///< 1频率
    F_2  = 0x01,                      ///< 2频率

} FREQUECY_TYPE_E;

/**
 * @enum FRAME_TYPE_E
 * @brief 空口帧类型
 */
typedef enum _FRAME_TYPE_E
{
    FT_VOICE_NO   = 0,                      ///< 非语音帧标识
    FT_VOICE_A    = 1,                      ///< 语音A帧标识
    FT_VOICE_B    = 2,                      ///< 语音B帧标识
    FT_VOICE_C    = 3,                      ///< 语音C帧标识
    FT_VOICE_D    = 4,                      ///< 语音D帧标识
    FT_VOICE_E    = 5,                      ///< 语音E帧标识
    FT_VOICE_F    = 6,                      ///< 语音F帧标识

} FRAME_TYPE_E;

/**
 * @enum AI_DATA_TYPE_E
 * @brief 空口帧类型
 */
typedef enum _AI_DATA_TYPE_E
{
    DT_PI_HEADER        = 0,                      ///< PI头帧标识
    DT_LC_HEADER        = 1,                      ///< LC头帧标识
    DT_LC_TERMINATOR    = 2,                      ///< LC终结帧标识
    DT_CSBK             = 3,
    DT_MBC_HEADER       = 4,
    DT_MBC_CONTINUE     = 5,
    DT_DATA_HEADER      = 6,
    DT_R_1_2_DATA       = 7,
    DT_R_3_4_DATA       = 8,
    DT_IDLE             = 9,
    DT_R_1_1_DATA       = 10

} AI_DATA_TYPE_E;


/******************************************************************************
 *   结构体声明
 *   *************************************************************************/
/**
 * @struct DLL_PRINT_T
 * @brief  链表节点数据结构
 */
typedef struct _DLL_PRINT_T 
{ 
    UINT8 AIUp;  
    UINT8 AIDown;  
    UINT8 CCUp;
    UINT8 CCDown;
    UINT8 NegrSw;
    UINT8 NegrCyc;      //minutes
    UINT8 PrintLv;      //打印级别
    UINT8 FrqSlt;       //主动发射频点时隙
} DLL_PRINT_T;

/**
 * @struct CCL_PRINT_T
 * @brief  链表节点数据结构
 */
typedef struct _CCL_PRINT_T
{ 
    UINT8 CclUp;  
    UINT8 CcDown;  
    UINT8 DllUp;
    UINT8 CclDown;
} CCL_PRINT_T;
 
/**
 * @struct DLL_FPGA_SHM_T
 * @brief DLL和FPGA共享内存数据
 */
typedef struct _DLL_FPGA_SHM_T
{
    UINT8   FollowEn;                  ///< FPGA跟随使能0:idle, 1:follow
    UINT8   CallingST;                 ///< 主呼标示
    UINT8   EmbInfo[16];               ///< 语音内嵌信令32bit
    UINT8   MSCC;                      ///< 终端CC
    UINT8   WLUCC;                     ///< 链路机CC
    UINT8   GpsData[20];               ///< 设备GPS数据
    UINT8   LockFlag;                  ///< GPS锁定标示
    UINT8   StunFlag[4];               ///< 设备遥晕标示
    UINT8   uRev0[10];                 ///< 预留0
    UINT8   uRev1[52];                 ///< 预留1
        
} DLL_FPGA_SHM_T;
 
/**
 * @struct NAS_INF_DL_T
 * @brief 数据链路下行结构体
 */
typedef struct _DATA_LINK_T
{
    UINT8 MsgType;         ///< 0x01:语音 0x02:数据 0x03: 邻点0x04:网管
    UINT8 FrmType;         ///< 见枚举 _FrameType_E
    UINT8 CC:   4;         ///< 色码
    UINT8 PI:   1;         ///< 加密标示
    UINT8 LCSS: 3;         ///< lc start/stop 标示
    UINT8 DataType;        ///< 空口数据类型
    UINT8 EmbInfo[4];      ///< 空口语音内嵌
    UINT8 Vari[2];         ///< 方差FD 上行有效
    UINT8 uRev1[10];       ///< 预留1
    UINT8 DataLen;         ///< 负载长度
    UINT8 PayLoad[36];     ///< 负载数据
    UINT8 uRev2[3];        ///< 预留2
    
} DATA_LINK_T;
 
/**
 * @struct NAS_INF_DL_T
 * @brief 数据链路下行结构体
 */
typedef struct _NAS_INF_DL_T
{
    UINT32 TimeStamp;      ///< 下行时间戳 <2880000
    UINT8 SlotNum;         ///< 0x00:s1 0x01:s2
    UINT8 TxFreq1;         ///< 0x00:useless 0x01:useful
    UINT8 TxFreq2;         ///< 0x00:useless 0x01:useful
    UINT8 ResFrqSlot;      ///< Reserve Flag: Freq:H4bit Slot:L4bit  0:invalid 1/2:valid
    DATA_LINK_T tDataLink[2];

} NAS_INF_DL_T;

/**
 * @struct NAS_INF_UL_T
 * @brief 上行数据链路结构体
 */
typedef struct _NAS_INF_UL_T
{
    UINT32 TimeStamp;      ///< 上行时间戳 <2880000
    UINT8 SlotNum;         ///< 0x00:s1 0x01:s2
    UINT8 RxFreq1;         ///< 0x00:useless 0x01:useful
    UINT8 RxFreq2;         ///< 0x00:useless 0x01:useful
    UINT8 uRev;            ///< 预留
    DATA_LINK_T tDataLink[2];
    
} NAS_INF_UL_T;


#endif

