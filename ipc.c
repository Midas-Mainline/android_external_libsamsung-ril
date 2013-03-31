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

#define LOG_TAG "RIL-IPC"
#include <utils/Log.h>

#include "samsung-ril.h"
#include <radio.h>

/*
 * IPC shared 
 */

void ipc_log_handler(const char *message, void *user_data)
{
	LOGD("ipc: %s", message);
}

/*
 * IPC FMT
 */

void ipc_fmt_send(const unsigned short command, const char type, unsigned char *data, const int length, unsigned char mseq)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	if (ril_data.ipc_fmt_client == NULL || ril_data.ipc_fmt_client->data == NULL)
		return;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_fmt_client->data;

	if (ipc_client_data->ipc_client == NULL)
		return;

	ipc_client = ipc_client_data->ipc_client;

	RIL_CLIENT_LOCK(ril_data.ipc_fmt_client);
	ipc_client_send(ipc_client, command, type, data, length, mseq);
	RIL_CLIENT_UNLOCK(ril_data.ipc_fmt_client);
}

int ipc_fmt_read_loop(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	struct ipc_message_info info;
	int ipc_client_fd;
	fd_set fds;

	if (client == NULL || client->data == NULL)
		return -EINVAL;

	ipc_client_data = (struct ipc_client_data *) client->data;

	if (ipc_client_data->ipc_client == NULL)
		return -EINVAL;

	ipc_client = ipc_client_data->ipc_client;
	ipc_client_fd = ipc_client_data->ipc_client_fd;

	while (1) {
		if (ipc_client_fd < 0) {
			LOGE("IPC FMT client fd is negative, aborting");
			return -1;
		}

		FD_ZERO(&fds);
		FD_SET(ipc_client_fd, &fds);

		select(FD_SETSIZE, &fds, NULL, NULL, NULL);

		if (FD_ISSET(ipc_client_fd, &fds)) {
			memset(&info, 0, sizeof(info));

			RIL_CLIENT_LOCK(client);
			if (ipc_client_recv(ipc_client, &info) < 0) {
				RIL_CLIENT_UNLOCK(client);
				LOGE("IPC FMT recv failed, aborting");
				return -1;
			}
			RIL_CLIENT_UNLOCK(client);

			ipc_fmt_dispatch(&info);

			if (info.data != NULL && info.length > 0)
				free(info.data);
		}
	}

	return 0;
}

int ipc_fmt_create(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	int ipc_client_fd;
	int rc;

	if (client == NULL)
		return -EINVAL;

	LOGD("Creating new FMT client");

	ipc_client_data = (struct ipc_client_data *) calloc(1, sizeof(struct ipc_client_data));
	ipc_client_data->ipc_client_fd = -1;

	client->data = (void *) ipc_client_data;

	ipc_client = ipc_client_new(IPC_CLIENT_TYPE_FMT);
	if (ipc_client == NULL) {
		LOGE("FMT client creation failed");
		goto error_client_create;
	}

	ipc_client_data->ipc_client = ipc_client;

	LOGD("Setting log handler");

	rc = ipc_client_set_log_handler(ipc_client, ipc_log_handler, NULL);
	if (rc < 0) {
		LOGE("Setting log handler failed");
		goto error_log_handler;
	}

	// ipc_client_set_handlers

	LOGD("Creating handlers common data");

	rc = ipc_client_create_handlers_common_data(ipc_client);
	if (rc < 0) {
		LOGE("Creating handlers common data failed");
		goto error_handlers_create;
	}

	LOGD("Starting modem bootstrap");

	rc = ipc_client_bootstrap_modem(ipc_client);
	if (rc < 0) {
		LOGE("Modem bootstrap failed");
		goto error_bootstrap;
	}

	LOGD("Client open...");

	rc = ipc_client_open(ipc_client);
	if (rc < 0) {
		LOGE("%s: failed to open ipc client", __func__);
		goto error_open;
	}

	LOGD("Obtaining ipc_client_fd");

	ipc_client_fd = ipc_client_get_handlers_common_data_fd(ipc_client);
	if (ipc_client_fd < 0) {
		LOGE("%s: client_fmt_fd is negative, aborting", __func__);
		goto error_get_fd;
	}

	ipc_client_data->ipc_client_fd = ipc_client_fd;

	LOGD("Client power on...");

	rc = ipc_client_power_on(ipc_client);
	if (rc < 0) {
		LOGE("%s: failed to power on ipc client", __func__);
		goto error_power_on;
	}

	LOGD("IPC FMT client done");

	return 0;

error:
	ipc_client_power_off(ipc_client);

error_power_on:
error_get_fd:
	ipc_client_close(ipc_client);

error_open:
error_bootstrap:
	ipc_client_destroy_handlers_common_data(ipc_client);

error_handlers_create:
error_log_handler:
	ipc_client_free(ipc_client);

error_client_create:
	ipc_client_data->ipc_client = NULL;
	ipc_client_data->ipc_client_fd = -1;

	free(ipc_client_data);
	client->data = NULL;

	return -1;
}

int ipc_fmt_destroy(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	int rc;

	if (client == NULL || client->data == NULL) {
		LOGE("Client was already destroyed");
		return 0;
	}

	ipc_client_data = (struct ipc_client_data *) client->data;
	ipc_client = ipc_client_data->ipc_client;

	LOGD("Destroying ipc fmt client");

	if (ipc_client != NULL) {
		ipc_client_power_off(ipc_client);
		ipc_client_close(ipc_client);
		ipc_client_destroy_handlers_common_data(ipc_client);
		ipc_client_free(ipc_client);
	}

	ipc_client_data->ipc_client = NULL;
	ipc_client_data->ipc_client_fd = -1;

	free(ipc_client_data);
	client->data = NULL;

	return 0;
}

/*
 * IPC RFS
 */

void ipc_rfs_send(const unsigned short command, unsigned char *data, const int length, unsigned char mseq)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	if (ril_data.ipc_rfs_client == NULL || ril_data.ipc_rfs_client->data == NULL)
		return;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_rfs_client->data;

	if (ipc_client_data->ipc_client == NULL)
		return;

	ipc_client = ipc_client_data->ipc_client;

	RIL_CLIENT_LOCK(ril_data.ipc_rfs_client);
	ipc_client_send(ipc_client, command, 0, data, length, mseq);
	RIL_CLIENT_UNLOCK(ril_data.ipc_rfs_client);
}

int ipc_rfs_read_loop(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	struct ipc_message_info info;
	int ipc_client_fd;
	fd_set fds;

	if (client == NULL || client->data == NULL)
		return -EINVAL;

	ipc_client_data = (struct ipc_client_data *) client->data;

	if (ipc_client_data->ipc_client == NULL)
		return -EINVAL;

	ipc_client = ipc_client_data->ipc_client;
	ipc_client_fd = ipc_client_data->ipc_client_fd;

	while (1) {
		if (ipc_client_fd < 0) {
			LOGE("IPC RFS client fd is negative, aborting");
			return -1;
		}

		FD_ZERO(&fds);
		FD_SET(ipc_client_fd, &fds);

		select(FD_SETSIZE, &fds, NULL, NULL, NULL);

		if (FD_ISSET(ipc_client_fd, &fds)) {
			memset(&info, 0, sizeof(info));

			RIL_CLIENT_LOCK(client);
			if (ipc_client_recv(ipc_client, &info) < 0) {
				RIL_CLIENT_UNLOCK(client);
				LOGE("IPC RFS recv failed, aborting");
				return -1;
			}
			RIL_CLIENT_UNLOCK(client);

			ipc_rfs_dispatch(&info);

			if (info.data != NULL && info.length > 0)
				free(info.data);
		}
	}

	return 0;
}

int ipc_rfs_create(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	int ipc_client_fd;
	int rc;

	if (client == NULL)
		return -EINVAL;

	LOGD("Creating new RFS client");

	ipc_client_data = (struct ipc_client_data *) calloc(1, sizeof(struct ipc_client_data));
	ipc_client_data->ipc_client_fd = -1;

	client->data = (void *) ipc_client_data;

	ipc_client = ipc_client_new(IPC_CLIENT_TYPE_RFS);
	if (ipc_client == NULL) {
		LOGE("RFS client creation failed");
		goto error_client_create;
	}

	ipc_client_data->ipc_client = ipc_client;

	LOGD("Setting log handler");

	rc = ipc_client_set_log_handler(ipc_client, ipc_log_handler, NULL);
	if (rc < 0) {
		LOGE("Setting log handler failed");
		goto error_log_handler;
	}

	// ipc_client_set_handlers

	LOGD("Creating handlers common data");

	rc = ipc_client_create_handlers_common_data(ipc_client);
	if (rc < 0) {
		LOGE("Creating handlers common data failed");
		goto error_handlers_create;
	}

	LOGD("Client open...");

	rc = ipc_client_open(ipc_client);
	if (rc < 0) {
		LOGE("%s: failed to open ipc client", __func__);
		goto error_open;
	}

	LOGD("Obtaining ipc_client_fd");

	ipc_client_fd = ipc_client_get_handlers_common_data_fd(ipc_client);
	if (ipc_client_fd < 0) {
		LOGE("%s: client_rfs_fd is negative, aborting", __func__);
		goto error_get_fd;
	}

	ipc_client_data->ipc_client_fd = ipc_client_fd;

	LOGD("IPC RFS client done");

	return 0;

error:
error_get_fd:
	ipc_client_close(ipc_client);

error_open:
	ipc_client_destroy_handlers_common_data(ipc_client);

error_handlers_create:
error_log_handler:
	ipc_client_free(ipc_client);

error_client_create:
	ipc_client_data->ipc_client = NULL;
	ipc_client_data->ipc_client_fd = -1;

	free(ipc_client_data);
	client->data = NULL;

	return -1;
}


int ipc_rfs_destroy(struct ril_client *client)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	int rc;

	if (client == NULL || client->data == NULL) {
		LOGE("Client was already destroyed");
		return 0;
	}

	ipc_client_data = (struct ipc_client_data *) client->data;
	ipc_client = ipc_client_data->ipc_client;

	LOGD("Destroying ipc rfs client");

	if (ipc_client != NULL) {
		ipc_client_close(ipc_client);
		ipc_client_destroy_handlers_common_data(ipc_client);
		ipc_client_free(ipc_client);
	}

	ipc_client_data->ipc_client = NULL;
	ipc_client_data->ipc_client_fd = -1;

	free(ipc_client_data);
	client->data = NULL;

	return 0;
}

/*
 * IPC clients structures
 */

struct ril_client_funcs ipc_fmt_client_funcs = {
	.create = ipc_fmt_create,
	.destroy = ipc_fmt_destroy,
	.read_loop = ipc_fmt_read_loop,
};

struct ril_client_funcs ipc_rfs_client_funcs = {
	.create = ipc_rfs_create,
	.destroy = ipc_rfs_destroy,
	.read_loop = ipc_rfs_read_loop,
};
