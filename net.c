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

#define LOG_TAG "RIL-NET"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

#include <plmn_list.h>

unsigned char ipc2ril_reg_state(unsigned char reg_state)
{
	switch(reg_state) {
		case IPC_NET_REGISTRATION_STATE_NONE:
			return 0;
		case IPC_NET_REGISTRATION_STATE_HOME:
			return 1;
		case IPC_NET_REGISTRATION_STATE_SEARCHING:
			return 2;
		case IPC_NET_REGISTRATION_STATE_EMERGENCY:
			return 10;
		case IPC_NET_REGISTRATION_STATE_ROAMING:
			return 5;
		case IPC_NET_REGISTRATION_STATE_UNKNOWN:
			return 4;
		default:
			LOGE("%s: invalid reg_state: %d", __func__, reg_state);
			return 0;
	}
}

unsigned char ipc2ril_act(unsigned char act)
{
	switch(act) {
		case IPC_NET_ACCESS_TECHNOLOGY_GPRS:
			return 1;
		case IPC_NET_ACCESS_TECHNOLOGY_EDGE:
			return 2;
		case IPC_NET_ACCESS_TECHNOLOGY_UMTS:
			return 3;
		case IPC_NET_ACCESS_TECHNOLOGY_GSM:
		case IPC_NET_ACCESS_TECHNOLOGY_GSM2:
		default:
			return 0;
	}
}

unsigned char ipc2ril_gprs_act(unsigned char act)
{
	switch(act) {
		case IPC_NET_ACCESS_TECHNOLOGY_GPRS:
			return 1;
		case IPC_NET_ACCESS_TECHNOLOGY_EDGE:
			return 2;
		case IPC_NET_ACCESS_TECHNOLOGY_UMTS:
			return 3;
		case IPC_NET_ACCESS_TECHNOLOGY_GSM:
		case IPC_NET_ACCESS_TECHNOLOGY_GSM2:
		default:
			return 0;
	}
}

int ipc2ril_mode_sel(unsigned char mode)
{
	switch(mode) {
		case 0:
			return 7; // auto mode
		case IPC_NET_MODE_SEL_GSM_UMTS:
			return 0;
		case IPC_NET_MODE_SEL_GSM_ONLY:
			return 1;
		case IPC_NET_MODE_SEL_UMTS_ONLY:
			return 2;
		default:
			return 0;
	}
}

unsigned char ril2ipc_mode_sel(int mode)
{
	switch(mode) {
		case 1: // GSM only
			return IPC_NET_MODE_SEL_GSM_ONLY;
		case 2: // WCDMA only
			return IPC_NET_MODE_SEL_UMTS_ONLY;
		case 0:
		default: // GSM/WCDMA + the rest
			return IPC_NET_MODE_SEL_GSM_UMTS;
	}
}

int ipc2ril_plmn_sel(unsigned char mode)
{
	switch(mode) {
		case IPC_NET_PLMN_SEL_MANUAL:
			return 1;
		case IPC_NET_PLMN_SEL_AUTO:
			return 0;
		default:
			return 0;
	}
}

unsigned char ril2ipc_plmn_sel(int mode)
{
	switch(mode) {
		case 0:
			return IPC_NET_PLMN_SEL_AUTO;
		case 1:
			return IPC_NET_PLMN_SEL_MANUAL;
		default:
			return 0;
	}
}

void ipc2ril_reg_state_resp(struct ipc_net_regist_response *netinfo, char *response[15])
{
	unsigned char reg_state;
	unsigned char act;

	if (netinfo == NULL || response == NULL)
		return;

	reg_state = ipc2ril_reg_state(netinfo->reg_state);
	act = ipc2ril_act(netinfo->act);

	memset(response, 0, sizeof(response));

	asprintf(&response[0], "%d", reg_state);
	asprintf(&response[1], "%x", netinfo->lac);
	asprintf(&response[2], "%x", netinfo->cid);
	asprintf(&response[3], "%d", act);
}

void ipc2ril_gprs_reg_state_resp(struct ipc_net_regist_response *netinfo, char *response[4])
{
	unsigned char reg_state;
	unsigned char act;

	if (netinfo == NULL || response == NULL)
		return;

	reg_state = ipc2ril_reg_state(netinfo->reg_state);
	act = ipc2ril_gprs_act(netinfo->act);

	memset(response, 0, sizeof(response));

	asprintf(&response[0], "%d", reg_state);
	asprintf(&response[1], "%x", netinfo->lac);
	asprintf(&response[2], "%x", netinfo->cid);
	asprintf(&response[3], "%d", act);
}

/*
 * Set all the tokens to data waiting.
 * For instance when only operator is updated by modem NOTI, we don't need
 * to ask the modem new NET Regist and GPRS Net Regist states so act like we got
 * these from modem NOTI too so we don't have to make the requests
 */
void ril_tokens_net_set_data_waiting(void)
{
	ril_data.tokens.registration_state = RIL_TOKEN_DATA_WAITING;
	ril_data.tokens.gprs_registration_state = RIL_TOKEN_DATA_WAITING;
	ril_data.tokens.operator = RIL_TOKEN_DATA_WAITING;
}

/*
 * Returns 1 if unsol data is waiting, 0 if not
 */
int ril_tokens_net_get_data_waiting(void)
{
	return ril_data.tokens.registration_state == RIL_TOKEN_DATA_WAITING || ril_data.tokens.gprs_registration_state == RIL_TOKEN_DATA_WAITING || ril_data.tokens.operator == RIL_TOKEN_DATA_WAITING;
}

void ril_tokens_net_state_dump(void)
{
	LOGD("ril_tokens_net_state_dump:\n\
	\tril_data.tokens.registration_state = %p\n\
	\tril_data.tokens.gprs_registration_state = %p\n\
	\tril_data.tokens.operator = %p\n", ril_data.tokens.registration_state, ril_data.tokens.gprs_registration_state, ril_data.tokens.operator);
}

void ril_plmn_split(char *plmn_data, char **plmn, unsigned int *mcc, unsigned int *mnc)
{
	char plmn_t[7];
	int i;

	memset(plmn_t, 0, sizeof(plmn_t));
	memcpy(plmn_t, plmn_data, 6);

	if (plmn_t[5] == '#')
		plmn_t[5] = '\0';

	if (plmn != NULL) {
		*plmn = malloc(sizeof(plmn_t));
		memcpy(*plmn, plmn_t, sizeof(plmn_t));
	}

	if (mcc == NULL || mnc == NULL)
		return;

	sscanf(plmn_t, "%3u%2u", mcc, mnc);
}

void ril_plmn_string(char *plmn_data, char *response[3])
{
	unsigned int mcc, mnc;
	char *plmn = NULL;

	int plmn_entries;
	int i;

	if (plmn_data == NULL || response == NULL)
		return;

	ril_plmn_split(plmn_data, &plmn, &mcc, &mnc);

	asprintf(&response[2], "%s", plmn);

	if (plmn != NULL)
		free(plmn);

	plmn_entries = sizeof(plmn_list) / sizeof(struct plmn_list_entry);

	LOGD("Found %d plmn records", plmn_entries);

	for (i = 0 ; i < plmn_entries ; i++) {
		if (plmn_list[i].mcc == mcc && plmn_list[i].mnc == mnc) {
			asprintf(&response[0], "%s", plmn_list[i].operator_long);
			asprintf(&response[1], "%s", plmn_list[i].operator_short);
			return;	
		}
	}

	response[0] = NULL;
	response[1] = NULL;
}

/*
 * How to handle NET unsol data from modem:
 * 1- Rx UNSOL (NOTI) data from modem
 * 2- copy data in a sized variable stored in radio
 * 3- make sure no SOL request is going on for this token
 * 4- copy data to radio structure
 * 5- if no UNSOL data is already waiting for a token, tell RILJ NETWORK_STATE_CHANGED
 * 6- set all the net tokens to RIL_TOKEN_DATA_WAITING
 * 7- RILJ will ask for OPERATOR, GPRS_REG_STATE and REG_STATE
 * for each request:
 * 8- if token is RIL_TOKEN_DATA_WAITING it's SOL request for modem UNSOL data
 * 9- send back modem data and tell E_SUCCESS to RILJ request
 * 10- set token to 0x00
 *
 * How to handle NET sol requests from RILJ:
 * 1- if token is 0x00 it's UNSOL RILJ request for modem data
 * 2- put RIL_Token in token
 * 3- request data to the modem
 * 4- Rx SOL (RESP) data from modem
 * 5- copy data to radio structure
 * 6- send back data to RILJ with token from modem message
 * 7- if token != RIL_TOKEN_DATA_WAITING, reset token to 0x00
 *
 * What if both are appening at the same time?
 * 1- RILJ requests modem data (UNSOL)
 * 2- token is 0x00 so send request to modem
 * 3- UNSOL data arrives from modem
 * 4- set all tokens to RIL_TOKEN_DATA_WAITING
 * 5- store data, tell RILJ NETWORK_STATE_CHANGED
 * 6- Rx requested data from modem
 * 7- copy data to radio structure
 * 8- token mismatch (is now RIL_TOKEN_DATA_WAITING)
 * 9- send back data to RIL with token from IPC message
 * 10- don't reset token to 0x00
 * 11- RILJ does SOL request for modem data (we know it's SOL because we didn't reset token)
 * 12- send back last data we have (from UNSOL RILJ request here)
 */

void ril_request_operator(RIL_Token t)
{
	char *response[3];
	size_t i;

	// IPC_NET_REGISTRATION_STATE_ROAMING is the biggest valid value
	if (ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_NONE ||
	ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_SEARCHING ||
	ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_UNKNOWN ||
	ril_data.state.netinfo.reg_state > IPC_NET_REGISTRATION_STATE_ROAMING) {
		ril_request_complete(t, RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW, NULL, 0);

		ril_data.tokens.operator = RIL_TOKEN_NULL;
		return;
	}

	if (ril_data.tokens.operator == RIL_TOKEN_DATA_WAITING) {
		LOGD("Got RILJ request for UNSOL data");

		/* Send back the data we got UNSOL */
		ril_plmn_string(ril_data.state.plmndata.plmn, response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}

		ril_data.tokens.operator = RIL_TOKEN_NULL;
	} else if (ril_data.tokens.operator == RIL_TOKEN_NULL) {
		LOGD("Got RILJ request for SOL data");
		/* Request data to the modem */
		ril_data.tokens.operator = t;

		ipc_fmt_send_get(IPC_NET_CURRENT_PLMN, ril_request_get_id(t));
	} else {
		LOGE("Another request is going on, returning UNSOL data");

		/* Send back the data we got UNSOL */
		ril_plmn_string(ril_data.state.plmndata.plmn, response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}
	}

	ril_tokens_net_state_dump();
}

void ipc_net_current_plmn(struct ipc_message_info *info)
{
	struct ipc_net_current_plmn_response *plmndata;
	RIL_Token t;

	char *response[3];
	size_t i;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_current_plmn_response))
		goto error;

	plmndata = (struct ipc_net_current_plmn_response *) info->data;
	t = ril_request_get_token(info->aseq);

	switch (info->type) {
		case IPC_TYPE_NOTI:
			LOGD("Got UNSOL Operator message");

			// IPC_NET_REGISTRATION_STATE_ROAMING is the biggest valid value
			if (ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_NONE ||
			ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_SEARCHING ||
			ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_UNKNOWN ||
			ril_data.state.netinfo.reg_state > IPC_NET_REGISTRATION_STATE_ROAMING) {
				/* Better keeping it up to date */
				memcpy(&(ril_data.state.plmndata), plmndata, sizeof(struct ipc_net_current_plmn_response));

				return;
			} else {
				if (ril_data.tokens.operator != RIL_TOKEN_NULL && ril_data.tokens.operator != RIL_TOKEN_DATA_WAITING) {
					LOGE("Another Operator Req is in progress, skipping");
					return;
				}

				memcpy(&(ril_data.state.plmndata), plmndata, sizeof(struct ipc_net_current_plmn_response));

				/* we already told RILJ to get the new data but it wasn't done yet */
				if (ril_tokens_net_get_data_waiting() && ril_data.tokens.operator == RIL_TOKEN_DATA_WAITING) {
					LOGD("Updating Operator data in background");
				} else {
					ril_tokens_net_set_data_waiting();
#if RIL_VERSION >= 6
					ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
					ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
				}
			}
			break;
		case IPC_TYPE_RESP:
			LOGD("Got SOL Operator message");

			// IPC_NET_REGISTRATION_STATE_ROAMING is the biggest valid value
			if (ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_NONE ||
			ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_SEARCHING ||
			ril_data.state.netinfo.reg_state == IPC_NET_REGISTRATION_STATE_UNKNOWN ||
			ril_data.state.netinfo.reg_state > IPC_NET_REGISTRATION_STATE_ROAMING) {
				/* Better keeping it up to date */
				memcpy(&(ril_data.state.plmndata), plmndata, sizeof(struct ipc_net_current_plmn_response));

				ril_request_complete(t, RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW, NULL, 0);

				if (ril_data.tokens.operator != RIL_TOKEN_DATA_WAITING)
					ril_data.tokens.operator = RIL_TOKEN_NULL;
				return;
			} else {
				if (ril_data.tokens.operator != t)
					LOGE("Operator tokens mismatch");

				/* Better keeping it up to date */
				memcpy(&(ril_data.state.plmndata), plmndata, sizeof(struct ipc_net_current_plmn_response));

				ril_plmn_string(plmndata->plmn, response);

				ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

				for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
					if (response[i] != NULL)
						free(response[i]);
				}

				if (ril_data.tokens.operator != RIL_TOKEN_DATA_WAITING)
					ril_data.tokens.operator = RIL_TOKEN_NULL;
			}
			break;
		default:
			LOGE("%s: unhandled ipc method: %d", __func__, info->type);
			break;
	}

	ril_tokens_net_state_dump();

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

#if RIL_VERSION >= 6
void ril_request_voice_registration_state(RIL_Token t)
#else
void ril_request_registration_state(RIL_Token t)
#endif
{
	struct ipc_net_regist_get regist_req;
	char *response[4];
	int i;

	if (ril_data.tokens.registration_state == RIL_TOKEN_DATA_WAITING) {
		LOGD("Got RILJ request for UNSOL data");

		/* Send back the data we got UNSOL */
		ipc2ril_reg_state_resp(&(ril_data.state.netinfo), response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < (int) (sizeof(response) / sizeof(char *)) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}

		ril_data.tokens.registration_state = RIL_TOKEN_NULL;
	} else if (ril_data.tokens.registration_state == RIL_TOKEN_NULL) {
		LOGD("Got RILJ request for SOL data");
		/* Request data to the modem */
		ril_data.tokens.registration_state = t;

		ipc_net_regist_get_setup(&regist_req, IPC_NET_SERVICE_DOMAIN_GSM);
		ipc_fmt_send(IPC_NET_REGIST, IPC_TYPE_GET, (void *)&regist_req, sizeof(struct ipc_net_regist_get), ril_request_get_id(t));
	} else {
		LOGE("Another request is going on, returning UNSOL data");

		/* Send back the data we got UNSOL */
		ipc2ril_reg_state_resp(&(ril_data.state.netinfo), response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < (int) (sizeof(response) / sizeof(char *)) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}
	}

	ril_tokens_net_state_dump();
}

#if RIL_VERSION >= 6
void ril_request_data_registration_state(RIL_Token t)
#else
void ril_request_gprs_registration_state(RIL_Token t)
#endif
{
	struct ipc_net_regist_get regist_req;
	char *response[4];
	size_t i;

	if (ril_data.tokens.gprs_registration_state == RIL_TOKEN_DATA_WAITING) {
		LOGD("Got RILJ request for UNSOL data");

		/* Send back the data we got UNSOL */
		ipc2ril_gprs_reg_state_resp(&(ril_data.state.gprs_netinfo), response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}

		ril_data.tokens.gprs_registration_state = RIL_TOKEN_NULL;
	} else if (ril_data.tokens.gprs_registration_state == RIL_TOKEN_NULL) {
		LOGD("Got RILJ request for SOL data");

		/* Request data to the modem */
		ril_data.tokens.gprs_registration_state = t;

		ipc_net_regist_get_setup(&regist_req, IPC_NET_SERVICE_DOMAIN_GPRS);
		ipc_fmt_send(IPC_NET_REGIST, IPC_TYPE_GET, (void *)&regist_req, sizeof(struct ipc_net_regist_get), ril_request_get_id(t));
	} else {
		LOGE("Another request is going on, returning UNSOL data");

		/* Send back the data we got UNSOL */
		ipc2ril_gprs_reg_state_resp(&(ril_data.state.gprs_netinfo), response);

		ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

		for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
			if (response[i] != NULL)
				free(response[i]);
		}
	}

	ril_tokens_net_state_dump();
}

void ipc_net_regist_unsol(struct ipc_message_info *info)
{
	struct ipc_net_regist_response *netinfo;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_regist_response))
		goto error;

	netinfo = (struct ipc_net_regist_response *) info->data;

	LOGD("Got UNSOL NetRegist message");

	switch(netinfo->domain) {
		case IPC_NET_SERVICE_DOMAIN_GSM:
			if (ril_data.tokens.registration_state != RIL_TOKEN_NULL && ril_data.tokens.registration_state != RIL_TOKEN_DATA_WAITING) {
				LOGE("Another NetRegist Req is in progress, skipping");
				return;
			}

			memcpy(&(ril_data.state.netinfo), netinfo, sizeof(struct ipc_net_regist_response));

			/* we already told RILJ to get the new data but it wasn't done yet */
			if (ril_tokens_net_get_data_waiting() && ril_data.tokens.registration_state == RIL_TOKEN_DATA_WAITING) {
				LOGD("Updating NetRegist data in background");
			} else {
				ril_tokens_net_set_data_waiting();
#if RIL_VERSION >= 6
				ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
				ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
			}
			break;

		case IPC_NET_SERVICE_DOMAIN_GPRS:
			if (ril_data.tokens.gprs_registration_state != RIL_TOKEN_NULL && ril_data.tokens.gprs_registration_state != RIL_TOKEN_DATA_WAITING) {
				LOGE("Another GPRS NetRegist Req is in progress, skipping");
				return;
			}

			memcpy(&(ril_data.state.gprs_netinfo), netinfo, sizeof(struct ipc_net_regist_response));

			/* we already told RILJ to get the new data but it wasn't done yet */
			if (ril_tokens_net_get_data_waiting() && ril_data.tokens.gprs_registration_state == RIL_TOKEN_DATA_WAITING) {
				LOGD("Updating GPRSNetRegist data in background");
			} else {
				ril_tokens_net_set_data_waiting();
#if RIL_VERSION >= 6
				ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
#else
				ril_request_unsolicited(RIL_UNSOL_RESPONSE_NETWORK_STATE_CHANGED, NULL, 0);
#endif
			}
			break;
		default:
			LOGE("%s: unhandled service domain: %d", __func__, netinfo->domain);
			break;
	}

	ril_tokens_net_state_dump();

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_net_regist_sol(struct ipc_message_info *info)
{
	struct ipc_net_regist_response *netinfo;
	RIL_Token t;

	char *response[4];
	size_t i;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_regist_response))
		goto error;

	netinfo = (struct ipc_net_regist_response *) info->data;
	t = ril_request_get_token(info->aseq);

	LOGD("Got SOL NetRegist message");

	switch(netinfo->domain) {
		case IPC_NET_SERVICE_DOMAIN_GSM:
			if (ril_data.tokens.registration_state != t)
				LOGE("Registration state tokens mismatch");

			/* Better keeping it up to date */
			memcpy(&(ril_data.state.netinfo), netinfo, sizeof(struct ipc_net_regist_response));

			ipc2ril_reg_state_resp(netinfo, response);

			ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

			for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
				if (response[i] != NULL)
					free(response[i]);
			}

			if (ril_data.tokens.registration_state != RIL_TOKEN_DATA_WAITING)
				ril_data.tokens.registration_state = RIL_TOKEN_NULL;
			break;
		case IPC_NET_SERVICE_DOMAIN_GPRS:
			if (ril_data.tokens.gprs_registration_state != t)
				LOGE("GPRS registration state tokens mismatch");

			/* Better keeping it up to date */
			memcpy(&(ril_data.state.gprs_netinfo), netinfo, sizeof(struct ipc_net_regist_response));

			ipc2ril_gprs_reg_state_resp(netinfo, response);

			ril_request_complete(t, RIL_E_SUCCESS, response, sizeof(response));

			for (i = 0; i < sizeof(response) / sizeof(char *) ; i++) {
				if (response[i] != NULL)
					free(response[i]);
			}
			if (ril_data.tokens.registration_state != RIL_TOKEN_DATA_WAITING)
				ril_data.tokens.gprs_registration_state = RIL_TOKEN_NULL;
			break;
		default:
			LOGE("%s: unhandled service domain: %d", __func__, netinfo->domain);
			break;
	}

	ril_tokens_net_state_dump();

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_net_regist(struct ipc_message_info *info)
{
	if (info == NULL)
		return;

	/* Don't consider this if modem isn't in normal power mode. */
	if (ril_data.state.power_state != IPC_PWR_PHONE_STATE_NORMAL)
		return;

	switch(info->type) {
		case IPC_TYPE_NOTI:
			ipc_net_regist_unsol(info);
			break;
		case IPC_TYPE_RESP:
			ipc_net_regist_sol(info);
			break;
		default:
			LOGE("%s: unhandled ipc method: %d", __func__, info->type);
			break;
	}

}

void ril_request_query_available_networks(RIL_Token t)
{
	ipc_fmt_send_get(IPC_NET_PLMN_LIST, ril_request_get_id(t));
}

void ipc_net_plmn_list(struct ipc_message_info *info)
{
	struct ipc_net_plmn_entries *entries_info;
	struct ipc_net_plmn_entry *entries;

	char **response;
	int length;
	int count;

	int index;
	int i;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_plmn_entries))
		goto error;

	entries_info = (struct ipc_net_plmn_entries *) info->data;
	entries = (struct ipc_net_plmn_entry *) (info->data + sizeof(struct ipc_net_plmn_entries));

	LOGD("Listed %d PLMNs\n", entries_info->num);

	length = sizeof(char *) * 4 * entries_info->num;
	response = (char **) calloc(1, length);

	count = 0;
	for (i = 0 ; i < entries_info->num ; i++) {
		// Assumed type for 'emergency only' PLMNs
		if (entries[i].type == 0x01)
			continue;

		index = count * 4;
		ril_plmn_string(entries[i].plmn, &response[index]);

		index = count * 4 + 3;
		switch (entries[i].status) {
			case IPC_NET_PLMN_STATUS_AVAILABLE:
				response[index] = strdup("available");
				break;
			case IPC_NET_PLMN_STATUS_CURRENT:
				response[index] = strdup("current");
				break;
			case IPC_NET_PLMN_STATUS_FORBIDDEN:
				response[index] = strdup("forbidden");
				break;
			default:
				response[index] = strdup("unknown");
				break;
		}

		count++;
	}

	length = sizeof(char *) * 4 * count;
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, response, length);

	for (i = 0 ; i < entries_info->num ; i++)
		if (response[i] != NULL)
			free(response[i]);

	free(response);

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_get_preferred_network_type(RIL_Token t)
{
	ipc_fmt_send_get(IPC_NET_MODE_SEL, ril_request_get_id(t));
}

void ril_request_set_preferred_network_type(RIL_Token t, void *data, size_t length)
{
	int ril_mode;
	struct ipc_net_mode_sel mode_sel;

	if (data == NULL || length < (int) sizeof(int))
		goto error;

	ril_mode = *((int *) data);

	mode_sel.mode_sel = ril2ipc_mode_sel(ril_mode);

	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_NET_MODE_SEL);

	ipc_fmt_send(IPC_NET_MODE_SEL, IPC_TYPE_SET, (unsigned char *) &mode_sel, sizeof(mode_sel), ril_request_get_id(t));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_net_mode_sel(struct ipc_message_info *info)
{
	struct ipc_net_mode_sel *mode_sel;
	int ril_mode;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_mode_sel))
		goto error;

	mode_sel = (struct ipc_net_mode_sel *) info->data;
	ril_mode = ipc2ril_mode_sel(mode_sel->mode_sel);

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &ril_mode, sizeof(ril_mode));

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_query_network_selection_mode(RIL_Token t)
{
	ipc_fmt_send_get(IPC_NET_PLMN_SEL, ril_request_get_id(t));
}

void ipc_net_plmn_sel(struct ipc_message_info *info)
{
	struct ipc_net_plmn_sel_get *plmn_sel;
	int ril_mode;

	if (info->data == NULL || info->length < sizeof(struct ipc_net_plmn_sel_get))
		goto error;

	plmn_sel = (struct ipc_net_plmn_sel_get *) info->data;
	ril_mode = ipc2ril_plmn_sel(plmn_sel->plmn_sel);

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &ril_mode, sizeof(ril_mode));

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_net_plmn_sel_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	int rc;

	phone_res = (struct ipc_gen_phone_res *) info->data;

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		if ((phone_res->code & 0x00ff) == 0x6f) {
			LOGE("Not authorized to register to this network!");
			ril_request_complete(ril_request_get_token(info->aseq), RIL_E_ILLEGAL_SIM_OR_ME, NULL, 0);
		} else {
			LOGE("There was an error during operator selection!");
			ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		}
		return;
	}

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, NULL, 0);
}

void ril_request_set_network_selection_automatic(RIL_Token t)
{
	struct ipc_net_plmn_sel_set plmn_sel;

	ipc_net_plmn_sel_set_setup(&plmn_sel, IPC_NET_PLMN_SEL_AUTO, NULL, IPC_NET_ACCESS_TECHNOLOGY_UNKNOWN);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_NET_PLMN_SEL, ipc_net_plmn_sel_complete);

	ipc_fmt_send(IPC_NET_PLMN_SEL, IPC_TYPE_SET, (unsigned char *) &plmn_sel, sizeof(plmn_sel), ril_request_get_id(t));
}

void ril_request_set_network_selection_manual(RIL_Token t, void *data, size_t length)
{
	struct ipc_net_plmn_sel_set plmn_sel;

	if (data == NULL || length < (int) sizeof(char *))
		return;

	// FIXME: We always assume UMTS capability
	ipc_net_plmn_sel_set_setup(&plmn_sel, IPC_NET_PLMN_SEL_MANUAL, data, IPC_NET_ACCESS_TECHNOLOGY_UMTS);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_NET_PLMN_SEL, ipc_net_plmn_sel_complete);

	ipc_fmt_send(IPC_NET_PLMN_SEL, IPC_TYPE_SET, (unsigned char *) &plmn_sel, sizeof(plmn_sel), ril_request_get_id(t));
}
