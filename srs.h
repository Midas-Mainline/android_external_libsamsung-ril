/*
 * This file is part of Samsung-RIL.
 *
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

#ifndef _SRS_H_
#define _SRS_H_

#include <stdlib.h>

#include <samsung-ril.h>

/*
 * Values
 */

#define SRS_CLIENT_CLOSE					0x00
#define SRS_SERVER_CLOSE					0x01
#define SRS_CLIENT_CHANGE					0x02
#define SRS_CLIENT_IO_ERROR					0x03

/*
 * Structures
 */

struct srs_client {
	int fd;
};

struct srs_data {
	int server_fd;
	int client_fd;

	int server_event_fd;
	int event_fd;

	pthread_t server_thread;

	struct list_head *clients;
};

struct srs_dispatch_handler {
	unsigned short command;
	int (*handler)(struct srs_message *message);
};

/*
 * Helpers
 */

/* Utils */
const char *srs_command_string(unsigned short command);
void srs_log_send(struct ril_client *client, struct srs_message *message);
void srs_log_recv(struct ril_client *client, struct srs_message *message);
int srs_header_setup(struct srs_header *header,
	const struct srs_message *message);
int srs_message_setup(const struct srs_header *header,
	struct srs_message *message);

/* SRS */
int srs_send(unsigned short command, const void *data, size_t size);
int srs_control_ping(struct srs_message *message);
int srs_test_set_radio_state(struct srs_message *message);

/* SRS client */
int srs_client_register(struct ril_client *ril_client, int fd);
int srs_client_unregister(struct ril_client *ril_client,
	struct srs_client *client);
int srs_client_flush(struct ril_client *ril_client);
struct srs_client *srs_client_find(struct ril_client *ril_client);
struct srs_client *srs_client_find_fd(struct ril_client *ril_client, int fd);
struct srs_client *srs_client_find_fd_set(struct ril_client *ril_client,
	fd_set *fds);
int srs_client_fd_set_setup(struct ril_client *ril_client, fd_set *fds);
int srs_client_send(struct ril_client *client, struct srs_message *message);
int srs_client_recv(struct ril_client *client, struct srs_message *message);

/* SRS server */
int srs_server_open(struct ril_client *client);
int srs_server_close(struct ril_client *client);
int srs_server_loop(struct ril_client *client);
void *srs_server_thread(void *data);

/* SRS client */
int srs_create(struct ril_client *client);
int srs_destroy(struct ril_client *client);
int srs_open(struct ril_client *client);
int srs_close(struct ril_client *ril_client);
int srs_dispatch(struct ril_client *client, struct srs_message *message);
int srs_loop(struct ril_client *ril_client);

/*
 * RIL client
 */

extern struct ril_client_handlers srs_handlers;
extern struct ril_client_callbacks srs_callbacks;
extern struct ril_client srs_client;

#endif
