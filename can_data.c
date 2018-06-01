/**
 * @file       	can_data.c
 * @brief      	candata manager
 * @details 	control all can data recv from any proto
 * @author    	tychen
 * @date       	2018.5
 * @version 	1.0
 * @par 	
 * @par
 */
#include "framework/can_data.h"

#define LOG(format, ...) fprintf(stderr, "[CAN_DAT][%d]:"format, __LINE__, ##__VA_ARGS__)
#define INFO(format, ...) fprintf(stderr, "[CAN_DAT][%d]:"format, __LINE__, ##__VA_ARGS__)

bool can_data_manager_init(struct can_data_manager_t *manager,
        struct can_dec_t *dec)
{
	manager->dec = dec;
	manager->dec->init(manager->dec->param, &manager->report);
	
	return true;
}

bool can_data_manager_canmsg_input(struct can_data_manager_t *manager,
        struct can_msg_t *msg)
{
	INFO("INPUT ID:%04x, DAT:%02x %02x %02x %02x %02x %02x %02x %02x\r\n", msg->canid,
			msg->data[0], msg->data[1], msg->data[2], msg->data[3],
			msg->data[4], msg->data[5], msg->data[6], msg->data[7]);
	
	manager->dec->deccanmsg(manager->dec->param, &manager->report, msg);
	
	for (int i = 0; i < CAN_MSG_LISTSIZE; i++)
	{
		if (manager->msglist[i].canid == msg->canid)
		{
			//update canmsg
			manager->msglist[i] = *msg;
			return true;
		}
		else if (manager->msglist[i].canid == CAN_ID_EMPTY)
		{
			//add new msg
			manager->msglist[i] = *msg;
			return true;
		}
	}
	
	return false;
}

static const struct can_msg_t empty =
{ .canid = CAN_ID_EMPTY, .data =
{ 0, 0, 0, 0, 0, 0, 0, 0 }, .datasize = 0, .updatetime = 0 };

struct can_msg_t *
can_data_manager_canmsg_get(struct can_data_manager_t *manager, uint32_t canid)
{
	for (int i = 0; i < CAN_MSG_LISTSIZE; i++)
	{
		if (manager->msglist[i].canid == canid)
		{
			//get canmsg
			return &manager->msglist[i];
		}
		else if (manager->msglist[i].canid == CAN_ID_EMPTY)
		{
			//no msg
			return (struct can_msg_t *) &empty;
		}
	}
	return (struct can_msg_t *) &empty;
}

