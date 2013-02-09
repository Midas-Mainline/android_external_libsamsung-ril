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

#define LOG_TAG "RIL-SEC"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

ril_sim_state ipc2ril_sim_state(struct ipc_sec_sim_status_response *pin_status)
{
	switch(pin_status->status) {
		case IPC_SEC_SIM_STATUS_INITIALIZING:
			return SIM_STATE_NOT_READY;
		case IPC_SEC_SIM_STATUS_LOCK_SC:
			switch(pin_status->facility_lock) {
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_UNLOCKED:
					return SIM_STATE_READY;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_PIN1_REQ:
					return SIM_STATE_PIN;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_PUK_REQ:
					return SIM_STATE_PUK;
				case IPC_SEC_FACILITY_LOCK_TYPE_SC_CARD_BLOCKED:
					return SIM_STATE_BLOCKED;
				default:
					LOGE("Unknown SIM facility lock: 0x%x", pin_status->facility_lock);
					return SIM_STATE_ABSENT;
			}
			break;
		case IPC_SEC_SIM_STATUS_LOCK_FD:
			return SIM_STATE_ABSENT;
		case IPC_SEC_SIM_STATUS_LOCK_PN:
			return SIM_STATE_NETWORK_PERSO;
		case IPC_SEC_SIM_STATUS_LOCK_PU:
			return SIM_STATE_NETWORK_SUBSET_PERSO;
		case IPC_SEC_SIM_STATUS_LOCK_PP:
			return SIM_STATE_SERVICE_PROVIDER_PERSO;
		case IPC_SEC_SIM_STATUS_LOCK_PC:
			return SIM_STATE_CORPORATE_PERSO;
		case IPC_SEC_SIM_STATUS_INIT_COMPLETE:
		case IPC_SEC_SIM_STATUS_PB_INIT_COMPLETE:
			return SIM_STATE_READY;
		case IPC_SEC_SIM_STATUS_SIM_LOCK_REQUIRED:
		case IPC_SEC_SIM_STATUS_INSIDE_PF_ERROR:
		case IPC_SEC_SIM_STATUS_CARD_NOT_PRESENT:
		case IPC_SEC_SIM_STATUS_CARD_ERROR:
			return SIM_STATE_ABSENT;
		default:
			LOGE("Unknown SIM status: 0x%x", pin_status->status);
			return SIM_STATE_ABSENT;
	}
}

/**
 * Update the radio state based on SIM state
 *
 * Out: RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
 *   Indicate when value of RIL_RadioState has changed
 *   Callee will invoke RIL_RadioStateRequest method on main thread
 */
void ril_state_update(ril_sim_state sim_state)
{
	RIL_RadioState radio_state;

	/* If power mode isn't at least normal, don't update RIL state */
	if (ril_data.state.power_state != IPC_PWR_PHONE_STATE_NORMAL)
		return;

	ril_data.state.sim_state = sim_state;

	switch(sim_state) {
		case SIM_STATE_READY:
#if RIL_VERSION >= 7
			radio_state = RADIO_STATE_ON;
#else
			radio_state = RADIO_STATE_SIM_READY;
#endif
			break;
		case SIM_STATE_NOT_READY:
			radio_state = RADIO_STATE_SIM_NOT_READY;
			break;
		case SIM_STATE_ABSENT:
		case SIM_STATE_PIN:
		case SIM_STATE_PUK:
		case SIM_STATE_BLOCKED:
		case SIM_STATE_NETWORK_PERSO:
		case SIM_STATE_NETWORK_SUBSET_PERSO:
		case SIM_STATE_CORPORATE_PERSO:
		case SIM_STATE_SERVICE_PROVIDER_PERSO:
			radio_state = RADIO_STATE_SIM_LOCKED_OR_ABSENT;
			break;
		default:
			radio_state = RADIO_STATE_SIM_NOT_READY;
			break;
	}


	ril_data.state.radio_state = radio_state;

	ril_tokens_check();

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED, NULL, 0);
}

#if RIL_VERSION >= 6
void ipc2ril_card_status(struct ipc_sec_sim_status_response *pin_status, RIL_CardStatus_v6 *card_status)
#else
void ipc2ril_card_status(struct ipc_sec_sim_status_response *pin_status, RIL_CardStatus *card_status)
#endif
{
	ril_sim_state sim_state;
	int app_status_array_length;
	int app_index;
	int i;

	static RIL_AppStatus app_status_array[] = {
		/* SIM_ABSENT = 0 */
		{ RIL_APPTYPE_UNKNOWN, RIL_APPSTATE_UNKNOWN, RIL_PERSOSUBSTATE_UNKNOWN,
		NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_NOT_READY = 1 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_DETECTED, RIL_PERSOSUBSTATE_UNKNOWN,
		NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_READY = 2 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_READY, RIL_PERSOSUBSTATE_READY,
		NULL, NULL, 0, RIL_PINSTATE_UNKNOWN, RIL_PINSTATE_UNKNOWN },
		/* SIM_PIN = 3 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PIN, RIL_PERSOSUBSTATE_UNKNOWN,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_PUK = 4 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_BLOCKED = 4 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_PUK, RIL_PERSOSUBSTATE_UNKNOWN,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_PERM_BLOCKED, RIL_PINSTATE_UNKNOWN },
		/* SIM_NETWORK_PERSO = 6 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_NETWORK_SUBSET_PERSO = 7 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_NETWORK_SUBSET,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_CORPORATE_PERSO = 8 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_CORPORATE,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
		/* SIM_SERVICE_PROVIDER_PERSO = 9 */
		{ RIL_APPTYPE_SIM, RIL_APPSTATE_SUBSCRIPTION_PERSO, RIL_PERSOSUBSTATE_SIM_SERVICE_PROVIDER,
		NULL, NULL, 0, RIL_PINSTATE_ENABLED_NOT_VERIFIED, RIL_PINSTATE_UNKNOWN },
	};

	app_status_array_length = sizeof(app_status_array) / sizeof(RIL_AppStatus);

	if (app_status_array_length > RIL_CARD_MAX_APPS)
		app_status_array_length = RIL_CARD_MAX_APPS;

	sim_state = ipc2ril_sim_state(pin_status);

	/* Card is assumed to be present if not explicitly absent */
	if (sim_state == SIM_STATE_ABSENT) {
		card_status->card_state = RIL_CARDSTATE_ABSENT;
	} else {
		card_status->card_state = RIL_CARDSTATE_PRESENT;
	}

	// FIXME: How do we know that?
	card_status->universal_pin_state = RIL_PINSTATE_UNKNOWN;

	// Initialize the apps
	for (i = 0 ; i < app_status_array_length ; i++) {
		memcpy((void *) &(card_status->applications[i]), (void *) &(app_status_array[i]), sizeof(RIL_AppStatus));
	}
	for (i = app_status_array_length ; i < RIL_CARD_MAX_APPS ; i++) {
		memset((void *) &(card_status->applications[i]), 0, sizeof(RIL_AppStatus));
	}

	// sim_state corresponds to the app index on the table
	card_status->gsm_umts_subscription_app_index = (int) sim_state;
	card_status->cdma_subscription_app_index = (int) sim_state;
	card_status->num_applications = app_status_array_length;

	LOGD("Selecting application #%d on %d", (int) sim_state, app_status_array_length);
}

void ril_tokens_pin_status_dump(void)
{
	LOGD("ril_tokens_pin_status_dump:\n\
	\tril_data.tokens.pin_status = %p\n", ril_data.tokens.pin_status);
}

/**
 * In: IPC_SEC_SIM_STATUS
 *   Provides SIM initialization/lock status
 *
 * Out: RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED
 *   Indicates that SIM state changes.
 *   Callee will invoke RIL_REQUEST_GET_SIM_STATUS on main thread
 *
 * Out: RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
 *   Indicate when value of RIL_RadioState has changed
 *   Callee will invoke RIL_RadioStateRequest method on main thread
 */
void ipc_sec_sim_status(struct ipc_message_info *info)
{
	RIL_Token t = ril_request_get_token(info->aseq);
	struct ipc_sec_sim_status_response *pin_status = (struct ipc_sec_sim_status_response *) info->data;
#if RIL_VERSION >= 6
	RIL_CardStatus_v6 card_status;
#else
	RIL_CardStatus card_status;
#endif
	ril_sim_state sim_state;

	switch(info->type) {
		case IPC_TYPE_NOTI:
			// Don't consider this if modem isn't in normal power mode
			if (ril_data.state.power_state != IPC_PWR_PHONE_STATE_NORMAL)
				return;

			LOGD("Got UNSOL PIN status message");

			if (ril_data.tokens.pin_status != (RIL_Token) 0x00 && ril_data.tokens.pin_status != RIL_TOKEN_DATA_WAITING) {
				LOGE("Another PIN status Req is in progress, skipping");
				return;
			}

			sim_state = ipc2ril_sim_state(pin_status);
			ril_state_update(sim_state);

			memcpy(&(ril_data.state.sim_pin_status), pin_status, sizeof(struct ipc_sec_sim_status_response));

			ril_data.tokens.pin_status = RIL_TOKEN_DATA_WAITING;
			ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
			break;
		case IPC_TYPE_RESP:
			LOGD("Got SOL PIN status message");

			if (ril_data.tokens.pin_status != t)
				LOGE("PIN status tokens mismatch");

			sim_state = ipc2ril_sim_state(pin_status);
			ril_state_update(sim_state);

			// Better keeping this up to date
			memcpy(&(ril_data.state.sim_pin_status), pin_status, sizeof(struct ipc_sec_sim_status_response));

			ipc2ril_card_status(pin_status, &card_status);
			ril_request_complete(t, RIL_E_SUCCESS, &card_status, sizeof(card_status));

			if (ril_data.tokens.pin_status != RIL_TOKEN_DATA_WAITING)
				ril_data.tokens.pin_status = (RIL_Token) 0x00;
			break;
		default:
			LOGE("%s: unhandled ipc method: %d", __FUNCTION__, info->type);
			break;
	}

	ril_tokens_pin_status_dump();
}

/**
 * In: RIL_REQUEST_GET_SIM_STATUS
 *   Requests status of the SIM interface and the SIM card
 */
void ril_request_get_sim_status(RIL_Token t)
{
	struct ipc_sec_sim_status_response *pin_status;
#if RIL_VERSION >= 6
	RIL_CardStatus_v6 card_status;
#else
	RIL_CardStatus card_status;
#endif

	if (ril_data.tokens.pin_status == RIL_TOKEN_DATA_WAITING) {
		LOGD("Got RILJ request for UNSOL data");
		hex_dump(&(ril_data.state.sim_pin_status), sizeof(struct ipc_sec_sim_status_response));
		pin_status = &(ril_data.state.sim_pin_status);

		ipc2ril_card_status(pin_status, &card_status);

		ril_request_complete(t, RIL_E_SUCCESS, &card_status, sizeof(card_status));

		ril_data.tokens.pin_status = (RIL_Token) 0x00;
	} else if (ril_data.tokens.pin_status == (RIL_Token) 0x00) {
		LOGD("Got RILJ request for SOL data");

		/* Request data to the modem */
		ril_data.tokens.pin_status = t;

		ipc_fmt_send_get(IPC_SEC_SIM_STATUS, ril_request_get_id(t));
	} else {
		LOGE("Another request is going on, returning UNSOL data");

		pin_status = &(ril_data.state.sim_pin_status);

		ipc2ril_card_status(pin_status, &card_status);
		ril_request_complete(t, RIL_E_SUCCESS, &card_status, sizeof(card_status));
	}

	ril_tokens_pin_status_dump();
}

/*
 * SIM I/O
 */

int ril_request_sim_io_register(RIL_Token t, unsigned char command, unsigned short fileid,
	unsigned char p1, unsigned char p2, unsigned char p3, void *data, int length)
{
	struct ril_request_sim_io_info *sim_io;
	struct list_head *list_end;
	struct list_head *list;

	sim_io = calloc(1, sizeof(struct ril_request_sim_io_info));
	if (sim_io == NULL)
		return -1;

	sim_io->command = command;
	sim_io->fileid = fileid;
	sim_io->p1 = p1;
	sim_io->p2 = p2;
	sim_io->p3 = p3;
	sim_io->data = data;
	sim_io->length = length;
	sim_io->token = t;

	list_end = ril_data.sim_io;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) sim_io, list_end, NULL);

	if (ril_data.sim_io == NULL)
		ril_data.sim_io = list;

	return 0;
}

void ril_request_sim_io_unregister(struct ril_request_sim_io_info *sim_io)
{
	struct list_head *list;

	if (sim_io == NULL)
		return;

	list = ril_data.sim_io;
	while (list != NULL) {
		if (list->data == (void *) sim_io) {
			memset(sim_io, 0, sizeof(struct ril_request_sim_io_info));
			free(sim_io);

			if (list == ril_data.sim_io)
				ril_data.sim_io = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct ril_request_sim_io_info *ril_request_sim_io_info_find(void)
{
	struct ril_request_sim_io_info *sim_io;
	struct list_head *list;

	list = ril_data.sim_io;
	while (list != NULL) {
		sim_io = (struct ril_request_sim_io_info *) list->data;
		if (sim_io == NULL)
			goto list_continue;

		return sim_io;

list_continue:
		list = list->next;
	}

	return NULL;
}

void ril_request_sim_io_info_clear(struct ril_request_sim_io_info *sim_io)
{
	if (sim_io == NULL)
		return;

	if (sim_io->data != NULL)
		free(sim_io->data);
}

void ril_request_sim_io_next(void)
{
	struct ril_request_sim_io_info *sim_io;
	unsigned char command;
	unsigned short fileid;
	unsigned char p1;
	unsigned char p2;
	unsigned char p3;
	void *data;
	int length;
	RIL_Token t;
	int rc;

	ril_data.tokens.sim_io = (RIL_Token) 0x00;

	sim_io = ril_request_sim_io_info_find();
	if (sim_io == NULL)
		return;

	command = sim_io->command;
	fileid = sim_io->fileid;
	p1 = sim_io->p1;
	p2 = sim_io->p2;
	p3 = sim_io->p3;
	data = sim_io->data;
	length = sim_io->length;
	t = sim_io->token;

	ril_request_sim_io_unregister(sim_io);

	ril_data.tokens.sim_io = t;

	ril_request_sim_io_complete(t, command, fileid, p1, p2, p3, data, length);
	if (data != NULL)
		free(data);
}

void ril_request_sim_io_complete(RIL_Token t, unsigned char command, unsigned short fileid,
	unsigned char p1, unsigned char p2, unsigned char p3, void *data, int length)
{
	struct ipc_sec_rsim_access_get *rsim_access = NULL;
	void *rsim_access_data = NULL;
	int rsim_access_length = 0;

	rsim_access_length += sizeof(struct ipc_sec_rsim_access_get);

	if (data != NULL && length > 0)
		rsim_access_length += length;

	rsim_access_data = calloc(1, rsim_access_length);
	rsim_access = (struct ipc_sec_rsim_access_get *) rsim_access_data;

	rsim_access->command = command;
	rsim_access->fileid = fileid;
	rsim_access->p1 = p1;
	rsim_access->p2 = p2;
	rsim_access->p3 = p3;

	if (data != NULL && length > 0)
		memcpy((void *) ((int) rsim_access_data + sizeof(struct ipc_sec_rsim_access_get)), data, length);

	ipc_fmt_send(IPC_SEC_RSIM_ACCESS, IPC_TYPE_GET, rsim_access_data, rsim_access_length, ril_request_get_id(t));

	free(rsim_access_data);
}

/**
 * In: RIL_REQUEST_SIM_IO
 *   Request SIM I/O operation.
 *   This is similar to the TS 27.007 "restricted SIM" operation
 *   where it assumes all of the EF selection will be done by the
 *   callee.
 *
 * Out: IPC_SEC_RSIM_ACCESS
 *   Performs a restricted SIM read operation
 */
void ril_request_sim_io(RIL_Token t, void *data, int length)
{
#if RIL_VERSION >= 6
	RIL_SIM_IO_v6 *sim_io = NULL;
#else
	RIL_SIM_IO *sim_io = NULL;
#endif
	void *sim_io_data = NULL;
	int sim_io_data_length = 0;
	int rc;

	if (data == NULL || length < (int) sizeof(*sim_io))
		return;

#if RIL_VERSION >= 6
	sim_io = (RIL_SIM_IO_v6 *) data;
#else
	sim_io = (RIL_SIM_IO *) data;
#endif

	// SIM IO data should be a string if present
	if (sim_io->data != NULL) {
		sim_io_data_length = strlen(sim_io->data) / 2;
		if (sim_io_data_length > 0) {
			sim_io_data = calloc(1, sim_io_data_length);
			hex2bin(sim_io->data, sim_io_data_length * 2, sim_io_data);
		}
	}

	if (ril_data.tokens.sim_io != (RIL_Token) 0x00) {
		LOGD("Another SIM I/O is being processed, adding to the list");

		rc = ril_request_sim_io_register(t, sim_io->command, sim_io->fileid,
			sim_io->p1, sim_io->p2, sim_io->p3, sim_io_data, sim_io_data_length);
		if (rc < 0) {
			LOGE("Unable to add the request to the list");

			ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
			if (sim_io_data != NULL)
				free(sim_io_data);
			// Send the next SIM I/O in the list
			ril_request_sim_io_next();
		}

		return;
	}

	ril_data.tokens.sim_io = t;

	ril_request_sim_io_complete(t, sim_io->command, sim_io->fileid,
		sim_io->p1, sim_io->p2, sim_io->p3, sim_io_data, sim_io_data_length);
	if (sim_io_data != NULL)
		free(sim_io_data);
}

/**
 * In: IPC_SEC_RSIM_ACCESS
 *   Provides restricted SIM read operation result
 *
 * Out: RIL_REQUEST_SIM_IO
 *   Request SIM I/O operation.
 *   This is similar to the TS 27.007 "restricted SIM" operation
 *   where it assumes all of the EF selection will be done by the
 *   callee.
 */
void ipc_sec_rsim_access(struct ipc_message_info *info)
{
	RIL_SIM_IO_Response sim_io_response;
	struct ipc_sec_rsim_access_response *rsim_access = NULL;
	void *rsim_access_data = NULL;
	char *sim_response = NULL;

	if (info == NULL || info->data == NULL || info->length < sizeof(struct ipc_sec_rsim_access_response))
		return;

	memset(&sim_io_response, 0, sizeof(sim_io_response));

	rsim_access = (struct ipc_sec_rsim_access_response *) info->data;
	rsim_access_data = (void *) ((int) info->data + sizeof(struct ipc_sec_rsim_access_response));

	sim_io_response.sw1 = rsim_access->sw1;
	sim_io_response.sw2 = rsim_access->sw2;

	if (rsim_access->len > 0) {
		sim_response = (char *) malloc(rsim_access->len * 2 + 1);
		bin2hex(rsim_access_data, rsim_access->len, sim_response);
		sim_io_response.simResponse = sim_response;
	} else {
		sim_io_response.simResponse = strdup("");
	}

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &sim_io_response, sizeof(sim_io_response));

	free(sim_io_response.simResponse);

	// Send the next SIM I/O in the list
	ril_request_sim_io_next();
}

/**
 * In: IPC_GEN_PHONE_RES
 *   Provides result of IPC_SEC_SIM_STATUS SET
 *
 * Out: RIL_REQUEST_ENTER_SIM_PIN
 *   Returns PIN SIM unlock result
 */
void ipc_sec_sim_status_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res = (struct ipc_gen_phone_res *) info->data;
	int rc;

	int attempts = -1;

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		if ((phone_res->code & 0x00ff) == 0x10) {
			LOGE("Wrong password!");
			ril_request_complete(ril_request_get_token(info->aseq), RIL_E_PASSWORD_INCORRECT, &attempts, sizeof(attempts));
		} else if ((phone_res->code & 0x00ff) == 0x0c) {
			LOGE("Wrong password and no attempts left!");

			attempts = 0;
			ril_request_complete(ril_request_get_token(info->aseq), RIL_E_PASSWORD_INCORRECT, &attempts, sizeof(attempts));

			ril_request_unsolicited(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
		} else {
			LOGE("There was an error during pin status complete!");
			ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		}
		return;
	}

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &attempts, sizeof(attempts));
}

/**
 * In: IPC_SEC_LOCK_INFO
 *   Provides number of retries left for a lock type
 */
void ipc_sec_lock_info(struct ipc_message_info *info)
{
	/*
	 * FIXME: solid way of handling lockinfo and sim unlock response together
	 * so we can return the number of attempts left in respondSecPinStatus
	 */
	int attempts;
	struct ipc_sec_lock_info_response *lock_info = (struct ipc_sec_lock_info_response *) info->data;

	if (lock_info->type == IPC_SEC_PIN_TYPE_PIN1) {
		attempts = lock_info->attempts;
		LOGD("%s: PIN1 %d attempts left", __FUNCTION__, attempts);
	} else {
		LOGE("%s: unhandled lock type %d", __FUNCTION__, lock_info->type);
	}
}

/**
 * In: RIL_REQUEST_ENTER_SIM_PIN
 *   Supplies SIM PIN. Only called if RIL_CardStatus has RIL_APPSTATE_PIN state
 * 
 * Out: IPC_SEC_SIM_STATUS SET
 *   Attempts to unlock SIM PIN1
 *
 * Out: IPC_SEC_LOCK_INFO
 *   Retrieves PIN1 lock status
 */
void ril_request_enter_sim_pin(RIL_Token t, void *data, size_t datalen)
{
	struct ipc_sec_pin_status_set pin_status;
	char *pin = ((char **) data)[0];
	unsigned char buf[9];

	/* 1. Send PIN */
	if (strlen(data) > 16) {
		LOGE("%s: pin exceeds maximum length", __FUNCTION__);
		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	}

	ipc_sec_pin_status_set_setup(&pin_status, IPC_SEC_PIN_TYPE_PIN1, pin, NULL);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_SEC_SIM_STATUS,
		ipc_sec_sim_status_complete);

	ipc_fmt_send_set(IPC_SEC_SIM_STATUS, ril_request_get_id(t), (unsigned char *) &pin_status, sizeof(pin_status));

	/* 2. Get lock status */
	// FIXME: This is not clean at all
	memset(buf, 0, sizeof(buf));
	buf[0] = 1;
	buf[1] = IPC_SEC_PIN_TYPE_PIN1;

	ipc_fmt_send(IPC_SEC_LOCK_INFO, IPC_TYPE_GET, buf, sizeof(buf), ril_request_get_id(t));
}

void ril_request_change_sim_pin(RIL_Token t, void *data, size_t datalen)
{
	char *password_old = ((char **) data)[0];
	char *password_new = ((char **) data)[1];
	struct ipc_sec_change_locking_pw_set locking_pw;

	memset(&locking_pw, 0, sizeof(locking_pw));

	locking_pw.facility = IPC_SEC_SIM_STATUS_LOCK_SC;

	locking_pw.length_new = strlen(password_new) > sizeof(locking_pw.password_new)
				? sizeof(locking_pw.password_new)
				: strlen(password_new);

	memcpy(locking_pw.password_new, password_new, locking_pw.length_new);

	locking_pw.length_old = strlen(password_old) > sizeof(locking_pw.password_old)
				? sizeof(locking_pw.password_old)
				: strlen(password_old);

	memcpy(locking_pw.password_old, password_old, locking_pw.length_old);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_SEC_CHANGE_LOCKING_PW,
		ipc_sec_sim_status_complete);

	ipc_fmt_send_set(IPC_SEC_CHANGE_LOCKING_PW, ril_request_get_id(t), (unsigned char *) &locking_pw, sizeof(locking_pw));
}

void ril_request_enter_sim_puk(RIL_Token t, void *data, size_t datalen)
{
	struct ipc_sec_pin_status_set pin_status;
	char *puk = ((char **) data)[0];
	char *pin = ((char **) data)[1];

	ipc_sec_pin_status_set_setup(&pin_status, IPC_SEC_PIN_TYPE_PIN1, pin, puk);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_SEC_SIM_STATUS,
		ipc_sec_sim_status_complete);

	ipc_fmt_send_set(IPC_SEC_SIM_STATUS, ril_request_get_id(t), (unsigned char *) &pin_status, sizeof(pin_status));
}

/**
 * In: IPC_SEC_PHONE_LOCK
 *
 * Out: RIL_REQUEST_QUERY_FACILITY_LOCK
 *   Query the status of a facility lock state
 */
void ipc_sec_phone_lock(struct ipc_message_info *info)
{
	int status;
	struct ipc_sec_phone_lock_response *lock = (struct ipc_sec_phone_lock_response *) info->data;

	status = lock->status;

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, &status, sizeof(status));
}

/**
 * In: RIL_REQUEST_QUERY_FACILITY_LOCK
 *   Query the status of a facility lock state
 *
 * Out: IPC_SEC_PHONE_LOCK GET
 */
void ril_request_query_facility_lock(RIL_Token t, void *data, size_t datalen)
{
	struct ipc_sec_phone_lock_get lock_request;

	char *facility = ((char **) data)[0];

	if (!strcmp(facility, "SC")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_SC;
	} else if (!strcmp(facility, "FD")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_FD;
	} else if (!strcmp(facility, "PN")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_PN;
	} else if (!strcmp(facility, "PU")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_PU;
	} else if (!strcmp(facility, "PP")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_PP;
	} else if (!strcmp(facility, "PC")) {
		lock_request.facility = IPC_SEC_FACILITY_TYPE_PC;
	} else {
		LOGE("%s: unsupported facility: %s", __FUNCTION__, facility);
		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	}

	ipc_fmt_send(IPC_SEC_PHONE_LOCK, IPC_TYPE_GET, (void *) &lock_request, sizeof(lock_request), ril_request_get_id(t));
}

// Both functions were the same
#define ipc_sec_phone_lock_complete \
	ipc_sec_sim_status_complete

/**
 * In: RIL_REQUEST_SET_FACILITY_LOCK
 *   Enable/disable one facility lock
 *
 * Out: IPC_SEC_PHONE_LOCK SET
 */
void ril_request_set_facility_lock(RIL_Token t, void *data, size_t datalen)
{
	struct ipc_sec_phone_lock_set lock_request;

	char *facility = ((char **) data)[0];
	char *lock = ((char **) data)[1];
	char *password = ((char **) data)[2];
	char *class = ((char **) data)[3];

	memset(&lock_request, 0, sizeof(lock_request));

	if (!strcmp(facility, "SC")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_SC;
	} else if (!strcmp(facility, "FD")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_FD;
	} else if (!strcmp(facility, "PN")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_PN;
	} else if (!strcmp(facility, "PU")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_PU;
	} else if (!strcmp(facility, "PP")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_PP;
	} else if (!strcmp(facility, "PC")) {
		lock_request.type = IPC_SEC_SIM_STATUS_LOCK_PC;
	} else {
		LOGE("%s: unsupported facility: %s", __FUNCTION__, facility);
		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	}

	lock_request.lock = lock[0] == '1' ? 1 : 0;
	lock_request.length = strlen(password) > sizeof(lock_request.password)
				? sizeof(lock_request.password)
				: strlen(password);

	memcpy(lock_request.password, password, lock_request.length);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_SEC_PHONE_LOCK,
		ipc_sec_phone_lock_complete);

	ipc_fmt_send(IPC_SEC_PHONE_LOCK, IPC_TYPE_SET, (void *) &lock_request, sizeof(lock_request), ril_request_get_id(t));
}
