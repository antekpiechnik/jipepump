#include <string.h>
#include "pipejump.h"

size_t pipejump_fetch_curl_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	memcpy(pipejump_response_buffer + pipejump_response_buffer_pos, ptr, size * nmemb);
	pipejump_response_buffer_pos += size * nmemb;
	return size * nmemb;
}

pipejump_client *pipejump_init(char *api_key)
{
	pipejump_client *client;
	CURL *curl_handle;
	struct curl_slist *headers;
	char auth_header[100];

	headers = NULL;

	client = malloc(sizeof(*client));
	client -> api_key = api_key;
	curl_handle = curl_easy_init();

	if (curl_handle == NULL)
	{
		perror("Cannot init curl");
		pipejump_close(client);
		return NULL;
	}

	sprintf(auth_header, "X-Pipejump-Auth: %s", client -> api_key);
	headers = curl_slist_append(headers, auth_header);
	headers = curl_slist_append(headers, "Accept: application/json");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, pipejump_fetch_curl_data);

	client -> curl_handle = curl_handle;
	client -> api_url = "https://sales.futuresimple.com/api/v1";
	return client;
}

void pipejump_close(pipejump_client *client)
{
	curl_easy_cleanup(client -> curl_handle);
	free(client);
}

pipejump_entity *pipejump_get_account(pipejump_client *client)
{
	return pipejump_request(client, "account", PIPEJUMP_ENTITY);
}

pipejump_entity *get_single_object(json_t *json_entity) {
	const char *key;
	const char *namespace;
	json_t *value;
	void *iter;
	pipejump_entity *entity;
	
	iter = json_object_iter(json_entity);
	namespace = json_object_iter_key(iter);
	json_entity = json_object_get(json_entity, namespace);
	iter = json_object_iter(json_entity);
	entity = pipejump_entity_init(namespace);
	while (iter)
	{
		key = json_object_iter_key(iter);
		value = json_object_iter_value(iter);
		pipejump_entity_set(entity, key, json_string_value(value));
		iter = json_object_iter_next(json_entity, iter);
	}
	return entity;
}

pipejump_entity *pipejump_entity_init(const char *name)
{
	pipejump_entity *entity;
	char **keys;
	char **values;
	char *new_name;

	new_name = malloc(sizeof(*name) * strlen(name));
	strcpy(new_name, name);

	keys = malloc(sizeof(*keys) * 100);
	values = malloc(sizeof(*keys) * 100);

	entity = malloc(sizeof(*entity));
	entity -> keys_size = 0;
	entity -> type = new_name;
	entity -> keys = keys;
	entity -> values = values;
	return entity;
}

void pipejump_entity_set(pipejump_entity *entity, const char *key, const char *value)
{
	int keys_size;
	char *new_key;
	char *new_value;

	new_key = malloc(sizeof(*key) * strlen(key));
	strcpy(new_key, key);
	if (value != NULL)
	{
		new_value = malloc(sizeof(*value) * strlen(value));
		strcpy(new_value, value);
	}

	keys_size = entity -> keys_size;
	(entity -> keys)[keys_size] = new_key;
	(entity -> values)[keys_size] = new_value;
	(entity -> keys_size)++;
}

void pipejump_entity_inspect(pipejump_entity *entity)
{
	int current_key;

	current_key = 0;
	printf("<%s>\n", entity -> type);
	while (current_key < entity -> keys_size)
	{
		current_key++;
	}
}

void *pipejump_request(pipejump_client *client, char *path, enum pipejump_entity_type type)
{
	CURLcode response;
	long response_code;
	char full_url[100];
	json_t *json_entity;

	pipejump_response_buffer_pos = 0;

	sprintf(full_url, "%s/%s", client -> api_url, path);
	curl_easy_setopt(client -> curl_handle, CURLOPT_URL, full_url);
	response = curl_easy_perform(client -> curl_handle);
	curl_easy_getinfo(client -> curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
	pipejump_response_buffer[pipejump_response_buffer_pos] = '\0';
	json_entity = json_loadb(pipejump_response_buffer, pipejump_response_buffer_pos, 0, NULL);

	switch(type) {
		case PIPEJUMP_ENTITY:
			return get_single_object(json_entity);
		case PIPEJUMP_COLLECTION:
			return NULL;
		default:
			return NULL;
	}

}
