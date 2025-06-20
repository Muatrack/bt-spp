/********************************************************************************************************
 * @file     app_att.c 
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
#include "tl_common.h"

#include "stack/ble/ble.h"


#if(FEATURE_TEST_MODE == TEST_GATT_SECURITY)
	//NOTICE:  ATT table defined in the file: feature_gatt_security.c
#else

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

static const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

static const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

static const u16 reportRefUUID = GATT_UUID_REPORT_REF;

static const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

static const u16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC;

static const u16 serviceChangeUUID = GATT_UUID_SERVICE_CHANGE;

static const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

static const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

static const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

static const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

static const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;

static const u16 my_appearanceUIID = GATT_UUID_APPEARANCE;

static const u16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM;

static const u16 my_appearance = GAP_APPEARE_UNKNOWN;

static const gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

static const u16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;

static u16 serviceChangeVal[2] = {0};

static u8 serviceChangeCCC[2] = {0,0};

static const u8 my_devName[] = {'F','e','a','t','u','r','e'};

static const u8 my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};



//////////////////////// OTA  ////////////////////////////////////////////////////
static const  u8 my_OtaUUID[16]					    = TELINK_SPP_DATA_OTA;
static const  u8 my_OtaServiceUUID[16]				= TELINK_OTA_UUID_SERVICE;
static u8 my_OtaData 						        = 0x00;
static const  u8 my_OtaName[] 						= {'O', 'T', 'A'};


#if(FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)
	////////////////////// SPP ////////////////////////////////////
	static const u8 TelinkSppServiceUUID[16]	      	    = TELINK_SPP_UUID_SERVICE;
	static const u8 TelinkSppDataServer2ClientUUID[16]      = TELINK_SPP_DATA_SERVER2CLIENT;
	static const u8 TelinkSppDataClient2ServerUUID[16]      = TELINK_SPP_DATA_CLIENT2SERVER;

	// Spp data from Server to Client characteristic variables
	static u8 SppDataServer2ClientDataCCC[2]  				= {0};
	//this array will not used for sending data(directly calling HandleValueNotify API), so cut array length from 20 to 1, saving some SRAM
	static u8 SppDataServer2ClientData[1] 					= {0};  //SppDataServer2ClientData[20]
	// Spp data from Client to Server characteristic variables
	//this array will not used for receiving data(data processed by Attribute Write CallBack function), so cut array length from 20 to 1, saving some SRAM
	static u8 SppDataClient2ServerData[1] 					= {0};  //SppDataClient2ServerData[20]

	//SPP data descriptor
	static const u8 TelinkSPPS2CDescriptor[] 		 		= "Telink SPP: Module->Phone";
	static const u8 TelinkSPPC2SDescriptor[]        		= "Telink SPP: Phone->Module";


	//// GAP attribute values
	static const u8 my_devNameCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
		U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
	};
	static const u8 my_appearanceCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
		U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
	};
	static const u8 my_periConnParamCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
		U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
	};


	//// GATT attribute values
	static const u8 my_serviceChangeCharVal[5] = {
		CHAR_PROP_INDICATE,
		U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
		U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
	};


	//// device Information  attribute values
	static const u8 my_PnCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
		U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
	};


	//// Telink spp  attribute values
	static const u8 TelinkSppDataServer2ClientCharVal[19] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(SPP_SERVER_TO_CLIENT_DP_H), U16_HI(SPP_SERVER_TO_CLIENT_DP_H),
		TELINK_SPP_DATA_SERVER2CLIENT
	};
	static const u8 TelinkSppDataClient2ServerCharVal[19] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(SPP_CLIENT_TO_SERVER_DP_H), U16_HI(SPP_CLIENT_TO_SERVER_DP_H),
		TELINK_SPP_DATA_CLIENT2SERVER
	};


	//// OTA attribute values
	static const u8 my_OtaCharVal[19] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
		TELINK_SPP_DATA_OTA
	};

	extern int module_onReceiveData(rf_packet_att_write_t *p);

	static const attribute_t my_Attributes[] = {

		{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute


		// 0001 - 0007  gap
		{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(u8*)(&my_characterUUID), (u8*)(my_devNameCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_devName), (u8*)(&my_devNameUUID), (u8*)(my_devName), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID), (u8*)(my_appearanceCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID), (u8*)(my_periConnParamCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},


		// 0008 - 000b gatt
		{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(u8*)(&my_characterUUID), 		(u8*)(my_serviceChangeCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (u8*)(&serviceChangeUUID), 	(u8*)(&serviceChangeVal), 0},
		{0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},


		// 000c - 000e  device Information Service
		{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_devServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_PnCharVal),(u8*)(&my_characterUUID), (u8*)(my_PnCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},


	    // 000f - 0016 SPP
		{8,ATT_PERMISSIONS_READ,2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&TelinkSppServiceUUID), 0},
	    // server to client TX
		{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSppDataServer2ClientCharVal),(u8*)(&my_characterUUID), 		(u8*)(TelinkSppDataServer2ClientCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,16,sizeof(SppDataServer2ClientData),(u8*)(&TelinkSppDataServer2ClientUUID),  (u8*)(SppDataServer2ClientData), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&SppDataServer2ClientDataCCC)},
		{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPS2CDescriptor),(u8*)&userdesc_UUID,(u8*)(&TelinkSPPS2CDescriptor)},
		// client to server RX
		{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSppDataClient2ServerCharVal),(u8*)(&my_characterUUID), 		(u8*)(TelinkSppDataClient2ServerCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(SppDataClient2ServerData),(u8*)(&TelinkSppDataClient2ServerUUID), (u8*)(SppDataClient2ServerData), &module_onReceiveData},	//value
		{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPC2SDescriptor),(u8*)&userdesc_UUID,(u8*)(&TelinkSPPC2SDescriptor)},


		////////////////////////////////////// OTA /////////////////////////////////////////////////////
		// 0017 - 001A OTA
		{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ, 2, sizeof(my_OtaCharVal),(u8*)(&my_characterUUID), (u8*)(my_OtaCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, &otaRead},			//value
		{0,ATT_PERMISSIONS_READ, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},

	};
#else

	//////////////////////// Battery /////////////////////////////////////////////////
	static const u16 my_batServiceUUID        = SERVICE_UUID_BATTERY;
	static const u16 my_batCharUUID       	  = CHARACTERISTIC_UUID_BATTERY_LEVEL;
	static u8 batteryValueInCCC[2];
	static u8 my_batVal[1] 					  = {99};

	//////////////////////// HID /////////////////////////////////////////////////////
	static const u16 my_hidServiceUUID        = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;

	static const u16 hidServiceUUID           = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;
	static const u16 hidProtocolModeUUID      = CHARACTERISTIC_UUID_HID_PROTOCOL_MODE;
	static const u16 hidReportUUID            = CHARACTERISTIC_UUID_HID_REPORT;
	static const u16 hidReportMapUUID         = CHARACTERISTIC_UUID_HID_REPORT_MAP;
	static const u16 hidbootKeyInReportUUID   = CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT;
	static const u16 hidbootKeyOutReportUUID  = CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT;
	static const u16 hidbootMouseInReportUUID = CHARACTERISTIC_UUID_HID_BOOT_MOUSE_INPUT;
	static const u16 hidinformationUUID       = CHARACTERISTIC_UUID_HID_INFORMATION;
	static const u16 hidCtrlPointUUID         = CHARACTERISTIC_UUID_HID_CONTROL_POINT;
	static const u16 hidIncludeUUID           = GATT_UUID_INCLUDE;

	static const u8 protocolMode = DFLT_HID_PROTOCOL_MODE;


	// Key in Report characteristic variables
	static u8 reportKeyIn[8];
	static u8 reportKeyInCCC[2];
	// HID Report Reference characteristic descriptor, key input
	static u8 reportRefKeyIn[2] =
				 { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_INPUT };

	// Key out Report characteristic variables
	static u8 reportKeyOut[1];
	static u8 reportKeyOutCCC[2];
	static u8 reportRefKeyOut[2] =
				 { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_OUTPUT };

	// Consumer Control input Report
	static u8 reportConsumerControlIn[2];
	static u8 reportConsumerControlInCCC[2];
	static u8 reportRefConsumerControlIn[2] = { HID_REPORT_ID_CONSUME_CONTROL_INPUT, HID_REPORT_TYPE_INPUT };



	// Boot Keyboard Input Report
	static u8 bootKeyInReport;
	static u8 bootKeyInReportCCC[2];

	// Boot Keyboard Output Report
	static u8 bootKeyOutReport;


	// HID Information characteristic
	static const u8 hidInformation[] =
	{
	  U16_LO(0x0111), U16_HI(0x0111),             // bcdHID (USB HID version)
	  0x00,                                       // bCountryCode
	  0x01                                        // Flags
	};

	// HID Control Point characteristic
	static u8 controlPoint;

	// HID Report Map characteristic
	// Keyboard report descriptor (using format for Boot interface descriptor)

	static const u8 reportMap[] =
	{
		//keyboard report in
		0x05, 0x01,     // Usage Pg (Generic Desktop)
		0x09, 0x06,     // Usage (Keyboard)
		0xA1, 0x01,     // Collection: (Application)
		0x85, HID_REPORT_ID_KEYBOARD_INPUT,     // Report Id (keyboard)
					  //
		0x05, 0x07,     // Usage Pg (Key Codes)
		0x19, 0xE0,     // Usage Min (224)  VK_CTRL:0xe0
		0x29, 0xE7,     // Usage Max (231)  VK_RWIN:0xe7
		0x15, 0x00,     // Log Min (0)
		0x25, 0x01,     // Log Max (1)
					  //
					  // Modifier byte
		0x75, 0x01,     // Report Size (1)   1 bit * 8
		0x95, 0x08,     // Report Count (8)
		0x81, 0x02,     // Input: (Data, Variable, Absolute)
					  //
					  // Reserved byte
		0x95, 0x01,     // Report Count (1)
		0x75, 0x08,     // Report Size (8)
		0x81, 0x01,     // Input: (static constant)

		//keyboard output
		//5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
		0x95, 0x05,    //Report Count (5)
		0x75, 0x01,    //Report Size (1)
		0x05, 0x08,    //Usage Pg (LEDs )
		0x19, 0x01,    //Usage Min
		0x29, 0x05,    //Usage Max
		0x91, 0x02,    //Output (Data, Variable, Absolute)
		//3 bit reserved
		0x95, 0x01,    //Report Count (1)
		0x75, 0x03,    //Report Size (3)
		0x91, 0x01,    //Output (static constant)

		// Key arrays (6 bytes)
		0x95, 0x06,     // Report Count (6)
		0x75, 0x08,     // Report Size (8)
		0x15, 0x00,     // Log Min (0)
		0x25, 0xF1,     // Log Max (241)
		0x05, 0x07,     // Usage Pg (Key Codes)
		0x19, 0x00,     // Usage Min (0)
		0x29, 0xf1,     // Usage Max (241)
		0x81, 0x00,     // Input: (Data, Array)

		0xC0,            // End Collection




		//consumer report in
		0x05, 0x0C,   // Usage Page (Consumer)
		0x09, 0x01,   // Usage (Consumer Control)
		0xA1, 0x01,   // Collection (Application)
		0x85, HID_REPORT_ID_CONSUME_CONTROL_INPUT,   //     Report Id
		0x75,0x10,     //global, report size 16 bits
		0x95,0x01,     //global, report count 1
		0x15,0x01,     //global, min  0x01
		0x26,0x8c,0x02,  //global, max  0x28c
		0x19,0x01,     //local, min   0x01
		0x2a,0x8c,0x02,  //local, max    0x28c
		0x81,0x00,     //main,  input data varible, absolute
		0xc0,        //main, end collection

	};

	// HID External Report Reference Descriptor for report map
	static u16 extServiceUUID;

	/////////////////////////////////////////////////////////
	static const u8 my_AudioUUID[16]   = TELINK_AUDIO_UUID_SERVICE;
	static const u8 my_MicUUID[16]		= TELINK_MIC_DATA;
	static const u8 my_SpeakerUUID[16]	= TELINK_SPEAKER_DATA;

	static u8 my_MicData 		= 0x80;
	static u8 my_SpeakerData 	= 0x81;

	static const u8  my_MicName[] = {'M', 'i', 'c'};
	static const u8  my_SpeakerName[] = {'S', 'p', 'e', 'a', 'k', 'e', 'r'};

	// Include attribute (Battery service)
	static const u16 include[3] = {BATT_PS_H, BATT_LEVEL_INPUT_CCB_H, SERVICE_UUID_BATTERY};


	//// GAP attribute values
	static const u8 my_devNameCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
		U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
	};
	static const u8 my_appearanceCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
		U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
	};
	static const u8 my_periConnParamCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
		U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
	};


	//// GATT attribute values
	static const u8 my_serviceChangeCharVal[5] = {
		CHAR_PROP_INDICATE,
		U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
		U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
	};


	//// device Information  attribute values
	static const u8 my_PnCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
		U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
	};


	//// HID attribute values
	static const u8 my_hidProtocolModeCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(HID_PROTOCOL_MODE_DP_H), U16_HI(HID_PROTOCOL_MODE_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE), U16_HI(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE)
	};
	static const u8 my_hidbootKeyInReporCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(HID_BOOT_KB_REPORT_INPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_INPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT)
	};
	static const u8 my_hidbootKeyOutReporCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(HID_BOOT_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_OUTPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT)
	};
	static const u8 my_hidReportCCinCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(HID_CONSUME_REPORT_INPUT_DP_H), U16_HI(HID_CONSUME_REPORT_INPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
	};
	static const u8 my_hidReportKEYinCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(HID_NORMAL_KB_REPORT_INPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_INPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
	};
	static const u8 my_hidReportKEYoutCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(HID_NORMAL_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_OUTPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
	};
	static const u8 my_hidReportMapCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(HID_REPORT_MAP_DP_H), U16_HI(HID_REPORT_MAP_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_REPORT_MAP), U16_HI(CHARACTERISTIC_UUID_HID_REPORT_MAP)
	};
	static const u8 my_hidinformationCharVal[5] = {
		CHAR_PROP_READ,
		U16_LO(HID_INFORMATION_DP_H), U16_HI(HID_INFORMATION_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_INFORMATION), U16_HI(CHARACTERISTIC_UUID_HID_INFORMATION)
	};
	static const u8 my_hidCtrlPointCharVal[5] = {
		CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(HID_CONTROL_POINT_DP_H), U16_HI(HID_CONTROL_POINT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_HID_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_HID_CONTROL_POINT)
	};


	//// Battery attribute values
	static const u8 my_batCharVal[5] = {
		CHAR_PROP_READ | CHAR_PROP_NOTIFY,
		U16_LO(BATT_LEVEL_INPUT_DP_H), U16_HI(BATT_LEVEL_INPUT_DP_H),
		U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
	};


	//// OTA attribute values
	static const u8 my_OtaCharVal[19] = {
		CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
		U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
		TELINK_SPP_DATA_OTA,
	};

	// TM : to modify
	static const attribute_t my_Attributes[] = {

		{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute


		// 0001 - 0007  gap
		{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(u8*)(&my_characterUUID), (u8*)(my_devNameCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_devName), (u8*)(&my_devNameUUID), (u8*)(my_devName), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(u8*)(&my_characterUUID), (u8*)(my_appearanceCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(u8*)(&my_characterUUID), (u8*)(my_periConnParamCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},


		// 0008 - 000b gatt
		{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gattServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(u8*)(&my_characterUUID), 		(u8*)(my_serviceChangeCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (u8*)(&serviceChangeUUID), 	(u8*)(&serviceChangeVal), 0},
		{0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(serviceChangeCCC), 0},

		// 000c - 000e  device Information Service
		{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_devServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_PnCharVal),(u8*)(&my_characterUUID), (u8*)(my_PnCharVal), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},

		////////////////////////////////////////////////- 485/ttl Data -/////////////////////////////////////////////

		////////////////////////////////////////////////- onewire/i2c Data -/////////////////////////////////////////
		////////////////////////////////////////////////- onewire/i2c Switch -///////////////////////////////////////

		////////////////////////////////////////////////////- hc595 -////////////////////////////////////////////////

		////////////////////////////////////// Battery Service //////////////////////////////////////////////////////
		// 002a - 002d
		{4,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_batServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_batCharVal),(u8*)(&my_characterUUID), (u8*)(my_batCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_READ,2,sizeof(my_batVal),(u8*)(&my_batCharUUID), 	(u8*)(my_batVal), 0},	//value
		{0,ATT_PERMISSIONS_RDWR,2,sizeof(batteryValueInCCC),(u8*)(&clientCharacterCfgUUID), 	(u8*)(batteryValueInCCC), 0},	//value

		////////////////////////////////////// OTA /////////////////////////////////////////////////////
		// 002e - 0031
		{4,ATT_PERMISSIONS_READ, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
		{0,ATT_PERMISSIONS_READ, 2, sizeof(my_OtaCharVal),(u8*)(&my_characterUUID), (u8*)(my_OtaCharVal), 0},				//prop
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, &otaRead},			//value
		{0,ATT_PERMISSIONS_READ, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},
	};
#endif


void	my_att_init (void)
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);
}

#endif

