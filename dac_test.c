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

#define SUCCESS printf("\xE2\x9C\x85 %s\n", __FUNCTION__);

#define EXPECT_EQ(s1, s2, condition, format)                                   \
  do {                                                                         \
    if (!(condition)) {                                                        \
      printf("\xE2\x9D\x8C %s\n", __FUNCTION__);                               \
      printf("\t line [%d]:\n", __LINE__);                                     \
                                                                               \
      printf("\t\t current  -> " DAC_ANSI_COLOR_RED);                          \
      printf((format), s1);                                                    \
      printf("\n" DAC_ANSI_COLOR_RESET);                                       \
      printf("\t\t expected -> " DAC_ANSI_COLOR_GREEN);                        \
      printf((format), s2);                                                    \
      printf("\n" DAC_ANSI_COLOR_RESET);                                       \
                                                                               \
      assert(!(condition));                                                    \
    }                                                                          \
  } while (0)

#define EXPECT_STRING_EQ(e1, e2)                                               \
  do {                                                                         \
    EXPECT_EQ(e1, e2, strcmp(e1, e2) == 0, "'%s'");                            \
  } while (0)

#define EXPECT_UINT_EQ(e1, e2)                                                 \
  do {                                                                         \
    EXPECT_EQ(e1, e2, e1 == e2, "%ld");                                        \
  } while (0)

#define EXPECT_INT_EQ(e1, e2)                                                  \
  do {                                                                         \
    EXPECT_EQ(e1, e2, e1 == e2, "%d");                                         \
  } while (0)

#define EXPECT_BOOL_EQ(e1, e2)                                                 \
  do {                                                                         \
    char *s1 = e1 ? "true" : "false";                                          \
    char *s2 = e2 ? "true" : "false";                                          \
    EXPECT_EQ(s1, s2, e1 == e2, "'%s'");                                       \
  } while (0)

#define EXPECT_NUMBER_CMP(e1, e2, condition, symbol, format)                   \
  do {                                                                         \
    if (!(condition)) {                                                        \
      printf("\xE2\x9D\x8C %s\n", __FUNCTION__);                               \
      printf("\t line [%d]:\n", __LINE__);                                     \
                                                                               \
      printf("\t\t expect ");                                                  \
      printf((format), e1);                                                    \
      printf(" %s ", (symbol));                                                \
      printf((format), e2);                                                    \
      printf(" but got the opposite.\n");                                      \
      assert(!(condition));                                                    \
    }                                                                          \
  } while (0)

#define EXPECT_INT_GT(e1, e2)                                                  \
  do {                                                                         \
    EXPECT_NUMBER_CMP(e1, e2, e1 > e2, ">", "%d");                             \
  } while (0)

#define EXPECT_INT_GTE(e1, e2)                                                 \
  do {                                                                         \
    EXPECT_NUMBER_CMP(e1, e2, e1 >= e2, ">=", "%d");                           \
  } while (0)

#define EXPECT_INT_LT(e1, e2)                                                  \
  do {                                                                         \
    EXPECT_NUMBER_CMP(e1, e2, e1 < e2, "<", "%d");                             \
  } while (0)

#define EXPECT_INT_LTE(e1, e2)                                                 \
  do {                                                                         \
    EXPECT_NUMBER_CMP(e1, e2, e1 <= e2, "<=", "%d");                           \
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
  EXPECT_STRING_EQ(dac_to_cstr(s1), "world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("world"));

  char *random_str = random_string(100);
  dac s2 = dac_new(random_str);
  EXPECT_STRING_EQ(dac_to_cstr(s2), random_str);
  EXPECT_UINT_EQ(dac_len(s2), strlen(random_str));
  free(random_str);

  SUCCESS;
}

void test_dac_eq() {
  EXPECT_BOOL_EQ(dac_eq(dac_new("hello"), dac_new("hello")), true);
  EXPECT_BOOL_EQ(dac_eq(dac_new("hello"), dac_new("hell")), false);
  SUCCESS;
}

void test_dac_contains() {
  dac s = dac_new("Hello world worldd 1234");
  EXPECT_BOOL_EQ(dac_contains(&s, dac_new("ldd 1234")), true);
  EXPECT_BOOL_EQ(dac_contains(&s, dac_new("dd 12345")), false);
  EXPECT_BOOL_EQ(dac_contains(&s, dac_new("Hello world 12345")), false);
  EXPECT_BOOL_EQ(dac_contains(&s, dac_new("worldd")), true);
  SUCCESS;
}

void test_dac_find() {
  dac s = dac_new("Hello world worldd 1234");
  EXPECT_INT_EQ(dac_find(&s, dac_new("ldd 1234")), 15);
  EXPECT_INT_EQ(dac_find(&s, dac_new("dd 12345")), -1);
  EXPECT_INT_EQ(dac_find(&s, dac_new("Hello world 12345")), -1);
  EXPECT_INT_EQ(dac_find(&s, dac_new("worldd")), 12);
  SUCCESS;
}

void test_dac_replace() {
  dac s1 = dac_new("Hello hello hello world");
  dac_replace(&s1, dac_new("hello"), dac_new("jim"));
  EXPECT_STRING_EQ(dac_to_cstr(s1), "Hello jim hello world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("Hello jim hello world"));

  dac s2 = dac_new("Hello hello hello world");
  dac_replace(&s2, dac_new("hello"), dac_new("jimjim"));
  EXPECT_STRING_EQ(dac_to_cstr(s2), "Hello jimjim hello world");
  EXPECT_UINT_EQ(dac_len(s2), strlen("Hello jimjim hello world"));
  SUCCESS;
}

void test_dac_replace_all() {
  dac s1 = dac_new("Hello hello hello world");
  dac_replace_all(&s1, dac_new("hello"), dac_new("jim"));
  EXPECT_STRING_EQ(dac_to_cstr(s1), "Hello jim jim world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("Hello jim jim world"));

  dac s2 = dac_new("Hello hello hello world");
  dac_replace_all(&s2, dac_new("hello"), dac_new("jimjim"));
  EXPECT_STRING_EQ(dac_to_cstr(s2), "Hello jimjim jimjim world");
  EXPECT_UINT_EQ(dac_len(s2), strlen("Hello jimjim jimjim world"));
  SUCCESS;
}

void test_dac_append() {
  dac s = dac_new("hello");
  dac_append(&s, dac_new("world"));
  EXPECT_STRING_EQ(dac_to_cstr(s), "helloworld");
  EXPECT_UINT_EQ(dac_len(s), strlen("helloworld"));
  EXPECT_STRING_EQ(dac_to_cstr(s), "helloworld");
  EXPECT_UINT_EQ(dac_len(s), strlen("helloworld"));
  SUCCESS;
}

void test_dac_append_many() {
  dac s = dac_new("01");
  dac arr[3] = {
      dac_new("234"),
      dac_new("5678"),
      dac_new("9abcdef"),
  };
  dac_append_many(&s, arr, 3);
  EXPECT_STRING_EQ(dac_to_cstr(s), "0123456789abcdef");
  EXPECT_UINT_EQ(dac_len(s), strlen("0123456789abcdef"));
  SUCCESS;
}

void test_dac_join() {
  dac arr[4] = {
      dac_new("1"),
      dac_new("2"),
      dac_new("3"),
      dac_new("4"),
  };
  dac s = dac_join(arr, DAC_ARRAY_LEN(arr), dac_new(",.,"));
  EXPECT_STRING_EQ(dac_to_cstr(s), "1,.,2,.,3,.,4");
  EXPECT_UINT_EQ(dac_len(s), strlen("1,.,2,.,3,.,4"));
  SUCCESS;
}

void test_dac_starts_with() {
  dac s = dac_new("Hello world");

  EXPECT_BOOL_EQ(dac_starts_with(s, dac_new("Hello w")), true);
  EXPECT_BOOL_EQ(dac_starts_with(s, dac_new("Hello world2")), false);
  EXPECT_BOOL_EQ(dac_starts_with(s, dac_new("")), false);
  SUCCESS;
}

void test_dac_ends_with() {
  dac s = dac_new("Hello world");

  EXPECT_BOOL_EQ(dac_ends_with(s, dac_new("o world")), true);
  EXPECT_BOOL_EQ(dac_ends_with(s, dac_new("1Hello world")), false);
  EXPECT_BOOL_EQ(dac_ends_with(s, dac_new("")), false);
  SUCCESS;
}

int main() {
  srand(time(NULL));

  test_dac_new();
  test_dac_eq();
  test_dac_contains();
  test_dac_find();
  test_dac_replace();
  test_dac_replace_all();
  test_dac_append();
  test_dac_append_many();
  test_dac_join();
  test_dac_starts_with();
  test_dac_ends_with();
  return 0;
}
