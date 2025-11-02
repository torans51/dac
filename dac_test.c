#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DAC_IMPLEMENTATION
#include "dac.h"

#define DAC_ANSI_COLOR_RED "\x1b[31m"
#define DAC_ANSI_COLOR_GREEN "\x1b[32m"
#define DAC_ANSI_COLOR_YELLOW "\x1b[33m"
#define DAC_ANSI_COLOR_BLUE "\x1b[34m"
#define DAC_ANSI_COLOR_MAGENTA "\x1b[35m"
#define DAC_ANSI_COLOR_CYAN "\x1b[36m"
#define DAC_ANSI_COLOR_RESET "\x1b[0m"

#define SUCCESS() printf("\xE2\x9C\x85 %s\n", __FUNCTION__);
#define EXPECT_STRING_EQ(s1, s2)                                                      \
  do {                                                                         \
    bool condition = strcmp(s1, s2) == 0;                                      \
    if (!condition) {                                                          \
      printf("\xE2\x9D\x8C %s\n", __FUNCTION__);                               \
      printf("\t line [%d]:\n", __LINE__);                                     \
      printf("\t\t current  -> " DAC_ANSI_COLOR_RED                            \
             "'%s'\n" DAC_ANSI_COLOR_RESET,                                    \
             s1);                                                              \
      printf("\t\t expected -> " DAC_ANSI_COLOR_GREEN                          \
             "'%s'\n" DAC_ANSI_COLOR_RESET,                                    \
             s2);                                                              \
      assert(!condition);                                                      \
    }                                                                          \
  } while (0)

#define EXPECT_NUMBER_EQ(n1, n2)                                                      \
  do {                                                                         \
    bool condition = n1 == n2;                                                 \
    if (!condition) {                                                          \
      printf("\xE2\x9D\x8C %s\n", __FUNCTION__);                               \
      printf("\t line [%d]:\n", __LINE__);                                     \
      printf("\t\t current  -> " DAC_ANSI_COLOR_RED                            \
             "%ld\n" DAC_ANSI_COLOR_RESET,                                     \
             n1);                                                              \
      printf("\t\t expected -> " DAC_ANSI_COLOR_GREEN                          \
             "%ld\n" DAC_ANSI_COLOR_RESET,                                     \
             n2);                                                              \
      assert(!condition);                                                      \
    }                                                                          \
  } while (0)

#define EXPECT_BOOL_EQ(b1, b2)                                                        \
  do {                                                                         \
    bool condition = b1 == b2;                                                 \
    if (!condition) {                                                          \
      printf("\xE2\x9D\x8C %s\n", __FUNCTION__);                               \
      printf("\t line [%d]:\n", __LINE__);                                     \
      printf("\t\t current  -> " DAC_ANSI_COLOR_RED                            \
             "%s\n" DAC_ANSI_COLOR_RESET,                                      \
             b1 ? "true" : "false");                                           \
      printf("\t\t expected -> " DAC_ANSI_COLOR_GREEN                          \
             "%s\n" DAC_ANSI_COLOR_RESET,                                      \
             b2 ? "true" : "false");                                           \
      assert(!condition);                                                      \
    }                                                                          \
  } while (0)

size_t random_num(size_t min, size_t max) { return rand() % (max - min) + min; }

char *random_string(size_t length) {
  char *s = malloc(sizeof(char) * length);
  for (size_t i = 0; i < length - 1; i++) {
    s[i] = random_num(32, 63);
  }
  s[length - 1] = 0;
  return s;
}

void test_dac_new() {
  dac s1 = dac_new("world");
  EXPECT_STRING_EQ(s1.ptr, "world");
  EXPECT_NUMBER_EQ(s1.count, strlen("world") + 1);
  EXPECT_STRING_EQ(dac_to_cstr(&s1), "world");
  EXPECT_NUMBER_EQ(dac_len(&s1), strlen("world"));

  char *random_str = random_string(100);
  dac s2 = dac_new(random_str);
  EXPECT_STRING_EQ(dac_to_cstr(&s2), random_str);
  EXPECT_NUMBER_EQ(dac_len(&s2), strlen(random_str));
  free(random_str);

  SUCCESS();
}

void test_dac_contains() {
  dac s = dac_new("Hello world worldd 1234");
  dac search1 = dac_new("ldd 1234");
  dac search2 = dac_new("dd 12345");
  dac search3 = dac_new("Hello world 12345");
  dac search4 = dac_new("worldd");
  EXPECT_BOOL_EQ(dac_contains(&s, &search1), true);
  EXPECT_BOOL_EQ(dac_contains(&s, &search2), false);
  EXPECT_BOOL_EQ(dac_contains(&s, &search3), false);
  EXPECT_BOOL_EQ(dac_contains(&s, &search4), true);
  SUCCESS();
}

void test_dac_append() {
  dac s1 = dac_new("hello");
  dac s2 = dac_new("world");
  dac_append(&s1, s2);
  EXPECT_STRING_EQ(dac_to_cstr(&s1), "helloworld");
  EXPECT_NUMBER_EQ(dac_len(&s1), strlen("helloworld"));
  EXPECT_STRING_EQ(dac_to_cstr(&s1), "helloworld");
  EXPECT_NUMBER_EQ(dac_len(&s1), strlen("helloworld"));
  SUCCESS();
}

void test_dac_append_many() {
  dac s = dac_new("01");
  dac arr[3] = {
      dac_new("234"),
      dac_new("5678"),
      dac_new("9abcdef"),
  };
  dac_append_many(&s, arr, 3);
  EXPECT_STRING_EQ(dac_to_cstr(&s), "0123456789abcdef");
  EXPECT_NUMBER_EQ(dac_len(&s), strlen("0123456789abcdef"));
  SUCCESS();
}

void test_dac_join() {
  dac arr[4] = {
      dac_new("1"),
      dac_new("2"),
      dac_new("3"),
      dac_new("4"),
  };
  dac delim = dac_new(",.,");
  dac s = dac_join(arr, DAC_ARRAY_LEN(arr), &delim);
  EXPECT_STRING_EQ(dac_to_cstr(&s), "1,.,2,.,3,.,4");
  EXPECT_NUMBER_EQ(dac_len(&s), strlen("1,.,2,.,3,.,4"));
  SUCCESS();
}

void test_dac_starts_with() {
  dac s = dac_new("Hello world");
  dac prefix1 = dac_new("Hello w");
  dac prefix2 = dac_new("Hello world2");
  dac prefix3 = dac_new("");

  EXPECT_BOOL_EQ(dac_starts_with(&s, prefix1), true);
  EXPECT_BOOL_EQ(dac_starts_with(&s, prefix2), false);
  EXPECT_BOOL_EQ(dac_starts_with(&s, prefix3), false);
  SUCCESS();
}


void test_dac_ends_with() {
  dac s = dac_new("Hello world");
  dac suffix1 = dac_new("o world");
  dac suffix2 = dac_new("1Hello world");
  dac suffix3 = dac_new("");

  EXPECT_BOOL_EQ(dac_ends_with(&s, suffix1), true);
  EXPECT_BOOL_EQ(dac_ends_with(&s, suffix2), false);
  EXPECT_BOOL_EQ(dac_ends_with(&s, suffix3), false);
  SUCCESS();
}
int main() {
  srand(time(NULL));

  test_dac_new();
  test_dac_contains();
  test_dac_append();
  test_dac_append_many();
  test_dac_join();
  test_dac_starts_with();
  test_dac_ends_with();
  return 0;
}
