/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2012 Paul Kocialkowski <contact@paulk.fr>
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
 *
 */

#define LOG_TAG "RIL-SAT"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

void ril_request_report_stk_service_is_running(RIL_Token t)
{
#ifndef DISABLE_STK
	ril_request_complete(t, RIL_E_SUCCESS, NULL, 0);
#else
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
#endif
}

void ipc_sat_proactive_cmd_unsol(struct ipc_message_info *info)
{
	char *hexdata;
	int length;

	if (info == NULL || info->data == NULL || info->length < 2)
		return;

	length = (info->length - 2);
	hexdata = (char *) calloc(1, length * 2 + 1);

	bin2hex((unsigned char *) info->data + 2, length, hexdata);

	ril_request_unsolicited(RIL_UNSOL_STK_PROACTIVE_COMMAND, hexdata, sizeof(char *));

	free(hexdata);
}

void ipc_sat_proactive_cmd_sol(struct ipc_message_info *info)
{
	unsigned char sw1, sw2;

	if (info == NULL || info->data == NULL || info->length < 2 * sizeof(unsigned char))
		goto error;

	sw1 = ((unsigned char*) info->data)[0];
	sw2 = ((unsigned char*) info->data)[1];

	if (sw1 == 0x90 && sw2 == 0x00) {
		ril_request_unsolicited(RIL_UNSOL_STK_SESSION_END, NULL, 0);
	} else {
		LOGE("%s: unhandled response sw1=%02x sw2=%02x", __func__, sw1, sw2);
	}

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_sat_proactive_cmd(struct ipc_message_info *info)
{
	if (info->type == IPC_TYPE_INDI) {
		ipc_sat_proactive_cmd_unsol(info);
	} else if (info->type == IPC_TYPE_RESP) {
		ipc_sat_proactive_cmd_sol(info);
	} else {
		LOGE("%s: unhandled proactive command response type %d",__func__, info->type);
	}
}

void ril_request_stk_send_terminal_response(RIL_Token t, void *data, size_t length)
{
	unsigned char buffer[264];
	int size;

	if (data == NULL || length < sizeof(char *))
		goto error;

	size = strlen(data) / 2;
	if (size > 255) {
		LOGE("%s: data exceeds maximum length", __func__);
		goto error;
	}

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = (unsigned char) size;
	hex2bin(data, strlen(data), &buffer[1]);

	ipc_fmt_send(IPC_SAT_PROACTIVE_CMD, IPC_TYPE_GET, buffer, sizeof(buffer), ril_request_get_id(t));

	ril_request_complete(t, RIL_E_SUCCESS, buffer, sizeof(char *));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_stk_send_envelope_command(RIL_Token t, void *data, size_t length)
{
	unsigned char buffer[264];
	int size;

	if (data == NULL || length < sizeof(char *))
		goto error;

	size = strlen(data) / 2;
	if (size > 255) {
		LOGE("%s: data exceeds maximum length", __func__);
		goto error;
	}

	memset(buffer, 0, sizeof(buffer));

	buffer[0] = (unsigned char) size;
	hex2bin(data, strlen(data), &buffer[1]);

	ipc_fmt_send(IPC_SAT_ENVELOPE_CMD, IPC_TYPE_EXEC, buffer, sizeof(buffer), ril_request_get_id(t));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_sat_envelope_cmd(struct ipc_message_info *info)
{
	char *hexdata;
	int size;

	if (info == NULL || info->data == NULL || info->length < 2)
		goto error;

	size = (info->length - 2);
	hexdata = (char *) calloc(1, size * 2 + 1);

	bin2hex((unsigned char *) info->data + 2, size, hexdata);

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, hexdata, sizeof(char *));

	free(hexdata);

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

