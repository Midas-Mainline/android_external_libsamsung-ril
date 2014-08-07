/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdlib.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>

int ipc_gen_phone_res_expect_register(struct ril_client *client,
	unsigned char aseq, unsigned short command,
	int (*callback)(struct ipc_message *message),
	int complete, int abort)
{
	struct ipc_gen_phone_res_expect *expect;
	struct ipc_fmt_data *data;
	struct list_head *list_end;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	expect = calloc(1, sizeof(struct ipc_gen_phone_res_expect));
	expect->aseq = aseq;
	expect->command = command;
	expect->callback = callback;
	expect->complete = complete;
	expect->abort = abort;

	list_end = data->gen_phone_res_expect;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) expect);

	if (data->gen_phone_res_expect == NULL)
		data->gen_phone_res_expect = list;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_gen_phone_res_expect_unregister(struct ril_client *client,
	struct ipc_gen_phone_res_expect *expect)
{
	struct ipc_fmt_data *data;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->gen_phone_res_expect;
	while (list != NULL) {
		if (list->data == (void *) expect) {
			memset(expect, 0, sizeof(struct ipc_gen_phone_res_expect));
			free(expect);

			if (list == data->gen_phone_res_expect)
				data->gen_phone_res_expect = list->next;

			list_head_free(list);

			break;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_gen_phone_res_expect_flush(struct ril_client *client)
{
	struct ipc_gen_phone_res_expect *expect;
	struct ipc_fmt_data *data;
	struct list_head *list;
	struct list_head *list_next;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->gen_phone_res_expect;
	while (list != NULL) {
		if (list->data != NULL) {
			expect = (struct ipc_gen_phone_res_expect *) list->data;
			memset(expect, 0, sizeof(struct ipc_gen_phone_res_expect));
			free(expect);
		}

		if (list == data->gen_phone_res_expect)
			data->gen_phone_res_expect = list->next;

		list_next = list->next;

		list_head_free(list);

list_continue:
		list = list_next;
	}

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

struct ipc_gen_phone_res_expect *ipc_gen_phone_res_expect_find_aseq(struct ril_client *client,
	unsigned char aseq)
{
	struct ipc_gen_phone_res_expect *expect;
	struct ipc_fmt_data *data;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return NULL;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->gen_phone_res_expect;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		expect = (struct ipc_gen_phone_res_expect *) list->data;

		if (expect->aseq == aseq) {
			RIL_CLIENT_UNLOCK(client);
			return expect;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(client);

	return NULL;
}

int ipc_gen_phone_res_expect_callback(unsigned char aseq, unsigned short command,
	int (*callback)(struct ipc_message *message))
{
	struct ril_client *client;
	int rc;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return -1;

	rc = ipc_gen_phone_res_expect_register(client, aseq, command, callback, 0, 0);
	if (rc < 0)
		return -1;

	return 0;
}

int ipc_gen_phone_res_expect_complete(unsigned char aseq, unsigned short command)
{
	struct ril_client *client;
	int rc;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return -1;

	rc = ipc_gen_phone_res_expect_register(client, aseq, command, NULL, 1, 0);
	if (rc < 0)
		return -1;

	return 0;
}

int ipc_gen_phone_res_expect_abort(unsigned char aseq, unsigned short command)
{
	struct ril_client *client;
	int rc;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return -1;

	rc = ipc_gen_phone_res_expect_register(client, aseq, command, NULL, 0, 1);
	if (rc < 0)
		return -1;

	return 0;
}

int ipc_gen_phone_res(struct ipc_message *message)
{
	struct ipc_gen_phone_res_expect *expect;
	struct ipc_gen_phone_res_data *data;
	struct ril_client *client;
	RIL_Errno error;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	expect = ipc_gen_phone_res_expect_find_aseq(client, message->aseq);
	if (expect == NULL) {
		RIL_LOGD("Ignoring generic response for command %s", ipc_command_string(IPC_COMMAND(data->group, data->index)));
		return 0;
	}

	if (IPC_COMMAND(data->group, data->index) != expect->command) {
		RIL_LOGE("Generic response commands mismatch: %s/%s", ipc_command_string(IPC_COMMAND(data->group, data->index)), ipc_command_string(expect->command));
		goto error;
	}

	RIL_LOGD("Generic response was expected");

	if (expect->callback != NULL) {
		rc = expect->callback(message);
		goto complete;
	}

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0)
		error = RIL_E_GENERIC_FAILURE;
	else
		error = RIL_E_SUCCESS;

	if (expect->complete || (expect->abort && error == RIL_E_GENERIC_FAILURE))
		ril_request_complete(ipc_fmt_request_token(message->aseq), error, NULL, 0);

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (expect != NULL)
		ipc_gen_phone_res_expect_unregister(client, expect);

	return rc;
}
