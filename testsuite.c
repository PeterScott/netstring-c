#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "netstring_stream.h"


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


void run_test_suite(void) {
  char *netstring;
  size_t netstring_len;
  int i, retval;

  /* ex1: hello world */
  retval = read_netstring(ex1, strlen(ex1), &netstring, &netstring_len);
  assert(netstring_len == 12); assert(strncmp(netstring, "hello world!", 12) == 0);
  assert(retval == 0);


  /* ex2: three netstrings, concatenated. */

  retval = read_netstring(ex2, strlen(ex2), &netstring, &netstring_len);
  assert(netstring_len == 3); assert(strncmp(netstring, "foo", 3) == 0);
  assert(retval == 0);

  retval = read_netstring(netstring+netstring_len+1, 9, &netstring, &netstring_len);
  assert(netstring_len == 0); assert(retval == 0);

  retval = read_netstring(netstring+netstring_len+1, 6, &netstring, &netstring_len);
  assert(netstring_len == 3); assert(strncmp(netstring, "bar", 3) == 0);
  assert(retval == 0);


  /* ex3: no comma */
  retval = read_netstring(ex3, strlen(ex3), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_COMMA);

  /* ex4: too short */
  retval = read_netstring(ex4, strlen(ex4), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_TOO_SHORT);

  /* ex5: leading zero */
  retval = read_netstring(ex5, strlen(ex5), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_LEADING_ZERO);

  /* ex6: too long */
  retval = read_netstring(ex6, strlen(ex6), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_TOO_LONG);

  /* ex7: no colon */
  retval = read_netstring(ex7, strlen(ex7), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_COLON);

  /* ex8: no number or colon */
  retval = read_netstring(ex8, strlen(ex8), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_LENGTH);

  /* ex9: no number */
  retval = read_netstring(ex9, strlen(ex9), &netstring, &netstring_len);
  assert(netstring_len == 0); assert(netstring == NULL);
  assert(retval == NETSTRING_ERROR_NO_LENGTH);
}
  
int main(void) {
  printf("Running netstring test suite...\n");
  run_test_suite();
  printf("All tests passed!\n");
  return 0;
}
