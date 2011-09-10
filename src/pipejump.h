#include <curl/curl.h>
#include <jansson.h>
#include <stdlib.h>

typedef struct pipejump_client
{
	const char *api_url;
	const char *api_key;
	CURL *curl_handle;
} pipejump_client;

typedef struct pipejump_entity
{
	char *type;
	char **keys;
	char **values;
	int keys_size;
} pipejump_entity;

enum pipejump_entity_type {
	PIPEJUMP_ENTITY,
	PIPEJUMP_COLLECTION
};

pipejump_client *pipejump_init(char *api_key);

pipejump_entity *pipejump_entity_init(const char *);
void pipejump_entity_set(pipejump_entity *, const char *, const char *);
void pipejump_entity_inspect(pipejump_entity *entity);

void pipejump_close(pipejump_client *);

pipejump_entity *pipejump_get_account(pipejump_client *);

void *pipejump_request(pipejump_client *, char *, enum pipejump_entity_type);

char pipejump_response_buffer[1024 * 8];
int pipejump_response_buffer_pos;
