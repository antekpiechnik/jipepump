#include <stdio.h>
#include "pipejump.h"

int main(int argc, char **argv)
{
	pipejump_client *client;
	pipejump_entity *account;

	client = pipejump_init("key");
	account = pipejump_get_account(client);
	pipejump_entity_inspect(account);
	pipejump_close(client);
	return 0;
}
