#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "netstring.h"


/* Good examples */
char ex1[] = "12:hello world!,";
char ex2[] = "3:foo,0:,3:bar,";

/* Bad examples */
char ex3[] = "12:hello world! "; /* No comma */
char ex4[] = "15:hello world!,"; /* Too short */
char ex5[] = "03:foo,";		 /* Leading zeros are forbidden */
char ex6[] = "999999999999999:haha lol,"; /* Too long */
char ex7[] = "3fool,";			  /* No colon */
char ex8[] = "what's up";		  /* No number or colon */
char ex9[] = ":what's up";		  /* No number */


void test_netstring_read(void) {
  char  *base;
  size_t base_len;
  char  *netstring;
  size_t netstring_len;
  int retval;

  /* ex1: hello world */
  base = ex1; base_len = strlen(ex1);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 12); assert(strncmp(netstring, "hello world!", 12) == 0);
  assert(base == ex1 + strlen(ex1)); assert(base_len == 0);
  assert(retval == 0);


  /* ex2: three netstrings, concatenated. */

  base = ex2; base_len = strlen(ex2);

  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 3); assert(strncmp(netstring, "foo", 3) == 0);
  assert(retval == 0);

  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(retval == 0);

  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 3); assert(strncmp(netstring, "bar", 3) == 0);
  assert(retval == 0); assert(base_len == 0);


  /* ex3: no comma */
  base = ex3; base_len = strlen(ex3);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_COMMA);

  /* ex4: too short */
  base = ex4; base_len = strlen(ex4);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_TOO_SHORT);

  /* ex5: leading zero */
  base = ex5; base_len = strlen(ex5);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_LEADING_ZERO);

  /* ex6: too long */
  base = ex6; base_len = strlen(ex6);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_TOO_LONG);

  /* ex7: no colon */
  base = ex7; base_len = strlen(ex7);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_COLON);

  /* ex8: no number or colon */
  base = ex8; base_len = strlen(ex8);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_LENGTH);

  /* ex9: no number */
  base = ex9; base_len = strlen(ex9);
  retval = netstring_read(&base, &base_len, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_LENGTH);
}

void test_netstring_buffer_size(void) {
  assert(netstring_buffer_size(0) == 3);
  assert(netstring_buffer_size(1) == 4);
  assert(netstring_buffer_size(2) == 5);
  assert(netstring_buffer_size(9) == 12);
  assert(netstring_buffer_size(10) == 14);
  assert(netstring_buffer_size(12345) == 12345 + 5 + 2);
}

void test_netstring_encode_new(void) {
  char *ns; size_t bytes;

  bytes = netstring_encode_new(&ns, "foo", 3);
  assert(ns != NULL); assert(strncmp(ns, "3:foo,", 6) == 0); assert(bytes == 6);
  free(ns);

  bytes = netstring_encode_new(&ns, NULL, 0);
  assert(ns != NULL); assert(strncmp(ns, "0:,", 3) == 0); assert(bytes == 3);
  free(ns);

  bytes = netstring_encode_new(&ns, "hello world!", 12); assert(bytes == 16);
  assert(ns != NULL); assert(strncmp(ns, "12:hello world!,", 16) == 0);
  free(ns);
}  

void test_netstring_add_ex(void) {
  char *ns=0; size_t bytes;

  bytes = netstring_add_ex(&ns, "foo", 3);
  assert(ns != NULL); assert(strncmp(ns, "3:foo,", 6) == 0); assert(bytes == 6);
  free(ns); ns = 0;

  bytes = netstring_add_ex(&ns, NULL, 0);
  assert(ns != NULL); assert(strncmp(ns, "0:,", 3) == 0); assert(bytes == 3);
  free(ns); ns = 0;

  bytes = netstring_add_ex(&ns, "hello world!", 12); assert(bytes == 16);
  assert(ns != NULL); assert(strncmp(ns, "12:hello world!,", 16) == 0);
  free(ns); ns = 0;

  bytes = netstring_add_ex(&ns, "hello world!", 5); assert(bytes == 8);
  puts(ns);
  assert(ns != NULL); assert(strncmp(ns, "5:hello,", 8) == 0);
  free(ns); ns = 0;
}

void test_netstring_add(void) {
  char *list=0;

  netstring_add(&list, "first");
  netstring_add(&list, "second");
  netstring_add(&list, "third");
  netstring_add(&list, "");
  assert(strcmp(list, "5:first,6:second,5:third,0:,") == 0);
  free(list);

}

int main(void) {
  printf("Running test suite...\n");
  test_netstring_read();
  test_netstring_buffer_size();
  test_netstring_encode_new();
  test_netstring_add_ex();
  test_netstring_add();
  printf("All tests passed!\n");
  return 0;
}
