/********************************************************************************************************
 * @file     feature_soft_timer.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
  * @date     May. 10, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
/*
 * app_soft_timer.c
 *
 *  Created on: 2018-10-10
 *      Author: Administrator
 */

#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "vendor/common/blt_soft_timer.h"
#include "vendor/common/blt_common.h"


#if (FEATURE_TEST_MODE == TEST_MAIN)



#define FEATURE_PM_ENABLE								1
#define FEATURE_DEEPSLEEP_RETENTION_ENABLE				1



#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8

#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16





_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};


_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};




#define		MY_RF_POWER_INDEX					RF_POWER_P3p01dBm

_attribute_ram_code_ void  func_suspend_exit (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}



void	task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //interval=10ms latency=99 timeout=4s

}

void	task_terminate (u8 e, u8 *p, int n)
{

}





int gpio_test0(void)
{
	//gpio 0 toggle to see the effect
	DBG_CHN3_TOGGLE;

	return 0;
}

_attribute_data_retention_	static u8 timer_change_flg = 0;
int gpio_test1(void)
{
	//gpio 1 toggle to see the effect
	DBG_CHN4_TOGGLE;


	timer_change_flg = !timer_change_flg;
	if(timer_change_flg){
		return 7000;
	}
	else{
		return 17000;
	}

}

int gpio_test2(void)
{
	//gpio 2 toggle to see the effect
	DBG_CHN5_TOGGLE;

	//timer last for 5 second
	if(clock_time_exceed(0, 5000000)){
		//return -1;
		//blt_soft_timer_delete(&gpio_test2);
	}
	else{

	}

	return 0;
}

int gpio_test3(void)
{
	//gpio 3 toggle to see the effect
	DBG_CHN6_TOGGLE;

	return 0;
}






void feature_main_init_normal(void)
{

	//random number generator must be initiated here( in the beginning of user_init_nromal)
	//when deepSleep retention wakeUp, no need initialize again
	random_generator_init();  //this is must


	u8  mac_public[6];
	u8  mac_random_static[6];
	blc_initMacAddress(CFG_ADR_MAC, mac_public, mac_random_static);

	rf_set_power_level_index (MY_RF_POWER_INDEX);

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();   //mandatory
	blc_ll_initStandby_module(mac_public);				//mandatory

	blc_ll_initAdvertising_module(mac_public); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initConnection_module();				//connection module  mandatory for BLE slave/master
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	////// Host Initialization  //////////
	blc_gap_peripheral_init();    //gap initialization
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	blc_smp_peripheral_init();									//smp initialization


///////////////////// USER application initialization ///////////////////
	u8 tbl_advData[] = {
		 0x08, 0x09, 't', 'e', 's', 't', 'T', 'I', 'M',
		 0x02, 0x01, 0x05,
		};
	u8	tbl_scanRsp [] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'T', 'I', 'M',
		};
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	if(status != BLE_SUCCESS){

	}

	bls_ll_setAdvEnable(1);  //adv enable

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);


	#if(FEATURE_PM_ENABLE)
		blc_ll_initPowerManagement_module();

		#if (FEATURE_DEEPSLEEP_RETENTION_ENABLE)
			bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
			blc_pm_setDeepsleepRetentionThreshold(50, 10);
			blc_pm_setDeepsleepRetentionEarlyWakeupTiming(200);
		#else
			bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
		#endif

		//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &func_suspend_enter);
		bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &func_suspend_exit);
	#else
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
	#endif





	//////////////// TEST  /////////////////////////
	//common/blt_soft_timer.h   #define		BLT_SOFTWARE_TIMER_ENABLE				1
	blt_soft_timer_init();
	blt_soft_timer_add(&gpio_test0, 23000);//23ms
	blt_soft_timer_add(&gpio_test1, 7000); //7ms <-> 17ms
	blt_soft_timer_add(&gpio_test2, 13000);//13ms
	blt_soft_timer_add(&gpio_test3, 27000);//27ms

}


_attribute_ram_code_ void feature_main_init_deepRetn(void)
{
#if (FEATURE_DEEPSLEEP_RETENTION_ENABLE)
	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();

	DBG_CHN0_HIGH;    //debug
#endif
}



#endif  //end of  (FEATURE_TEST_MODE == TEST_MAIN)
