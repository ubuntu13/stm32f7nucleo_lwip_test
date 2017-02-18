//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "main.h"
#include "cmsis_os.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "app_ethernet.h"
#include "httpserver-netconn.h"
#include "udp_test.h"

//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"
//#include "queue.h"

struct netif gnetif; /* network interface structure */

static void StartThread(void const * argument);
static void Netif_Config(void);
static void MPU_Config(void);

int main(void)
{
	/* Configure the MPU attributes as Device memory for ETH DMA descriptors */
	MPU_Config();

	LED_Init();

	uart5_init(115200);

	/* Init thread */
#if defined(__GNUC__)
	osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
#else
	  osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
#endif

	osThreadCreate (osThread(Start), NULL);

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	for( ;; );
}

void LED_thread(void const * argument)
{
	while(1)
	{
		vTaskDelay(500/portTICK_RATE_MS); // 2Hz
		LED_Toggle(LED2);
	}
}

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
static void StartThread(void const * argument)
{
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);

  /* Initialize the LwIP stack */
  Netif_Config();

  /* Initialize webserver demo */
  //http_server_netconn_init();

  udp_test_init();

  /* Notify user about the network interface config */
  User_notification(&gnetif);

#ifdef USE_DHCP
  /* Start DHCPClient */
  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(DHCP), &gnetif);
#endif

  osThreadDef(LED, LED_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 1);
  osThreadCreate (osThread(LED), NULL);

  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(NULL);
  }
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */

  /* add the network interface */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
}

/**
  * @brief  Configure the MPU attributes as Device for  Ethernet Descriptors in the SRAM1.
  * @note   The Base Address is 0x20020000 since this memory interface is the AXI.
  *         The Configured Region Size is 256B (size of Rx and Tx ETH descriptors)
  *
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as Device for Ethernet Descriptors in the SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20020000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

//void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
//{
//	/* This function will get called if a task overflows its stack.   If the
//	parameters are corrupt then inspect pxCurrentTCB to find which was the
//	offending task. */
//
//	( void ) pxTask;
//	( void ) pcTaskName;
//
//	for( ;; );
//}

// ----------------------------------------------------------------------------
