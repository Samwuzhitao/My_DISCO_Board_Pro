																			   /**
  ******************************************************************************
  * @file    helloworld.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   This file contains all the functions prototypes for the helloworld.c 
  *          file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HELLOWERLOD_H
#define __HELLOWERLOD_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "lwip/tcp.h"
#include <string.h>
/* Includes ------------------------------------------------------------------*/


/** @defgroup helloworld_Exported_Functions
  * @{
  */
  /* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static err_t CMD_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static err_t CMD_accept(void *arg, struct tcp_pcb *pcb, err_t err);
static void  CMD_conn_err(void *arg, err_t err);
//static void CMD_Handler(struct name *name,struct tcp_pcb *pcb,struct r_ctrl *r_ctrl);
void CMD_init(void);

/**
  * @}
  */



#ifdef __cplusplus
}
#endif

#endif /* __HELLOWERLOD_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

