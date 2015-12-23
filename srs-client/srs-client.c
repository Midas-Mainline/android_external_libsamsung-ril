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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/eventfd.h>

#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cutils/sockets.h>

#include <telephony/ril.h>

#include <samsung-ril-socket.h>
#include <srs-client.h>

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
		default:
			snprintf((char *) &command_string, sizeof(command_string), "0x%04x", command);
			return command_string;
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

int srs_ping(struct srs_client *client)
{
	struct srs_message message;
	struct srs_control_ping_data ping;
	struct srs_control_ping_data *pong;
	int rc;

	if (client == NULL)
		return -1;

	memset(&message, 0, sizeof(message));

	memset(&ping, 0, sizeof(ping));
	ping.caffe = SRS_CONTROL_CAFFE;

	rc = srs_client_send(client, SRS_CONTROL_PING, &ping, sizeof(ping));
	if (rc < 0)
		goto error;

	rc = srs_client_poll(client);
	if (rc <= 0)
		goto error;

	rc = srs_client_recv(client, &message);
	if (rc < 0 || message.data == NULL || message.size < sizeof(struct srs_control_ping_data))
		goto error;

	pong = (struct srs_control_ping_data *) message.data;
	if (pong->caffe != SRS_CONTROL_CAFFE)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (message.data != NULL && message.size > 0)
		free(message.data);

	return rc;
}

/*
 * SRS client
 */

struct srs_client *srs_client_create(void)
{
	struct srs_client *client;

	signal(SIGPIPE, SIG_IGN);

	client = (struct srs_client *) calloc(1, sizeof(struct srs_client));
	client->fd = -1;
	client->event_fd = -1;

	pthread_mutex_init(&client->mutex, NULL);

	return client;
}

int srs_client_destroy(struct srs_client *client)
{
	if (client == NULL)
		return -1;

	pthread_mutex_destroy(&client->mutex);

	memset(client, 0, sizeof(struct srs_client));
	free(client);

	return 0;
}

int srs_client_open(struct srs_client *client)
{
	int flags;
	int fd = 0;
	int i = 0;
	int rc;

	if (client == NULL)
		return -1;

	SRS_CLIENT_LOCK(client);

	do {
		if (fd < 0)
			usleep(50000);

#if RIL_VERSION >= 6
		fd = socket_local_client(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
#else
		fd = socket_local_client(SRS_SOCKET_NAME, ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
#endif

		flags = fcntl(fd, F_GETFL);
		flags |= O_NONBLOCK;
		fcntl(fd, F_SETFL, flags);

		i++;
	} while (fd < 0 && i < 5);

	if (fd < 0)
		goto error;

	client->fd = fd;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	SRS_CLIENT_UNLOCK(client);

	return rc;
}

int srs_client_close(struct srs_client *client)
{
	if (client == NULL )
		return -1;

	SRS_CLIENT_LOCK(client);

	if (client->fd >= 0) {
		close(client->fd);
		client->fd = -1;
	}

	SRS_CLIENT_UNLOCK(client);

	return 0;
}

int srs_client_poll(struct srs_client *client)
{
	struct timeval timeout;
	fd_set fds;
	int rc;

	if (client == NULL || client->fd < 0)
		return -1;

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 50000;

	FD_ZERO(&fds);
	FD_SET(client->fd, &fds);

	rc = select(client->fd + 1, &fds, NULL, NULL, &timeout);

	return rc;
}

int srs_client_send(struct srs_client *client, unsigned short command,
	void *data, size_t size)
{
	struct srs_message message;
	struct srs_header header;
	void *buffer = NULL;
	size_t length;
	struct timeval timeout;
	fd_set fds;
	unsigned char *p;
	int rc;

	if (client == NULL || client->fd < 0)
		return -1;

	SRS_CLIENT_LOCK(client);

	memset(&message, 0, sizeof(message));
	message.command = command;
	message.data = data;
	message.size = size;

	srs_header_setup(&header, &message);

	length = header.length;
	buffer = calloc(1, length);

	memcpy(buffer, &header, sizeof(header));
	if (message.data != NULL && message.size > 0) {
		p = (unsigned char *) buffer + sizeof(header);
		memcpy(p, message.data, message.size);
	}

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	FD_ZERO(&fds);
	FD_SET(client->fd, &fds);

	rc = select(client->fd + 1, NULL, &fds, NULL, &timeout);
	if (rc <= 0 || !FD_ISSET(client->fd, &fds))
		goto error;

	rc = write(client->fd, buffer, length);
	if (rc < (int) length)
		goto error;

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	SRS_CLIENT_UNLOCK(client);

	return rc;
}

int srs_client_recv(struct srs_client *client, struct srs_message *message)
{
	struct srs_header *header;
	void *buffer = NULL;
	size_t length;
	struct timeval timeout;
	fd_set fds;
	unsigned char *p;
	int rc;

	if (client == NULL || client->fd < 0 || message == NULL)
		return -1;

	SRS_CLIENT_LOCK(client);

	length = SRS_BUFFER_LENGTH;
	buffer= calloc(1, length);

	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_usec = 300;

	FD_ZERO(&fds);
	FD_SET(client->fd, &fds);

	rc = select(client->fd + 1, &fds, NULL, NULL, &timeout);
	if (rc <= 0 || !FD_ISSET(client->fd, &fds))
		goto error;

	rc = read(client->fd, buffer, length);
	if (rc < (int) sizeof(struct srs_header))
		goto error;

	header = (struct srs_header *) buffer;

	srs_message_setup(header, message);

	length = header->length - sizeof(struct srs_header);
	if (length > 0) {
		message->size = length;
		message->data = calloc(1, length);

		p = (unsigned char *) buffer + sizeof(struct srs_header);
		memcpy(message->data, p, length);
	}

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (buffer != NULL)
		free(buffer);

	SRS_CLIENT_UNLOCK(client);

	return rc;
}

int srs_client_loop(struct srs_client *client)
{
	struct srs_message message;
	unsigned long int event;
	fd_set fds;
	int fd_max;
	int rc;

	if (client == NULL || client->callback == NULL)
		return -1;

	while (1) {
		if (client->fd < 0 || client->event_fd < 0)
			return -1;

		FD_ZERO(&fds);
		FD_SET(client->fd, &fds);
		FD_SET(client->event_fd, &fds);

		fd_max = client->fd > client->event_fd ? client->fd : client->event_fd;

		rc = select(fd_max + 1, &fds, NULL, NULL, NULL);
		if (rc < 0)
			return -1;

		if (FD_ISSET(client->event_fd, &fds)) {
			read(client->event_fd, &event, sizeof(event));
			break;
		}

		if (!FD_ISSET(client->fd, &fds))
			continue;

		memset(&message, 0, sizeof(struct srs_message));

		rc = srs_client_recv(client, &message);
		if (rc < 0)
			return -1;

		client->callback(client, &message);

		if (message.data != NULL && message.size > 0)
			free(message.data);
	}

	return 0;
}

void *srs_client_thread(void *data)
{
	struct srs_client *client;
	int rc = 0;
	int i = 0;

	if (data == NULL)
		return NULL;

	client = (struct srs_client *) data;

	do {
		if (rc < 0) {
			rc = srs_client_close(client);
			if (rc < 0)
				break;

			rc = srs_client_open(client);
			if (rc < 0)
				break;
		}

		rc = srs_client_loop(client);

		i++;
	} while (rc < 0 && i < 5);

	return 0;
}

int srs_client_loop_start(struct srs_client *client,
	void (*callback)(struct srs_client *client, struct srs_message *message))
{
	pthread_attr_t attr;
	int event_fd = -1;
	int rc;

	if (client == NULL || callback == NULL)
		return -1;

	SRS_CLIENT_LOCK(client);

	event_fd = eventfd(0, EFD_NONBLOCK);
	if (event_fd < 0)
		goto error;

	client->event_fd = event_fd;
	client->callback = callback;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	rc = pthread_create(&client->thread, &attr, srs_client_thread, (void *) client);
	if (rc != 0)
		goto error;

	rc = 0;
	goto complete;

error:
	if (event_fd >= 0) {
		close(event_fd);
		client->event_fd = -1;
	}

	rc = -1;

complete:
	SRS_CLIENT_UNLOCK(client);

	return rc;
}

int srs_client_loop_stop(struct srs_client *client)
{
	unsigned long int event;

	if (client == NULL || client->event_fd < 0)
		return -1;

	SRS_CLIENT_LOCK(client);

	event = 1;
	write(client->event_fd, &event, sizeof(event));

	SRS_CLIENT_UNLOCK(client);

	pthread_join(client->thread, NULL);

	close(client->event_fd);
	client->event_fd = -1;

	return 0;
}
