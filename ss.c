/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2017 Wolfgang Wiedmeyer <wolfgit@wiedmeyer.de>
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

#define LOG_TAG "RIL-SS"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>

int ipc_ss_ussd_callback(struct ipc_message *message)
{
	struct ipc_gen_phone_res_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_gen_phone_res_data))
		return -1;

	data = (struct ipc_gen_phone_res_data *) message->data;

	rc = ipc_gen_phone_res_check(data);
	if (rc < 0) {
		RIL_LOGE("There was an error, aborting USSD request");
		goto error;
	}

	RIL_LOGD("USSD callback code 0x%04x", data->code);

	// catch error codes if no IPC_SS_USSD notification is sent
	if ((data->code & 0xff) == 0x32 || (data->code & 0xff) == 0x24)
		goto error;

	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_SUCCESS, NULL, 0);
	goto complete;

error:
	ril_request_data_free(RIL_REQUEST_SEND_USSD);
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}

/*
 * Converts ASCII (7 bits) data to GSM7 (8 bits)
 */
int ril2ipc_ascii2gsm7(char *data, unsigned char **data_enc, int length)
{
	int d_off, d_pos, a_off, a_pos = 0;
	int i;
	int enc_length;
	unsigned char *enc;

	enc_length = ((length * 7) - (length * 7) % 8) / 8;
	enc_length += (length * 7) % 8 > 0 ? 1 : 0;

	// FIXME: why does Samsung do this?
	enc_length++;

	enc = malloc(enc_length);
	memset(enc, 0, enc_length);

	for (i = 0 ; i < length ; i++) {
		// offset from the right of data to keep
		d_off = i % 8;

		// position of the data we keep
		d_pos = ((i * 7) - (i * 7) % 8) / 8;
		d_pos += (i * 7) % 8 > 0 ? 1 : 0;

		// adding the data with correct offset
		enc[d_pos] |= data[i] >> d_off;

		// numbers of bits to omit to get data to add another place
		a_off = 8 - d_off;
		// position (on the encoded feed) of the data to add
		a_pos = d_pos - 1;

		// adding the data to add at the correct position
		enc[a_pos] |= data[i] << a_off;
	}

	*data_enc = enc;

	// FIXME: what is going on here?
	enc[enc_length - 2] |= 0x30;
	enc[enc_length - 1] = 0x02;

	return enc_length;
}

int ril_request_send_ussd(void *data, size_t size, RIL_Token token)
{
	char *data_enc = NULL;
	int data_enc_len = 0;
	char *message = NULL;
	struct ipc_ss_ussd_header *ussd = NULL;
	int message_size = 0xc0;
	void *ussd_state_data;
	size_t ussd_state_size;
	unsigned char ussd_state = 0;
	struct ril_request *request;
	int rc;

	if (data == NULL || size < sizeof(char *))
		goto error;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	request = ril_request_find_request_status(RIL_REQUEST_SEND_USSD, RIL_REQUEST_HANDLED);
	if (request != NULL) {
		return RIL_REQUEST_UNHANDLED;
	}

	ussd_state_size = ril_request_data_size_get(RIL_REQUEST_SEND_USSD);
	ussd_state_data = ril_request_data_get(RIL_REQUEST_SEND_USSD);

	if (ussd_state_data != NULL && ussd_state_size > 0) {
		ussd_state = *((unsigned char *) ussd_state_data);
		free(ussd_state_data);
	}

	switch (ussd_state) {
		case 0:
		case IPC_SS_USSD_NO_ACTION_REQUIRE:
		case IPC_SS_USSD_TERMINATED_BY_NET:
		case IPC_SS_USSD_OTHER_CLIENT:
		case IPC_SS_USSD_NOT_SUPPORT:
		case IPC_SS_USSD_TIME_OUT:
			RIL_LOGD("USSD Tx encoding is GSM7");

			data_enc_len = ril2ipc_ascii2gsm7(data, (unsigned char**)&data_enc, (int) size);
			if (data_enc_len > message_size) {
				RIL_LOGE("USSD message size is too long, aborting");
				goto error;
			}

			message = malloc(message_size);
			memset(message, 0, message_size);

			ussd = (struct ipc_ss_ussd_header *) message;
			ussd->state = IPC_SS_USSD_NO_ACTION_REQUIRE;
			ussd->dcs = 0x0f; // GSM7 in that case
			ussd->length = data_enc_len;

			memcpy((void *) (message + sizeof(struct ipc_ss_ussd_header)), data_enc, data_enc_len);

			break;
		case IPC_SS_USSD_ACTION_REQUIRE:
		default:
			RIL_LOGD("USSD Tx encoding is ASCII");

			data_enc_len = asprintf(&data_enc, "%s", (char*)data);

			if (data_enc_len > message_size) {
				RIL_LOGE("USSD message size is too long, aborting");
				goto error;
			}

			message = malloc(message_size);
			memset(message, 0, message_size);

			ussd = (struct ipc_ss_ussd_header *) message;
			ussd->state = IPC_SS_USSD_ACTION_REQUIRE;
			ussd->dcs = 0x0f; // ASCII in that case
			ussd->length = data_enc_len;

			memcpy((void *) (message + sizeof(struct ipc_ss_ussd_header)), data_enc, data_enc_len);

			break;
	}

	if (message == NULL) {
		RIL_LOGE("USSD message is empty, aborting");
		goto error;
	}

	ipc_gen_phone_res_expect_callback(ipc_fmt_request_seq(token), IPC_SS_USSD,
		ipc_ss_ussd_callback);

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SS_USSD, IPC_TYPE_EXEC, (void *) message, message_size);
	if (rc < 0)
		goto error;

	rc = RIL_REQUEST_HANDLED;
	goto complete;

error:
	ril_request_complete(token, RIL_E_GENERIC_FAILURE, NULL, 0);
	rc = RIL_REQUEST_COMPLETED;

complete:
	if (data_enc != NULL && data_enc_len > 0)
		free(data_enc);

	return rc;
}

int ril_request_cancel_ussd(void *data, size_t size, RIL_Token token)
{
	struct ipc_ss_ussd_header ussd;
	int rc;

	rc = ril_radio_state_check(RADIO_STATE_SIM_NOT_READY);
	if (rc < 0)
		return RIL_REQUEST_UNHANDLED;

	memset(&ussd, 0, sizeof(ussd));

	ussd.state = IPC_SS_USSD_TERMINATED_BY_NET;

	rc = ipc_gen_phone_res_expect_complete(ipc_fmt_request_seq(token), IPC_SS_USSD);
	if (rc < 0)
		goto error;

	rc = ipc_fmt_send(ipc_fmt_request_seq(token), IPC_SS_USSD, IPC_TYPE_EXEC, (void *) &ussd, sizeof(ussd));
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

int ipc2ril_ussd_state(struct ipc_ss_ussd_header *ussd, char *message[2])
{
	if (ussd == NULL || message == NULL)
		return -1;

	switch (ussd->state) {
		case IPC_SS_USSD_NO_ACTION_REQUIRE:
			asprintf(&message[0], "%d", 0);
			break;
		case IPC_SS_USSD_ACTION_REQUIRE:
			asprintf(&message[0], "%d", 1);
			break;
		case IPC_SS_USSD_TERMINATED_BY_NET:
			asprintf(&message[0], "%d", 2);
			break;
		case IPC_SS_USSD_OTHER_CLIENT:
			asprintf(&message[0], "%d", 3);
			break;
		case IPC_SS_USSD_NOT_SUPPORT:
			asprintf(&message[0], "%d", 4);
			break;
		case IPC_SS_USSD_TIME_OUT:
			asprintf(&message[0], "%d", 5);
			break;
	}

	return 0;
}

int ipc2ril_ussd_encoding(int data_encoding)
{
	switch (data_encoding >> 4) {
	case 0x00:
	case 0x02:
	case 0x03:
		return USSD_ENCODING_GSM7;
	case 0x01:
		if (data_encoding == 0x10)
			return USSD_ENCODING_GSM7;
		if (data_encoding == 0x11)
			return USSD_ENCODING_UCS2;
		break;
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		if (data_encoding & 0x20)
			return USSD_ENCODING_UNKNOWN;
		if (((data_encoding >> 2) & 3) == 0)
			return USSD_ENCODING_GSM7;
		if (((data_encoding >> 2) & 3) == 2)
			return USSD_ENCODING_UCS2;
		break;
	case 0xF:
		if (!(data_encoding & 4))
			return USSD_ENCODING_GSM7;
		break;
	}

	return USSD_ENCODING_UNKNOWN;
}

/* writes the utf8 character encoded in v
 * to the buffer utf8 at the specified offset
 */
int ipc2ril_utf8_write(char *utf8, int offset, int v)
{
	int result;

	if (v < 0x80) {
		result = 1;
		if (utf8)
			utf8[offset] = (char)v;
	} else if (v < 0x800) {
		result = 2;
		if (utf8) {
			utf8[offset + 0] = (char)(0xc0 | (v >> 6));
			utf8[offset + 1] = (char)(0x80 | (v & 0x3f));
		}
	} else if (v < 0x10000) {
		result = 3;
		if (utf8) {
			utf8[offset + 0] = (char)(0xe0 | (v >> 12));
			utf8[offset + 1] = (char)(0x80 | ((v >> 6) & 0x3f));
			utf8[offset + 2] = (char)(0x80 | (v & 0x3f));
		}
	} else {
		result = 4;
		if (utf8) {
			utf8[offset + 0] = (char)(0xf0 | ((v >> 18) & 0x7));
			utf8[offset + 1] = (char)(0x80 | ((v >> 12) & 0x3f));
			utf8[offset + 2] = (char)(0x80 | ((v >> 6) & 0x3f));
			utf8[offset + 3] = (char)(0x80 | (v & 0x3f));
		}
	}

	return result;
}

#define  GSM_7BITS_ESCAPE   0x1b

/* For each gsm7 code value, this table gives the equivalent
 * UTF-8 code point.
 */
unsigned short gsm7bits_to_unicode[128] = {
  '@', 0xa3,  '$', 0xa5, 0xe8, 0xe9, 0xf9, 0xec, 0xf2, 0xc7, '\n', 0xd8, 0xf8, '\r', 0xc5, 0xe5,
0x394,  '_',0x3a6,0x393,0x39b,0x3a9,0x3a0,0x3a8,0x3a3,0x398,0x39e,    0, 0xc6, 0xe6, 0xdf, 0xc9,
  ' ',  '!',  '"',  '#', 0xa4,  '%',  '&', '\'',  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
 0xa1,  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z', 0xc4, 0xd6,0x147, 0xdc, 0xa7,
 0xbf,  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z', 0xe4, 0xf6, 0xf1, 0xfc, 0xe0,
};

/* For each gsm7 extended code value, this table gives the equivalent
 * UTF-8 code point.
 */
unsigned short gsm7bits_extend_to_unicode[128] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,'\f',   0,   0,   0,   0,   0,
    0,   0,   0,   0, '^',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0, '{', '}',   0,   0,   0,   0,   0,'\\',
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '[', '~', ']',   0,
  '|',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,0x20ac, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

int ipc2ril_utf8_from_gsm7(unsigned char *data, char **utf8, int length)
{
	int septet_offset = 0;
	int shift = (septet_offset & 7);
	int septet_count = 0, escaped = 0, result = 0;
	unsigned char *src = data;

	src += (septet_offset >> 3);
	septet_count = (length * 8) / 7; // round down

	for (; septet_count > 0; septet_count--) {
		int c = (src[0] >> shift) & 0x7f;
		int v;
		if (shift > 1) {
			c = ((src[1] << (8-shift)) | c) & 0x7f;
		}
		if (escaped) {
			v = gsm7bits_extend_to_unicode[c];
		} else if (c == GSM_7BITS_ESCAPE) {
			escaped = 1;
			goto NextSeptet;
		} else {
			v = gsm7bits_to_unicode[c];
		}
		result += ipc2ril_utf8_write(*utf8, result, v);

	NextSeptet:
		shift += 7;
		if (shift >= 8) {
			shift -= 8;
			src += 1;
		}
	}

	return  result;
}

int ipc_ss_ussd(struct ipc_message *message)
{
	char *data_dec = NULL;
	int data_dec_len = 0;
	char *ussd_message[2];
	int ussd_encoding;
	struct ipc_ss_ussd_header *ussd = NULL;
	unsigned char state;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct ipc_ss_ussd_header))
		goto error;

	memset(ussd_message, 0, sizeof(ussd_message));

	ussd = (struct ipc_ss_ussd_header *) message->data;

	rc = ipc2ril_ussd_state(ussd, ussd_message);
	if (rc < 0)
		goto error;

	ril_request_data_set_uniq(RIL_REQUEST_SEND_USSD, (void *) &ussd->state, sizeof(unsigned char));

	if (ussd->length > 0 && message->size > 0 && message->data != NULL) {
		ussd_encoding = ipc2ril_ussd_encoding(ussd->dcs);
		switch (ussd_encoding) {
			case USSD_ENCODING_GSM7:
				RIL_LOGD("USSD Rx encoding is GSM7");

				data_dec_len = ipc2ril_utf8_from_gsm7((unsigned char *) message->data + sizeof(struct ipc_ss_ussd_header),
									&data_dec, message->size - sizeof(struct ipc_ss_ussd_header));

				data_dec = malloc(data_dec_len+1);
				memset(data_dec, 0, data_dec_len);

				ipc2ril_utf8_from_gsm7((unsigned char *) message->data + sizeof(struct ipc_ss_ussd_header),
							&data_dec, message->size - sizeof(struct ipc_ss_ussd_header));
				asprintf(&ussd_message[1], "%s", data_dec);
				ussd_message[1][data_dec_len] = '\0';
				break;
			case USSD_ENCODING_UCS2:
				RIL_LOGD("USSD Rx encoding %x is UCS2", ussd->dcs);

				data_dec_len = message->size - sizeof(struct ipc_ss_ussd_header);
				ussd_message[1] = malloc(data_dec_len * 4 + 1);

				int i, result = 0;
				char *ucs2 = (char*)message->data + sizeof(struct ipc_ss_ussd_header);
				for (i = 0; i < data_dec_len; i += 2) {
					int c = (ucs2[i] << 8) | ucs2[1 + i];
					result += ipc2ril_utf8_write(ussd_message[1], result, c);
				}
				ussd_message[1][result] = '\0';
				break;
			default:
				RIL_LOGD("USSD Rx encoding %x is unknown, assuming ASCII",
					ussd->dcs);

				data_dec_len = message->size - sizeof(struct ipc_ss_ussd_header);
				asprintf(&ussd_message[1], "%s", (unsigned char *) message->data + sizeof(struct ipc_ss_ussd_header));
				ussd_message[1][data_dec_len] = '\0';
				break;
		}
	}

	ril_request_unsolicited(RIL_UNSOL_ON_USSD, ussd_message, sizeof(ussd_message));
	goto complete;

error:
	ril_request_complete(ipc_fmt_request_token(message->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);

complete:
	return 0;
}
