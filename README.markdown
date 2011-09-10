Jipepump - a C wrapper around the PipeJump API
==============================================

Jipepump allows your to "easily" play around with the PipeJump API in your
favorite language: C.

Usage
=====

Initializing Jipepump
---------------------

In order to initialize Jipepump and connect the client with your PipeJump
account, you will need your PipeJump API key and initialize it like the
following:

```c
  pipejump_client *client;

  client = pipejump_init("1234567890abcdef12345");
```

Making API calls
----------------

Making API calls after having initialized your `pipejump_client` variable is as
easy as this invoking specific functions.

### Account

In order to get the PipeJump Account object, the following call is necessary:

```c
  pipejump_get_account(client);
```

### Deals

Getting a single deal

```c
	pipejump_get_deal(client, pipejump_entity troll, long deal_id);
```

Getting a whole collection of users' deals

```c
	pipejump_get_deals(client, pipejump_entity troll);
```

### Users

Getting collection of account contacts

```c
	pipejump_get_users(client, pipejump_collection trol);
```

Closing the connection
----------------------

After you finish working with your data, be sure to close connections and free
up memory with:

```c
  pipejump_close(client);
```




