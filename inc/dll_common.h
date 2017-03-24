#ifndef __DLL_COMMON_H__
#define __DLL_COMMON_H__
/**
 * Copyright (C), 2016-2020, Hytera Comm. Co., Ltd.
 * @file    DLL.h
 * @brief   ����������·��Э�鹫��ȫ�����͡��������ṹ���궨��
 * @author  ������
 * @version v1.0
 * @date    2016-7-20
 */
/*
 *   �޸���ʷ
 *   2016-7-20                 ������                     �����ļ�
 *   *************************************************************************/

/******************************************************************************
 *   ����ͷ�ļ�����
 *   *************************************************************************/
#include "typedef.h"


/******************************************************************************
 *   �궨��
 *   *************************************************************************/
// �ŵ���ϵͳģʽ
#define PDT_WORK_MODE            ((UINT8)0)       ///< PDT
#define DMR_WORK_MODE            ((UINT8)1)       ///< DMR

/**
 * @def  FPGA_SHM_DEV
 * @brief FPGA�����ڴ��豸����
 */
#define FPGA_SHM_DEV           "/dev/axiram-config"

/**
 * @def  FPGA_SHM_ADDR
 * @brief FPGA�����ڴ��׵�ַ
 */
#define FPGA_SHM_ADDR          0xc0011000

/**
 * @def  FPGA_SHM_LEN
 * @brief FPGA�����ڴ泤��
 */
#define FPGA_SHM_LEN           0x1000

/**
 * @def  NAS_KILL_FLAG
 * @brief NASң�б�ʾ
 */
#define NAS_KILL_FLAG          0x12de7c53


/******************************************************************************
 *   ö������
 *   *************************************************************************/
/**
 * @enum DLL_INF_MSG_TYPE_E
 * @brief ��·�㵽�ӿڲ���Ϣ����
 */
typedef enum _DLL_INF_MSG_TYPE_E
{
    DI_MSG_IDLE  = 0x00,                      ///< ������Ϣ
    DI_MSG_VOICE = 0x01,                      ///< ������Ϣ
    DI_MSG_DATA  = 0x02,                      ///< ������Ϣ
    DI_MSG_NEAR  = 0x03,                      ///< �ڵ���Ϣ
    DI_MSG_NM    = 0x04,                      ///< ������Ϣ
    DI_MSG_WLU   = 0x05,                      ///< WLU������Ϣ

} DLL_INF_MSG_TYPE_E;

/**
 * @enum SLOT_TYPE_E
 * @brief ʱ϶����
 */
typedef enum _SLOT_TYPE_E
{
    S_1  = 0x00,                      ///< 1ʱ϶
    S_2  = 0x01,                      ///< 2ʱ϶

} SLOT_TYPE_E;

/**
 * @enum FREQUECY_TYPE_E
 * @brief Ƶ������
 */
typedef enum _FREQUECY_TYPE_E
{
    F_1  = 0x00,                      ///< 1Ƶ��
    F_2  = 0x01,                      ///< 2Ƶ��

} FREQUECY_TYPE_E;

/**
 * @enum FRAME_TYPE_E
 * @brief �տ�֡����
 */
typedef enum _FRAME_TYPE_E
{
    FT_VOICE_NO   = 0,                      ///< ������֡��ʶ
    FT_VOICE_A    = 1,                      ///< ����A֡��ʶ
    FT_VOICE_B    = 2,                      ///< ����B֡��ʶ
    FT_VOICE_C    = 3,                      ///< ����C֡��ʶ
    FT_VOICE_D    = 4,                      ///< ����D֡��ʶ
    FT_VOICE_E    = 5,                      ///< ����E֡��ʶ
    FT_VOICE_F    = 6,                      ///< ����F֡��ʶ

} FRAME_TYPE_E;

/**
 * @enum AI_DATA_TYPE_E
 * @brief �տ�֡����
 */
typedef enum _AI_DATA_TYPE_E
{
    DT_PI_HEADER        = 0,                      ///< PIͷ֡��ʶ
    DT_LC_HEADER        = 1,                      ///< LCͷ֡��ʶ
    DT_LC_TERMINATOR    = 2,                      ///< LC�ս�֡��ʶ
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
 *   �ṹ������
 *   *************************************************************************/
/**
 * @struct DLL_PRINT_T
 * @brief  ����ڵ����ݽṹ
 */
typedef struct _DLL_PRINT_T 
{ 
    UINT8 AIUp;  
    UINT8 AIDown;  
    UINT8 CCUp;
    UINT8 CCDown;
    UINT8 NegrSw;
    UINT8 NegrCyc;      //minutes
    UINT8 PrintLv;      //��ӡ����
    UINT8 FrqSlt;       //��������Ƶ��ʱ϶
} DLL_PRINT_T;

/**
 * @struct CCL_PRINT_T
 * @brief  ����ڵ����ݽṹ
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
 * @brief DLL��FPGA�����ڴ�����
 */
typedef struct _DLL_FPGA_SHM_T
{
    UINT8   FollowEn;                  ///< FPGA����ʹ��0:idle, 1:follow
    UINT8   CallingST;                 ///< ������ʾ
    UINT8   EmbInfo[16];               ///< ������Ƕ����32bit
    UINT8   MSCC;                      ///< �ն�CC
    UINT8   WLUCC;                     ///< ��·��CC
    UINT8   GpsData[20];               ///< �豸GPS����
    UINT8   LockFlag;                  ///< GPS������ʾ
    UINT8   StunFlag[4];               ///< �豸ң�α�ʾ
    UINT8   uRev0[10];                 ///< Ԥ��0
    UINT8   uRev1[52];                 ///< Ԥ��1
        
} DLL_FPGA_SHM_T;
 
/**
 * @struct NAS_INF_DL_T
 * @brief ������·���нṹ��
 */
typedef struct _DATA_LINK_T
{
    UINT8 MsgType;         ///< 0x01:���� 0x02:���� 0x03: �ڵ�0x04:����
    UINT8 FrmType;         ///< ��ö�� _FrameType_E
    UINT8 CC:   4;         ///< ɫ��
    UINT8 PI:   1;         ///< ���ܱ�ʾ
    UINT8 LCSS: 3;         ///< lc start/stop ��ʾ
    UINT8 DataType;        ///< �տ���������
    UINT8 EmbInfo[4];      ///< �տ�������Ƕ
    UINT8 Vari[2];         ///< ����FD ������Ч
    UINT8 uRev1[10];       ///< Ԥ��1
    UINT8 DataLen;         ///< ���س���
    UINT8 PayLoad[36];     ///< ��������
    UINT8 uRev2[3];        ///< Ԥ��2
    
} DATA_LINK_T;
 
/**
 * @struct NAS_INF_DL_T
 * @brief ������·���нṹ��
 */
typedef struct _NAS_INF_DL_T
{
    UINT32 TimeStamp;      ///< ����ʱ��� <2880000
    UINT8 SlotNum;         ///< 0x00:s1 0x01:s2
    UINT8 TxFreq1;         ///< 0x00:useless 0x01:useful
    UINT8 TxFreq2;         ///< 0x00:useless 0x01:useful
    UINT8 ResFrqSlot;      ///< Reserve Flag: Freq:H4bit Slot:L4bit  0:invalid 1/2:valid
    DATA_LINK_T tDataLink[2];

} NAS_INF_DL_T;

/**
 * @struct NAS_INF_UL_T
 * @brief ����������·�ṹ��
 */
typedef struct _NAS_INF_UL_T
{
    UINT32 TimeStamp;      ///< ����ʱ��� <2880000
    UINT8 SlotNum;         ///< 0x00:s1 0x01:s2
    UINT8 RxFreq1;         ///< 0x00:useless 0x01:useful
    UINT8 RxFreq2;         ///< 0x00:useless 0x01:useful
    UINT8 uRev;            ///< Ԥ��
    DATA_LINK_T tDataLink[2];
    
} NAS_INF_UL_T;


#endif

