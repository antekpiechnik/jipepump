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

void pipejump_get_account(pipejump_client *client, pipejump_entity *account)
{
	pipejump_request(client, account, "account", PIPEJUMP_ENTITY);
}

void pipejump_get_deal(pipejump_client *client, pipejump_entity *deal, long deal_id)
{
	char path[100];
	sprintf(path, "deals/%ld", deal_id);
	pipejump_request(client, deal, path, PIPEJUMP_ENTITY);
}

void get_single_object(json_t *json_entity, pipejump_entity *entity) {
	const char *key;
	const char *namespace;
	json_t *value;
	void *iter;
	
	iter = json_object_iter(json_entity);
	namespace = json_object_iter_key(iter);
	entity -> name = (char *)namespace;
	json_entity = json_object_get(json_entity, namespace);
	iter = json_object_iter(json_entity);
	while (iter)
	{
		key = json_object_iter_key(iter);
		value = json_object_iter_value(iter);
		switch (json_typeof(value))
		{
			case JSON_STRING:
				pipejump_entity_set(entity, key, (void *)json_string_value(value), PIPEJUMP_STRING);
				break;
			case JSON_INTEGER:
				pipejump_entity_set(entity, key, (void *)json_integer_value(value), PIPEJUMP_INTEGER);
				break;
		}

		iter = json_object_iter_next(json_entity, iter);
	}
}

pipejump_entity *pipejump_entity_init()
{
	pipejump_entity *entity;
	char **keys;
	void **values;
	enum pipejump_value_type *types;

	keys = malloc(sizeof(*keys) * 100);
	values = malloc(sizeof(*keys) * 100);
	types = malloc(sizeof(*types) * 100);

	entity = malloc(sizeof(*entity));
	entity -> keys_size = 0;
	entity -> keys = keys;
	entity -> values = values;
	entity -> types = types;
	entity -> type = PIPEJUMP_ENTITY;
	return entity;
}

void pipejump_entity_set(pipejump_entity *entity, char *key, void *value, enum pipejump_value_type type)
{
	int keys_size;

	keys_size = entity -> keys_size;
	(entity -> keys)[keys_size] = key;
	(entity -> values)[keys_size] = value;
	(entity -> types)[keys_size] = type;
	(entity -> keys_size)++;
}

void pipejump_entity_free(pipejump_entity *entity)
{
	free(entity -> keys);
	free(entity -> values);
	free(entity -> types);
	free(entity);
}

void pipejump_entity_inspect(pipejump_entity *entity)
{
	int current_key;
	char *key;
	void *value;
	enum pipejump_entity_type type;

	current_key = 0;
	printf("<%s", entity -> name);
	while (current_key < entity -> keys_size)
	{
		printf(" ");
		key = entity -> keys[current_key];
		value = entity -> values[current_key];
		type = entity -> types[current_key];
		switch (type)
		{
			case PIPEJUMP_STRING:
				printf("%s:\"%s\"", key, (char *)value);
				break;
			case PIPEJUMP_INTEGER:
				printf("%s:%ld", key, (long)value);
				break;
		}
		current_key++;
	}
	printf(">\n");
}

void pipejump_request(pipejump_client *client, void *object, char *path, enum pipejump_entity_type type)
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
			get_single_object(json_entity, (pipejump_entity *)object);
			break;
		case PIPEJUMP_COLLECTION:
			break;
		default:
			break;
	}

}
