/****************************************************************
 file:         at_main.c
 description:  the header file of at main function implemention
 date:         2019/6/4
 author:       liuquanfu
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "init.h"
#include "tcom_api.h"
#include "init.h"
#include "gpio.h"
#include "timer.h"
#include "pm_api.h"
#include "at_api.h"
#include "cfg_api.h"
#include "dev_api.h"
#include "shell_api.h"
#include "scom_api.h"
#include "signal.h"
#include "ble.h"
#include "pwdg.h" 
#include "com_app_def.h"
#include "init.h"
#include "bt_interface.h"
#include "hz_bt.pb-c.h"
#include "hz_bt_usrdata.h"
#include "btsock.h"
#include "cm256_if.h"

static unsigned char g_aucReg = BT_UNAUTH;// 0 : unauth 1:auth

bt_info_t g_bt_into;

bt_recv_t g_bt_recv;
hz_protocol_t g_hz_protocol;
 /******************************************************************************
 * Function Name  : check_sum
 * Description	  :  
 * Input		  :  
 * Return		  : NONE
 ******************************************************************************/
 int check_sum(uint8_t *data, int len)
 {
	 uint8_t cs = 0;
 
	 while (len--)
	 {
		 cs ^= *data++;
	 }
 
	 return cs;
 }

 /******************************************************************************
 * Function Name  : bt_set_auth_flag
 * Description	  :  
 * Input		  :  
 * Return		  : NONE
 ******************************************************************************/
void bt_set_auth_flag(unsigned char ucAuth)
{
	g_aucReg = ucAuth;
}
/******************************************************************************
* Function Name  : bt_get_auth_flag
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
unsigned char bt_get_auth_flag(void)
{
	return g_aucReg;
}
/******************************************************************************
* Function Name  : bt_set_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
void bt_set_init(void)
{
	memset(&g_bt_into, 0, sizeof(bt_info_t));
}
/******************************************************************************
* Function Name  : bt_auth_response_set
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int bt_auth_response_set(bt_send_t        *src, uint8_t *buf, size_t *bufsz)
{
	int res = YT_OK;
    AuthenticationResponse *response;
    size_t len;
	log_i(LOG_BLE, "bt_auth_response_set");
    if ((response = (AuthenticationResponse*) calloc(1, sizeof(AuthenticationResponse))) == NULL)
    {
        
        return YT_ERR;
    }

	authentication_response__init(response);

	response->auth_result = src->auth_result; //True: auth_success  False: auth_Fail
	response->failure_reasons = src->failure_reasons;

	if (hz_pb_bytes_set(&response->ver_data, (uint8_t *)&src->ver_data1, src->ver_data1_len))
    {
        goto set_exit0;
    }
   
	len = authentication_response__get_packed_size(response);
	if (len > *bufsz)
    {
        goto set_exit0;
    }
	*bufsz = len;

	authentication_response__pack(response, buf);
	goto set_exit;

set_exit0:
    res = YT_ERR;
set_exit:
    authentication_response__free_unpacked(response, NULL);
    return res;
}

/******************************************************************************
* Function Name  : bt_set_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int bt_ack_response_set(bt_send_t        *src, uint8_t *buf, size_t *bufsz)
{
	int res = YT_OK;
    ACK *response;
    size_t len;
    if ((response = (ACK*) calloc(1, sizeof(ACK))) == NULL)
    {
        return YT_ERR;
    }

	ack__init(response);
	response->ack_state = src->ack.state;
	response->msg_type = src->ack.msg_type;
	response->execution_result = src->ack.execution_result;
	response->failure_reasons = src->ack.failure_reasons;

	log_i(LOG_BLE, "src->ack.state = %d",src->ack.state);
	log_i(LOG_BLE, "src->ack.msg_type =%d",src->ack.msg_type);
	log_i(LOG_BLE, "src->ack.execution_result =%d",src->ack.execution_result);
	log_i(LOG_BLE, "src->ack.failure_reasons =%d",src->ack.failure_reasons);
	
   
	len = ack__get_packed_size(response);
	if (len > *bufsz)
    {
        goto set_exit0;
    }
	
	*bufsz = len;

	ack__pack(response, buf);
	goto set_exit;

set_exit0:
    res = YT_ERR;
set_exit:
    ack__free_unpacked(response, NULL);
    return res;
}


/******************************************************************************
* Function Name  : bt_set_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int bt_vihe_info_response_set(bt_send_t        *src, uint8_t *buf, size_t *bufsz)
{
	int res = YT_OK;
    VehicleInfor *response;
    size_t len;
    if ((response = (VehicleInfor*) calloc(1, sizeof(VehicleInfor))) == NULL)
    {
        return YT_ERR;
    }

#if 0
	vehicle_infor__init(response);
	response->vehiclie_door_state = src->vehi_info.vehiclie_door_state;
	response->sunroof_state 	  = src->vehi_info.sunroof_state ;
	response->electric_door_state = src->vehi_info.electric_door_state;
	response->fine_car_state	  = src->vehi_info.fine_car_state;
	response->charge_state 		  = src->vehi_info.charge_state;
	response->power_state         = src->vehi_info.power_state;
    log_i(LOG_BLE, "src->vehi_info.vehiclie_door_state = %d", src->vehi_info.vehiclie_door_state);
	log_i(LOG_BLE, "src->vehi_info.sunroof_state =%d", src->vehi_info.sunroof_state);

	log_i(LOG_BLE, "src->vehi_info.electric_door_state = %d", src->vehi_info.electric_door_state);
	log_i(LOG_BLE, "src->vehi_info.fine_car_state =%d", src->vehi_info.fine_car_state);
	log_i(LOG_BLE, "src->vehi_info.charge_state =%d", src->vehi_info.charge_state);
	log_i(LOG_BLE, "src->vehi_info.power_state =%d", src->vehi_info.power_state);
#endif

	vehicle_infor__init(response);

	//response->vehiclie_door_state = src->vehi_info.vehiclie_door_state;
	response->sunroof_state 	  = src->vehi_info.sunroof_state ;
	response->electric_door_state = src->vehi_info.electric_door_state;
	response->fine_car_state	  = src->vehi_info.fine_car_state;
	//response->vehicle_lrdoor_state= src->vehi_info.vehicle_lrdoor_state;
	response->vehicle_door_state  = src->vehi_info.vehicle_door_state;
	response->remaining_mileage	  = src->vehi_info.remaining_mileage;
	response->power_control_state = src->vehi_info.power_control_state;
	response->remaining_capacity  = src->vehi_info.remaining_capacity;
	response->remaining_mileage  = src->vehi_info.remaining_mileage;
	response->total_mileage  = src->vehi_info.total_mileage;
	response->car_gear_position  = src->vehi_info.car_gear_position;
	//response->charge_state 		  = src->vehi_info.charge_state;
	//response->power_state         = src->vehi_info.power_state;
    log_i(LOG_BLE, "src->vehi_info.vehicle_door_state = %d", src->vehi_info.vehicle_door_state);
	log_i(LOG_BLE, "src->vehi_info.sunroof_state =%d", src->vehi_info.sunroof_state);

	log_i(LOG_BLE, "src->vehi_info.electric_door_state = %d", src->vehi_info.electric_door_state);
	log_i(LOG_BLE, "src->vehi_info.fine_car_state =%d", src->vehi_info.fine_car_state);
	//log_i(LOG_BLE, "src->vehi_info.charge_state =%d", src->vehi_info.charge_state);
	log_i(LOG_BLE, "src->vehi_info.power_control_state =%d", src->vehi_info.power_control_state);


	if(pb_vihe_charge_set(&response->charge_infor, src))
	{
		 goto set_exit0;
	}

	if(pb_vihe_air_set(&response->air_infor, src))
	{
		 goto set_exit0;
	}

	
	if(pb_vihe_door_set(&response->lrdoor_infor, src))
	{
		 goto set_exit0;
	}

	if(pb_vihe_tire_set(&response->tire_infor, src))
	{
		 goto set_exit0;
	}

	len = vehicle_infor__get_packed_size(response);
	if (len > *bufsz)
    {
        goto set_exit0;
    }
	
	*bufsz = len;

	vehicle_infor__pack(response, buf);
	goto set_exit;

set_exit0:
    res = YT_ERR;
set_exit:
    vehicle_infor__free_unpacked(response, NULL);
    return res;
}

/******************************************************************************
* Function Name  : bt_set_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int bt_channel_response_set(bt_send_t *src, uint8_t *buf, size_t *bufsz)
{
	int res = 0;
	ProtocolHeader *response_hd;
    size_t len;

	if ((response_hd = (ProtocolHeader*) calloc(1, sizeof(ProtocolHeader))) == NULL)
    {
        return -1;
    }
	
	log_i(LOG_BLE, "bt_channel_response_set************1");
	protocol_header__init(response_hd);

	if(pb_appliheader_set(&response_hd->head, src))
	{
		 goto set_exit0;
	}

	if(pb_TimeStamp_set(&response_hd->timestamp, src))
	{
		 goto set_exit0;
	}

	response_hd->msgcarrierlen = src->ver_data1_len;
	if (hz_pb_bytes_set(&response_hd->msgcarrier, (uint8_t *)&src->ver_data1, src->ver_data1_len))
    {
        goto set_exit0;
    }
	
	len = protocol_header__get_packed_size(response_hd);
	if (len > *bufsz)
    {
        goto set_exit0;
    }

	*bufsz = len;

	protocol_header__pack(response_hd, buf);
	goto set_exit;

set_exit0:
    res = -1;
set_exit:
    protocol_header__free_unpacked(response_hd, NULL);
    return res;
}
/******************************************************************************
* Function Name  : hz_bt_send_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int hz_bt_send_init(bt_send_t *src, uint8_t *buf, size_t *bufsz)
{
	unsigned char aucTmp[1024]={0};
	size_t len = 1024; 
	int iRet = YT_ERR;
	RTCTIME time;
    tm_get_abstime(&time);

	src->protocol_version = PROTOCOL_VER;
    src->Timestamp.year  = time.year;
	src->Timestamp.month = time.mon;
	src->Timestamp.day = time.mday;
	src->Timestamp.hour = time.hour;
	src->Timestamp.minute = time.min;
	src->Timestamp.second = time.sec;

	log_i(LOG_BLE, " src->Timestamp.year=%d", src->Timestamp.year);
	log_i(LOG_BLE, " src->Timestamp.month=%d", src->Timestamp.month);
	log_i(LOG_BLE, " src->Timestamp.day=%d", src->Timestamp.day);
	log_i(LOG_BLE, " src->Timestamp.hour=%d", src->Timestamp.hour);
	log_i(LOG_BLE, " src->Timestamp.second =%d", src->Timestamp.second);

	if (BT_AH_MSG_TYPE_SE_FUN_RESP == src->msg_type)
	{
		iRet = bt_auth_response_set(src, aucTmp, &len);
		if (YT_ERR == iRet)
		{
			log_i(LOG_BLE, "bt_auth_response_set fail");
			return YT_ERR;
		}
			
	}
	else if(APPLICATION_HEADER__MESSAGE_TYPE__ACK  == src->msg_type)
	{
		  iRet = bt_ack_response_set(src, aucTmp, &len);
		  if (YT_ERR == iRet)
		  {
			  log_i(LOG_BLE, "bt_auth_response_set fail");
			  return YT_ERR;
		  }
		
	}
	else if(APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Infor  == src->msg_type)
	{
		  iRet = bt_vihe_info_response_set(src, aucTmp, &len);
		  if (YT_ERR == iRet)
		  {
			  log_i(LOG_BLE, "bt_auth_response_set fail");
			  return YT_ERR;
		  }
	}
	else
	{
		return YT_ERR;
	}
	
	src->ver_data1_len = len;
	memcpy(src->ver_data1, aucTmp, len);
	log_i(LOG_BLE, " len =%d", len);

	if (0 == len )
	{
	    log_e(LOG_BLE, " len =%d ERR", len);
		return YT_ERR;
	}

	iRet = bt_channel_response_set(src, buf, bufsz);		
	if (YT_ERR == iRet)
	{
		return YT_ERR;
	}
	
    return YT_OK;
}
/******************************************************************************
* Function Name  : send_to_app
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int send_to_app(unsigned char *pucInData, unsigned int *pulInLen, unsigned char *pucOutData, unsigned int *pulOutLen)
{
	unsigned char aucTmp[1024] = {0};
	unsigned char ency[1024] = {0};
	int offset = 0, len = 0, ency_len = 0 , xor =0;
	int iRet = YT_ERR;

	len = strlen(HZ_P_HEAD);
	memcpy(aucTmp, HZ_P_HEAD, len);
	offset += len;

	if (BT_AUTH_SUCCESS == bt_get_auth_flag())
	{
	    if (BT_AH_MSG_TYPE_SE_FUN_RESP == g_hz_protocol.hz_send.msg_type)
	    {
	    	aucTmp[offset++] = HZ_P_NO_ENCY;
	    }
		else
		{
			aucTmp[offset++] = HZ_P_ENCY;
		}
	}
	else
	{
		
	}

	if (g_hz_protocol.ency)
	{
		if (strlen((const char *)g_hz_protocol.sekey) < 64)
		{
			log_e(LOG_BLE, "send_to_app  sekey err");  
			return YT_ERR;
		}
		
		ApiBLETraceBuf((unsigned char *)g_hz_protocol.sekey, 80);
		ApiBLETraceBuf(pucInData, *pulInLen); 
		iRet = HzBtSymEncrypt((char *)pucInData, *pulInLen, (char *)ency, &ency_len, (char *)g_hz_protocol.sekey , 1);
		log_i(LOG_BLE, "*********************4");	
		if (1 == iRet)
		{
	 		if (NULL == ency)
			{
				log_i(LOG_BLE, "chiper is null");
		  		return YT_ERR;
	    	}
			
			log_i(LOG_BLE, "ency_len = %d",ency_len);
	  		ApiBLETraceBuf(ency, (unsigned int)ency_len);
		}
		else 
		{
			return YT_ERR;
		}
			
	}
	else
	{
		ency_len = *pulInLen;
		memcpy(ency, pucInData, ency_len);
	}
	
	aucTmp[offset++] = (ency_len);
	aucTmp[offset++] = (ency_len) >> 8;

	aucTmp[offset++] = LOBYTE(g_hz_protocol.msg_cnt);
	aucTmp[offset++] = HIBYTE(g_hz_protocol.msg_cnt);
	aucTmp[offset++] = LOBYTE(HIWORD(g_hz_protocol.msg_cnt));
	aucTmp[offset++] = HIBYTE(HIWORD(g_hz_protocol.msg_cnt));

	memcpy(aucTmp + offset, ency,  ency_len);
	offset += (ency_len);

    xor = check_sum(&aucTmp[7], (offset -7));
	aucTmp[offset++] = xor;
	
	len = strlen(HZ_P_END);
	memcpy(aucTmp + offset, HZ_P_END, len);
	offset += len;

	*pulOutLen = offset;	 
	memcpy(pucOutData, aucTmp, offset);
	ApiBLETraceBuf(pucOutData, *pulOutLen);
	return YT_OK;
}
/******************************************************************************
* Function Name  : hz_bt_send_init
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int set_cmd (uint8_t *buf, size_t len)
{
	ProtocolHeader *request =  protocol_header__unpack(NULL, len, buf);
	if (NULL == request)
	{
		log_e(LOG_BLE, "protocol_header__unpack Fail");
		return YT_ERR;
	}

	log_i(LOG_BLE, "request->head->protocol_version = %d",request->head->protocol_version);
	log_i(LOG_BLE, "request->head->msg_type = %d",request->head->msg_type);
	log_i(LOG_BLE, "request->timestamp->year = %d",request->timestamp->year);
	log_i(LOG_BLE, "request->head->month = %d",request->timestamp->month);
	log_i(LOG_BLE, "request->head->day = %d",request->timestamp->day);
	log_i(LOG_BLE, "request->head->hour = %d",request->timestamp->hour);
	log_i(LOG_BLE, "request->head->minute = %d",request->timestamp->minute);
	log_i(LOG_BLE, "request->head->second = %d",request->timestamp->second);
	log_i(LOG_BLE, "request->msgcarrierlen = %d",request->msgcarrierlen);
	log_i(LOG_BLE, "request->msgcarrier.len = %d",request->msgcarrier.len);
	switch(request->head->msg_type)
	{
		case APPLICATION_HEADER__MESSAGE_TYPE__VEHICLE_DOOR:
			{
				VehicleDoor *door =  vehicle_door__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
				if (NULL == door)
				{
					log_e(LOG_BLE, "vehicle_door__unpack Fail");
					return YT_ERR;
				}
				g_hz_protocol.hz_send.ack.state = door->door_state ;
				log_i(LOG_BLE, "door->door_state = %d",door->door_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_VEH_DOOR;

				if (door)
			    {
			        vehicle_door__free_unpacked(door, NULL);
			    }
			}
			break;
		case APPLICATION_HEADER__MESSAGE_TYPE__PANORAMIC_SUNROOF:
			{
				PanoramicSunroof *sunroof =  panoramic_sunroof__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
			
				if (NULL == sunroof)
				{
					log_e(LOG_BLE, "panoramic_sunroof__unpack Fail");
					return YT_ERR;
				}
				g_hz_protocol.hz_send.ack.state =  sunroof->panoramic_sunroof_state;
				log_i(LOG_BLE, "sunroof->panoramic_sunroof_state = %d",sunroof->panoramic_sunroof_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_PAN_SUNROOf;

				if (sunroof)
			    {
			        panoramic_sunroof__free_unpacked(sunroof, NULL);
			    }
			}
			break;
		case APPLICATION_HEADER__MESSAGE_TYPE__ELECTRIC_DOOR:
			{
				ElectricDoor *elec_door =  electric_door__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
			
				if (NULL == elec_door)
				{
					log_e(LOG_BLE, "electric_door__unpack Fail");
					return YT_ERR;
				}
				g_hz_protocol.hz_send.ack.state =  elec_door->electric_door_state;
			    log_i(LOG_BLE, "elec_door->electric_door_state = %d",elec_door->electric_door_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_ELEC_DOOR;
				
				if (elec_door)
			    {
			        electric_door__free_unpacked(elec_door, NULL);
			    }
			}
			break;
		case APPLICATION_HEADER__MESSAGE_TYPE__REMOTE_FINE_CAR:
			{
				RemoteFineCar *find_car =  remote_fine_car__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
				if (NULL == find_car)
				{
					log_e(LOG_BLE, "remote_fine_car__unpack Fail");
					return YT_ERR;
				}
				g_hz_protocol.hz_send.ack.state =  find_car->fine_car_state;
				 log_i(LOG_BLE, "find_car->fine_car_state = %d",find_car->fine_car_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_REMOTE_FIND_CAR;

				if (find_car)
			    {
			        remote_fine_car__free_unpacked(find_car, NULL);
			    }
			}
			break;
		case APPLICATION_HEADER__MESSAGE_TYPE__CHARGE:
			{
				Charge *charge =  charge__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
				if (NULL == charge)
				{
					log_e(LOG_BLE, "remote_fine_car__unpack Fail");
					return YT_ERR;
				}
				g_hz_protocol.hz_send.ack.state =  charge->charge_state;
				 log_i(LOG_BLE, "charge->charge_state = %d",charge->charge_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_CHARGE;

				if (charge)
			    {
			        charge__free_unpacked(charge, NULL);
			    }
			}
			break;
		case APPLICATION_HEADER__MESSAGE_TYPE__POWER_CONTRO:
			{
				PowerControl *control =  power_control__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
				if (NULL == control)
				{
					log_e(LOG_BLE, "remote_fine_car__unpack Fail");
					return YT_ERR;
				}
				
			    g_hz_protocol.hz_send.ack.state =  control->power_state;
				log_i(LOG_BLE, "control->power_state = %d",control->power_state);
				g_hz_protocol.hz_send.ack.msg_type = BT_ACK_MSG_TYPE_POWER_CONTRO;

				if (control)
			    {
			        power_control__free_unpacked(control, NULL);
			    }
			}
			break;
		default:
		    break;
	}

	if (APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Status == request->head->msg_type)
	{
		g_hz_protocol.hz_send.msg_type =   APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Infor;
	}
	else
	{
		g_hz_protocol.hz_send.msg_type =   APPLICATION_HEADER__MESSAGE_TYPE__ACK;
	}
	if (request)
    {
        protocol_header__free_unpacked(request, NULL);
    }
	
	g_hz_protocol.type = request->head->msg_type;
	
	return YT_OK;
}
/******************************************************************************
* Function Name  : bt_send_cmd_pack
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int bt_send_cmd_pack(bt_ack_t state, bt_info_state_t indata,  uint8_t *out, size_t *out_len)
{
	int temp_len = 0;
	temp_len = 1024;
	int iRet = YT_ERR;
	unsigned char Temp[1024]={0};
	
	if (BT_AUTH_SUCCESS != bt_get_auth_flag())
	{
		return YT_ERR;
	}

	if (APPLICATION_HEADER__MESSAGE_TYPE__ACK == g_hz_protocol.hz_send.msg_type) 
	{
		g_hz_protocol.hz_send.ack.msg_type = state.msg_type;
		g_hz_protocol.hz_send.ack.state = state.state;
		g_hz_protocol.hz_send.ack.execution_result = state.execution_result;
		g_hz_protocol.hz_send.ack.failure_reasons = state.failure_reasons;
	}
	else if (APPLICATION_HEADER__MESSAGE_TYPE__Vehicle_Infor == g_hz_protocol.hz_send.msg_type) 
	{
	   #if 1
		log_i(LOG_BLE, "vehicle_door_state = %d",indata.vihe_info.vehicle_door_state);
		log_i(LOG_BLE, "sunroof_state = %d",indata.vihe_info.sunroof_state);
		log_i(LOG_BLE, "electric_door_state = %d",indata.vihe_info.electric_door_state);
		log_i(LOG_BLE, "fine_car_state = %d",indata.vihe_info.fine_car_state);
		log_i(LOG_BLE, "power_control_state = %d",indata.vihe_info.power_control_state);
		log_i(LOG_BLE, "remaining_capacity = %d",indata.vihe_info.remaining_capacity);
		//log_i(LOG_BLE, "vehicle_lrdoor_state = %d",indata.vihe_info.vehicle_lrdoor_state);
		log_i(LOG_BLE, "vehicle_air_state = %d",indata.vihe_air.vehicle_air_state);
		log_i(LOG_BLE, "air_temperature = %d",indata.vihe_air.air_temperature);
		log_i(LOG_BLE, "air_conditioning_mode = %d",indata.vihe_air.air_conditioning_mode);
		log_i(LOG_BLE, "air_condition_reservation = %d",indata.vihe_air.air_condition_reservation);
		log_i(LOG_BLE, "vehicle_mainseat_state = %d",indata.vihe_air.vehicle_mainseat_state);
		log_i(LOG_BLE, "vehicle_secondseat_state = %d",indata.vihe_air.vehicle_secondseat_state);
		log_i(LOG_BLE, "reservation_hour1 = %d",indata.vihe_air.reservation_hour1);
		log_i(LOG_BLE, "reservation_minute1 = %d",indata.vihe_air.reservation_minute1);
		log_i(LOG_BLE, "reservation_hour2 = %d",indata.vihe_air.reservation_hour2);
		log_i(LOG_BLE, "reservation_minute2 = %d",indata.vihe_air.reservation_minute2);
		log_i(LOG_BLE, "reservation_hour3 = %d",indata.vihe_air.reservation_hour3);
		log_i(LOG_BLE, "reservation_minute3 = %d",indata.vihe_air.reservation_minute3);
		log_i(LOG_BLE, "charge_state = %d",indata.vihe_charge.charge_state);
		log_i(LOG_BLE, "charge_reservation = %d",indata.vihe_charge.charge_reservation);
		log_i(LOG_BLE, "reservation_hour = %d",indata.vihe_charge.reservation_hour);
		log_i(LOG_BLE, "reservation_minute = %d",indata.vihe_charge.reservation_minute);
		log_i(LOG_BLE, "remaining_charge_hour = %d",indata.vihe_charge.remaining_charge_hour);
		log_i(LOG_BLE, "remaining_charge_minute = %d",indata.vihe_charge.remaining_charge_minute);

	#endif
		memcpy(&g_hz_protocol.hz_send.vehi_info, &indata.vihe_info,sizeof(bt_vihe_info_t));
		memcpy(&g_hz_protocol.hz_send.vehi_charge, &indata.vihe_charge,sizeof(bt_vihe_charge_t));
		memcpy(&g_hz_protocol.hz_send.vehi_air, &indata.vihe_air,sizeof(bt_vihe_air_t));
		memcpy(&g_hz_protocol.hz_send.vehi_door, &indata.vihe_door,sizeof(bt_info_state_t));
		memcpy(&g_hz_protocol.hz_send.vehi_tire, &indata.vihe_tire,sizeof(bt_vihe_tire_t));
	}
	else
	{
		return YT_ERR;
	}
	  
	if (YT_ERR == hz_bt_send_init(&g_hz_protocol.hz_send, Temp, (size_t *)&temp_len))
	{
		log_e(LOG_BLE, "hz_bt_send_init Fail");
		return YT_ERR;
	}
	
	log_i(LOG_BLE, "temp_len = %d \r\n",temp_len);	
	log_i(LOG_BLE, "g_hz_protocol.hz_send.ack.msg_type = %d",g_hz_protocol.hz_send.ack.msg_type);	
	ApiBLETraceBuf(Temp, temp_len);

	iRet = send_to_app(Temp, (unsigned int *)&temp_len, out, (unsigned int *)out_len);
	if (YT_ERR == iRet)
	{
	   return YT_ERR;
	}
	return YT_OK;
}
/******************************************************************************
* Function Name  : pb_recv_app_process
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int pb_recv_app_process(uint8_t *out, size_t *out_len, uint8_t *buf, size_t len)
{
	int iRet = YT_ERR;
	unsigned char BackInfo[1024]={0};
	unsigned char Temp[1024]={0};
	unsigned char ency[1024]={0};
	char sekey[1024]={0};
	int info_len = 0, temp_len = 0;
	int se_len = 0,ency_len = 0;
	log_i(LOG_BLE, "pb_recv_app_process");

    if ( g_hz_protocol.ency)
	{
		if( 2 == bt_get_auth_flag())
		{
			log_e(LOG_BLE, "no auth");
	   		return YT_ERR;
		}
		
	    if (64 != g_hz_protocol.se_len)
	   	{
	   		
	   		log_e(LOG_BLE, "g_hz_protocol.se_len len err");
	   		return YT_ERR;
	   	}

		log_i(LOG_BLE, "g_hz_protocol.sekey:%s",g_hz_protocol.sekey);
	    iRet = HzBtSymEncrypt((char *)buf, len, (char *)ency, &ency_len, (char *)g_hz_protocol.sekey, 0);
		 
		if (1 == iRet)
		{
			if (NULL == ency)
			{
				   log_e(LOG_BLE, "ency is null");
				   return -1;
			}

			if (ency_len < 0)
			{
				 log_e(LOG_BLE, "ency_len is null");
				 return -1;
			}
			
			log_i(LOG_BLE, "ency_len = %d",ency_len);
			log_i(LOG_BLE, "ency is succes");
			ApiBLETraceBuf(ency, ency_len);
		}
		else
		{
			log_e(LOG_BLE, "ency is fail");
			return -1;
		}

        iRet = set_cmd (ency, ency_len) ;
		if (YT_ERR == iRet)
		{
			log_e(LOG_BLE, "set_cmd Fail");
			return iRet;
		}
		 //return YT_OK;
    }
	else
	{
	    unsigned int tem_len = 18;
		unsigned char vin[20] = {0};
		cfg_get_user_para(CFG_ITEM_GB32960_VIN, vin, &tem_len);
		
		if (strlen((char *)vin) < 17)
		{
			memcpy(vin, "000000000000000", strlen("000000000000000"));
		}
		
		ProtocolHeader *request =  protocol_header__unpack(NULL, len, buf);
		if (NULL == request)
		{
			log_e(LOG_BLE, "protocol_header__unpack Fail");
			return YT_ERR;
		}

		log_i(LOG_BLE, "request->head->protocol_version = %d",request->head->protocol_version);
		log_i(LOG_BLE, "request->head->msg_type = %d",request->head->msg_type);
		log_i(LOG_BLE, "request->head->msg_type = %d",request->timestamp->year);
		log_i(LOG_BLE, "request->head->month = %d",request->timestamp->month);
		log_i(LOG_BLE, "request->head->day = %d",request->timestamp->day);
		log_i(LOG_BLE, "request->head->hour = %d",request->timestamp->hour);
		log_i(LOG_BLE, "request->head->minute = %d",request->timestamp->minute);
		log_i(LOG_BLE, "request->head->second = %d",request->timestamp->second);
		log_i(LOG_BLE, "request->msgcarrierlen = %d",request->msgcarrierlen);
		log_i(LOG_BLE, "request->msgcarrier.len = %d",request->msgcarrier.len);
		
	    if (APPLICATION_HEADER__MESSAGE_TYPE__VEHICLE_SECURITY_FUNC == request->head->msg_type)
	    {
	        unsigned char url_adr[30] = "/usrdata/pem/tbox.crl";
			AuthenticationInfo *auth_info =  authentication_info__unpack(NULL, request->msgcarrierlen, request->msgcarrier.data);
			if (auth_info->authentication_data.len < 50)
			{
				log_e(LOG_BLE, "get cert Fail");
				return YT_ERR;
			}

			memset(Temp, 0, sizeof(Temp));
			memcpy(Temp, (char *)auth_info->authentication_data.data,  auth_info->authentication_data.len);
			ApiBLETraceBuf((unsigned char *)Temp, auth_info->authentication_data.len);
			iRet = HzRequestInfo ((char *)Temp, (char *)&url_adr, (char *)&vin, NULL, NULL, NULL,  (char *)BackInfo, &info_len, sekey, &se_len);
			log_i(LOG_BLE, "iRet = %d",iRet);
			if (1 ==iRet)
			{
				log_i(LOG_BLE, "HzRequestInfo sucess");
				bt_set_auth_flag(BT_AUTH_SUCCESS);
			}
			else
			{
				log_e(LOG_BLE, "HzRequestInfo Fail");
				bt_set_auth_flag(BT_AUTH_FAIL);
			}

			if ((0 == se_len) || (0 == info_len))
			{
				log_e(LOG_BLE, "no key Fail or no return");
				bt_set_auth_flag(BT_AUTH_FAIL);
			}

			
	        g_hz_protocol.hz_send.msg_type =   BT_AH_MSG_TYPE_SE_FUN_RESP;

			
			//g_hz_protocol.hz_send.auth_result = 1; //True: auth_success  False: auth_Fail
			g_hz_protocol.hz_send.auth_result = bt_get_auth_flag(); //True: auth_success  False: auth_Fail

			if( 2 == bt_get_auth_flag())
			{
				g_hz_protocol.hz_send.failure_reasons = 1;
			}
			else
			{
				g_hz_protocol.hz_send.failure_reasons = 0;
			}
			
			g_hz_protocol.hz_send.ver_data1_len = info_len;
			g_hz_protocol.type = request->head->msg_type;
			memcpy(g_hz_protocol.hz_send.ver_data1, BackInfo, info_len);
			memset(g_hz_protocol.sekey, 0, sizeof(g_hz_protocol.sekey));
			g_hz_protocol.se_len = request->head->msg_type;
			memcpy(g_hz_protocol.sekey, sekey, se_len);
			g_hz_protocol.se_len = se_len;
	        log_i(LOG_BLE, "g_hz_protocol.hz_send.ver_data1_len = %d",info_len);
			ApiBLETraceBuf(g_hz_protocol.hz_send.ver_data1, info_len);
			log_i(LOG_BLE, "se_len = %d",se_len);
			ApiBLETraceBuf((unsigned char *)sekey, se_len);
			
	        temp_len = 1024;
			memset(Temp, 0, sizeof(Temp));
			if (YT_ERR == hz_bt_send_init(&g_hz_protocol.hz_send, Temp, (size_t *)&temp_len))
			{
				log_e(LOG_BLE, "hz_bt_send_init Fail");
				return YT_ERR;
			}

			log_i(LOG_BLE, "temp_len = %d",temp_len);  
			ApiBLETraceBuf(Temp, temp_len);
			
			iRet = send_to_app(Temp, (unsigned int *)&temp_len, out, (unsigned int *)out_len);
			if (YT_ERR == iRet)
			{
				return YT_ERR;
			}

			if (request)
		    {
		        protocol_header__free_unpacked(request, NULL);
		    }
	    }
	}
	return YT_OK;
}
/******************************************************************************
* Function Name  : test_bt_hz
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
void test_bt_hz(void)
{
	unsigned char aucOut[1024]={0};
	int iOutLen = 0;
	unsigned char aucbuf[1024]={0};
	int bufsz = 29;
	memcpy(aucbuf, "\x0a\x04\x08\x01\x10\x01\x12\x0c\x08\x13\x10\x07\x18\x04\x20\x0d\x28\x1c\x30\x2f\x18\x03\x22\x03\x73\x6f\x73",bufsz);
    printf("test_bt_hz");
	pb_recv_app_process(aucOut, (size_t *)&iOutLen,aucbuf, bufsz);
	printf("test_bt_hz2");
}
void reset_hz_data(void)
{
	memset(&g_hz_protocol, 0, sizeof(hz_protocol_t));
	bt_set_auth_flag(BT_UNAUTH);
}

int check_start(unsigned char *pucInData,unsigned int *pulInLen, unsigned int *pulOutLen)
{
	int i = 0, inlen = *pulInLen;
	
	for(i = 0; i < inlen; i++)	
	{
		if (0 != strncmp((const char *)(pucInData+i),"#START*", 7))
		{
			//log_e(LOG_BLE, "hz_protocol_process Head fail");
			continue;
		}
		else
		{
		    *pulOutLen = i;
			return YT_OK;
		}
		
	}
	return YT_ERR;
}

/******************************************************************************
* Function Name  : hz_protocol_process
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int hz_protocol_process1(unsigned char *pucInData,unsigned int *pulInLen,unsigned char *pucOutData,unsigned int *pulOutLen)
{
	unsigned char aucbuf[1024]={0};
	int bufsz = 29;
	int iRet = -1 , xor = 0;
	unsigned int index =0;
	unsigned char root_path[1024]={0};
	unsigned char sec_path[1024]={0};

	sprintf((char * __restrict__)&root_path, "%s", COM_APP_PEM_ROOT_DIR) ;
	sprintf((char * __restrict__)&sec_path, "%s",  COM_APP_PEM_TPONE_DIR) ;
	iRet = HzBtCertcfg((char *)&root_path, (char *)&sec_path);
	if (iRet)
	{
		log_i(LOG_BLE, "HzBtCertcfg sucess");
	}
	else
	{
		log_e(LOG_BLE, "HzBtCertcfg Fail");
		return YT_ERR;
	}

	showversion((char *)aucbuf);
	log_i(LOG_BLE, "the version+++++++++++++++++[%s]",aucbuf);



	if (*pulInLen <19)
	{
		log_e(LOG_BLE, "len err");
		return YT_ERR;
	}

#if 1
	if (YT_OK != check_start(pucInData, pulInLen, &index))
	{
		log_e(LOG_BLE, "no find start");
		return YT_ERR;
	}
#endif

	bufsz =(((int)(pucInData[index+9]<<8))&0xFF00) | (((int)(pucInData[index+8]))&0xFF);
	log_i(LOG_BLE, "pucInData[9] = %x",pucInData[index+9]);
	log_i(LOG_BLE, "pucInData[8] = %x",pucInData[index+8]);
	log_i(LOG_BLE, "bufsz = %x\r\n",bufsz);

	if (bufsz != (*pulInLen - 20 - index))
	{
		log_e(LOG_BLE, "hz_protocol_process len sum err");
		return YT_ERR;
	}

	if (0 != strncmp((const char *)(pucInData+index),"#START*", 7))
	{
		log_e(LOG_BLE, "hz_protocol_process Head fail");
		return YT_ERR;
	}

    printf("END = %x",pucInData[*pulInLen - 5 - index]);
	if (0 != strncmp((const char *)&pucInData[*pulInLen - 5 -index], "#END*", 5))
	{
		log_e(LOG_BLE, "hz_protocol_process End fail");
		return YT_ERR;
	}	

	g_hz_protocol.ency = pucInData[index+7];
	log_i(LOG_BLE, "g_hz_protocol.ency = %d",g_hz_protocol.ency);

	g_hz_protocol.msg_cnt = MAKEDWORD(pucInData[index + 13],pucInData[index + 12],pucInData[index + 11],pucInData[index + 10]);
	
	log_i(LOG_BLE, "g_hz_protocol.msg_cnt = %d",g_hz_protocol.msg_cnt);

	xor = check_sum(&pucInData[index + 7], (bufsz + 14 -7));

	log_i(LOG_BLE, "pucInData[*pulInLen - 5] = %x",pucInData[*pulInLen - 5 - index]);
	if (pucInData[*pulInLen - 6 - index] != xor)
	{
		log_e(LOG_BLE, "hz_protocol_process xor fail");
		return YT_ERR;
	}
	
	iRet = pb_recv_app_process(pucOutData, pulOutLen, &pucInData[14 + index], bufsz);
	if(YT_ERR == iRet)
	{
		return YT_ERR;
	}
     
	log_i(LOG_BLE, "*pulOutLen = %d",*pulOutLen);  
	ApiBLETraceBuf(pucOutData, *pulOutLen);
	return YT_OK;
}

/******************************************************************************
* Function Name  : hz_protocol_process
* Description	 :	
* Input 		 :	
* Return		 : NONE
******************************************************************************/
int hz_protocol_process(unsigned char *pucInData,unsigned int *pulInLen,unsigned char *pucOutData,unsigned int *pulOutLen)
{
	unsigned char aucbuf[1024]={0};
	int bufsz = 29;
	int iRet = -1 , xor = 0;
	//unsigned int index =0;
	unsigned char root_path[1024]={0};
	unsigned char sec_path[1024]={0};

	sprintf((char * __restrict__)&root_path, "%s", COM_APP_PEM_ROOT_DIR) ;
	sprintf((char * __restrict__)&sec_path, "%s",  COM_APP_PEM_TPONE_DIR) ;
	iRet = HzBtCertcfg((char *)&root_path, (char *)&sec_path);
	if (iRet)
	{
		log_i(LOG_BLE, "HzBtCertcfg sucess");
	}
	else
	{
		log_e(LOG_BLE, "HzBtCertcfg Fail");
		return YT_ERR;
	}

	showversion((char *)aucbuf);
	log_i(LOG_BLE, "the version+++++++++++++++++[%s]",aucbuf);



	if (*pulInLen <19)
	{
		log_e(LOG_BLE, "len err");
		return YT_ERR;
	}

#if 0
	if (YT_OK != check_start(pucInData, pulInLen, &index))
	{
		log_e(LOG_BLE, "no find start");
		return YT_ERR;
	}
#endif

	bufsz =(((int)(pucInData[9]<<8))&0xFF00) | (((int)(pucInData[8]))&0xFF);
	log_i(LOG_BLE, "pucInData[9] = %x",pucInData[9]);
	log_i(LOG_BLE, "pucInData[8] = %x",pucInData[8]);
	log_i(LOG_BLE, "bufsz = %x\r\n",bufsz);

	if (bufsz != (*pulInLen - 20 ))
	{
		log_e(LOG_BLE, "hz_protocol_process len sum err");
		return YT_ERR;
	}

	if (0 != strncmp((const char *)pucInData,"#START*", 7))
	{
		log_e(LOG_BLE, "hz_protocol_process Head fail");
		return YT_ERR;
	}

    printf("END = %x",pucInData[*pulInLen - 5]);
	if (0 != strncmp((const char *)&pucInData[*pulInLen - 5], "#END*", 5))
	{
		log_e(LOG_BLE, "hz_protocol_process End fail");
		ApiBLETraceBuf(pucInData, *pulInLen);
		return YT_ERR;
	}	

	g_hz_protocol.ency = pucInData[7];
	log_i(LOG_BLE, "g_hz_protocol.ency = %d",g_hz_protocol.ency);

	g_hz_protocol.msg_cnt = MAKEDWORD(pucInData[13],pucInData[12],pucInData[11],pucInData[10]);
	
	log_i(LOG_BLE, "g_hz_protocol.msg_cnt = %d",g_hz_protocol.msg_cnt);

	xor = check_sum(&pucInData[7], (bufsz+14 -7));

	log_i(LOG_BLE, "pucInData[*pulInLen - 5] = %x",pucInData[*pulInLen - 5]);
	if (pucInData[*pulInLen - 6] != xor)
	{
		log_e(LOG_BLE, "hz_protocol_process xor fail");
		return YT_ERR;
	}
	
	iRet = pb_recv_app_process(pucOutData, pulOutLen, &pucInData[14], bufsz);
	if(YT_ERR == iRet)
	{
		return YT_ERR;
	}
     
	log_i(LOG_BLE, "*pulOutLen = %d",*pulOutLen);  
	ApiBLETraceBuf(pucOutData, *pulOutLen);
	return YT_OK;
}

