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

#include <pthread.h>

#define LOG_TAG "RIL"
#include <utils/Log.h>

#include "samsung-ril.h"

struct ril_client *ril_client_new(struct ril_client_funcs *client_funcs)
{
	struct ril_client *ril_client;
	int rc;

	ril_client = calloc(1, sizeof(struct ril_client));

	if (client_funcs != NULL) {
		ril_client->funcs.create = client_funcs->create;
		ril_client->funcs.destroy = client_funcs->destroy;
		ril_client->funcs.read_loop = client_funcs->read_loop;
	}

	pthread_mutex_init(&(ril_client->mutex), NULL);

	return ril_client;
}

int ril_client_free(struct ril_client *client)
{
	if (client == NULL)
		return -1;

	pthread_mutex_destroy(&(client->mutex));

	free(client);

	return 0;
}

int ril_client_create(struct ril_client *client)
{
	int rc;
	int c;

	if (client == NULL || client->funcs.create == NULL)
		return -1;

	for (c = RIL_CLIENT_MAX_TRIES ; c > 0 ; c--) {
		LOGD("Creating RIL client inners, try #%d", RIL_CLIENT_MAX_TRIES - c + 1);

		rc = client->funcs.create(client);
		if (rc < 0)
			LOGE("RIL client inners creation failed");
		else
			break;

		usleep(500000);
	}

	if (c == 0) {
		LOGE("RIL client inners creation failed too many times");
		client->state = RIL_CLIENT_ERROR;
		return -1;
	}

	client->state = RIL_CLIENT_CREATED;

	return 0;
}

int ril_client_destroy(struct ril_client *client)
{
	int rc;
	int c;

	if (client == NULL || client->funcs.destroy == NULL)
		return -1;

	for (c = RIL_CLIENT_MAX_TRIES ; c > 0 ; c--) {
		LOGD("Destroying RIL client inners, try #%d", RIL_CLIENT_MAX_TRIES - c + 1);

		rc = client->funcs.destroy(client);
		if (rc < 0)
			LOGE("RIL client inners destroying failed");
		else
			break;

		usleep(500000);
	}

	if (c == 0) {
		LOGE("RIL client inners destroying failed too many times");
		client->state = RIL_CLIENT_ERROR;
		return -1;
	}

	client->state = RIL_CLIENT_DESTROYED;

	return 0;
}

void *ril_client_thread(void *data)
{
	struct ril_client *client;
	int rc;
	int c;

	if (data == NULL)
		return NULL;

	client = (struct ril_client *) data;

	if (client->funcs.read_loop == NULL)
		return NULL;

	for (c = RIL_CLIENT_MAX_TRIES ; c > 0 ; c--) {
		client->state = RIL_CLIENT_READY;

		rc = client->funcs.read_loop(client);
		if (rc < 0) {
			client->state = RIL_CLIENT_ERROR;

			LOGE("RIL client read loop failed");

			ril_client_destroy(client);
			ril_client_create(client);

			continue;
		} else {
			client->state = RIL_CLIENT_CREATED;

			LOGD("RIL client read loop ended");
			break;
		}
	}

	if (c == 0) {
		LOGE("RIL client read loop failed too many times");
		client->state = RIL_CLIENT_ERROR;
	}

	// Destroy everything here

	rc = ril_client_destroy(client);
	if (rc < 0)
		LOGE("RIL client destroy failed");

	rc = ril_client_free(client);
	if (rc < 0)
		LOGE("RIL client free failed");

	return 0;
}

int ril_client_thread_start(struct ril_client *client)
{
	pthread_attr_t attr;
	int rc;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	rc = pthread_create(&(client->thread), &attr, ril_client_thread, (void *) client);

	if (rc != 0) {
		LOGE("RIL client thread creation failed");
		return -1;
	}

	return 0;
}
