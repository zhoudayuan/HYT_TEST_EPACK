/**
 * Copyright (C), 1993-2010, Hytera Comm. Co., Ltd.
 * @file    cli.h
 * @brief   为其它模块提供命令行接口，使各模块可以通过串口人机交互
 * @author  柳智奇
 * @version v1.0
 * @date    2010-10-13
 */
/*
 *   函数列表
 *   1. CLI_HexToStr                    将16进制无符号数转换成字符串
 *   2. CLI_StrToHex                    将字符串转换成16进制无符号数
 *   3. CLI_StrToDec                    将字符串转换成10进制无符号数
 *   4. CLI_RegisterCommand             注册一条命令行接口命令
 *   5. CLI_Printf                      以队列缓冲方式输出命令行接口打印信息
 *   6. CLI_Init                        初始化命令行接口模块，分配资源占用
 *   
 *   修改历史
 *   2010-10-13            建立文件                    柳智奇
 *   *************************************************************************/

/******************************************************************************
 *   引用头文件声明
 *   *************************************************************************/

/******************************************************************************
 *   编译控制宏定义
 *   *************************************************************************/
/**
 * @def _ENABLE_CLI
 * @brief 使能命令行接口功能开关
 */
#define _ENABLE_CLI  
/**
 * @def _USE_NET_NCOM
 * @brief 使用网口而非串口输入/输出
 */
//#define _USE_NET_NCOM  
/******************************************************************************
 *   类型定义
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
 * @brief 命令行接口命令最多命令个数
 */
#define CLI_NODE_NUM  200
/**
 * @def CLI_ARC_MAX
 * @brief 命令行接口命令最多参数个数
 */
#define CLI_ARC_MAX   10
/**
 * @def CLI_ARG_MAX
 * @brief 命令行接口命令参数最大字节长度
 */
#define CLI_ARG_MAX   256
/**
 * @def CLI_ROOT_NAME
 * @brief 命令行接口根节点名称
 */
#define CLI_ROOT_NAME "NAS"
/**
 * @def CLI_ROOT_DESC
 * @brief 命令行接口根节点描述文字
 */
#define CLI_ROOT_DESC "NAS command line interface"
/**
 * @enum CLI_PRINT_TYPE_E
 * @brief 打印模块
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
 * @brief 命令行接口命令节点注册结构
 */
typedef struct _CLI_CMD_T
{
    //const CHAR pcClass[16];                                ///< 父节点名称
    //const CHAR pcName[16];                                 ///< 本节点名称
    //const CHAR pcDescription[128];                          ///< 本节点描述
     CHAR pcClass[16];                                ///< 父节点名称
     CHAR pcName[16];                                 ///< 本节点名称
     CHAR pcDescription[128];                          ///< 本节点描述
    STATUS  (*pfcallback)(UINT32 u4Arc, CHAR* pcArg[]); ///< 本节点回调函数

} CLI_CMD_T;

/**
 * @struct CLI_MSG_T
 * @brief 消息队列传递消息结构
 */
typedef struct _CLI_MSG_T
{
	long msgType;
	UINT8 tmsgArray[2000];
} CLI_MSG_T;
/******************************************************************************
 *   全局变量定义
 *   *************************************************************************/

/******************************************************************************
 *   局部变量定义
 *   *************************************************************************/

/******************************************************************************
 *   内部函数实现
 *   *************************************************************************/
#define CLI_Printf(...) do{\
        char buf[2048];\
        int len;\
        len = sprintf(buf,__VA_ARGS__);\
        send(i4FifoOutFd, buf, len,0);\
}while(0);
/******************************************************************************
 *   可调用函数实现
 *   *************************************************************************/
/**
 * @brief   将16进制无符号数转换成字符串
 */
void CLI_HexToStr(UINT8* puData, UINT32 u4Data);

/**
 * @brief   将字符串转换成16进制无符号数
 */
UINT32 CLI_StrToHex(UINT8* puData);

/**
 * @brief   将字符串转换成10进制无符号数
 */
UINT32 CLI_StrToDec(UINT8* puData);

/**
 * @brief   注册一条命令行接口命令
 */
STATUS CLI_RegisterCommand(CLI_CMD_T* ptCmd);

/**
 * @brief   初始化命令行接口模块，分配资源占用
 */
void CLI_Init(void);

