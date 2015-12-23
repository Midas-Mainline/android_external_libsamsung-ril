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

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/eventfd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <cutils/sockets.h>

#define LOG_TAG "RIL-SRS"
#include <utils/Log.h>
#include <hardware_legacy/power.h>

#include <samsung-ril.h>
#include <utils.h>

/*
 * Utils
 */

const char *srs_command_string(unsigned short command)
{
	static char command_string[7] = { 0 };

	switch (command) {
		case SRS_CONTROL_PING:
			return "SRS_CONTROL_PING";
		case SRS_SND_SET_CALL_VOLUME:
			return "SRS_SND_SET_CALL_VOLUME";
		case SRS_SND_SET_CALL_AUDIO_PATH:
			return "SRS_SND_SET_CALL_AUDIO_PATH";
		case SRS_SND_SET_CALL_CLOCK_SYNC:
			return "SRS_SND_SET_CALL_CLOCK_SYNC";
		case SRS_SND_SET_MIC_MUTE:
			return "SRS_SND_SET_MIC_MUTE";
		case SRS_TEST_SET_RADIO_STATE:
			return "SRS_TEST_SET_RADIO_STATE";
		default:
			snprintf((char *) &command_string, sizeof(command_string), "0x%04x", command);
			return command_string;
	}
}

void srs_log_send(struct ril_client *client, struct srs_message *message)
{
	if (client == NULL || message == NULL)
		return;

	RIL_LOGD("\n");
	RIL_LOGD("%s: Sent %s message", __func__, client->name);
	RIL_LOGD("%s: Message: command=%s, size=%d", __func__, srs_command_string(message->command), message->size);
	if (message->size > 0) {
		RIL_LOGD("=================================== %s data ===================================", client->name);
		data_dump(message->data, message->size);
		RIL_LOGD("================================================================================");
	}
}

void srs_log_recv(struct ril_client *client, struct srs_message *message)
{
	if (client == NULL || message == NULL)
		return;

	RIL_LOGD("\n");
	RIL_LOGD("%s: Received %s message", __func__, client->name);
	RIL_LOGD("%s: Message: command=%s, size=%d", __func__, srs_command_string(message->command), message->size);
	if (message->size > 0) {
		RIL_LOGD("=================================== %s data ===================================", client->name);
		data_dump(message->data, message->size);
		RIL_LOGD("================================================================================");
	}
}

int srs_header_setup(struct srs_header *header,
	const struct srs_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(header, 0, sizeof(struct srs_header));
	header->length = message->size + sizeof(struct srs_header);
	header->group = SRS_GROUP(message->command);
	header->index = SRS_INDEX(message->command);

	return 0;
}

int srs_message_setup(const struct srs_header *header,
	struct srs_message *message)
{
	if (header == NULL || message == NULL)
		return -1;

	memset(message, 0, sizeof(struct srs_message));
	message->command = SRS_COMMAND(header->group, header->index);
	message->data = NULL;
	message->size = 0;

	return 0;
}

/*
 * SRS
 */

int srs_send(unsigned short command, const void *data, size_t size)
{
	struct ril_client *ril_client;
	struct srs_data *srs_data;
	struct srs_client *client;
	struct srs_message message;
	int rc;

	ril_client = ril_client_find_id(RIL_CLIENT_SRS);
	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	srs_data = (struct srs_data *) ril_client->data;
	if (srs_data->event_fd < 0)
		return -1;

	if (!ril_client->available) {
		RIL_LOGE("%s client is not available", ril_client->name);
		return -1;
	}

	memset(&message, 0, sizeof(message));
	message.command = command;
	message.data = (void *) data;
	message.size = size;

	acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

	rc = srs_client_send(ril_client, &message);
	if (rc < 0) {
		RIL_LOGE("Sending to %s client failed", ril_client->name);

		release_wake_lock(RIL_VERSION_STRING);

		eventfd_send(srs_data->event_fd, SRS_CLIENT_IO_ERROR);
		return -1;
	}

	return 0;
}

int srs_control_ping(struct srs_message *message)
{
	struct srs_control_ping_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_control_ping_data))
		return -1;

	data = (struct srs_control_ping_data *) message->data;
	if (data->caffe == SRS_CONTROL_CAFFE) {
		rc = srs_send(SRS_CONTROL_PING, data, sizeof(struct srs_control_ping_data));
		if (rc < 0) {
			RIL_LOGE("Sending SRS control ping failed");
			return 0;
		}
	}

	return 0;
}

int srs_test_set_radio_state(struct srs_message *message)
{
	struct srs_test_set_radio_state_data *data;
	int rc;

	if (message == NULL || message->data == NULL || message->size < sizeof(struct srs_test_set_radio_state_data))
		return -1;

	data = (struct srs_test_set_radio_state_data *) message->data;

	ril_radio_state_update((RIL_RadioState) data->state);

	return 0;
}

/*
 * SRS client
 */

int srs_client_register(struct ril_client *ril_client, int fd)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list_end;
	struct list_head *list;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	client = calloc(1, sizeof(struct srs_client));
	client->fd = fd;

	list_end = data->clients;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc(list_end, NULL, (void *) client);

	if (data->clients == NULL)
		data->clients = list;

	RIL_CLIENT_UNLOCK(ril_client);

	return 0;
}

int srs_client_unregister(struct ril_client *ril_client,
	struct srs_client *client)
{
	struct srs_data *data;
	struct list_head *list;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	list = data->clients;
	while (list != NULL) {
		if (list->data == (void *) client) {
			memset(client, 0, sizeof(struct srs_client));
			free(client);

			if (list == data->clients)
				data->clients = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return 0;
}

int srs_client_flush(struct ril_client *ril_client)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list;
	struct list_head *list_next;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	list = data->clients;
	while (list != NULL) {
		if (list->data != NULL) {
			client = (struct srs_client *) list->data;
			memset(client, 0, sizeof(struct srs_client));
			free(client);
		}

		if (list == data->clients)
			data->clients = list->next;

		list_next = list->next;

		list_head_free(list);

list_continue:
		list = list_next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return 0;
}

struct srs_client *srs_client_find(struct ril_client *ril_client)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list;

	if (ril_client == NULL || ril_client->data == NULL)
		return NULL;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	list = data->clients;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		client = (struct srs_client *) list->data;

		RIL_CLIENT_UNLOCK(ril_client);
		return client;

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return NULL;
}

struct srs_client *srs_client_find_fd(struct ril_client *ril_client, int fd)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list;

	if (ril_client == NULL || ril_client->data == NULL)
		return NULL;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	list = data->clients;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		client = (struct srs_client *) list->data;

		if (client->fd == fd) {
			RIL_CLIENT_UNLOCK(ril_client);
			return client;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return NULL;
}

struct srs_client *srs_client_find_fd_set(struct ril_client *ril_client,
	fd_set *fds)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list;

	if (ril_client == NULL || ril_client->data == NULL || fds == NULL )
		return NULL;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	list = data->clients;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		client = (struct srs_client *) list->data;

		if (FD_ISSET(client->fd, fds)) {
			FD_CLR(client->fd, fds);

			RIL_CLIENT_UNLOCK(ril_client);
			return client;
		}

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return NULL;
}

int srs_client_fd_set_setup(struct ril_client *ril_client, fd_set *fds)
{
	struct srs_data *data;
	struct srs_client *client;
	struct list_head *list;
	int fd_max = -1;

	if (ril_client == NULL || ril_client->data == NULL || fds == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;

	RIL_CLIENT_LOCK(ril_client);

	FD_ZERO(fds);

	list = data->clients;
	while (list != NULL) {
		if (list->data == NULL)
			goto list_continue;

		client = (struct srs_client *) list->data;

		FD_SET(client->fd, fds);
		if (client->fd > fd_max)
			fd_max = client->fd;

list_continue:
		list = list->next;
	}

	RIL_CLIENT_UNLOCK(ril_client);

	return fd_max;
}

int srs_client_send(struct ril_client *client, struct srs_message *message)
{
	struct srs_data *data;
	struct srs_header header;
	void *buffer = NULL;
	size_t length;
	struct timeval timeout;
	fd_set fds;
	unsigned char *p;
	int rc;

	if (client == NULL || client->data == NULL || message == NULL)
		return -1;

	data = (struct srs_data *) client->data;
	if (data->client_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	srs_header_setup(&header, message);

	length = header.length;
	buffer = calloc(1, length);

	memcpy(buffer, &header, sizeof(header));
	if (message->data != NULL && message->size > 0) {
		p = (unsigned char *) buffer + sizeof(header);
		memcpy(p, message->data, message->size);
	}

	srs_log_send(client, message);

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	FD_ZERO(&fds);
	FD_SET(data->client_fd, &fds);

	rc = select(data->client_fd + 1, NULL, &fds, NULL, &timeout);
	if (rc <= 0 || !FD_ISSET(data->client_fd, &fds)) {
		RIL_LOGE("Polling %s client failed", client->name);
		goto error;
	}

	rc = write(data->client_fd, buffer, length);
	if (rc < (int) length) {
		RIL_LOGE("Writing to %s client failed", client->name);
		goto error;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int srs_client_recv(struct ril_client *client, struct srs_message *message)
{
	struct srs_data *data;
	struct srs_header *header;
	void *buffer = NULL;
	size_t length;
	struct timeval timeout;
	fd_set fds;
	unsigned char *p;
	int rc;

	if (client == NULL || client->data == NULL || message == NULL)
		return -1;

	data = (struct srs_data *) client->data;
	if (data->client_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	length = SRS_BUFFER_LENGTH;
	buffer= calloc(1, length);

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	FD_ZERO(&fds);
	FD_SET(data->client_fd, &fds);

	rc = select(data->client_fd + 1, &fds, NULL, NULL, &timeout);
	if (rc <= 0 || !FD_ISSET(data->client_fd, &fds)) {
		RIL_LOGE("Polling %s client failed", client->name);
		goto error;
	}

	rc = read(data->client_fd, buffer, length);
	if (rc < (int) sizeof(struct srs_header)) {
		RIL_LOGE("Reading from %s client failed", client->name);
		goto error;
	}

	header = (struct srs_header *) buffer;

	srs_message_setup(header, message);

	length = header->length - sizeof(struct srs_header);
	if (length > 0) {
		message->size = length;
		message->data = calloc(1, length);

		p = (unsigned char *) buffer + sizeof(struct srs_header);
		memcpy(message->data, p, length);
	}

	srs_log_recv(client, message);

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	RIL_CLIENT_UNLOCK(client);

	return rc;
}

/*
 * SRS server
 */

int srs_server_open(struct ril_client *client)
{
	struct srs_data *data;
	int server_fd;
	int flags;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct srs_data *) client->data;
	if (data->server_event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(client);

	unlink(SRS_SOCKET_NAME);

#if RIL_VERSION >= 6
	server_fd = socket_local_server(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
#else
	server_fd = socket_local_server(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
#endif
	if (server_fd < 0) {
		RIL_LOGE("Opening %s server failed", client->name);
		goto error;
	}

	flags = fcntl(server_fd, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(server_fd, F_SETFL, flags);

	data->server_fd = server_fd;

	eventfd_flush(data->server_event_fd);

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int srs_server_close(struct ril_client *ril_client)
{
	struct srs_data *data;
	struct srs_client *client;
	eventfd_t event;
	int rc;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;
	if (data->server_event_fd < 0)
		return -1;

	do {
		client = srs_client_find(ril_client);
		if (client == NULL)
			break;

		if (client->fd >= 0)
			close(client->fd);

		rc = srs_client_unregister(ril_client, client);
		if (rc < 0) {
			RIL_LOGE("Unregistering %s client failed", ril_client->name);
			return -1;
		}
	} while (client != NULL);

	RIL_CLIENT_LOCK(ril_client);

	rc = eventfd_send(data->server_event_fd, SRS_SERVER_CLOSE);
	if (rc < 0) {
		RIL_LOGE("Sending %s server close event failed", ril_client->name);
		goto error;
	}

	if (data->server_fd >= 0) {
		close(data->server_fd);
		data->server_fd = -1;
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(ril_client);

	return rc;
}

int srs_server_loop(struct ril_client *client)
{
	struct srs_data *data;
	struct sockaddr_un client_addr;
	socklen_t client_addr_len;
	eventfd_t event;
	int client_fd;
	fd_set fds;
	int fd_max;
	int flags;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct srs_data *) client->data;
	if (data->server_fd < 0 || data->server_event_fd < 0 || data->event_fd < 0)
		return -1;

	while (1) {
		if (!client->available) {
			RIL_LOGE("%s client is not available", client->name);
			return -1;
		}

		FD_ZERO(&fds);
		FD_SET(data->server_fd, &fds);
		FD_SET(data->server_event_fd, &fds);

		fd_max = data->server_fd > data->server_event_fd ? data->server_fd : data->server_event_fd;

		rc = select(fd_max + 1, &fds, NULL, NULL, NULL);
		if (rc < 0) {
			RIL_LOGE("Polling %s server failed", client->name);
			return -1;
		}

		if (FD_ISSET(data->server_event_fd, &fds)) {
			rc = eventfd_recv(data->server_event_fd, &event);
			if (rc < 0)
				return -1;

			switch (event) {
				case SRS_SERVER_CLOSE:
					return 0;
			}
		}

		if (!FD_ISSET(data->server_fd, &fds))
			continue;

		client_fd = accept(data->server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (client_fd < 0) {
			RIL_LOGE("Accepting new %s client failed", client->name);
			return -1;
		}

		flags = fcntl(client_fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(client_fd, F_SETFL, flags);

		rc = srs_client_register(client, client_fd);
		if (rc < 0) {
			RIL_LOGE("Registering new %s client failed", client->name);
			return -1;
		}

		RIL_LOGD("Registered new %s client", client->name);

		rc = eventfd_send(data->event_fd, SRS_CLIENT_CHANGE);
		if (rc < 0) {
			RIL_LOGE("Sending %s change event failed", client->name);
			return -1;
		}
	}

	return 0;
}

void *srs_server_thread(void *data)
{
	struct ril_client *client;
	int failures = 0;
	int rc;

	if (data == NULL)
		return NULL;

	client = (struct ril_client *) data;

	do {
		if (failures) {
			rc = srs_server_close(client);
			if (rc < 0)
				goto failure;

			rc = srs_server_open(client);
			if (rc < 0)
				goto failure;
		}

		rc = srs_server_loop(client);
		if (rc < 0) {
			RIL_LOGE("%s server loop failed", client->name);
			goto failure;
		} else {
			RIL_LOGE("%s server loop terminated", client->name);
			break;
		}

failure:
		failures++;
	} while (failures < RIL_CLIENT_RETRY_COUNT);

	srs_server_close(client);

	RIL_LOGD("Stopped %s server loop", client->name);

	return NULL;
}

/*
 * SRS client
 */

int srs_create(struct ril_client *client)
{
	struct srs_data *data;
	int server_event_fd = -1;
	int event_fd = -1;
	int rc;

	if (client == NULL)
		return -1;

	signal(SIGPIPE, SIG_IGN);

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	data = (struct srs_data *) calloc(1, sizeof(struct srs_data));
	data->server_fd = -1;
	data->client_fd = -1;

	server_event_fd = eventfd(0, EFD_NONBLOCK);
	if (server_event_fd < 0) {
		RIL_LOGE("Creating %s server event failed", client->name);
		goto error;
	}

	event_fd = eventfd(0, EFD_NONBLOCK);
	if (event_fd < 0) {
		RIL_LOGE("Creating %s event failed", client->name);
		goto error;
	}

	data->server_event_fd = server_event_fd;
	data->event_fd = event_fd;

	client->data = data;

	rc = 0;
	goto complete;

error:
	if (server_event_fd >= 0)
		close(server_event_fd);

	if (event_fd >= 0)
		close(event_fd);

	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int srs_destroy(struct ril_client *client)
{
	struct srs_data *data;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct srs_data *) client->data;

	if (client->available)
		srs_close(client);

	RIL_CLIENT_LOCK(client);

	client->available = 0;

	if (data->server_event_fd >= 0)
		close(data->server_event_fd);

	if (data->event_fd >= 0)
		close(data->event_fd);

	memset(data, 0, sizeof(struct srs_data));
	free(data);

	client->data = NULL;

	RIL_CLIENT_UNLOCK(client);

	return 0;
}

int srs_open(struct ril_client *client)
{
	struct srs_data *data;
	pthread_attr_t attr;
	int rc;

	if (client == NULL || client->data == NULL)
		return -1;

	data = (struct srs_data *) client->data;
	if (data->event_fd < 0)
		return -1;

	rc = srs_server_open(client);
	if (rc < 0) {
		RIL_LOGE("Opening %s server failed", client->name);
		return -1;
	}

	RIL_CLIENT_LOCK(client);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	rc = pthread_create(&data->server_thread, &attr, srs_server_thread, (void *) client);
	if (rc != 0) {
		RIL_LOGE("Starting %s server loop failed", client->name);
		goto error;
	}

	RIL_LOGD("Started %s server loop", client->name);

	eventfd_flush(data->event_fd);

	client->available = 1;

	rc = 0;
	goto complete;

error:
	srs_server_close(client);

	rc = -1;

complete:
	RIL_CLIENT_UNLOCK(client);

	return rc;
}

int srs_close(struct ril_client *ril_client)
{
	struct srs_data *data;
	struct srs_client *client;
	eventfd_t event;
	int rc;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;
	if (data->event_fd < 0)
		return -1;

	RIL_CLIENT_LOCK(ril_client);

	rc = eventfd_send(data->event_fd, SRS_CLIENT_CLOSE);
	if (rc < 0) {
		RIL_LOGE("Sending %s close event failed", ril_client->name);
		RIL_CLIENT_UNLOCK(ril_client);

		return -1;
	}

	data->client_fd = -1;

	RIL_CLIENT_UNLOCK(ril_client);

	rc = srs_server_close(ril_client);
	if (rc < 0) {
		RIL_LOGE("Closing %s server failed", ril_client->name);
		return -1;
	}

	rc = srs_client_flush(ril_client);
	if (rc < 0) {
		RIL_LOGE("Flushing %s client failed", ril_client->name);
		return -1;
	}

	pthread_join(data->server_thread, NULL);

	return 0;
}

int srs_dispatch(struct ril_client *client, struct srs_message *message)
{
	unsigned int i;
	int rc;

	if (client == NULL || message == NULL || ril_data == NULL)
		return -1;

	RIL_LOCK();

	for (i = 0; i < srs_dispatch_handlers_count; i++) {
		if (srs_dispatch_handlers[i].handler == NULL)
			continue;

		if (srs_dispatch_handlers[i].command == message->command) {
			rc = srs_dispatch_handlers[i].handler(message);
			if (rc < 0) {
				RIL_LOGE("Handling %s message failed", client->name);
				goto error;
			}

			rc = 0;
			goto complete;
		}
	}

	RIL_LOGD("Unhandled %s message: %s", client->name, srs_command_string(message->command));

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	RIL_UNLOCK();

	return rc;
}

int srs_loop(struct ril_client *ril_client)
{
	struct srs_data *data;
	struct srs_client *client;
	struct srs_message message;
	eventfd_t event;
	fd_set fds;
	int fd_max;
	int rc;

	if (ril_client == NULL || ril_client->data == NULL)
		return -1;

	data = (struct srs_data *) ril_client->data;
	if (data->event_fd < 0)
		return -1;

	while (1) {
		if (!ril_client->available) {
			RIL_LOGE("%s client is not available", ril_client->name);
			return -1;
		}

		fd_max = srs_client_fd_set_setup(ril_client, &fds);

		FD_SET(data->event_fd, &fds);

		fd_max = fd_max > data->event_fd ? fd_max : data->event_fd;

		rc = select(fd_max + 1, &fds, NULL, NULL, NULL);
		if (rc < 0) {
			RIL_LOGE("Polling %s client failed", ril_client->name);
			return -1;
		}

		if (FD_ISSET(data->event_fd, &fds)) {
			rc = eventfd_read(data->event_fd, &event);
			if (rc < 0)
				return -1;

			switch (event) {
				case SRS_CLIENT_CLOSE:
					return 0;
				case SRS_CLIENT_CHANGE:
					break;
				case SRS_CLIENT_IO_ERROR:
					if (client->fd >= 0)
						close(client->fd);

					data->client_fd = -1;

					srs_client_unregister(ril_client, client);
					break;
			}
		}

		do {
			client = srs_client_find_fd_set(ril_client, &fds);
			if (client == NULL)
				break;

			data->client_fd = client->fd;

			memset(&message, 0, sizeof(message));

			RIL_LOCK();
			acquire_wake_lock(PARTIAL_WAKE_LOCK, RIL_VERSION_STRING);

			rc = srs_client_recv(ril_client, &message);
			if (rc < 0) {
				RIL_LOGE("Receiving from %s client failed", ril_client->name);

				release_wake_lock(RIL_VERSION_STRING);
				RIL_UNLOCK();

				if (client->fd >= 0)
					close(client->fd);

				data->client_fd = -1;

				srs_client_unregister(ril_client, client);
				continue;
			}

			RIL_UNLOCK();

			rc = srs_dispatch(ril_client, &message);
			if (rc < 0) {
				RIL_LOGE("Dispatching %s message failed", ril_client->name);

				if (message.data != NULL && message.size > 0)
					free(message.data);

				if (client->fd >= 0)
					close(client->fd);

				data->client_fd = -1;

				srs_client_unregister(ril_client, client);
				continue;
			}

			if (message.data != NULL && message.size > 0)
				free(message.data);

			data->client_fd = -1;
		} while (client != NULL);
	}

	return 0;
}

/*
 * RIL client
 */

struct ril_client_handlers srs_handlers = {
	.create = srs_create,
	.destroy = srs_destroy,
	.open = srs_open,
	.close = srs_close,
	.loop = srs_loop,
};


struct ril_client_callbacks srs_callbacks = {
	.request_register = NULL,
	.request_unregister = NULL,
	.flush = NULL,
};

struct ril_client srs_client = {
	.id = RIL_CLIENT_SRS,
	.name = "SRS",
	.handlers = &srs_handlers,
	.callbacks = &srs_callbacks,
};
