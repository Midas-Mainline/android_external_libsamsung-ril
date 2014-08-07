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
#include <string.h>
#include <ctype.h>
#include <sys/eventfd.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include <samsung-ril.h>
#include <utils.h>

struct list_head *list_head_alloc(struct list_head *prev, struct list_head *next,
	const void *data)
{
	struct list_head *list;

	list = calloc(1, sizeof(struct list_head));
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

int data_dump(const void *data, size_t size)
{
	unsigned int cols = 8;
	unsigned int cols_count = 2;
	int spacer;
	char string[81];
	size_t length;
	char *print;
	unsigned char *p;
	unsigned int offset;
	unsigned int rollback;
	unsigned int i, j, k;
	int rc;

	if (data == NULL || size == 0)
		return -1;

	// spacer = string length - offset print length - data print length - ascii print length
	spacer = (sizeof(string) - 1) - 6 - (3 * cols * cols_count - 1 + (cols_count - 1)) - (cols * cols_count + cols_count - 1);

	// Need 3 spacers
	spacer /= 3;

	if (spacer <= 0)
		return -1;

	p = (unsigned char *) data;
	offset = 0;

	while (offset < size) {
		rollback = 0;

		print = (char *) &string;
		length = sizeof(string);

		// Offset print

		rc = snprintf(print, length, "[%04x]", offset);
		print += rc;
		length -= rc;

		// Spacer print

		for (i = 0; i < (unsigned int) spacer; i++) {
			*print++ = ' ';
			length--;
		}

		// Data print

		for (i = 0; i < cols_count; i++) {
			for (j = 0; j < cols; j++) {
				if (offset < size) {
					rc = snprintf(print, length, "%02X", *p);
					print += rc;
					length -= rc;

					p++;
					offset++;
					rollback++;
				} else {
					for (k = 0; k < 2; k++) {
						*print++ = ' ';
						length--;
					}
				}

				if (j != (cols - 1)) {
					*print++ = ' ';
					length--;
				}
			}

			if (i != (cols_count - 1)) {
				for (k = 0; k < 2; k++) {
					*print++ = ' ';
					length--;
				}
			}
		}

		// Spacer print

		for (i = 0; i < (unsigned int) spacer; i++) {
			*print++ = ' ';
			length--;
		}

		// ASCII print

		p -= rollback;
		offset -= rollback;

		for (i = 0; i < cols_count; i++) {
			for (j = 0; j < cols; j++) {
				if (offset < size) {
					if (isascii(*p) && isprint(*p))
						*print = *p;
					else
						*print = '.';

					print++;
					length--;

					p++;
					offset++;
					rollback++;
				}
			}

			if (i != (cols_count - 1) && offset < size) {
				*print++ = ' ';
				length--;
			}
		}

		*print = '\0';

		RIL_LOGD("%s", string);
	}

	return 0;
}

int strings_array_free(char **array, size_t size)
{
	unsigned int count;
	unsigned int i;

	if (array == NULL)
		return -1;

	if (size == 0) {
		for (i = 0; array[i] != NULL; i++)
			free(array[i]);
	} else {
		count = size / sizeof(char *);
		if (count == 0)
			return -1;

		for (i = 0; i < count; i++) {
			if (array[i] != NULL)
				free(array[i]);
		}
	}

	return 0;
}

int eventfd_flush(int fd)
{
	eventfd_t flush;
	int rc;

	rc = eventfd_read(fd, &flush);
	if (rc < 0)
		return -1;

	return 0;
}

int eventfd_recv(int fd, eventfd_t *event)
{
	int rc;

	rc = eventfd_read(fd, event);
	if (rc < 0)
		return -1;

	return 0;
}

int eventfd_send(int fd, eventfd_t event)
{
	int rc;

	eventfd_flush(fd);

	rc = eventfd_write(fd, event);
	if (rc < 0)
		return -1;

	return 0;
}
