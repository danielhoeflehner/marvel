#ifndef __url_h__
#define __url_h__

#include "str.h"

typedef struct uri_maker uri_maker;
typedef char *(*hasher_func)(uri_maker *self);
typedef char *(*build_req_func)(uri_maker *self);

struct uri_maker {
  str *endpoint;
  str *query;
  str *ts;
  str *pr_api_key;
  str *pub_api_key;
  hasher_func get_hash;
  build_req_func build_req;
};

uri_maker *uri_maker_create();
void uri_maker_destroy(uri_maker **self);

#endif