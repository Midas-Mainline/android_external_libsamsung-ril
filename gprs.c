/*
 * This file is part of Samsung-RIL.
 *
 * Copyright (C) 2011-2013 Paul Kocialkowski <contact@paulk.fr>
 * Copyright (C) 2011 Denis 'GNUtoo' Carikli <GNUtoo@no-log.org>
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

#include <netinet/in.h>
#include <arpa/inet.h>

#define LOG_TAG "RIL-GPRS"
#include <utils/Log.h>
#include <cutils/properties.h>

#if RIL_VERSION >= 6
#include <netutils/ifc.h>
#endif

#include "samsung-ril.h"
#include "util.h"

#if RIL_VERSION >= 6
RIL_DataCallFailCause ipc2ril_gprs_fail_cause(unsigned short fail_cause)
#else
RIL_LastDataCallActivateFailCause ipc2ril_gprs_fail_cause(unsigned short fail_cause)
#endif
{
	switch (fail_cause) {

		case IPC_GPRS_FAIL_INSUFFICIENT_RESOURCES:
			return PDP_FAIL_INSUFFICIENT_RESOURCES;
		case IPC_GPRS_FAIL_MISSING_UKNOWN_APN:
			return PDP_FAIL_MISSING_UKNOWN_APN;
		case IPC_GPRS_FAIL_UNKNOWN_PDP_ADDRESS_TYPE:
			return PDP_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;
		case IPC_GPRS_FAIL_USER_AUTHENTICATION:
			return PDP_FAIL_USER_AUTHENTICATION;
		case IPC_GPRS_FAIL_ACTIVATION_REJECT_GGSN:
			return PDP_FAIL_ACTIVATION_REJECT_GGSN;
		case IPC_GPRS_FAIL_ACTIVATION_REJECT_UNSPECIFIED:
			return PDP_FAIL_ACTIVATION_REJECT_UNSPECIFIED;
		case IPC_GPRS_FAIL_SERVICE_OPTION_NOT_SUPPORTED:
			return PDP_FAIL_SERVICE_OPTION_NOT_SUPPORTED;
		case IPC_GPRS_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED:
			return PDP_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED;
		case IPC_GPRS_FAIL_SERVICE_OPTION_OUT_OF_ORDER:
			return PDP_FAIL_SERVICE_OPTION_OUT_OF_ORDER;
		case IPC_GPRS_FAIL_NSAPI_IN_USE:
			return PDP_FAIL_NSAPI_IN_USE;
		default:
			return PDP_FAIL_ERROR_UNSPECIFIED;
	}
}

int ipc2ril_gprs_connection_active(unsigned char state)
{
	switch (state) {
		case IPC_GPRS_STATE_DISABLED:
			return 1;
		case IPC_GPRS_STATE_ENABLED:
			return 2;
		case IPC_GPRS_STATE_NOT_ENABLED:
		default:
			return 0;
	}
}

int ril_gprs_connection_register(int cid)
{
	struct ril_gprs_connection *gprs_connection;
	struct list_head *list_end;
	struct list_head *list;

	gprs_connection = calloc(1, sizeof(struct ril_gprs_connection));
	if (gprs_connection == NULL)
		return -1;

	gprs_connection->cid = cid;

	list_end = ril_data.gprs_connections;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = list_head_alloc((void *) gprs_connection, list_end, NULL);

	if (ril_data.gprs_connections == NULL)
		ril_data.gprs_connections = list;

	return 0;
}

void ril_gprs_connection_unregister(struct ril_gprs_connection *gprs_connection)
{
	struct list_head *list;

	if (gprs_connection == NULL)
		return;

	list = ril_data.gprs_connections;
	while (list != NULL) {
		if (list->data == (void *) gprs_connection) {
			memset(gprs_connection, 0, sizeof(struct ril_gprs_connection));
			free(gprs_connection);

			if (list == ril_data.gprs_connections)
				ril_data.gprs_connections = list->next;

			list_head_free(list);

			break;
		}
list_continue:
		list = list->next;
	}
}

struct ril_gprs_connection *ril_gprs_connection_find_cid(int cid)
{
	struct ril_gprs_connection *gprs_connection;
	struct list_head *list;

	list = ril_data.gprs_connections;
	while (list != NULL) {
		gprs_connection = (struct ril_gprs_connection *) list->data;
		if (gprs_connection == NULL)
			goto list_continue;

		if (gprs_connection->cid == cid)
			return gprs_connection;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct ril_gprs_connection *ril_gprs_connection_find_token(RIL_Token t)
{
	struct ril_gprs_connection *gprs_connection;
	struct list_head *list;

	list = ril_data.gprs_connections;
	while (list != NULL) {
		gprs_connection = (struct ril_gprs_connection *) list->data;
		if (gprs_connection == NULL)
			goto list_continue;

		if (gprs_connection->token == t)
			return gprs_connection;

list_continue:
		list = list->next;
	}

	return NULL;
}

struct ril_gprs_connection *ril_gprs_connection_start(void)
{
	struct ipc_client_gprs_capabilities gprs_capabilities;
	struct ril_gprs_connection *gprs_connection;
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;
	struct list_head *list;
	int cid, cid_max;
	int rc;
	int i;

	if (ril_data.ipc_fmt_client == NULL || ril_data.ipc_fmt_client->data == NULL)
		return NULL;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_fmt_client->data;

	if (ipc_client_data->ipc_client == NULL)
		return NULL;

	ipc_client = ipc_client_data->ipc_client;

	ipc_client_gprs_get_capabilities(ipc_client, &gprs_capabilities);
	cid_max = gprs_capabilities.cid_max;

	for (i = 0 ; i < cid_max ; i++) {
		cid = i + 1;
		list = ril_data.gprs_connections;
		while (list != NULL) {
			if (list->data == NULL)
				goto list_continue;

			gprs_connection = (struct ril_gprs_connection *) list->data;
			if (gprs_connection->cid == cid) {
				cid = 0;
				break;
			}

list_continue:
			list = list->next;
		}

		if (cid > 0)
			break;
	}

	if (cid <= 0) {
		LOGE("Unable to find an unused cid, aborting");
		return NULL;
	}

	LOGD("Using GPRS connection cid: %d", cid);
	rc = ril_gprs_connection_register(cid);
	if (rc < 0)
		return NULL;

	gprs_connection = ril_gprs_connection_find_cid(cid);
	return gprs_connection;
}

void ril_gprs_connection_stop(struct ril_gprs_connection *gprs_connection)
{
	if (gprs_connection == NULL)
		return;

	if (gprs_connection->interface != NULL)
		free(gprs_connection->interface);

	ril_gprs_connection_unregister(gprs_connection);
}

void ipc_gprs_pdp_context_enable_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	struct ril_gprs_connection *gprs_connection;
	int rc;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	gprs_connection = ril_gprs_connection_find_token(ril_request_get_token(info->aseq));

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		LOGE("There was an error, aborting PDP context complete");

		gprs_connection->fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;
		gprs_connection->token = RIL_TOKEN_NULL;
		ril_data.state.gprs_last_failed_cid = gprs_connection->cid;

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	LOGD("Waiting for IP configuration!");
}

void ipc_gprs_define_pdp_context_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	struct ril_gprs_connection *gprs_connection;
	struct ril_request_info *request;
	int aseq;
	int rc;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	gprs_connection = ril_gprs_connection_find_token(ril_request_get_token(info->aseq));

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		LOGE("There was an error, aborting define PDP context complete");

		gprs_connection->fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;
		gprs_connection->token = RIL_TOKEN_NULL;
		ril_data.state.gprs_last_failed_cid = gprs_connection->cid;

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	request = ril_request_info_find_id(info->aseq);
	aseq = ril_request_id_get();

	if (request != NULL)
		request->id = aseq;

	ipc_gen_phone_res_expect_to_func(aseq, IPC_GPRS_PDP_CONTEXT,
		ipc_gprs_pdp_context_enable_complete);

	ipc_fmt_send(IPC_GPRS_PDP_CONTEXT, IPC_TYPE_SET,
			(void *) &(gprs_connection->context),
			sizeof(struct ipc_gprs_pdp_context_set), aseq);
}

void ipc_gprs_port_list_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	struct ril_gprs_connection *gprs_connection;
	struct ril_request_info *request;
	int aseq;
	int rc;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	gprs_connection = ril_gprs_connection_find_token(ril_request_get_token(info->aseq));

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		LOGE("There was an error, aborting port list complete");

		gprs_connection->fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;
		gprs_connection->token = RIL_TOKEN_NULL;
		ril_data.state.gprs_last_failed_cid = gprs_connection->cid;

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	request = ril_request_info_find_id(info->aseq);
	aseq = ril_request_id_get();

	if (request != NULL)
		request->id = aseq;
	ipc_gen_phone_res_expect_to_func(aseq, IPC_GPRS_DEFINE_PDP_CONTEXT,
		ipc_gprs_define_pdp_context_complete);

	ipc_fmt_send(IPC_GPRS_DEFINE_PDP_CONTEXT, IPC_TYPE_SET,
		(void *) &(gprs_connection->define_context),
		sizeof(struct ipc_gprs_define_pdp_context),
		aseq);
}

void ril_request_setup_data_call(RIL_Token t, void *data, int length)
{
	struct ril_gprs_connection *gprs_connection = NULL;
	struct ipc_client_gprs_capabilities gprs_capabilities;
	struct ipc_gprs_port_list port_list;
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	char *username = NULL;
	char *password = NULL;
	char *apn = NULL;

	if (data == NULL || length < (int) (4 * sizeof(char *)))
		goto error;

	if (ril_data.ipc_fmt_client == NULL || ril_data.ipc_fmt_client->data == NULL)
		goto error;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_fmt_client->data;

	if (ipc_client_data->ipc_client == NULL)
		goto error;

	ipc_client = ipc_client_data->ipc_client;

	apn = ((char **) data)[2];
	username = ((char **) data)[3];
	password = ((char **) data)[4];

	LOGD("Requesting data connection to APN '%s'\n", apn);

	gprs_connection = ril_gprs_connection_start();

	if (!gprs_connection) {
		LOGE("Unable to create GPRS connection, aborting");

		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	gprs_connection->token = t;

	// Create the structs with the apn
	ipc_gprs_define_pdp_context_setup(&(gprs_connection->define_context),
		gprs_connection->cid, 1, apn);

	// Create the structs with the username/password tuple
	ipc_gprs_pdp_context_setup(&(gprs_connection->context),
		gprs_connection->cid, 1, username, password);

	ipc_client_gprs_get_capabilities(ipc_client, &gprs_capabilities);

	// If the device has the capability, deal with port list
	if (gprs_capabilities.port_list) {
		ipc_gprs_port_list_setup(&port_list);

		ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_GPRS_PORT_LIST,
			ipc_gprs_port_list_complete);

		ipc_fmt_send(IPC_GPRS_PORT_LIST, IPC_TYPE_SET,
			(void *) &port_list, sizeof(struct ipc_gprs_port_list), ril_request_get_id(t));
	} else {
		ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_GPRS_DEFINE_PDP_CONTEXT,
			ipc_gprs_define_pdp_context_complete);

		ipc_fmt_send(IPC_GPRS_DEFINE_PDP_CONTEXT, IPC_TYPE_SET,
			(void *) &(gprs_connection->define_context),
				sizeof(struct ipc_gprs_define_pdp_context), ril_request_get_id(t));
	}

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_gprs_ip_configuration(struct ipc_message_info *info)
{
	struct ril_gprs_connection *gprs_connection;
	struct ipc_gprs_ip_configuration *ip_configuration;

	if (info->data == NULL || info->length < sizeof(struct ipc_gprs_ip_configuration))
		goto error;

	ip_configuration = (struct ipc_gprs_ip_configuration *) info->data;

	gprs_connection = ril_gprs_connection_find_cid(ip_configuration->cid);

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	LOGD("Obtained IP Configuration");

	// Copy the obtained IP configuration to the GPRS connection structure
	memcpy(&(gprs_connection->ip_configuration),
		ip_configuration, sizeof(struct ipc_gprs_ip_configuration));

	LOGD("Waiting for GPRS call status");

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ipc_gprs_pdp_context_disable_complete(struct ipc_message_info *info)
{
	struct ipc_gen_phone_res *phone_res;
	struct ril_gprs_connection *gprs_connection;
	int rc;

	phone_res = (struct ipc_gen_phone_res *) info->data;
	gprs_connection = ril_gprs_connection_find_token(ril_request_get_token(info->aseq));

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	rc = ipc_gen_phone_res_check(phone_res);
	if (rc < 0) {
		LOGE("There was an error, aborting PDP context complete");

		// RILJ is not going to ask for fail reason
		ril_gprs_connection_stop(gprs_connection);

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	LOGD("Waiting for GPRS call status");
}

void ril_request_deactivate_data_call(RIL_Token t, void *data, int length)
{
	struct ril_gprs_connection *gprs_connection;
	struct ipc_gprs_pdp_context_set context;

	char *cid;
	int rc;

	if (data == NULL || length < (int) sizeof(char *))
		goto error;

	cid = ((char **) data)[0];

	gprs_connection = ril_gprs_connection_find_cid(atoi(cid));

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	gprs_connection->token = t;

	ipc_gprs_pdp_context_setup(&context, gprs_connection->cid, 0, NULL, NULL);

	ipc_gen_phone_res_expect_to_func(ril_request_get_id(t), IPC_GPRS_PDP_CONTEXT,
		ipc_gprs_pdp_context_disable_complete);

	ipc_fmt_send(IPC_GPRS_PDP_CONTEXT, IPC_TYPE_SET,
		(void *) &context, sizeof(struct ipc_gprs_pdp_context_set), ril_request_get_id(t));

	return;

error:
	ril_request_complete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
}

#if RIL_VERSION >= 6
int ipc_gprs_connection_enable(struct ril_gprs_connection *gprs_connection,
	RIL_Data_Call_Response_v6 *setup_data_call_response)
#else
int ipc_gprs_connection_enable(struct ril_gprs_connection *gprs_connection,
	char **setup_data_call_response)
#endif
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;
	struct ipc_gprs_ip_configuration *ip_configuration;

	char *interface = NULL;
	char *ip;
	char *gateway;
	char *subnet_mask;
	in_addr_t subnet_mask_addr;
	char *dns1;
	char *dns2;

	char prop_name[PROPERTY_KEY_MAX];

	int rc;

	if (gprs_connection == NULL || setup_data_call_response == NULL)
		return -EINVAL;

	if (ril_data.ipc_fmt_client == NULL || ril_data.ipc_fmt_client->data == NULL)
		return -EINVAL;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_fmt_client->data;

	if (ipc_client_data->ipc_client == NULL)
		return -EINVAL;

	ipc_client = ipc_client_data->ipc_client;

	ip_configuration = &(gprs_connection->ip_configuration);

	asprintf(&ip, "%i.%i.%i.%i",
		(ip_configuration->ip)[0],
		(ip_configuration->ip)[1],
		(ip_configuration->ip)[2],
		(ip_configuration->ip)[3]);

	// FIXME: gateway isn't reliable!
	asprintf(&gateway, "%i.%i.%i.%i",
		(ip_configuration->ip)[0],
		(ip_configuration->ip)[1],
		(ip_configuration->ip)[2],
		(ip_configuration->ip)[3]);

	// FIXME: subnet isn't reliable!
	asprintf(&subnet_mask, "255.255.255.255");

	asprintf(&dns1, "%i.%i.%i.%i",
		(ip_configuration->dns1)[0],
		(ip_configuration->dns1)[1],
		(ip_configuration->dns1)[2],
		(ip_configuration->dns1)[3]);
	asprintf(&dns2, "%i.%i.%i.%i",
		(ip_configuration->dns2)[0],
		(ip_configuration->dns2)[1],
		(ip_configuration->dns2)[2],
		(ip_configuration->dns2)[3]);	

	if (ipc_client_gprs_handlers_available(ipc_client)) {
		rc = ipc_client_gprs_activate(ipc_client, gprs_connection->cid);
		if (rc < 0) {
			// This is not a critical issue
			LOGE("Failed to activate interface!");
		}
	}

	interface = ipc_client_gprs_get_iface(ipc_client, gprs_connection->cid);
	if (interface == NULL) {
		// This is not a critical issue, fallback to rmnet
		LOGE("Failed to get interface name!");
		asprintf(&interface, "rmnet%d", gprs_connection->cid - 1);
	}

	if (gprs_connection->interface == NULL && interface != NULL) {
		gprs_connection->interface = strdup(interface);
	}

	LOGD("Using net interface: %s\n", interface);

	LOGD("GPRS configuration: iface: %s, ip:%s, "
			"gateway:%s, subnet_mask:%s, dns1:%s, dns2:%s",
		interface, ip, gateway, subnet_mask, dns1, dns2);

	subnet_mask_addr = inet_addr(subnet_mask);

#if RIL_VERSION >= 6
	rc = ifc_configure(interface, inet_addr(ip),
		ipv4NetmaskToPrefixLength(subnet_mask_addr),
		inet_addr(gateway),
		inet_addr(dns1), inet_addr(dns2));
#else
	rc = ifc_configure(interface, inet_addr(ip),
		subnet_mask_addr,
		inet_addr(gateway),
		inet_addr(dns1), inet_addr(dns2));
#endif

	if (rc < 0) {
		LOGE("ifc_configure failed");

		free(interface);
		return -1;
	}

	snprintf(prop_name, PROPERTY_KEY_MAX, "net.%s.dns1", interface);
	property_set(prop_name, dns1);
	snprintf(prop_name, PROPERTY_KEY_MAX, "net.%s.dns2", interface);
	property_set(prop_name, dns2);
	snprintf(prop_name, PROPERTY_KEY_MAX, "net.%s.gw", interface);
	property_set(prop_name, gateway);

#if RIL_VERSION >= 6
	setup_data_call_response->status = 0;
	setup_data_call_response->cid = gprs_connection->cid;
	setup_data_call_response->active = 1;
	setup_data_call_response->type = strdup("IP");

	setup_data_call_response->ifname = interface;
	setup_data_call_response->addresses = ip;
	setup_data_call_response->gateways = gateway;
	asprintf(&setup_data_call_response->dnses, "%s %s", dns1, dns2);
#else
	asprintf(&(setup_data_call_response[0]), "%d", gprs_connection->cid);
	setup_data_call_response[1] = interface;
	setup_data_call_response[2] = ip;

	free(gateway);
#endif

	free(subnet_mask);
	free(dns1);
	free(dns2);

	return 0;
}

int ipc_gprs_connection_disable(struct ril_gprs_connection *gprs_connection)
{
	struct ipc_client_data *ipc_client_data;
	struct ipc_client *ipc_client;

	char *interface;
	int rc;

	if (gprs_connection == NULL)
		return -EINVAL;

	if (ril_data.ipc_fmt_client == NULL || ril_data.ipc_fmt_client->data == NULL)
		return -EINVAL;

	ipc_client_data = (struct ipc_client_data *) ril_data.ipc_fmt_client->data;

	if (ipc_client_data->ipc_client == NULL)
		return -EINVAL;

	ipc_client = ipc_client_data->ipc_client;

	if (gprs_connection->interface == NULL) {
		interface = ipc_client_gprs_get_iface(ipc_client, gprs_connection->cid);
		if (interface == NULL) {
			// This is not a critical issue, fallback to rmnet
			LOGE("Failed to get interface name!");
			asprintf(&interface, "rmnet%d", gprs_connection->cid);
		}
	} else {
		interface = gprs_connection->interface;
	}

	LOGD("Using net interface: %s\n", interface);

	rc = ifc_down(interface);

	if (gprs_connection->interface == NULL)
		free(interface);

	if (rc < 0) {
		LOGE("ifc_down failed");
	}

	if (ipc_client_gprs_handlers_available(ipc_client)) {
		rc = ipc_client_gprs_deactivate(ipc_client, gprs_connection->cid);
		if (rc < 0) {
			// This is not a critical issue
			LOGE("Failed to deactivate interface!");
		}
	}

	return 0;
}

#if RIL_VERSION >= 6
void ril_data_call_response_free(RIL_Data_Call_Response_v6 *response)
#else
void ril_data_call_response_free(RIL_Data_Call_Response *response)
#endif
{
	if (response == NULL)
		return;

	if (response->type != NULL)
		free(response->type);

#if RIL_VERSION >= 6
	if (response->addresses)
		free(response->addresses);
	if (response->ifname)
		free(response->ifname);
	if (response->dnses)
		free(response->dnses);
	if (response->gateways)
		free(response->gateways);
#else
	if (response->apn)
		free(response->apn);
	if (response->address)
		free(response->address);
#endif
}

void ipc_gprs_call_status(struct ipc_message_info *info)
{
	struct ril_gprs_connection *gprs_connection;
	struct ipc_gprs_call_status *call_status;

#if RIL_VERSION >= 6
	RIL_Data_Call_Response_v6 setup_data_call_response;
#else
	char *setup_data_call_response[3] = { NULL, NULL, NULL };
#endif

	int rc;

	if (info->data == NULL || info->length < sizeof(struct ipc_gprs_call_status))
		goto error;

	call_status = (struct ipc_gprs_call_status *) info->data;

	memset(&setup_data_call_response, 0, sizeof(setup_data_call_response));

	gprs_connection = ril_gprs_connection_find_cid(call_status->cid);

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection, aborting");

		ril_request_complete(ril_request_get_token(info->aseq),
			RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}

	if (call_status->fail_cause == 0) {
		if (!gprs_connection->enabled &&
			call_status->state == IPC_GPRS_STATE_ENABLED &&
			gprs_connection->token != RIL_TOKEN_NULL) {
			LOGD("GPRS connection is now enabled");

			rc = ipc_gprs_connection_enable(gprs_connection,
				&setup_data_call_response);
			if (rc < 0) {
				LOGE("Failed to enable and configure GPRS interface");

				gprs_connection->enabled = 0;
				gprs_connection->fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;
				ril_data.state.gprs_last_failed_cid = gprs_connection->cid;

				ril_request_complete(gprs_connection->token,
					RIL_E_GENERIC_FAILURE, NULL, 0);
			} else {
				LOGD("GPRS interface enabled");

				gprs_connection->enabled = 1;

				ril_request_complete(gprs_connection->token,
					RIL_E_SUCCESS, &setup_data_call_response,
					sizeof(setup_data_call_response));
				gprs_connection->token = RIL_TOKEN_NULL;
			}
#if RIL_VERSION >= 6
			ril_data_call_response_free(&setup_data_call_response);
#else
			if (setup_data_call_response[0] != NULL)
				free(setup_data_call_response[0]);
			if (setup_data_call_response[1] != NULL)
				free(setup_data_call_response[1]);
			if (setup_data_call_response[2] != NULL)
				free(setup_data_call_response[2]);
#endif
		} else if (gprs_connection->enabled &&
			call_status->state == IPC_GPRS_STATE_DISABLED &&
			gprs_connection->token != RIL_TOKEN_NULL) {
			LOGD("GPRS connection is now disabled");

			rc = ipc_gprs_connection_disable(gprs_connection);
			if (rc < 0) {
				LOGE("Failed to disable GPRS interface");

				ril_request_complete(gprs_connection->token,
					RIL_E_GENERIC_FAILURE, NULL, 0);

				// RILJ is not going to ask for fail reason
				ril_gprs_connection_stop(gprs_connection);
			} else {
				LOGD("GPRS interface disabled");

				gprs_connection->enabled = 0;

				ril_request_complete(gprs_connection->token,
					RIL_E_SUCCESS, NULL, 0);

				ril_gprs_connection_stop(gprs_connection);
			}
		} else {
			LOGE("GPRS connection reported as changed though state is not OK:"
			"\n\tgprs_connection->enabled=%d\n\tgprs_connection->token=0x%x",
				gprs_connection->enabled, (unsigned)gprs_connection->token);

			ril_unsol_data_call_list_changed();
		}
	} else {
		if (!gprs_connection->enabled &&
			(call_status->state == IPC_GPRS_STATE_NOT_ENABLED ||
			call_status->state == IPC_GPRS_STATE_DISABLED) &&
			gprs_connection->token != RIL_TOKEN_NULL) {
			LOGE("Failed to enable GPRS connection");

			gprs_connection->enabled = 0;
			gprs_connection->fail_cause =
				ipc2ril_gprs_fail_cause(call_status->fail_cause);
			ril_data.state.gprs_last_failed_cid = gprs_connection->cid;

			ril_request_complete(gprs_connection->token,
				RIL_E_GENERIC_FAILURE, NULL, 0);
			gprs_connection->token = RIL_TOKEN_NULL;

			ril_unsol_data_call_list_changed();
		} else if (gprs_connection->enabled &&
			call_status->state == IPC_GPRS_STATE_DISABLED) {
			LOGE("GPRS connection suddently got disabled");

			rc = ipc_gprs_connection_disable(gprs_connection);
			if (rc < 0) {
				LOGE("Failed to disable GPRS interface");

				// RILJ is not going to ask for fail reason
				ril_gprs_connection_stop(gprs_connection);
			} else {
				LOGE("GPRS interface disabled");

				gprs_connection->enabled = 0;
				ril_gprs_connection_stop(gprs_connection);
			}

			ril_unsol_data_call_list_changed();
		} else {
			LOGE("GPRS connection reported to have failed though state is OK:"
			"\n\tgprs_connection->enabled=%d\n\tgprs_connection->token=0x%x",
				gprs_connection->enabled, (unsigned)gprs_connection->token);

			ril_unsol_data_call_list_changed();
		}
	}

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_request_last_data_call_fail_cause(RIL_Token t)
{
	struct ril_gprs_connection *gprs_connection;
	int last_failed_cid;
	int fail_cause;

	last_failed_cid = ril_data.state.gprs_last_failed_cid;

	if (!last_failed_cid) {
		LOGE("No GPRS connection was reported to have failed");

		goto fail_cause_unspecified;
	}

	gprs_connection = ril_gprs_connection_find_cid(last_failed_cid);

	if (!gprs_connection) {
		LOGE("Unable to find GPRS connection");

		goto fail_cause_unspecified;
	}

	fail_cause = gprs_connection->fail_cause;

	LOGD("Destroying GPRS connection with cid: %d", gprs_connection->cid);
	ril_gprs_connection_stop(gprs_connection);

	goto fail_cause_return;

fail_cause_unspecified:
	fail_cause = PDP_FAIL_ERROR_UNSPECIFIED;

fail_cause_return:
	ril_data.state.gprs_last_failed_cid = 0;

	ril_request_complete(t, RIL_E_SUCCESS, &fail_cause, sizeof(fail_cause));
}

/*
 * Some modem firmwares have a bug that will make the first cid (1) overriden
 * by the current cid, thus reporting it twice, with a wrong 2nd status.
 *
 * This shouldn't change anything to healthy structures.
 */
#if RIL_VERSION >= 6
void ipc_gprs_pdp_context_fix(RIL_Data_Call_Response_v6 *data_call_list, int c)
#else
void ipc_gprs_pdp_context_fix(RIL_Data_Call_Response *data_call_list, int c)
#endif
{
	int i, j, k;

	for (i = 0 ; i < c ; i++) {
		for (j = i-1 ; j >= 0 ; j--) {
			if (data_call_list[i].cid == data_call_list[j].cid) {
				for (k = 0 ; k < c ; k++) {
					if (data_call_list[k].cid == 1) {
						data_call_list[i].cid = 0;
						break;
					}
				}

				data_call_list[i].cid = 1;
			}
		}
	}
}

void ipc_gprs_pdp_context(struct ipc_message_info *info)
{
	struct ril_gprs_connection *gprs_connection;
	struct ipc_gprs_ip_configuration *ip_configuration;
	struct ipc_gprs_pdp_context_get *context;

#if RIL_VERSION >= 6
	RIL_Data_Call_Response_v6 data_call_list[IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT];
#else
	RIL_Data_Call_Response data_call_list[IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT];
#endif

	memset(data_call_list, 0, sizeof(data_call_list));

	int i;

	if (info->data == NULL || info->length < sizeof(struct ipc_gprs_pdp_context_get))
		goto error;

	context = (struct ipc_gprs_pdp_context_get *) info->data;

	for (i = 0 ; i < IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT ; i++) {
		data_call_list[i].cid = context->desc[i].cid;
		data_call_list[i].active =
			ipc2ril_gprs_connection_active(context->desc[i].state);

		if (context->desc[i].state == IPC_GPRS_STATE_ENABLED) {
			gprs_connection = ril_gprs_connection_find_cid(context->desc[i].cid);

			if (gprs_connection == NULL) {
				LOGE("CID %d reported as enabled but not listed here",
					context->desc[i].cid);
				continue;
			}

			ip_configuration = &(gprs_connection->ip_configuration);

			char *addr = NULL;
			asprintf(&addr, "%i.%i.%i.%i",
				(ip_configuration->ip)[0],
				(ip_configuration->ip)[1],
				(ip_configuration->ip)[2],
				(ip_configuration->ip)[3]);

#if RIL_VERSION >= 6
			RIL_Data_Call_Response_v6 *resp = &data_call_list[i];
#else
			RIL_Data_Call_Response *resp = &data_call_list[i];
#endif

			resp->type = strdup("IP");

#if RIL_VERSION < 6
			resp->address = addr;
			asprintf(&(resp->apn), "%s",
				gprs_connection->define_context.apn);
#else
			resp->addresses = addr;
			resp->gateways = strdup(addr);
			resp->ifname = strdup(gprs_connection->interface);
			asprintf(&resp->dnses, "%i.%i.%i.%i %i.%i.%i.%i",
				ip_configuration->dns1[0],
				ip_configuration->dns1[1],
				ip_configuration->dns1[2],
				ip_configuration->dns1[3],

				ip_configuration->dns2[0],
				ip_configuration->dns2[1],
				ip_configuration->dns2[2],
				ip_configuration->dns2[3]);
#endif
		}
	}

	ipc_gprs_pdp_context_fix(data_call_list, IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT);

	if (info->aseq == 0xff)
		ril_request_unsolicited(RIL_UNSOL_DATA_CALL_LIST_CHANGED,
			&data_call_list, sizeof(data_call_list));
	else
		ril_request_complete(ril_request_get_token(info->aseq), RIL_E_SUCCESS,
			&data_call_list, sizeof(data_call_list));

	for (i = 0; i < IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT; i++) {
		ril_data_call_response_free(data_call_list + i);
	}

	return;

error:
	ril_request_complete(ril_request_get_token(info->aseq), RIL_E_GENERIC_FAILURE, NULL, 0);
}

void ril_unsol_data_call_list_changed(void)
{
	ipc_fmt_send_get(IPC_GPRS_PDP_CONTEXT, 0xff);
}

void ril_request_data_call_list(RIL_Token t)
{
	ipc_fmt_send_get(IPC_GPRS_PDP_CONTEXT, ril_request_get_id(t));
}
