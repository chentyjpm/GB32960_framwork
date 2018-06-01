/**
 * @file       	can_data.h
 * @brief      	headfile of candata manager
 * @details 	control all can data recv from any proto
 * @author    	tychen
 * @date       	2018.5
 * @version 	1.0
 * @par 	
 * @par
 */
#ifndef _CAN_DAT_
#define _CAN_DAT_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "framework/gb32960.h"

#define	CAN_DATA_SIZEMAX	8
#define CAN_ID_EMPTY		0
struct can_msg_t
{
	uint32_t updatetime;
	uint32_t canid;
	uint8_t data[CAN_DATA_SIZEMAX];
	uint8_t datasize;
};

struct can_dec_t
{
	void *param;bool
	(*init)(void *param, struct cloudmsg_fullreport_t *report);bool
	(*fini)(void *param, struct cloudmsg_fullreport_t *report);bool
	(*deccanmsg)(void *param, struct cloudmsg_fullreport_t *report,
	        struct can_msg_t *msg);
};

#define CAN_MSG_LISTSIZE	64
struct can_data_manager_t
{
	uint32_t msglistcnt;
	struct can_msg_t msglist[CAN_MSG_LISTSIZE];

	//gb32960report
	struct cloudmsg_fullreport_t report;

	struct can_dec_t *dec;
};

extern struct can_dec_t excodec;

bool
can_data_manager_init(struct can_data_manager_t *manager,
        struct can_dec_t *dec);
bool
can_data_manager_canmsg_input(struct can_data_manager_t *manager,
        struct can_msg_t *msg);
struct can_msg_t *
can_data_manager_canmsg_get(struct can_data_manager_t *manager, uint32_t canid);

#endif
