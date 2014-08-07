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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <sys/eventfd.h>

struct list_head {
	struct list_head *prev;
	struct list_head *next;
	const void *data;
};

struct list_head *list_head_alloc(struct list_head *prev, struct list_head *next,
	const void *data);
void list_head_free(struct list_head *list);
int data_dump(const void *data, size_t size);
int strings_array_free(char **array, size_t size);
int eventfd_flush(int fd);
int eventfd_recv(int fd, eventfd_t *event);
int eventfd_send(int fd, eventfd_t event);

#endif
