/**
 * @file       	gb32960.h
 * @brief      	headfile of gb32960 module
 * @details 	this module will process all the gbdata to fullreport and manager the storagefile in flash disk
 * @author    	tychen
 * @date       	2018.5
 * @version 	1.0
 * @par 	
 * @par
 */

#ifndef _GB32960_
#define _GB32960_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define _PACK_START
#define _PACK_END	__attribute__ ((packed))

#define CLOUDPKG_MAXSIZE	2048

struct cloudpkg_t
{
	struct cloudmsg_t *pkg;
	uint16_t size;
	uint8_t mem[CLOUDPKG_MAXSIZE];
};

#define VINCODE_SIZE	17
#define DATA_SIZE_MAX	65531

#define CLOUDMSG_SIZE(datasize)	(2 + 2 + VINCODE_SIZE + 3 + datasize + 1)
#define CLOUDMSG_BCCSIZE(datasize) (2 + VINCODE_SIZE + 3 + datasize)

struct _PACK_START cloudmsg_t
{
	uint8_t head[2]; //default head is "##"
	uint8_t cmd;
	uint8_t ack;
//size 4
	uint8_t vincode[VINCODE_SIZE];

	uint8_t encryptmode;
	uint16_t datalen;
//size 3
	uint8_t data[CLOUDPKG_MAXSIZE - CLOUDMSG_SIZE(0)];
	uint8_t bcc;
//size 1
}_PACK_END;

enum cloudcmd_t
{
	CMD_VEH_SIGNIN = 0x01,
	CMD_RT_UPLOAD = 0x02,
	CMD_FIX_UPLOAD = 0x03,
	CMD_VEH_SIGNOUT = 0x04,
	CMD_PLT_SIGNIN = 0x05,
	CMD_PLT_SIGNOUT = 0x06,
	CMD_EP_RESV0 = 0x07,
	CMD_EP_RESV1 = 0x08,
	CMD_UPLOAD_RESV = 0x09,
	//RESV to 0x7F
	CMD_EP_RESV2 = 0x80,
	//RESV to 0x82
	CMD_DOWNLOAD_RESV = 0x83,
	//RESV to 0xBF
	CMD_PLT_RESV = 0xC0,
	//RESV to 0xFE
	CMD_END = 0xFF
};

char *
cmdstr(enum cloudcmd_t cmd);

enum cloudack_t
{
	ACK_OK = 0x01, ACK_ERR = 0x02, ACK_DUPVIN = 0x03, ACK_CMD = 0xFE,
//notify this is a cmd
};

char *
ackstr(enum cloudack_t ack);

enum cloudencript_t
{
	DAT_CRIPT_NONE = 0x01,
	DAT_CRIPT_RSA = 0x02,
	DAT_CRIPT_AES128 = 0x03,
	DAT_CRIPT_ERR = 0xFE,
};

//base time
struct _PACK_START cloudtime_t
{
	//use GMT +8 Beijing Time
	uint8_t year; //0-99
	uint8_t mon;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}_PACK_END;

#define BATTERY_CNTMAX									48
#define BATTERY_CODEMAX									4
#define ICCID_SIZE										20
#define CLOUDMSG_VEH_SIGNIN_SIZE(batcnt, batsize) 		(batcnt * batsize + 2 + ICCID_SIZE + 8)
#define CLOUDMSG_VEHICLE_SIGNIN_PACK(out, src, size) 	\
	(out = &src; \
	size = CLOUDMSG_VEH_SIGNIN_SIZE(src->batterycnt, src->batterycodesize);)
struct _PACK_START cloudmsg_vehicle_signin_t
{
	struct cloudtime_t curtime;
	uint16_t seq;
	uint8_t iccid[ICCID_SIZE];
	uint8_t batterycnt;
	uint8_t batterycodesize;
	uint8_t batterycode[BATTERY_CNTMAX * BATTERY_CODEMAX];
	
}_PACK_END;

#define CLOUDMSG_REALTIME_UPLOAD_PACK(out, src, size)
struct _PACK_START cloudmsg_realtime_upload_t
{
	struct cloudtime_t curtime;
	uint8_t report[1];
	//can dup
	//type2
	//dat2
}_PACK_END;

enum cloudmsg_reporttype_t
{
	REPORT_FULLVEHICLE = 0x01,
	REPORT_MOTO = 0x02,
	REPORT_FUELCELL = 0x03,
	REPORT_ENGINE = 0x04,
	REPORT_LOCATION = 0x05,
	REPORT_ABSMAX = 0x06,
	REPORT_ALARM = 0x07,
	REPORT_ERR = 0xFE,
};

char *
reportstr(enum cloudmsg_reporttype_t cmd);

#define CLOUDMSG_FULLVEHICLEINFO_PACK(out, src, size)	\
		{	*size = sizeof(struct cloudmsg_fullvehicleinfo_t); \
			memcpy(out,src,*size);}
struct _PACK_START cloudmsg_fullvehicleinfo_t
{
	uint8_t vehiclestate;
	uint8_t chgstate;
	uint8_t runmode;
	uint16_t speed;
	//speed = km/h *10 100km/s -- 1000
	uint32_t accumulationkms;
	uint16_t totalvol;
	//vol = V *10 100V -- 1000
	uint16_t totalcur;
	//cur = A *10 - 10000  100A -- 11000
	uint8_t SOCvalue;
	//0 - 100 0xFE = ERR
	uint8_t dcdcstate;
	//
	uint8_t gearstate;
	//
	uint16_t isolateres;
	//kohm
	uint8_t drvtap;
	uint8_t breaktap;
}_PACK_END;

#define MOTO_CNTMAX	4

#define CLOUDMSG_MOTOINFO_SIZE(msg)	(msg.count * 12 + 1)

struct _PACK_START cloudmsg_motordetail_t
{
	uint8_t seq;
	uint8_t state;
	uint8_t drivertemp;
	uint16_t speedrpm;
	//speed = rpm - 20000
	uint16_t torque;
	//torque = N.M /10 - 20000
	uint8_t mototemp;
	uint16_t drivervolin;
	//vol = V *10 100V -- 1000
	uint16_t drivercurin;
	//cur = A *10 - 10000  100A -- 11000
}_PACK_END;

#define CLOUDMSG_MOTOINFO_PACK			cloudmsg_motorinfo_pack
struct _PACK_START cloudmsg_motorinfo_t
{
	uint8_t count;
	struct _PACK_START cloudmsg_motordetail_t moto[MOTO_CNTMAX];
}_PACK_END;

void
cloudmsg_motorinfo_pack(uint8_t *out, struct cloudmsg_motorinfo_t *src,
        uint16_t *size);

#define CLOUDMSG_FUELCELLINFO_PACK		cloudmsg_fuelcellinfo_pack
struct _PACK_START cloudmsg_fuelcellinfo_t
{
	uint16_t vol;
	uint16_t cur;
	uint16_t reducerate;
	//reducerate = kg/100km * 100
	uint16_t batterytempcnt;
	uint8_t batterytemp[BATTERY_CNTMAX];
	//0-240max temp = C + 40
	
	uint16_t hydrogenmaxtemp;
	uint8_t hydrogenmaxtempid;
	uint16_t hydrogenmaxratio;
	uint8_t hydrogenmaxratioid;
	uint16_t hydrogenmaxpressure;
	uint8_t hydrogenmaxpressureid;
	uint8_t highvoldcdcstate;
}_PACK_END;

void
cloudmsg_fuelcellinfo_pack(uint8_t *out, struct cloudmsg_fuelcellinfo_t *src,
        uint16_t *size);

#define CLOUDMSG_ENGINEINFO_PACK(out, src, size)	\
		{	*size = sizeof(struct cloudmsg_engineinfo_t);\
			memcpy(out,src,*size);}
struct _PACK_START cloudmsg_engineinfo_t
{
	uint8_t state;
	uint16_t speedrpm;
	uint16_t reducerate;
}_PACK_END;

#define LOC_FIXED	0x00
#define LOC_UNFIXED	0x01
#define LAT_NORTH	0x00
#define LAT_SORTH	0x02
#define LIT_EAST	0x00
#define LIT_WEST	0x04

#define CLOUDMSG_LOCATIONINFO_PACK(out, src, size)	\
	{	*size = sizeof(struct cloudmsg_locationinfo_t);\
		memcpy(out,src,*size);}
struct _PACK_START cloudmsg_locationinfo_t
{
	uint8_t fixstate;
	uint32_t longitude;
	uint32_t latitude;
	//0.0000001C
}_PACK_END;

#define CLOUDMSG_ABSOLUTEMAXINFO_PACK(out, src, size)	\
		{	*size = sizeof(struct cloudmsg_minmaxcellinfo_t);\
			memcpy(out,src,*size);}
struct _PACK_START cloudmsg_minmaxcellinfo_t
{
	uint8_t maxvolcellsysid;
	uint8_t maxvolcellid;
	uint16_t maxvolcellvalue;
	uint8_t minvolcellsysid;
	uint8_t minvolcellid;
	uint16_t minvolcellvalue;

	uint8_t maxtempcellsysid;
	uint8_t maxtempcellid;
	uint8_t maxtempcellvalue;
	uint8_t mintempcellsysid;
	uint8_t mintempcellid;
	uint8_t mintempcellvalue;
}_PACK_END;

#define ALARM_MAXCNT	4
#define CLOUDMSG_ALARMINFO_PACK		cloudmsg_alarminfo_pack
struct _PACK_START cloudmsg_alarminfo_t
{
	uint8_t maxalarmlevel;
	uint32_t commonalarmflag;

	uint8_t batteryalarmcnt;
	uint32_t batteryalarm[ALARM_MAXCNT];

	uint8_t motoalarmcnt;
	uint32_t motoalarm[ALARM_MAXCNT];

	uint8_t enginealarmcnt;
	uint32_t enginealarm[ALARM_MAXCNT];

	uint8_t otheralarmcnt;
	uint32_t otheralarm[ALARM_MAXCNT];
}_PACK_END;

void
cloudmsg_alarminfo_pack(uint8_t *out, struct cloudmsg_alarminfo_t *src,
        uint16_t *size);

struct _PACK_START cloudmsg_userdefinfo_t
{
	uint16_t size;
	uint8_t dat[1];
}_PACK_END;

struct _PACK_START cloudmsg_vehicle_signout_t
{
	struct cloudtime_t curtime;
	uint16_t seq;
}_PACK_END;


#define CLOUDMSG_REMOTECTL_SIZE(paramsize) 		(paramsize + 7)

#define CLOUDMSG_REMOTECTL_PARAMSIZEMAX		8

struct _PACK_START cloudmsg_remotectl_t
{
	struct cloudtime_t curtime;
	uint8_t type;
	uint8_t param[CLOUDMSG_REMOTECTL_PARAMSIZEMAX];
}_PACK_END;


#define GB_BATCODEMAX		8
struct cloudmsg_fullreport_t
{
	uint8_t vincode[VINCODE_SIZE];
	uint8_t batterycnt;
	uint8_t batterycodesize;
	uint8_t batterycode[GB_BATCODEMAX];

	struct cloudmsg_fullvehicleinfo_t fullvehicleinfo;
	struct cloudmsg_motorinfo_t motorinfo;
	struct cloudmsg_fuelcellinfo_t fuelcellinfo;
	struct cloudmsg_engineinfo_t engineinfo;
	struct cloudmsg_locationinfo_t locationinfo;
	struct cloudmsg_minmaxcellinfo_t minmaxcellinfo;
	struct cloudmsg_alarminfo_t alarminfo;
};

enum cloudgbid_t
{
	GBID_FC_VELSTATUS = 0x100,
	GBID_FC_CHGSTATUS,
	GBID_FC_RUNSTATUS,
	GBID_FC_VELSPEED,
	GBID_FC_VELMILEAGE,
	GBID_FC_VELVOL,
	GBID_FC_VELCUR,
	GBID_FC_SOC,
	GBID_FC_DCDC,
	GBID_FC_ISDRV,
	GBID_FC_ISBREAK,
	GBID_FC_GEAR,
	GBID_FC_ISO,
	GBID_FC_DRVTAP,
	GBID_FC_BREAKTAP,
	
	GBID_MT_CNT = 0x200,
	GBID_MT0_STATE = 0x210,
	GBID_MT0_SEQ,
	GBID_MT0_DRVTEMP,
	GBID_MT0_SPEED,
	GBID_MT0_TORQUE,
	GBID_MT0_MTTEMP,
	GBID_MT0_DRVVOL,
	GBID_MT0_DRVCUR,
	
	//TODO MT1 MT2 MT3
	GBID_MT1_STATE = 0x220,
	GBID_MT1_SEQ,
	GBID_MT1_DRVTEMP,
	GBID_MT1_SPEED,
	GBID_MT1_TORQUE,
	GBID_MT1_MTTEMP,
	GBID_MT1_DRVVOL,
	GBID_MT1_DRVCUR,
	
	GBID_MT2_STATE = 0x230,
	GBID_MT2_SEQ,
	GBID_MT2_DRVTEMP,
	GBID_MT2_SPEED,
	GBID_MT2_TORQUE,
	GBID_MT2_MTTEMP,
	GBID_MT2_DRVVOL,
	GBID_MT2_DRVCUR,
	
	GBID_MT3_STATE = 0x240,
	
	GBID_FU_BATTEMPCNT = 0x300,
	GBID_FU_BATTEMP0,
	GBID_FU_BATTEMP1,
	GBID_FU_BATTEMP2,
	GBID_FU_BATTEMP3,
	GBID_FU_BATTEMP4,
	GBID_FU_BATTEMP5,
	GBID_FU_BATTEMP6,
	GBID_FU_BATTEMP7,
	GBID_FU_VOL = 0x380,
	GBID_FU_CUR,
	GBID_FU_REDUCERATE,
	GBID_FU_MAXTEMP,
	GBID_FU_MAXTEMPID,
	GBID_FU_MAXRATIO,
	GBID_FU_MAXRATIOID,
	GBID_FU_MAXPRESS,
	GBID_FU_MAXPRESSID,
	GBID_FU_HVDCDC,
	
	GBID_EG_STATE = 0x400,
	GBID_EG_SPEED,
	GBID_EG_REDUCERATE,
	
	GBID_LO_FIXSTATE = 0x500,
	GBID_LO_LONG,
	GBID_LO_LAT,
	
	GBID_MM_MAXVCSID = 0x600,
	GBID_MM_MAXVCID,
	GBID_MM_MAXVC,
	GBID_MM_MINVCSID,
	GBID_MM_MINVCID,
	GBID_MM_MINVC,
	GBID_MM_MAXTCSID,
	GBID_MM_MAXTCID,
	GBID_MM_MAXTC,
	GBID_MM_MINTCSID,
	GBID_MM_MINTCID,
	GBID_MM_MINTC,
	
	GBID_EA_MAXLEVEL = 0x700,
	GBID_EA_GPFLAG,
	GBID_EA_BATCNT,
	GBID_EA_BAT0,
	GBID_EA_BAT1,
	GBID_EA_MTCNT,
	GBID_EA_MT0,
	GBID_EA_MT1,
	GBID_EA_EGCNT,
	GBID_EA_EG0,
	GBID_EA_EG1,
	GBID_EA_OTCNT,
	GBID_EA_OT0,
	GBID_EA_OT1,
};

char *
gbidstr(enum cloudgbid_t cmd);

bool
cloudmsg_fullreport_setval(struct cloudmsg_fullreport_t *report, uint32_t id,
        uint32_t value);
#endif
