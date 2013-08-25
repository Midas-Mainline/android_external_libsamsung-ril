/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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

#define LOG_TAG "RIL-SVC"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

int ril_oem_hook_svc_session_start(void)
{
	if (ril_data.oem_hook_svc_session != NULL) {
		LOGE("OEM hook SVC session was already started");
		return -1;
	}

	ril_data.oem_hook_svc_session = calloc(1, sizeof(struct ril_oem_hook_svc_session));

	return 0;
}

void ril_oem_hook_svc_session_stop(void)
{
	if (ril_data.oem_hook_svc_session == NULL) {
		LOGE("OEM hook SVC session was already stopped");
		return;
	}

	if (ril_data.oem_hook_svc_session->display_screen != NULL)
		free(ril_data.oem_hook_svc_session->display_screen);

	free(ril_data.oem_hook_svc_session);
	ril_data.oem_hook_svc_session = NULL;
}

void ipc_svc_display_screen(struct ipc_message_info *info)
{
	char svc_end_message[] = "Samsung-RIL is asking you to End service mode";
	struct ipc_svc_display_screen_header *header;
	struct ipc_svc_display_screen_data *data;

	if (info == NULL || info->data == NULL || info->length < sizeof(header->count))
		return;

	if (ril_data.oem_hook_svc_session == NULL)
		goto error;

	header = (struct ipc_svc_display_screen_header *) info->data;

	if (header->count == 0) {
		if (ril_data.oem_hook_svc_session->token != RIL_TOKEN_NULL) {
			ril_request_complete(ril_data.oem_hook_svc_session->token, RIL_E_SUCCESS, &svc_end_message, sizeof(svc_end_message));
			ril_data.oem_hook_svc_session->token = RIL_TOKEN_NULL;
		}

		ril_oem_hook_svc_session_stop();

		return;
	}

	data = (struct ipc_svc_display_screen_data *) ((unsigned char *) info->data + sizeof(struct ipc_svc_display_screen_header));

	// Invalid responses don't start at index 0
	if (data->index != 0)
		return;

	if (ril_data.oem_hook_svc_session->display_screen != NULL) {
		free(ril_data.oem_hook_svc_session->display_screen);
		ril_data.oem_hook_svc_session->display_screen = NULL;
		ril_data.oem_hook_svc_session->display_screen_length = 0;
	}

	ril_data.oem_hook_svc_session->display_screen_length = header->count * sizeof(struct ipc_svc_display_screen_data);
	ril_data.oem_hook_svc_session->display_screen = malloc(ril_data.oem_hook_svc_session->display_screen_length);
	memcpy(ril_data.oem_hook_svc_session->display_screen, data, ril_data.oem_hook_svc_session->display_screen_length);

	if (ril_data.oem_hook_svc_session->token != RIL_TOKEN_NULL) {
		ril_request_complete(ril_data.oem_hook_svc_session->token, RIL_E_SUCCESS, ril_data.oem_hook_svc_session->display_screen, ril_data.oem_hook_svc_session->display_screen_length);

		ril_data.oem_hook_svc_session->token = RIL_TOKEN_NULL;
	}

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_svc_callback(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	int rc;

	if (info == NULL || info->data == NULL || info->length < (int) sizeof(struct ipc_gen_phone_res))
		return;

	if (ril_data.oem_hook_svc_session == NULL)
		goto error;

	phone_res = (struct ipc_gen_phone_res *) info->data;

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0)
		goto error;

	if (ril_data.oem_hook_svc_session->token != RIL_TOKEN_NULL) {
		LOGE("%s: Another token is waiting", __func__);
		goto error;
	}

	ril_data.oem_hook_svc_session->token = ril_request_get_token(info->aseq);

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_oem_hook_raw(RIL_Token token, void *data, int length)
{
	RIL_OEMHookHeader *header;
	RIL_OEMHookSvcEnterMode *svc_enter_mode;
	RIL_OEMHookSvcEndMode *svc_end_mode;
	RIL_OEMHookSvcKey *svc_key;
	struct ipc_svc_enter_data svc_enter_data;
	struct ipc_svc_end_data svc_end_data;
	struct ipc_svc_pro_keycode_data svc_pro_keycode_data;
	int svc_length;
	int rc;

	if (data == NULL || length < (int) sizeof(RIL_OEMHookHeader))
		goto error;

	header = (RIL_OEMHookHeader *) data;

	// Only SVC is supported
	if (header->tag != RIL_OEM_HOOK_TAG_SVC)
		goto error;

	svc_length = (header->length & 0xff) << 8 | (header->length & 0xff00) >> 8;

	switch (header->command) {
		case RIL_OEM_COMMAND_SVC_ENTER_MODE:
			if (svc_length < (int) (sizeof(RIL_OEMHookHeader) + sizeof(RIL_OEMHookSvcEnterMode)))
				goto error;

			svc_enter_mode = (RIL_OEMHookSvcEnterMode *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			if (svc_enter_mode->query) {
				if (ril_data.oem_hook_svc_session == NULL)
					goto error;

				if (ril_data.oem_hook_svc_session->display_screen != NULL && ril_data.oem_hook_svc_session->display_screen_length > 0)
					ril_request_complete(token, RIL_E_SUCCESS, ril_data.oem_hook_svc_session->display_screen, ril_data.oem_hook_svc_session->display_screen_length);
				else
					goto error;
			} else {
				rc = ril_oem_hook_svc_session_start();
				if (rc < 0) {
					LOGE("%s: Unable to start OEM hook SVC session", __func__);
					goto error;
				}

				memset(&svc_enter_data, 0, sizeof(svc_enter_data));
				svc_enter_data.mode = svc_enter_mode->mode;
				svc_enter_data.type = svc_enter_mode->type;

				if (svc_enter_data.mode == IPC_SVC_MODE_MONITOR)
					svc_enter_data.unknown = 0x00;
				else
					svc_enter_data.unknown = 0x10;

				ipc_gen_phone_res_expect_to_func(ril_request_get_id(token), IPC_SVC_ENTER, ipc_svc_callback);

				ipc_fmt_send(IPC_SVC_ENTER, IPC_TYPE_SET, (unsigned char *) &svc_enter_data, sizeof(svc_enter_data), ril_request_get_id(token));
			}
			break;
		case RIL_OEM_COMMAND_SVC_END_MODE:
			if (svc_length < (int) (sizeof(RIL_OEMHookHeader) + sizeof(RIL_OEMHookSvcEndMode)))
				goto error;

			svc_end_mode = (RIL_OEMHookSvcEndMode *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			ril_oem_hook_svc_session_stop();

			memset(&svc_end_data, 0, sizeof(svc_end_data));
			svc_end_data.mode = svc_end_mode->mode;

			ipc_gen_phone_res_expect_to_complete(ril_request_get_id(token), IPC_SVC_END);

			ipc_fmt_send(IPC_SVC_END, IPC_TYPE_SET, (unsigned char *) &svc_end_data, sizeof(svc_end_data), ril_request_get_id(token));
			break;
		case RIL_OEM_COMMAND_SVC_KEY:
			if (svc_length < (int) (sizeof(RIL_OEMHookHeader) + sizeof(RIL_OEMHookSvcKey)))
				goto error;

			svc_key = (RIL_OEMHookSvcKey *) ((unsigned char *) data + sizeof(RIL_OEMHookHeader));

			if (svc_key->query) {
				if (ril_data.oem_hook_svc_session == NULL)
					goto error;

				if (ril_data.oem_hook_svc_session->display_screen != NULL && ril_data.oem_hook_svc_session->display_screen_length > 0)
					ril_request_complete(token, RIL_E_SUCCESS, ril_data.oem_hook_svc_session->display_screen, ril_data.oem_hook_svc_session->display_screen_length);
				else
					goto error;
			} else {
				if (ril_data.oem_hook_svc_session == NULL)
					goto error;

				memset(&svc_pro_keycode_data, 0, sizeof(svc_pro_keycode_data));
				svc_pro_keycode_data.key = svc_key->key;

				ipc_gen_phone_res_expect_to_func(ril_request_get_id(token), IPC_SVC_PRO_KEYCODE, ipc_svc_callback);

				ipc_fmt_send(IPC_SVC_PRO_KEYCODE, IPC_TYPE_SET, (unsigned char *) &svc_pro_keycode_data, sizeof(svc_pro_keycode_data), ril_request_get_id(token));
			}
			break;
	}

	return;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
}
