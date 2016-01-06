/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2014 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on the CyanogenMod Smdk4210RIL implementation:
 * Copyright (C) 2011 The CyanogenMod Project
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
#include <ctype.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>
#include <plmn_list.h>

#if RIL_VERSION >= 6
int ipc2ril_disp_rssi(unsigned char rssi, RIL_SignalStrength_v6 *strength)
#else
int ipc2ril_disp_rssi(unsigned char rssi, RIL_SignalStrength *strength)
#endif
{
	int asu;

	if (strength == NULL)
		return -1;

#if RIL_VERSION >= 6
	memset(strength, -1, sizeof(RIL_SignalStrength_v6));
#else
	memset(strength, -1, sizeof(RIL_SignalStrength));
#endif

	asu = (int) rssi / -2 + 56;

	if (asu < 0)
		asu = 0;
	else if (asu > 31)
		asu = 31;

	strength->GW_SignalStrength.signalStrength = asu;
	strength->GW_SignalStrength.bitErrorRate = 99;

	RIL_LOGD("Signal strength is %d", strength->GW_SignalStrength.signalStrength);

	return 0;
}

#if RIL_VERSION >= 6
int ipc2ril_disp_icon_info(struct ipc_disp_icon_info_response_data *data,
	RIL_SignalStrength_v6 *strength)
#else
int ipc2ril_disp_icon_info(struct ipc_disp_icon_info_response_data *data,
	RIL_SignalStrength *strength)
#endif
{
	int asu_bars[] = { 1, 3, 5, 8, 12, 15 };
	unsigned int asu_bars_count = sizeof(asu_bars) / sizeof(int);
	unsigned char asu_bars_index;
	int rc;

	if (data == NULL || strength == NULL)
		return -1;

	if (!(data->flags & IPC_DISP_ICON_INFO_FLAG_RSSI))
		return -1;

#if RIL_VERSION >= 6
	memset(strength, -1, sizeof(RIL_SignalStrength_v6));
#else
	memset(strength, -1, sizeof(RIL_SignalStrength));
#endif

	asu_bars_index = data->rssi;
	if (asu_bars_index >= asu_bars_count)
		asu_bars_index = asu_bars_count - 1;

	strength->GW_SignalStrength.signalStrength = asu_bars[asu_bars_index];

	strength->GW_SignalStrength.bitErrorRate = 99;

	RIL_LOGD("Signal strength is %d", strength->GW_SignalStrength.signalStrength);

	return 0;
}

#if RIL_VERSION >= 6
int ipc2ril_disp_rssi_info(struct ipc_disp_rssi_info_data *data,
	RIL_SignalStrength_v6 *strength)
#else
int ipc2ril_disp_rssi_info(struct ipc_disp_rssi_info_data *data,
	RIL_SignalStrength *strength)
#endif
{
	int rc;

	if (data == NULL)
		return -1;

	rc = ipc2ril_disp_rssi(data->rssi, strength);
	if (rc < 0)
		return -1;

	return 0;
}

int ipc2ril_net_plmn_sel(struct ipc_net_plmn_sel_response_data *data)
{
	if (data == NULL)
		return -1;

	switch (data->plmn_sel) {
		case IPC_NET_PLMN_SEL_AUTO:
			return 0;
		case IPC_NET_PLMN_SEL_MANUAL:
			return 1;
		default:
			return -1;
	}
}

int ipc2ril_net_regist_response(struct ipc_net_regist_response_data *data,
	unsigned char hsdpa_status, char **registration,
	size_t registration_size)
{
#if RIL_VERSION >= 6
	RIL_RadioTechnology act;
#else
	int act;
#endif
	unsigned char status;

	if (data == NULL || registration == NULL)
		return -1;

	memset(registration, 0, registration_size);

#if RIL_VERSION >= 6
	switch (data->act) {
		case IPC_NET_ACCESS_TECHNOLOGY_GSM:
		case IPC_NET_ACCESS_TECHNOLOGY_GSM2:
#if RIL_VERSION >= 7
			act = RADIO_TECH_GSM;
#else
			act = RADIO_TECH_UNKNOWN;
#endif
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_GPRS:
			act = RADIO_TECH_GPRS;
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_EDGE:
			act = RADIO_TECH_EDGE;
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_UMTS:
			switch (hsdpa_status) {
				case IPC_GPRS_HSDPA_STATUS_HSDPA:
					act = RADIO_TECH_HSDPA;
					break;
				case IPC_GPRS_HSDPA_STATUS_HSPAP:
					act = RADIO_TECH_HSPAP;
					break;
				default:
					act = RADIO_TECH_UMTS;
					break;
			}
			break;
		default:
			act = RADIO_TECH_UNKNOWN;
			break;
	}
#else
	switch (data->act) {
		case IPC_NET_ACCESS_TECHNOLOGY_GSM:
		case IPC_NET_ACCESS_TECHNOLOGY_GSM2:
			act = 0;
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_GPRS:
			act = 1;
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_EDGE:
			act = 2;
			break;
		case IPC_NET_ACCESS_TECHNOLOGY_UMTS:
			switch (hsdpa_status) {
				case IPC_GPRS_HSDPA_STATUS_HSDPA:
				case IPC_GPRS_HSDPA_STATUS_HSPAP:
					act = 9;
					break;
				default:
					act = 3;
					break;
			}
			break;
		default:
			act = 0;
			break;
	}
#endif

	switch (data->status) {
		case IPC_NET_REGISTRATION_STATUS_NONE:
			status = 0;
			break;
		case IPC_NET_REGISTRATION_STATUS_HOME:
			status = 1;
			break;
		case IPC_NET_REGISTRATION_STATUS_SEARCHING:
			status = 12;
			break;
		case IPC_NET_REGISTRATION_STATUS_EMERGENCY:
			status = 10;
			break;
		case IPC_NET_REGISTRATION_STATUS_UNKNOWN:
			status = 14;
			break;
		case IPC_NET_REGISTRATION_STATUS_ROAMING:
			status = 5;
			break;
		default:
			status = 0;
			break;
	}

	asprintf(&registration[0], "%d", status);
	asprintf(&registration[1], "%x", data->lac);
	asprintf(&registration[2], "%x", data->cid);
	asprintf(&registration[3], "%d", act);

	return 0;
}

int ipc2ril_net_operator(char *data, size_t size, char **plmn,
	char **operator_long, char **operator_short)
{
	char buffer[7] = { 0 };
	unsigned int mcc = 0;
	unsigned int mnc = 0;
	unsigned int count;
	unsigned int i;
	int rc;

	if (data == NULL || size == 0 || plmn == NULL)
		return -1;

	*plmn = NULL;

	count = size / sizeof(char);

	for (i = 0; i < count; i++) {
		if (!isdigit(data[i])) {
			buffer[i] = '\0';
			break;
		}

		buffer[i] = data[i];
	}

	if (buffer[0] == '\0')
		goto error;

	*plmn = strdup(buffer);

	if (operator_long == NULL || operator_short == NULL) {
		rc = 0;
		goto complete;
	}

	*operator_long = NULL;
	*operator_short = NULL;

	rc = sscanf((char *) &buffer, "%3u%2u", &mcc, &mnc);
	if (rc < 2)
		goto error;

	for (i = 0 ; i < plmn_list_count ; i++) {
		if (plmn_list[i].mcc == mcc && plmn_list[i].mnc == mnc) {
			*operator_long = strdup(plmn_list[i].operator_long);
			*operator_short = strdup(plmn_list[i].operator_short);
		}
	}

	if (*operator_long == NULL || *operator_short == NULL) {
		RIL_LOGE("%s: Finding operator with PLMN %d%d failed", __func__, mcc, mnc);
		goto error;
	}

	rc = 0;
	goto complete;

error:
	if (*plmn != NULL) {
		free(*plmn);
		*plmn = NULL;
	}

	if (operator_long != NULL && *operator_long != NULL) {
		free(*operator_long);
		*operator_long = NULL;
	}

	if (operator_short != NULL && *operator_short != NULL) {
		free(*operator_short);
		*operator_short = NULL;
	}

	rc = -1;

complete:
	return rc;
}

int ipc2ril_net_serving_network(struct ipc_net_serving_network_data *data,
	char **operator)
{
	int rc;

	if (operator == NULL)
		return -1;

	rc = ipc2ril_net_operator((char *) &data->plmn, sizeof(data->plmn), &operator[2], &operator[0], &operator[1]);
	if (rc < 0)
		return -1;

	return 0;
}

#if RIL_VERSION >= 6
RIL_PreferredNetworkType ipc2ril_net_mode_sel(struct ipc_net_mode_sel_data *data)
#else
int ipc2ril_net_mode_sel(struct ipc_net_mode_sel_data *data)
#endif
{
	if (data == NULL)
		return -1;

#if RIL_VERSION >= 6
	switch (data->mode_sel) {
		case IPC_NET_MODE_SEL_GSM_UMTS:
			return PREF_NET_TYPE_GSM_WCDMA;
		case IPC_NET_MODE_SEL_GSM_ONLY:
			return PREF_NET_TYPE_GSM_ONLY;
		case IPC_NET_MODE_SEL_UMTS_ONLY:
			return PREF_NET_TYPE_WCDMA;
		default:
			return -1;
	}
#else
	switch (data->mode_sel) {
		case IPC_NET_MODE_SEL_GSM_UMTS:
			return 0;
		case IPC_NET_MODE_SEL_GSM_ONLY:
			return 1;
		case IPC_NET_MODE_SEL_UMTS_ONLY:
			return 2;
		default:
			return -1;
	}
#endif
}

#if RIL_VERSION >= 6
unsigned char ril2ipc_net_mode_sel(RIL_PreferredNetworkType type)
#else
unsigned char ril2ipc_net_mode_sel(int type)
#endif
{
#if RIL_VERSION >= 6
	switch (type) {
		case PREF_NET_TYPE_GSM_WCDMA:
		case PREF_NET_TYPE_GSM_WCDMA_AUTO:
			return IPC_NET_MODE_SEL_GSM_UMTS;
		case PREF_NET_TYPE_GSM_ONLY:
			return IPC_NET_MODE_SEL_GSM_ONLY;
		case PREF_NET_TYPE_WCDMA:
			return IPC_NET_MODE_SEL_UMTS_ONLY;
		default:
			return IPC_NET_MODE_SEL_GSM_UMTS;
	}
#else
	switch (type) {
		case 0:
		case 3:
			return IPC_NET_MODE_SEL_GSM_UMTS;
		case 1:
			return IPC_NET_MODE_SEL_GSM_ONLY;
		case 2:
			return IPC_NET_MODE_SEL_UMTS_ONLY;
		default:
			return IPC_NET_MODE_SEL_GSM_UMTS;
	}
#endif
}

int ipc_disp_icon_info(struct ipc_message *message)
{
	struct ipc_disp_icon_info_response_data *data;
#if RIL_VERSION >= 6
	RIL_SignalStrength_v6 strength;
#else
	RIL_SignalStrength strength;
#endif
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_disp_icon_info_response_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_disp_icon_info_response_data *) message->data;

	rc = ipc2ril_disp_icon_info(data, &strength);
	if (rc < 0) {
		if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq))
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

		return 0;
	}

	if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq))
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &strength, sizeof(strength));
	else
		ril_request_unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, (void *) &strength, sizeof(strength));

	return 0;
}

int ril_request_signal_strength(void *data, size_t size, RIL_Token token)
{
	struct ipc_disp_icon_info_request_data request_data;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	memset(&request_data, 0, sizeof(request_data));
	request_data.flags = IPC_DISP_ICON_INFO_FLAG_RSSI;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_DISP_ICON_INFO, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_disp_rssi_info(struct ipc_message *message)
{
	struct ipc_disp_rssi_info_data *data;
#if RIL_VERSION >= 6
	RIL_SignalStrength_v6 strength;
#else
	RIL_SignalStrength strength;
#endif
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_disp_rssi_info_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_disp_rssi_info_data *) message->data;

	rc = ipc2ril_disp_rssi_info(data, &strength);
	if (rc < 0)
		return 0;

	ril_request_unsolicited(RIL_UNSOL_SIGNAL_STRENGTH, (void *) &strength, sizeof(strength));

	return 0;
}

int ipc_net_plmn_sel(struct ipc_message *message)
{
	struct ipc_net_plmn_sel_response_data *data;
	int selection;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_net_plmn_sel_response_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_net_plmn_sel_response_data *) message->data;

	selection = ipc2ril_net_plmn_sel(data);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &selection, sizeof(selection));

	return 0;
}

int ril_request_query_network_selection_mode(void *data, size_t size,
	RIL_Token token)
{
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_PLMN_SEL, IPC_TYPE_GET, NULL, 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_net_plmn_sel_callback(struct ipc_message *message)
{
	struct ipc_gen_phone_res_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0) {
		if ((data->code & 0xff) == 0x6f) {
			RIL_LOGE("%s: Unauthorized network selection", __func__);
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_ILLEGAL_SIM_OR_ME, NULL, 0);
		} else {
			RIL_LOGE("%s: Network selection failed", __func__);
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		}
	} else {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
	}

	return 0;
}

int ril_request_set_network_selection_automatic(void *data, size_t size,
	RIL_Token token)
{
	struct ipc_net_plmn_sel_request_data request_data;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_net_plmn_sel_setup(&request_data, IPC_NET_PLMN_SEL_AUTO, NULL, IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN);
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_NET_PLMN_SEL, ipc_net_plmn_sel_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_PLMN_SEL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
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

int ril_request_set_network_selection_manual(void *data, size_t size,
	RIL_Token token)
{
	struct ipc_net_plmn_sel_request_data request_data;
	int rc;

	if (data == NULL || size < sizeof(char)) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_net_plmn_sel_setup(&request_data, IPC_NET_PLMN_SEL_MANUAL, (const char *) data, IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN);
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_NET_PLMN_SEL, ipc_net_plmn_sel_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_PLMN_SEL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
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

int ipc_net_serving_network(struct ipc_message *message)
{
	struct ipc_net_serving_network_data *data;
	char *operator[3] = { NULL };
	unsigned char count;
	unsigned int i;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_net_regist_response_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_net_serving_network_data *) message->data;

	rc = ipc2ril_net_serving_network(data, (char **) &operator);
	if (rc < 0) {
		if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq))
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

		return 0;
	}

	count = sizeof(operator) / sizeof(char *);

	if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq)) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &operator, sizeof(operator));

		for (i = 0; i < count; i++) {
			if (operator[i] != NULL)
				free(operator[i]);
		}
	} else {
		ril_request_data_set(RIL_REQUEST_OPERATOR, (void *) operator, sizeof(operator));
#if RIL_VERSION >= 6
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
	}

	return 0;
}

int ril_request_operator(void *data, size_t size, RIL_Token token)
{
	struct ril_request *request;
	void *operator_data;
	size_t operator_size;
	char **operator;
	unsigned char count;
	unsigned int i;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_OPERATOR, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	operator_size = ril_request_data_size_get(RIL_REQUEST_OPERATOR);
	operator_data = ril_request_data_get(RIL_REQUEST_OPERATOR);

	if (operator_data != NULL && operator_size > 0) {
		ril_request_complete(token, RIL_E_SUCCESS, operator_data, operator_size);

		count = operator_size / sizeof(char *);
		operator = (char **) operator_data;

		for (i = 0; i < count; i++) {
			if (operator[i] != NULL)
				free(operator[i]);
		}

		free(operator_data);

		return RIL_REQUEST_COMPLETED;
	} else {
		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_SERVING_NETWORK, IPC_TYPE_GET, NULL, 0);
		if (rc < 0) {
			ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
			return RIL_REQUEST_COMPLETED;
		}

		return RIL_REQUEST_HANDLED;
	}
}

int ipc_net_plmn_list(struct ipc_message *message)
{
	struct ipc_net_plmn_list_entry *entry;
	char **networks = NULL;
	size_t networks_size;
	unsigned int networks_count = 0;
	char **network;
	unsigned char count;
	unsigned char index;
	unsigned int i;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_net_plmn_list_header))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	count = ipc_net_plmn_list_count_extract(message->data, message->size);
	if (count == 0) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
		return 0;
	}

	networks_size = count * 4 * sizeof(char *);
	networks = calloc(1, networks_size);

	for (index = 0; index < count; index++) {
		entry = ipc_net_plmn_list_entry_extract(message->data, message->size, index);
		if (entry == NULL)
			goto error;

		network = (char **) ((unsigned char *) networks + networks_count * 4 * sizeof(char *));

		rc = ipc2ril_net_operator(entry->plmn, sizeof(entry->plmn), &network[2], &network[0], &network[1]);
		if (rc < 0)
			goto error;

		switch (entry->status) {
			case IPC_NET_PLMN_STATUS_AVAILABLE:
				network[3] = strdup("available");
				break;
			case IPC_NET_PLMN_STATUS_CURRENT:
				network[3] = strdup("current");
				break;
			case IPC_NET_PLMN_STATUS_FORBIDDEN:
				network[3] = strdup("forbidden");
				break;
			default:
				network[3] = strdup("unknown");
				break;
		}

		networks_count++;
	}

	RIL_LOGD("Found %d available networks", networks_count);

	networks_size = networks_count * 4 * sizeof(char *);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) networks, networks_size);

	goto complete;

error:
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	if (networks != NULL && networks_size > 0) {
		for (index = 0; index < networks_count; index++) {
			if (networks[index] != NULL)
				free(networks[index]);
		}

		free(networks);
	}

	return 0;
}

int ril_request_query_available_networks(void *data, size_t size,
	RIL_Token token)
{
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_QUERY_AVAILABLE_NETWORKS, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_PLMN_LIST, IPC_TYPE_GET, NULL, 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_net_regist(struct ipc_message *message)
{
	struct ipc_net_regist_response_data *data;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	struct ipc_client_gprs_capabilities gprs_capabilities;
	char *voice_registration[15] = { NULL };
	char *data_registration[5] = { NULL };
	char **registration;
	size_t registration_size;
	unsigned int count;
	unsigned int i;
	int request;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_net_regist_response_data))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return 0;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_net_regist_response_data *) message->data;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	if (data->domain == IPC_NET_SERVICE_DOMAIN_GSM) {
#if RIL_VERSION >= 6
		request = RIL_REQUEST_VOICE_REGISTRATION_STATE;
#else
		request = RIL_REQUEST_REGISTRATION_STATE;
#endif

		registration = (char **) &voice_registration;
		registration_size = sizeof(voice_registration);
		count = registration_size / sizeof(char *);
	} else if (data->domain == IPC_NET_SERVICE_DOMAIN_GPRS) {
#if RIL_VERSION >= 6
		request = RIL_REQUEST_DATA_REGISTRATION_STATE;
#else
		request = RIL_REQUEST_GPRS_REGISTRATION_STATE;
#endif

		registration = (char **) &data_registration;
		registration_size = sizeof(data_registration);
		count = registration_size / sizeof(char *);
	} else {
		RIL_LOGD("%s: Invalid networking registration domain (0x%x)", __func__, data->domain);
		return 0;
	}

	rc = ipc2ril_net_regist_response(data, ipc_fmt_data->hsdpa_status_data.status, registration, registration_size);
	if (rc < 0)
		goto error;

	if (data->domain == IPC_NET_SERVICE_DOMAIN_GPRS) {
		if (ipc_fmt_data->ipc_client == NULL)
			goto error;

		rc = ipc_client_gprs_get_capabilities(ipc_fmt_data->ipc_client, &gprs_capabilities);
		if (rc < 0)
			goto error;

		asprintf(&registration[4], "%d", gprs_capabilities.cid_count);
	}

	if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq)) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) registration, registration_size);

		for (i = 0; i < count; i++) {
			if (registration[i] != NULL)
				free(registration[i]);
		}

		return 0;
	} else {
		ril_request_data_set(request, (void *) registration, registration_size);

#if RIL_VERSION >= 6
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
	}

	goto complete;

error:
	if (ipc_seq_valid(message->aseq))
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

	for (i = 0; i < count; i++) {
		if (registration[i] != NULL)
			free(registration[i]);
	}

complete:
	return 0;
}

#if RIL_VERSION >= 6
int ril_request_voice_registration_state(void *data, size_t size,
	RIL_Token token)
#else
int ril_request_registration_state(void *data, size_t size, RIL_Token token)
#endif
{
	struct ipc_net_regist_request_data request_data;
	struct ril_request *request;
	void *registration_data;
	size_t registration_size;
	char **registration;
	unsigned int count;
	unsigned int i;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

#if RIL_VERSION >= 6
	request = ril_request_find_request_status(RIL_REQUEST_VOICE_REGISTRATION_STATE, RIL_REQUEST_HANDLED);
#else
	request = ril_request_find_request_status(RIL_REQUEST_REGISTRATION_STATE, RIL_REQUEST_HANDLED);
#endif
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

#if RIL_VERSION >= 6
	registration_size = ril_request_data_size_get(RIL_REQUEST_VOICE_REGISTRATION_STATE);
	registration_data = ril_request_data_get(RIL_REQUEST_VOICE_REGISTRATION_STATE);
#else
	registration_size = ril_request_data_size_get(RIL_REQUEST_REGISTRATION_STATE);
	registration_data = ril_request_data_get(RIL_REQUEST_REGISTRATION_STATE);
#endif
	if (registration_data != NULL && registration_size > 0) {
		ril_request_complete(token, RIL_E_SUCCESS, registration_data, registration_size);

		count = registration_size / sizeof(char *);
		registration = (char **) registration_data;

		for (i = 0; i < count; i++) {
			if (registration[i] != NULL)
				free(registration[i]);
		}

		free(registration_data);

		rc = RIL_REQUEST_COMPLETED;
	} else {
		rc = ipc_net_regist_setup(&request_data, IPC_NET_SERVICE_DOMAIN_GSM);
		if (rc < 0)
			goto error;

		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_REGIST, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
		if (rc < 0)
			goto error;

		rc = RIL_REQUEST_HANDLED;
	}

	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

#if RIL_VERSION >= 6
int ril_request_data_registration_state(void *data, size_t size,
	RIL_Token token)
#else
int ril_request_gprs_registration_state(void *data, size_t size,
	RIL_Token token)
#endif
{
	struct ipc_net_regist_request_data request_data;
	struct ril_request *request;
	void *registration_data;
	size_t registration_size;
	char **registration;
	unsigned int count;
	unsigned int i;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

#if RIL_VERSION >= 6
	request = ril_request_find_request_status(RIL_REQUEST_DATA_REGISTRATION_STATE, RIL_REQUEST_HANDLED);
#else
	request = ril_request_find_request_status(RIL_REQUEST_GPRS_REGISTRATION_STATE, RIL_REQUEST_HANDLED);
#endif
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

#if RIL_VERSION >= 6
	registration_size = ril_request_data_size_get(RIL_REQUEST_DATA_REGISTRATION_STATE);
	registration_data = ril_request_data_get(RIL_REQUEST_DATA_REGISTRATION_STATE);
#else
	registration_size = ril_request_data_size_get(RIL_REQUEST_GPRS_REGISTRATION_STATE);
	registration_data = ril_request_data_get(RIL_REQUEST_GPRS_REGISTRATION_STATE);
#endif
	if (registration_data != NULL && registration_size > 0) {
		ril_request_complete(token, RIL_E_SUCCESS, registration_data, registration_size);

		count = registration_size / sizeof(char *);
		registration = (char **) registration_data;

		for (i = 0; i < count; i++) {
			if (registration[i] != NULL)
				free(registration[i]);
		}

		free(registration_data);

		rc = RIL_REQUEST_COMPLETED;
	} else {
		rc = ipc_net_regist_setup(&request_data, IPC_NET_SERVICE_DOMAIN_GPRS);
		if (rc < 0)
			goto error;

		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_REGIST, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
		if (rc < 0)
			goto error;

		rc = RIL_REQUEST_HANDLED;
	}

	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_net_mode_sel(struct ipc_message *message)
{
	struct ipc_net_mode_sel_data *data;
	int type;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_net_mode_sel_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_net_mode_sel_data *) message->data;

	type = ipc2ril_net_mode_sel(data);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &type, sizeof(type));

	return 0;
}

int ril_request_get_preferred_network_type(void *data, size_t size,
	RIL_Token token)
{
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_MODE_SEL, IPC_TYPE_GET, NULL, 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ril_request_set_preferred_network_type(void *data, size_t size,
	RIL_Token token)
{
	struct ipc_net_mode_sel_data request_data;
	int type;
	int rc;

	if (data == NULL || size < sizeof(int))
		goto error;

	type = *((int *) data);

	memset(&request_data, 0, sizeof(request_data));
	request_data.mode_sel = ril2ipc_net_mode_sel(type);

	if (request_data.mode_sel == 0) {
		ril_request_complete(token, RIL_E_MODE_NOT_SUPPORTED, NULL, 0);

		rc = RIL_REQUEST_COMPLETED;
		goto complete;
	}

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_NET_MODE_SEL);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_NET_MODE_SEL, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
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
