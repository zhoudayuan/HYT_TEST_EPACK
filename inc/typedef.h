/******************************************************************************
 * Copyright(C), 1993-2009, HYT
 * File name: odp_internal.h
 * Author: zhiqi.liu    Version:  0.1alpha      Date: 2009-07-23
 * Description:
 *     本文件包含平台相关定义
 * Others:
 *
 * History:
 * 1. Date:
 * Author:
 * Modification:
******************************************************************************/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

/** @defgroup systype 1. 平台相关定义 */
/** @{ */
#define VOID           void

/** @defgroup 变量类型定义 */
/** @{ */
#define NO_ERR                      (0)     ///< 数据成功
#define ERR                         (1)     ///< 数据错误


#ifndef NULL
#define NULL           ((void*)0)
#endif

#ifndef BOOL
typedef int            BOOL;
#endif
#ifndef TRUE
#define TRUE           ((BOOL) 1)
#endif
#ifndef FALSE
#define FALSE          ((BOOL) 0)
#endif

typedef unsigned long  UINT32;
typedef signed long    INT32;
typedef unsigned short UINT16;
typedef signed short   INT16;
typedef unsigned char  UINT8;
typedef signed char    INT8;
typedef char           CHAR;

typedef int            STATUS;
typedef void*          HANDLE_T;
/** @} */

#endif /* _TYPEDEF_H_ */

