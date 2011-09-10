#include <stdio.h>
#include "pipejump.h"

int main()
{
	pipejump_client *client;
	pipejump_entity *account, *deal;

	client = pipejump_init("key");
	account = pipejump_entity_init();
	deal = pipejump_entity_init();
	pipejump_get_account(client, account);
	pipejump_get_deal(client, deal, 36298);
	pipejump_entity_inspect(account);
	pipejump_entity_inspect(deal);
	pipejump_entity_free(account);
	pipejump_entity_free(deal);
	pipejump_close(client);
	return 0;
}
