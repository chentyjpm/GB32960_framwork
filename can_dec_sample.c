/**
 * @file       	can_dec_sample.c
 * @brief      	can proto manager
 * @details 	splt all proto related to can dec
 * @author    	tychen
 * @date       	2018.5
 * @version 	1.0
 * @par 	
 * @par
 */
#include "string.h"
#include "framework/can_data.h"

#define LOG(format, ...) fprintf(stderr, "[CAN_DEC][%d]:"format, __LINE__, ##__VA_ARGS__)

struct decmap_t
{
	uint32_t canid;
	uint16_t startbit;
	uint16_t bitsize;
	uint32_t
	(*conv)(uint32_t src);
	uint32_t targetid;
};

uint32_t can_conv_sample_velstatus(uint32_t val)
{
	switch(val)
	{
	case 0x0a: return 0x02;//OFF
	case 0x8a: return 0x01;//ACC
	case 0xca: return 0x01;//ON
	default: return 0xFE;//ERR
	}
}

uint32_t can_conv_sample_velspeed(uint32_t val)
{
	val = val*10;
	return htons(val);
}

uint32_t can_conv_sample_velmileage(uint32_t val)
{
	val = val*10;
	return htonl(val);
}

const struct decmap_t dec[] =
{
		{ 0x001, 24, 8, NULL, GBID_FC_VELSTATUS },
		{ 0x002, 0, 24, can_conv_sample_velmileage, GBID_FC_VELMILEAGE },
		{ 0x003, 32, 8, can_conv_sample_velspeed, GBID_FC_VELSPEED },
		{ 0x004, 0, 8, can_conv_sample_velstatus, GBID_FC_VELSTATUS },
};

static uint64_t getbit(uint8_t *dat, uint16_t start, uint16_t size)
{
	uint64_t cache;
	memcpy((uint8_t*) &cache, dat, sizeof(uint64_t));
	return ((cache >> start) & ((1 << (size + 1)) - 1));
}

bool can_dec_sample_init(void *param, struct cloudmsg_fullreport_t *report)
{
	//set vin
	memcpy(report->vincode, "LSVFA49123213059", sizeof("LSVFA49123213059"));

	cloudmsg_fullreport_setval(report, GBID_FC_RUNSTATUS, 0xFE);
	cloudmsg_fullreport_setval(report, GBID_FC_CHGSTATUS, 0xFF);
	cloudmsg_fullreport_setval(report, GBID_FC_VELSPEED, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_VELMILEAGE, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_VELVOL, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_VELCUR, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_ISO, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_SOC, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_GEAR, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_DRVTAP, 0);
	cloudmsg_fullreport_setval(report, GBID_FC_BREAKTAP, 0);

	//set batcnt
	cloudmsg_fullreport_setval(report, GBID_MT_CNT, 1);

	return true;
}

bool can_dec_sample_fini(void *param, struct cloudmsg_fullreport_t *report)
{
	return true;
}

bool can_dec_sample_deccanmsg(void *param,
        struct cloudmsg_fullreport_t *report, struct can_msg_t *msg)
{
	LOG("DEC ID %04x \r\n", msg->canid);

	for (int i = 0; i < (sizeof(dec) / sizeof(dec[0])); i++)
	{
		if (dec[i].canid == msg->canid)
		{
			uint32_t dat;
			dat = getbit(msg->data, dec[i].startbit, dec[i].bitsize);
			if (dec[i].conv != NULL)
				dat = dec[i].conv(dat);
			cloudmsg_fullreport_setval(report, dec[i].targetid, dat);

			LOG("SET ID:%s VAL:%d\r\n", gbidstr (dec[i].targetid), dat);
		}
	}
	return true;
}

struct can_dec_t excodec =
{
        .param = NULL,
        .init = can_dec_sample_init,
        .fini = can_dec_sample_fini,
        .deccanmsg = can_dec_sample_deccanmsg };
