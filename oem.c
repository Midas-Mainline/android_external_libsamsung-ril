/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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
#include <ril_oem.h>

int ipc_svc_display_screen(struct ipc_message *message)
{
	struct ipc_svc_display_screen_entry *entry;
	char svc_end_message[34] = "  End service mode";
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	struct ril_request *request;
	void *screen_data = NULL;
	size_t screen_size = 0;
	unsigned char count;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_svc_display_screen_header))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return 0;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	count = ipc_svc_display_screen_count_extract(message->data, message->size);

	if (count > 0) {
		if (!ipc_fmt_data->svc_session)
			ipc_fmt_data->svc_session = 1;

		entry = ipc_svc_display_screen_extract(message->data, message->size, 0);
		if (entry == NULL)
			goto error;

		// Invalid messages have bogus first entry index
		if (entry->index != 0)
			goto complete;

		screen_size = count * sizeof(struct ipc_svc_display_screen_entry);
		screen_data = (void *) entry;
	} else {
		if (ipc_fmt_data->svc_session)
			ipc_fmt_data->svc_session = 0;

		ril_request_data_free(RIL_REQUEST_OEM_HOOK_RAW);

		screen_size = sizeof(svc_end_message);
		screen_data = (void *) &svc_end_message;
	}

	request = ril_request_find_request_status(RIL_REQUEST_OEM_HOOK_RAW, RIL_REQUEST_HANDLED);
	if (request != NULL)
		ril_request_complete(request->token, RIL_E_SUCCESS, screen_data, screen_size);
	else
		ril_request_data_set_uniq(RIL_REQUEST_OEM_HOOK_RAW, screen_data, screen_size);

	goto complete;

error:
	request = ril_request_find_request_status(RIL_REQUEST_OEM_HOOK_RAW, RIL_REQUEST_HANDLED);
	if (request != NULL)
		ril_request_complete(request->token, RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ril_request_oem_hook_raw(void *data, size_t size, RIL_Token token)
{
	struct ipc_svc_enter_data svc_enter_data;
	struct ipc_svc_end_data svc_end_data;
	struct ipc_svc_pro_keycode_data svc_pro_keycode_data;
	RIL_OEMHookHeader *header;
	RIL_OEMHookSvcEnterMode *svc_enter_mode;
	RIL_OEMHookSvcEndMode *svc_end_mode;
	RIL_OEMHookSvcKey *svc_key;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	struct ril_request *request;
	void *screen_data = NULL;
	size_t screen_size = 0;
	unsigned int length;
	int rc;

	if (data == NULL || size < sizeof(RIL_OEMHookHeader))
		goto error;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		goto error;

	request = ril_request_find_request_status(RIL_REQUEST_OEM_HOOK_RAW, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	header = (RIL_OEMHookHeader *) data;

	// Only SVC is supported
	if (header->tag != RIL_OEM_HOOK_TAG_SVC)
		goto error;

	length = size - sizeof(RIL_OEMHookHeader);

	switch (header->command) {
		case RIL_OEM_COMMAND_SVC_ENTER_MODE:
			if (length < sizeof(RIL_OEMHookSvcEnterMode))
				goto error;

			svc_enter_mode = (RIL_OEMHookSvcEnterMode *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			if (svc_enter_mode->query) {
				screen_size = ril_request_data_size_get(RIL_REQUEST_OEM_HOOK_RAW);
				screen_data = ril_request_data_get(RIL_REQUEST_OEM_HOOK_RAW);

				ril_request_complete(token, RIL_E_SUCCESS, screen_data, screen_size);

				rc = RIL_REQUEST_COMPLETED;
				goto complete;
			} else {
				rc = ipc_svc_enter_setup(&svc_enter_data, svc_enter_mode->mode, svc_enter_mode->type);
				if (rc < 0)
					goto error;

				rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_SVC_ENTER);
				if (rc < 0)
					goto error;

				rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SVC_ENTER, IPC_TYPE_SET, (void *) &svc_enter_data, sizeof(svc_enter_data));
				if (rc < 0)
					goto error;
			}
			break;
		case RIL_OEM_COMMAND_SVC_END_MODE:
			if (length < sizeof(RIL_OEMHookSvcEndMode))
				goto error;

			svc_end_mode = (RIL_OEMHookSvcEndMode *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			memset(&svc_end_data, 0, sizeof(svc_end_data));
			svc_end_data.mode = svc_end_mode->mode;

			rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_SVC_END);
			if (rc < 0)
				goto error;

			rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SVC_END, IPC_TYPE_SET, (void *) &svc_end_data, sizeof(svc_end_data));
			if (rc < 0)
				goto error;

			if (ipc_fmt_data->svc_session)
				ipc_fmt_data->svc_session = 0;

			ril_request_data_free(RIL_REQUEST_OEM_HOOK_RAW);
			break;
		case RIL_OEM_COMMAND_SVC_KEY:
			if (length < sizeof(RIL_OEMHookSvcKey))
				goto error;

			svc_key = (RIL_OEMHookSvcKey *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			if (svc_key->query) {
				screen_size = ril_request_data_size_get(RIL_REQUEST_OEM_HOOK_RAW);
				screen_data = ril_request_data_get(RIL_REQUEST_OEM_HOOK_RAW);

				ril_request_complete(token, RIL_E_SUCCESS, screen_data, screen_size);

				rc = RIL_REQUEST_COMPLETED;
				goto complete;
			} else {
				if (!ipc_fmt_data->svc_session)
					goto error;

				memset(&svc_pro_keycode_data, 0, sizeof(svc_pro_keycode_data));
				svc_pro_keycode_data.key = svc_key->key;

				rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_SVC_PRO_KEYCODE);
				if (rc < 0)
					goto error;

				rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SVC_PRO_KEYCODE, IPC_TYPE_SET, (void *) &svc_pro_keycode_data, sizeof(svc_pro_keycode_data));
				if (rc < 0)
					goto error;
			}
			break;
		default:
			goto error;
	}

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_data_free(RIL_REQUEST_OEM_HOOK_RAW);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	if (screen_data != NULL && screen_size > 0)
		free(screen_data);

	return rc;
}
