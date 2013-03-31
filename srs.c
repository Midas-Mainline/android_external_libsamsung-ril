/*
 * This file is part of Samsung-RIL.
 *
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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <cutils/sockets.h>

#define LOG_TAG "RIL-SRS"
#include <utils/Log.h>

#include "samsung-ril.h"
#include "util.h"

int srs_client_register(struct srs_client_data *srs_client_data, int fd)
{
	struct srs_client_info *client;
	struct list_head *list_end;
	struct list_head *list;

	if (srs_client_data == NULL)
		return -1;

	client = calloc(1, sizeof(struct srs_client_info));
	if (client == NULL)
		return -1;

	client->fd = fd;

	list_end = srs_client_data->clients;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) client, list_end, NULL);

	if (srs_client_data->clients == NULL)
		srs_client_data->clients = list;

	return 0;
}

void srs_client_unregister(struct srs_client_data *srs_client_data, struct srs_client_info *client)
{
	struct list_head *list;

	if (srs_client_data == NULL || client == NULL)
		return;

	list = srs_client_data->clients;
	while (list != NULL) {
		if (list->data == (void *) client) {
			memset(client, 0, sizeof(struct srs_client_info));
			free(client);

			if (list == srs_client_data->clients)
				srs_client_data->clients = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct srs_client_info *srs_client_info_find(struct srs_client_data *srs_client_data)
{
	struct srs_client_info *client;
	struct list_head *list;

	list = srs_client_data->clients;
	while (list != NULL) {
		client = (struct srs_client_info *) list->data;
		if (client == NULL)
			goto list_continue;

		return client;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct srs_client_info *srs_client_info_find_fd(struct srs_client_data *srs_client_data, int fd)
{
	struct srs_client_info *client;
	struct list_head *list;

	list = srs_client_data->clients;
	while (list != NULL) {
		client = (struct srs_client_info *) list->data;
		if (client == NULL)
			goto list_continue;

		if (client->fd == fd)
			return client;

list_continue:
		list = list->next;
	}

	return NULL;
}

int srs_client_info_fill_fd_set(struct srs_client_data *srs_client_data, fd_set *fds)
{
	struct srs_client_info *client;
	struct list_head *list;
	int fd_max;

	if (srs_client_data == NULL || fds == NULL)
		return -1;

	fd_max = -1;
	list = srs_client_data->clients;
	while (list != NULL) {
		client = (struct srs_client_info *) list->data;
		if (client == NULL)
			goto list_continue;

		FD_SET(client->fd, fds);
		if (client->fd > fd_max)
			fd_max = client->fd;

list_continue:
		list = list->next;
	}

	return fd_max;
}

int srs_client_info_get_fd_set(struct srs_client_data *srs_client_data, fd_set *fds)
{
	struct srs_client_info *client;
	struct list_head *list;
	int fd;

	if (srs_client_data == NULL || fds == NULL)
		return -1;

	list = srs_client_data->clients;
	while (list != NULL) {
		client = (struct srs_client_info *) list->data;
		if (client == NULL)
			goto list_continue;

		if (FD_ISSET(client->fd, fds)) {
			FD_CLR(client->fd, fds);
			return client->fd;
		}

list_continue:
		list = list->next;
	}

	return -1;
}

int srs_client_send_message(struct srs_client_data *srs_client_data, struct srs_message *message)
{
	struct srs_header header;
	void *data;

	struct timeval timeout;
	fd_set fds;
	int rc;

	if (srs_client_data == NULL || message == NULL)
		return -EINVAL;

	memset(&header, 0, sizeof(header));
	header.group = SRS_GROUP(message->command);
	header.index = SRS_INDEX(message->command);
	header.length = message->length + sizeof(header);

	data = calloc(1, header.length);
	memcpy(data, &header, sizeof(header));
	memcpy((void *) ((char *) data + sizeof(header)), message->data, message->length);

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	if (srs_client_data->client_fd < 0) {
		rc = -1;
		goto complete;
	}

	FD_ZERO(&fds);
	FD_SET(srs_client_data->client_fd, &fds);

	rc = select(srs_client_data->client_fd + 1, NULL, &fds, NULL, &timeout);

	if (!FD_ISSET(srs_client_data->client_fd, &fds)) {
		LOGE("SRS write select failed on fd %d", srs_client_data->client_fd);
		rc = -1;
		goto complete;
	}

	rc = write(srs_client_data->client_fd, data, header.length);
	if (rc < (int) sizeof(struct srs_header)) {
		LOGE("SRS write failed on fd %d with %d bytes", srs_client_data->client_fd, rc);
		rc = -1;
		goto complete;
	}

complete:
	free(data);

	return rc;
}

int srs_client_send(struct srs_client_data *srs_client_data, unsigned short command, void *data, int length)
{
	struct srs_client_info *client;
	struct srs_message message;
	int rc;

	if (srs_client_data == NULL)
		return -1;

	memset(&message, 0, sizeof(message));
	message.command = command;
	message.length = length;
	message.data = data;

	RIL_CLIENT_LOCK(srs_client_data->client);
	rc = srs_client_send_message(srs_client_data, &message);
	RIL_CLIENT_UNLOCK(srs_client_data->client);

	if (rc <= 0) {
		LOGD("SRS client with fd %d terminated", srs_client_data->client_fd);

		client = srs_client_info_find_fd(srs_client_data, srs_client_data->client_fd);
		if (client != NULL)
			srs_client_unregister(srs_client_data, client);
		close(srs_client_data->client_fd);
		srs_client_data->client_fd = -1;
	}

	return rc;
}

int srs_send(unsigned short command, void *data, int length)
{
	struct srs_client_data *srs_client_data;
	int rc;

	if (ril_data.srs_client == NULL || ril_data.srs_client->data == NULL)
		return -EINVAL;

	srs_client_data = (struct srs_client_data *) ril_data.srs_client->data;

	LOGD("SEND SRS: fd=%d command=%d length=%d", srs_client_data->client_fd, command, length);
	if (data != NULL && length > 0) {
		LOGD("==== SRS DATA DUMP ====");
		hex_dump(data, length);
		LOGD("=======================");
	}

	return srs_client_send(srs_client_data, command, data, length);
}

int srs_client_recv(struct srs_client_data *srs_client_data, struct srs_message *message)
{
	struct srs_header *header;
	void *data;

	struct timeval timeout;
	fd_set fds;
	int rc;

	if (srs_client_data == NULL || message == NULL)
		return -1;

	data = calloc(1, SRS_DATA_MAX_SIZE);

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	if (srs_client_data->client_fd < 0) {
		rc = -1;
		goto complete;
	}

	FD_ZERO(&fds);
	FD_SET(srs_client_data->client_fd, &fds);

	rc = select(srs_client_data->client_fd + 1, &fds, NULL, NULL, &timeout);

	if (!FD_ISSET(srs_client_data->client_fd, &fds)) {
		LOGE("SRS read select failed on fd %d", srs_client_data->client_fd);
		rc = -1;
		goto complete;
	}

	rc = read(srs_client_data->client_fd, data, SRS_DATA_MAX_SIZE);
	if (rc < (int) sizeof(struct srs_header)) {
		LOGE("SRS read failed on fd %d with %d bytes", srs_client_data->client_fd, rc);
		rc = -1;
		goto complete;
	}

	header = (struct srs_header *) data;

	memset(message, 0, sizeof(struct srs_message));
	message->command = SRS_COMMAND(header);
	message->length = header->length - sizeof(struct srs_header);
	message->data = NULL;

	if (message->length > 0) {
		message->data = calloc(1, message->length);
		memcpy(message->data, (void *) ((char *) data + sizeof(struct srs_header)), message->length);
	}

complete:
	free(data);

	return rc;
}

void srs_control_ping(struct srs_message *message)
{
	int caffe;

	if (message == NULL || message->data == NULL || message->length < (int) sizeof(int))
		return;

	caffe = *((int *) message->data);

	if (caffe == SRS_CONTROL_CAFFE)
		srs_send(SRS_CONTROL_PING, &caffe, sizeof(caffe));
}

static int srs_server_open(void)
{
	int server_fd;
	int t;

	for (t = 0 ; t < 5 ; t++) {
		unlink(SRS_SOCKET_NAME);
#if RIL_VERSION >= 6
		server_fd = socket_local_server(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
#else
		server_fd = socket_local_server(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
#endif
		if (server_fd >= 0)
			return server_fd;
	}

	return -1;
}

void *srs_client_read_loop(void *data)
{
	struct srs_client_info *client;
	struct srs_client_data *srs_client_data;
	struct srs_message message;
	struct timeval timeout;
	fd_set fds;
	int fd_max;
	int fd;
	int rc;

	if (data == NULL)
		pthread_exit(NULL);

	srs_client_data = (struct srs_client_data *) data;

	while (srs_client_data->running) {
		FD_ZERO(&fds);

		SRS_CLIENT_LOCK();
		fd_max = srs_client_info_fill_fd_set(srs_client_data, &fds);
		SRS_CLIENT_UNLOCK();

		if (fd_max < 0) {
			usleep(3000);
			continue;
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = 3000;

		select(fd_max + 1, &fds, NULL, NULL, &timeout);

		SRS_CLIENT_LOCK();
		while ((fd = srs_client_info_get_fd_set(srs_client_data, &fds)) >= 0) {
			srs_client_data->client_fd = fd;

			RIL_CLIENT_LOCK(srs_client_data->client);
			rc = srs_client_recv(srs_client_data, &message);
			if (rc <= 0) {
				LOGD("SRS client with fd %d terminated", fd);

				client = srs_client_info_find_fd(srs_client_data, fd);
				if (client != NULL)
					srs_client_unregister(srs_client_data, client);
				close(fd);

				RIL_CLIENT_UNLOCK(srs_client_data->client);
				continue;
			}
			RIL_CLIENT_UNLOCK(srs_client_data->client);

			LOGD("RECV SRS: fd=%d command=%d length=%d", fd, message.command, message.length);
			if (message.data != NULL && message.length > 0) {
				LOGD("==== SRS DATA DUMP ====");
				hex_dump(message.data, message.length);
				LOGD("=======================");
			}

			srs_dispatch(&message);

			if (message.data != NULL && message.length > 0)
				free(message.data);

			srs_client_data->client_fd = -1;
		}
		SRS_CLIENT_UNLOCK();
	}

	pthread_exit(NULL);
	return NULL;
}

int srs_read_loop(struct ril_client *client)
{
	struct srs_client_data *srs_client_data;

	struct sockaddr_un client_addr;
	int client_addr_len;
	pthread_attr_t attr;
	int flags;
	int fd;
	int rc;

	if (client == NULL || client->data == NULL)
		return -EINVAL;

	srs_client_data = (struct srs_client_data *) client->data;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	srs_client_data->running = 1;

	rc = pthread_create(&srs_client_data->thread, &attr, srs_client_read_loop, (void *) srs_client_data);
	if (rc < 0) {
		LOGE("Unable to create SRS client read loop thread");
		return -1;
	}

	while (srs_client_data->server_fd >= 0) {
		fd = accept(srs_client_data->server_fd, (struct sockaddr *) &client_addr,
			&client_addr_len);
		if (fd < 0) {
			LOGE("Unable to accept new SRS client");
			break;
		}

		flags = fcntl(fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(fd, F_SETFL, flags);

		LOGD("Accepted new SRS client from fd %d", fd);

		SRS_CLIENT_LOCK();
		rc = srs_client_register(srs_client_data, fd);
		SRS_CLIENT_UNLOCK();
		if (rc < 0) {
			LOGE("Unable to register SRS client");
			break;
		}
	}

	LOGE("SRS server failure");

	srs_client_data->running = 0;

	// Wait for the thread to finish
	pthread_join(srs_client_data->thread, NULL);

	return 0;
}

int srs_create(struct ril_client *client)
{
	struct srs_client_data *srs_client_data;

	if (client == NULL)
		return -EINVAL;

	LOGD("Creating new SRS client");

	signal(SIGPIPE, SIG_IGN);

	srs_client_data = (struct srs_client_data *) calloc(1, sizeof(struct srs_client_data));

	srs_client_data->server_fd = srs_server_open();
	if (srs_client_data->server_fd < 0) {
		LOGE("SRS server creation failed");
		goto error;
	}

	pthread_mutex_init(&srs_client_data->mutex, NULL);

	srs_client_data->client = client;
	client->data = (void *) srs_client_data;

	return 0;

error:
	free(srs_client_data);

	return -1;
}

int srs_destroy(struct ril_client *client)
{
	struct srs_client_data *srs_client_data = NULL;
	struct srs_client_info *client_info;

	if (client == NULL || client->data == NULL) {
		LOGE("Client was already destroyed");
		return 0;
	}

	srs_client_data = (struct srs_client_data *) client->data;

	pthread_mutex_destroy(&srs_client_data->mutex);

	while ((client_info = srs_client_info_find(srs_client_data)) != NULL) {
		close(client_info->fd);
		srs_client_unregister(srs_client_data, client_info);
	}

	if (srs_client_data->server_fd > 0)
		close(srs_client_data->server_fd);

	srs_client_data->server_fd = -1;
	srs_client_data->client_fd = -1;
	srs_client_data->clients = NULL;
	srs_client_data->running = 0;

	free(srs_client_data);
	client->data = NULL;

	return 0;
}

struct ril_client_funcs srs_client_funcs = {
	.create = srs_create,
	.destroy = srs_destroy,
	.read_loop = srs_read_loop,
};
