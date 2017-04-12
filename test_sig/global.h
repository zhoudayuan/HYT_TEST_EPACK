#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <pthread.h>
#include <stdbool.h>

#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define RTP_DATA_SIZE 27
#define MAX_CALL_NUM 30
#define MAX_SMS 220
#define MAX_DATA_LENGTH 768     //1024
#define U8_NONE 0xff
#define U16_NONE 0xffff
#define U32_NONE 0xffffffff
#define NET_HEAD 0x13ec
#define CENTER_HEAD 0xea01
#define CENTER_TAIL 0xed02
#define MAX_SMS_LENGTH 512
#define AMBE_VOICE_LENGTH 27    //ABME语音长度
#define QUEUE_MAX_SIZE 640
#define FILE_PATH "/root/AMBE_Test.ap"
#define RECV_WIRELESS_CMD_PORT 10012
#define RECV_WIRELESS_VOICE_PORT 10010
#define SEND_WIRELESS_CMD_PORT 10013
#define SEND_WIRELESS_VOICE_PORT 10011
#define AMBE_PACK_LENGTH 64 //语音包长度
#define CENTER_IP "127.0.0.1"
//#define CENTER_IP "192.168.66.133"
//#define CENTER_IP "192.168.66.47"
//#define CENTER_IP "192.168.1.173"
#define WIRELESS_IP "127.0.0.1"
#define CENTER_PORT 7011  //2222
#define WIRELESS_PORT 7000  //4444
#define DAEMON_PORT 7009
#define LOG_PORT 7010
#define DAEMON_IP "127.0.0.1"
//#define LOG_IP "127.0.0.1"
#define LOG_IP "127.0.0.1"
#define WIRELESS_BRIDGE_PATH    "/home/wireless_bridge_module"
#define CENTER_MODULE_PATH    "/home/center_module"
#define VARIANCE_TYPE 0x0016
#pragma pack(1)

enum LOG_LEVEL
{
    DEBUG = 1,
    WARNING = 2,
    ERROR = 3,
};

enum RECVD_ACK
{
    N_RECVD = 0,
    RECVD = 1
};

enum SEND_DIRECTION
{
    TO_MAIN_CTRL = 0,
    TO_WIRELESS = 1
};

enum RTP_FRAME_TYPE
{
    PCMU = 0x00,
    GSM = 0x03,
    PCMA = 0x08,
    QCELP = 0x0c,
    ACELP = 0x63,
    SELP = 0xcc,
    AMBE = 0xce
};

enum MODULE_TYPE
{
    TYPE_WIRELESS = 0x00,
    TYPE_NM = 0x01,
    TYPE_INTERCONNECTION = 0x02,
    TYPE_REPETER = 0x03,
    TYPE_POWMANAGE = 0x04,
    TYPE_HANDCALL = 0x05,
    TYPE_CODECMANAGE = 0x06,
    TYPE_DSP = 0x07,
    TYPE_GSM = 0x08
};

enum SIGNALLING_TYPE
{
    AC_STAT_INFO = 0x0001,
    AC_STAT_QUERRY = 0x0002,
    AC_CALL_REQ = 0x0003,
    AC_CALL_ACK = 0x0004,
    AC_VOICE_LINK = 0x0005,
    AC_RELEASE_INFO = 0x0006,
    AC_RELEASE_ACK = 0x0007,
    AC_UNIT_REBOOT = 0x0008,
    AC_REBOOT_ACK = 0x0009,
    AC_PTT_CMD = 0x000a,
    AC_PTT_ACK = 0x000b,
    AC_SPEAK_ME = 0x000c,
    AC_DTMF_DATA = 0x000d,
    AC_SMS_SEND = 0x000e,
    AC_VOICE_LINK_ACK = 0x000f,
    AC_CALL_FILTER_SEND = 0x0010,
    AC_CALL_FILTER_ACK = 0x0011,
    AC_CALL_OPTION_SEND = 0x0012,
    AC_CALL_OPTION_ACK = 0x0013,
    AC_SMS_ACK = 0x0014,
    AC_PTT_OFF_ACK = 0x0015,
    AC_ALARM_SEND = 0x0016,
    AC_ALARM_END = 0x0017,
    AC_HOLDING_CALL = 0x0018,
    AC_HOLDING_ACK = 0x0019,
    AC_REDIAL_CALL = 0x001a,
    AC_REDIAL_ACK = 0x001b,
    AC_CALL_COMBINE = 0x001c,
    AC_COMBINE_ACK = 0x001d,
    AC_CSTAT_QUERRY = 0x001e,
    AC_CSTAT_INFO = 0x001f,
    AC_DTMF_SWITCH = 0x0020,
    AC_DTMF_SWITCH_ACK = 0x0021,
    AC_DTMF_ACK = 0x0022,
    AC_PICK_UP_ACK = 0x0023,
    AC_ADD_GROUP   = 0x0024,
    AC_ADD_GROUP_ACK = 0x0025,
    AC_PILOT_LAMP_CMD = 0x0026,
    AC_TELNUM_QUERRY = 0x0027,
    AC_TELNUM_INFO = 0x0028,
    AC_TEL_CMD = 0x0029,
    AC_TEL_CMD_ACK = 0x002a,
    AC_DAEMON_QUERY = 0x002e,
    AC_DAEMON_QUERY_ACK = 0x002f,
    AC_CENTER_VOICE_FLAG = 0x1000,
    AC_LOG_INFO = 0x0200,
};



#if 1
enum SMS_TYPE
{
    MESSAGE_PRIVATE_CALL   = 0x00,  //单呼
    MESSAGE_GROUP_CALL     = 0x01,  //组呼
    STATUS_PRIVATE_CALL    = 0x02,  //状态呼
    STATUS_GROUP_CALL      = 0x03,  //状态组

    STUN_REQ_MS            = 0x07,  //遥晕MS
    STUN_REQ_NAS           = 0x08,  //遥晕NAS
    KILL_REQ_NAS           = 0x09,  //遥毙NAS
    GPS_REPORT_MS          = 0x0a,  //GPS上拉
    GPS_REPORT_NAS         = 0x0b,  //
    REVIVE_REQ_NAS         = 0x0c,  //激活请求
    REVIVE_REQ_MS          = 0x0d,
    NEIGHBOR_QUERY         = 0x0e,  //邻站信息查询
    NEIGHBOR_QUERY_ACK     = 0x20,  //邻站信息响应
    GPS_REPORT_MS_ACK      = 0x21,  //响应拉MS
    GPS_REPORT_NAS_ACK     = 0x22,  //响应拉NAS
    STUN_REQ_MS_ACK        = 0x23,  //
    STUN_REQ_NAS_ACK       = 0x24,
    KILL_REQ_NAS_ACK       = 0x25,
    REVIVE_REQ_NAS_ACK     = 0x26,  //激活NAS响应
    REVIVE_REQ_MS_ACK      = 0x27,  //激活MS响应
    NAS_NEAR_REPORT        = 0x28,  //
    VARIANCE_HRESHOLD      = 0x29,  //方差门限，场强值

    //STUN_NAS_REPORT        = 0x30,
    //KILL_NAS_REPORT        = 0x31,
    //REVIVE_NAS_REPORT      = 0x32,

    DISCON_ALARM           = 0x40,
    MS_ALARM               = 0x41,
    DISCON_NAS_ALARM_CLEAR = 0x42,
    MS_ALARM_CLEAR         = 0x43,

    DSP_PRINT_LOG          = 0x16,        //DSP打印
};
#endif

enum CMD_TYPE
{
    ACK_OK = 0x04,      /* for call_ack /release_ack /reboot_ack /voice_link /ptt_ack /speak_me*/
    ACK_FAIL = 0x05,     /* for call_ack /release_ack /reboot_ack /voice_link /ptt_ack /speak_me*/
    CMD_ON = 0x06,      /* for ptt_cmd*/  //??????
    CMD_OFF = 0x07     /* for ptt_cmd*/
};

typedef struct
{
    u8 CsrcCount:4;
    u8 ExHeadFlag:1;
    u8 PaddingFlag:1;
    u8 Version:2;
    u8 PayloadType:7;
    u8 Marker:1;
    u16 SequenceNum;
    u32 TimeStamp;
    u32 Ssrc;
} RTP_FIX_HEAD_S;

typedef struct
{
    u16 ProfileHead;
    u16 ExheadLength;      // 32bit number == 4 bytes number
    u32 VoiceId;
    u32 Gps;
    u8 CodecFlag;
    u8 FrameFlag;
    u8 PathNum;         // show which path voice 0xff indicates invalid value, "0" indicates the first path
    u8 PackRate;           // packet rate "20" or "60" milliseconds
    u8 EncryptFlag;
    u8 Reserve[3];
} RTP_EX_HEAD_S;

typedef struct
{
    RTP_FIX_HEAD_S RtpHead;
    RTP_EX_HEAD_S RtpEx;
    u8 Payload[RTP_DATA_SIZE];
    u8 ReserveData[5];
} LINK_VOICE_DATA_S;

typedef struct
{
    u8 Reseve[6];
    u16 InfPort;
    u32 Ip;
} Wl_INTERCONNECT_S;

typedef struct
{
    u8 PathNum;
    u8 Reseve1[11];
} _RESERVED_S;

typedef union
{
    _RESERVED_S Rev;
    Wl_INTERCONNECT_S WI_CONNECT;
} EX_HEAD_S;

typedef struct
{
    u16 Head;
    u16 Type;
    u16 DataLength;
    u16 SourceId;
    u16 DestId;
    u8 SourceType;
    u8 SigVersion;
    EX_HEAD_S ExHead;
    u32 CheckVal;
} SIGNALLING_HEAD_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    u32 CallId;
    u32 VoiceId;
    u8 PttStat;
    u8 Reserved;             // not used for adapt module and center , fill all "f"
    u8 TalkingNum[MAX_CALL_NUM];
    u8 CallingNum[MAX_CALL_NUM];
    u8 CalledNum[MAX_CALL_NUM];
} PTT_CMD_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    u32 CallId;
    u32 VoiceId;
    u8 Ack;
    u8 Reserved;             // not used for adapt module and center , fill all "f"
    u8 TalkingNum[MAX_CALL_NUM];
    u8 CallingNum[MAX_CALL_NUM];
    u8 CalledNum[MAX_CALL_NUM];
} PTT_ACK_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    u32 CallId;
    u32 VoiceId;
    u8 Reserved[2];             // not used for adapt module and center , fill all "f"
    u8 TalkingNum[MAX_CALL_NUM];
    u8 CallingNum[MAX_CALL_NUM];
    u8 CalledNum[MAX_CALL_NUM];
} PTT_OFF_ACK_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    u32 CallId;
    u32 VoiceId;
    u8 SourceStat;
    u8 SmsType;
    u16 ValidLength;
    u8 SmsFormat;
    u8 SenderNum[MAX_CALL_NUM];
    u8 ReceiverNum[MAX_CALL_NUM];
    u8 SmsData[MAX_SMS_LENGTH];
    u8 Reserve[3];
} SMS_INFO_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    u32 CallId;
    u32 VoiceId;
    u8 SourceStat;
    u8 Ack;
    u8 Reserved[2];
} SMS_ACK_S;

typedef struct
{
    u16 Head;
    u8  Type;
    u16 DataLength;
    u8  Data[MAX_DATA_LENGTH];      //???
    u16 Reserved;
    u16 CRC;
    u16 Tail;
} MAINCTRL_DATA_S;

typedef struct
{
    SIGNALLING_HEAD_S SigHead;
    LINK_VOICE_DATA_S VoiceData;
} CENTER_VOICE_DATA_S;

typedef struct SendList
{
    u8  SendBuf[512];        //发送缓冲区
    u16 DataLen;             //数据长度
    u8  Direction;           //发送方向
    u16 OverTime;           //超时计数器
    u16 SendTimes;          //发送次数
    u32 CallId;             //用于判断是否是本节点短回复
    u8 RecvAck;             //收到Ack标识
    struct SendList *pNextNode; //指向下一个发送缓冲的指针
} SEND_LIST_S;

typedef struct CircleQueue
{
    u16 Front;
    u16 Rear;
    u8 Data[QUEUE_MAX_SIZE];
} CIRCLE_QUEUE_S;

typedef struct MutexStruct
{
    pthread_mutex_t WTCVoiceLock;   //无线给主控语音锁
    pthread_mutex_t CTWVoiceLock;   //主控给中心语音锁
    bool WTCLockStatus; //无线给主控的锁状态
    bool CTWLockStatus; //主控给无线的锁状态
}MUTEX_STATUS_S;

typedef union GpsUnionData
{
    struct
    {
        u8  Ns: 1;           //维度指示 0:SOUTH 1:NORTH
        u8  Ndeg: 7;         //维度    0~89
        u8  Ew: 1;           //经度指示 0:EAST 1:WEST
        u8  Nminmm: 7;       //纬分的整数部分 0~59
        u16  Nminf;       //纬分的小数部分 0~9999
        u8  Edeg;         //经度    0~179
        u8  Eminmm;       //经分的整数部分 0~59
        u16  Eminf;       //经分的小数部分 0~9999
    }Data_bit;
    u8 Data[8];
}GPS_DATA_U;

typedef struct GpsStructData
{
    u8 DeviceID[4];
    GPS_DATA_U GpsUnion;
    u8 DateTime[14];
}GPS_DATA_S;

typedef struct VarianceStructData
{
    SIGNALLING_HEAD_S SigHead;
    u16 VarianceVal;
}VARIANCE_DATA_S;

typedef struct DaemonHeartBeat
{
    SIGNALLING_HEAD_S SigHead;
    u16 Reversed;
}DAEMON_HEART_BEAT_S;

typedef struct LogInfo
{
    SIGNALLING_HEAD_S SigHead;
    u8 LogLevel;
    u8 LogContent[512];
}LOG_INFO_S;

#pragma pack()

#endif

