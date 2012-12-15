/**
 * This file is part of samsung-ril.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2012 Paul Kocialkowski <contact@paulk.fr>
 *
 * samsung-ril is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * samsung-ril is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with samsung-ril.  If not, see <http://www.gnu.org/licenses/>.
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

/**
 * In: IPC_SAT_PROACTIVE_CMD
 *   STK proactive command
 *
 * Out: RIL_UNSOL_STK_PROACTIVE_COMMAND
 */
void ipc_sat_proactive_cmd_unsol(struct ipc_message_info *info)
{
	int data_len = (info->length-2);
	char *hexdata;

	hexdata = (char*)malloc(data_len*2+1);

	bin2hex((unsigned char*)info->data+2, data_len, hexdata);

	ril_request_unsolicited(RIL_UNSOL_STK_PROACTIVE_COMMAND, hexdata, sizeof(char*));

	free(hexdata);
}

/**
 * In: IPC_SAT_PROACTIVE_CMD RESP
 *   STK proactive command
 *
 * Out: RIL_UNSOL_STK_SESSION_END
 */
void ipc_sat_proactive_cmd_sol(struct ipc_message_info *info)
{
	unsigned char sw1, sw2;

	sw1 = ((unsigned char*)info->data)[0];
	sw2 = ((unsigned char*)info->data)[1];

	if(sw1 == 0x90 && sw2 == 0x00) {
		ril_request_unsolicited(RIL_UNSOL_STK_SESSION_END, NULL, 0);
	} else {
		LOGE("%s: unhandled response sw1=%02x sw2=%02x", __FUNCTION__, sw1, sw2);
	}
}

/**
 * Proactive command indi/resp helper function
 */
void ipc_sat_proactive_cmd(struct ipc_message_info *info)
{
	if(info->type == IPC_TYPE_INDI) {
		ipc_sat_proactive_cmd_unsol(info);
	} else if(info->type == IPC_TYPE_RESP) {
		ipc_sat_proactive_cmd_sol(info);
	} else {
		LOGE("%s: unhandled proactive command response type %d",__func__, info->type);
	}
}

/**
 * In: RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE
 *   Requests to send a terminal response to SIM for a received
 *   proactive command
 *
 * Out: IPC_SAT_PROACTIVE_CMD GET
 */
void ril_request_stk_send_terminal_response(RIL_Token t, void *data, size_t datalen)
{
	unsigned char buf[264];
	int len = (strlen(data) / 2);

	if(len > 255) {
		LOGE("%s: data exceeds maximum length", __FUNCTION__);
		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	}

	memset(buf, 0, sizeof(buf));

	buf[0] = len;
	hex2bin(data, strlen(data), &buf[1]);

	ipc_fmt_send(IPC_SAT_PROACTIVE_CMD, IPC_TYPE_GET, buf, sizeof(buf), ril_request_get_id(t));

	ril_request_complete(t, RIL_E_SUCCESS, buf, sizeof(char*));
}

/**
 * In: RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND
 *   Requests to send a SAT/USAT envelope command to SIM.
 *   The SAT/USAT envelope command refers to 3GPP TS 11.14 and 3GPP TS 31.111
 *
 * Out: IPC_SAT_ENVELOPE_CMD EXEC
 */
void ril_request_stk_send_envelope_command(RIL_Token t, void *data, size_t datalen)
{
	unsigned char buf[264];
	int len = (strlen(data) / 2);

	if(len > 255) {
		LOGE("%s: data exceeds maximum length", __FUNCTION__);
		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	}

	memset(buf, 0, sizeof(buf));

	buf[0] = len;
	hex2bin(data, strlen(data), &buf[1]);

	ipc_fmt_send(IPC_SAT_ENVELOPE_CMD, IPC_TYPE_EXEC, buf, sizeof(buf), ril_request_get_id(t));
}

/**
 * In: IPC_SAT_ENVELOPE_CMD EXEC
 *
 * Out: RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND
 *   Requests to send a SAT/USAT envelope command to SIM.
 *   The SAT/USAT envelope command refers to 3GPP TS 11.14 and 3GPP TS 31.111
 */
void ipc_sat_envelope_cmd(struct ipc_message_info *info)
{
	int data_len = (info->length-2);
	char *hexdata;

	hexdata = (char*)malloc(data_len*2+1);

	bin2hex((unsigned char*)info->data+2, data_len, hexdata);

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, hexdata, sizeof(char*));

	free(hexdata);
}

