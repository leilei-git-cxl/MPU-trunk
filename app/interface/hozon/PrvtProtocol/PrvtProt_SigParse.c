/******************************************************
�ļ�����	PrvtProt_SigParse.c

������	��ҵ˽��Э�飨�㽭���ڣ�	
Data			Vasion			author
2018/1/10		V1.0			liujian
*******************************************************/

/*******************************************************
description�� include the header file
*******************************************************/
#include "init.h"
#include "log.h"
#include "can_api.h"
#include "gps_api.h"
#include "at.h"
#include "PrvtProt_SigParse.h"
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
static PP_canSign_t PP_canSign;
static pthread_mutex_t datatx = PTHREAD_MUTEX_INITIALIZER;
/******************************************************
description�� function code
******************************************************/
/******************************************************
*��������InitPPsignFltr_Parameter

*��  �Σ�

*����ֵ��

*��  ����

*��  ע��
******************************************************/
void InitPrvtProt_SignParse_Parameter(void)
{
	memset(&PP_canSign,0,sizeof(PP_canSign_t));
}


/******************************************************
*��������PrvtProt_data_parse_surfix
*��  �Σ�
*����ֵ��
*��  ������ȡ����
*��  ע��
******************************************************/
int PrvtProt_data_parse_surfix(int sigid, const char *sfx)
{
    uint32_t pptype, ppindex;

    assert(sigid > 0 && sfx != NULL);

    if (2 != sscanf(sfx, "R%1x%3x", &pptype, &ppindex))
    {
        return 0;
    }

	pthread_mutex_lock(&datatx);
    switch (pptype)
    {
		case PP_RMTCTRL_CANSIGN:
		{
			if (ppindex >= PP_MAX_RMTCTRL_CANSIGN_INFO)
            {
                log_e(LOG_HOZON, "rmt ctrl can sign info over %d! ", ppindex);
                break;
            }
			PP_canSign.rmtCtrlSign.info[ppindex] = sigid;
		}
		break;
        default:
            log_o(LOG_HOZON, "unkonwn type %s%x", sfx ,pptype);
       break;
    }
	pthread_mutex_unlock(&datatx);

    return 5;
}

/*
 *	EHB故障及有效性
  */
unsigned char PrvtProt_SignParse_EHBFaiSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_EHBFAIST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_EHBFAIST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	系统故障灯-限功率
  */
unsigned char PrvtProt_SignParse_VCUPwrTriLvl(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCUPWRTRILVL] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCUPWRTRILVL])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	系统故障灯-限功率
  */
unsigned char PrvtProt_SignParse_VCUSysLightSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCUSYSLIGHTST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCUSYSLIGHTST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	当前最高放电故障等级
  */
unsigned char PrvtProt_SignParse_BMSDisChgFlt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BMSDISCHGFLT] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BMSDISCHGFLT])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	ESC Brake Warning Lamp sts
  */
unsigned char PrvtProt_SignParse_BkWarnLampSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ESCBRAKEWARNMAMP] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ESCBRAKEWARNMAMP])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	EGSM故障状态
  */
unsigned char PrvtProt_SignParse_EGSMErrSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_EGSMERRST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_EGSMERRST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	AVH lamp req sts
  */
unsigned char PrvtProt_SignParse_AVHLampSts(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AVHLAMPREQ] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AVHLAMPREQ])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	动力电池系统故障
  */
unsigned char PrvtProt_SignParse_DiagSts(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_DIAGST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_DIAGST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	总电流过流
  */
unsigned char PrvtProt_SignParse_overCurrSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_OVERCURRENTST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_OVERCURRENTST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	mcu2 fault level
  */
unsigned char PrvtProt_SignParse_Mcu2FltLvl(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_MCU2FAULTLVL] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_MCU2FAULTLVL])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	系统故障状态
  */
unsigned char PrvtProt_SignParse_SysFaultSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCU5SYSFAUTST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_VCU5SYSFAUTST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	BTM充电请求
  */
unsigned char PrvtProt_SignParse_BTMChrgCmd(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BTMCHARGECMD] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BTMCHARGECMD])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	HV Ready状态
  */
unsigned char PrvtProt_SignParse_HVReadySt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_HVREADY] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_HVREADY])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	空调auto状态
  */
unsigned char PrvtProt_SignParse_ACAutoSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ACAUTOST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ACAUTOST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	总计里程同步状态
  */
unsigned char PrvtProt_SignParse_OdomtrUpdtSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ODOMETERUPDATE] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ODOMETERUPDATE])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	充电枪连接状态
  */
unsigned char PrvtProt_SignParse_chrgGunCnctSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHRGGUNCNCTLIST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHRGGUNCNCTLIST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	蓝牙一键启动状态
  */
unsigned char PrvtProt_SignParse_BleStartSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BLUETOOTHSTARTST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_BLUETOOTHSTARTST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	CO2浓度报警状态
  */
unsigned char PrvtProt_SignParse_CO2DensitySt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CO2DENSITYSTS] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CO2DENSITYSTS])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	 空气净化器状态/
	pm2.5有效性״̬
  */
unsigned char PrvtProt_SignParse_pm25valid(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_PM25VALID] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_PM25VALID])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 *	 OTA模式失败原因״̬
  */
unsigned char PrvtProt_SignParse_OtaFailSts(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_OTAMODEFAILSTS] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_OTAMODEFAILSTS])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	车辆运动模式״̬
  */
unsigned char PrvtProt_SignParse_readyLightSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_READYLIGHTST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_READYLIGHTST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	充电预约使能状态״̬
  */
unsigned char PrvtProt_SignParse_chrgAptEnSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHARGEAPPOINTEN] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHARGEAPPOINTEN])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	充电ON/OFF状态
  */
unsigned char PrvtProt_SignParse_chrgOnOffSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHARGEON] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_CHARGEON])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	寻车状态״̬
  */
unsigned char PrvtProt_SignParse_findcarSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_FINDCAR] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_FINDCAR])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	天窗状态״̬
  */
unsigned char PrvtProt_SignParse_sunroofSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_SUNROOFOPEN] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_SUNROOFOPEN])->value: 0xff;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 远程启动状态״̬
  */
unsigned char PrvtProt_SignParse_RmtStartSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_HIGHVOIELEC] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_HIGHVOIELEC])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 主驾加热状态״̬
  */
unsigned char PrvtProt_SignParse_DrivHeatingSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_DRIVHEATING] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_DRIVHEATING])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 副驾驶加热状态״̬
  */
unsigned char PrvtProt_SignParse_PassHeatingSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_PASSHEATING] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_PASSHEATING])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 取消启动状态״̬
  */
unsigned char PrvtProt_SignParse_cancelEngiSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ENGIFORBID] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_ENGIFORBID])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 认证状态״̬
  */
unsigned char PrvtProt_SignParse_autheSt(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AUTHEST] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AUTHEST])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}

/*
 	 认证失败原因
  */
unsigned char PrvtProt_SignParse_authefailresion(void)
{
	unsigned char st;
	pthread_mutex_lock(&datatx);
	st = PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AUTHEFAILRESION] ?
				 dbc_get_signal_from_id(PP_canSign.rmtCtrlSign.info[PP_CANSIGN_AUTHEFAILRESION])->value: 0x0;
	pthread_mutex_unlock(&datatx);
	return st;
}
