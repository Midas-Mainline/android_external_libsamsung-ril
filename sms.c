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

#define LOG_TAG "RIL-SMS"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

unsigned short ril2ipc_sms_ack_error(int success, int failcause)
{
	if (success) {
		return IPC_SMS_ACK_NO_ERROR;
	} else {
		switch(failcause) {
			case 0xD3:
				return IPC_SMS_ACK_PDA_FULL_ERROR;
			default:
				return IPC_SMS_ACK_UNSPEC_ERROR;
		}
	}
}

RIL_Errno ipc2ril_sms_ack_error(unsigned short error, int *error_code)
{
	// error_code is defined in See 3GPP 27.005, 3.2.5 for GSM/UMTS

	if (error_code == NULL)
		return RIL_E_GENERIC_FAILURE;

	switch(error) {
		case IPC_SMS_ACK_NO_ERROR:
			*error_code = -1;
			return RIL_E_SUCCESS;
		default:
			// unknown error
			*error_code = 500;
			return RIL_E_GENERIC_FAILURE;
	}
}

/*
 * Outgoing SMS functions
 */

int ril_request_send_sms_register(char *pdu, int pdu_length, unsigned char *smsc, int smsc_length, RIL_Token t)
{
	struct ril_request_send_sms_info *send_sms;
	struct list_head *list_end;
	struct list_head *list;

	send_sms = calloc(1, sizeof(struct ril_request_send_sms_info));
	if (send_sms == NULL)
		return -1;

	send_sms->pdu = pdu;
	send_sms->pdu_length = pdu_length;
	send_sms->smsc = smsc;
	send_sms->smsc_length = smsc_length;
	send_sms->token = t;

	list_end = ril_data.outgoing_sms;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) send_sms, list_end, NULL);

	if (ril_data.outgoing_sms == NULL)
		ril_data.outgoing_sms = list;

	return 0;
}

void ril_request_send_sms_unregister(struct ril_request_send_sms_info *send_sms)
{
	struct list_head *list;

	if (send_sms == NULL)
		return;

	list = ril_data.outgoing_sms;
	while (list != NULL) {
		if (list->data == (void *) send_sms) {
			memset(send_sms, 0, sizeof(struct ril_request_send_sms_info));
			free(send_sms);

			if (list == ril_data.outgoing_sms)
				ril_data.outgoing_sms = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct ril_request_send_sms_info *ril_request_send_sms_info_find(void)
{
	struct ril_request_send_sms_info *send_sms;
	struct list_head *list;

	list = ril_data.outgoing_sms;
	while (list != NULL) {
		send_sms = (struct ril_request_send_sms_info *) list->data;
		if (send_sms == NULL)
			goto list_continue;

		return send_sms;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct ril_request_send_sms_info *ril_request_send_sms_info_find_token(RIL_Token t)
{
	struct ril_request_send_sms_info *send_sms;
	struct list_head *list;

	list = ril_data.outgoing_sms;
	while (list != NULL) {
		send_sms = (struct ril_request_send_sms_info *) list->data;
		if (send_sms == NULL)
			goto list_continue;

		if (send_sms->token == t)
			return send_sms;

list_continue:
		list = list->next;
	}

	return NULL;
}

void ril_request_send_sms_info_clear(struct ril_request_send_sms_info *send_sms)
{
	if (send_sms == NULL)
		return;

	if (send_sms->pdu != NULL)
		free(send_sms->pdu);

	if (send_sms->smsc != NULL)
		free(send_sms->smsc);
}

void ril_request_send_sms_next(void)
{
	struct ril_request_send_sms_info *send_sms;
	RIL_Token t;
	char *pdu;
	int pdu_length;
	unsigned char *smsc;
	int smsc_length;
	int rc;

	ril_data.tokens.outgoing_sms = (RIL_Token) 0x00;

	send_sms = ril_request_send_sms_info_find();
	if (send_sms == NULL)
		return;

	t = send_sms->token;
	pdu = send_sms->pdu;
	pdu_length = send_sms->pdu_length;
	smsc = send_sms->smsc;
	smsc_length = send_sms->smsc_length;

	ril_request_send_sms_unregister(send_sms);

	if (pdu == NULL) {
		LOGE("SMS send request has no valid PDU");
		if (smsc != NULL)
			free(smsc);
		return;
	}

	ril_data.tokens.outgoing_sms = t;
	if (smsc == NULL) {
		// We first need to get SMS SVC before sending the message
		LOGD("We have no SMSC, let's ask one");

		rc = ril_request_send_sms_register(pdu, pdu_length, NULL, 0, t);
		if (rc < 0) {
			LOGE("Unable to add the request to the list");

			ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
			if (pdu != NULL)
				free(pdu);
			// Send the next SMS in the list
			ril_request_send_sms_next();
		}

		ipc_fmt_send_get(IPC_SMS_SVC_CENTER_ADDR, ril_request_get_id(t));
	} else {
		ril_request_send_sms_complete(t, pdu, pdu_length, smsc, smsc_length);
		if (pdu != NULL)
			free(pdu);
		if (smsc != NULL)
			free(smsc);
	}
}

void ril_request_send_sms_complete(RIL_Token t, char *pdu, int pdu_length, unsigned char *smsc, int smsc_length)
{
	struct ipc_sms_send_msg_request send_msg;
	unsigned char send_msg_type;

	unsigned char *pdu_hex;
	int pdu_hex_length;

	void *data;
	int length;

	unsigned char *p;

	if (pdu == NULL || pdu_length <= 0 || smsc == NULL || smsc_length <= 0)
		goto error;

	if ((pdu_length / 2 + smsc_length) > 0xfe) {
		LOGE("PDU or SMSC too large, aborting");
		goto error;
	}

	pdu_hex_length = pdu_length % 2 == 0 ? pdu_length / 2 :
		(pdu_length ^ 1) / 2;

	// Length of the final message
	length = sizeof(send_msg) + pdu_hex_length + smsc_length;

	LOGD("Sending SMS message (length: 0x%x)!", length);

	pdu_hex = calloc(1, pdu_hex_length);
	hex2bin(pdu, pdu_length, pdu_hex);
	send_msg_type = IPC_SMS_MSG_SINGLE;

	/* PDU operations */
	int pdu_tp_da_index = 2;
	unsigned char pdu_tp_da_len = pdu_hex[pdu_tp_da_index];

	if (pdu_tp_da_len > 0xff / 2) {
		LOGE("PDU TP-DA Len failed (0x%x)\n", pdu_tp_da_len);
		goto pdu_end;
	}

	LOGD("PDU TP-DA Len is 0x%x\n", pdu_tp_da_len);

	int pdu_tp_udh_index = pdu_tp_da_index + pdu_tp_da_len;
	unsigned char pdu_tp_udh_len = pdu_hex[pdu_tp_udh_index];

	if (pdu_tp_udh_len > 0xff / 2 || pdu_tp_udh_len < 5) {
		LOGE("PDU TP-UDH Len failed (0x%x)\n", pdu_tp_udh_len);
		goto pdu_end;
	}

	LOGD("PDU TP-UDH Len is 0x%x\n", pdu_tp_udh_len);

	int pdu_tp_udh_num_index = pdu_tp_udh_index + 4;
	unsigned char pdu_tp_udh_num = pdu_hex[pdu_tp_udh_num_index];

	if (pdu_tp_udh_num > 0xf) {
		LOGE("PDU TP-UDH Num failed (0x%x)\n", pdu_tp_udh_num);
		goto pdu_end;
	}

	int pdu_tp_udh_seq_index = pdu_tp_udh_index + 5;
	unsigned char pdu_tp_udh_seq = pdu_hex[pdu_tp_udh_seq_index];

	if (pdu_tp_udh_seq > 0xf || pdu_tp_udh_seq > pdu_tp_udh_num) {
		LOGE("PDU TP-UDH Seq failed (0x%x)\n", pdu_tp_udh_seq);
		goto pdu_end;
	}

	LOGD("We are sending message %d on %d\n", pdu_tp_udh_seq, pdu_tp_udh_num);

	if (pdu_tp_udh_num > 1) {
		LOGD("We are sending a multi-part message!");
		send_msg_type = IPC_SMS_MSG_MULTIPLE;
	}

pdu_end:
	// Alloc memory for the final message
	data = calloc(1, length);

	// Clear and fill the IPC structure part of the message
	memset(&send_msg, 0, sizeof(struct ipc_sms_send_msg_request));
	send_msg.type = IPC_SMS_TYPE_OUTGOING;
	send_msg.msg_type = send_msg_type;
	send_msg.length = (unsigned char) (pdu_hex_length + smsc_length + 1);
	send_msg.smsc_len = smsc_length;

	// Copy the parts of the message
	p = data;
	memcpy(p, &send_msg, sizeof(send_msg));
	p += sizeof(send_msg);
	memcpy(p, smsc, smsc_length);
	p += smsc_length;
	memcpy(p, pdu_hex, pdu_hex_length);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_SMS_SEND_MSG, ipc_sms_send_msg_complete);

	ipc_fmt_send(IPC_SMS_SEND_MSG, IPC_TYPE_EXEC, data, length, ril_request_get_id(t));

	free(pdu_hex);
	free(data);

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
	// Send the next SMS in the list
	ril_request_send_sms_next();
}

void ril_request_send_sms(RIL_Token t, void *data, size_t length)
{
	char *pdu = NULL;
	int pdu_length;
	unsigned char *smsc = NULL;
	int smsc_length;
	int rc;

	if (data == NULL || length < (int) (2 * sizeof(char *)))
		goto error;

	pdu = ((char **) data)[1];
	smsc = ((unsigned char **) data)[0];
	pdu_length = 0;
	smsc_length = 0;

	if (pdu != NULL) {
		pdu_length = strlen(pdu) + 1;
		pdu = strdup(pdu);
	}
	if (smsc != NULL) {
		smsc_length = strlen((char *) smsc);
		smsc = (unsigned char *) strdup((char *) smsc);
	}

	if (ril_data.tokens.outgoing_sms != (RIL_Token) 0x00) {
		LOGD("Another outgoing SMS is being processed, adding to the list");

		rc = ril_request_send_sms_register(pdu, pdu_length, smsc, smsc_length, t);
		if (rc < 0) {
			LOGE("Unable to add the request to the list");
			goto error;
		}

		return;
	}

	ril_data.tokens.outgoing_sms = t;
	if (smsc == NULL) {
		// We first need to get SMS SVC before sending the message
		LOGD("We have no SMSC, let's ask one");

		rc = ril_request_send_sms_register(pdu, pdu_length, NULL, 0, t);
		if (rc < 0) {
			LOGE("Unable to add the request to the list");
			goto error;
		}

		ipc_fmt_send_get(IPC_SMS_SVC_CENTER_ADDR, ril_request_get_id(t));
	} else {
		ril_request_send_sms_complete(t, pdu, pdu_length, smsc, smsc_length);
		if (pdu != NULL)
			free(pdu);
		if (smsc != NULL)
			free(smsc);
	}

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

	if (pdu != NULL)
		free(pdu);
	if (smsc != NULL)
		free(smsc);
	// Send the next SMS in the list
	ril_request_send_sms_next();
}

void ril_request_send_sms_expect_more(RIL_Token t, void *data, size_t length)
{
	// No particular treatment here, we already have a queue
	ril_request_send_sms(t, data, length);
}

void ipc_sms_svc_center_addr(struct ipc_message_info *info)
{
	struct ril_request_send_sms_info *send_sms;
	RIL_Token t;
	char *pdu;
	int pdu_length;
	unsigned char *smsc;
	int smsc_length;
	int rc;

	if (info == NULL || info->data == NULL || info->length < sizeof(unsigned char))
		goto error;

	send_sms = ril_request_send_sms_info_find_token(ril_request_get_token(info->aseq));
	if (send_sms == NULL || send_sms->pdu == NULL || send_sms->pdu_length <= 0) {
		LOGE("The request wasn't queued, reporting generic error!");

		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		ril_request_send_sms_info_clear(send_sms);
		ril_request_send_sms_unregister(send_sms);
		// Send the next SMS in the list
		ril_request_send_sms_next();

		return;
	}

	t = send_sms->token;
	pdu = send_sms->pdu;
	pdu_length = send_sms->pdu_length;
	smsc = (unsigned char *) info->data + sizeof(unsigned char);
	smsc_length = (int) ((unsigned char *) info->data)[0];

	LOGD("Got SMSC, completing the request");
	ril_request_send_sms_unregister(send_sms);
	ril_request_send_sms_complete(t, pdu, pdu_length, smsc, smsc_length);
	if (pdu != NULL)
		free(pdu);

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_sms_send_msg_complete(struct ipc_message_info *info)
{
	struct ril_request_send_sms_info *send_sms;
	struct ipc_gen_phone_res *phone_res;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	if (ipc_gen_phone_res_check(phone_res) < 0) {
		LOGE("IPC_GEN_PHONE_RES indicates error, abort request to RILJ");

		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
		// Send the next SMS in the list
		ril_request_send_sms_next();
	}
}

void ipc_sms_send_msg(struct ipc_message_info *info)
{
	struct ipc_sms_send_msg_response *report_msg;
	RIL_SMS_Response response;
	RIL_Errno e;

	if (info == NULL || info->data == NULL || info->length < sizeof(struct ipc_sms_send_msg_response))
		goto error;

	report_msg = (struct ipc_sms_send_msg_response *) info->data;

	LOGD("Got ACK for msg_tpid #%d\n", report_msg->msg_tpid);

	memset(&response, 0, sizeof(response));
	response.messageRef = report_msg->msg_tpid;
	response.ackPDU = NULL;

	e = ipc2ril_sms_ack_error(report_msg->error, &response.errorCode);

	ril_request_complete(ril_request_get_token(info->aseq), e, (void *) &response, sizeof(response));

	// Send the next SMS in the list
	ril_request_send_sms_next();

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

/*
 * Incoming SMS functions
 */

int ipc_sms_incoming_msg_register(char *pdu, int length, unsigned char type, unsigned char tpid)
{
	struct ipc_sms_incoming_msg_info *incoming_msg;
	struct list_head *list_end;
	struct list_head *list;

	incoming_msg = calloc(1, sizeof(struct ipc_sms_incoming_msg_info));
	if (incoming_msg == NULL)
		return -1;

	incoming_msg->pdu = pdu;
	incoming_msg->length = length;
	incoming_msg->type = type;
	incoming_msg->tpid = tpid;

	list_end = ril_data.incoming_sms;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) incoming_msg, list_end, NULL);

	if (ril_data.incoming_sms == NULL)
		ril_data.incoming_sms = list;

	return 0;
}

void ipc_sms_incoming_msg_unregister(struct ipc_sms_incoming_msg_info *incoming_msg)
{
	struct list_head *list;

	if (incoming_msg == NULL)
		return;

	list = ril_data.incoming_sms;
	while (list != NULL) {
		if (list->data == (void *) incoming_msg) {
			memset(incoming_msg, 0, sizeof(struct ipc_sms_incoming_msg_info));
			free(incoming_msg);

			if (list == ril_data.incoming_sms)
				ril_data.incoming_sms = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct ipc_sms_incoming_msg_info *ipc_sms_incoming_msg_info_find(void)
{
	struct ipc_sms_incoming_msg_info *incoming_msg;
	struct list_head *list;

	list = ril_data.incoming_sms;
	while (list != NULL) {
		incoming_msg = (struct ipc_sms_incoming_msg_info *) list->data;
		if (incoming_msg == NULL)
			goto list_continue;

		return incoming_msg;

list_continue:
		list = list->next;
	}

	return NULL;
}

void ipc_sms_incoming_msg_next(void)
{
	struct ipc_sms_incoming_msg_info *incoming_msg;

	ril_data.state.sms_incoming_msg_tpid = 0;

	incoming_msg = ipc_sms_incoming_msg_info_find();
	if (incoming_msg == NULL)
		return;

	ipc_sms_incoming_msg_complete(incoming_msg->pdu, incoming_msg->length, incoming_msg->type, incoming_msg->tpid);
	ipc_sms_incoming_msg_unregister(incoming_msg);
}

void ipc_sms_incoming_msg_complete(char *pdu, int length, unsigned char type, unsigned char tpid)
{
	if (pdu == NULL || length <= 0)
		return;

	ril_data.state.sms_incoming_msg_tpid = tpid;

	if (type == IPC_SMS_TYPE_POINT_TO_POINT) {
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NEW_SMS, pdu, length);
	} else if (type == IPC_SMS_TYPE_STATUS_REPORT) {
		ril_request_unsolicited(RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT, pdu, length);
	} else {
		LOGE("Unhandled message type: %x", type);
	}

	free(pdu);
}

void ipc_sms_incoming_msg(struct ipc_message_info *info)
{
	struct ipc_sms_incoming_msg *msg;
	unsigned char *pdu_hex;
	char *pdu;
	int length;
	int rc;

	if (info == NULL || info->data == NULL || info->length < sizeof(struct ipc_sms_incoming_msg))
		goto error;

	msg = (struct ipc_sms_incoming_msg *) info->data;
	pdu_hex = ((unsigned char *) info->data + sizeof(struct ipc_sms_incoming_msg));

	length = msg->length * 2 + 1;
	pdu = (char *) calloc(1, length);

	bin2hex(pdu_hex, msg->length, pdu);

	if (ril_data.state.sms_incoming_msg_tpid != 0) {
		LOGD("Another message is waiting ACK, queuing");
		rc = ipc_sms_incoming_msg_register(pdu, length, msg->type, msg->msg_tpid);
		if (rc < 0)
			LOGE("Unable to register incoming msg");

		return;
	}

	ipc_sms_incoming_msg_complete(pdu, length, msg->type, msg->msg_tpid);

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_sms_acknowledge(RIL_Token t, void *data, size_t length)
{
	struct ipc_sms_deliver_report_request report_msg;
	int success, fail_cause;

	if (data == NULL || length < 2 * sizeof(int))
		goto error;

	success = ((int *) data)[0];
	fail_cause = ((int *) data)[1];

	if (ril_data.state.sms_incoming_msg_tpid == 0) {
		LOGE("There is no SMS message to ACK!");
		goto error;
	}

	report_msg.type = IPC_SMS_TYPE_STATUS_REPORT;
	report_msg.error = ril2ipc_sms_ack_error(success, fail_cause);
	report_msg.msg_tpid = ril_data.state.sms_incoming_msg_tpid;
	report_msg.unk = 0;

	ipc_gen_phone_res_expect_to_abort(ril_request_get_id(t), IPC_SMS_DELIVER_REPORT);

	ipc_fmt_send(IPC_SMS_DELIVER_REPORT, IPC_TYPE_EXEC, (void *) &report_msg, sizeof(report_msg), ril_request_get_id(t));

	ipc_sms_incoming_msg_next();

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);

	ipc_sms_incoming_msg_next();
}

void ipc_sms_deliver_report(struct ipc_message_info *info)
{
	struct ipc_sms_deliver_report_response *report_msg;
	RIL_Errno e;
	int error_code;

	if (info == NULL || info->data == NULL || info->length < sizeof(struct ipc_sms_deliver_report_response))
		goto error;

	report_msg = (struct ipc_sms_deliver_report_response *) info->data;
	e = ipc2ril_sms_ack_error(report_msg->error, &error_code);

	ril_request_complete(ril_request_get_token(info->aseq), e, NULL, 0);

	return;

error:
	if (info != NULL)
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

/*
 * Apparently non-SMS-messages-related function
 */

void ipc_sms_device_ready(struct ipc_message_info *info)
{
#if RIL_VERSION >= 7
	if (ril_data.state.radio_state == RADIO_STATE_ON) {
#else
	if (ril_data.state.radio_state == RADIO_STATE_SIM_READY) {
#endif
		ipc_fmt_send(IPC_SMS_DEVICE_READY, IPC_TYPE_SET, NULL, 0, info->aseq);
	}

	ril_tokens_check();
}
