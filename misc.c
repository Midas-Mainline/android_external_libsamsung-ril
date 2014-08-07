/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
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

int ipc_misc_me_version(struct ipc_message *message)
{
	struct ipc_misc_me_version_response_data *data;
	char *baseband_version;
	int active;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_misc_me_version_response_data))
		return -1;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_misc_me_version_response_data *) message->data;

	baseband_version = strndup(data->software_version, sizeof(data->software_version));

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) baseband_version, sizeof(baseband_version));

	free(baseband_version);

	return 0;
}

int ril_request_baseband_version(void *data, size_t size, RIL_Token token)
{
	struct ipc_misc_me_version_request_data request_data;
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_BASEBAND_VERSION, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_misc_me_version_setup(&request_data);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_MISC_ME_VERSION, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_misc_me_imsi(struct ipc_message *message)
{
	char *imsi;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_misc_me_imsi_header))
		return -1;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	imsi = ipc_misc_me_imsi_imsi_extract(message->data, message->size);
	if (imsi == NULL) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		return 0;
	}

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) imsi, sizeof(imsi));

	free(imsi);

	return 0;
}

int ril_request_get_imsi(void *data, size_t size, RIL_Token token)
{
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_GET_IMSI, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_MISC_ME_IMSI, IPC_TYPE_GET, NULL, 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_misc_me_sn(struct ipc_message *message)
{
	struct ipc_misc_me_sn_response_data *data;
	struct ril_request *request;
	char *imei;
	char *imeisv;
	unsigned int offset;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_misc_me_sn_response_data))
		return -1;

	if (message->type != IPC_TYPE_RESP)
		return 0;

	data = (struct ipc_misc_me_sn_response_data *) message->data;

	if (data->type != IPC_MISC_ME_SN_SERIAL_NUM)
		return 0;

	imei = ipc_misc_me_sn_extract(data);
	if (imei == NULL) {
		request = ril_request_find_request_status(RIL_REQUEST_GET_IMEI, RIL_REQUEST_HANDLED);
		if (request != NULL)
			ril_request_complete(request->token, RIL_E_GENERIC_FAILURE, NULL, 0);

		request = ril_request_find_request_status(RIL_REQUEST_GET_IMEISV, RIL_REQUEST_HANDLED);
		if (request != NULL)
			ril_request_complete(request->token, RIL_E_GENERIC_FAILURE, NULL, 0);

		return 0;
	}

	offset = strlen(imei) - 2;
	imeisv = strdup((char *) (imei + offset));

	imei[offset] = '\0';

	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEI, RIL_REQUEST_HANDLED);
	if (request != NULL)
		ril_request_complete(request->token, RIL_E_SUCCESS, (void *) imei, sizeof(imei));

	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEISV, RIL_REQUEST_HANDLED);
	if (request != NULL)
		ril_request_complete(request->token, RIL_E_SUCCESS, (void *) imeisv, sizeof(imeisv));

	free(imei);
	free(imeisv);

	return 0;
}

int ril_request_get_imei(void *data, size_t size, RIL_Token token)
{
	struct ipc_misc_me_sn_request_data request_data;
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEI, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	// The response to the IMEISV request will hold IMEI as well
	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEISV, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_HANDLED;

	request_data.type = IPC_MISC_ME_SN_SERIAL_NUM;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_MISC_ME_SN, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ril_request_get_imeisv(void *data, size_t size, RIL_Token token)
{
	struct ipc_misc_me_sn_request_data request_data;
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEISV, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	// The response to the IMEI request will hold IMEISV as well
	request = ril_request_find_request_status(RIL_REQUEST_GET_IMEI, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_HANDLED;

	request_data.type = IPC_MISC_ME_SN_SERIAL_NUM;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_MISC_ME_SN, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_misc_time_info(struct ipc_message *message)
{
	struct ipc_misc_time_info_data *data;
	char *string = NULL;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_misc_time_info_data))
		return -1;

	data = (struct ipc_misc_time_info_data *) message->data;

	asprintf(&string, "%02u/%02u/%02u,%02u:%02u:%02u%c%02d,%02d", data->year, data->mon, data->day, data->hour, data->min, data->sec, data->tz < 0 ? '-' : '+', data->tz < 0 ? -data->tz : data->tz, data->dl);

	ril_request_unsolicited(RIL_UNSOL_NITZ_TIME_RECEIVED, string, sizeof(string));

	if (string != NULL)
		free(string);

	return 0;
}

int ril_request_screen_state(void *data, size_t size, RIL_Token token)
{
	int value;
	int rc;

	if (data == NULL || size < sizeof(int)) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	value = *((int *) data);

	if (value)
#if RIL_VERSION >= 6
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif

	ril_request_complete(token, RIL_E_SUCCESS, NULL, 0);

	return RIL_REQUEST_COMPLETED;
}
