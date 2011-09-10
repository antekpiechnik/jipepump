#include <stdio.h>
#include "pipejump.h"

int main(int argc, char **argv)
{
	pipejump_client *client;
	pipejump_collection *collection;
	pipejump_entity *entity;
	int i;

	client = pipejump_init(argv[1]);
	collection = pipejump_collection_init();
	pipejump_get_deals(client, collection);
	i = 0;
	while (i < collection -> size)
	{
		pipejump_entity_inspect(collection -> values[i]);
		i++;
	}
	entity = pipejump_entity_init();
	pipejump_get_deal(client, entity, 34842);
	pipejump_entity_inspect(entity);
	entity = (collection -> values)[0];
	pipejump_close(client);
	return 0;
}
