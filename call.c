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

#define LOG_TAG "RIL-CALL"
#include <utils/Log.h>

#include "samsung-ril.h"

unsigned char ril2ipc_call_identity(int clir)
{
	switch (clir) {
		case 0:
			return IPC_CALL_IDENTITY_DEFAULT;
		case 1:
			return IPC_CALL_IDENTITY_SHOW;
		case 2:
			return IPC_CALL_IDENTITY_HIDE;
		default:
			LOGE("Unknown call identity: 0x%x", clir);
			return IPC_CALL_IDENTITY_DEFAULT;
	}
}

unsigned char ipc2ril_call_list_entry_state(unsigned char call_state)
{
	switch (call_state) {
		case IPC_CALL_LIST_ENTRY_STATE_ACTIVE:
			return RIL_CALL_ACTIVE;
		case IPC_CALL_LIST_ENTRY_STATE_HOLDING:
			return RIL_CALL_HOLDING;
		case IPC_CALL_LIST_ENTRY_STATE_DIALING:
			return RIL_CALL_DIALING;
		case IPC_CALL_LIST_ENTRY_STATE_ALERTING:
			return RIL_CALL_ALERTING;
		case IPC_CALL_LIST_ENTRY_STATE_INCOMING:
			return RIL_CALL_INCOMING;
		case IPC_CALL_LIST_ENTRY_STATE_WAITING:
			return RIL_CALL_WAITING;
		default:
			LOGE("Unknown call list entry state: 0x%x", call_state);
			return -1;
	}
}

RIL_LastCallFailCause ipc2ril_call_fail_cause(unsigned char end_cause)
{
	switch (end_cause) {
		case IPC_CALL_END_CAUSE_NORMAL:
		case IPC_CALL_END_CAUSE_REJECTED:
			return CALL_FAIL_NORMAL;
		case IPC_CALL_END_CAUSE_UNSPECIFIED:
		default:
			LOGE("Unknown call fail cause: 0x%x", end_cause);
			return CALL_FAIL_ERROR_UNSPECIFIED;
	}
}

void ipc_call_incoming(struct ipc_message_info *info)
{
	ril_request_unsolicited(RIL_UNSOL_CALL_RING, NULL, 0);

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
}

void ipc_call_status(struct ipc_message_info *info)
{
	struct ipc_call_status *call_status;

	if (info->data == NULL || info->length < sizeof(struct ipc_call_status))
		return;

	call_status = (struct ipc_call_status *) info->data;

	LOGD("Updating call status data");
	memcpy(&(ril_data.state.call_status), call_status, sizeof(struct ipc_call_status));

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
}

void ril_request_dial(RIL_Token t, void *data, size_t length)
{
	RIL_Dial *dial;
	struct ipc_call_outgoing call;
	int clir;

	if (data == NULL || length < sizeof(RIL_Dial))
		goto error;

	dial = (RIL_Dial *) data;

	if (strlen(dial->address) > sizeof(call.number)) {
		printf("Outgoing call number too long\n");
		goto error;
	}

	memset(&call, 0, sizeof(call));
	call.type = IPC_CALL_TYPE_VOICE;
	call.identity = ril2ipc_call_identity(dial->clir);
	call.prefix = dial->address[0] == '+' ? IPC_CALL_PREFIX_INTL : IPC_CALL_PREFIX_NONE;
	call.length = strlen(dial->address);
	memcpy(call.number, dial->address, strlen(dial->address));

	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_CALL_OUTGOING);

	ipc_fmt_send(IPC_CALL_OUTGOING, IPC_TYPE_EXEC, (unsigned char *) &call, sizeof(call), ril_request_get_id(t));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_get_current_calls(RIL_Token t)
{
	ipc_fmt_send_get(IPC_CALL_LIST, ril_request_get_id(t));
}

void ipc_call_list(struct ipc_message_info *info)
{
	struct ipc_call_list_entry *entry;
	unsigned char count;
	char *number;
	RIL_Call **current_calls = NULL;
	int i;

	if (info->data == NULL || info->length < sizeof(unsigned char))
		goto error;

	if (info->type != IPC_TYPE_RESP)
		return;

	count = *((unsigned char *) info->data);
	if (count == 0) {
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, NULL, 0);
		return;
	}

	current_calls = (RIL_Call **) calloc(1, count * sizeof(RIL_Call *));
	entry = (struct ipc_call_list_entry *) ((char *) info->data + sizeof(unsigned char));

	for (i = 0 ; i < count ; i++) {
		if (((int) entry - (int) info->data) >= (int) info->length)
			goto error;

		number = ((char *) entry) + sizeof(struct ipc_call_list_entry);

		current_calls[i] = (RIL_Call *) calloc(1, sizeof(RIL_Call));

		current_calls[i]->state = ipc2ril_call_list_entry_state(entry->state);
		current_calls[i]->index = entry->idx;
		current_calls[i]->toa = (entry->number_len > 0 && number[0] == '+') ? 145 : 129;
		current_calls[i]->isMpty = entry->mpty;
		current_calls[i]->isMT = (entry->term == IPC_CALL_TERM_MT);
		current_calls[i]->als = 0;
		current_calls[i]->isVoice  = (entry->type == IPC_CALL_TYPE_VOICE);
		current_calls[i]->isVoicePrivacy = 0;
		current_calls[i]->number = strdup(number);
		current_calls[i]->numberPresentation = (entry->number_len > 0) ? 0 : 2;
		current_calls[i]->name = NULL;
		current_calls[i]->namePresentation = 2;
		current_calls[i]->uusInfo = NULL;

		entry = (struct ipc_call_list_entry *) (number + entry->number_len);
	}

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, current_calls, (count * sizeof(RIL_Call *)));

	for (i = 0 ; i < count ; i++) {
		if (current_calls[i]->number != NULL)
			free(current_calls[i]->number);

		free(current_calls[i]);
	}

	free(current_calls);

	return;

error:
	if (current_calls != NULL) {
		for (i = 0 ; i < count ; i++) {
			if (current_calls[i]->number != NULL)
				free(current_calls[i]->number);

			free(current_calls[i]);
		}

		free(current_calls);
	}

	if (info->type == IPC_TYPE_RESP)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_hangup(RIL_Token t)
{
	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_CALL_RELEASE);

	ipc_fmt_send_exec(IPC_CALL_RELEASE, ril_request_get_id(t));

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
}


void ril_request_answer(RIL_Token t)
{
	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_CALL_ANSWER);

	ipc_fmt_send_exec(IPC_CALL_ANSWER, ril_request_get_id(t));

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);
}

void ril_request_last_call_fail_cause(RIL_Token t)
{
	RIL_LastCallFailCause fail_cause;
	struct ipc_call_status *call_status = &(ril_data.state.call_status);

	fail_cause = ipc2ril_call_fail_cause(call_status->end_cause);

	// Empty global call_status
	memset(call_status, 0, sizeof(struct ipc_call_status));

	ril_request_complete(t, RIL_E_SUCCESS, &fail_cause, sizeof(RIL_LastCallFailCause));
}

void ril_request_dtmf(RIL_Token t, void *data, int length)
{
	struct ipc_call_cont_dtmf cont_dtmf;
	unsigned char tone;
	unsigned char count;

	unsigned char *burst;
	int burst_length;

	int i;

	if (data == NULL || length < (int) sizeof(unsigned char))
		goto error;

	tone = *((unsigned char *) data);
	count = 1;

	if (ril_data.state.dtmf_tone != 0) {
		LOGD("Another tone wasn't stopped, stopping it before anything");

		cont_dtmf.state = IPC_CALL_DTMF_STATE_STOP;
		cont_dtmf.tone = 0;

		ipc_fmt_send(IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (void *) &cont_dtmf, sizeof(cont_dtmf), ril_request_get_id(t));

		usleep(300);
	}

	burst_length = sizeof(struct ipc_call_cont_dtmf) * count + 1;
	burst = calloc(1, burst_length);

	burst[0] = count;

	// Apparently, it's possible to set multiple DTMF tones on this message
	for (i = 0 ; i < count ; i++) {
		cont_dtmf.state = IPC_CALL_DTMF_STATE_START;
		cont_dtmf.tone = tone;

		memcpy(burst + 1 + sizeof(struct ipc_call_cont_dtmf) * i, &cont_dtmf, sizeof(cont_dtmf));
	}

	ipc_gen_phone_res_expect_to_abort(ril_request_get_id(t), IPC_CALL_BURST_DTMF);

	ipc_fmt_send(IPC_CALL_BURST_DTMF, IPC_TYPE_EXEC, burst, burst_length, ril_request_get_id(t));

	free(burst);

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_call_burst_dtmf(struct ipc_message_info *info)
{
	unsigned char code;

	if (info->data == NULL || info->length < sizeof(unsigned char))
		goto error;

	code = *((unsigned char *) info->data);

	// This apparently should return 1, or perhaps that is the DTMF tones count
	if (code == 0) {
		LOGD("Apparently, something went wrong with DTMF burst (code=0x%x)", code);
		goto error;
	}

	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS, NULL, 0);

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_dtmf_start(RIL_Token t, void *data, int length)
{
	struct ipc_call_cont_dtmf cont_dtmf;
	unsigned char tone;

	if (data == NULL || length < (int) sizeof(unsigned char))
		goto error;

	tone = *((unsigned char *) data);

	if (ril_data.state.dtmf_tone != 0) {
		LOGD("Another tone wasn't stopped, stopping it before anything");

		cont_dtmf.state = IPC_CALL_DTMF_STATE_STOP;
		cont_dtmf.tone = 0;

		ipc_fmt_send(IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (unsigned char *) &cont_dtmf, sizeof(cont_dtmf), ril_request_get_id(t));

		usleep(300);
	}

	ril_data.state.dtmf_tone = cont_dtmf.tone;

	cont_dtmf.state = IPC_CALL_DTMF_STATE_START;
	cont_dtmf.tone = tone;

	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_CALL_CONT_DTMF);

	ipc_fmt_send(IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (unsigned char *) &cont_dtmf, sizeof(cont_dtmf), ril_request_get_id(t));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_dtmf_stop(RIL_Token t)
{
	struct ipc_call_cont_dtmf cont_dtmf;

	ril_data.state.dtmf_tone = 0;

	cont_dtmf.state = IPC_CALL_DTMF_STATE_STOP;
	cont_dtmf.tone = 0;

	ipc_gen_phone_res_expect_to_complete(ril_request_get_id(t), IPC_CALL_CONT_DTMF);

	ipc_fmt_send(IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (unsigned char *) &cont_dtmf, sizeof(cont_dtmf), ril_request_get_id(t));
}
