/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 *
 * Samsung-RIL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Samsung-RIL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Samsung-RIL.  If not, see <http://www.gnu.org/licenses/>.
 */

#define LOG_TAG "RIL-GEN"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

/*
 * IPC_GEN_PHONE_RES has shared aseq (in the header), group, index and type (in the data)
 * with the original request it responds to. 
 * On this implementation, we just check aseq and command (group and index).
 * aseq permits to identify the queued request and do what's wanted.
 * It can be either call a function with the struct ipc_message_info,
 * complete the request to RILJ (with or without an error),
 * return to RILJ if there is an error in this response.
 * 
 * It would have been possible to deal with group, index and type only and use 
 * callback functions. Though, what is currently being used is more a "standard"
 * error catch system, that requires less code (no particular function), while
 * it also permits using custom functions, when IPC_GEN_PHONE_RES code is 
 * request-specific.
 * 
 * On a custom function, don't forget to get a clean new aseq if you're going to 
 * send some data to the modem, just liek this:
 * aseq = ril_request_reg_id(ril_request_get_token(info->aseq));
 *
 * Please use the GEN_PHONE_RES engine as often as possible!
 */

/*
 * GEN expects functions
 */

int ipc_gen_phone_res_expect_register(unsigned char aseq, unsigned short command,
	void (*func)(struct ipc_message_info *info), int complete, int abort)
{
	struct ipc_gen_phone_res_expect_info *expect;
	struct list_head *list_end;
	struct list_head *list;

	expect = calloc(1, sizeof(struct ipc_gen_phone_res_expect_info));
	if (expect == NULL)
		return -1;

	expect->aseq = aseq;
	expect->command = command;
	expect->func = func;
	expect->complete = complete;
	expect->abort = abort;

	list_end = ril_data.generic_responses;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) expect, list_end, NULL);

	if (ril_data.generic_responses == NULL)
		ril_data.generic_responses = list;

	return 0;
}

void ipc_gen_phone_res_expect_unregister(struct ipc_gen_phone_res_expect_info *expect)
{
	struct list_head *list;

	if (expect == NULL)
		return;

	list = ril_data.generic_responses;
	while (list != NULL) {
		if (list->data == (void *) expect) {
			memset(expect, 0, sizeof(struct ipc_gen_phone_res_expect_info));
			free(expect);

			if (list == ril_data.generic_responses)
				ril_data.generic_responses = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct ipc_gen_phone_res_expect_info *ipc_gen_phone_res_expect_info_find_aseq(unsigned char aseq)
{
	struct ipc_gen_phone_res_expect_info *expect;
	struct list_head *list;

	list = ril_data.generic_responses;
	while (list != NULL) {
		expect = (struct ipc_gen_phone_res_expect_info *) list->data;
		if (expect == NULL)
			goto list_continue;

		if (expect->aseq == aseq)
			return expect;

list_continue:
		list = list->next;
	}

	return NULL;
}

int ipc_gen_phone_res_expect_to_func(unsigned char aseq, unsigned short command,
	void (*func)(struct ipc_message_info *info))
{
	return ipc_gen_phone_res_expect_register(aseq, command, func, 0, 0);
}

int ipc_gen_phone_res_expect_to_complete(unsigned char aseq, unsigned short command)
{
	return ipc_gen_phone_res_expect_register(aseq, command, NULL, 1, 0);
}

int ipc_gen_phone_res_expect_to_abort(unsigned char aseq, unsigned short command)
{
	return ipc_gen_phone_res_expect_register(aseq, command, NULL, 0, 1);
}

/*
 * GEN dequeue function
 */

void ipc_gen_phone_res(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res_expect_info *expect;
	struct ipc_gen_phone_res *phone_res;
	RIL_Errno e;
	int rc;

	if (info->data == NULL || info->length < sizeof(struct ipc_gen_phone_res))
		return;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	expect = ipc_gen_phone_res_expect_info_find_aseq(info->aseq);

	if (expect == NULL) {
		RIL_LOGD("aseq: 0x%x not found in the IPC_GEN_PHONE_RES queue", info->aseq);
		return;
	}

	RIL_LOGD("aseq: 0x%x found in the IPC_GEN_PHONE_RES queue!", info->aseq);

	if (expect->command != IPC_COMMAND(phone_res)) {
		RIL_LOGE("IPC_GEN_PHONE_RES aseq (0x%x) doesn't match the queued one with command (0x%x)", 
			expect->aseq, expect->command);

		if (expect->func != NULL) {
			RIL_LOGE("Not safe to run the custom function, reporting generic failure");
			ril_request_complete(ril_request_get_token(expect->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
			goto unregister;
		}
	}

	if (expect->func != NULL) {
		expect->func(info);
		goto unregister;
	}

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0)
		e = RIL_E_GENERIC_FAILURE;
	else
		e = RIL_E_SUCCESS;

	if (expect->complete || (expect->abort && e == RIL_E_GENERIC_FAILURE))
		ril_request_complete(ril_request_get_token(expect->aseq), e, NULL, 0);

unregister:
	ipc_gen_phone_res_expect_unregister(expect);
}
