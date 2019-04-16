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
#include <sys/eventfd.h>

#define LOG_TAG "RIL-IPC"
#include <utils/Log.h>
#include <hardware_legacy/power.h>

#include <samsung-ril.h>
#include <utils.h>

/*
 * Utils
 */

void ipc_log_handler(void *log_data, const char *message)
{
	RIL_LOGD("%s", message);
}

/*
 * IPC FMT
 */

int ipc_fmt_send(unsigned char mseq, unsigned short command, unsigned char type,
	const void *data, size_t size)
{
	struct ril_client *client;
	struct ipc_fmt_data *ipc_fmt_data;
	int rc;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return -1;

	ipc_fmt_data = (struct ipc_fmt_data *) client->data;
	if (ipc_fmt_data->ipc_client == NULL || ipc_fmt_data->event_fd < 0)
		return -1;

	if (!client->available) {
		RIL_LOGE("%s client is not available", client->name);
		return -1;
	}

	RIL_CLIENT_LOCK(client);
	acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

	rc = ipc_client_send(ipc_fmt_data->ipc_client, mseq, command, type, data, size);
	if (rc < 0) {
		RIL_LOGE("Sending to %s client failed", client->name);
		goto error;
	}

	rc = 0;
	goto complete;

error:
	eventfd_send(ipc_fmt_data->event_fd, IPC_CLIENT_IO_ERROR);

	rc = -1;

complete:
	release_wake_lock(RIL_VERSION_STRING);
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

unsigned char ipc_fmt_seq(void)
{
	struct ril_client *client;
	struct ipc_fmt_data *data;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL || client->data == NULL)
		return 0xff;

	data = (struct ipc_fmt_data *) client->data;

	data->seq++;

	if (data->seq % 0xff == 0x00)
		data->seq = 0x01;

	return data->seq;
}

unsigned char ipc_fmt_request_seq(RIL_Token token)
{
	struct ril_client *client;
	struct ipc_fmt_request *request;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return 0xff;

	request = ipc_fmt_request_find_token(client, token);
	if (request == NULL)
		return 0xff;

	if (request->seq == 0xff)
		request->seq = ipc_fmt_seq();

	return request->seq;
}

RIL_Token ipc_fmt_request_token(unsigned char seq)
{
	struct ril_client *client;
	struct ipc_fmt_request *request;

	client = ril_client_find_id(RIL_CLIENT_IPC_FMT);
	if (client == NULL)
		return NULL;

	request = ipc_fmt_request_find_seq(client, seq);
	if (request == NULL)
		return NULL;

	return request->token;
}

/*
 * IPC FMT client
 */

int ipc_fmt_create(struct ril_client *client)
{
	struct ipc_fmt_data *data = NULL;
	struct ipc_client *ipc_client = NULL;
	int event_fd = -1;
	int rc = 0;

	if (client == NULL)
		return -1;

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	data = (struct ipc_fmt_data *) calloc(1, sizeof(struct ipc_fmt_data));

	event_fd = eventfd(0, EFD_NONBLOCK);
	if (event_fd < 0) {
		RIL_LOGE("Creating %s event failed", client->name);
		goto error;
	}

	data->event_fd = event_fd;

	ipc_client = ipc_client_create(IPC_CLIENT_TYPE_FMT);
	if (ipc_client == NULL) {
		RIL_LOGE("Creating %s client failed", client->name);
		goto error;
	}

	rc = ipc_client_data_create(ipc_client);
	if (rc < 0) {
		RIL_LOGE("Creating %s client data failed", client->name);
		goto error;
	}

	rc = ipc_client_log_callback_register(ipc_client, ipc_log_handler, NULL);
	if (rc < 0) {
		RIL_LOGE("Setting %s client log handler failed", client->name);
		goto error;
	}

	data->ipc_client = ipc_client;
	client->data = (void *) data;

	rc = 0;
	goto complete;

error:
	if (event_fd >= 0)
		close(event_fd);

	if (ipc_client != NULL) {
		ipc_client_data_destroy(ipc_client);
		ipc_client_destroy(ipc_client);
	}

	if (data != NULL)
		free(data);

	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_fmt_destroy(struct ril_client *client)
{
	struct ipc_fmt_data *data;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	if (client->available)
		ipc_fmt_close(client);

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	if (data->event_fd >= 0)
		close(data->event_fd);

	if (data->ipc_client != NULL) {
		ipc_client_data_destroy(data->ipc_client);
		ipc_client_destroy(data->ipc_client);
	}

	RIL_CLIENT_UNLOCK(client);

	ipc_fmt_flush(client);

	RIL_CLIENT_LOCK(client);

	memset(data, 0, sizeof(struct ipc_fmt_data));
	free(data);

	client->data = NULL;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_fmt_open(struct ril_client *client)
{
	struct ipc_fmt_data *data;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	if (client->failures != 1) {
		rc = ipc_client_boot(data->ipc_client);
		if (rc < 0) {
			RIL_LOGE("Booting %s client failed", client->name);
			goto error;
		}

		rc = ipc_client_power_on(data->ipc_client);
		if (rc < 0) {
			RIL_LOGE("Powering on %s client failed", client->name);
			goto error;
		}
	}

	rc = ipc_client_open(data->ipc_client);
	if (rc < 0) {
		RIL_LOGE("Opening %s client failed", client->name);
		goto error;
	}

	if (client->failures == 1)
		ril_radio_state_update(RADIO_STATE_OFF);

	eventfd_flush(data->event_fd);

	client->available = 1;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_fmt_close(struct ril_client *client)
{
	struct ipc_fmt_data *data;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	rc = eventfd_send(data->event_fd, IPC_CLIENT_CLOSE);
	if (rc < 0) {
		RIL_LOGE("Sending %s close event failed", client->name);
		goto error;
	}

	rc = ipc_client_close(data->ipc_client);
	if (rc < 0) {
		RIL_LOGE("Closing %s client failed", client->name);
		goto error;
	}

	if (client->failures != 1) {
		rc = ipc_client_power_off(data->ipc_client);
		if (rc < 0) {
			RIL_LOGE("Powering off %s client failed", client->name);
			goto error;
		}
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_fmt_dispatch(struct ril_client *client, struct ipc_message *message)
{
	unsigned int i;
	int rc;

	if (client == NULL || message == NULL || ril_data == NULL)
		return -1;

	RIL_LOCK();

	for (i = 0; i < ipc_fmt_dispatch_handlers_count; i++) {
		if (ipc_fmt_dispatch_handlers[i].handler == NULL)
			continue;

		if (ipc_fmt_dispatch_handlers[i].command == message->command) {
			rc = ipc_fmt_dispatch_handlers[i].handler(message);
			if (rc < 0)
				goto error;

			rc = 0;
			goto complete;
		}
	}

	RIL_LOGD("Unhandled %s message: %s", client->name, ipc_command_string(message->command));

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_UNLOCK();

	return rc;
}

int ipc_fmt_loop(struct ril_client *client)
{
	struct ipc_fmt_data *data;
	struct ipc_message message;
	struct ipc_poll_fds fds;
	int fds_array[] = { 0 };
	unsigned int count;
	eventfd_t event;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	memset(&fds, 0, sizeof(fds));
	fds.fds = (int *) &fds_array;

	count = sizeof(fds_array) / sizeof(int);

	while (1) {
		if (!client->available) {
			RIL_LOGE("%s client is not available", client->name);
			goto error;
		}

		fds_array[0] = data->event_fd;
		fds.count = count;

		rc = ipc_client_poll(data->ipc_client, &fds, NULL);
		if (rc < 0) {
			RIL_LOGE("Polling %s client failed", client->name);
			goto error;
		}

		if (fds.fds[0] == data->event_fd && fds.count > 0) {
			rc = eventfd_recv(data->event_fd, &event);
			if (rc < 0)
				goto error;

			switch (event) {
				case IPC_CLIENT_CLOSE:
					rc = 0;
					goto complete;
				case IPC_CLIENT_IO_ERROR:
					goto error;
			}
		}

		if ((unsigned int) rc == fds.count)
			continue;

		memset(&message, 0, sizeof(message));

		RIL_LOCK();
		RIL_CLIENT_LOCK(client);
		acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

		rc = ipc_client_recv(data->ipc_client, &message);
		if (rc < 0) {
			RIL_LOGE("Receiving from %s client failed", client->name);

			release_wake_lock(RIL_VERSION_STRING);
			RIL_CLIENT_UNLOCK(client);
			RIL_UNLOCK();

			goto error;
		}

		release_wake_lock(RIL_VERSION_STRING);
		RIL_CLIENT_UNLOCK(client);
		RIL_UNLOCK();

		rc = ipc_fmt_dispatch(client, &message);
		if (rc < 0) {
			RIL_LOGE("Dispatching %s message failed", client->name);

			if (message.data != NULL && message.size > 0)
				free(message.data);

			goto error;
		}

		if (client->failures)
			client->failures = 0;

		if (message.data != NULL && message.size > 0)
			free(message.data);
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

	RIL_LOCK();
	ril_radio_state_update(RADIO_STATE_UNAVAILABLE);
	RIL_UNLOCK();

complete:
	return rc;
}

int ipc_fmt_request_register(struct ril_client *client, int request,
	RIL_Token token)
{
	struct ipc_fmt_data *data;
	struct ipc_fmt_request *ipc_fmt_request;
	struct list_head *list_end;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	ipc_fmt_request = (struct ipc_fmt_request *) calloc(1, sizeof(struct ipc_fmt_request));
	ipc_fmt_request->request = request;
	ipc_fmt_request->token = token;
	ipc_fmt_request->seq = 0xff;

	list_end = data->requests;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) ipc_fmt_request);

	if (data->requests == NULL)
		data->requests = list;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_fmt_request_unregister(struct ril_client *client, int request,
	RIL_Token token)
{
	struct ipc_fmt_data *data;
	struct ipc_fmt_request *ipc_fmt_request;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		ipc_fmt_request = (struct ipc_fmt_request *) list->data;

		if (ipc_fmt_request->request == request && ipc_fmt_request->token == token) {
			memset(ipc_fmt_request, 0, sizeof(struct ipc_fmt_request));
			free(ipc_fmt_request);

			if (list == data->requests)
				data->requests = list->next;

			list_head_free(list);

			break;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_fmt_request_flush(struct ril_client *client)
{
	struct ipc_fmt_data *data;
	struct ipc_fmt_request *ipc_fmt_request;
	struct list_head *list;
	struct list_head *list_next;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->requests;
	while (list != NULL) {
		if (list->data != NULL) {
			ipc_fmt_request = (struct ipc_fmt_request *) list->data;

			memset(ipc_fmt_request, 0, sizeof(struct ipc_fmt_request));
			free(ipc_fmt_request);
		}

		if (list == data->requests)
			data->requests = list->next;

		list_next = list->next;

		list_head_free(list);

		list = list_next;
	}

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

struct ipc_fmt_request *ipc_fmt_request_find_token(struct ril_client *client,
	RIL_Token token)
{
	struct ipc_fmt_data *data;
	struct ipc_fmt_request *request;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return NULL;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ipc_fmt_request *) list->data;

		if (request->token == token) {
			RIL_CLIENT_UNLOCK(client);
			return request;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(client);

	return NULL;
}

struct ipc_fmt_request *ipc_fmt_request_find_seq(struct ril_client *client,
	unsigned char seq)
{
	struct ipc_fmt_data *data;
	struct ipc_fmt_request *request;
	struct list_head *list;

	if (client == NULL || client->data == NULL)
		return NULL;

	data = (struct ipc_fmt_data *) client->data;

	RIL_CLIENT_LOCK(client);

	list = data->requests;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		request = (struct ipc_fmt_request *) list->data;

		if (request->seq == seq) {
			RIL_CLIENT_UNLOCK(client);
			return request;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(client);

	return NULL;
}

int ipc_fmt_flush(struct ril_client *client)
{
	struct ipc_fmt_data *data;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_fmt_data *) client->data;

	ipc_fmt_request_flush(client);

	ipc_gen_phone_res_expect_flush(client);

	RIL_CLIENT_LOCK(client);

	memset(&data->sim_icc_type_data, 0, sizeof(data->sim_icc_type_data));
	memset(&data->hsdpa_status_data, 0, sizeof(data->hsdpa_status_data));
	data->svc_session = 0;

	data->seq = 0x00;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}
/*
 * IPC RFS
 */

int ipc_rfs_send(unsigned char mseq, unsigned short command, const void *data,
	size_t size)
{
	struct ril_client *client;
	struct ipc_rfs_data *ipc_rfs_data;
	int rc;

	client = ril_client_find_id(RIL_CLIENT_IPC_RFS);
	if (client == NULL || client->data == NULL)
		return -1;

	ipc_rfs_data = (struct ipc_rfs_data *) client->data;
	if (ipc_rfs_data->ipc_client == NULL || ipc_rfs_data->event_fd < 0)
		return -1;

	if (!client->available) {
		RIL_LOGE("%s client is not available", client->name);
		return -1;
	}

	RIL_CLIENT_LOCK(client);
	acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

	rc = ipc_client_send(ipc_rfs_data->ipc_client, mseq, command, 0x00, data, size);
	if (rc < 0) {
		RIL_LOGE("Sending to %s client failed", client->name);
		goto error;
	}

	rc = 0;
	goto complete;

error:
	eventfd_send(ipc_rfs_data->event_fd, IPC_CLIENT_IO_ERROR);

	rc = -1;

complete:
	release_wake_lock(RIL_VERSION_STRING);
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

/*
 * IPC RFS client
 */

int ipc_rfs_create(struct ril_client *client)
{
	struct ipc_rfs_data *data = NULL;
	struct ipc_client *ipc_client = NULL;
	int event_fd = -1;
	int rc = 0;

	if (client == NULL)
		return -1;

	data = (struct ipc_rfs_data *) calloc(1, sizeof(struct ipc_rfs_data));

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	event_fd = eventfd(0, EFD_NONBLOCK);
	if (event_fd < 0) {
		RIL_LOGE("Creating %s event failed", client->name);
		goto error;
	}

	data->event_fd = event_fd;

	ipc_client = ipc_client_create(IPC_CLIENT_TYPE_RFS);
	if (ipc_client == NULL) {
		RIL_LOGE("Creating %s client failed", client->name);
		goto error;
	}

	rc = ipc_client_data_create(ipc_client);
	if (rc < 0) {
		RIL_LOGE("Creating %s client data failed", client->name);
		goto error;
	}

	rc = ipc_client_log_callback_register(ipc_client, ipc_log_handler, NULL);
	if (rc < 0) {
		RIL_LOGE("Setting %s client log handler failed", client->name);
		goto error;
	}

	data->ipc_client = ipc_client;
	client->data = (void *) data;

	rc = 0;
	goto complete;

error:
	if (event_fd >= 0)
		close(event_fd);

	if (ipc_client != NULL) {
		ipc_client_data_destroy(ipc_client);
		ipc_client_destroy(ipc_client);
	}

	if (data != NULL)
		free(data);

	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_rfs_destroy(struct ril_client *client)
{
	struct ipc_rfs_data *data;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_rfs_data *) client->data;

	if (client->available)
		ipc_rfs_close(client);

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	if (data->event_fd >= 0)
		close(data->event_fd);

	if (data->ipc_client != NULL) {
		ipc_client_data_destroy(data->ipc_client);
		ipc_client_destroy(data->ipc_client);
	}

	memset(data, 0, sizeof(struct ipc_rfs_data));
	free(data);

	client->data = NULL;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int ipc_rfs_open(struct ril_client *client)
{
	struct ipc_rfs_data *data;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_rfs_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	rc = ipc_client_open(data->ipc_client);
	if (rc < 0) {
		RIL_LOGE("Opening %s client failed", client->name);
		goto error;
	}

	eventfd_flush(data->event_fd);

	client->available = 1;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_rfs_close(struct ril_client *client)
{
	struct ipc_rfs_data *data;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_rfs_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	rc = eventfd_send(data->event_fd, IPC_CLIENT_CLOSE);
	if (rc < 0) {
		RIL_LOGE("Sending %s close event failed", client->name);
		goto error;
	}

	rc = ipc_client_close(data->ipc_client);
	if (rc < 0) {
		RIL_LOGE("Closing %s client failed", client->name);
		goto error;
	}

	rc = ipc_client_power_off(data->ipc_client);
	if (rc < 0) {
		RIL_LOGE("Powering off %s client failed", client->name);
		goto error;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int ipc_rfs_dispatch(struct ril_client *client, struct ipc_message *message)
{
	unsigned int i;
	int rc;

	if (client == NULL || message == NULL || ril_data == NULL)
		return -1;

	RIL_LOCK();

	for (i = 0; i < ipc_rfs_dispatch_handlers_count; i++) {
		if (ipc_rfs_dispatch_handlers[i].handler == NULL)
			continue;

		if (ipc_rfs_dispatch_handlers[i].command == message->command) {
			rc = ipc_rfs_dispatch_handlers[i].handler(message);
			if (rc < 0)
				goto error;

			rc = 0;
			goto complete;
		}
	}

	RIL_LOGD("Unhandled %s message: %s", client->name, ipc_command_string(message->command));

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_UNLOCK();

	return rc;
}

int ipc_rfs_loop(struct ril_client *client)
{
	struct ipc_rfs_data *data;
	struct ipc_message message;
	struct ipc_poll_fds fds;
	int fds_array[] = { 0 };
	unsigned int count;
	eventfd_t event;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct ipc_rfs_data *) client->data;
	if (data->ipc_client == NULL || data->event_fd < 0)
		return -1;

	memset(&fds, 0, sizeof(fds));
	fds.fds = (int *) &fds_array;

	count = sizeof(fds_array) / sizeof(int);

	while (1) {
		if (!client->available) {
			RIL_LOGE("%s client is not available", client->name);
			return -1;
		}

		fds_array[0] = data->event_fd;
		fds.count = count;

		rc = ipc_client_poll(data->ipc_client, &fds, NULL);
		if (rc < 0) {
			RIL_LOGE("Polling %s client failed", client->name);
			return -1;
		}

		if (fds.fds[0] == data->event_fd && fds.count > 0) {
			rc = eventfd_recv(data->event_fd, &event);
			if (rc < 0)
				return -1;

			switch (event) {
				case IPC_CLIENT_CLOSE:
					return 0;
				case IPC_CLIENT_IO_ERROR:
					return -1;
			}
		}

		if ((unsigned int) rc == fds.count)
			continue;

		memset(&message, 0, sizeof(message));

		RIL_LOCK();
		RIL_CLIENT_LOCK(client);
		acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

		rc = ipc_client_recv(data->ipc_client, &message);
		if (rc < 0) {
			RIL_LOGE("Receiving from %s client failed", client->name);

			release_wake_lock(RIL_VERSION_STRING);
			RIL_CLIENT_UNLOCK(client);
			RIL_UNLOCK();

			return -1;
		}

		release_wake_lock(RIL_VERSION_STRING);
		RIL_CLIENT_UNLOCK(client);
		RIL_UNLOCK();

		rc = ipc_rfs_dispatch(client, &message);
		if (rc < 0) {
			RIL_LOGE("Dispatching %s message failed", client->name);

			if (message.data != NULL && message.size > 0)
				free(message.data);

			return -1;
		}

		if (client->failures)
			client->failures = 0;

		if (message.data != NULL && message.size > 0)
			free(message.data);
	}

	return 0;
}

/*
 * RIL clients
 */

struct ril_client_handlers ipc_fmt_handlers = {
	.create = ipc_fmt_create,
	.destroy = ipc_fmt_destroy,
	.open = ipc_fmt_open,
	.close = ipc_fmt_close,
	.loop = ipc_fmt_loop,
};

struct ril_client_handlers ipc_rfs_handlers = {
	.create = ipc_rfs_create,
	.destroy = ipc_rfs_destroy,
	.open = ipc_rfs_open,
	.close = ipc_rfs_close,
	.loop = ipc_rfs_loop,
};

struct ril_client_callbacks ipc_fmt_callbacks = {
	.request_register = ipc_fmt_request_register,
	.request_unregister = ipc_fmt_request_unregister,
	.flush = ipc_fmt_flush,
};

struct ril_client_callbacks ipc_rfs_callbacks = {
	.request_register = NULL,
	.request_unregister = NULL,
	.flush = NULL,
};

struct ril_client ipc_fmt_client = {
	.id = RIL_CLIENT_IPC_FMT,
	.name = "IPC FMT",
	.handlers = &ipc_fmt_handlers,
	.callbacks = &ipc_fmt_callbacks,
};

struct ril_client ipc_rfs_client = {
	.id = RIL_CLIENT_IPC_RFS,
	.name = "IPC RFS",
	.handlers = &ipc_rfs_handlers,
	.callbacks = &ipc_rfs_callbacks,
};
