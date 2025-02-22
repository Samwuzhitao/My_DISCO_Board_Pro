/**
  ******************************************************************************
  * @file    tcp_client.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011 
  * @brief   tcp echoclient application using LwIP RAW API
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

u8_t  recev_buf[50];
char wendu[5] = {"18.9"};
__IO uint32_t message_count=0;

u8_t   data[1024];

char Rec_Date[500];

struct tcp_pcb *echoclient_pcb;


/* ECHO protocol states */
enum echoclient_states
{
  ES_NOT_CONNECTED = 0,
  ES_CONNECTED,
  ES_RECEIVED,
  ES_CLOSING,
};


/* structure to be passed as argument to the tcp callbacks */
struct echoclient
{
  enum echoclient_states state; /* connection status */
	//u8_t retries;
  struct tcp_pcb *pcb;          /* pointer on the current tcp_pcb */
  struct pbuf *p_tx;            /* pointer on pbuf to be transmitted */
};


/* Private function prototypes -----------------------------------------------*/
//static err_t tcp_client_accpt(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct echoclient * es);
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_client_send(struct tcp_pcb *tpcb, struct echoclient * es);
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Connects to the TCP echo server
  * @param  None
  * @retval None
  */
void tcp_client_connect(void)
{
  struct ip_addr DestIPaddr;
 
	IP4_ADDR( &DestIPaddr, 42,96,164,52 );
	
  /* create new tcp pcb */
  echoclient_pcb = tcp_new();
       
  if (echoclient_pcb != NULL)
  {
		err_t err;
		
		err = tcp_bind(echoclient_pcb,IP_ADDR_ANY,80);
    
    if(  err == ERR_OK )	
		{		
				//echoclient_pcb = tcp_listen(echoclient_pcb);
			  /* connect to destination address/port */
			  tcp_connect(echoclient_pcb,&DestIPaddr,80,tcp_client_connected);
				//tcp_accept(echoclient_pcb,tcp_client_accpt);
			
		}
		else
		{
			/* abort ? */
		}
  }
	else
	{
		 /* abort ? */
	}
}

//static err_t tcp_client_accpt(void *arg, struct tcp_pcb *newpcb, err_t err)
//{
//		err_t err;
//		struct echoclient 
//	
//}
static void TCP_Printf_State( struct echoclient *es )
{
	printf("\r\n Current TCP Link State :");
	switch(es->state)
	{
		case ES_NOT_CONNECTED:
			printf("ES_NOT_CONNECTED\r\n"); break;
		case ES_CONNECTED:
			printf("ES_CONNECTED\r\n"); break;
		case ES_RECEIVED:
			printf("ES_RECEIVED\r\n"); break;
		case ES_CLOSING:
			printf("ES_CLOSING\r\n"); break;
		default: break;
	}
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  struct echoclient *es = NULL;

	
  if (err == ERR_OK)   
  {
    /* allocate structure es to maintain tcp connection informations */
    es = (struct echoclient *)mem_malloc(sizeof(struct echoclient));
  
		TCP_Printf_State(es);
		
    if (es != NULL)
    {
      es->state = ES_CONNECTED;
      es->pcb = tpcb;
      
			data[0] = 0;
			strcat((char*)data, "POST /v1.0/device/343196/sensor/380844/datapoints/ HTTP/1.1\r\n");
			strcat((char*)data, "Host:api.yeelink.net\r\n");
			strcat((char*)data, "Accept:*/*\r\n");
			strcat((char*)data, "U-ApiKey:b8dbd5d8030f459b59f66a88d6c7aedf\r\n"); 
			strcat((char*)data, "Content-Length:15\r\n");
			strcat((char*)data, "Content-type:application/x-www-form-urlencoded\r\n");
			strcat((char*)data, "Connection:  Keep-Alive\r\n");
			strcat((char*)data, "\r\n");
			strcat((char*)data, "{\"value\":");
			strcat((char*)data, wendu);
			strcat((char*)data, "}");
			strcat((char*)data, "\r\n");
			
      /* allocate pbuf */
      es->p_tx = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)data) , PBUF_POOL);
         
      if (es->p_tx)
      {       
        /* copy data to pbuf */
        pbuf_take(es->p_tx, (char*)data, strlen((char*)data));
        
        /* pass newly allocated es structure as argument to tpcb */
        tcp_arg(tpcb, es);
  
        /* initialize LwIP tcp_recv callback function */ 
        tcp_recv(tpcb, tcp_client_recv);
  
        /* initialize LwIP tcp_sent callback function */
        tcp_sent(tpcb, tcp_client_sent);
  
        /* initialize LwIP tcp_poll callback function */
        tcp_poll(tpcb, tcp_client_poll, 1);
    
        /* send data */
        tcp_client_send(tpcb,es);
				
				/* close connection */
				tcp_client_connection_close(tpcb, es);
        
        return ERR_OK;
      }
    }
//    else
//    {
//      /* close connection */
//      tcp_client_connection_close(tpcb, es);
//      
//      /* return memory allocation error */
//      return ERR_MEM;  
//    }
  }
//  else
//  {
//    /* close connection */
//    tcp_client_connection_close(tpcb, es);
//  }
//  return err;
}
    
/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback 
  * @param tpcb: tcp connection control block 
  * @param err: receive error code 
  * @retval err_t: retuned error  
  */
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
  struct echoclient *es;
  err_t ret_err;
  

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct echoclient *)arg;
	
	TCP_Printf_State(es);
	
  // 如果没有这句的调用，网页端将无法接受的数据。 
	tcp_recved(tpcb, p->tot_len); //接收pbuf数据	
	
  /* if we receive an empty tcp frame from server => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p_tx == NULL)
    {
       /* we're done sending, close connection */
       tcp_client_connection_close(tpcb, es);
    }
    else
    {    
      /* send remaining data*/
      tcp_client_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from echo server but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_CONNECTED)
  {
    /* increment message count */
    message_count++;
         
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len); 

		strcpy(Rec_Date,p->payload);
		
		
    pbuf_free(p);
    tcp_client_connection_close(tpcb, es);
    ret_err = ERR_OK;
  }

  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief function used to send data
  * @param  tpcb: tcp control block
  * @param  es: pointer on structure of type echoclient containing info on data 
  *             to be sent
  * @retval None 
  */
static void tcp_client_send(struct tcp_pcb *tpcb, struct echoclient * es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
	TCP_Printf_State(es);
	
  while ((wr_err == ERR_OK) &&
         (es->p_tx != NULL) && 
         (es->p_tx->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p_tx;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 0);
		
		//printf("TCP_write function State:%d\r\n\r\n",wr_err);
    //printf(ptr->payload);
		
    if (wr_err == ERR_OK)
    { 
      /* continue with next pbuf in chain (if any) */
      es->p_tx = ptr->next;
      
      if(es->p_tx != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p_tx);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later, defer to poll */
     es->p_tx = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct echoclient *es;

  es = (struct echoclient*)arg;
	
	TCP_Printf_State(es);
	
  if (es != NULL)
  {
    if (es->p_tx != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_client_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /* close tcp connection */
        tcp_client_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent 
  * @retval err_t: returned error code
  */
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct echoclient *es;

  LWIP_UNUSED_ARG(len);

  es = (struct echoclient *)arg;
 
	TCP_Printf_State(es);
	
  if(es->p_tx != NULL)
  {
    /* still got pbufs to send */
    tcp_client_send(tpcb, es);
  }

  return ERR_OK;
}

/**
  * @brief This function is used to close the tcp connection with server
  * @param tpcb: tcp connection control block
  * @param es: pointer on echoclient structure
  * @retval None
  */
static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct echoclient * es )
{
  /* remove callbacks */
  tcp_recv(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_poll(tpcb, NULL,0);
	
	TCP_Printf_State(es);
	
  if (es != NULL)
  {
    mem_free(es);
  }

  /* close tcp connection */
  tcp_close(tpcb);
	
 	TCP_Printf_State(es);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
