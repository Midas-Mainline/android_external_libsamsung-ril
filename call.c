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
			return IPC_CALL_IDENTITY_DEFAULT;
	}
}

unsigned char ipc2ril_call_list_entry_state(unsigned char status)
{
	switch (status) {
		case IPC_CALL_LIST_ENTRY_STATUS_ACTIVE:
			return RIL_CALL_ACTIVE;
		case IPC_CALL_LIST_ENTRY_STATUS_HOLDING:
			return RIL_CALL_HOLDING;
		case IPC_CALL_LIST_ENTRY_STATUS_DIALING:
			return RIL_CALL_DIALING;
		case IPC_CALL_LIST_ENTRY_STATUS_ALERTING:
			return RIL_CALL_ALERTING;
		case IPC_CALL_LIST_ENTRY_STATUS_INCOMING:
			return RIL_CALL_INCOMING;
		case IPC_CALL_LIST_ENTRY_STATUS_WAITING:
			return RIL_CALL_WAITING;
		default:
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
			return CALL_FAIL_ERROR_UNSPECIFIED;
	}
}

int ril_request_dial(void *data, size_t size, RIL_Token token)
{
	struct ipc_call_outgoing_data request_data;
	RIL_Dial *dial = NULL;
	unsigned char identity;
	unsigned char prefix;
	int rc;

	if (data == NULL || size < sizeof(RIL_Dial))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	dial = (RIL_Dial *) data;

	if (dial->address == NULL)
		goto error;

	identity = ril2ipc_call_identity(dial->clir);
	prefix = dial->address[0] == '+' ? IPC_CALL_PREFIX_INTL : IPC_CALL_PREFIX_NONE;

	rc = ipc_call_outgoing_setup(&request_data, IPC_CALL_TYPE_VOICE, identity, prefix, dial->address);
	if (rc < 0)
		goto error;

	free(dial->address);
	dial = NULL;

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_CALL_OUTGOING);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_CALL_OUTGOING, IPC_TYPE_EXEC, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (dial != NULL && dial->address != NULL)
		free(dial->address);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_call_incoming(__attribute__((unused)) struct ipc_message *message)
{
	ril_request_unsolicited(RIL_UNSOL_CALL_RING, NULL, 0);

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);

	return 0;
}

int ril_request_hangup(__attribute__((unused)) void *data,
		       __attribute__((unused)) size_t size, RIL_Token token)
{
	int hangup;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	hangup = 1;
	ril_request_data_set_uniq(RIL_REQUEST_HANGUP, &hangup, sizeof(hangup));

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_CALL_RELEASE);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_CALL_RELEASE, IPC_TYPE_EXEC, NULL, 0);
	if (rc < 0)
		goto error;

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_data_free(RIL_REQUEST_HANGUP);

	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_answer(__attribute__((unused)) void *data,
		       __attribute__((unused)) size_t size,
		       RIL_Token token)
{
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_CALL_ANSWER);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_CALL_ANSWER, IPC_TYPE_EXEC, NULL, 0);
	if (rc < 0)
		goto error;

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ipc_call_status(struct ipc_message *message)
{
	struct ipc_call_status_data *data;
	int fail_cause;
	void *hangup_data;
	size_t hangup_size;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_call_status_data))
		return -1;

	data = (struct ipc_call_status_data *) message->data;

	// Nobody will ask for a call fail cause when we hangup ourselves
	hangup_size = ril_request_data_size_get(RIL_REQUEST_HANGUP);
	hangup_data = ril_request_data_get(RIL_REQUEST_HANGUP);

	if (data->status == IPC_CALL_STATUS_RELEASED && (hangup_data == NULL || hangup_size == 0)) {
		fail_cause = ipc2ril_call_fail_cause(data->end_cause);

		ril_request_data_set_uniq(RIL_REQUEST_LAST_CALL_FAIL_CAUSE, &fail_cause, sizeof(fail_cause));
	} else if (hangup_data != NULL && hangup_size > 0) {
		free(hangup_data);
	}

	ril_request_unsolicited(RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED, NULL, 0);

	return 0;
}

int ril_request_last_call_fail_cause(__attribute__((unused)) void *data,
				     __attribute__((unused)) size_t size,
				     RIL_Token token)
{
	void *fail_cause_data;
	size_t fail_cause_size;
	int fail_cause;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	fail_cause_size = ril_request_data_size_get(RIL_REQUEST_LAST_CALL_FAIL_CAUSE);
	fail_cause_data = ril_request_data_get(RIL_REQUEST_LAST_CALL_FAIL_CAUSE);

	if (fail_cause_data == NULL || fail_cause_size < sizeof(fail_cause)) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	fail_cause = *((int *) fail_cause_data);

	ril_request_complete(token, RIL_E_SUCCESS, &fail_cause, sizeof(fail_cause));

	free(fail_cause_data);

	return RIL_REQUEST_COMPLETED;
}

int ipc_call_list(struct ipc_message *message)
{
	struct ipc_call_list_entry *entry;
	RIL_Call **calls = NULL;
	size_t calls_size;
	unsigned int calls_count = 0;
	unsigned char count;
	unsigned char index;
	char *number;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_call_list_header))
		return -1;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	count = ipc_call_list_count_extract(message->data, message->size);
	if (count == 0) {
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
		return 0;
	}

	calls_size = count * sizeof(RIL_Call *);
	calls = (RIL_Call **) calloc(1, calls_size);

	for (index = 0; index < count; index++) {
		entry = ipc_call_list_entry_extract(message->data, message->size, index);
		if (entry == NULL)
			goto error;

		number = ipc_call_list_entry_number_extract(entry);
		if (number == NULL)
			goto error;

		calls[index] = (RIL_Call *) calloc(1, sizeof(RIL_Call));
		calls[index]->state = ipc2ril_call_list_entry_state(entry->status);
		calls[index]->index = entry->id;
		calls[index]->isMpty = entry->mpty;
		calls[index]->isMT = entry->term == IPC_CALL_TERM_MT;
		calls[index]->als = 0;
		calls[index]->isVoice = entry->type == IPC_CALL_TYPE_VOICE;
		calls[index]->isVoicePrivacy = 0;
		calls[index]->number = strdup(number);
		calls[index]->numberPresentation = (entry->number_length > 0) ? 0 : 2;
		calls[index]->name = NULL;
		calls[index]->namePresentation = 2;
		calls[index]->uusInfo = NULL;

		if (entry->number_length > 0 && number != NULL && number[0] == '+')
			calls[index]->toa = 145;
		else
			calls[index]->toa = 129;

		calls_count++;
	}

	calls_size = calls_count * sizeof(RIL_Call *);

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, (void *) calls, calls_size);

	goto complete;

error:
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	if (calls != NULL && calls_size > 0) {
		for (index = 0; index < calls_count; index++) {
			if (calls[index] == NULL)
				continue;

			if (calls[index]->number != NULL)
				free(calls[index]->number);

			free(calls[index]);
		}

		free(calls);
	}

	return 0;
}

int ril_request_get_current_calls(__attribute__((unused)) void *data,
				  __attribute__((unused)) size_t size,
				  RIL_Token token)
{
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_SUCCESS, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_CALL_LIST, IPC_TYPE_GET, NULL, 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}

int ipc_call_cont_dtmf_callback(struct ipc_message *message)
{
	struct ipc_gen_phone_res_data *data;
	void *dtmf_data;
	size_t dtmf_size;
	char tone;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0)
		goto error;

	dtmf_size = ril_request_data_size_get(RIL_REQUEST_DTMF_START);
	dtmf_data = ril_request_data_get(RIL_REQUEST_DTMF_START);

	if (dtmf_data != NULL && dtmf_size >= sizeof(tone)) {
		tone = *((char *) dtmf_data);

		// Register a new DTMF tone
		ril_request_data_set(RIL_REQUEST_DTMF_START, dtmf_data, dtmf_size);

		free(dtmf_data);

		rc = ril_request_dtmf_start_complete(message->aseq, tone);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_DTMF_START);
			goto error;
		}
	}

	dtmf_size = ril_request_data_size_get(RIL_REQUEST_DTMF);
	dtmf_data = ril_request_data_get(RIL_REQUEST_DTMF);

	if (dtmf_data != NULL && dtmf_size >= sizeof(tone)) {
		tone = *((char *) dtmf_data);

		free(dtmf_data);

		rc = ril_request_dtmf_complete(message->aseq, tone);
		if (rc < 0)
			goto error;
	}

	goto complete;

error:
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

int ipc_call_burst_dtmf(struct ipc_message *message)
{
	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_call_burst_dtmf_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);

	return 0;
}

int ril_request_dtmf_complete(unsigned char aseq, char tone)
{
	struct ipc_call_burst_dtmf_request_entry entry;
	void *request_data = NULL;
	size_t request_size = 0;
	int rc;

	memset(&entry, 0, sizeof(entry));
	entry.status = IPC_CALL_DTMF_STATUS_START;
	entry.tone = tone;

	request_size = ipc_call_burst_dtmf_size_setup(&entry, 1);
	if (request_size == 0)
		goto error;

	request_data = ipc_call_burst_dtmf_setup(&entry, 1);
	if (request_data == NULL)
		goto error;

	rc = ipc_gen_phone_res_expect_abort(aseq, IPC_CALL_BURST_DTMF);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(aseq, IPC_CALL_BURST_DTMF, IPC_TYPE_EXEC, request_data, request_size);
	if (rc < 0)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (request_data != NULL && request_size > 0)
		free(request_data);

	return rc;
}

int ril_request_dtmf(void *data, size_t size, RIL_Token token)
{
	struct ril_request *request;
	void *dtmf_data;
	size_t dtmf_size;
	char tone;
	int rc;

	if (data == NULL || size < sizeof(char))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_START, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_STOP, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	tone = *((char *) data);

	// A previous DTMF tone was started
	dtmf_size = ril_request_data_size_get(RIL_REQUEST_DTMF_START);
	dtmf_data = ril_request_data_get(RIL_REQUEST_DTMF_START);

	if (dtmf_data != NULL && dtmf_size >= sizeof(tone)) {
		free(dtmf_data);

		// Let the callback know what to do after completing the previous DTMF tone
		ril_request_data_set(RIL_REQUEST_DTMF, data, size);

		rc = ril_request_dtmf_stop_complete(ipc_fmt_request_seq(token), 1);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_DTMF);
			goto error;
		}

		rc = RIL_REQUEST_HANDLED;
		goto complete;
	}

	rc = ril_request_dtmf_complete(ipc_fmt_request_seq(token), tone);
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

int ril_request_dtmf_start_complete(unsigned char aseq, char tone)
{
	struct ipc_call_cont_dtmf_data request_data;
	int rc;

	memset(&request_data, 0, sizeof(request_data));
	request_data.status = IPC_CALL_DTMF_STATUS_START;
	request_data.tone = tone;

	rc = ipc_gen_phone_res_expect_complete(aseq, IPC_CALL_CONT_DTMF);
	if (rc < 0)
		return -1;

	rc = ipc_fmt_send(aseq, IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return -1;

	return 0;
}

int ril_request_dtmf_start(void *data, size_t size, RIL_Token token)
{
	struct ril_request *request;
	void *dtmf_data;
	size_t dtmf_size;
	char tone;
	int rc;

	if (data == NULL || size < sizeof(char))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_START, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_STOP, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	tone = *((char *) data);

	// A previous DTMF tone was started
	dtmf_size = ril_request_data_size_get(RIL_REQUEST_DTMF_START);
	dtmf_data = ril_request_data_get(RIL_REQUEST_DTMF_START);

	if (dtmf_data != NULL && dtmf_size >= sizeof(tone)) {
		free(dtmf_data);

		// Let the callback know what to do after completing the previous DTMF tone
		ril_request_data_set(RIL_REQUEST_DTMF_START, data, size);

		rc = ril_request_dtmf_stop_complete(ipc_fmt_request_seq(token), 1);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_DTMF_START);
			goto error;
		}

		rc = RIL_REQUEST_HANDLED;
		goto complete;
	}

	// Register a new DTMF tone
	ril_request_data_set(RIL_REQUEST_DTMF_START, data, size);

	rc = ril_request_dtmf_start_complete(ipc_fmt_request_seq(token), tone);
	if (rc < 0) {
		ril_request_data_free(RIL_REQUEST_DTMF_START);
		goto error;
	}

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	return rc;
}

int ril_request_dtmf_stop_complete(unsigned char aseq, int callback)
{
	struct ipc_call_cont_dtmf_data request_data;
	int rc;

	memset(&request_data, 0, sizeof(request_data));
	request_data.status = IPC_CALL_DTMF_STATUS_STOP;
	request_data.tone = 0;

	if (callback)
		rc = ipc_gen_phone_res_expect_callback(aseq, IPC_CALL_CONT_DTMF, ipc_call_cont_dtmf_callback);
	else
		rc = ipc_gen_phone_res_expect_complete(aseq, IPC_CALL_CONT_DTMF);

	if (rc < 0)
		return -1;

	rc = ipc_fmt_send(aseq, IPC_CALL_CONT_DTMF, IPC_TYPE_SET, (void *) &request_data, sizeof(request_data));
	if (rc < 0)
		return -1;

	return 0;
}

int ril_request_dtmf_stop(__attribute__((unused)) void *data,
			  __attribute__((unused)) size_t size,
			  RIL_Token token)
{
	struct ril_request *request;
	void *dtmf_data;
	size_t dtmf_size;
	int rc;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_START, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_DTMF_STOP, RIL_REQUEST_HANDLED);
	if (request != NULL)
		return RIL_REQUEST_UNHANDLED;

	// Clear the DTMF tone
	dtmf_size = ril_request_data_size_get(RIL_REQUEST_DTMF_START);
	dtmf_data = ril_request_data_get(RIL_REQUEST_DTMF_START);

	if (dtmf_data != NULL && dtmf_size >= sizeof(char))
		free(dtmf_data);

	rc = ril_request_dtmf_stop_complete(ipc_fmt_request_seq(token), 0);
	if (rc < 0) {
		ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
		return RIL_REQUEST_COMPLETED;
	}

	return RIL_REQUEST_HANDLED;
}
