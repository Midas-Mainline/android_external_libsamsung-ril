/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2010-2011 Joerie de Gram <j.de.gram@gmail.com>
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define LOG_TAG "RIL-UTIL"
#include <utils/Log.h>
#include "util.h"

#include "samsung-ril.h"

/*
 * List
 */

struct list_head *list_head_alloc(void *data, struct list_head *prev, struct list_head *next)
{
	struct list_head *list;

	list = calloc(1, sizeof(struct list_head));
	if (list == NULL)
		return NULL;

	list->data = data;
	list->prev = prev;
	list->next = next;

	if (prev != NULL)
		prev->next = list;
	if (next != NULL)
		next->prev = list;

	return list;
}

void list_head_free(struct list_head *list)
{
	if (list == NULL)
		return;

	if (list->next != NULL)
		list->next->prev = list->prev;
	if (list->prev != NULL)
		list->prev->next = list->next;

	memset(list, 0, sizeof(struct list_head));
	free(list);
}

/*
 * Converts a hexidecimal string to binary
 */
void hex2bin(const char *data, int length, unsigned char *buf)
{
	int i = 0;
	char b = 0;
	unsigned char *p = buf;

	length ^= 0x01;

	while (i < length) {
		b = 0;

		if (data[i] - '0' < 10)
			b = data[i] - '0';
		else if (data[i] - 'a' < 7)
			b = data[i] - 'a' + 10;
		else if (data[i] - 'A' < 7)
			b = data[i] - 'A' + 10;
		i++;

		b = (b << 4);

		if (data[i] - '0' < 10)
			b |= data[i] - '0';
		else if (data[i] - 'a' < 7)
			b |= data[i] - 'a' + 10;
		else if (data[i] - 'A' < 7)
			b |= data[i] - 'A' + 10;
		i++;

		*p++ = b;
	}
}

/*
 * Converts binary data to a hexidecimal string
 */
void bin2hex(const unsigned char *data, int length, char *buf)
{
	int i;
	char b;
	char *p = buf;

	for (i = 0; i < length; i++) {
		b = 0;

		b = (data[i] >> 4 & 0x0f);
		b += (b < 10) ? '0' : ('a' - 10);
		*p++ = b;

		b = (data[i] & 0x0f);
		b += (b < 10) ? '0' : ('a' - 10);
		*p++ = b;
	}

	*p = '\0';
}

/*
 * Converts GSM7 (8 bits) data to ASCII (7 bits)
 */
int gsm72ascii(unsigned char *data, char **data_dec, int length)
{
	int t, u, d, o = 0;
	int i;

	int dec_length;
	char *dec;

	dec_length = ((length * 8) - ((length * 8) % 7) ) / 7;
	dec = malloc(dec_length);

	memset(dec, 0, dec_length);

	for (i = 0 ; i < length ; i++)
	{
		d = 7 - i % 7;
		if (d == 7 && i != 0)
			o++;

		t = (data[i] - (((data[i] >> d) & 0xff) << d));
		u = (data[i] >> d) & 0xff;

		dec[i+o]+=t << (i + o) % 8;

		if (u)
			dec[i+1+o]+=u;
	}

	*data_dec = dec;

	return dec_length;
}

/*
 * Converts ASCII (7 bits) data to GSM7 (8 bits)
 */
int ascii2gsm7_ussd(char *data, unsigned char **data_enc, int length)
{
	int d_off, d_pos, a_off, a_pos = 0;
	int i;

	int enc_length;
	unsigned char *enc;

	enc_length = ((length * 7) - (length * 7) % 8) / 8;
	enc_length += (length * 7) % 8 > 0 ? 1 : 0;

	//FIXME: why does samsung does that?
	enc_length++;

	enc = malloc(enc_length);
	memset(enc, 0, enc_length);

	for (i = 0 ; i < length ; i++)
	{
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

	//FIXME: what is going on here?
	enc[enc_length - 2] |= 0x30;
	enc[enc_length - 1] = 0x02;

	return enc_length;
}

size_t ascii2gsm7(char *ascii, unsigned char *gsm7)
{
	int ascii_length;
	int gsm7_length;
	int offset;

	unsigned char *p;
	int i;

	if (ascii == NULL)
		return -1;

	ascii_length = strlen(ascii);

	gsm7_length = ((ascii_length * 7) - (ascii_length * 7) % 8) / 8;
	gsm7_length = (ascii_length * 7) % 8 > 0 ? gsm7_length + 1 : gsm7_length;

	if (gsm7 == NULL)
		return gsm7_length;

	memset(gsm7, 0, gsm7_length);

	offset = 0;
	p = gsm7;

	for (i = 0; i < ascii_length; i++) {
		*p |= ((ascii[i] & 0x7f) >> offset) & 0xff;

		if (offset) {
			p--;
			*p |= ((ascii[i] & ((1 << (offset + 1)) - 1)) << (8 - offset)) & 0xff;
			p++;
		}

		if (offset < 7)
			p++;

		offset++;
		offset %= 8;
	}

	return gsm7_length;
}

void hex_dump(void *data, int size)
{
	/* dumps size bytes of *data to stdout. Looks like:
	 * [0000] 75 6E 6B 6E 6F 77 6E 20
	 *				  30 FF 00 00 00 00 39 00 unknown 0.....9.
	 * (in a single line of course)
	 */

	unsigned char *p = data;
	unsigned char c;
	int n;
	char bytestr[4] = {0};
	char addrstr[10] = {0};
	char hexstr[ 16*3 + 5] = {0};
	char charstr[16*1 + 5] = {0};
	for (n=1;n<=size;n++) {
		if (n%16 == 1) {
			/* store address for this line */
			snprintf(addrstr, sizeof(addrstr), "%.4x",
			   ((unsigned int)p-(unsigned int)data) );
		}

		c = *p;
		if (isalnum(c) == 0) {
			c = '.';
		}

		/* store hex str (for left side) */
		snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
		strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

		/* store char str (for right side) */
		snprintf(bytestr, sizeof(bytestr), "%c", c);
		strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

		if (n%16 == 0) {
			/* line completed */
			LOGD("[%4.4s]   %-50.50s  %s", addrstr, hexstr, charstr);
			hexstr[0] = 0;
			charstr[0] = 0;
		} else if (n%8 == 0) {
			/* half line: add whitespaces */
			strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
			strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
		}
		p++; /* next byte */
	}

	if (strlen(hexstr) > 0) {
		/* print rest of buffer if not empty */
		LOGD("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
	}
}

/* writes the utf8 character encoded in v
 * to the buffer utf8 at the specified offset
 */
int utf8_write(char *utf8, int offset, int v)
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

SmsCodingScheme sms_get_coding_scheme(int dataCoding)
{
	switch (dataCoding >> 4) {
	case 0x00:
	case 0x02:
	case 0x03:
		return SMS_CODING_SCHEME_GSM7;
	case 0x01:
		if (dataCoding == 0x10)
			return SMS_CODING_SCHEME_GSM7;
		if (dataCoding == 0x11)
			return SMS_CODING_SCHEME_UCS2;
		break;
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		if (dataCoding & 0x20)
			return SMS_CODING_SCHEME_UNKNOWN;
		if (((dataCoding >> 2) & 3) == 0)
			return SMS_CODING_SCHEME_GSM7;
		if (((dataCoding >> 2) & 3) == 2)
			return SMS_CODING_SCHEME_UCS2;
		break;
	case 0xF:
		if (!(dataCoding & 4))
			return SMS_CODING_SCHEME_GSM7;
		break;
	}
	return SMS_CODING_SCHEME_UNKNOWN;
}

char *pdu_create(char *number, char *message)
{
	unsigned char pdu_first[] = { 0x00, 0x04 };
	unsigned char pdu_toa[] = { 0x91 };
	unsigned char pdu_tp[] = { 0x00, 0x00 };
	unsigned char timestamp[7] = { 0 };
	time_t t;
	struct tm *tm;

	unsigned char number_length;
	unsigned char message_length;

	unsigned char *buffer = NULL;
	char *pdu = NULL;
	size_t length = 0;

	unsigned char *p;
	unsigned char a;
	char c;
	int i;

	if (number == NULL || message == NULL || strlen(message) > 0xff)
		return NULL;

	number_length = strlen(number) & 0xff;
	if (number_length % 2 != 0)
		number_length++;
	number_length /= 2;

	message_length = ascii2gsm7(message, NULL) & 0xff;

	length = sizeof(pdu_first) + sizeof(number_length) + sizeof(pdu_toa) + number_length + sizeof(pdu_tp) + sizeof(timestamp) + sizeof(message_length) + message_length;
	buffer = calloc(1, length);

	p = (unsigned char *) buffer;

	memcpy(p, &pdu_first, sizeof(pdu_first));
	p += sizeof(pdu_first);

	number_length = strlen(number) & 0xff;

	memcpy(p, &number_length, sizeof(number_length));
	p += sizeof(number_length);
	memcpy(p, &pdu_toa, sizeof(pdu_toa));
	p += sizeof(pdu_toa);

	i = 0;
	while (i < number_length) {
		c = number[i++];

		if (isdigit(c))
			*p = (c - '0') & 0x0f;

		if (i < number_length) {
			c = number[i++];
			if (isdigit(c))
				*p |= ((c - '0') & 0x0f) << 4;
		} else {
			*p |= 0xf << 4;
		}

		p++;
	}

	memcpy(p, &pdu_tp, sizeof(pdu_tp));
	p += sizeof(pdu_tp);

	t = time(NULL);
	tm = localtime(&t);

	a = (tm->tm_year - 100);
	timestamp[0] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = (tm->tm_mon + 1);
	timestamp[1] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = tm->tm_mday;
	timestamp[2] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = tm->tm_hour;
	timestamp[3] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = tm->tm_min;
	timestamp[4] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = tm->tm_sec;
	timestamp[5] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);
	a = (unsigned char) (-timezone / 900);
	timestamp[6] = ((a - (a % 10)) / 10) | ((a % 10) * 0x10);

	memcpy(p, &timestamp, sizeof(timestamp));
	p += sizeof(timestamp);

	message_length = strlen(message) & 0xff;

	memcpy(p, &message_length, sizeof(message_length));
	p += sizeof(message_length);

	ascii2gsm7(message, p);
	p += message_length;

	pdu = (char *) calloc(1, length * 2 + 1);

	bin2hex(buffer, length, pdu);

	free(buffer);

	return pdu;
}
