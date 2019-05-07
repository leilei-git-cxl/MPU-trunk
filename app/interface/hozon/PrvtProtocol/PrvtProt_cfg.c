/******************************************************
�ļ�����	PrvtProt_cfg.c

������	��ҵ˽��Э�飨�㽭���ڣ�	
Data			Vasion			author
2018/1/10		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "gps_api.h"
#include "../sockproxy/sockproxy_data.h"
#include "PrvtProt_cfg.h"
/*******************************************************
description�� global variable definitions
*******************************************************/

/*******************************************************
description�� static variable definitions
*******************************************************/


/*******************************************************
description�� function declaration
*******************************************************/
/*Global function declaration*/

/*Static function declaration*/

/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������PrvtProt_rcvMsg
*��  �Σ�
*����ֵ��
*��  ������ȡ����
*��  ע��
******************************************************/
int PrvtProtCfg_rcvMsg(unsigned char* buf,int buflen)
{
	return RdSockproxyData_Queue(SP_PRIV,buf,buflen);
}

/******************************************************
*��������PrvtProtCfg_ecallTriggerEvent
*��  �Σ�
*����ֵ��
*��  ������ȡecall����״̬
*��  ע��
******************************************************/
int PrvtProtCfg_ecallTriggerEvent(void)
{
	return 0;
}

/******************************************************
*��������PrvtProtCfg_engineSt
*��  �Σ�
*����ֵ��
*��  ������ȡ������״̬:1-Ϩ��;2-����
*��  ע��
******************************************************/
int PrvtProtCfg_engineSt(void)
{
	return 2;
}

/******************************************************
*��������PrvtProtCfg_totalOdoMr
*��  �Σ�
*����ֵ��
*��  ������ȡ���
*��  ע��
******************************************************/
int PrvtProtCfg_totalOdoMr(void)
{
	return 100;
}

/******************************************************
*��������PrvtProtCfg_gpsStatus
*��  �Σ�
*����ֵ��
*��  ������ȡgps״̬
*��  ע��
******************************************************/
int PrvtProtCfg_gpsStatus(void)
{
	int ret = 0;
	if(gps_get_fix_status() == 2)
	{
		ret = 1;
	}
	
	return ret;	
}

/******************************************************
*��������PrvtProtCfg_gpsData
*��  �Σ�
*����ֵ��
*��  ������ȡgps����
*��  ע��
******************************************************/
void PrvtProtCfg_gpsData(PrvtProtcfg_gpsData_t *gpsDt)
{
	GPS_DATA gps_snap;

	gps_get_snap(&gps_snap);
	gpsDt->time = gps_snap.time;
	gpsDt->date = gps_snap.date;
	gpsDt->latitude = gps_snap.latitude;
	gpsDt->is_north = gps_snap.is_north;
	gpsDt->longitude = gps_snap.longitude;
	gpsDt->is_east = gps_snap.is_east;
	gpsDt->knots = gps_snap.knots;
	gpsDt->direction = gps_snap.direction;
	gpsDt->height = gps_snap.msl;
	gpsDt->hdop = gps_snap.hdop;
	gpsDt->kms = gps_snap.kms;
}