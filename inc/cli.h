/**
 * Copyright (C), 1993-2010, Hytera Comm. Co., Ltd.
 * @file    cli.h
 * @brief   Ϊ����ģ���ṩ�����нӿڣ�ʹ��ģ�����ͨ�������˻�����
 * @author  ������
 * @version v1.0
 * @date    2010-10-13
 */
/*
 *   �����б�
 *   1. CLI_HexToStr                    ��16�����޷�����ת�����ַ���
 *   2. CLI_StrToHex                    ���ַ���ת����16�����޷�����
 *   3. CLI_StrToDec                    ���ַ���ת����10�����޷�����
 *   4. CLI_RegisterCommand             ע��һ�������нӿ�����
 *   5. CLI_Printf                      �Զ��л��巽ʽ��������нӿڴ�ӡ��Ϣ
 *   6. CLI_Init                        ��ʼ�������нӿ�ģ�飬������Դռ��
 *   
 *   �޸���ʷ
 *   2010-10-13            �����ļ�                    ������
 *   *************************************************************************/

/******************************************************************************
 *   ����ͷ�ļ�����
 *   *************************************************************************/

/******************************************************************************
 *   ������ƺ궨��
 *   *************************************************************************/
/**
 * @def _ENABLE_CLI
 * @brief ʹ�������нӿڹ��ܿ���
 */
#define _ENABLE_CLI  
/**
 * @def _USE_NET_NCOM
 * @brief ʹ�����ڶ��Ǵ�������/���
 */
//#define _USE_NET_NCOM  
/******************************************************************************
 *   ���Ͷ���
 *   *************************************************************************/
 /**
 * @def OSR_OK
 * @brief 
 */
 #define OSR_OK       ((STATUS)0)
 /**
 * @def OSR_OK
 * @brief
 */
#define OSR_FAIL     ((STATUS)-1)
/**
 * @def CLI_NODE_NUM
 * @brief �����нӿ���������������
 */
#define CLI_NODE_NUM  200
/**
 * @def CLI_ARC_MAX
 * @brief �����нӿ���������������
 */
#define CLI_ARC_MAX   10
/**
 * @def CLI_ARG_MAX
 * @brief �����нӿ������������ֽڳ���
 */
#define CLI_ARG_MAX   256
/**
 * @def CLI_ROOT_NAME
 * @brief �����нӿڸ��ڵ�����
 */
#define CLI_ROOT_NAME "NAS"
/**
 * @def CLI_ROOT_DESC
 * @brief �����нӿڸ��ڵ���������
 */
#define CLI_ROOT_DESC "NAS command line interface"
/**
 * @enum CLI_PRINT_TYPE_E
 * @brief ��ӡģ��
 */
typedef enum _CLI_PRINT_TYPE_E
{
    CLI_PRINT_TYPE_ROUTE              = 0,                      ///
    CLI_PRINT_TYPE_DLL        = 1,                      ///
    CLI_PRINT_TYPE_CCL       = 2,                      ///
    CLI_PRINT_TYPE_MGR                  = 3  ,                  ///
    CLI_PRINT_TYPE_LOG                  = 4 ,                  ///
    CLI_PRINT_TYPE_CLI                  = 5,                  ///
} CLI_PRINT_TYPE_E;

/**
 * @struct CLI_CMD_T
 * @brief �����нӿ�����ڵ�ע��ṹ
 */
typedef struct _CLI_CMD_T
{
    //const CHAR pcClass[16];                                ///< ���ڵ�����
    //const CHAR pcName[16];                                 ///< ���ڵ�����
    //const CHAR pcDescription[128];                          ///< ���ڵ�����
     CHAR pcClass[16];                                ///< ���ڵ�����
     CHAR pcName[16];                                 ///< ���ڵ�����
     CHAR pcDescription[128];                          ///< ���ڵ�����
    STATUS  (*pfcallback)(UINT32 u4Arc, CHAR* pcArg[]); ///< ���ڵ�ص�����

} CLI_CMD_T;

/**
 * @struct CLI_MSG_T
 * @brief ��Ϣ���д�����Ϣ�ṹ
 */
typedef struct _CLI_MSG_T
{
	long msgType;
	UINT8 tmsgArray[2000];
} CLI_MSG_T;
/******************************************************************************
 *   ȫ�ֱ�������
 *   *************************************************************************/

/******************************************************************************
 *   �ֲ���������
 *   *************************************************************************/

/******************************************************************************
 *   �ڲ�����ʵ��
 *   *************************************************************************/
#define CLI_Printf(...) do{\
        char buf[2048];\
        int len;\
        len = sprintf(buf,__VA_ARGS__);\
        send(i4FifoOutFd, buf, len,0);\
}while(0);
/******************************************************************************
 *   �ɵ��ú���ʵ��
 *   *************************************************************************/
/**
 * @brief   ��16�����޷�����ת�����ַ���
 */
void CLI_HexToStr(UINT8* puData, UINT32 u4Data);

/**
 * @brief   ���ַ���ת����16�����޷�����
 */
UINT32 CLI_StrToHex(UINT8* puData);

/**
 * @brief   ���ַ���ת����10�����޷�����
 */
UINT32 CLI_StrToDec(UINT8* puData);

/**
 * @brief   ע��һ�������нӿ�����
 */
STATUS CLI_RegisterCommand(CLI_CMD_T* ptCmd);

/**
 * @brief   ��ʼ�������нӿ�ģ�飬������Դռ��
 */
void CLI_Init(void);

