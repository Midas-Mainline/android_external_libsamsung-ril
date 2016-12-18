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
#include <utils.h>
#include <sim.h>

#if RIL_VERSION >= 6
RIL_RadioState ipc2ril_sec_pin_status_response(struct ipc_sec_pin_status_response_data *data,
	RIL_CardStatus_v6 *card_status)
#else
RIL_RadioState ipc2ril_sec_pin_status_response(struct ipc_sec_pin_status_response_data *data,
	RIL_CardStatus *card_status)
#endif
{
	RIL_AppStatus app_statuses[] = {
		// Absent
		{ RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN, RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		// Not ready
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED, RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		// Ready
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_READY, RIL_PERSOSUBSTATE_READY, NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		// PIN lock
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PIN, RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		// PUK lock
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		// PUK locked
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN, NULL, NULL, 0, RIL_PINSTATE_ENABLED_PERM_BLOCKED, RIL_PINSTATE_UNKNOWN },
		// Perso network
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK, NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		// Perso network subset
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET, NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		// Perso corporate
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_CORPORATE, NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		// Perso service provider
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER, NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
	};
	RIL_RadioState radio_state;
	unsigned int index;
	unsigned int count;
	unsigned int i;

	if (data == NULL || card_status == NULL)
		return 0;

	count = sizeof(app_statuses) / sizeof(RIL_AppStatus);

	switch (data->status) {
		case IPC_SEC_PIN_STATUS_LOCK_SC:
			switch (data->facility_lock) {
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_UNLOCKED:
					index = 2;
					break;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ:
					index = 3;
					break;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ:
					index = 4;
					break;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED:
					index = 5;
					break;
				default:
					index = 0;
					break;
			}
			break;
		case IPC_SEC_PIN_STATUS_LOCK_FD:
			index = 0;
			break;
		case IPC_SEC_PIN_STATUS_LOCK_PN:
			index = 6;
			break;
		case IPC_SEC_PIN_STATUS_LOCK_PU:
			index = 7;
			break;
		case IPC_SEC_PIN_STATUS_LOCK_PP:
			index = 9;
			break;
		case IPC_SEC_PIN_STATUS_LOCK_PC:
			index = 8;
			break;
		case IPC_SEC_PIN_STATUS_READY:
		case IPC_SEC_PIN_STATUS_INIT_COMPLETE:
		case IPC_SEC_PIN_STATUS_PB_INIT_COMPLETE:
			index = 2;
			break;
		case IPC_SEC_PIN_STATUS_SIM_LOCK_REQUIRED:
		case IPC_SEC_PIN_STATUS_INSIDE_PF_ERROR:
		case IPC_SEC_PIN_STATUS_CARD_NOT_PRESENT:
		case IPC_SEC_PIN_STATUS_CARD_ERROR:
		default:
			index = 0;
			break;
	}

	switch (index) {
		case 1:
			radio_state = RADIO_STATE_SIM_NOT_READY;
			break;
		case 2:
			radio_state = RADIO_STATE_SIM_READY;
			break;
		default:
			radio_state = RADIO_STATE_SIM_LOCKED_OR_ABSENT;
	}

#if RIL_VERSION >= 6
	memset(card_status, 0, sizeof(RIL_CardStatus_v6));
#else
	memset(card_status, 0, sizeof(RIL_CardStatus));
#endif

	if (index == 0)
		card_status->card_state = RIL_CARDSTATE_ABSENT;
	else
		card_status->card_state = RIL_CARDSTATE_PRESENT;

	card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;

	card_status->cdma_subscription_app_index = -1;
#if RIL_VERSION >= 7
	card_status->ims_subscription_app_index = -1;
#endif

	memcpy((void *) &card_status->applications[0], (void *) &app_statuses[index], sizeof(RIL_AppStatus));

	card_status->gsm_umts_subscription_app_index = 0;
	card_status->num_applications = 1;

	RIL_LOGD("%s: Selecting status application %d on %d", __func__, index, count);

	return radio_state;
}

unsigned char ril2ipc_sec_facility_type(char *facility)
{
	if (facility == NULL)
		return 0;

	if (!strcmp(facility, "SC"))
		return IPC_SEC_FACILITY_TYPE_SC;
	else if (!strcmp(facility, "FD"))
		return IPC_SEC_FACILITY_TYPE_FD;
	else if (!strcmp(facility, "PN"))
		return IPC_SEC_FACILITY_TYPE_PN;
	else if (!strcmp(facility, "PU"))
		return IPC_SEC_FACILITY_TYPE_PU;
	else if (!strcmp(facility, "PP"))
		return IPC_SEC_FACILITY_TYPE_PP;
	else if (!strcmp(facility, "PC"))
		return IPC_SEC_FACILITY_TYPE_PC;
	else
		return 0;
}

int ipc_sec_pin_status_callback(struct ipc_message *message)
{
	struct ipc_gen_phone_res_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0) {
		// Return the original SIM status
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
	} else {
		// Destroy the original SIM status
		ril_request_data_free(RIL_REQUEST_GET_SIM_STATUS);
	}

	return 0;
}

int ipc_sec_pin_status(struct ipc_message *message)
{
	struct ipc_sec_pin_status_response_data *data;
	struct ipc_sec_pin_status_request_data request_data;
#if RIL_VERSION >= 6
	RIL_CardStatus_v6 card_status;
#else
	RIL_CardStatus card_status;
#endif
	RIL_RadioState radio_state;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sec_pin_status_response_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	data = (struct ipc_sec_pin_status_response_data *) message->data;

	radio_state = ipc2ril_sec_pin_status_response(data, &card_status);
	if (radio_state == 0)
		return 0;

	if (card_status.applications[0].app_type == RIL_APPTYPE_SIM && card_status.applications[0].app_state == RIL_APPSTATE_PIN && ril_data->sim_pin != NULL) {
		ril_request_data_set_uniq(RIL_REQUEST_GET_SIM_STATUS, (void *) &card_status, sizeof(card_status));

		rc = ipc_sec_pin_status_setup(&request_data, IPC_SEC_PIN_TYPE_PIN1, ril_data->sim_pin, NULL);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_GET_SIM_STATUS);
			return 0;
		}

		rc = ipc_gen_phone_res_expect_callback(message->aseq, IPC_SEC_PIN_STATUS, ipc_sec_pin_status_callback);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_GET_SIM_STATUS);
			return 0;
		}

		rc = ipc_fmt_send(message->aseq, IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_GET_SIM_STATUS);
			return 0;
		}

	}

	ril_radio_state_update(radio_state);

	if (message->type == IPC_TYPE_RESP && ipc_seq_valid(message->aseq)) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &card_status, sizeof(card_status));
	} else {
		ril_request_data_set_uniq(RIL_REQUEST_GET_SIM_STATUS, (void *) &card_status, sizeof(card_status));
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
	}

	return 0;
}

int ril_request_get_sim_status(void *data, size_t size, RIL_Token token)
{
	void *card_status_data;
	size_t card_status_size;
#if RIL_VERSION >= 6
	RIL_CardStatus_v6 *card_status;
#else
	RIL_CardStatus *card_status;
#endif
	struct ril_request *request;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_GET_SIM_STATUS, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	card_status_size = ril_request_data_size_get(RIL_REQUEST_GET_SIM_STATUS);
	card_status_data = ril_request_data_get(RIL_REQUEST_GET_SIM_STATUS);

#if RIL_VERSION >= 6
	if (card_status_data != NULL && card_status_size >= sizeof(RIL_CardStatus_v6)) {
		card_status = (RIL_CardStatus_v6 *) ril_request_data_get(RIL_REQUEST_GET_SIM_STATUS);
#else
	if (card_status_data != NULL && card_status_size >= sizeof(RIL_CardStatus)) {
		card_status = (RIL_CardStatus *) ril_request_data_get(RIL_REQUEST_GET_SIM_STATUS);
#endif
		ril_request_complete(token, RIL_E_SUCCESS, card_status_data, card_status_size);

		free(card_status_data);

		return RIL_REQUEST_COMPLETED;
	} else {
		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_GET, NULL, 0);
		if (rc < 0) {
			ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
			return RIL_REQUEST_COMPLETED;
		}

		return RIL_REQUEST_HANDLED;
	}
}

int ipc_sec_phone_lock(struct ipc_message *message)
{
	struct ipc_sec_phone_lock_response_data *data;
	int active;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sec_phone_lock_response_data))
		return -1;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sec_phone_lock_response_data *) message->data;

	active = !!data->active;

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, &active, sizeof(active));

	return 0;
}

int ril_request_query_facility_lock(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_phone_lock_request_get_data request_data;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 4 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	request_data.facility_type = ril2ipc_sec_facility_type(values[0]);
	if (request_data.facility_type == 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PHONE_LOCK, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc =  RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_sec_callback(struct ipc_message *message)
{
	struct ipc_sec_lock_infomation_request_data request_data;
	struct ipc_gen_phone_res_data *data;
	struct ril_request *request = NULL;
	void *request_complete_data;
	size_t request_complete_size;
	unsigned char facility_type;
	char **values;
	int retry_count;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	request = ril_request_find_token(ipc_fmt_request_token(message->aseq));
	if (request == NULL)
		goto error;

	if (request->request == RIL_REQUEST_ENTER_SIM_PIN || request->request == RIL_REQUEST_CHANGE_SIM_PIN) {
		// Grab the count of remaining tries before completing the request

		ril_request_data_set_uniq(request->request, (void *) data, sizeof(struct ipc_gen_phone_res_data));

		rc = ipc_sec_lock_infomation_setup(&request_data, IPC_SEC_PIN_TYPE_PIN1);
		if (rc < 0) {
			ril_request_data_free(request->request);
			goto error;
		}

		rc = ipc_fmt_send(message->aseq, IPC_SEC_LOCK_INFOMATION, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
		if (rc < 0) {
			ril_request_data_free(request->request);
			goto error;
		}
	} else if (request->request == RIL_REQUEST_ENTER_SIM_PIN2 || request->request == RIL_REQUEST_CHANGE_SIM_PIN2) {
		// Grab the count of remaining tries before completing the request

		ril_request_data_set_uniq(request->request, (void *) data, sizeof(struct ipc_gen_phone_res_data));

		rc = ipc_sec_lock_infomation_setup(&request_data, IPC_SEC_PIN_TYPE_PIN2);
		if (rc < 0) {
			ril_request_data_free(request->request);
			goto error;
		}

		rc = ipc_fmt_send(message->aseq, IPC_SEC_LOCK_INFOMATION, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
		if (rc < 0) {
			ril_request_data_free(request->request);
			goto error;
		}
	} else if (request->request == RIL_REQUEST_SET_FACILITY_LOCK) {
		values = (char **) request->data;

		request_complete_size = ril_request_data_size_get(RIL_REQUEST_SET_FACILITY_LOCK);
		request_complete_data = ril_request_data_get(RIL_REQUEST_SET_FACILITY_LOCK);

		rc = ipc_gen_phone_res_check(data);

		if (request_complete_data != NULL && request_complete_size > 0 && rc >= 0) {
			rc = ipc_gen_phone_res_expect_callback(message->aseq, IPC_SEC_PHONE_LOCK, ipc_sec_callback);
			if (rc < 0) {
				strings_array_free(values, request->size);
				goto error;
			}

			rc = ipc_fmt_send(message->aseq, IPC_SEC_PHONE_LOCK, IPC_TYPE_SET, request_complete_data, request_complete_size);
			if (rc < 0) {
				strings_array_free(values, request->size);
				goto error;
			}
		} else {
			// When FD facility PIN2 unlock failed, ask the count of remaining tries directly

			facility_type = ril2ipc_sec_facility_type(values[0]);

			strings_array_free(values, request->size);

			// Grab the count of remaining tries before completing the request

			ril_request_data_set_uniq(RIL_REQUEST_SET_FACILITY_LOCK, (void *) data, sizeof(struct ipc_gen_phone_res_data));

			if (facility_type == IPC_SEC_FACILITY_TYPE_FD) {
				rc = ipc_sec_lock_infomation_setup(&request_data, IPC_SEC_PIN_TYPE_PIN2);
				if (rc < 0) {
					ril_request_data_free(request->request);
					goto error;
				}
			} else {
				rc = ipc_sec_lock_infomation_setup(&request_data, IPC_SEC_PIN_TYPE_PIN1);
				if (rc < 0) {
					ril_request_data_free(request->request);
					goto error;
				}
			}

			rc = ipc_fmt_send(message->aseq, IPC_SEC_LOCK_INFOMATION, IPC_TYPE_GET, (void *) &request_data, sizeof(request_data));
			if (rc < 0) {
				ril_request_data_free(request->request);
				goto error;
			}
		}
	} else if (request->request == RIL_REQUEST_SIM_IO) {
		request_complete_size = ril_request_data_size_get(RIL_REQUEST_SIM_IO);
		request_complete_data = ril_request_data_get(RIL_REQUEST_SIM_IO);

		rc = ipc_gen_phone_res_check(data);
		if (rc < 0) {
			ril_request_complete(request->token, RIL_E_SIM_PIN2, NULL, 0);
			goto complete;
		}

		if (request_complete_data != NULL && request_complete_size > 0) {
			rc = ipc_fmt_send(message->aseq, IPC_SEC_RSIM_ACCESS, IPC_TYPE_GET, request_complete_data, request_complete_size);
			if (rc < 0)
				goto error;
		} else {
			goto error;
		}
	} else {
		retry_count = -1;

		rc = ipc_gen_phone_res_check(data);
		if (rc < 0) {
			if ((data->code & 0xff) == 0x10) {
				RIL_LOGE("%s: Wrong password", __func__);
				ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_PASSWORD_INCORRECT, &retry_count, sizeof(retry_count));
			} else if ((data->code & 0xff) == 0x0c) {
				RIL_LOGE("%s: Wrong password and no attempts left", __func__);
				retry_count = 0;
				ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_PASSWORD_INCORRECT, &retry_count, sizeof(retry_count));
			} else {
				ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, &retry_count, sizeof(retry_count));
			}
		} else {
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, &retry_count, sizeof(retry_count));
		}
	}

	if (request->request == RIL_REQUEST_ENTER_SIM_PUK || request->request == RIL_REQUEST_ENTER_SIM_PUK2)
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);

	goto complete;

error:
	if (request != NULL)
		ril_request_complete(request->token, RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ril_request_set_facility_lock(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_phone_lock_request_set_data request_data;
	struct ipc_sec_pin_status_request_data pin_request_data;
	struct ril_request *request;
	unsigned char facility_type;
	unsigned char active;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 4 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_SET_FACILITY_LOCK, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	facility_type = ril2ipc_sec_facility_type(values[0]);
	if (facility_type == 0)
		goto error;

	active = values[1][0] == '1';

	rc = ipc_sec_phone_lock_request_set_setup(&request_data, facility_type, active, values[2]);
	if (rc < 0)
		goto error;

	if (facility_type == IPC_SEC_FACILITY_TYPE_FD) {
		// FD facility requires PIN2 unlock first

		rc = ipc_sec_pin_status_setup(&pin_request_data, IPC_SEC_PIN_TYPE_PIN2, values[2], NULL);
		if (rc < 0)
			goto error;

		ril_request_data_set_uniq(RIL_REQUEST_SET_FACILITY_LOCK, &request_data, sizeof(request_data));

		rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_SET_FACILITY_LOCK);
			goto error;
		}

		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &pin_request_data, sizeof(pin_request_data));
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_SET_FACILITY_LOCK);
			goto error;
		}

		rc = RIL_REQUEST_HANDLED;
		goto complete;
	}

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PHONE_LOCK, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PHONE_LOCK, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_enter_sim_pin(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_pin_status_request_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *) || ril_data == NULL)
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_ENTER_SIM_PIN, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;
	if (values[0] == NULL)
		goto error;

	if (ril_data->sim_pin != NULL)
		free(ril_data->sim_pin);

	ril_data->sim_pin = strdup(values[0]);

	rc = ipc_sec_pin_status_setup(&request_data, IPC_SEC_PIN_TYPE_PIN1, values[0], NULL);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_enter_sim_puk(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_pin_status_request_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_ENTER_SIM_PUK, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	rc = ipc_sec_pin_status_setup(&request_data, IPC_SEC_PIN_TYPE_PIN1, values[1], values[0]);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_enter_sim_pin2(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_pin_status_request_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_ENTER_SIM_PIN2, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	rc = ipc_sec_pin_status_setup(&request_data, IPC_SEC_PIN_TYPE_PIN2, values[0], NULL);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_enter_sim_puk2(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_pin_status_request_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_ENTER_SIM_PUK2, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	rc = ipc_sec_pin_status_setup(&request_data, IPC_SEC_PIN_TYPE_PIN2, values[1], values[0]);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_change_sim_pin(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_change_locking_pw_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 3 * sizeof(char *))
		goto error;

	request = ril_request_find_request_status(RIL_REQUEST_CHANGE_SIM_PIN, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	rc = ipc_sec_change_locking_pw_setup(&request_data, IPC_SEC_FACILITY_TYPE_SC, values[0], values[1]);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_CHANGE_LOCKING_PW, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_CHANGE_LOCKING_PW, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_change_sim_pin2(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_change_locking_pw_data request_data;
	struct ril_request *request;
	char **values = NULL;
	int rc;

	if (data == NULL || size < 3 * sizeof(char *))
		goto error;

	request = ril_request_find_request_status(RIL_REQUEST_CHANGE_SIM_PIN, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;

	rc = ipc_sec_change_locking_pw_setup(&request_data, IPC_SEC_FACILITY_TYPE_FD, values[0], values[1]);
	if (rc < 0)
		goto error;

	strings_array_free(values, size);
	values = NULL;

	rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_CHANGE_LOCKING_PW, ipc_sec_callback);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_CHANGE_LOCKING_PW, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_sec_rsim_access(struct ipc_message *message)
{
	struct ipc_sec_rsim_access_response_header *header;
	struct ipc_sec_rsim_access_usim_response_header *usim_header;
	struct sim_file_response sim_file_response;
	struct ril_request *request;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	RIL_SIM_IO_Response response;
#if RIL_VERSION >= 6
	RIL_SIM_IO_v6 *sim_io;
#else
	RIL_SIM_IO *sim_io;
#endif
	unsigned char *p;
	unsigned int offset;
	unsigned int i;
	void *data;
	size_t size;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sec_rsim_access_response_header))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return 0;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	header = (struct ipc_sec_rsim_access_response_header *) message->data;

	size = ipc_sec_rsim_access_size_extract(message->data, message->size);
	data = ipc_sec_rsim_access_extract(message->data, message->size);

	request = ril_request_find_token(ipc_fmt_request_token(message->aseq));
#if RIL_VERSION >= 6
	if (request == NULL || request->data == NULL || request->size < sizeof(RIL_SIM_IO_v6))
#else
	if (request == NULL || request->data == NULL || request->size < sizeof(RIL_SIM_IO))
#endif
		return 0;

#if RIL_VERSION >= 6
	sim_io = (RIL_SIM_IO_v6 *) request->data;
#else
	sim_io = (RIL_SIM_IO *) request->data;
#endif

	memset(&response, 0, sizeof(response));
	response.sw1 = header->sw1;
	response.sw2 = header->sw2;

	switch (sim_io->command) {
		case SIM_COMMAND_READ_BINARY:
		case SIM_COMMAND_READ_RECORD:
			if (header->length == 0)
				break;

			response.simResponse = data2string(data, header->length);
			break;
		case SIM_COMMAND_GET_RESPONSE:
			if (header->length == 0)
				break;

			if (ipc_fmt_data->sim_icc_type_data.type == 0x01) {
				response.simResponse = data2string(data, header->length);
				break;
			}

			if (header->length < sizeof(struct ipc_sec_rsim_access_usim_response_header))
				break;

			usim_header = (struct ipc_sec_rsim_access_usim_response_header *) data;

			memset(&sim_file_response, 0, sizeof(sim_file_response));

			offset = sizeof(struct ipc_sec_rsim_access_usim_response_header) + usim_header->offset;
			if (offset > header->length)
				break;

			offset = usim_header->offset - 2;
			p = (unsigned char *) usim_header + offset;

			sim_file_response.file_id[0] = p[0];
			sim_file_response.file_id[1] = p[1];

			offset = header->length - 2;
			p = (unsigned char *) usim_header;

			while (offset > 2) {
				if (p[offset] == 0x88) {
					offset -= 2;
					break;
				}

				offset--;
			}

			if (offset <= 2)
				break;

			p = (unsigned char *) usim_header + offset;

			sim_file_response.file_size[0] = p[0];
			sim_file_response.file_size[1] = p[1];

			// Fallback to EF
			sim_file_response.file_type = SIM_FILE_TYPE_EF;
			for (i = 0; i < sim_file_ids_count; i++) {
				if (sim_io->fileid == sim_file_ids[i].file_id) {
					sim_file_response.file_type = sim_file_ids[i].type;
					break;
				}
			}

			sim_file_response.access_condition[0] = 0x00;
			sim_file_response.access_condition[1] = 0xff;
			sim_file_response.access_condition[2] = 0xff;

			sim_file_response.file_status = 0x01;
			sim_file_response.file_length = 0x02;

			switch (usim_header->file_structure) {
				case IPC_SEC_RSIM_FILE_STRUCTURE_TRANSPARENT:
					sim_file_response.file_structure = SIM_FILE_STRUCTURE_TRANSPARENT;
					break;
				case IPC_SEC_RSIM_FILE_STRUCTURE_LINEAR_FIXED:
				default:
					sim_file_response.file_structure = SIM_FILE_STRUCTURE_LINEAR_FIXED;
					break;
			}

			sim_file_response.record_length = usim_header->length;

			response.simResponse = data2string((void *) &sim_file_response, sizeof(sim_file_response));
			break;
		case SIM_COMMAND_UPDATE_BINARY:
		case SIM_COMMAND_UPDATE_RECORD:
		case SIM_COMMAND_SEEK:
		default:
			response.simResponse = NULL;
			break;
	}

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) &response, sizeof(response));

	if (response.simResponse != NULL)
		free(response.simResponse);

	return 0;
}

int ril_request_sim_io(void *data, size_t size, RIL_Token token)
{
	struct ipc_sec_rsim_access_request_header request_header;
	struct ipc_sec_pin_status_request_data pin_request_data;
	struct ril_request *request;
#if RIL_VERSION >= 6
	RIL_SIM_IO_v6 *sim_io = NULL;
#else
	RIL_SIM_IO *sim_io = NULL;
#endif
	void *sim_io_data = NULL;
	size_t sim_io_size = 0;
	void *request_data = NULL;
	size_t request_size = 0;
	int pin_request = 0;
	int rc;

#if RIL_VERSION >= 6
	if (data == NULL || size < sizeof(RIL_SIM_IO_v6))
#else
	if (data == NULL || size < sizeof(RIL_SIM_IO))
#endif
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0) {
		rc = ril_radio_state_check(RADIO_STATE_SIM_LOCKED_OR_ABSENT);
		if (rc < 0)
			return RIL_REQUEST_UNHANDLED;
		else
			RIL_LOGE("%s: SIM is locked or absent.", __func__);
	}

	request = ril_request_find_request_status(RIL_REQUEST_SIM_IO, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

#if RIL_VERSION >= 6
	sim_io = (RIL_SIM_IO_v6 *) data;
#else
	sim_io = (RIL_SIM_IO *) data;
#endif

	if (sim_io->data != NULL) {
		sim_io_size = string2data_size(sim_io->data);
		if (sim_io_size == 0)
			goto error;

		sim_io_data = string2data(sim_io->data);
		if (sim_io_data == NULL)
			goto error;
	}

	if (sim_io->pin2 != NULL) {
		// PIN2 unlock first

		pin_request = 1;

		rc = ipc_sec_pin_status_setup(&pin_request_data, IPC_SEC_PIN_TYPE_PIN2, sim_io->pin2, NULL);
		if (rc < 0)
			goto error;
	}

	if (sim_io->path != NULL)
		free(sim_io->path);

	if (sim_io->data != NULL)
		free(sim_io->data);

	if (sim_io->pin2 != NULL)
		free(sim_io->pin2);

#if RIL_VERSION >= 6
	if (sim_io->aidPtr != NULL)
		free(sim_io->aidPtr);
#endif

	memset(&request_header, 0, sizeof(request_header));
	request_header.command = sim_io->command;
	request_header.file_id = sim_io->fileid;
	request_header.p1 = sim_io->p1;
	request_header.p2 = sim_io->p2;
	request_header.p3 = sim_io->p3;

	sim_io = NULL;

	request_size = ipc_sec_rsim_access_size_setup(&request_header, sim_io_data, sim_io_size);
	if (request_size == 0)
		goto error;

	request_data = ipc_sec_rsim_access_setup(&request_header, sim_io_data, sim_io_size);
	if (request_data == NULL)
		goto error;

	if (pin_request) {
		// PIN2 unlock first

		ril_request_data_set_uniq(RIL_REQUEST_SIM_IO, request_data, request_size);

		rc = ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, ipc_sec_callback);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_SIM_IO);
			goto error;
		}

		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_PIN_STATUS, IPC_TYPE_SET, (void *) &pin_request_data, sizeof(pin_request_data));
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_SIM_IO);
			goto error;
		}

		rc = RIL_REQUEST_HANDLED;
		goto complete;
	}

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SEC_RSIM_ACCESS, IPC_TYPE_GET, request_data, request_size);
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (sim_io != NULL) {
		if (sim_io->path != NULL)
			free(sim_io->path);

		if (sim_io->data != NULL)
			free(sim_io->data);

		if (sim_io->pin2 != NULL)
			free(sim_io->pin2);

#if RIL_VERSION >= 6
		if (sim_io->aidPtr != NULL)
			free(sim_io->aidPtr);
#endif
	}

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	if (sim_io_data != NULL && sim_io_size > 0)
		free(sim_io_data);

	if (request_data != NULL && request_size > 0)
		free(request_data);

	return rc;
}

int ipc_sec_sim_icc_type(struct ipc_message *message)
{
	struct ipc_sec_sim_icc_type_data *data;
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sec_sim_icc_type_data))
		return -1;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return 0;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;

	data = (struct ipc_sec_sim_icc_type_data *) message->data;

	if (ipc_fmt_data->sim_icc_type_data.type != data->type)
		ipc_fmt_data->sim_icc_type_data.type = data->type;

	return 0;
}

int ipc_sec_lock_infomation(struct ipc_message *message)
{
	struct ipc_sec_lock_infomation_response_data *data;
	struct ipc_gen_phone_res_data *gen_phone_res;
	int requests[] = { RIL_REQUEST_ENTER_SIM_PIN, RIL_REQUEST_CHANGE_SIM_PIN, RIL_REQUEST_ENTER_SIM_PIN2, RIL_REQUEST_CHANGE_SIM_PIN2, RIL_REQUEST_SET_FACILITY_LOCK };
	void *gen_phone_res_data = NULL;
	size_t gen_phone_res_size = 0;
	int retry_count;
	unsigned int count;
	unsigned int i;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sec_lock_infomation_response_data))
		return -1;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return 0;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sec_lock_infomation_response_data *) message->data;
	if (data->type != IPC_SEC_PIN_TYPE_PIN1 && data->type != IPC_SEC_PIN_TYPE_PIN2)
		return 0;

	count = sizeof(requests) / sizeof(int);

	for (i = 0; i < count; i++) {
		gen_phone_res_size = ril_request_data_size_get(requests[i]);
		if (gen_phone_res_size < sizeof(struct ipc_gen_phone_res_data))
			continue;

		gen_phone_res_data = ril_request_data_get(requests[i]);
		if (gen_phone_res_data == NULL)
			continue;

		break;
	}

	if (gen_phone_res_data == NULL || gen_phone_res_size < sizeof(struct ipc_gen_phone_res_data))
		return 0;

	gen_phone_res = (struct ipc_gen_phone_res_data *) gen_phone_res_data;

	retry_count = data->retry_count;

	rc = ipc_gen_phone_res_check(gen_phone_res);
	if (rc < 0) {
		if ((gen_phone_res->code & 0xff) == 0x10) {
			RIL_LOGE("%s: Wrong password and %d attempts left", __func__, retry_count);
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_PASSWORD_INCORRECT, &retry_count, sizeof(retry_count));
		} else if ((gen_phone_res->code & 0xff) == 0x0c) {
			RIL_LOGE("%s: Wrong password and no attempts left", __func__);
			retry_count = 0;
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_PASSWORD_INCORRECT, &retry_count, sizeof(retry_count));
		} else {
			ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, &retry_count, sizeof(retry_count));
		}
	} else {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, &retry_count, sizeof(retry_count));
	}

	free(gen_phone_res_data);

	return 0;
}
