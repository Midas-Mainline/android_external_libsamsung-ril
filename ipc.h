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

#ifndef _SAMSUNG_RIL_IPC_H_
#define _SAMSUNG_RIL_IPC_H_

#include <stdlib.h>

#include <samsung-ril.h>

struct ril_client;

/*
 * Values
 */

#define IPC_CLIENT_CLOSE					0x00
#define IPC_CLIENT_IO_ERROR					0x01

/*
 * Structures
 */

struct ipc_fmt_request {
	int request;
	RIL_Token token;
	unsigned char seq;
};

struct ipc_fmt_data {
	struct ipc_client *ipc_client;
	int event_fd;

	struct list_head *gen_phone_res_expect;

	struct ipc_sec_sim_icc_type_data sim_icc_type_data;
	struct ipc_gprs_hsdpa_status_data hsdpa_status_data;
	int svc_session;

	struct list_head *requests;
	unsigned char seq;
};

struct ipc_rfs_data {
	struct ipc_client *ipc_client;
	int event_fd;
};

struct ipc_dispatch_handler {
	unsigned short command;
	int (*handler)(struct ipc_message *message);
};

/*
 * Helpers
 */

/* Utils */
void ipc_log_handler(void *log_data, const char *message);

/* IPC FMT */
int ipc_fmt_send(unsigned char mseq, unsigned short command, unsigned char type,
	const void *data, size_t size);
unsigned char ipc_fmt_seq(void);
unsigned char ipc_fmt_request_seq(RIL_Token token);
RIL_Token ipc_fmt_request_token(unsigned char seq);

/* IPC FMT client */
int ipc_fmt_create(struct ril_client *client);
int ipc_fmt_destroy(struct ril_client *client);
int ipc_fmt_open(struct ril_client *client);
int ipc_fmt_close(struct ril_client *client);
int ipc_fmt_dispatch(struct ril_client *client, struct ipc_message *message);
int ipc_fmt_loop(struct ril_client *client);
int ipc_fmt_request_register(struct ril_client *client, int request,
	RIL_Token token);
int ipc_fmt_request_unregister(struct ril_client *client, int request,
	RIL_Token token);
int ipc_fmt_request_flush(struct ril_client *client);
struct ipc_fmt_request *ipc_fmt_request_find_token(struct ril_client *client,
	RIL_Token token);
struct ipc_fmt_request *ipc_fmt_request_find_seq(struct ril_client *client,
	unsigned char seq);
int ipc_fmt_flush(struct ril_client *client);

/* IPC RFS */
int ipc_rfs_send(unsigned char mseq, unsigned short command, const void *data,
	size_t size);

/* IPC RFS client */
int ipc_rfs_create(struct ril_client *client);
int ipc_rfs_destroy(struct ril_client *client);
int ipc_rfs_open(struct ril_client *client);
int ipc_rfs_close(struct ril_client *client);
int ipc_rfs_dispatch(struct ril_client *client, struct ipc_message *message);
int ipc_rfs_loop(struct ril_client *client);

/*
 * RIL clients
 */

extern struct ril_client_handlers ipc_fmt_handlers;
extern struct ril_client_handlers ipc_rfs_handlers;
extern struct ril_client_callbacks ipc_fmt_callbacks;
extern struct ril_client_callbacks ipc_rfs_callbacks;
extern struct ril_client ipc_fmt_client;
extern struct ril_client ipc_rfs_client;

#endif
