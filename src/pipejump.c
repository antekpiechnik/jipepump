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
	json_t *json_entity;

	json_entity = pipejump_request(client, "account");
	return NULL;
}

json_t *pipejump_request(pipejump_client *client, char *path)
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
	return json_entity;
}
