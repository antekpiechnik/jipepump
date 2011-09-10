#include <stdio.h>
#include "pipejump.h"

int main()
{
	pipejump_client *client;
	pipejump_collection *collection;
	int i;

	client = pipejump_init("key");
	collection = pipejump_collection_init();
	pipejump_get_deals(client, collection);
	i = 0;
	while (i < collection -> size)
	{
		pipejump_entity_inspect(collection -> values[i]);
		i++;
	}
	pipejump_close(client);
	return 0;
}
