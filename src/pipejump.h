#include <curl/curl.h>
#include <stdlib.h>

typedef struct pipejump_client
{
	const char *api_key;
	CURL *curl_handle;
} pipejump_client;

typedef struct pipejump_account
{
	void *dummy;
} pipejump_account;

pipejump_client *pipejump_init(char *api_key);

void pipejump_close(pipejump_client *);

pipejump_account *pipejump_get_account(pipejump_client *);
