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

void pipejump_get_deals(pipejump_client *client, pipejump_collection *deals)
{
	pipejump_request(client, deals, "deals", PIPEJUMP_COLLECTION);
}

void pipejump_get_users(pipejump_client *client, pipejump_collection *collection)
{
	pipejump_request(client, collection, "collection", PIPEJUMP_COLLECTION);
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
				pipejump_entity_set(entity, (char *)key, (void *)json_string_value(value), PIPEJUMP_STRING);
				break;
			case JSON_INTEGER:
				pipejump_entity_set(entity, (char *)key, (void *)json_integer_value(value), PIPEJUMP_INTEGER);
				break;
			case JSON_NULL:
				pipejump_entity_set(entity, (char *)key, NULL, PIPEJUMP_NULL);
				break;
			case JSON_TRUE:
				pipejump_entity_set(entity, (char *)key, NULL, PIPEJUMP_TRUE);
				break;
			case JSON_FALSE:
				pipejump_entity_set(entity, (char *)key, NULL, PIPEJUMP_FALSE);
				break;
			case JSON_OBJECT:
			case JSON_ARRAY:
			case JSON_REAL:
				break;
		}

		iter = json_object_iter_next(json_entity, iter);
	}
}

void get_multiple_objects(json_t *json_array, pipejump_collection *entities) {
	int i, size;

	size = json_array_size(json_array);

	i = 0;
	for (i = 0; i < size; i++)
	{
		get_single_object(json_array_get(json_array, i), entities -> values[i]);
	}
	entities -> size = i;
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

void *pipejump_entity_get(pipejump_entity *entity, char *key)
{
	int keys_size, i;

	keys_size = entity -> keys_size;

	i = 0;
	while (i < keys_size && strcmp(key, (entity -> keys)[i]))
	{
		i++;
	}
	if (i >= keys_size) return NULL;
	return (entity -> values)[i];
}

int pipejump_entity_save(pipejump_entity *entity)
{
	return 1;
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
	enum pipejump_value_type type;

	current_key = 0;
	printf("<%s:%ld", entity -> name, (unsigned long)pipejump_entity_get(entity, "id"));
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
			case PIPEJUMP_NULL:
				printf("%s:null", key);
				break;
			case PIPEJUMP_TRUE:
				printf("%s:true", key);
				break;
			case PIPEJUMP_FALSE:
				printf("%s:false", key);
				break;
		}
		current_key++;
	}
	printf(">\n");
}

pipejump_collection *pipejump_collection_init()
{
	pipejump_collection *collection;
	int i;
	
	collection = malloc(sizeof(pipejump_collection));
	collection -> values = malloc(sizeof(*(collection -> values)) * 300);
	for (i = 0; i < 300; i++)
	{
		collection -> values[i] = pipejump_entity_init();
	}
	collection -> size = 0;

	return collection;
}

void pipejump_make_request(pipejump_client *client, char *path, void *data)
{
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
			get_multiple_objects(json_entity, (pipejump_collection *)object);
			break;
		default:
			break;
	}

}
