#include "pipejump.h"

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

	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://sales.futuresimple.com/api/v1/account");
	sprintf(auth_header, "X-Pipejump-Auth: %s", client -> api_key);
	headers = curl_slist_append(headers, auth_header);
	headers = curl_slist_append(headers, "Accept: application/json");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

	client -> curl_handle = curl_handle;
	return client;
}

void pipejump_close(pipejump_client *client)
{
	curl_easy_cleanup(client -> curl_handle);
	free(client);
}

pipejump_account *pipejump_get_account(pipejump_client *client)
{
	CURLcode response;
	
	response = curl_easy_perform(client -> curl_handle);
	printf("%d\n", response);
	return NULL;
}
