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
	void *dummy;
} pipejump_entity;

pipejump_client *pipejump_init(char *api_key);

void pipejump_close(pipejump_client *);

pipejump_entity *pipejump_get_account(pipejump_client *);

json_t *pipejump_request(pipejump_client *, char *);

char pipejump_response_buffer[1024 * 8];
int pipejump_response_buffer_pos;
