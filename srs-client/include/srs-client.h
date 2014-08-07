/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
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

#ifndef _SRS_CLIENT_H_
#define _SRS_CLIENT_H_

#include <stdlib.h>
#include <pthread.h>

#include <samsung-ril-socket.h>

#define SRS_CLIENT_LOCK(client)			pthread_mutex_lock(&client->mutex)
#define SRS_CLIENT_UNLOCK(client)		pthread_mutex_unlock(&client->mutex)

struct srs_client {
	int fd;
	int event_fd;

	pthread_mutex_t mutex;
	pthread_t thread;

	void (*callback)(struct srs_client *client, struct srs_message *message);
};

const char *srs_command_string(unsigned short command);
int srs_header_setup(struct srs_header *header,
	const struct srs_message *message);
int srs_message_setup(const struct srs_header *header,
	struct srs_message *message);

int srs_ping(struct srs_client *client);

struct srs_client *srs_client_create(void);
int srs_client_destroy(struct srs_client *client);
int srs_client_open(struct srs_client *client);
int srs_client_close(struct srs_client *client);
int srs_client_poll(struct srs_client *client);
int srs_client_send(struct srs_client *client, unsigned short command,
	void *data, size_t size);
int srs_client_recv(struct srs_client *client, struct srs_message *message);
int srs_client_loop(struct srs_client *client);
int srs_client_loop_start(struct srs_client *client,
	void (*callback)(struct srs_client *client, struct srs_message *message));
int srs_client_loop_stop(struct srs_client *client);

#endif
