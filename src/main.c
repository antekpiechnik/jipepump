#include <stdio.h>
#include "pipejump.h"

int main(int argc, char **argv)
{
	pipejump_client *client;

	client = pipejump_init("key");
	pipejump_get_account(client);
	pipejump_close(client);
	return 0;
}
