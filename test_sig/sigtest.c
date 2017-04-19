/*
//    printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);

����-1�� �ɷ��滻Ϊ�����ļ��е�ö��
#define CC_SEND_VOICE_PORT              (10011)         // (6017)   //
#define CC_SEND_SIG_PORT                (10013)         //(6018)
����-2��
AMBE_FILE_NAME   �Ƿ��Ƕ������ļ�
����-3
delay(35);  ��ʱ���ٺ���
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
//����
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>
#include "sigtest.h"
#include "global.h"


 //---����---
#define CC_SEND_VOICE_PORT         (10011)         // (6017)   //
#define CC_SEND_SIG_PORT           (10013)         //(6018)
//---����-����---
#define AMBE_FILE_NAME             "AMBE_Test.ap"
#define NVOC_FILE_NAME             "NVOC_Test.ap"
#define AMBE_FORMAT                 1
#define NVOC_FORMAT                 2
#define POCKET_BYTE_MAX             1000    // ������������
#define POCKET_BYTE_MIN             200     // ��������С����
#define BIG_ENDIAN_TEST             1
#define LITTLE_ENDIAN_TEST          0
#define FLC_LEN                     9



/** @defgroup CSBK������ */
/** @{ */
#define PRE_CSBKO               0x3d    //111101
#define REQ_CSBKO               0x29    //101001
#define ACK_CSBKO               0x2c    //101100
#define ALARM_CSBKO             0x37    //110111
/** @} */

/** @defgroup ����ҵ�������� */
/** @{ */
#define CALL_ALERT_SSO          0x05
#define EN_RADIO_SSO            0x09
#define DIS_RADIO_SSO           0x0a
#define DIGITAL_ALARM_SSO       (0x0f-0x01) // ��ʱNASѡ��0X0E





/**
 * @def  RTP_MAX_DATA_SIZE
 * @brief  �����������س���
 */
#define RTP_MAX_DATA_SIZE           28
#define OPTION_PRESS                 1
#define OPTION_FEATURE               2
//---����-�ַ�������---
#define OK                           0
#define NO_INPUT                     1
#define TOO_LONG                     2


#define GET_SUCCEED                  0
#define TOO_LONG_ERR                -1
#define NO_INPUT_ERR                -2
#define NOT_DIGIT_ERR               -3
#define CR_ERR                      -4

#define MSG_GET_OK                   0
#define MSG_TOO_LONG_ERR            -1
#define MSG_NO_INPUT_ERR            -2

#define  MS_ID_SIZE         3
#define  NAS_ID_SIZE        1
#define  GROUP_ID_SIZE      MS_ID_SIZE

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
 * @enum FRAME_TYPE_E
 * @brief �տ�֡����
 */
typedef enum _FRAME_TYPE_E
{
    FT_VOICE_NO   = 0,                         ///< ������֡��ʶ
    FT_VOICE_A    = 1,                         ///< ����A֡��ʶ
    FT_VOICE_B    = 2,                         ///< ����B֡��ʶ
    FT_VOICE_C    = 3,                         ///< ����C֡��ʶ
    FT_VOICE_D    = 4,                         ///< ����D֡��ʶ
    FT_VOICE_E    = 5,                         ///< ����E֡��ʶ
    FT_VOICE_F    = 6,                         ///< ����F֡��ʶ

} FRAME_TYPE_E;


typedef  enum  _DATA_TEST_ITEM
{
    ITEM_MS_REMOTE_KILL  = 0,
    ITEM_MS_ENABLE       = 1,
    ITEM_MS_GPS_REPORT   = 2,
    ITEM_NAS_DISABLE     = 3,
    ITEM_NAS_ENABLE      = 4,
    ITEM_NAS_GPS_REPORT  = 5,
    ITEM_NAS_STUN        = 6,
    ITEM_NEIGHBOR_QUERY  = 7,
    ITEM_SHORT_MESSAGE   = 8,
    ITEM_AlARM_REQ       = 9,
    ITEM_MAX             = 10
} DATA_TEST_ITEM;



/**
 * @struct  SUPS_CSBK_PDU
 * @brief  ����ҵ��CSBK
 */
typedef struct _SUPS_CSBK_PDU
{
    UINT8 uCSBKO:6;
    UINT8 uPF  :1;
    UINT8 uLB  :1;
    UINT8 uFID;
    UINT8 uSSO;
    UINT8 uREV;
    UINT8 auTADDR[3];
    UINT8 auSADDR[3];
    UINT8 auCRC[2];
} SUPS_CSBK_PDU;




/******************************************************************************
 *   ö������
 *   *************************************************************************/


/**
 * @enum CCL_DATA_TYPE_E
 * @brief CCL����֡����
 */
typedef enum _CCL_DATA_TYPE_E
{
    //�տ���������DC_MSG_DATA
    CT_JUNK_DATA            = 0x00,                      ///< ��������Ϣ
    CT_PI_HEADER            = 0x01,                      ///< PIͷ֡��ʶ
    CT_LC_HEADER            = 0x02,                      ///< LCͷ֡��ʶ
    CT_LC_TERMINATOR        = 0x03,                      ///< LC�ս�֡��ʶ
    CT_PACKET_DATA          = 0x04,                      ///< ����Ϣ

    CT_GPS_REPORT_REQ_MS    = 0x11,                      ///< ��̨GPS����
    CT_GPS_REPORT_ACK_MS    = 0x12,                      ///< ��̨GPS����
    CT_DISABLE_REQ_MS       = 0x13,                      ///< ҡ��
    CT_DISABLE_ACK_MS       = 0x14,                      ///< ҡ��
    CT_ENABLE_REQ_MS        = 0x15,                      ///< �ն˼���
    CT_ENABLE_ACK_MS        = 0x16,                      ///< �ն˼���

    //��·����������DC_MSG_WLU
    CT_GPS_REPORT_REQ_NAS   = 0x21,                      ///< GPS����
    CT_GPS_REPORT_ACK_NAS   = 0x22,                      ///< GPS����
    CT_STUN_REQ_NAS         = 0x23,                      ///< ҡ��
    CT_STUN_ACK_NAS         = 0x24,                      ///< ҡ��
    CT_KILL_REQ_NAS         = 0x25,                      ///< ҡ��
    CT_KILL_ACK_NAS         = 0x26,                      ///< ҡ��
    CT_ENABLE_REQ_NAS       = 0x27,                      ///< WLU  ����
    CT_ENABLE_ACK_NAS       = 0x28,                      ///< WLU  ����
    CT_STUN_RPT_NAS         = 0x29,                      ///< WLU  ң���ϱ�
    CT_KILL_RPT_NAS         = 0x2a,                      ///< WLU  ң���ϱ�
    CT_ENABLE_RPT_NAS       = 0x2b,                      ///< WLU  �����ϱ�

    CT_NEGHR_QUERY          = 0x31,                      ///< �ٵ���Ϣ��ѯ
    CT_NEGHR_QUERY_ACK      = 0x32,                      ///< �ٵ���Ϣ��Ӧ
    CT_NEGHR_REPORT         = 0x33,                      ///< �ٵ���Ϣ�ϱ�
    CT_DISCON_ALARM         = 0x34,                      ///< �����澯
    CT_DISCON_ALARM_CLEAR   = 0x35,                      ///< �����澯���

    CT_DISCON_HANDMI_ALARM  = 0x36,                      ///< ����澯

} CCL_DATA_TYPE_E;


//---����---
// malloc �ڴ�
typedef struct _CACHE_T
{
    char *Buff;
    int len;
} CACHE_T;

/**
 * @struct _CENTER_CMD_SHAREHEADER
 * @brief  ���뵥Ԫ���������������ͷ
 */
typedef struct  _CENTER_CMD_SHARE_HEAD
{
    unsigned short SigHead;             // ��ʶAC�ӿ�����ͷ��־(0x13ec)
    unsigned short SigType;             // �������� ��ʶAC�ӿ�PTT�������0x000a��
    unsigned short Datalength;          // ��Ч���ݳ��� ��ʶ������Ч���ݳ��ȣ�100��
    unsigned short SourceID;            // Դ��Ԫ�� ��ʶAC�ӿڷ��͵�ԪIDֵ
    unsigned short DestID;              // Ŀ�ĵ�Ԫ�� ��ʶAC�ӿڽ��յ�ԪIDֵ
    unsigned char  SourceType;          // Դ��Ԫ�豸���� ��ʶ���͵�Ԫ�豸���ͣ���ʽ��
    unsigned char  SigVersion;          // ����汾�� ��ʶ�������ʺϰ汾��0x02(��������0x01�汾������)
    unsigned char  PathNum;             // ͨ������ ��ʶ���������ڸõ�Ԫ��ͨ�����룬��������ͨ��������ȫF
    unsigned char  Exhead[11];          // ��չͷ ��չͷ����������Ĳ���
    unsigned int   CheckVal;            // У��ֵ ��ʶ����У����(01�汾 ��02�汾 Ŀǰ����дȫF����ʾ��У�鷽ʽ)
} CENTER_CMD_SHARE_HEAD ;





/**
 * @struct  _PTT_CMD
 * @brief  PTT-ON  PTT-OFF  ����ṹ��
 */
typedef struct _PTT_CMD
{
    CENTER_CMD_SHARE_HEAD  SharedHead;
    unsigned int  CallId;               // ������ID ��ʶһ�κ��еĿ�����ID
    unsigned int  VoiceId;              // ҵ����ID ��ʶһ�κ��е�ҵ����ID
    unsigned char PttStat;              // PTT����״̬  ��ʶPTT����״̬������CMD_ON=0x06���ͷ�CMD_OFF=0x07��
    unsigned char Reserved;             // �ֽڶ���ռλ����������Ĳ���
    unsigned char TalkingNum[30];       // ��ʶ����ͨ��PTT�Ľ���������
    unsigned char CallingNum[30];       // ��ʶ���κ��е����к���
    unsigned char CalledNum[30];        // ��ʶ���κ��еı��к��루��š����ţ�
} PTT_CMD;


/**
 * @struct  _Rtp_Ex_Head
 * @brief RTP ��չͷ
 */
typedef struct _Rtp_Ex_Head
{
     unsigned short profile_head;
     unsigned short exhead_length;      // 32bit number == 4 bytes number
     unsigned int   voice_id;
     unsigned int   gps;
     unsigned char  codec_flag;
     unsigned char  frame_flag;
     unsigned char  path_num;           // show which path voice 0xff indicates invalid value, "0" indicates the first path
     unsigned char  pack_rate;          // packet rate "20" or "60" milliseconds
     unsigned char  encrypt_flag;
     unsigned char  encode_flag;
     unsigned char  encode_frame;
     unsigned char  enconde_rate;
} Rtp_Ex_Head;



/**
 * @struct  _Rtp_Fix_Head
 * @brief RTP �̶�ͷ
 */

typedef struct   _Rtp_Fix_Head
{
    unsigned char   version:2;
    unsigned char   padding_flag:1;
    unsigned char   ex_head_flag:1;
    unsigned char   csrc_count:4;
    unsigned char   marker:1;
    unsigned char   payload_type:7;
    unsigned short  sequence_num;
    unsigned int    time_stamp;
    unsigned int    ssrc;
} Rtp_Fix_Head;

/**
 * @struct   _CENTER_VOICE_DATA
 * @brief �����������ݰ���ʽ
 */
typedef struct _CENTER_VOICE_DATA
{
    Rtp_Fix_Head    Rtp_Head;
    Rtp_Ex_Head     Rtp_Ex;
    unsigned char   Payload[RTP_MAX_DATA_SIZE];
    unsigned char   Reserve_Data[5];
} CENTER_VOICE_DATA;



/**
 * @struct DLL_CCL_UL_T
 * @brief ������·���е�CCL�ṹ��
 */
typedef struct _DLL_CCL_UL_T
{
    UINT8 MsgType;                      ///< 0x01:���� 0x02:���� 0x03:�ڵ�
    UINT8 FrmType;                      ///< ��ö�� _FrameType_E
    UINT8 DataType;                     ///< ��������
    UINT8 SrcId[3];                     ///< ԴID
    UINT8 DstId[3];                     ///< Ŀ��ID
    UINT8 Vari[2];                      ///< ����FD ������Ч
    UINT8 uRev0[8];                     ///< Ԥ��0
    UINT8 DataLen;                      ///< ���س���
    UINT8 PayLoad[500];                 ///< ��������

} DLL_CCL_UL_T;


typedef void (*pITEM_TEST_F)();
typedef void (*pITEM_TEST_INIT)(int item);
typedef struct _DATA_ITEM_FUN_T
{
    DATA_TEST_ITEM item;
    pITEM_TEST_INIT pFunItem;
    pITEM_TEST_F pFunOdp;
    const char *token;
    const char *ItemName;
} DATA_ITEM_FUN_T;



//---����---

static char *s_ProcessName;
static unsigned char s_DstRcv[3];
static unsigned char s_CallingID[3];
static unsigned char s_CalledID[3];
static unsigned char s_GroupID[3];
static int s_Message_len;
static char s_SrcDev;
static char s_DstDev;



//---����-ipc-Socke--
static int s_tCenterVoiceSockfd;
static struct sockaddr_in s_tCenterVoiceAddr;
static int s_tCenterSigSockfd;                  // cc2ccl send VoiceData socket
static struct sockaddr_in s_tCenterSigAddr;     // cc2ccl send VoiceData addr
static int CCLSocket;                           // �������ݸ� CCL
struct sockaddr_in CclAddr;                     // ����CCL�˿�
static int SigSocket;
struct sockaddr_in CclSigAddr;
socklen_t UDPSize  = sizeof(struct sockaddr_in);
socklen_t AddrSize = sizeof(struct sockaddr_in);

//---����-�߳�---
static pthread_t ODP_VoicePid;                  // cc2ccl ��������ѹ������
static pthread_t ODP_DataPid;                   // cc2ccl �������й��ܲ���
static pthread_t ODP_DataRcvPid;                // cc2ccl �������й��ܲ���


//---����---
static void ShowHelp();
unsigned int get_dst_U32(const char *name, const char *prmpt, int min, int max);  // ���ú���
static void delay(unsigned long msec);
static void ProcessNameSave(char *name);
static char *ProcessNameGet();
static void TestInit();
static int GetRandDigit();
static void Pause();
static int IsBigEndian();
static unsigned int U32Change2BigEndian(unsigned int data);
static int get_value_u32(const char *name, const char *prmpt, unsigned int *pDstID);
static int get_src_dst_id(const char *name, char *SrcDev, char *DstDev);
static int getLine (const char *prmpt, char *buff, int sz);
static int digit_get(const char *prmpt, char *buff, int sz, int *pDigit);
static unsigned long get_file_size(const char *path);
static void set_item_ID_input(int item, unsigned char *pBuff, char SrcDev, char DstDev);



//---����-ipc-Socke--
static void IPC_SocketVoiceSoundCc2cclInit();
static void IPC_SocketVoiceSigCc2cclInit();
static void IPC_SocketVoiceDll2CclInit();
static void IPC_SocketDataCc2cclInit();
static void IPC_Ccl2ccInit();

//---����-����---
static void TestPress();
static int TestOption(int argc, char *argv[]);
//---����-��������--
static int TestDataCc2ccl();
static int VoiceCc2cclFormatGet();
static int VoiceCc2cclLenGet();
static int DataCc2CclItemGet();
//---����-��������-----
static void IDP_SendCclData(DLL_CCL_UL_T * ptCclData);

//---����-����
static void SetVoiceID();
static void TestVoiceDll2ccl();
static void TestVoiceCc2ccl();
static void VoiceCc2cclPptOn();
static void VoiceCc2cclSend(int len, int format);
static void VoiceCc2cclPptOff();
static void PressVoiceCc2cclSend();
static void VoiceDdl2CclSend(int len, int format);
static void TestVoiceDll2ccl();
static void VoiceDdl2CclData(unsigned char DataType);
static int  VoiceDdl2CclLenGet();

//---����-�߳�--
static void *PressODPDataTask(void * p);
static void *PressODPVoiceTask(void * p);
static void PressVoiceTaskCreate();
static void PressDataTaskCreate();
static void DataCc2CclIStart(int item);
static void TestDataCheckTaskCreate();
static void *TestDataCheckTask(void * p);


//---����-���ܲ���-�������--
static void ODP_Alarm_req(void);                //9
static void ODP_ShortMessage(void);             //8
static void ODP_NearNodeInfoQuery(void);        //7
static void ODP_NasStunIdt(void);               //6
static void ODP_NasGpsReport(void);             //5
static void ODP_NasEnableIdt(void);             //4
static void ODP_NasDisableIdt(void);            //3
static void ODP_MsGpsReport(void);              //2
static void ODP_MsEnableIdt(void);              //1
static void ODP_MsDisableIdt(void);             //0
//---����-���ܲ���-init--
static void Ms_ItemHandler(int item);
static void Nas_ItemNasHandler(int item);
static void MS_GroupMsgHandler(int item);
static void MS_GroupAlarmHandler(int item);      // 9


// GPS
static void PrintFormatGpsInfo(SMS_INFO_S *SmsInfo);



DATA_ITEM_FUN_T atDataItemFun[] = {

    {ITEM_MS_REMOTE_KILL , Ms_ItemHandler        , ODP_MsDisableIdt      , "MS",     "MS Remote Kill"     },   // 0
    {ITEM_MS_ENABLE      , Ms_ItemHandler        , ODP_MsEnableIdt       , "MS",     "Ms Enable"          },   // 1
    {ITEM_MS_GPS_REPORT  , Ms_ItemHandler        , ODP_MsGpsReport       , "MS",     "Ms Gps Report"      },   // 2
    {ITEM_NAS_DISABLE    , Nas_ItemNasHandler    , ODP_NasDisableIdt     , "Nas",    "Nas Disable"        },   // 3
    {ITEM_NAS_ENABLE     , Nas_ItemNasHandler    , ODP_NasEnableIdt      , "Nas",    "Nas Enable"         },   // 4
    {ITEM_NAS_GPS_REPORT , Nas_ItemNasHandler    , ODP_NasGpsReport      , "Nas",    "Nas Gps Report"     },   // 5
    {ITEM_NAS_STUN       , Nas_ItemNasHandler    , ODP_NasStunIdt        , "Nas",    "Nas Stun"           },   // 6
    {ITEM_NEIGHBOR_QUERY , Nas_ItemNasHandler    , ODP_NearNodeInfoQuery , "Nas",    "Nas Neighbor Query" },   // 7
    {ITEM_SHORT_MESSAGE  , MS_GroupMsgHandler    , ODP_ShortMessage      , "Group",  "Ms Short Message"   },   // 8
    {ITEM_AlARM_REQ      , MS_GroupAlarmHandler  , ODP_Alarm_req         , "Group",  "Alarm req"          },   // 9
    {ITEM_MAX            , NULL                  , NULL                  ,  NULL,     NULL                }    // 10
};




//---��������---



void ShowHelp()
{
    char *name = NULL;
    if ((name = ProcessNameGet()) == NULL)
    {
        printf("\nerr: NO name\n");
        exit(EXIT_FAILURE);
    }


    printf("\n\
    Usage:\n\
    %s  [-vduphVDUPH]\n\n\
    explain:\n\
        -v -- voice test, cc to ccl\n\
        -d -- data  test, cc to ccl\n\
        -g -- data  test, ccl to cc \n\
        -u -- voice test, dll to ccl\n\
        -p -- pressure test\n\
        -h -- help\n\
    \n\n\
    \n", name);
}


// �жϴ�˻���С��
int IsBigEndian()
{
    union _NUM
    {
        unsigned int  a;
        unsigned char b;
    } num;
    num.a = 0x12345678;
    if (num.b == 0x12)
    {
        return BIG_ENDIAN_TEST;
    }
    return LITTLE_ENDIAN_TEST;
}

// ת��Ϊ��ϴ洢
unsigned int U32Change2BigEndian(unsigned int data)
{
    unsigned int val = data;
    if (IsBigEndian() == LITTLE_ENDIAN_TEST)
    {
        val = (                                 \
            ( (data & (0XFF<<(8*3)))  >>  8*3 )|\
            ( (data & (0XFF<<(8*2)))  >>  8*1 )|\
            ( (data & (0XFF<<(8*1)))  <<  8*1 )|\
            ( (data & (0XFF<<(8*0)))  <<  8*3 ) \
        );
    }
    return val;
}





/*
 * name:   ��ǰ��������
 * prmpt:  �ַ���������ʾ
 * pDstID: ����������յ�ת�������buff������ASCIIת�������δ���ñ�����
 * buff:   �ڲ�ת���ã������������ASCII��,
 *         ע��:
 *               32λ���εķ�Χ��0~0x7fffffff(2147483647)
 *               2147483647 �Ǹ�10λ���֣�����buff�趨11���ֽڣ�
 *               ��Ϊ
 *                   buff �洢ASCII��  2147483647 + '\0' һ����Ҫ12���ֽ�, digit_get �Զ�ȥ��'\n'
 *
 *
*/
int get_value_u32(const char *name, const char *prmpt, unsigned int *pDstID)
{
    char buff[11];
    while (1)
    {
        printf("In fun:[%s], input now:\n\n", name);
        if (digit_get(prmpt, buff, sizeof(buff), (int *)pDstID) == GET_SUCCEED)
        {
            break;
        }
    }
    return *pDstID;
}


int get_src_dst_id(const char *name, char *SrcDev, char *DstDev)
{
    char buff[3]; // ���ֵ31, stdin�洢��buff������: '3'+'1'+'\n'+'\0', ����digit_get�ɽ� '\n'ȥ���������������ĳ���Ϊ3
    int digit;
    int ret;

    printf("please input the SrcDev ID and DstDev ID to test...\n");
    while(1)
    {
        printf("[%s]...\nPlease Inpur Srcdev Value (0~31):\n\n", name);
        if ((ret = digit_get("Enter Src ID>", buff, sizeof(buff), &digit)) == GET_SUCCEED)
        {
            if (digit >= 0 && digit <= 31 )
            {
                *SrcDev = digit;
                break;
            }
            printf("\nThe Value Range Err\n");
        }
    }

    while(1)
    {
        printf("\n\nPlease Inpur DstDev Value (0~31):\n\n");
        if ((ret = digit_get("Enter Dst ID>", buff, sizeof(buff), &digit)) == GET_SUCCEED)
        {
            if (digit <= 31 && digit >= 0)
            {
                *DstDev = digit;
                break;
            }
            printf("\nThe Value Range Err\n");
        }
    }
    return 0;
}


/**
 * @brief ����׼�����ַ����洢��buff
 * @param [in] prmpt  �ַ�����ʾ��Ϣ
 * @param [in] buff   ���뻺��
 * @param [in] sz     ���뻺�泤��
 * @param [out] NO_INPUT
 * @param [out] TOO_LONG
 * @param [out] OK;
 * @author  �ܴ�Ԫ
 * @since   trunk.00001
 * @bug
 */
int getLine(const char *prmpt, char *buff, int sz)
{
    int ch, extra;

    if (prmpt != NULL)
    {
        // ��ʾ������ʾ
        printf("%s", prmpt);
        fflush(stdout);
    }
    // �ַ�������
    if (fgets(buff, sz, stdin) == NULL)
    {
        printf("NO_INPUT\n");
        return NO_INPUT;
    }
    // �ж�buff���Ƿ������ַ�ȫ���洢
    if (buff[strlen(buff)-1] != '\n')
    {
        // û��ȫ���洢
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
        {
            extra = 1;
        }
        // ���� TOO_LONG ˵����������ݳ������泤��
        return (extra == 1) ? TOO_LONG : OK;
    }
    //  ȫ���洢, ��'\n'�滻Ϊ'\0'
    buff[strlen(buff)-1] = '\0';
    return OK;
}



/**
 * @brief ����׼�����ַ����洢��buff, ����ʾ����ԭ��
 * @param [in] prmpt  �ַ�����ʾ��Ϣ
 * @param [in] buff   ���뻺��
 * @param [in] sz     ���뻺�泤��
 * @param [in/out]    �������ַ���ת��Ϊ���δ洢�� pDigit
 * @param [out] TOO_LONG
 * @param [out] OK;
 * @author  �ܴ�Ԫ
 * @since   trunk.00001
 * @bug
 */
int digit_get(const char *prmpt, char *buff, int sz, int *pDigit)
{
    char *pbuff = buff;
    int ret, ch;
    ret = getLine(prmpt, buff, sz);
    if (ret == TOO_LONG)
    {
        printf("\n\nInput Number Is Too Long, Please Try Again\n\n");
        return TOO_LONG_ERR;
    }
    else if (ret == NO_INPUT)
    {
        printf ("\n\nNo Input, Please Try Again\n\n");
        return NO_INPUT_ERR;
    }
    else if (ret == OK)
    {
        while ((ch = *pbuff++) != '\0')
        {
            if (isdigit(ch) == 0)
            {
                printf("\n\nInput Is Not Digit, Please Try Again\n\n");
                return NOT_DIGIT_ERR;
            }
        }

        if (buff[0] == '\0')
        {
            printf("\n\nInput Data Is only Enter, Try Again\n\n");
            return CR_ERR;
        }
        *pDigit = atoi(buff);
    }
    return  GET_SUCCEED;
}


// ��ͣ���ȴ�����س�
static void Pause()
{
    int ch;
    printf("\n\nPause!!! please press [Enter] key to continue...\n\n\n");
    while (((ch = getchar()) != '\n') && (ch != EOF));
}


// �������ȡ����
int GetRandDigit()
{
    static int sRandFlag = 0;
    if (sRandFlag == 0)
    {
        srand((unsigned int)time(NULL));
        sRandFlag = 1;
    }
    return rand();
}


// ������ʱ����
void delay(unsigned long msec)
{
    struct timeval tv;
    tv.tv_sec  = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;

    switch (select(0, NULL, NULL, NULL, &tv))
    {
        case -1:
        {
            printf("delay Error!\n");
            break;
        }
        case 0:
        {
            break;
        }
        default:
        {
            printf("delay default\n");
            break;
        }
    }
}



// ����������
void ProcessNameSave(char *name)
{
    s_ProcessName = name;
}

// ��������ȡ
char *ProcessNameGet()
{
    return (s_ProcessName == NULL )? NULL :s_ProcessName;
}



// ��ȡ�ļ��ֽ���
// ʧ�ܷ���-1
unsigned long get_file_size(const char *path)
{
    struct stat statbuff;
    return (stat(path, &statbuff) < 0) ? -1: statbuff.st_size;
}


// ����ͨ���׽��ֳ�ʼ��
void IPC_SocketVoiceSoundCc2cclInit()
{

    s_tCenterVoiceSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_tCenterVoiceSockfd  < 0)
    {
          printf("[%s:%d] err: creat CC voice sound socket\n", __FUNCTION__, __LINE__);
          perror("socket>");
          exit(EXIT_FAILURE);
    }

    //  CC2CCL, CCL�˿ڳ�ʼ��  send
    bzero(&s_tCenterVoiceAddr, sizeof(s_tCenterVoiceAddr));
    s_tCenterVoiceAddr.sin_family = AF_INET;
//  s_tCenterVoiceAddr.sin_addr.s_addr =htonl(INADDR_ANY);  // htonl(INADDR_ANY);  //inet_addr("127.0.0.1");//���ﲻһ�� , ��ϲ��д��
    s_tCenterVoiceAddr.sin_addr.s_addr =inet_addr("127.0.0.1");//���ﲻһ��
    s_tCenterVoiceAddr.sin_port = htons(CC_SEND_VOICE_PORT);
}



void IPC_SocketVoiceSigCc2cclInit()
{
    s_tCenterSigSockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (s_tCenterSigSockfd < 0)
    {
          printf("[%s:%d] err: creat CC voice sig socket\n", __FUNCTION__, __LINE__);
          perror("socket>");
          exit(EXIT_FAILURE);
    }

    bzero(&s_tCenterSigAddr, sizeof(s_tCenterSigAddr));
    s_tCenterSigAddr.sin_family = AF_INET;
    s_tCenterSigAddr.sin_addr.s_addr =inet_addr("127.0.0.1");       //���ﲻһ��
    s_tCenterSigAddr.sin_port = htons(CC_SEND_SIG_PORT);

}



void IPC_SocketVoiceDll2CclInit()
{
    CCLSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (CCLSocket < 0)
    {
        printf("[%s:%d] err: creat Dll2Cc voice socket\n", __FUNCTION__, __LINE__);
        perror("socket>");
        exit(EXIT_FAILURE);
    }
    bzero(&CclAddr, sizeof(struct sockaddr_in));
    CclAddr.sin_family = AF_INET;
    CclAddr.sin_port = htons(SOCK_PORT_DLL_CCL);
    CclAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    return;
}




void IPC_SocketDataCc2cclInit()
{
    SigSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (SigSocket < 0)
    {
        printf("[%s:%d] err: creat SigSocket\n", __FUNCTION__, __LINE__);
        perror("socket>");
        exit(EXIT_FAILURE);
    }
    // cc2ccl
    bzero(&CclSigAddr, sizeof(struct sockaddr_in));
    CclSigAddr.sin_family = AF_INET;
    CclSigAddr.sin_port = htons(SOCK_PORT_CC_CCL_S);
    CclSigAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
}


// ccl2cc
static int s_CCsigSocket;
static struct sockaddr_in g_Ccl2ccSigAddr;
static void IPC_Ccl2ccInit();
// ccl2cc
void IPC_Ccl2ccInit()
{

    s_CCsigSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (s_CCsigSocket < 0)
    {
        printf("[%s:%d] err: creat s_CCsigSocket\n", __FUNCTION__, __LINE__);
        perror("socket>");
        exit(EXIT_FAILURE);
    }

    // ccl2cc
    bzero(&g_Ccl2ccSigAddr, sizeof(struct sockaddr_in));
    g_Ccl2ccSigAddr.sin_family = AF_INET;
    g_Ccl2ccSigAddr.sin_port = htons(SOCK_PORT_CCL_CC_S);
    g_Ccl2ccSigAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(s_CCsigSocket, (struct sockaddr *)(&g_Ccl2ccSigAddr), sizeof(struct sockaddr_in)) == -1)
    {
        printf("CC: Socket Binds Error : %s\n\a", strerror(errno));
        perror("socket>");
        exit(1);
    }
}



void VoiceCc2cclPptOn()
{
    unsigned char SndBuf[512];
    PTT_CMD *ptSndBuf = (PTT_CMD *)SndBuf;
    unsigned short SndLen;
//    struct timeval  sigstart;
//    struct timezone tz;
    memset(ptSndBuf, 0, sizeof(ptSndBuf));
    ptSndBuf->SharedHead.SigHead = 0xec13;
    ptSndBuf->PttStat = 0x06;              //PTT mingling 0X06
    ptSndBuf->SharedHead.SigType = 0x000a; //PTT_ON
    ptSndBuf->CallingNum[0] = s_CallingID[0];
    ptSndBuf->CallingNum[1] = s_CallingID[1];
    ptSndBuf->CallingNum[2] = s_CallingID[2];
    ptSndBuf->CalledNum[0]  = s_CalledID[0];
    ptSndBuf->CalledNum[1]  = s_CalledID[1];
    ptSndBuf->CalledNum[2]  = s_CalledID[2];
    SndLen = sendto(s_tCenterSigSockfd, ptSndBuf, sizeof(PTT_CMD), 0, (struct sockaddr *)&s_tCenterSigAddr, sizeof(s_tCenterSigAddr));
    printf("PTT on sendlen =%d\n",SndLen);
//  gettimeofday(&sigstart, &tz);
//  printf("ptt on time: %d s, %d us \n",  (int)sigstart.tv_sec, (int)sigstart.tv_usec);
}





void VoiceCc2cclPptOff()
{
    unsigned char SndBuf[512];
    PTT_CMD *ptSndBuf = (PTT_CMD *)SndBuf;
    unsigned short SndLen;
//    struct timeval    sigstop;
//    struct timezone   tz;
    memset(ptSndBuf, 0, sizeof(PTT_CMD));
    ptSndBuf->SharedHead.SigHead = 0xec13;
    ptSndBuf->PttStat = 0x07;               //PTT mingling
    ptSndBuf->SharedHead.SigType = 0x000a;  //PTT_ON
    ptSndBuf->CallingNum[0] = s_CallingID[0];
    ptSndBuf->CallingNum[1] = s_CallingID[1];
    ptSndBuf->CallingNum[2] = s_CallingID[2];
    ptSndBuf->CalledNum[0]  = s_CalledID[0];
    ptSndBuf->CalledNum[1]  = s_CalledID[1];
    ptSndBuf->CalledNum[2]  = s_CalledID[2];
    SndLen = sendto( s_tCenterSigSockfd,  ptSndBuf, sizeof(PTT_CMD), 0, (struct sockaddr *)&s_tCenterSigAddr, sizeof(s_tCenterSigAddr)); // @@����ʲô��˼
    printf("PTT OFF sendlen =%d\n",SndLen);
//  gettimeofday(&sigstop,&tz);
//  printf("ptt off time: %d s, %d us\n", (int)sigstop.tv_sec, (int)sigstop.tv_usec);
}





/**
 * @brief   ����MS ң��ָʾ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_MsDisableIdt(void)         // 0
{
    printf("Ms Disable idte download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = STUN_REQ_MS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
    CcSMessage.MsgData.sender_num[0] = 0x00;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x01;
    CcSMessage.MsgData.receiver_num[0] = s_DstRcv[0];
    CcSMessage.MsgData.receiver_num[1] = s_DstRcv[1];
    CcSMessage.MsgData.receiver_num[2] = s_DstRcv[2];
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}



/**
 * @brief   ����MS ����ָʾ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_MsEnableIdt(void)      // 1
{
    printf("Ms enable idte download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = REVIVE_REQ_MS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
    CcSMessage.MsgData.sender_num[0] = 0x00;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x01;
    CcSMessage.MsgData.receiver_num[0] = s_DstRcv[0];
    CcSMessage.MsgData.receiver_num[1] = s_DstRcv[1];
    CcSMessage.MsgData.receiver_num[2] = s_DstRcv[2];
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}



/**
 * @brief   ����MS GPS��������
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_MsGpsReport(void)      // 2
{
    printf("Ms gps report download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = GPS_REPORT_MS;
    CcSMessage.MsgData.valid_length = 4;
    CcSMessage.MsgData.sms_format = 0x01;
    CcSMessage.MsgData.sender_num[0] = 0x00;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x01;
    CcSMessage.MsgData.receiver_num[0] = s_DstRcv[0];
    CcSMessage.MsgData.receiver_num[1] = s_DstRcv[1];
    CcSMessage.MsgData.receiver_num[2] = s_DstRcv[2];
    UINT8 data[4] = {0x0a, 0x00, 0x00, 0x8f};
    data[1] = s_DstRcv[0];
    data[2] = s_DstRcv[1];
    data[3] = s_DstRcv[2];
    memcpy(CcSMessage.MsgData.sms_data, data, 8);
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}






/**
 * @brief   ����NAS ң��ָʾ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_NasDisableIdt(void)        // 3
{
    printf("Nas Disable idte download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = KILL_REQ_NAS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
//  CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//  CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}




/**
 * @brief   ����NAS ����ָʾ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
static void ODP_NasEnableIdt(void)   // 4
{
    printf("Nas enable idte download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = REVIVE_REQ_NAS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
//    CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//    CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}




/**
 * @brief   ����NAS GPS��������
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_NasGpsReport(void)   //5
{
    printf("Nas gps report download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = GPS_REPORT_NAS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
//    CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//    CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    // ����ѹ�����ǹ��ܲ���
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}



/**
 * @brief   ����NAS ң��ָʾ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_NasStunIdt(void)       // 6
{
    printf("Nas stun idte download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = STUN_REQ_NAS;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
//    CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//    CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}




/**
 * @brief   �����ٵ���Ϣ��ѯ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_NearNodeInfoQuery(void)        // 7
{
    printf("Near Node info query download \n");
    CC_CCL_DL_T CcSMessage;

    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = NEIGHBOR_QUERY;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;
//    CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//    CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}





/**
 * @brief   ���ж���Ϣ����
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
void ODP_ShortMessage(void)     //8
{

    int i;
    int pDigit_len;
    const char *pDigit= "0123456789ABCDEF";  // 16�ַ�
    unsigned char *pSms_data;
    CC_CCL_DL_T CcSMessage;

    printf("short message download \n");
    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;

    memset(CcSMessage.Ex_head, 0xff, 11);

    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = MESSAGE_GROUP_CALL;

    CcSMessage.MsgData.valid_length = s_Message_len*2;
    CcSMessage.MsgData.sms_format = 0x01;
    CcSMessage.MsgData.sender_num[0] = 0x00;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x01;

    CcSMessage.MsgData.receiver_num[0] = s_GroupID[0];
    CcSMessage.MsgData.receiver_num[1] = s_GroupID[1];
    CcSMessage.MsgData.receiver_num[2] = s_GroupID[2];

    CcSMessage.MsgData.sms_data[0] = 'h';
    CcSMessage.MsgData.sms_data[1] = 0x00;
    CcSMessage.MsgData.sms_data[2] = 'y';
    CcSMessage.MsgData.sms_data[3] = 0x00;
    CcSMessage.MsgData.sms_data[4] = 't';
    CcSMessage.MsgData.sms_data[5] = 0x00;
    CcSMessage.MsgData.sms_data[6] = 'e';
    CcSMessage.MsgData.sms_data[7] = 0x00;
    CcSMessage.MsgData.sms_data[8] = 'r';
    CcSMessage.MsgData.sms_data[9] = 0x00;
    CcSMessage.MsgData.sms_data[10] = 'a';
    CcSMessage.MsgData.sms_data[11] = 0x00;
    CcSMessage.MsgData.sms_data[12] = '1';
    CcSMessage.MsgData.sms_data[13] = 0x00;
    CcSMessage.MsgData.sms_data[14] = '2';
    CcSMessage.MsgData.sms_data[15] = 0x00;
    CcSMessage.MsgData.sms_data[16] = '3';
    CcSMessage.MsgData.sms_data[17] = 0x00;

    CcSMessage.MsgData.valid_length = s_Message_len*2;

    pSms_data = CcSMessage.MsgData.sms_data;
    memset(pSms_data, 0, s_Message_len*2);  //��Ч���ͳ��ȡ�UTF-16 ռ�������ֽ�
    pDigit_len = strlen(pDigit);

    for (i = 0; i < s_Message_len ; i++)
    {
        pSms_data[i*2] = pDigit[i % pDigit_len];
    }

    pSms_data = &CcSMessage.MsgData.sms_data[0];

    printf("Msg_len=(%d)\n", s_Message_len);

    printf("Msg_buf:\n");
    for (i = 0; i < s_Message_len*2; i++)
    {
        printf("\t(%d)=(%c)-(%#04x)\n", i, pSms_data[i], pSms_data[i]);
    }

    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);
}



void ODP_Alarm_req(void)     //9
{

    printf("Alarm download \n");
    CC_CCL_DL_T CcSMessage;
//    SUPS_CSBK_PDU *pSUPS_CSBK_PDU = NULL;
    CcSMessage.Head = htons(0x13ec);
    CcSMessage.Type = 0x000e;
    CcSMessage.Datalength = sizeof(MSG_DATA_DL_T);
    CcSMessage.Source_ID = 0xffff;
    CcSMessage.Dest_ID = 0xffff;
    CcSMessage.Source_Type = 0xff;
    CcSMessage.Sig_Version = 0x02;
    CcSMessage.Path_Num = 0xff;
    CcSMessage.Check_Val = 0xffffffff;
    memset(CcSMessage.Ex_head, 0xff, 11);
    CcSMessage.MsgData.call_id = 0x0001;
    CcSMessage.MsgData.voice_id = 0xffff;
    CcSMessage.MsgData.source_stat = 0;
    CcSMessage.MsgData.sms_type = MS_ALARM;
    CcSMessage.MsgData.valid_length = 0;
    CcSMessage.MsgData.sms_format = 0x01;

    CcSMessage.MsgData.sender_num[0] = 0x00;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x01;
    CcSMessage.MsgData.receiver_num[0] = s_GroupID[0];
    CcSMessage.MsgData.receiver_num[1] = s_GroupID[1];
    CcSMessage.MsgData.receiver_num[2] = s_GroupID[2];
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);


#if 0
//  CcSMessage.MsgData.sender_num[0] = SrcDev;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.sender_num[1] = 0x00;
    CcSMessage.MsgData.sender_num[2] = 0x00;
//  CcSMessage.MsgData.receiver_num[0] = DstDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    CcSMessage.MsgData.receiver_num[1] = 0x00;
    CcSMessage.MsgData.receiver_num[2] = 0x00;
    CcSMessage.MsgData.sender_num[0] = s_SrcDev;
    CcSMessage.MsgData.receiver_num[0] = s_DstDev;
    sendto(SigSocket, &CcSMessage, sizeof(CC_CCL_DL_T), 0, (struct sockaddr *)(&CclSigAddr), UDPSize);


    // ��������д�ⲿ��
    pSUPS_CSBK_PDU = (SUPS_CSBK_PDU *)CcSMessage.MsgData.sms_data;

    pSUPS_CSBK_PDU->uLB    = 1;
    pSUPS_CSBK_PDU->uPF    = 0;
    pSUPS_CSBK_PDU->uCSBKO = ALARM_CSBKO;
    pSUPS_CSBK_PDU->uFID   = 0x68;
    pSUPS_CSBK_PDU->uSSO   = DIGITAL_ALARM_SSO;

    pSUPS_CSBK_PDU->auTADDR[0]= s_GroupID[0]
    pSUPS_CSBK_PDU->auTADDR[1]= s_GroupID[1]
    pSUPS_CSBK_PDU->auTADDR[2]= s_GroupID[2]


#endif









#if 0
    /**
    * @struct  SUPS_CSBK_PDU
    * @brief  ����ҵ��CSBK
    */
    typedef struct _SUPS_CSBK_PDU
    {
        UINT8 uCSBKO:6;
        UINT8 uPF  :1;
        UINT8 uLB  :1;
        UINT8 uFID;
        UINT8 uSSO;
        UINT8 uREV;
        UINT8 auTADDR[3];
        UINT8 auSADDR[3];
        UINT8 auCRC[2];
    } SUPS_CSBK_PDU;
#endif



}




// ��̨����׼��
void Ms_ItemHandler(int item)
{
    char name[100];
    unsigned int MsDst, dst = 0;
    unsigned char *pBuff = (unsigned char *)&dst;
    snprintf(name, sizeof(name), "%d-%s", item, atDataItemFun[item].ItemName);
    while (1)
    {
        printf("please input the dst ID 0~16777215(0xFFFFFF) to test\n");
        dst = get_value_u32(name, "dst ID>", &dst);
        if (dst >= 0  && dst <= 16777215)
        {
            break;
        }
        printf("the val is not in range\n\n");
    }
    // dst ��ֵ����ɾ
    MsDst = dst;
    dst = U32Change2BigEndian(dst) >> 8;
    set_item_ID_input(item, pBuff, 0, 0);
    printf("\nSend:\n\tNas ---------------------> MsDst(%d)\n", MsDst);
    Pause();
    atDataItemFun[item].pFunOdp();
}


// ��·������׼��
void Nas_ItemNasHandler(int item)
{
    char name[100];
    char SrcDev, DstDev;
    snprintf(name, sizeof(name), "%d-%s", item, atDataItemFun[item].ItemName);
    get_src_dst_id(name, &SrcDev, &DstDev);
    set_item_ID_input(item, NULL, SrcDev, DstDev);
    printf("\nSend:\n\tSrc(%d) ---------------------> Dst(%d)\n", SrcDev, DstDev);
    Pause();
    atDataItemFun[item].pFunOdp();
}



void MS_GroupMsgHandler(int item)      // 8
{
    // ��ID
    char name[100];
    unsigned int Group, dst = 0;
    unsigned int len = 0;
    unsigned char *pBuff = (unsigned char *)&dst;
    snprintf(name, sizeof(name), "%d-%s", item, atDataItemFun[item].ItemName);
    while (1)
    {
        printf("please input the group ID 0~16777215(0xFFFFFF) to test\n");
        dst = get_value_u32(name, "group ID>", &dst);
        if (dst >= 0  && dst <= 16777215)
        {
            printf("you get the group_ID %d(%#010x)\n\n", dst, dst);
            break;
        }
        printf("the msg group_ID is not in range\n\n");
    }
    Group = dst;
    dst = U32Change2BigEndian(dst) >> 8;
    set_item_ID_input(item, pBuff, 0, 0);

    // ������
    while (1)
    {
        printf("please input the packet len 1~100(valid date 0~31) to test \n");
        len = get_value_u32(name, "packet len>", &dst);
        if (len > 0  && len <= 100)
        {
            break;
        }
        printf("the the msg len is not in range\n\n");
    }

    s_Message_len = len;
    printf("\nSend:\n\tNas ---------------------> Group(%d)\n", Group);
    Pause();
    atDataItemFun[item].pFunOdp();  // ODP_ShortMessage();
}


void MS_GroupAlarmHandler(int item)  //  9
{

    // ��ID
    char name[100];
    unsigned int Group, dst = 0;
    unsigned char *pBuff = (unsigned char *)&dst;
    snprintf(name, sizeof(name), "%d-%s", item, atDataItemFun[item].ItemName);
    while (1)
    {
        printf("please input the group ID 0~16777215(0xFFFFFF) to test\n");
        dst = get_value_u32(name, "group ID>", &dst);
        if (dst >= 0  && dst <= 16777215)
        {
            printf("you get the group_ID %d(%#010x)\n\n", dst, dst);
            break;
        }
        printf("the msg group_ID is not in range\n\n");
    }
    Group = dst;
    dst = U32Change2BigEndian(dst) >> 8;
    set_item_ID_input(item, pBuff, 0, 0);
    printf("\nSend:\n\tNas ---------------------> Group(%d)\n", Group);
    Pause();
    atDataItemFun[item].pFunOdp();
}



// ����ȫ�ֱ���
void set_item_ID_input(int item, unsigned char *pBuff, char SrcDev, char DstDev)
{
    const char *token;
    token = atDataItemFun[item].token;
    if ((strncmp(token, "MS", strlen(token)) == 0) && (pBuff != NULL))
    {
        s_DstRcv[0] = pBuff[0];
        s_DstRcv[1] = pBuff[1];
        s_DstRcv[2] = pBuff[2];
        // item-0   item-1  item-2
    }
    else if (strncmp(token, "Nas", strlen(token)) == 0)
    {
        s_SrcDev = SrcDev;
        s_DstDev = DstDev;
        // item-3, item-4, item-5, item-6, item-7

    }
    else if (strncmp(token, "Group", strlen(token)) == 0)
    {
        s_GroupID[0] = pBuff[0];
        s_GroupID[1] = pBuff[1];
        s_GroupID[2] = pBuff[2];
        // item-8
    }
    else
    {
        printf("[%s:%d]: toake err\n", __FUNCTION__, __LINE__);
        exit(0);
    }
}






void PressVoiceCc2cclSend()
{
    FILE *fp;
    int format, SendLen;
    int ret, index;
    const char *pVoiceFileName = NULL;
    CACHE_T tCache;
    CACHE_T *pCache = &tCache;
    CENTER_VOICE_DATA tCenterVoiceData;
    CENTER_VOICE_DATA *pSendBuff = &tCenterVoiceData;
    memset(pSendBuff, 0, sizeof(CENTER_VOICE_DATA));
    format = GetRandDigit() % 2;
    pVoiceFileName = ((format == AMBE_FORMAT) ? AMBE_FILE_NAME : NVOC_FILE_NAME);

    // �ļ��ֽ�����Ϊ27K, һ֡27bytes, SendLen�ĳ����趨Ϊ200~1000
    SendLen  = GetRandDigit() % POCKET_BYTE_MAX;
    SendLen = (SendLen < POCKET_BYTE_MIN) ? (SendLen + POCKET_BYTE_MIN) : SendLen;      // ��С200��
    printf("[%s:%d] format=%d, SendLen=%d\n",__FUNCTION__, __LINE__, format, SendLen);  //

    // 1-���ж��ļ��Ƿ����
    if ((ret = access(pVoiceFileName, F_OK)) < 0)
    {
        printf("%s is not exist(ret=%d)\n", pVoiceFileName, ret);
        perror("access>");
        exit(EXIT_FAILURE);
    }

    // 2-���ļ���ȡ
    if ((fp = fopen(pVoiceFileName, "rb")) == NULL)
    {
        printf("CC open file err\n");
        perror("fopen>");
        exit(EXIT_FAILURE);
    }

    // 3-��ȡ�ļ��ֽ���
    pCache->len = get_file_size(pVoiceFileName);

    // 4-��ȡ��̬�ڴ�
    if ((pCache->Buff = (char *)malloc(pCache->len)) == NULL)
    {
        perror("malloc>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // 5-���ļ���ȡ���ڴ���
    ret = fread(pCache->Buff, sizeof(pCache->Buff[0]), pCache->len, fp);
    if (ret != pCache->len)
    {
        perror("fread>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

#if 0
    // 6-�ж��Ƿ�ȫ������
    ret = feof(fp);
    printf("feof: ret=%d\n", ret);
    // �ж��Ƿ�ȫ������
    if (!feof(fp))
    {
        printf("Error reading from input file.\n");
        free(pCache->pBuff);
        perror("feof>");
        exit(EXIT_FAILURE);
    }
#endif

    // 6-�ر��ļ�
    if (fclose(fp) != 0)
    {
        printf("Error closing input file.\n");
        free(pCache->Buff);
        perror("fclose>");
        exit(EXIT_FAILURE);
    }

    // 7-����������
    // pCache->len ������27�ı���, �ó���������ļ���27k
    // SendLen 200~1000
    for(index = 0; (27*index < pCache->len) && (index < SendLen); index++)
    {
        memcpy(pSendBuff->Payload, pCache->Buff + 27*index, 27);
        sendto(s_tCenterVoiceSockfd, pSendBuff, sizeof(CENTER_VOICE_DATA), 0, (struct sockaddr *)&s_tCenterVoiceAddr, sizeof(s_tCenterVoiceAddr));
        delay(35);
    }

    free(pCache->Buff);
}




void *PressODPVoiceTask(void * p)
{
    printf("\n Start Voice Press \n\n");
    while (1)
    {
        VoiceCc2cclPptOn();
        PressVoiceCc2cclSend();
        VoiceCc2cclPptOff();
    }
    pthread_exit(NULL);
}




void SetVoiceID()
{
    unsigned int dst;
    char name[200];
    printf("\nInit Voice outband Calling_ID & Called_ID\n");
    snprintf(name, sizeof(name), "Fun:[%s], %s", __FUNCTION__, "input the [Calling] ID (0~16777215) to test, CallingID=any key\n");
    dst = get_dst_U32(name, "Entry CallingID>", 0, 16777215);
    dst = U32Change2BigEndian(dst) >> 8;
    s_CallingID[0] = ((unsigned char *)&dst)[0];
    s_CallingID[1] = ((unsigned char *)&dst)[1];
    s_CallingID[2] = ((unsigned char *)&dst)[2];
    snprintf(name, sizeof(name), "Fun:[%s], %s", __FUNCTION__, "input the [Called] ID (0~16777215) to test, CalledID=GroupID\n");
    dst = get_dst_U32(name, "Entry CalledID>", 0, 16777215);
    dst = U32Change2BigEndian(dst) >> 8;
    s_CalledID[0] = ((unsigned char *)&dst)[0];
    s_CalledID[1] = ((unsigned char *)&dst)[1];
    s_CalledID[2] = ((unsigned char *)&dst)[2];
    printf("\nPlease confirm your input\n");
    printf("s_CallingID[0]=%-8d(%#04x)\n", s_CallingID[0], s_CallingID[0]);
    printf("s_CallingID[1]=%-8d(%#04x)\n", s_CallingID[1], s_CallingID[1]);
    printf("s_CallingID[2]=%-8d(%#04x)\n", s_CallingID[2], s_CallingID[2]);
    printf("s_CalledID[0]=%-8d(%#04x)\n",  s_CallingID[0], s_CalledID[0]);
    printf("s_CalledID[1]=%-8d(%#04x)\n",  s_CallingID[1], s_CalledID[1]);
    printf("s_CalledID[2]=%-8d(%#04x)\n",  s_CallingID[2], s_CalledID[2]);
    Pause();
}




void SetSrcDstID()
{
    char name[50];
    printf("\nInit NAS SrcID & DstID \n");
    snprintf(name, sizeof(name), "%s", __FUNCTION__);
    get_src_dst_id(name, &s_SrcDev, &s_DstDev);
    printf("\nPlease confirm your input\n");
    printf("s_SrcDev=%d\t(%#04x)\n", s_SrcDev, s_SrcDev);
    printf("s_DstDev=%d\t(%#04x)\n", s_DstDev, s_DstDev);
    Pause();
}



unsigned int get_dst_U32(const char *name, const char *prmpt, int min, int max)
{
    char buff[11];          // ��������ַ�
    unsigned int dst;
    printf("%s", name);

    while (1)
    {
        if (digit_get(prmpt, buff, sizeof(buff), (int *)&dst) == GET_SUCCEED)
        {
            if ((int)dst >= min  &&  (int) dst <= max)
            {
                printf("\nYou get %d(%#010x)\n\n", dst, dst);
                break;
            }
            printf("\nDst val not in range, try again!!! \n\n");

        }
    }
    return dst;
}





int get_Packet_len(const char *name, const char *prmpt)
{
    char buff[11];          // ��������ַ�
    unsigned int dst;
    printf("%s\n\n", name);

    while (1)
    {
        if (digit_get(prmpt, buff, sizeof(buff), (int *)&dst) == GET_SUCCEED)
        {
            if (dst >= 0  &&  dst <= 100)
            {
                printf("\nYou get %d(%#010x)\n\n", dst, dst);
                break;
            }
            printf("\nthe id is not in range, try again!!! \n\n");

        }
    }
    return dst;
}




void SetGroupID()
{
    unsigned int dst;
    char name[200];
    printf("\nInit SMS outband Group ID\n");
    snprintf(name, sizeof(name), "Fun:[%s], %s", __FUNCTION__, "Input the [Group] ID (0~16777215) to test...\n");
    dst = get_dst_U32(name, "Entry Group ID>", 0, 16777215);
    dst = U32Change2BigEndian(dst) >> 8;
    s_GroupID[0] = ((unsigned char *)&dst)[0];
    s_GroupID[1] = ((unsigned char *)&dst)[1];
    s_GroupID[2] = ((unsigned char *)&dst)[2];
    printf("\nPlease confirm your input\n");
    printf("s_GroupID[0]=%d\t(%#04x)\n", s_GroupID[0], s_GroupID[0]);
    printf("s_GroupID[1]=%d\t(%#04x)\n", s_GroupID[1], s_GroupID[1]);
    printf("s_GroupID[2]=%d\t(%#04x)\n", s_GroupID[2], s_GroupID[2]);
    Pause();
}


void SetMsgLen()
{
    char name[200];
    printf("\nInit SMS len\n");
    snprintf(name, sizeof(name), "[%s], %s", __FUNCTION__, "please input [SMS len] (0~100) to test...\n");
    s_Message_len = get_dst_U32(name, "Entry len", 0, 100);
    printf("\nPlease confirm your input\n");
    printf("s_Message_len=%d\t(%#04x)\n", s_Message_len, s_Message_len);
    Pause();
}



// ѹ��������
void *PressODPDataTask(void * p)
{

    int index, item;
    while (1)
    {
        item = GetRandDigit() % ITEM_MAX;
        sleep(1);
        for (index = 0; atDataItemFun[index].item != ITEM_MAX; index++)
        {
            if (item == atDataItemFun[index].item)
            {
                (atDataItemFun[index].pFunOdp)();
                break;
            }
        }
    }
    pthread_exit(NULL);
}





void PressVoiceTaskCreate()
{
    int status;
    status = pthread_create(&ODP_VoicePid, NULL, PressODPVoiceTask, NULL);
    if (status != 0)
    {
        perror("Press Voice pthread_create>");
        exit(EXIT_FAILURE);
    }
    pthread_detach(ODP_VoicePid);
}



void PressDataTaskCreate()
{
    int status;
    printf("\n Start Data Press \n\n");
    status = pthread_create(&ODP_DataPid, NULL, PressODPDataTask, NULL);
    if (status != 0)
    {
        perror("Press Data pthread_create>");
        exit(EXIT_FAILURE);
    }
    pthread_detach(ODP_DataPid);
}





/*
 * ��ȡ��������ʽ
*/
int VoiceCc2cclFormatGet()
{
    unsigned int format;
    char name[50];
    while (1)
    {
        printf("\n\n1) The voice format as follow\n\
        1 - ambe\n\
        2 - nvoc\n\
        \nPlease input the number to select:\n");
        snprintf(name, sizeof(name), "%s", __FUNCTION__);
        format = get_value_u32(name, "Entry Format>", &format);
        if ((format != AMBE_FORMAT) && (format != NVOC_FORMAT))
        {
            printf("\nThere is no this option, try again\n");
            Pause();
            continue;
        }
        return format;
    }
}



/*
 * ��ȡ����������
*/
int VoiceCc2cclLenGet()
{
    unsigned int len;
    char name[50];
    while (1)
    {
        printf("\n\n2) Input the packet len, range: (%d ~ %d)*27bytes\n", POCKET_BYTE_MIN, POCKET_BYTE_MAX);
        snprintf(name, sizeof(name), "%s", __FUNCTION__);
        len = get_value_u32(name, "Entry len>", &len);
        if ((len >= POCKET_BYTE_MIN) && (len <= POCKET_BYTE_MAX))
        {
            return len;
        }
        else if (len > POCKET_BYTE_MAX )
        {
            printf("\nThe packet len you input is too large,\
            \nSo, set it as the default maximum (%d)\n\n", POCKET_BYTE_MAX);
            return POCKET_BYTE_MAX;
        }
        else if (len < POCKET_BYTE_MIN)
        {
            printf("\nThe packet len you input is too little,\
            \nSo, set it as the default minimum (%d)\n\n", POCKET_BYTE_MIN);
            return POCKET_BYTE_MIN;
        }
        else
        {
            printf("[%s:%d] len=(%d)(%x) err \n", __FUNCTION__, __LINE__, len, len);
            exit(EXIT_FAILURE);
        }
    }
}




void VoiceCc2cclSend(int len,  int format)
{
    FILE *fp;
    int ret, index;
    const char *pVoiceFileName = NULL;
    CACHE_T tCache;
    CACHE_T *pCache = &tCache;
    CENTER_VOICE_DATA tCenterVoiceData;
    CENTER_VOICE_DATA *pSendBuff = &tCenterVoiceData;
    memset(pSendBuff, 0, sizeof(CENTER_VOICE_DATA));

    pVoiceFileName = ((format == AMBE_FORMAT) ? AMBE_FILE_NAME : NVOC_FILE_NAME);
    // 1-���ж��ļ��Ƿ����
    if ((ret = access(pVoiceFileName, F_OK)) < 0)
    {
        printf("%s is not exist(ret=%d)\n", pVoiceFileName, ret);
        perror("access>");
        exit(EXIT_FAILURE);
    }

    // 2-���ļ���ȡ
    if ((fp = fopen(pVoiceFileName, "rb")) == NULL)
    {
        printf("CC open file err\n");
        perror("fopen>");
        exit(EXIT_FAILURE);
    }

    // 3-��ȡ�ļ��ֽ���
    pCache->len = get_file_size(pVoiceFileName);


    // 4-��ȡ��̬�ڴ�
    if ((pCache->Buff = (char *)malloc(pCache->len)) == NULL)
    {
        perror("malloc>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // 5-���ļ���ȡ���ڴ���
    ret = fread(pCache->Buff, sizeof(pCache->Buff[0]), pCache->len, fp);
    if (ret != pCache->len)
    {
        perror("fread>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

#if 0
    // 6-�ж��Ƿ�ȫ������
    ret = feof(fp);
    printf("feof: ret=%d\n", ret);
    // �ж��Ƿ�ȫ������
    if (!feof(fp))
    {
        printf("Error reading from input file.\n");
        free(pCache->pBuff);
        exit(EXIT_FAILURE);
    }
#endif



    // 6-�ر��ļ�
    if (fclose(fp) != 0)
    {
        printf("Error closing input file.\n");
        free(pCache->Buff);
        exit(EXIT_FAILURE);
    }

    // 7-����������
    // pCache->len ������27�ı���, �ó���������ļ���27k
    // len 200~1000  len
    for(index = 0; (27*index < pCache->len) && (index < len); index++)
    {
        memcpy(pSendBuff->Payload, pCache->Buff + 27*index, 27);
        sendto(s_tCenterVoiceSockfd, pSendBuff, sizeof(CENTER_VOICE_DATA), 0, (struct sockaddr *)&s_tCenterVoiceAddr, sizeof(s_tCenterVoiceAddr));
        delay(35);
    }

    printf("\n\ncc send (%d) to ccl\n\n", index);
    free(pCache->Buff);

}




int DataCc2CclItemGet()
{
    int item;
    unsigned int len;
    char name[50];
    snprintf(name, sizeof(name), "%s", __FUNCTION__);
    while (1)
    {
printf("\n\nData test items as follows\n\n\
\t0 - MS Remote Kill\n\
\t1 - Ms Enable\n\
\t2 - Ms Gps Report\n\
\t3 - Nas Disable\n\
\t4 - Nas Enable\n\
\t5 - Nas Gps Report\n\
\t6 - Nas Stun\n\
\t7 - Nas Neighbor Query\n\
\t8 - Ms Short Message\n\
\t9 - Nas Alarm req\n\
\nPlease press the 0 ~ 9 to select...\n");

        item = get_value_u32(name, "Entry Item>", &len);
        if ((item >=0 ) && (item <= 9))
        {
            Pause();
            return item;
        }
        else if (item > 9)
        {
            printf("\nThere is no this option, try again ...\n");
            Pause();
        }
    }
}





void DataCc2CclIStart(int item)
{
    int index;
    for (index = 0; atDataItemFun[index].item != ITEM_MAX; index++)
    {
        if (item == atDataItemFun[index].item)
        {
            (atDataItemFun[index].pFunItem)(item);
            break;
        }
    }
}










/**
 * @brief  ��������п��Ʋ㷢�����ݴ���
 *
 * @param [in] ptCclData       ���з�������
 * @author  ������
 * @since   trunk.00001
 * @bug
 */
static void IDP_SendCclData(DLL_CCL_UL_T * ptCclData)
{
    sendto(CCLSocket, ptCclData, sizeof(DLL_CCL_UL_T), 0, (struct sockaddr *)(&CclAddr), AddrSize);
    return;
}





void VoiceDdl2CclData(unsigned char DataType)
{
    DLL_CCL_UL_T tCclData;
    DLL_CCL_UL_T *ptCclData = &tCclData;
    memset(ptCclData, 0, sizeof(DLL_CCL_UL_T));
    ptCclData->MsgType  = DI_MSG_DATA;
    ptCclData->FrmType  = FT_VOICE_NO;
//    ptCclData->DataType = DataType;
    ptCclData->DataType = CT_LC_HEADER;
    ptCclData->DataLen  = FLC_LEN;
    ptCclData->SrcId[0] = 0X00;
    ptCclData->SrcId[1] = 0X03;
    ptCclData->SrcId[2] = 0XE8;
    ptCclData->DstId[0] = 0X55;
    ptCclData->DstId[1] = 0X55;
    ptCclData->DstId[2] = 0X55;
//  memcpy(ptCclData->PayLoad, pDataLink->PayLoad, FLC_LEN);
    IDP_SendCclData(ptCclData);
}








void VoiceDdl2CclSend(int len, int format)
{
    FILE *fp;
    int i, ret;
    DLL_CCL_UL_T tCclData;
    DLL_CCL_UL_T *ptCclData = &tCclData;
    CACHE_T tCache;
    CACHE_T *pCache = &tCache;

    const char *pVoiceFileName = ((format == AMBE_FORMAT) ? AMBE_FILE_NAME : NVOC_FILE_NAME);

    // 1-���ж��ļ��Ƿ����
    if ((ret = access(pVoiceFileName, F_OK)) < 0)
    {
        printf("%s is not exist(ret=%d)\n", pVoiceFileName, ret);
        perror("access>");
        exit(EXIT_FAILURE);
    }

    // 2-���ļ���ȡ
    if ((fp = fopen(pVoiceFileName, "rb")) == NULL)
    {
        printf("CC open file err\n");
        perror("fopen>");
        exit(EXIT_FAILURE);
    }

    // 3-��ȡ�ļ��ֽ���
    pCache->len = get_file_size(pVoiceFileName);


    // 4-��ȡ��̬�ڴ�
    if ((pCache->Buff = (char *)malloc(pCache->len)) == NULL)
    {
        perror("malloc>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // 5-���ļ���ȡ���ڴ���
    ret = fread(pCache->Buff, sizeof(pCache->Buff[0]), pCache->len, fp);
    if (ret != pCache->len)
    {
        perror("fread>");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

#if 0
        // 6-�ж��Ƿ�ȫ������
        ret = feof(fp);
        printf("feof: ret=%d\n", ret);
        // �ж��Ƿ�ȫ������
        if (!feof(fp))
        {
            printf("Error reading from input file.\n");
            free(pCache->pBuff);
            exit(EXIT_FAILURE);
        }
#endif



    // 6-�ر��ļ�
    if (fclose(fp) != 0)
    {
        printf("Error closing input file.\n");
        free(pCache->Buff);
        exit(EXIT_FAILURE);
    }


    //������ֵ
    ptCclData->MsgType  = DI_MSG_VOICE;
    ptCclData->DataType = 0;
    ptCclData->DataLen  = 27;


    while (len -- )
    {
        for (i = FT_VOICE_A; i < (FT_VOICE_F + 1); i++)
        {

            ptCclData->FrmType = i;
            //27 �ֽ����ݿ���
            memcpy(ptCclData->PayLoad, pCache->Buff+i*27, 27);
            IDP_SendCclData(ptCclData);
        }
    }
}




int VoiceDdl2CclLenGet()
{
    char name[50];
    unsigned int len;
    snprintf(name, sizeof(name), "%s", __FUNCTION__);
    while (1)
    {
        printf("please input the len (0~1000)to test...\n");
        len = get_value_u32(name, "Entry len>", &len);
        if (len*27 <= 27000)
        {
            return len;
        }
        printf("the len is not in range\n\n");
    }
}









// GPS
void PrintFormatGpsInfo(SMS_INFO_S *SmsInfo)
{
    GPS_DATA_S GpsData;
    char chTmp[16] = {0}, chNMin[32] = {0}, chEMin[32] = {0}, chNDeg[32] = {0}, chEDeg[32] = {0};
    double dNs, dEw;
    memset(chNDeg, ' ', 32);
    memset(chEDeg, ' ', 32);
    memcpy(GpsData.DeviceID, SmsInfo->SmsData, 4);
    memcpy(GpsData.GpsUnion.Data, &SmsInfo->SmsData[4], 8);
    memcpy(GpsData.DateTime, &SmsInfo->SmsData[12], 14);
    memset(SmsInfo->SmsData, 0x00, 512);
    sprintf(chNMin, "%d.%d", GpsData.GpsUnion.Data_bit.Nminmm, GpsData.GpsUnion.Data_bit.Nminf);
    sprintf(chEMin, "%d.%d", GpsData.GpsUnion.Data_bit.Eminmm, GpsData.GpsUnion.Data_bit.Eminf);
    dNs = GpsData.GpsUnion.Data_bit.Ndeg + atof(chNMin) / 60.0;
    dEw = GpsData.GpsUnion.Data_bit.Edeg + atof(chEMin) / 60.0;
    sprintf(chNDeg, "%f", dNs);
    sprintf(chEDeg, "%f", dEw);
    printf("NS: %s\n", chNDeg);
    printf("EW: %s\n", chEDeg);
    printf("GpsData.Ns=%d, GpsData.Ndeg=%d, GpsData.Nminmm=%d, GpsData.Nminf=%d\n", GpsData.GpsUnion.Data_bit.Ns, GpsData.GpsUnion.Data_bit.Ndeg, GpsData.GpsUnion.Data_bit.Nminmm, GpsData.GpsUnion.Data_bit.Nminf);
    printf("GpsData.Ew=%d, GpsData.Edeg=%d, GpsData.Eminmm=%d, GpsData.Eminf=%d\n", GpsData.GpsUnion.Data_bit.Ew, GpsData.GpsUnion.Data_bit.Edeg, GpsData.GpsUnion.Data_bit.Eminmm, GpsData.GpsUnion.Data_bit.Eminf);
    memcpy(SmsInfo->SmsData, GpsData.DeviceID, 4);
    if (GpsData.GpsUnion.Data_bit.Ns == 0)
    {
        SmsInfo->SmsData[4] = 'S';
    }
    else
    {
        SmsInfo->SmsData[4] = 'N';
    }
    sprintf(chTmp, "%02d%02d.%04d", GpsData.GpsUnion.Data_bit.Ndeg, GpsData.GpsUnion.Data_bit.Nminmm, GpsData.GpsUnion.Data_bit.Nminf);
    printf("Ndeg: %s\n", chTmp);
    memcpy(&SmsInfo->SmsData[5], chNDeg, 9);
    if (GpsData.GpsUnion.Data_bit.Ew == 0)
    {
        SmsInfo->SmsData[14] = 'E';
    }
    else
    {
        SmsInfo->SmsData[14] = 'W';
    }
    sprintf(chTmp, "%03d%02d.%04d", GpsData.GpsUnion.Data_bit.Edeg, GpsData.GpsUnion.Data_bit.Eminmm, GpsData.GpsUnion.Data_bit.Eminf);
    printf("Edeg: %s\n", chTmp);
    memcpy(&SmsInfo->SmsData[15], chEDeg, 10);
    memcpy(&SmsInfo->SmsData[35], GpsData.DateTime, 14);
    SmsInfo->ValidLength = 49;  //4+31+14
//  printf("@@@@@@@@Wireless_Bridge: GPS Info is %s\n", SmsInfo->SmsData);
}


#if 0


/**
 * @enum     _CENTER _SIG_TYPE_EN
 * @brief  ���뵥Ԫ��������������
 */
typedef  enum  _INF_SIG_TYPE_EN
{
    SIG_PTT_CMD         =0X000a,                    //PTT ���� ����
    SIG_PTT_ON_ACK      =0x000b,                    //PTT  ON �ظ� ����
    SIG_SMS_SEND        =0x000e,                    //����Ϣ����
    SIG_SMS_ACK         =0x0014,                    //����Ϣ�ظ�����
    SIG_PTT_OFF_ACK     =0x0015 ,                   //PTT  OFF�ظ� ����
}INF_SIG_TYPE_EN;




typedef void (*pSMS_CALL)(SMS_INFO_S *ptSmsInfo);
typedef struct _DATA_INFO_T
{
    unsigned short TypeFlag;
    const char *TypeStr;
    pSMS_CALL  pSmsFun;
} DATA_INFO_T;

DATA_INFO_T tSigType[] = {
    {SIG_PTT_CMD            ,   "sig_ptt_cmd"           },
    {SIG_PTT_ON_ACK         ,   "sig_ptt_on_ack"        },
    {SIG_SMS_SEND           ,   "sig_sms_send"          },
    {SIG_SMS_ACK            ,   "sig_sms_ack"           },
    {SIG_PTT_OFF_ACK        ,   "sig_ptt_off_ack"       },
};

DATA_INFO_T tSmsType[] = {
    {MESSAGE_PRIVATE_CALL   ,  "message_private_cal"    },
    {MESSAGE_GROUP_CALL     ,  "message_group_call"     },
    {STATUS_PRIVATE_CALL    ,  "status_private_call"    },
    {STATUS_GROUP_CALL      ,  "status_group_call"      },
    {STUN_REQ_MS            ,  "stun_req_ms"            },
    {STUN_REQ_NAS           ,  "stun_req_nas"           },
    {KILL_REQ_NAS           ,  "kill_req_nas"           },
    {GPS_REPORT_MS          ,  "gps_report_ms"          },
    {GPS_REPORT_NAS         ,  "gps_report_nas"         },
    {REVIVE_REQ_NAS         ,  "revive_req_nas"         },
    {REVIVE_REQ_MS          ,  "revive_req_ms"          },
    {NEIGHBOR_QUERY         ,  "neighbor_query"         },
    {NEIGHBOR_QUERY_ACK     ,  "neighbor_query_ack"     },
    {GPS_REPORT_MS_ACK      ,  "gps_report_ms_ack"      },
    {GPS_REPORT_NAS_ACK     ,  "gps_report_nas_ack"     },
    {STUN_REQ_MS_ACK        ,  "stun_req_ms_ack"        },
    {STUN_REQ_NAS_ACK       ,  "stun_req_nas_ack"       },
    {KILL_REQ_NAS_ACK       ,  "kill_req_nas_ack"       },
    {REVIVE_REQ_NAS_ACK     ,  "revive_req_nas_ack"     },
    {REVIVE_REQ_MS_ACK      ,  "revive_req_ms_ack"      },
    {NAS_NEAR_REPORT        ,  "nas_near_report"        },
    {VARIANCE_HRESHOLD      ,  "variance_hreshold"      },
    {DISCON_ALARM           ,  "discon_alarm"           },
    {MS_ALARM               ,  "ms_alarm"               },
    {DISCON_NAS_ALARM_CLEAR ,  "discon_nas_alarm_clear" },
    {MS_ALARM_CLEAR         ,  "ms_alarm_clear"         },
    {DSP_PRINT_LOG          ,  "dsp_print_log"          },
};



void *TestDataCheckTask(void * p)
{
    int ret;
    unsigned short i;
    CENTER_CMD_SHARE_HEAD *pHead;
    ssize_t count;
    fd_set Fdsr;
    struct sockaddr_in in_addr;
    struct timeval tv;
    socklen_t SinSize;
    unsigned char RecvBuf[1024];
    unsigned char DataExch[3];
    unsigned char ExchTmp;
    PTT_CMD_S PttCmd;
    SMS_INFO_S SmsInfo;
    unsigned int nSenderNum = 0, nRecvNum = 0;
    IPC_Ccl2ccInit();  // Ccl2cc �׽��ֽ���ͨ�ų�ʼ��
    bzero(&in_addr, sizeof(struct sockaddr_in));

    while (1)
    {
        FD_ZERO(&Fdsr);
        FD_SET(s_CCsigSocket, &Fdsr);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        ret = select(s_CCsigSocket + 1, &Fdsr, NULL, NULL, &tv);
        if (ret < 0)
        {
            printf("ipc ret < 0\n");
            printf("\n[%s:%d] select ret<0\n", __FUNCTION__, __LINE__);
            continue;
        }
        else if (ret == 0)
        {
            continue;
        }

        if (FD_ISSET(s_CCsigSocket, &Fdsr))
        {
            count = recvfrom(s_CCsigSocket, RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)(&in_addr), &SinSize);
            if (count < 0)
            {
                continue;
            }
        }

//      ��������
        printf("\n\n==================CHECK CMD=======================\n");
        pHead = (CENTER_CMD_SHARE_HEAD *)RecvBuf;

        for (i = 0; i < sizeof(tSigType)/sizeof(tSigType[0]) ;i++)
        {
            if (pHead->SigType == tSigType[i].TypeFlag)
            {
                printf("pHead->SigType=%d(%#06x)[%s]\n", pHead->SigType, pHead->SigType, tSigType[i].TypeStr);
            }
        }


        if ((RecvBuf[0] == 0x13) && (RecvBuf[1] == 0xec))       // ����ͷ����
        {
            if ((RecvBuf[2] == 0x0a) && (RecvBuf[3] == 0x00))   //PTT_CMD
            {
                memcpy(&PttCmd, RecvBuf, sizeof(PTT_CMD_S));    //
                if (PttCmd.PttStat == CMD_ON)   //
                {
                    printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);
                }

            }
            if ((RecvBuf[2] == 0x0b) && (RecvBuf[3] == 0x00))   //PTT_ACK
            {
                printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);

            }
            if ((RecvBuf[2] == 0x15) && (RecvBuf[3] == 0x00))   //PTT_OFF_ACK
            {
                printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);

            }
            if ((RecvBuf[2] == 0x0e) && (RecvBuf[3] == 0x00))   //SMS_INFO
            {

                memcpy(&SmsInfo, RecvBuf, sizeof(SMS_INFO_S));
                for (i = 0; i < sizeof(tSmsType)/sizeof(tSmsType[0]); i++)
                {
                    if (SmsInfo.SmsType == tSmsType[i].TypeFlag)
                    {
                        printf("SmsInfo.SmsType=%d(%#04x)[%s]\n", SmsInfo.SmsType, SmsInfo.SmsType, tSmsType[i].TypeStr);
                    }
                }

                if (SmsInfo.SmsType == MESSAGE_GROUP_CALL)
                {
                    memcpy(DataExch, SmsInfo.SenderNum, 3);
                    memset(SmsInfo.SenderNum, 0x00, 30);
                    // �ߵ�˳��
                    ExchTmp     = DataExch[0];
                    DataExch[0] = DataExch[2];
                    DataExch[2] = ExchTmp;
                    memcpy(&nSenderNum, DataExch, 3);
                    sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);
                    memcpy(DataExch, SmsInfo.ReceiverNum, 3);
                    memset(SmsInfo.ReceiverNum, 0x00, 30);
                    ExchTmp = DataExch[0];
                    DataExch[0] = DataExch[2];
                    DataExch[2] = ExchTmp;
                    memcpy(&nRecvNum, DataExch, 3);
                    sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                    // ��ӡ����
                    printf("Src(%s) ---------------------> Dst(%s)\n", SmsInfo.SenderNum, SmsInfo.ReceiverNum);
                    printf("MsgValidLen=(%d), MsgCharNum=(%d)\n", SmsInfo.ValidLength, SmsInfo.ValidLength/2);
                    printf("Msginfo:\n\t");
                    for (i = 0; i < SmsInfo.ValidLength; i++)
                    {
                        printf("%c ", SmsInfo.SmsData[i]);
                    }
                    printf("\n\n");


                }
                else if ((SmsInfo.SmsType == GPS_REPORT_MS_ACK) || (SmsInfo.SmsType == GPS_REPORT_NAS_ACK))
                {


                    if (SmsInfo.SmsType == GPS_REPORT_NAS_ACK)
                    {
                        memcpy(&nSenderNum, SmsInfo.SenderNum, 1);
                        sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);
                        memcpy(&nRecvNum, SmsInfo.ReceiverNum, 1);
                        sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                        printf("\n\nNasID(%d) response to (%d)\n\n", nSenderNum, nRecvNum);
                        printf("NAS GPS info:\n");
                        PrintFormatGpsInfo(&SmsInfo);
                    }

                    if (SmsInfo.SmsType == GPS_REPORT_MS_ACK)
                    {

                        memcpy(DataExch, SmsInfo.SenderNum, 3);
                        memset(SmsInfo.SenderNum, 0x00, 30);
                        ExchTmp = DataExch[0];
                        DataExch[0] = DataExch[2];
                        DataExch[2] = ExchTmp;
                        memcpy(&nSenderNum, DataExch, 3);
                        sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);

                        memcpy(DataExch, SmsInfo.ReceiverNum, 3);
                        memset(SmsInfo.ReceiverNum, 0x00, 30);
                        ExchTmp = DataExch[0];
                        DataExch[0] = DataExch[2];
                        DataExch[2] = ExchTmp;
                        memcpy(&nRecvNum, DataExch, 3);

                        sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                        printf("Src(%s) ---------------> Dst(%s)\n", SmsInfo.SenderNum, SmsInfo.ReceiverNum);
                        printf("MsgValidLen=(%d)\n", SmsInfo.ValidLength);
                        printf("MS GPS info:\n");
                        PrintFormatGpsInfo(&SmsInfo);
                    }

//                  SendMainCtrlGPSInfo(&SmsInfo);
//                  SendLogInfo(DEBUG, "Wireless_Bridge: Recv GPS_Info From Wireless\n");
                }
                else if (  (SmsInfo.SmsType == NEIGHBOR_QUERY_ACK) \
                        || (SmsInfo.SmsType == STUN_REQ_MS_ACK)    \
                        || (SmsInfo.SmsType == STUN_REQ_NAS_ACK)   \
                        || (SmsInfo.SmsType == KILL_REQ_NAS_ACK)   \
                        || (SmsInfo.SmsType == REVIVE_REQ_NAS_ACK) \
                        || (SmsInfo.SmsType == REVIVE_REQ_MS_ACK)  \
                        || (SmsInfo.SmsType == STUN_REQ_NAS)       \
                        || (SmsInfo.SmsType == KILL_REQ_NAS)       \
                        || (SmsInfo.SmsType == REVIVE_REQ_NAS)     \
                        )
                {
                    if ((SmsInfo.SmsType == STUN_REQ_MS_ACK) || (SmsInfo.SmsType == REVIVE_REQ_MS_ACK))
                    {
                        memcpy(DataExch, SmsInfo.SenderNum, 3);
                        memset(SmsInfo.SenderNum, 0x00, 30);
                        ExchTmp = DataExch[0];
                        DataExch[0] = DataExch[2];
                        DataExch[2] = ExchTmp;
                        memcpy(&nSenderNum, DataExch, 3);
                        sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);
                        memcpy(DataExch, SmsInfo.ReceiverNum, 3);
                        memset(SmsInfo.ReceiverNum, 0x00, 30);
                        ExchTmp = DataExch[0];
                        DataExch[0] = DataExch[2];
                        DataExch[2] = ExchTmp;
                        memcpy(&nRecvNum, DataExch, 3);
                        sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                    }

                    if ((SmsInfo.SmsType == NEIGHBOR_QUERY_ACK) || (SmsInfo.SmsType == STUN_REQ_NAS_ACK) || (SmsInfo.SmsType == KILL_REQ_NAS_ACK) || (SmsInfo.SmsType == REVIVE_REQ_NAS_ACK))
                    {
                        memcpy(&nSenderNum, SmsInfo.SenderNum, 1);
                        memset(SmsInfo.SenderNum, 0x00, 30);
                        sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);

                        memcpy(&nRecvNum, SmsInfo.ReceiverNum, 1);
                        memset(SmsInfo.ReceiverNum, 0x00, 30);
                        sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                    }


                    if ((SmsInfo.SmsType == STUN_REQ_NAS) || (SmsInfo.SmsType == KILL_REQ_NAS) || (SmsInfo.SmsType == REVIVE_REQ_NAS))  //遥晕遥毙激活基站
                    {
                        memcpy(&nSenderNum, SmsInfo.SenderNum, 1);
                        memset(SmsInfo.SenderNum, 0x00, 30);
                        sprintf((char *)SmsInfo.SenderNum, "%d", nSenderNum);

                        memcpy(&nRecvNum, SmsInfo.ReceiverNum, 1);
                        memset(SmsInfo.ReceiverNum, 0x00, 30);
                        sprintf((char *)SmsInfo.ReceiverNum, "%d", nRecvNum);
                    }


                }

            }
            if ((RecvBuf[2] == 0x14) && (RecvBuf[3] == 0x00))   //SMS_ACK
            {
                printf("\n\n[%s:%d]\n\n", __FUNCTION__, __LINE__);
            }

        }
    }

    pthread_exit(NULL);
}


#else

/**
 * @enum     _CENTER _SIG_TYPE_EN
 * @brief  ���뵥Ԫ��������������
 */
typedef  enum  _INF_SIG_TYPE_EN
{
    SIG_PTT_CMD         =0X000a,                    //PTT ���� ����
    SIG_PTT_ON_ACK      =0x000b,                    //PTT  ON �ظ� ����
    SIG_SMS_SEND        =0x000e,                    //����Ϣ����
    SIG_SMS_ACK         =0x0014,                    //����Ϣ�ظ�����
    SIG_PTT_OFF_ACK     =0x0015 ,                   //PTT  OFF�ظ� ����
}INF_SIG_TYPE_EN;


void get_id_cc(unsigned char *buf, int len)
{

    unsigned char ExchTmp;
    unsigned char DataExch[MS_ID_SIZE];
    unsigned int  uNum = 0;

    if (len == MS_ID_SIZE)
    {
        memcpy(DataExch, buf, MS_ID_SIZE);
        memset(buf, 0x00, MAX_CALL_NUM);
        ExchTmp = DataExch[0];
        DataExch[0] = DataExch[2];
        DataExch[2] = ExchTmp;
        memcpy(&uNum, DataExch, MS_ID_SIZE);
        sprintf((char *)buf, "%d", uNum);
    }
    else if (len == NAS_ID_SIZE)
    {
        memcpy(&uNum, buf, NAS_ID_SIZE);
        memset(buf, 0x00, MAX_CALL_NUM);
        sprintf((char *)buf, "%d", uNum);
    }
    else
    {
        printf("[%s:%d]len err", __FUNCTION__, __LINE__);
    }
}



void SMS_MessageGroupCall(SMS_INFO_S *ptSmsInfo)
{
    int i;
    printf("In Fun:[%s]\n", __FUNCTION__);
    get_id_cc(ptSmsInfo->SenderNum,   GROUP_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, GROUP_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    printf("Short Msg info:\n\t");
    for (i = 0; i < ptSmsInfo->ValidLength; i++)
    {
        printf("%c ", ptSmsInfo->SmsData[i]);
    }
    printf("\n\n");
}

void SMS_GpsReportMsAck(SMS_INFO_S *ptSmsInfo)
{
    printf("In Fun: [%s]\n", __FUNCTION__);
    get_id_cc(ptSmsInfo->SenderNum, MS_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, MS_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    PrintFormatGpsInfo(ptSmsInfo);
    printf("\n\n");
}

void SMS_GpsReportNasAck(SMS_INFO_S *ptSmsInfo)
{
    printf("In Fun: [%s]\n", __FUNCTION__);
    get_id_cc(ptSmsInfo->SenderNum, NAS_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, NAS_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    PrintFormatGpsInfo(ptSmsInfo);
    printf("\n\n");
}

void SMS_MsCommonCall(SMS_INFO_S *ptSmsInfo)
{
    get_id_cc(ptSmsInfo->SenderNum, MS_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, MS_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    printf("\n\n");
}

void SMS_NasCommonCall(SMS_INFO_S *ptSmsInfo)
{
    get_id_cc(ptSmsInfo->SenderNum, NAS_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, NAS_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    printf("\n\n");
}



void SMS_AlarmGroupCall(SMS_INFO_S *ptSmsInfo)
{
    get_id_cc(ptSmsInfo->SenderNum, GROUP_ID_SIZE);
    get_id_cc(ptSmsInfo->ReceiverNum, GROUP_ID_SIZE);
    printf("Receive:\n\tSrc(%s) ---------------------> Dst(%s)\n", ptSmsInfo->SenderNum, ptSmsInfo->ReceiverNum);
    printf("\n\n");
}



typedef void (*pSMS_CALL)(SMS_INFO_S *ptSmsInfo);
typedef struct _DATA_INFO_T
{
    unsigned short TypeFlag;
    const char *TypeStr;
    pSMS_CALL  pSmsFun;
} DATA_INFO_T;



DATA_INFO_T tSigType[] = {
    {SIG_PTT_CMD            ,   "sig_ptt_cmd"           },
    {SIG_PTT_ON_ACK         ,   "sig_ptt_on_ack"        },
    {SIG_SMS_SEND           ,   "sig_sms_send"          },
    {SIG_SMS_ACK            ,   "sig_sms_ack"           },
    {SIG_PTT_OFF_ACK        ,   "sig_ptt_off_ack"       },
};


// ����
DATA_INFO_T tSmsType[] = {
    {MESSAGE_PRIVATE_CALL   ,  "message_private_cal"    , NULL},
    {MESSAGE_GROUP_CALL     ,  "message_group_call"     , SMS_MessageGroupCall},
    {STATUS_PRIVATE_CALL    ,  "status_private_call"    , NULL},
    {STATUS_GROUP_CALL      ,  "status_group_call"      , NULL},
    {STUN_REQ_MS            ,  "stun_req_ms"            , NULL},
    {STUN_REQ_NAS           ,  "stun_req_nas"           , SMS_NasCommonCall},
    {KILL_REQ_NAS           ,  "kill_req_nas"           , SMS_NasCommonCall},
    {GPS_REPORT_MS          ,  "gps_report_ms"          , NULL},
    {GPS_REPORT_NAS         ,  "gps_report_nas"         , NULL},
    {REVIVE_REQ_NAS         ,  "revive_req_nas"         , SMS_NasCommonCall},
    {REVIVE_REQ_MS          ,  "revive_req_ms"          , NULL},
    {NEIGHBOR_QUERY         ,  "neighbor_query"         , NULL},
    {NEIGHBOR_QUERY_ACK     ,  "neighbor_query_ack"     , SMS_NasCommonCall},
    {GPS_REPORT_MS_ACK      ,  "gps_report_ms_ack"      , SMS_GpsReportMsAck},
    {GPS_REPORT_NAS_ACK     ,  "gps_report_nas_ack"     , SMS_GpsReportNasAck},
    {STUN_REQ_MS_ACK        ,  "stun_req_ms_ack"        , SMS_MsCommonCall},
    {STUN_REQ_NAS_ACK       ,  "stun_req_nas_ack"       , SMS_NasCommonCall},
    {KILL_REQ_NAS_ACK       ,  "kill_req_nas_ack"       , SMS_NasCommonCall},
    {REVIVE_REQ_NAS_ACK     ,  "revive_req_nas_ack"     , SMS_NasCommonCall},
    {REVIVE_REQ_MS_ACK      ,  "revive_req_ms_ack"      , SMS_MsCommonCall},
    {NAS_NEAR_REPORT        ,  "nas_near_report"        , NULL},
    {VARIANCE_HRESHOLD      ,  "variance_hreshold"      , NULL},
    {DISCON_ALARM           ,  "discon_alarm"           , NULL},
    {MS_ALARM               ,  "ms_alarm"               , SMS_AlarmGroupCall},
    {DISCON_NAS_ALARM_CLEAR ,  "discon_nas_alarm_clear" , NULL},
    {MS_ALARM_CLEAR         ,  "ms_alarm_clear"         , NULL},
    {DSP_PRINT_LOG          ,  "dsp_print_log"          , NULL},
};








void *TestDataCheckTask(void * p)
{
    int ret;
    unsigned short i;
    CENTER_CMD_SHARE_HEAD *pHead;
    ssize_t count;
    fd_set Fdsr;
    struct sockaddr_in in_addr;
    struct timeval tv;
    socklen_t SinSize;
    unsigned char RecvBuf[1024];
#if 0
    unsigned char DataExch[3];
    unsigned char ExchTmp;
    unsigned int nSenderNum = 0, nRecvNum = 0;
#endif
    PTT_CMD_S PttCmd;
    SMS_INFO_S SmsInfo;
    IPC_Ccl2ccInit();  // Ccl2cc �׽��ֽ���ͨ�ų�ʼ��
    bzero(&in_addr, sizeof(struct sockaddr_in));
    while (1)
    {
        FD_ZERO(&Fdsr);
        FD_SET(s_CCsigSocket, &Fdsr);
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        ret = select(s_CCsigSocket + 1, &Fdsr, NULL, NULL, &tv);
        if (ret < 0)
        {
            printf("ipc ret < 0\n");
            printf("\n[%s:%d] select ret<0\n", __FUNCTION__, __LINE__);
            continue;
        }
        else if (ret == 0)
        {
            continue;
        }

        if (FD_ISSET(s_CCsigSocket, &Fdsr))
        {
            count = recvfrom(s_CCsigSocket, RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)(&in_addr), &SinSize);
            if (count < 0)
            {
                continue;
            }
        }

        // ��������
        printf("\n\n==================RECEIVE MSG=======================\n");
        pHead = (CENTER_CMD_SHARE_HEAD *)RecvBuf;

        for (i = 0; i < sizeof(tSigType)/sizeof(tSigType[0]); i++)
        {
            if (pHead->SigType == tSigType[i].TypeFlag)
            {
                printf("pHead->SigType=%d(%#06x)[%s]\n", pHead->SigType, pHead->SigType, tSigType[i].TypeStr);
            }
        }


        if ((RecvBuf[0] == 0x13) && (RecvBuf[1] == 0xec))       // ����ͷ����
        {
            if ((RecvBuf[2] == 0x0a) && (RecvBuf[3] == 0x00))   //PTT_CMD
            {
                memcpy(&PttCmd, RecvBuf, sizeof(PTT_CMD_S));
                if (PttCmd.PttStat == CMD_ON)   //
                {
                    printf("\n\n[%s:%d] Ptt Stat is on \n\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    printf("\n\n[%s:%d] Ptt Stat is off \n\n", __FUNCTION__, __LINE__);
                }

            }
            if ((RecvBuf[2] == 0x0b) && (RecvBuf[3] == 0x00))   //PTT_ACK
            {
                printf("\n\n[%s:%d] PTT_ACK  \n\n", __FUNCTION__, __LINE__);

            }
            if ((RecvBuf[2] == 0x15) && (RecvBuf[3] == 0x00))   //PTT_OFF_ACK
            {
                printf("\n\n[%s:%d] PTT_OFF_ACK \n\n", __FUNCTION__, __LINE__);

            }
            if ((RecvBuf[2] == 0x0e) && (RecvBuf[3] == 0x00))   //SMS_INFO
            {
                memcpy(&SmsInfo, RecvBuf, sizeof(SMS_INFO_S));
                for (i = 0; i < sizeof(tSmsType)/sizeof(tSmsType[0]); i++)
                {
                    if (tSmsType[i].TypeFlag ==  SmsInfo.SmsType)
                    {
                        printf("prompt:SmsType=%d(%#04x)\nMsg:[%s]\n", SmsInfo.SmsType, SmsInfo.SmsType, tSmsType[i].TypeStr);
                        if (tSmsType[i].pSmsFun != NULL)
                        {
                            tSmsType[i].pSmsFun(&SmsInfo);
                        }
                        else
                        {
                            printf("[%s:%d] err: no function\n", __FUNCTION__, __LINE__);
                        }
                    }
                }
            }
            if ((RecvBuf[2] == 0x14) && (RecvBuf[3] == 0x00))   //SMS_ACK
            {
                printf("\n\n[%s:%d] SMS_ACK \n\n", __FUNCTION__, __LINE__);
            }

        }
    }

    pthread_exit(NULL);
}
#endif


void TestDataCheckTaskCreate()
{
    int status;
    status = pthread_create(&ODP_DataRcvPid, NULL, TestDataCheckTask, NULL);
    if (status != 0)
    {
        printf("\nerr: create ODP press data pthread\n");
        perror("pthread_create>");
        exit(EXIT_FAILURE);
    }
    pthread_detach(ODP_DataRcvPid);
}




// �������в���
int TestDataCc2ccl()
{
    int item;
    item = DataCc2CclItemGet();
    delay(50);      // �ȴ����ս��̾���
    DataCc2CclIStart(item);
    return 0;
}


// �������в���
void TestVoiceCc2ccl()
{
    int format, len;
    format = VoiceCc2cclFormatGet();  // ��ȡ��������ʽ
    len = VoiceCc2cclLenGet();        // ��ȡ����������
    SetVoiceID();                     // ��ȡԴ��ַ��Ŀ�ĵ�ַ
    VoiceCc2cclPptOn();
    VoiceCc2cclSend(len, format);
    VoiceCc2cclPptOff();
}


// �������в���
void TestVoiceDll2ccl()
{
    int format, FrameLen;
    format = VoiceCc2cclFormatGet();  // ��ȡ��������ʽ
    FrameLen = VoiceDdl2CclLenGet();
    SetVoiceID();                     // ��������-�ֶ�����ȫ�ֱ���CallingID, CalledID
    VoiceDdl2CclData(CT_LC_HEADER);
    VoiceDdl2CclSend(FrameLen, format);
    VoiceDdl2CclData(CT_LC_TERMINATOR);
}


/*
 * ѹ������
 */
void TestPress()
{
    SetVoiceID();       // ��������-�ֶ�����ԴID��Ŀ��ID
    SetGroupID();       // ���Ź���-������ID
    SetSrcDstID();      // ԴID&Ŀ��ID
    SetMsgLen();        // ���ö��ŵĳ���
    PressVoiceTaskCreate();
    PressDataTaskCreate();
}


/*
 * test options
 */
int TestOption(int argc, char *argv[])
{
    int opt;
    const char *optstring = "vduphgVDUPHG";


    while ((opt = getopt(argc, argv, optstring)) != -1)
    {
#if 0
        // ��麯������ֵ
        printf("opt    = %c\n",    opt);
        printf("optarg = %s\n", optarg);
        printf("optind = %d\n", optind);
        printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
#endif
        switch (opt)
        {
            case 'v':
            case 'V':
            {
                // �������в���
                TestVoiceCc2ccl();
                exit(EXIT_SUCCESS);
                break;
            }
            case 'u':
            case 'U':
            {
                // �������в���
                TestVoiceDll2ccl();
                exit(EXIT_SUCCESS);
                break;
            }
            case 'd':
            case 'D':
            {
                // �������в���
                TestDataCheckTaskCreate();  // �ȴ����ճ���
                TestDataCc2ccl();           // ��������
                break;
            }
            case 'g':
            case 'G':
            {
                // �������в���
                printf("============Wait For Data From Ms Or Nas============\n\n");
                TestDataCheckTaskCreate();
                break;
            }
            case 'p':
            case 'P':
            {
                // ѹ������
                TestPress();
                break;
            }
            case 'h':
            case 'H':
            {
                // ����
                ShowHelp();
                exit(EXIT_SUCCESS);
                break;
            }
            default:
            {
                fprintf(stderr, "Usage: %s [-vduph] \n", argv[0]);
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
    return 0;
}



void TestInit()
{
    // IPC ��ʼ��
    IPC_SocketVoiceSoundCc2cclInit();
    IPC_SocketVoiceSigCc2cclInit();
    IPC_SocketVoiceDll2CclInit();
    IPC_SocketDataCc2cclInit();
}



void CompileTime()
{
    printf("Compile Time: [%s - %s]\n\n\n",  __DATE__, __TIME__);
}



/*
 * ./argv[0]    -v  # �������в���
 *              -d  # �������в���
 *              -u  # �������в���
 *              -p  # ѹ������
 *              -h  # ����
 *              -g  # ��������
*/
int main(int argc, char *argv[])
{
    CompileTime();
    ProcessNameSave(argv[0]);  // ���ý�����

    if (argc == 1)
    {
        ShowHelp();
        exit(EXIT_SUCCESS);
    }
    else if (argc == 2)
    {
        TestInit();
        TestOption(argc, argv); // ���Գ���
    }
    else
    {
        printf("\nErr: option is too many...\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\n~~Press~the~[CTR+c]~to~quit~~\n\n");
    while(1)
    {
        sleep(5);
    }
    return 0;
}

