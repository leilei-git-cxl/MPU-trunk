#include <string.h>
#include "init.h"
#include "uds_diag.h"
#include "diag.h"
#include "status_sync.h"
#include "timer.h"
#include "rds.h"
#include "uds.h"
#include "fault_sync.h"
#include "gps_api.h"
#include "dev_api.h"
#include "at_api.h"
#include "cfg_api.h"

/* GPS Ant status. match to mcu*/
enum
{
    GPSANT_UNKNOW = 0,
    GPSANT_OK,
    GPSANT_SHORT,
    GPSANT_OPEN
};

/* MIC VOLTAGE status */
enum
{
    MICV_ERROR = 0,
    MICV_OK
};

enum
{
    MIC_UNKNOW = 0,
    MIC_SHORT_GND,
    MIC_SHORT_POWER,
    MIC_OPEN,
    MIC_OK
};

/*SOS �������𣬰���״̬*/
enum
{
    SOS_UNKNOW = 0,
    SOS_UP,
    SOS_DOWN,
};

enum
{
    SPK_UNKNOW = 0,
    SPK_SHORT,
    SPK_OPEN,
    SPK_OK
};

enum
{
    ACC_R_UNKNOW = 0,
    ACC_R_OK,
    ACC_R_ERROR
};

enum
{
    BAT_POWER_UNKNOW = 0,
    BAT_POWER_OK,
    BAT_POWER_LOW,
    BAT_POWER_HIGH,
    BAT_POWER_NONE
};

/*period test*/
/****************************************************************
0 indicates no fault;       1 indicates fault
****************************************************************/
int ft_uds_diag_dev_ecall(void)
{
    int status = 0;

    status  = flt_get_by_id(SOSBTN);

    if (status == SOS_DOWN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_gps_ant_gnd(void)
{
    int status = 0;

    status  = flt_get_by_id(GPS_ANT);

    if (status == GPSANT_SHORT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_gps_ant_power(void)
{
    /*not support*/
    /*
    int status = 0;
    status  = flt_get_by_id(GPS_ANT);
    */

    return 0;
}

int ft_uds_diag_gps_ant_open(void)
{
    int status = 0;

    status  = flt_get_by_id(GPS_ANT);

    if (status == GPSANT_OPEN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_dev_gps_module(void)
{
    if (GPS_ERROR == gps_get_fix_status())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_wan_ant_gnd(void)
{
    int status = 0;

    status = dev_diag_get_ant_status(ANT_4G_MAIN);

    if (status == ANT_SHORT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_wan_ant_power(void)
{
    /*not support*/
    return 0;
}

int ft_uds_diag_wan_ant_open(void)
{
    int status = 0;

    status = dev_diag_get_ant_status(ANT_4G_MAIN);

    if (status == ANT_OPEN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_gsm(void)
{
    /*uncertain*/
    int status = 0;

    status = at_get_status();

    if ((0 == status) || (1 == status))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int ft_uds_diag_mic_gnd(void)
{
    int status = 0;

    status  = flt_get_by_id(MICSTATUS);

    if (status == MIC_SHORT_GND)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_sim(void)
{
    int status = 0;

    status = flt_get_by_id(SIM);

    if ((0 == status) || (1 == status))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*from st status*/
int ft_uds_diag_bat_low(void)
{
    int status = 0;

    status = flt_get_by_id(BATVOL);

    if (status == BAT_POWER_LOW)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

int ft_uds_diag_bat_high(void)
{
    int status = 0;

    status = flt_get_by_id(BATVOL);

    if (status == BAT_POWER_HIGH)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

/*0: unknown; 1: health; 2: unhealth; 3: no bat*/
int ft_uds_diag_bat_aged(void)
{
    int status = 0;
    status = flt_get_by_id(BAT);

    if (status == 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int ft_uds_diag_gsense(void)
{
    int status = 0;

    status = flt_get_by_id(GSENSE);

    if (status == ACC_R_ERROR)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*0: normal; 1: low; 2: high*/
/*12:[9.5, 15.5]    24:[18, 36]*/
int ft_uds_diag_power_high(void)
{
    int ret = 0;
    uint16_t power;
    unsigned char power_thred;

    unsigned int len = sizeof(power_thred);

    ret = cfg_get_para(CFG_ITEM_FT_UDS_POWER, (void *)&power_thred, &len);

    if (ret != 0)
    {
        return ret;
    }

    len = sizeof(power);

    if (st_get(ST_ITEM_POW_VOLTAGE, (void *)&power, (void *)&len) == 0)
    {
        if (power_thred == UDS_24V_POWER)
        {
            if (power > 36000)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (power_thred == UDS_12V_POWER)
        {
            if (power > 15500)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    return ret;

}

int ft_uds_diag_power_low(void)
{
    int ret = 0;
    uint16_t power;
    unsigned char power_thred;

    unsigned int len = sizeof(power_thred);

    ret = cfg_get_para(CFG_ITEM_FT_UDS_POWER, (void *)&power_thred, &len);

    if (ret != 0)
    {
        return ret;
    }

    len = sizeof(power);

    if ((st_get(ST_ITEM_POW_VOLTAGE, (void *)&power, (void *)&len)) == 0)
    {
        if (power_thred == UDS_24V_POWER)
        {
            if (power < 18000)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (power_thred == UDS_12V_POWER)
        {
            if (power < 9500)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

