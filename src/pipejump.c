#include <string.h>
#include "pipejump.h"

size_t fetch_curl_response(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	char *output;

	output = malloc(size * nmemb + 1);
	memcpy(output, ptr, size * nmemb);
	output[size * nmemb] = '\0';
	fprintf(stdout, "%s", output);
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

	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://sales.futuresimple.com/api/v1/account");
	sprintf(auth_header, "X-Pipejump-Auth: %s", client -> api_key);
	headers = curl_slist_append(headers, auth_header);
	headers = curl_slist_append(headers, "Accept: application/json");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, fetch_curl_response);

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
	long response_code;
	
	response = curl_easy_perform(client -> curl_handle);
	curl_easy_getinfo(client -> curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
	printf("%ld\n", response_code);
	return NULL;
}
