#include <curl/curl.h>
#include <jansson.h>
#include <stdlib.h>

typedef struct pipejump_client
{
	const char *api_url;
	const char *api_key;
	CURL *curl_handle;
} pipejump_client;

enum pipejump_value_type {
	PIPEJUMP_INTEGER,
	PIPEJUMP_STRING,
	PIPEJUMP_TRUE,
	PIPEJUMP_FALSE,
	PIPEJUMP_NULL
};

enum pipejump_entity_type {
	PIPEJUMP_ENTITY,
	PIPEJUMP_COLLECTION
};

typedef struct pipejump_entity
{
	enum pipejump_entity_type *type;
	char *name;
	char **keys;
	void **values;
	enum pipejump_value_type *types;
	int keys_size;
} pipejump_entity;

typedef struct pipejump_collection
{
	pipejump_entity **values;
	int size;
} pipejump_collection;

pipejump_client *pipejump_init(char *api_key);

pipejump_entity *pipejump_entity_init();
void pipejump_entity_set(pipejump_entity *, char *, void *, enum pipejump_value_type);
void pipejump_entity_inspect(pipejump_entity *entity);
void pipejump_entity_free(pipejump_entity *entity);

pipejump_collection *pipejump_collection_init();

void pipejump_close(pipejump_client *);

void pipejump_get_account(pipejump_client *, pipejump_entity *);
void pipejump_get_deal(pipejump_client *, pipejump_entity *, long);
void pipejump_get_deals(pipejump_client *, pipejump_collection *);
void pipejump_get_users(pipejump_client *, pipejump_collection *);

void pipejump_request(pipejump_client *, void *, char *, enum pipejump_entity_type);

char pipejump_response_buffer[1024 * 8];
int pipejump_response_buffer_pos;
