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
#include <ctype.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>
#include <telephony/ril.h>

#include <samsung-ril.h>
#include <utils.h>

/*
 * RIL data
 */

struct ril_data *ril_data = NULL;

/*
 * RIL clients
 */

struct ril_client *ril_clients[] = {
	&ipc_fmt_client,
	&ipc_rfs_client,
	&srs_client,
};

unsigned int ril_clients_count = sizeof(ril_clients) /
	sizeof(struct ril_client *);

struct ipc_dispatch_handler ipc_fmt_dispatch_handlers[] = {
	/* Power */
	{
		.command = IPC_PWR_PHONE_PWR_UP,
		.handler = ipc_pwr_phone_pwr_up,
	},
	{
		.command = IPC_PWR_PHONE_RESET,
		.handler = ipc_pwr_phone_reset,
	},
	{
		.command = IPC_PWR_PHONE_STATE,
		.handler = ipc_pwr_phone_state,
	},
	/* Call */
	{
		.command = IPC_CALL_INCOMING,
		.handler = ipc_call_incoming,
	},
	{
		.command = IPC_CALL_STATUS,
		.handler = ipc_call_status,
	},
	{
		.command = IPC_CALL_LIST,
		.handler = ipc_call_list,
	},
	{
		.command = IPC_CALL_BURST_DTMF,
		.handler = ipc_call_burst_dtmf,
	},
	/* SMS */
	{
		.command = IPC_SMS_SEND_MSG,
		.handler = ipc_sms_send_msg,
	},
	{
		.command = IPC_SMS_INCOMING_MSG,
		.handler = ipc_sms_incoming_msg,
	},
	{
		.command = IPC_SMS_SAVE_MSG,
		.handler = ipc_sms_save_msg,
	},
	{
		.command = IPC_SMS_DEL_MSG,
		.handler = ipc_sms_del_msg,
	},
	{
		.command = IPC_SMS_DELIVER_REPORT,
		.handler = ipc_sms_deliver_report,
	},
	{
		.command = IPC_SMS_SVC_CENTER_ADDR,
		.handler = ipc_sms_svc_center_addr,
	},
	/* SIM */
	{
		.command = IPC_SEC_PIN_STATUS,
		.handler = ipc_sec_pin_status,
	},
	{
		.command = IPC_SEC_PHONE_LOCK,
		.handler = ipc_sec_phone_lock,
	},
	{
		.command = IPC_SEC_RSIM_ACCESS,
		.handler = ipc_sec_rsim_access,
	},
	{
		.command = IPC_SEC_SIM_ICC_TYPE,
		.handler = ipc_sec_sim_icc_type,
	},
	{
		.command = IPC_SEC_LOCK_INFOMATION,
		.handler = ipc_sec_lock_infomation,
	},
	/* Network */
	{
		.command = IPC_DISP_ICON_INFO,
		.handler = ipc_disp_icon_info,
	},
	{
		.command = IPC_DISP_RSSI_INFO,
		.handler = ipc_disp_rssi_info,
	},
	{
		.command = IPC_NET_PLMN_SEL,
		.handler = ipc_net_plmn_sel,
	},
	{
		.command = IPC_NET_SERVING_NETWORK,
		.handler = ipc_net_serving_network,
	},
	{
		.command = IPC_NET_PLMN_LIST,
		.handler = ipc_net_plmn_list,
	},
	{
		.command = IPC_NET_REGIST,
		.handler = ipc_net_regist,
	},
	{
		.command = IPC_NET_MODE_SEL,
		.handler = ipc_net_mode_sel,
	},
	/* Misc */
	{
		.command = IPC_MISC_ME_VERSION,
		.handler = ipc_misc_me_version,
	},
	{
		.command = IPC_MISC_ME_IMSI,
		.handler = ipc_misc_me_imsi,
	},
	{
		.command = IPC_MISC_ME_SN,
		.handler = ipc_misc_me_sn,
	},
	{
		.command = IPC_MISC_TIME_INFO,
		.handler = ipc_misc_time_info,
	},
	/* SS */
	{
		.command = IPC_SS_USSD,
		.handler = ipc_ss_ussd,
	},
	/* OEM */
	{
		.command = IPC_SVC_DISPLAY_SCREEN,
		.handler = ipc_svc_display_screen,
	},
	/* Data */
	{
		.command = IPC_GPRS_PS,
		.handler = ipc_gprs_ps,
	},
	{
		.command = IPC_GPRS_PDP_CONTEXT,
		.handler = ipc_gprs_pdp_context,
	},
	{
		.command = IPC_GPRS_IP_CONFIGURATION,
		.handler = ipc_gprs_ip_configuration,
	},
	{
		.command = IPC_GPRS_HSDPA_STATUS,
		.handler = ipc_gprs_hsdpa_status,
	},
	{
		.command = IPC_GPRS_CALL_STATUS,
		.handler = ipc_gprs_call_status,
	},
	/* GEN */
	{
		.command = IPC_GEN_PHONE_RES,
		.handler = ipc_gen_phone_res,
	},
};

unsigned int ipc_fmt_dispatch_handlers_count = sizeof(ipc_fmt_dispatch_handlers) /
	sizeof(struct ipc_dispatch_handler);

struct ipc_dispatch_handler ipc_rfs_dispatch_handlers[] = {
	{
		.command = IPC_RFS_NV_READ_ITEM,
		.handler = ipc_rfs_nv_read_item,
	},
	{
		.command = IPC_RFS_NV_WRITE_ITEM,
		.handler = ipc_rfs_nv_write_item,
	},
};

unsigned int ipc_rfs_dispatch_handlers_count = sizeof(ipc_rfs_dispatch_handlers) /
	sizeof(struct ipc_dispatch_handler);

struct srs_dispatch_handler srs_dispatch_handlers[] = {
	{
		.command = SRS_CONTROL_PING,
		.handler = srs_control_ping,
	},
	{
		.command = SRS_SND_SET_CALL_VOLUME,
		.handler = srs_snd_set_call_volume,
	},
	{
		.command = SRS_SND_SET_CALL_AUDIO_PATH,
		.handler = srs_snd_set_call_audio_path,
	},
	{
		.command = SRS_SND_SET_CALL_CLOCK_SYNC,
		.handler = srs_snd_set_call_clock_sync,
	},
	{
		.command = SRS_SND_SET_MIC_MUTE,
		.handler = srs_snd_set_mic_mute,
	},
	{
		.command = SRS_TEST_SET_RADIO_STATE,
		.handler = srs_test_set_radio_state,
	},
};

unsigned int srs_dispatch_handlers_count = sizeof(srs_dispatch_handlers) /
	sizeof(struct srs_dispatch_handler);

/*
 * RIL request
 */

struct ril_request_handler ril_request_handlers[] = {
	/* Power */
	{
		.request = RIL_REQUEST_RADIO_POWER,
		.handler = ril_request_radio_power,
	},
	/* Call */
	{
		.request = RIL_REQUEST_DIAL,
		.handler = ril_request_dial,
	},
	{
		.request = RIL_REQUEST_HANGUP,
		.handler = ril_request_hangup,
	},
	{
		.request = RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
		.handler = ril_request_hangup,
	},
	{
		.request = RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
		.handler = ril_request_hangup,
	},
	{
		.request = RIL_REQUEST_ANSWER,
		.handler = ril_request_answer,
	},
	{
		.request = RIL_REQUEST_LAST_CALL_FAIL_CAUSE,
		.handler = ril_request_last_call_fail_cause,
	},
	{
		.request = RIL_REQUEST_GET_CURRENT_CALLS,
		.handler = ril_request_get_current_calls,
	},
	{
		.request = RIL_REQUEST_DTMF,
		.handler = ril_request_dtmf,
	},
	{
		.request = RIL_REQUEST_DTMF_START,
		.handler = ril_request_dtmf_start,
	},
	{
		.request = RIL_REQUEST_DTMF_STOP,
		.handler = ril_request_dtmf_stop,
	},
	/* SMS */
	{
		.request = RIL_REQUEST_SEND_SMS,
		.handler = ril_request_send_sms,
	},
	{
		.request = RIL_REQUEST_SEND_SMS_EXPECT_MORE,
		.handler = ril_request_send_sms,
	},
	{
		.request = RIL_REQUEST_WRITE_SMS_TO_SIM,
		.handler = ril_request_write_sms_to_sim,
	},
	{
		.request = RIL_REQUEST_DELETE_SMS_ON_SIM,
		.handler = ril_request_delete_sms_on_sim,
	},
	{
		.request = RIL_REQUEST_SMS_ACKNOWLEDGE,
		.handler = ril_request_sms_acknowledge,
	},
	/* SIM */
	{
		.request = RIL_REQUEST_GET_SIM_STATUS,
		.handler = ril_request_get_sim_status,
	},
	{
		.request = RIL_REQUEST_QUERY_FACILITY_LOCK,
		.handler = ril_request_query_facility_lock,
	},
	{
		.request = RIL_REQUEST_SET_FACILITY_LOCK,
		.handler = ril_request_set_facility_lock,
	},
	{
		.request = RIL_REQUEST_ENTER_SIM_PIN,
		.handler = ril_request_enter_sim_pin,
	},
	{
		.request = RIL_REQUEST_ENTER_SIM_PUK,
		.handler = ril_request_enter_sim_puk,
	},
	{
		.request = RIL_REQUEST_ENTER_SIM_PIN2,
		.handler = ril_request_enter_sim_pin2,
	},
	{
		.request = RIL_REQUEST_ENTER_SIM_PUK2,
		.handler = ril_request_enter_sim_puk2,
	},
	{
		.request = RIL_REQUEST_CHANGE_SIM_PIN,
		.handler = ril_request_change_sim_pin,
	},
	{
		.request = RIL_REQUEST_CHANGE_SIM_PIN2,
		.handler = ril_request_change_sim_pin2,
	},
	{
		.request = RIL_REQUEST_SIM_IO,
		.handler = ril_request_sim_io,
	},
	/* Network */
	{
		.request = RIL_REQUEST_SIGNAL_STRENGTH,
		.handler = ril_request_signal_strength,
	},
	{
		.request = RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE,
		.handler = ril_request_query_network_selection_mode,
	},
	{
		.request = RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC,
		.handler = ril_request_set_network_selection_automatic,
	},
	{
		.request = RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL,
		.handler = ril_request_set_network_selection_manual,
	},
	{
		.request = RIL_REQUEST_OPERATOR,
		.handler = ril_request_operator,
	},
	{
		.request = RIL_REQUEST_QUERY_AVAILABLE_NETWORKS,
		.handler = ril_request_query_available_networks,
	},
	{
#if RIL_VERSION >= 6
		.request = RIL_REQUEST_VOICE_REGISTRATION_STATE,
		.handler = ril_request_voice_registration_state,
#else
		.request = RIL_REQUEST_REGISTRATION_STATE,
		.handler = ril_request_registration_state,
#endif
	},
	{
#if RIL_VERSION >= 6
		.request = RIL_REQUEST_DATA_REGISTRATION_STATE,
		.handler = ril_request_data_registration_state,
#else
		.request = RIL_REQUEST_GPRS_REGISTRATION_STATE,
		.handler = ril_request_gprs_registration_state,
#endif
	},
	{
		.request = RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,
		.handler = ril_request_get_preferred_network_type,
	},
	{
		.request = RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,
		.handler = ril_request_set_preferred_network_type,
	},
	/* Sound */
	{
		.request = RIL_REQUEST_SET_MUTE,
		.handler = ril_request_set_mute,
	},
	/* Misc */
	{
		.request = RIL_REQUEST_BASEBAND_VERSION,
		.handler = ril_request_baseband_version,
	},
	{
		.request = RIL_REQUEST_GET_IMSI,
		.handler = ril_request_get_imsi,
	},
	{
		.request = RIL_REQUEST_GET_IMEI,
		.handler = ril_request_get_imei,
	},
	{
		.request = RIL_REQUEST_GET_IMEISV,
		.handler = ril_request_get_imeisv,
	},
	{
		.request = RIL_REQUEST_SCREEN_STATE,
		.handler = ril_request_screen_state,
	},
	/* SS */
	{
		.request = RIL_REQUEST_SEND_USSD,
		.handler = ril_request_send_ussd,
	},
	{
		.request = RIL_REQUEST_CANCEL_USSD,
		.handler = ril_request_cancel_ussd,
	},
	/* OEM */
	{
		.request = RIL_REQUEST_OEM_HOOK_RAW,
		.handler = ril_request_oem_hook_raw,
	},
	/* Data */
	{
		.request = RIL_REQUEST_SETUP_DATA_CALL,
		.handler = ril_request_setup_data_call,
	},
	{
		.request = RIL_REQUEST_DEACTIVATE_DATA_CALL,
		.handler = ril_request_deactivate_data_call,
	},
	{
		.request = RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE,
		.handler = ril_request_last_data_call_fail_cause,
	},
	{
		.request = RIL_REQUEST_DATA_CALL_LIST,
		.handler = ril_request_data_call_list,
	},
};

unsigned int ril_request_handlers_count = sizeof(ril_request_handlers) /
	sizeof(struct ril_request_handler);

int ril_request_stats_log(void)
{
	struct ril_request *request;
	struct list_head *list;
	unsigned int pending = 0;
	unsigned int handled = 0;
	unsigned int unhandled = 0;
	unsigned int count = 0;

	if (ril_data == NULL)
		return -1;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ril_request *) list->data;

		switch (request->status) {
			case RIL_REQUEST_PENDING:
				pending++;
				break;
			case RIL_REQUEST_HANDLED:
				handled++;
				break;
			case RIL_REQUEST_UNHANDLED:
				unhandled++;
				break;
		}

		count++;

list_continue:
		list = list->next;
	}

	RIL_LOGD("%d RIL request%s in the queue (%d pending, %d handled, %d unhandled)", count, count > 1 ? "s" : "", pending, handled, unhandled);

	count = 0;

	list = ril_data->requests_data;
	while (list != NULL) {
		count++;

		list = list->next;
	}

	if (count > 0)
		RIL_LOGD("%d RIL request%s data in the queue", count, count > 1 ? "s" : "");

	RIL_REQUEST_UNLOCK();

	return 0;
}

int ril_request_register(int request, void *data, size_t size, RIL_Token token)
{
	struct ril_request *ril_request;
	struct list_head *list_end;
	struct list_head *list;
	unsigned int i;

	if (ril_data == NULL)
		return -1;

	RIL_REQUEST_LOCK();

	ril_request = (struct ril_request *) calloc(1, sizeof(struct ril_request));
	ril_request->request = request;
	ril_request->data = NULL;
	ril_request->size = size;
	ril_request->token = token;
	ril_request->status = RIL_REQUEST_PENDING;

	if (size > 0) {
		ril_request->data = calloc(1, size);
		memcpy(ril_request->data, data, size);
	}

	list_end = ril_data->requests;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) ril_request);

	if (ril_data->requests == NULL)
		ril_data->requests = list;

	for (i = 0; i < ril_clients_count; i++) {
		if (ril_clients[i] == NULL)
			continue;

		ril_client_request_register(ril_clients[i], request, token);
	}

	RIL_REQUEST_UNLOCK();

	return 0;
}

int ril_request_unregister(struct ril_request *request)
{
	struct list_head *list;
	unsigned int i;

	if (request == NULL || ril_data == NULL)
		return -1;

	RIL_REQUEST_LOCK();

	for (i = 0; i < ril_clients_count; i++) {
		if (ril_clients[i] == NULL)
			continue;

		ril_client_request_unregister(ril_clients[i], request->request, request->token);
	}

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == (void *) request) {
			if (request->data != NULL && request->size > 0)
				free(request->data);

			memset(request, 0, sizeof(struct ril_request));
			free(request);

			if (list == ril_data->requests)
				ril_data->requests = list->next;

			list_head_free(list);

			break;
		}

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return 0;
}

int ril_request_flush(void)
{
	struct ril_request *request;
	struct list_head *list;
	struct list_head *list_next;

	if (ril_data == NULL)
		return -1;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data != NULL) {
			request = (struct ril_request *) list->data;

			if (request->data != NULL && request->size > 0)
				free(request->data);

			memset(request, 0, sizeof(struct ril_request));
			free(request);
		}

		if (list == ril_data->requests)
			ril_data->requests = list->next;

		list_next = list->next;

		list_head_free(list);

list_continue:
		list = list_next;
	}

	RIL_REQUEST_UNLOCK();

	return 0;
}

struct ril_request *ril_request_find(void)
{
	struct ril_request *request;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ril_request *) list->data;

		RIL_REQUEST_UNLOCK();
		return request;

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return NULL;
}

struct ril_request *ril_request_find_request_status(int request, int status)
{
	struct ril_request *ril_request;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		ril_request = (struct ril_request *) list->data;

		if (ril_request->request == request && ril_request->status == status) {
			RIL_REQUEST_UNLOCK();
			return ril_request;
		}

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return NULL;
}

struct ril_request *ril_request_find_request(int request)
{
	struct ril_request *ril_request;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		ril_request = (struct ril_request *) list->data;

		if (ril_request->request == request) {
			RIL_REQUEST_UNLOCK();
			return ril_request;
		}

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return NULL;
}

struct ril_request *ril_request_find_token(RIL_Token token)
{
	struct ril_request *request;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ril_request *) list->data;

		if (request->token == token) {
			RIL_REQUEST_UNLOCK();
			return request;
		}

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return NULL;
}

struct ril_request *ril_request_find_status(int status)
{
	struct ril_request *request;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	RIL_REQUEST_LOCK();

	list = ril_data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ril_request *) list->data;

		if (request->status == status) {
			RIL_REQUEST_UNLOCK();
			return request;
		}

list_continue:
		list = list->next;
	}

	RIL_REQUEST_UNLOCK();

	return NULL;
}

int ril_request_complete(RIL_Token token, RIL_Errno error, void *data,
	size_t size)
{
	struct ril_request *request;

	if (ril_data == NULL || ril_data->env == NULL || ril_data->env->OnRequestComplete == NULL)
		return -1;

	if (token == NULL)
		return 0;

	request = ril_request_find_token(token);
	if (request == NULL)
		goto complete;

	ril_request_unregister(request);

	ril_request_stats_log();

complete:
	ril_data->env->OnRequestComplete(token, error, data, size);

	RIL_REQUEST_LOOP_UNLOCK();

	return 0;
}

int ril_request_unsolicited(int request, void *data, size_t size)
{
	if (ril_data == NULL || ril_data->env == NULL || ril_data->env->OnUnsolicitedResponse == NULL)
		return -1;

	ril_data->env->OnUnsolicitedResponse(request, data, size);

	return 0;
}

int ril_request_timed_callback(RIL_TimedCallback callback, void *data,
	const struct timeval *time)
{
	if (ril_data == NULL || ril_data->env == NULL || ril_data->env->RequestTimedCallback == NULL)
		return -1;

	ril_data->env->RequestTimedCallback(callback, data, time);

	return 0;
}

int ril_request_dispatch(struct ril_request *request)
{
	unsigned int i;
	int status;
	int rc;

	if (request == NULL || ril_data == NULL)
		return -1;

	for (i = 0; i < ril_request_handlers_count; i++) {
		if (ril_request_handlers[i].handler == NULL)
			continue;

		if (ril_request_handlers[i].request == request->request) {
			status = ril_request_handlers[i].handler(request->data, request->size, request->token);
			switch (status) {
				case RIL_REQUEST_PENDING:
				case RIL_REQUEST_HANDLED:
				case RIL_REQUEST_UNHANDLED:
					request->status = status;
					break;
				case RIL_REQUEST_COMPLETED:
					break;
				default:
					RIL_LOGE("Handling RIL request %d failed", request->request);
					return -1;
			}

			return 0;
		}
	}

	RIL_LOGD("Unhandled RIL request: %d", request->request);
	ril_request_complete(request->token, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);

	return 0;
}

void *ril_request_loop(void *data)
{
	struct ril_request *request;
	int rc;

	if (ril_data == NULL)
		return NULL;

	while (1) {
		RIL_REQUEST_LOOP_LOCK();

		RIL_LOCK();

		rc = ril_radio_state_check(RADIO_STATE_OFF);
		if (rc < 0) {
			RIL_UNLOCK();
			continue;
		}

		do {
			request = ril_request_find_status(RIL_REQUEST_UNHANDLED);
			if (request == NULL)
				break;

			request->status = RIL_REQUEST_PENDING;
		} while (request != NULL);

		do {
			request = ril_request_find_status(RIL_REQUEST_PENDING);
			if (request == NULL)
				break;

			rc = ril_request_dispatch(request);
			if (rc < 0)
				ril_request_unregister(request);
		} while (request != NULL);

		RIL_UNLOCK();
	}

	return NULL;
}

/*
 * RIL request data
 */

int ril_request_data_register(int request, void *data, size_t size)
{
	struct ril_request_data *request_data;
	struct list_head *list_end;
	struct list_head *list;
	unsigned int i;

	if (data == NULL || ril_data == NULL)
		return -1;

	request_data = (struct ril_request_data *) calloc(1, sizeof(struct ril_request_data));
	request_data->request = request;
	request_data->data = data;
	request_data->size = size;

	list_end = ril_data->requests_data;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) request_data);

	if (ril_data->requests_data == NULL)
		ril_data->requests_data = list;

	return 0;
}

int ril_request_data_unregister(struct ril_request_data *request_data)
{
	struct list_head *list;
	unsigned int i;

	if (request_data == NULL || ril_data == NULL)
		return -1;

	list = ril_data->requests_data;
	while (list != NULL) {
		if (list->data == (void *) request_data) {
			memset(request_data, 0, sizeof(struct ril_request_data));
			free(request_data);

			if (list == ril_data->requests_data)
				ril_data->requests_data = list->next;

			list_head_free(list);

			break;
		}

list_continue:
		list = list->next;
	}

	return 0;
}

int ril_request_data_flush(void)
{
	struct ril_request_data *request_data;
	struct list_head *list;
	struct list_head *list_next;

	if (ril_data == NULL)
		return -1;

	list = ril_data->requests_data;
	while (list != NULL) {
		if (list->data != NULL) {
			request_data = (struct ril_request_data *) list->data;

			if (request_data->data != NULL && request_data->size > 0)
				free(request_data->data);

			memset(request_data, 0, sizeof(struct ril_request_data));
			free(request_data);
		}

		if (list == ril_data->requests_data)
			ril_data->requests_data = list->next;

		list_next = list->next;

		list_head_free(list);

list_continue:
		list = list_next;
	}

	return 0;
}

struct ril_request_data *ril_request_data_find_request(int request)
{
	struct ril_request_data *request_data;
	struct list_head *list;

	if (ril_data == NULL)
		return NULL;

	list = ril_data->requests_data;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request_data = (struct ril_request_data *) list->data;

		if (request_data->request == request)
			return request_data;

list_continue:
		list = list->next;
	}

	return NULL;
}

int ril_request_data_free(int request)
{
	struct ril_request_data *request_data;

	do {
		request_data = ril_request_data_find_request(request);
		if (request_data == NULL)
			break;

		if (request_data->data != NULL && request_data->size > 0)
			free(request_data->data);

		ril_request_data_unregister(request_data);
	} while (request_data != NULL);

	return 0;
}

int ril_request_data_set(int request, void *data, size_t size)
{
	void *buffer;
	int rc;

	if (data == NULL || size == 0)
		return -1;

	buffer = calloc(1, size);
	memcpy(buffer, data, size);

	rc = ril_request_data_register(request, buffer, size);
	if (rc < 0)
		return -1;

	return 0;
}

int ril_request_data_set_uniq(int request, void *data, size_t size)
{
	int rc;

	ril_request_data_free(request);

	rc = ril_request_data_set(request, data, size);
	if (rc < 0)
		return -1;

	return 0;
}

size_t ril_request_data_size_get(int request)
{
	struct ril_request_data *request_data;

	request_data = ril_request_data_find_request(request);
	if (request_data == NULL)
		return 0;

	return request_data->size;
}

void *ril_request_data_get(int request)
{
	struct ril_request_data *request_data;
	void *buffer;

	request_data = ril_request_data_find_request(request);
	if (request_data == NULL)
		return NULL;

	buffer = request_data->data;

	ril_request_data_unregister(request_data);

	return buffer;
}

/*
 * RIL radio state
 */

int ril_radio_state_update(RIL_RadioState radio_state)
{
	struct ril_request *request;
	unsigned int i;
	int rc;

	if (ril_data == NULL)
		return -1;

	if (ril_data->radio_state == radio_state)
		return 0;

	RIL_LOGD("Updating RIL radio state to %d", radio_state);

	ril_data->radio_state = radio_state;
	ril_request_unsolicited(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, NULL, 0);

	switch (ril_data->radio_state) {
		case RADIO_STATE_UNAVAILABLE:
			do {
				request = ril_request_find();
				if (request == NULL)
					break;

				ril_request_complete(request->token, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);

				ril_request_unregister(request);
			} while (request != NULL);

			ril_request_flush();
			ril_request_data_flush();

			ril_request_stats_log();

			for (i = 0; i < ril_clients_count; i++) {
				if (ril_clients[i] == NULL)
					continue;

				ril_client_flush(ril_clients[i]);
			}
		case RADIO_STATE_OFF:
			ril_data_connection_flush();

			ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
			ril_request_unsolicited(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0);
			ril_request_unsolicited(RIL_UNSOL_DATA_CALL_LIST_CHANGED, NULL, 0);
			ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
			break;
		default:
			RIL_REQUEST_LOOP_UNLOCK();
			break;
	}

	return 0;
}

int ril_radio_state_check(RIL_RadioState radio_state)
{
	RIL_RadioState radio_states[] = {
		RADIO_STATE_UNAVAILABLE,
		RADIO_STATE_OFF,
		RADIO_STATE_ON,
		RADIO_STATE_NV_NOT_READY,
		RADIO_STATE_NV_READY,
		RADIO_STATE_SIM_NOT_READY,
		RADIO_STATE_SIM_LOCKED_OR_ABSENT,
		RADIO_STATE_SIM_READY,
	};
	unsigned int index;
	unsigned int count;
	unsigned int i;

	if (ril_data == NULL)
		return -1;

	count = sizeof(radio_states) / sizeof(RIL_RadioState);

	for (i = 0; i < count; i++)
		if (radio_states[i] == radio_state)
			break;

	index = i;

	for (i = 0; i < count; i++)
		if (radio_states[i] == ril_data->radio_state)
			break;

	if (i < index)
		return -1;

	return 0;
}

/*
 * RIL data
 */

int ril_data_create(void)
{
	ril_data = (struct ril_data *) calloc(1, sizeof(struct ril_data));

	pthread_mutex_init(&ril_data->mutex, NULL);
	pthread_mutex_init(&ril_data->request_mutex, NULL);
	pthread_mutex_init(&ril_data->request_loop_mutex, NULL);

	RIL_REQUEST_LOOP_LOCK();

	ril_data->radio_state = RADIO_STATE_UNAVAILABLE;

	return 0;
}

int ril_data_destroy(void)
{
	if (ril_data == NULL)
		return -1;

	pthread_mutex_destroy(&ril_data->mutex);
	pthread_mutex_destroy(&ril_data->request_mutex);
	pthread_mutex_destroy(&ril_data->request_loop_mutex);

	free(ril_data);
	ril_data = NULL;

	return 0;
}

/*
 * RIL interface
 */

void ril_on_request(int request, void *data, size_t size, RIL_Token token)
{
	struct ril_request *ril_request;
	void *buffer = NULL;
	unsigned int strings_count = 0;
	unsigned int i;
	char *c;

	RIL_LOCK();

	ril_request = ril_request_find_token(token);
	if (ril_request != NULL)
		ril_request_unregister(ril_request);

	switch (request) {
		case RIL_REQUEST_DIAL:
			if (data == NULL || size < sizeof(RIL_Dial))
				break;

			buffer = calloc(1, size);

			memcpy(buffer, data, size);

			if (((RIL_Dial *) data)->address != NULL)
				((RIL_Dial *) buffer)->address = strdup(((RIL_Dial *) data)->address);

			data = buffer;
			break;
		case RIL_REQUEST_WRITE_SMS_TO_SIM:
			if (data == NULL || size < sizeof(RIL_SMS_WriteArgs))
				break;


			buffer = calloc(1, size);

			memcpy(buffer, data, size);

			if (((RIL_SMS_WriteArgs *) data)->pdu != NULL)
				((RIL_SMS_WriteArgs *) buffer)->pdu = strdup(((RIL_SMS_WriteArgs *) data)->pdu);

			if (((RIL_SMS_WriteArgs *) data)->smsc != NULL)
				((RIL_SMS_WriteArgs *) buffer)->smsc = strdup(((RIL_SMS_WriteArgs *) data)->smsc);

			data = buffer;
			break;
		case RIL_REQUEST_SEND_SMS:
		case RIL_REQUEST_SEND_SMS_EXPECT_MORE:
		case RIL_REQUEST_QUERY_FACILITY_LOCK:
		case RIL_REQUEST_SET_FACILITY_LOCK:
		case RIL_REQUEST_ENTER_SIM_PIN:
		case RIL_REQUEST_ENTER_SIM_PUK:
		case RIL_REQUEST_ENTER_SIM_PIN2:
		case RIL_REQUEST_ENTER_SIM_PUK2:
		case RIL_REQUEST_CHANGE_SIM_PIN:
		case RIL_REQUEST_CHANGE_SIM_PIN2:
			strings_count = size / sizeof(char *);
			break;
		case RIL_REQUEST_SIM_IO:
#if RIL_VERSION >= 6
			if (data == NULL || size < sizeof(RIL_SIM_IO_v6))
#else
			if (data == NULL || size < sizeof(RIL_SIM_IO))
#endif
				break;

			buffer = calloc(1, size);

			memcpy(buffer, data, size);

#if RIL_VERSION >= 6
			if (((RIL_SIM_IO_v6 *) data)->path != NULL)
				((RIL_SIM_IO_v6 *) buffer)->path = strdup(((RIL_SIM_IO_v6 *) data)->path);

			if (((RIL_SIM_IO_v6 *) data)->data != NULL)
				((RIL_SIM_IO_v6 *) buffer)->data = strdup(((RIL_SIM_IO_v6 *) data)->data);

			if (((RIL_SIM_IO_v6 *) data)->pin2 != NULL)
				((RIL_SIM_IO_v6 *) buffer)->pin2 = strdup(((RIL_SIM_IO_v6 *) data)->pin2);

			if (((RIL_SIM_IO_v6 *) data)->aidPtr != NULL)
				((RIL_SIM_IO_v6 *) buffer)->aidPtr = strdup(((RIL_SIM_IO_v6 *) data)->aidPtr);
#else
			if (((RIL_SIM_IO *) data)->path != NULL)
				((RIL_SIM_IO *) buffer)->path = strdup(((RIL_SIM_IO *) data)->path);

			if (((RIL_SIM_IO *) data)->data != NULL)
				((RIL_SIM_IO *) buffer)->data = strdup(((RIL_SIM_IO *) data)->data);

			if (((RIL_SIM_IO *) data)->pin2 != NULL)
				((RIL_SIM_IO *) buffer)->pin2 = strdup(((RIL_SIM_IO *) data)->pin2);
#endif

			data = buffer;
			break;
		case RIL_REQUEST_SETUP_DATA_CALL:
		case RIL_REQUEST_DEACTIVATE_DATA_CALL:
			strings_count = size / sizeof(char *);
			break;
		default:
			if (data == NULL || size != sizeof(char *))
				break;

			c = (char *) data;

			for (i = 0; isprint(c[i]); i++);

			if (i > 0 && c[i] == '\0') {
				size = i + 1;
				RIL_LOGD("Detected string with a size of %d byte%s", size, size > 0 ? "s" : "");
			}

			break;
	}

	if (strings_count > 0 && data != NULL && size >= strings_count * sizeof(char *)) {
		buffer = calloc(1, size);

		for (i = 0; i < strings_count; i++) {
			if (((char **) data)[i] != NULL) {
				c = strdup(((char **) data)[i]);
				((char **) buffer)[i] = c;
			}
		}

		data = buffer;
	}

	ril_request_register(request, data, size, token);

	if (buffer != NULL)
		free(buffer);

	ril_request_stats_log();

	RIL_UNLOCK();

	RIL_REQUEST_LOOP_UNLOCK();
}


RIL_RadioState ril_on_state_request(void)
{
	if (ril_data == NULL)
		return RADIO_STATE_UNAVAILABLE;

	return ril_data->radio_state;
}

int ril_supports(int request)
{
	unsigned int i;

	for (i = 0; i < ril_request_handlers_count; i++) {
		if (ril_request_handlers[i].handler == NULL)
			continue;

		if (ril_request_handlers[i].request == request)
			return 1;
	}

	return 0;
}

void ril_on_cancel(RIL_Token token)
{
	struct ril_request *request;

	RIL_LOCK();

	request = ril_request_find_token(token);
	if (request == NULL) {
		RIL_UNLOCK();
		return;
	}

	ril_request_unregister(request);

	ril_request_stats_log();

	RIL_UNLOCK();
}

const char *ril_get_version(void)
{
	return RIL_VERSION_STRING;
}

RIL_RadioFunctions ril_radio_functions = {
	RIL_VERSION,
	ril_on_request,
	ril_on_state_request,
	ril_supports,
	ril_on_cancel,
	ril_get_version
};

const RIL_RadioFunctions *RIL_Init(const struct RIL_Env *env, int argc,
	char **argv)
{
	RIL_RadioFunctions *radio_functions;
	pthread_attr_t attr;
	int failures;
	unsigned int i;
	int rc;

	if (env == NULL)
		return NULL;

	rc = ril_data_create();
	if (rc < 0) {
		RIL_LOGE("Creating RIL data failed");
		return NULL;
	}

	RIL_LOCK();

	ril_data->env = env;

	for (i = 0; i < ril_clients_count; i++) {
		if (ril_clients[i] == NULL)
			continue;

		rc = ril_client_create(ril_clients[i]);
		if (rc < 0)
			goto error;
	}

	for (i = 0; i < ril_clients_count; i++) {
		if (ril_clients[i] == NULL)
			continue;

		failures = 0;

		do {
			rc = ril_client_open(ril_clients[i]);
			if (rc < 0) {
				failures++;
				usleep(RIL_CLIENT_RETRY_DELAY);
			}
		} while (rc < 0 && failures < RIL_CLIENT_RETRY_COUNT);

		if (rc < 0)
			goto error;

		rc = ril_client_loop(ril_clients[i]);
		if (rc < 0)
			goto error;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	rc = pthread_create(&ril_data->request_thread, &attr, ril_request_loop, NULL);
	if (rc != 0) {
		RIL_LOGE("Starting request loop failed");
		goto error;
	}

	radio_functions = &ril_radio_functions;
	goto complete;

error:
	radio_functions = NULL;

complete:
	RIL_UNLOCK();

	return radio_functions;
}
