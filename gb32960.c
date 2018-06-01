/**
 * @file       	gb32960.c
 * @brief      	gb32960 report and storage manager file
 * @details 	this module will process all the gbdata to fullreport and manager the storagefile in flash disk
 * @author    	tychen
 * @date       	2018.5
 * @version 	1.0
 * @par 	
 * @par
 */

#include "framework/gb32960.h"

#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <string.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <sys/vfs.h>

#define LOG(format, ...) //fprintf(stderr, "[CLOUD_GB32960][%d]:"format, __LINE__, ##__VA_ARGS__)
#define INFO(format, ...) fprintf(stderr, "[CLOUD_GB32960][%d]:"format, __LINE__, ##__VA_ARGS__)

#define CHECK_RES(x,o,e) {if(!(x)) {LOG("ERROR %d \n", o); e;}}

char *
ackstr(enum cloudack_t ack)
{
	switch (ack)
	{
	case ACK_OK:
		return "ACK_OK";
	case ACK_ERR:
		return "ACK_ERRFAIL";
	case ACK_DUPVIN:
		return "ACK_ERRDUPVIN";
	case ACK_CMD:
		return "ACK_PKGCMD";
	default:
		return "ACK_UNKNOW";
	}
}

char *
cmdstr(enum cloudcmd_t cmd)
{
	switch (cmd)
	{
	case CMD_VEH_SIGNIN:
		return "CMD_VEH_SIGNIN";
	case CMD_RT_UPLOAD:
		return "CMD_RT_UPLOAD";
	case CMD_FIX_UPLOAD:
		return "CMD_FIX_UPLOAD";
	case CMD_VEH_SIGNOUT:
		return "CMD_VEH_SIGOUT";
	case CMD_PLT_SIGNIN:
		return "CMD_PLT_SIGNIN";
	case CMD_PLT_SIGNOUT:
		return "CMD_PLT_SIGOUT";
	default:
		return "CMD_UNKNOW";
	}
}

char *
reportstr(enum cloudmsg_reporttype_t cmd)
{
	switch (cmd)
	{
	case REPORT_FULLVEHICLE:
		return "REPORT_FULLVEHICLE";
	case REPORT_MOTO:
		return "REPORT_MOTO";
	case REPORT_FUELCELL:
		return "REPORT_FUELCELL";
	case REPORT_ENGINE:
		return "REPORT_ENGINE";
	case REPORT_LOCATION:
		return "REPORT_LOCATION";
	case REPORT_ABSMAX:
		return "REPORT_ABSMAX";
	case REPORT_ALARM:
		return "REPORT_ALARM";
	default:
		return "REPORT_UNKNOW";
	}
}

char *
gbidstr(enum cloudgbid_t id)
{
	switch (id)
	{
	case GBID_FC_VELSTATUS:
		return "GBID_FC_VELSTATUS";
	case GBID_FC_CHGSTATUS:
		return "GBID_FC_CHGSTATUS";
	case GBID_FC_RUNSTATUS:
		return "GBID_FC_RUNSTATUS";
	case GBID_FC_VELSPEED:
		return "GBID_FC_VELSPEED";
	case GBID_FC_VELMILEAGE:
		return "GBID_FC_VELMILEAGE";
	case GBID_FC_VELVOL:
		return "GBID_FC_VELVOL";
	case GBID_FC_VELCUR:
		return "GBID_FC_VELCUR";
	case GBID_FC_SOC:
		return "GBID_FC_SOC";
	case GBID_FC_DCDC:
		return "GBID_FC_DCDC";
	case GBID_FC_GEAR:
		return "GBID_FC_GEAR";
	case GBID_FC_ISO:
		return "GBID_FC_ISO";
	case GBID_FC_DRVTAP:
		return "GBID_FC_DRVTAP";
	case GBID_FC_BREAKTAP:
		return "GBID_FC_BREAKTAP";
		
	case GBID_MT_CNT:
		return "GBID_MT_CNT";
		
	case GBID_MT0_SEQ:
		return "GBID_MT0_SEQ";
	case GBID_MT0_STATE:
		return "GBID_MT0_STATE";
	case GBID_MT0_DRVTEMP:
		return "GBID_MT0_DRVTEMP";
	case GBID_MT0_SPEED:
		return "GBID_MT0_SPEED";
	case GBID_MT0_TORQUE:
		return "GBID_MT0_TORQUE";
	case GBID_MT0_MTTEMP:
		return "GBID_MT0_MTTEMP";
	case GBID_MT0_DRVVOL:
		return "GBID_MT0_DRVVOL";
	case GBID_MT0_DRVCUR:
		return "GBID_MT0_DRVCUR";
		
	case GBID_MT1_SEQ:
		return "GBID_MT1_SEQ";
	case GBID_MT1_STATE:
		return "GBID_MT1_STATE";
	case GBID_MT1_DRVTEMP:
		return "GBID_MT1_DRVTEMP";
	case GBID_MT1_SPEED:
		return "GBID_MT1_SPEED";
	case GBID_MT1_TORQUE:
		return "GBID_MT1_TORQUE";
	case GBID_MT1_MTTEMP:
		return "GBID_MT1_MTTEMP";
	case GBID_MT1_DRVVOL:
		return "GBID_MT1_DRVVOL";
	case GBID_MT1_DRVCUR:
		return "GBID_MT1_DRVCUR";
		
		//TODO MT1 MT2 MT3
	case GBID_MT2_STATE:
		return "GBID_MT2_STATE";
	case GBID_MT3_STATE:
		return "GBID_MT3_STATE";
		
	case GBID_FU_BATTEMPCNT:
		return "GBID_FU_BATTEMPCNT";
	case GBID_FU_BATTEMP0:
		return "GBID_FU_BATTEMP0";
	case GBID_FU_BATTEMP1:
		return "GBID_FU_BATTEMP1";
	case GBID_FU_BATTEMP2:
		return "GBID_FU_BATTEMP2";
	case GBID_FU_VOL:
		return "GBID_FU_VOL";
	case GBID_FU_CUR:
		return "GBID_FU_CUR";
	case GBID_FU_REDUCERATE:
		return "GBID_FU_REDUCERATE";
	case GBID_FU_MAXTEMP:
		return "GBID_FU_MAXTEMP";
	case GBID_FU_MAXTEMPID:
		return "GBID_FU_MAXTEMPID";
	case GBID_FU_MAXRATIO:
		return "GBID_FU_MAXRATIO";
	case GBID_FU_MAXRATIOID:
		return "GBID_FU_MAXRATIOID";
	case GBID_FU_MAXPRESS:
		return "GBID_FU_MAXPRESS";
	case GBID_FU_MAXPRESSID:
		return "GBID_FU_MAXPRESSID";
	case GBID_FU_HVDCDC:
		return "GBID_FU_HVDCDC";
		
	case GBID_EG_STATE:
		return "GBID_EG_STATE";
	case GBID_EG_SPEED:
		return "GBID_EG_SPEED";
	case GBID_EG_REDUCERATE:
		return "GBID_EG_REDUCERATE";
		
	case GBID_LO_FIXSTATE:
		return "GBID_LO_FIXSTATE";
	case GBID_LO_LONG:
		return "GBID_LO_LONG";
	case GBID_LO_LAT:
		return "GBID_LO_LAT";
		
	case GBID_MM_MAXVCSID:
		return "GBID_MM_MAXVCSID";
	case GBID_MM_MAXVCID:
		return "GBID_MM_MAXVCID";
	case GBID_MM_MAXVC:
		return "GBID_MM_MAXVC";
	case GBID_MM_MINVCSID:
		return "GBID_MM_MINVCSID";
	case GBID_MM_MINVCID:
		return "GBID_MM_MINVCID";
	case GBID_MM_MINVC:
		return "GBID_MM_MINVC";
	case GBID_MM_MAXTCSID:
		return "GBID_MM_MAXTCSID";
	case GBID_MM_MAXTCID:
		return "GBID_MM_MAXTCID";
	case GBID_MM_MAXTC:
		return "GBID_MM_MAXTC";
	case GBID_MM_MINTCSID:
		return "GBID_MM_MINTCSID";
	case GBID_MM_MINTCID:
		return "GBID_MM_MINTCID";
	case GBID_MM_MINTC:
		return "GBID_MM_MINTC";
		
	case GBID_EA_MAXLEVEL:
		return "GBID_EA_MAXLEVEL";
	case GBID_EA_GPFLAG:
		return "GBID_EA_GPFLAG";
	case GBID_EA_BATCNT:
		return "GBID_EA_BATCNT";
	case GBID_EA_BAT0:
		return "GBID_EA_BAT0";
	case GBID_EA_BAT1:
		return "GBID_EA_BAT1";
	case GBID_EA_MTCNT:
		return "GBID_EA_MTCNT";
	case GBID_EA_MT0:
		return "GBID_EA_MT0";
	case GBID_EA_MT1:
		return "GBID_EA_MT1";
	case GBID_EA_EGCNT:
		return "GBID_EA_EGCNT";
	case GBID_EA_EG0:
		return "GBID_EA_EG0";
	case GBID_EA_EG1:
		return "GBID_EA_EG1";
	case GBID_EA_OTCNT:
		return "GBID_EA_OTCNT";
	case GBID_EA_OT0:
		return "GBID_EA_OT0";
	case GBID_EA_OT1:
		return "GBID_EA_OT1";
	default:
		return "ID_UNKNOW";
	}
}
;

bool cloudmsg_fullreport_setval(struct cloudmsg_fullreport_t *report,
        uint32_t id, uint32_t value)
{
	switch (id)
	{
//FULLVEHINFO
	case GBID_FC_VELSTATUS:
		report->fullvehicleinfo.vehiclestate = value;
		break;
	case GBID_FC_CHGSTATUS:
		report->fullvehicleinfo.chgstate = value;
		break;
	case GBID_FC_RUNSTATUS:
		report->fullvehicleinfo.runmode = value;
		break;
	case GBID_FC_VELSPEED:
		report->fullvehicleinfo.speed = value;
		break;
	case GBID_FC_VELMILEAGE:
		report->fullvehicleinfo.accumulationkms = value;
		break;
	case GBID_FC_VELVOL:
		report->fullvehicleinfo.totalvol = value;
		break;
	case GBID_FC_VELCUR:
		report->fullvehicleinfo.totalcur = value;
		break;
	case GBID_FC_SOC:
		report->fullvehicleinfo.SOCvalue = value;
		break;
	case GBID_FC_DCDC:
		report->fullvehicleinfo.dcdcstate = value;
		break;
	case GBID_FC_GEAR:
		report->fullvehicleinfo.gearstate = value;
		break;
	case GBID_FC_ISO:
		report->fullvehicleinfo.isolateres = value;
		break;
	case GBID_FC_DRVTAP:
		report->fullvehicleinfo.drvtap = value;
		break;
	case GBID_FC_BREAKTAP:
		report->fullvehicleinfo.breaktap = value;
		break;
		
//MOTOINFO
	case GBID_MT_CNT:
		report->motorinfo.count = value;
		break;
		
	case GBID_MT0_SEQ:
		report->motorinfo.moto[0].seq = value;
		break;
	case GBID_MT0_STATE:
		report->motorinfo.moto[0].state = value;
		break;
	case GBID_MT0_DRVTEMP:
		report->motorinfo.moto[0].drivertemp = value;
		break;
	case GBID_MT0_SPEED:
		report->motorinfo.moto[0].speedrpm = value;
		break;
	case GBID_MT0_TORQUE:
		report->motorinfo.moto[0].torque = value;
		break;
	case GBID_MT0_MTTEMP:
		report->motorinfo.moto[0].mototemp = value;
		break;
	case GBID_MT0_DRVVOL:
		report->motorinfo.moto[0].drivervolin = value;
		break;
	case GBID_MT0_DRVCUR:
		report->motorinfo.moto[0].drivercurin = value;
		break;
		
	case GBID_MT1_SEQ:
		report->motorinfo.moto[1].seq = value;
		break;
	case GBID_MT1_STATE:
		report->motorinfo.moto[1].state = value;
		break;
	case GBID_MT1_DRVTEMP:
		report->motorinfo.moto[1].drivertemp = value;
		break;
	case GBID_MT1_SPEED:
		report->motorinfo.moto[1].speedrpm = value;
		break;
	case GBID_MT1_TORQUE:
		report->motorinfo.moto[1].torque = value;
		break;
	case GBID_MT1_MTTEMP:
		report->motorinfo.moto[1].mototemp = value;
		break;
	case GBID_MT1_DRVVOL:
		report->motorinfo.moto[1].drivervolin = value;
		break;
	case GBID_MT1_DRVCUR:
		report->motorinfo.moto[1].drivercurin = value;
		break;
		
//FULLCELLINFO
	case GBID_FU_BATTEMPCNT:
		report->fuelcellinfo.batterytempcnt = value;
		break;
	case GBID_FU_BATTEMP0:
		report->fuelcellinfo.batterytemp[0] = value;
		break;
	case GBID_FU_BATTEMP1:
		report->fuelcellinfo.batterytemp[1] = value;
		break;
	case GBID_FU_BATTEMP2:
		report->fuelcellinfo.batterytemp[2] = value;
		break;
	case GBID_FU_VOL:
		report->fuelcellinfo.vol = value;
		break;
	case GBID_FU_CUR:
		report->fuelcellinfo.cur = value;
		break;
	case GBID_FU_REDUCERATE:
		report->fuelcellinfo.reducerate = value;
		break;
	case GBID_FU_MAXTEMP:
		report->fuelcellinfo.hydrogenmaxtemp = value;
		break;
	case GBID_FU_MAXTEMPID:
		report->fuelcellinfo.hydrogenmaxtempid = value;
		break;
	case GBID_FU_MAXRATIO:
		report->fuelcellinfo.hydrogenmaxratio = value;
		break;
	case GBID_FU_MAXRATIOID:
		report->fuelcellinfo.hydrogenmaxratioid = value;
		break;
	case GBID_FU_MAXPRESS:
		report->fuelcellinfo.hydrogenmaxpressure = value;
		break;
	case GBID_FU_MAXPRESSID:
		report->fuelcellinfo.hydrogenmaxpressureid = value;
		break;
	case GBID_FU_HVDCDC:
		report->fuelcellinfo.highvoldcdcstate = value;
		break;
		
//ENGINEINFO
	case GBID_EG_STATE:
		report->engineinfo.state = value;
		break;
	case GBID_EG_SPEED:
		report->engineinfo.speedrpm = value;
		break;
	case GBID_EG_REDUCERATE:
		report->engineinfo.reducerate = value;
		break;
		
//LOCATIONINFO
	case GBID_LO_FIXSTATE:
		report->locationinfo.fixstate = value;
		break;
	case GBID_LO_LONG:
		report->locationinfo.longitude = value;
		break;
	case GBID_LO_LAT:
		report->locationinfo.latitude = value;
		break;
		
//MANMINCELLINFO
	case GBID_MM_MAXVCSID:
		report->minmaxcellinfo.maxvolcellsysid = value;
		break;
	case GBID_MM_MAXVCID:
		report->minmaxcellinfo.maxvolcellid = value;
		break;
	case GBID_MM_MAXVC:
		report->minmaxcellinfo.maxvolcellvalue = value;
		break;
	case GBID_MM_MINVCSID:
		report->minmaxcellinfo.minvolcellsysid = value;
		break;
	case GBID_MM_MINVCID:
		report->minmaxcellinfo.minvolcellid = value;
		break;
	case GBID_MM_MINVC:
		report->minmaxcellinfo.minvolcellvalue = value;
		break;
	case GBID_MM_MAXTCSID:
		report->minmaxcellinfo.maxtempcellsysid = value;
		break;
	case GBID_MM_MAXTCID:
		report->minmaxcellinfo.maxtempcellid = value;
		break;
	case GBID_MM_MAXTC:
		report->minmaxcellinfo.maxtempcellvalue = value;
		break;
	case GBID_MM_MINTCSID:
		report->minmaxcellinfo.mintempcellsysid = value;
		break;
	case GBID_MM_MINTCID:
		report->minmaxcellinfo.mintempcellid = value;
		break;
	case GBID_MM_MINTC:
		report->minmaxcellinfo.mintempcellvalue = value;
		break;
		
//ALARM
	case GBID_EA_MAXLEVEL:
		report->alarminfo.maxalarmlevel = value;
		break;
	case GBID_EA_GPFLAG:
		report->alarminfo.commonalarmflag = value;
		break;
	case GBID_EA_BATCNT:
		report->alarminfo.batteryalarmcnt = value;
		break;
	case GBID_EA_BAT0:
		report->alarminfo.batteryalarm[0] = value;
		break;
	case GBID_EA_BAT1:
		report->alarminfo.batteryalarm[1] = value;
		break;
	case GBID_EA_MTCNT:
		report->alarminfo.motoalarmcnt = value;
		break;
	case GBID_EA_MT0:
		report->alarminfo.motoalarm[0] = value;
		break;
	case GBID_EA_MT1:
		report->alarminfo.motoalarm[1] = value;
		break;
	case GBID_EA_EGCNT:
		report->alarminfo.enginealarmcnt = value;
		break;
	case GBID_EA_EG0:
		report->alarminfo.enginealarm[0] = value;
		break;
	case GBID_EA_EG1:
		report->alarminfo.enginealarm[1] = value;
		break;
	case GBID_EA_OTCNT:
		report->alarminfo.otheralarmcnt = value;
		break;
	case GBID_EA_OT0:
		report->alarminfo.otheralarm[0] = value;
		break;
	case GBID_EA_OT1:
		report->alarminfo.otheralarm[1] = value;
		break;
		
	default:
		LOG("UN HANDLE TAP %04x %s\r\n", id, gbidstr (id));
		break;
	}
	
	return true;
}

void cloudmsg_motorinfo_pack(uint8_t *out, struct cloudmsg_motorinfo_t *src,
        uint16_t *size)
{
	uint32_t datsize = 0;
	*size = 0;
	
	if (src->count > MOTO_CNTMAX)
	{
		LOG("ERR MOTO CNT %d\r\n", src->count);
		*size = 0;
		return;
	}
	*out = src->count;
	out++;
	datsize++;
	for (uint32_t i = 0; i < src->count; i++)
	{
		memcpy(out, &src->moto[i], sizeof(struct cloudmsg_motordetail_t));
		out += sizeof(struct cloudmsg_motordetail_t);
		datsize += sizeof(struct cloudmsg_motordetail_t);
	}
	
	*size = datsize;
	
}

void cloudmsg_fuelcellinfo_pack(uint8_t *out,
        struct cloudmsg_fuelcellinfo_t *src, uint16_t *size)
{
	uint32_t pos = offsetof(struct cloudmsg_fuelcellinfo_t, batterytemp);
	uint32_t datsize = 0;
	uint16_t fixsize;
	*size = 0;
	
	//LOG("POS BATTEMP %d\r\n", pos);
	memcpy(out, src, pos);
	datsize += pos;
	out += pos;
	
	fixsize = htons(src->batterytempcnt);
	
	if (fixsize > BATTERY_CNTMAX)
	{
		LOG("ERR BATTEMP CNT %d\r\n", fixsize);
		fixsize = 0;
	}
	
	memcpy(out, src->batterytemp, fixsize);
	datsize += fixsize;
	out += fixsize;
	
	pos = offsetof(struct cloudmsg_fuelcellinfo_t, hydrogenmaxtemp);
	memcpy(out, src, sizeof(struct cloudmsg_fuelcellinfo_t) - pos);
	datsize += sizeof(struct cloudmsg_fuelcellinfo_t) - pos;
	out += sizeof(struct cloudmsg_fuelcellinfo_t) - pos;
	
	*size = datsize;
}

void cloudmsg_alarminfo_pack(uint8_t *out, struct cloudmsg_alarminfo_t *src,
        uint16_t *size)
{
	uint32_t datsize = 0;
	uint32_t alarmcnt;
	*size = 0;
	
	*out = src->maxalarmlevel;
	out++;
	datsize++;
	
	memcpy(out, &src->commonalarmflag, sizeof(uint32_t));
	out += sizeof(uint32_t);
	datsize += sizeof(uint32_t);
	
	if (src->batteryalarmcnt > ALARM_MAXCNT)
	{
		alarmcnt = 0;
	}
	else
	{
		alarmcnt = src->batteryalarmcnt;
	}
	
	*out = alarmcnt;
	out++;
	datsize++;
	
	for (uint32_t i = 0; i < alarmcnt; i++)
	{
		memcpy(out, &src->batteryalarm[i], sizeof(uint32_t));
		out += sizeof(uint32_t);
		datsize += sizeof(uint32_t);
	}
	
	if (src->motoalarmcnt > ALARM_MAXCNT)
	{
		alarmcnt = 0;
	}
	else
	{
		alarmcnt = src->motoalarmcnt;
	}
	
	*out = alarmcnt;
	out++;
	datsize++;
	
	for (uint32_t i = 0; i < alarmcnt; i++)
	{
		memcpy(out, &src->motoalarm[i], sizeof(uint32_t));
		out += sizeof(uint32_t);
		datsize += sizeof(uint32_t);
	}
	
	if (src->enginealarmcnt > ALARM_MAXCNT)
	{
		alarmcnt = 0;
	}
	else
	{
		alarmcnt = src->enginealarmcnt;
	}
	
	*out = alarmcnt;
	out++;
	datsize++;
	
	for (uint32_t i = 0; i < alarmcnt; i++)
	{
		memcpy(out, &src->enginealarm[i], sizeof(uint32_t));
		out += sizeof(uint32_t);
		datsize += sizeof(uint32_t);
	}
	
	if (src->otheralarmcnt > ALARM_MAXCNT)
	{
		alarmcnt = 0;
	}
	else
	{
		alarmcnt = src->otheralarmcnt;
	}
	
	*out = alarmcnt;
	out++;
	datsize++;
	
	for (uint32_t i = 0; i < alarmcnt; i++)
	{
		memcpy(out, &src->otheralarm[i], sizeof(uint32_t));
		out += sizeof(uint32_t);
		datsize += sizeof(uint32_t);
	}
	
	*size = datsize;
	
}



