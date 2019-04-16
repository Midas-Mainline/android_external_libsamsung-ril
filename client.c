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
#include <pthread.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include <samsung-ril.h>

struct ril_client *ril_client_find_id(int id)
{
	unsigned int i;

	if (ril_clients_count == 0)
		return NULL;

	for (i = 0; i < ril_clients_count; i++) {
		if (ril_clients[i] == NULL)
			continue;

		if (ril_clients[i]->id == id)
			return ril_clients[i];
	}

	return NULL;
}

int ril_client_create(struct ril_client *client)
{
	int rc;

	if (client == NULL)
		return -1;

	client->data = NULL;
	pthread_mutex_init(&client->mutex, NULL);

	if (client->handlers != NULL && client->handlers->create != NULL) {
		rc = client->handlers->create(client);
		if (rc < 0) {
			RIL_LOGE("Creating %s client failed", client->name);
			return -1;
		}
	}

	RIL_LOGD("Created %s client", client->name);

	return 0;
}

int ril_client_destroy(struct ril_client *client)
{
	int rc;

	if (client == NULL)
		return -1;

	if (client->handlers != NULL && client->handlers->destroy != NULL) {
		rc = client->handlers->destroy(client);
		if (rc < 0) {
			RIL_LOGE("Destroying %s client failed", client->name);
			return -1;
		}
	}

	pthread_mutex_destroy(&client->mutex);

	RIL_LOGD("Destroyed %s client", client->name);

	return 0;
}

int ril_client_open(struct ril_client *client)
{
	int rc = 0;

	if (client == NULL)
		return -1;

	if (client->handlers == NULL || client->handlers->open == NULL)
		return -1;

	rc = client->handlers->open(client);
	if (rc < 0) {
		RIL_LOGE("Opening %s client failed", client->name);
		return -1;
	}

	RIL_LOGD("Opened %s client", client->name);

	return 0;
}

int ril_client_close(struct ril_client *client)
{
	int rc;

	if (client == NULL)
		return -1;

	if (client->handlers == NULL || client->handlers->close == NULL)
		return -1;

	rc = client->handlers->close(client);
	if (rc < 0) {
		RIL_LOGE("Closing %s client failed", client->name);
		return -1;
	}

	RIL_LOGD("Closed %s client", client->name);

	return 0;
}

void *ril_client_thread(void *data)
{
	struct ril_client *client;
	int rc;

	if (data == NULL || ril_data == NULL)
		return NULL;

	client = (struct ril_client *) data;

	client->failures = 0;

	do {
		if (client->failures) {
			usleep(RIL_CLIENT_RETRY_DELAY);

			RIL_LOCK();

			rc = ril_client_close(client);
			if (rc < 0) {
				RIL_UNLOCK();
				goto failure;
			}

			rc = ril_client_open(client);
			if (rc < 0) {
				RIL_UNLOCK();
				goto failure;
			}

			RIL_UNLOCK();
		}

		rc = client->handlers->loop(client);
		if (rc < 0) {
			RIL_LOGE("%s client loop failed", client->name);
			goto failure;
		} else {
			RIL_LOGE("%s client loop terminated", client->name);
			break;
		}

failure:
		client->failures++;
	} while (client->failures < RIL_CLIENT_RETRY_COUNT);

	RIL_LOCK();

	ril_client_close(client);
	ril_client_destroy(client);

	RIL_UNLOCK();

	RIL_LOGD("Stopped %s client loop", client->name);

	return NULL;
}

int ril_client_loop(struct ril_client *client)
{
	pthread_attr_t attr;
	int rc;

	if (client == NULL)
		return -1;

	if (client->handlers == NULL || client->handlers->loop == NULL)
		return -1;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	rc = pthread_create(&client->thread, &attr, ril_client_thread, (void *) client);
	if (rc != 0) {
		RIL_LOGE("Starting %s client loop failed", client->name);
		return -1;
	}

	RIL_LOGD("Started %s client loop", client->name);

	return 0;
}

int ril_client_request_register(struct ril_client *client, int request,
	RIL_Token token)
{
	int rc = 0;

	if (client == NULL || client->callbacks == NULL || client->callbacks->request_register == NULL)
		return -1;

	rc = client->callbacks->request_register(client, request, token);
	if (rc < 0)
		return -1;

	return 0;
}

int ril_client_request_unregister(struct ril_client *client, int request,
	RIL_Token token)
{
	int rc = 0;

	if (client == NULL || client->callbacks == NULL || client->callbacks->request_unregister == NULL)
		return -1;

	rc = client->callbacks->request_unregister(client, request, token);
	if (rc < 0)
		return -1;

	return 0;
}

int ril_client_flush(struct ril_client *client)
{
	int rc = 0;

	if (client == NULL || client->callbacks == NULL || client->callbacks->flush == NULL)
		return -1;

	rc = client->callbacks->flush(client);
	if (rc < 0)
		return -1;

	return 0;
}
