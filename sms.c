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

unsigned short ril2ipc_sms_ack(int success, int fail_cause)
{
	if (success) {
		return IPC_SMS_ACK_NO_ERROR;
	} else {
		switch (fail_cause) {
			case 0xD3:
				return IPC_SMS_ACK_PDA_FULL_ERROR;
			default:
				return IPC_SMS_ACK_UNSPEC_ERROR;
		}
	}
}

RIL_Errno ipc2ril_sms_ack_error(unsigned short ack)
{
	switch (ack) {
		case IPC_SMS_ACK_NO_ERROR:
			return RIL_E_SUCCESS;
		default:
			return RIL_E_GENERIC_FAILURE;
	}
}

int ipc2ril_sms_ack_error_code(unsigned short ack)
{
	// Error code is defined in 3GPP 27.005, 3.2.5 for GSM/UMTS

	switch (ack) {
		case IPC_SMS_ACK_NO_ERROR:
			return -1;
		default:
			return 500;
	}
}

unsigned char ril2ipc_sms_status(int status)
{
	switch (status) {
		case 0:
			return IPC_SMS_STATUS_REC_UNREAD;
		case 1:
			return IPC_SMS_STATUS_REC_READ;
		case 2:
			return IPC_SMS_STATUS_STO_UNSENT;
		case 3:
			return IPC_SMS_STATUS_STO_SENT;
		default:
			return 0;
	}
}

int ipc_sms_send_msg(struct ipc_message *message)
{
	struct ipc_sms_send_msg_response_data *data;
	RIL_SMS_Response response;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_send_msg_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sms_send_msg_response_data *) message->data;

	memset(&response, 0, sizeof(response));
	response.messageRef = data->id;
	response.ackPDU = NULL;

	ril_request_complete(ipc_fmt_request_token(message->aseq), ipc2ril_sms_ack_error(data->ack), &response, sizeof(response));

	return 0;
}

int ril_request_send_sms_complete(unsigned char seq, const void *smsc,
	size_t smsc_size, const void *pdu, size_t pdu_size)
{
	struct ipc_sms_send_msg_request_header request_header;
	unsigned int count;
	unsigned int index;
	void *data = NULL;
	size_t size = 0;
	unsigned char *p;
	int rc;

	if (smsc == NULL || smsc_size == 0 || pdu == NULL || pdu_size == 0)
		return -1;

	if (!ipc_seq_valid(seq))
		return -1;

	memset(&request_header, 0, sizeof(request_header));
	request_header.type = IPC_SMS_TYPE_OUTGOING;
	request_header.msg_type = IPC_SMS_MSG_TYPE_SINGLE;

	p = (unsigned char *) pdu;

	// PDU TP-DA length
	p += 2;

	if (*p > (255 / 2)) {
		RIL_LOGE("PDU TP-DA length failed (0x%x)", *p);
		goto setup;
	}

	// PDU TP-UDH length
	p += *p;

	if (*p > (255 / 2) || *p < 5) {
		RIL_LOGE("PDU TP-UDH length failed (0x%x)", *p);
		goto setup;
	}

	// PDU TO-UDH count
	p += 4;
	count = (unsigned int) *p;

	if (count > 0x0f) {
		RIL_LOGE("PDU TP-UDH count failed (%d)", count);
		goto setup;
	}

	// PDU TO-UDH index
	p += 1;
	index = (unsigned int) *p;

	if (index > count) {
		RIL_LOGE("PDU TP-UDH index failed (%d)", index);
		goto setup;
	}

	if (count > 1) {
		request_header.msg_type = IPC_SMS_MSG_TYPE_MULTIPLE;
		RIL_LOGD("Sending multi-part message %d/%d\n", index, count);
	}

setup:
	size = ipc_sms_send_msg_size_setup(&request_header, smsc, smsc_size, pdu, pdu_size);
	if (size == 0)
		goto error;

	data = ipc_sms_send_msg_setup(&request_header, smsc, smsc_size, pdu, pdu_size);
	if (data == NULL)
		goto error;

	rc = ipc_gen_phone_res_expect_abort(seq, IPC_SMS_SEND_MSG);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(seq, IPC_SMS_SEND_MSG, IPC_TYPE_EXEC, data, size);
	if (rc < 0)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (data != NULL && size > 0)
		free(data);

	return rc;
}

int ril_request_send_sms(void *data, size_t size, RIL_Token token)
{
	struct ril_request *request_send_sms, *request_send_sms_expect_more;
	char **values = NULL;
	void *smsc = NULL;
	size_t smsc_size = 0;
	void *pdu = NULL;
	size_t pdu_size = 0;
	unsigned int i;
	int rc;

	if (data == NULL || size < 2 * sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request_send_sms = ril_request_find_request_status(RIL_REQUEST_SEND_SMS, RIL_REQUEST_HANDLED);
	request_send_sms_expect_more = ril_request_find_request_status(RIL_REQUEST_SEND_SMS_EXPECT_MORE, RIL_REQUEST_HANDLED);
	if (request_send_sms != NULL || request_send_sms_expect_more != NULL)
		return RIL_REQUEST_UNHANDLED;

	values = (char **) data;
	if (values[1] == NULL)
		goto error;

	pdu_size = string2data_size(values[1]);
	if (pdu_size == 0)
		goto error;

	pdu = string2data(values[1]);
	if (pdu == NULL)
		goto error;

	if (values[0] != NULL) {
		smsc_size = string2data_size(values[0]);
		if (smsc_size == 0)
			goto error;

		smsc = string2data(values[0]);
		if (smsc == NULL)
			goto error;
	}

	strings_array_free(values, size);
	values = NULL;

	if (smsc != NULL && smsc_size > 0) {
		rc = ril_request_send_sms_complete(ipc_fmt_request_seq(token), pdu, pdu_size, (void *) ((unsigned char *) smsc + sizeof(unsigned char)), ((unsigned char *) smsc)[0]);
		if (rc < 0)
			goto error;
	} else {
		ril_request_data_set_uniq(RIL_REQUEST_SEND_SMS, pdu, pdu_size);

		rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SMS_SVC_CENTER_ADDR, IPC_TYPE_GET, NULL, 0);
		if (rc < 0) {
			ril_request_data_free(RIL_REQUEST_SEND_SMS);
			goto error;
		}
	}

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	if (values != NULL)
		strings_array_free(values, size);

	ril_request_complete(token, RIL_E_SMS_SEND_FAIL_RETRY, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	if (smsc != NULL && smsc_size > 0)
		free(smsc);

	if (pdu != NULL && pdu_size > 0)
		free(pdu);

	return rc;
}

int ipc_sms_incoming_msg(struct ipc_message *message)
{
	struct ipc_sms_incoming_msg_header *header;
	void *pdu;
	size_t pdu_size;
	char *pdu_string;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_incoming_msg_header))
		return -1;

	header = (struct ipc_sms_incoming_msg_header *) message->data;

	pdu_size = ipc_sms_incoming_msg_pdu_size_extract(message->data, message->size);
	if (pdu_size == 0)
		return 0;

	pdu = ipc_sms_incoming_msg_pdu_extract(message->data, message->size);
	if (pdu == NULL)
		return 0;

	pdu_string = data2string(pdu, pdu_size);
	if (pdu_string == NULL)
		return 0;

	ril_request_data_set_uniq(RIL_REQUEST_SMS_ACKNOWLEDGE, &header->id, sizeof(header->id));

	if (header->type == IPC_SMS_TYPE_STATUS_REPORT)
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, (void *) pdu_string, sizeof(pdu_string));
	else
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NEW_SMS, (void *) pdu_string, sizeof(pdu_string));

	free(pdu_string);

	return 0;
}

int ipc_sms_save_msg(struct ipc_message *message)
{
	struct ipc_sms_save_msg_response_data *data;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_save_msg_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sms_save_msg_response_data *) message->data;

	if (data->error)
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
	else
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);

	return 0;
}

int ril_request_write_sms_to_sim(void *data, size_t size, RIL_Token token)
{
	struct ipc_sms_save_msg_request_header request_header;
	RIL_SMS_WriteArgs *args = NULL;
	void *smsc = NULL;
	size_t smsc_size = 0;
	void *pdu = NULL;
	size_t pdu_size = 0;
	void *request_data = NULL;
	size_t request_size = 0;
	int rc;

	if (data == NULL || size < sizeof(RIL_SMS_WriteArgs))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	args = (RIL_SMS_WriteArgs *) data;
	if (args->pdu == NULL) {
		if (args->smsc != NULL)
			free(args->smsc);

		goto error;
	}

	memset(&request_header, 0, sizeof(request_header));
	request_header.status = ril2ipc_sms_status(args->status);

	pdu_size = string2data_size(args->pdu);
	if (pdu_size == 0) {
		free(args->pdu);

		if (args->smsc != NULL)
			free(args->smsc);

		goto error;
	}

	pdu = string2data(args->pdu);
	if (pdu == NULL) {
		free(args->pdu);

		if (args->smsc != NULL)
			free(args->smsc);

		goto error;
	}

	free(args->pdu);

	if (args->smsc == NULL)
		goto setup;

	smsc_size = string2data_size(args->smsc);
	if (smsc_size == 0) {
		free(args->smsc);

		goto error;
	}

	smsc = string2data(args->smsc);
	if (smsc == NULL) {
		free(args->smsc);

		goto error;
	}

	free(args->smsc);

setup:
	request_size = ipc_sms_save_msg_size_setup(&request_header, smsc, smsc_size, pdu, pdu_size);
	if (request_size == 0)
		goto error;

	request_data = ipc_sms_save_msg_setup(&request_header, smsc, smsc_size, pdu, pdu_size);
	if (request_data == NULL)
		goto error;

	rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_SMS_SAVE_MSG);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SMS_SAVE_MSG, IPC_TYPE_EXEC, request_data, request_size);
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);

	rc = RIL_REQUEST_COMPLETED;

complete:
	if (smsc != NULL && smsc_size > 0)
		free(smsc);

	if (pdu != NULL && pdu_size > 0)
		free(pdu);

	if (request_data != NULL && request_size > 0)
		free(request_data);

	return rc;
}

int ipc_sms_del_msg(struct ipc_message *message)
{
	struct ipc_sms_del_msg_response_data *data;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_del_msg_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sms_del_msg_response_data *) message->data;

	if (data->error)
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
	else
		ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);

	return 0;
}

int ril_request_delete_sms_on_sim(void *data, size_t size, RIL_Token token)
{
	struct ipc_sms_del_msg_request_data request_data;
	int index;
	int rc;

	if (data == NULL || size < sizeof(int))
		goto error;

	index = *((int *) data);
	if (index == 0)
		goto error;

	rc = ipc_sms_del_msg_setup(&request_data, (unsigned short) (index - 1));
	if (rc < 0)
		goto error;

	rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_SMS_DEL_MSG);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SMS_DEL_MSG, IPC_TYPE_EXEC, (void *) &request_data, sizeof(request_data));
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

int ipc_sms_deliver_report(struct ipc_message *message)
{
	struct ipc_sms_deliver_report_response_data *data;
	RIL_Errno error;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_deliver_report_response_data))
		return -1;

	if (!ipc_seq_valid(message->aseq))
		return 0;

	data = (struct ipc_sms_deliver_report_response_data *) message->data;

	error = ipc2ril_sms_ack_error(data->ack);

	ril_request_complete(ipc_fmt_request_token(message->aseq), error, NULL, 0);

	return 0;
}

int ril_request_sms_acknowledge(void *data, size_t size, RIL_Token token)
{
	struct ipc_sms_deliver_report_request_data report_data;
	void *id_data;
	size_t id_size;
	unsigned char id;
	int *values;
	int rc;

	if (data == NULL || size < 2 * sizeof(int))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	values = (int *) data;

	id_size = ril_request_data_size_get(RIL_REQUEST_SMS_ACKNOWLEDGE);
	id_data = ril_request_data_get(RIL_REQUEST_SMS_ACKNOWLEDGE);

	if (id_data == NULL || id_size == 0) {
		RIL_LOGE("%s: No SMS left to acknowledge", __func__);
		goto error;
	}

	id = *((unsigned char *) id_data);

	free(id_data);

	memset(&report_data, 0, sizeof(report_data));
	report_data.type = IPC_SMS_TYPE_STATUS_REPORT;
	report_data.ack = ril2ipc_sms_ack(values[0], values[1]);
	report_data.id = id;

	rc = ipc_gen_phone_res_expect_abort(ipc_fmt_request_seq(token), IPC_SMS_DELIVER_REPORT);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SMS_DELIVER_REPORT, IPC_TYPE_EXEC, (void *) &report_data, sizeof(report_data));
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

int ipc_sms_svc_center_addr(struct ipc_message *message)
{
	struct ipc_sms_svc_center_addr_header *header;
	void *smsc = NULL;
	size_t smsc_size = 0;
	void *pdu = NULL;
	size_t pdu_size = 0;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_sms_svc_center_addr_header))
		return -1;

	if (message->type != IPC_TYPE_RESP || !ipc_seq_valid(message->aseq))
		return 0;

	header = (struct ipc_sms_svc_center_addr_header *) message->data;
	if (header->length == 0 || header->length > (message->size - sizeof(struct ipc_sms_svc_center_addr_header)))
		goto error;

	pdu_size = ril_request_data_size_get(RIL_REQUEST_SEND_SMS);
	if (pdu_size == 0)
		goto error;

	pdu = ril_request_data_get(RIL_REQUEST_SEND_SMS);
	if (pdu == NULL)
		goto error;

	smsc_size = ipc_sms_svc_center_addr_smsc_size_extract(message->data, message->size);
	if (smsc_size == 0)
		goto error;

	smsc = ipc_sms_svc_center_addr_smsc_extract(message->data, message->size);
	if (smsc == NULL)
		goto error;

	rc = ril_request_send_sms_complete(message->aseq, smsc, smsc_size, pdu, pdu_size);
	if (rc < 0)
		goto error;

	goto complete;

error:
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SMS_SEND_FAIL_RETRY, NULL, 0);

complete:
	if (pdu != NULL)
		free(pdu);

	return 0;
}
