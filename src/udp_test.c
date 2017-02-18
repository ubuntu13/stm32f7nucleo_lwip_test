/*
 * udp_test.c
 *
 *  Created on: 2017Äê2ÔÂ18ÈÕ
 *      Author: william
 */

#include "udp_test.h"
#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"

#include "uart.h"

/*-----------------------------------------------------------------------------------*/
static void udp_thread(void *arg)
{
  struct netconn *conn;
  struct netbuf *buf;
  char buffer[4096];
  err_t err;
  LWIP_UNUSED_ARG(arg);

#if LWIP_IPV6
  conn = netconn_new(NETCONN_UDP_IPV6);
  netconn_bind(conn, IP6_ADDR_ANY, 7);
#else /* LWIP_IPV6 */
  conn = netconn_new(NETCONN_UDP);
  netconn_bind(conn, IP_ADDR_ANY, 7);
#endif /* LWIP_IPV6 */
  LWIP_ERROR("udpecho: invalid conn", (conn != NULL), return;);

  while(1)
  {
	  err = netconn_recv(conn, &buf);
	  if(err == ERR_OK)
	  {
		  /*  no need netconn_connect here, since the netbuf contains the address */
		  if(netbuf_copy(buf, buffer, sizeof(buffer)) != buf->p->tot_len)
		  {
			  //LWIP_DEBUGF(LWIP_DBG_ON, ("netbuf_copy failed\n"));
			  MY_NOTICE("netbuf_copy failed\n");
		  }
		  else
		  {
			  buffer[buf->p->tot_len] = '\0';
			  err = netconn_send(conn, buf);
			  if(err != ERR_OK)
			  {
				  //LWIP_DEBUGF(LWIP_DBG_ON, ("netconn_send failed: %d\n", (int)err));
				  MY_PRINTF("netconn_send failed: %d\n", (int)err);
			  }
			  else
			  {
				  //LWIP_DEBUGF(LWIP_DBG_ON, ("got %s\n", buffer));
				  MY_PRINTF("got %s\n", buffer);
			  }
		  }
		  netbuf_delete(buf);
	  }
  }
}
/*-----------------------------------------------------------------------------------*/
void udp_test_init(void)
{
  sys_thread_new("udp_thread", udp_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

#endif /* LWIP_NETCONN */


