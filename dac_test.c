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
    char *_s1 = e1 ? "true" : "false";                                         \
    char *_s2 = e2 ? "true" : "false";                                         \
    EXPECT_EQ(_s1, _s2, e1 == e2, "'%s'");                                     \
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
  dac *s1 = dac_new("world");
  EXPECT_STRING_EQ(dac_to_cstr(s1), "world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("world"));
  dac_free(s1);

  char *random_str = random_string(100);
  dac *s2 = dac_new(random_str);
  EXPECT_STRING_EQ(dac_to_cstr(s2), random_str);
  EXPECT_UINT_EQ(dac_len(s2), strlen(random_str));
  free(random_str);
  dac_free(s2);

  SUCCESS;
}

void test_dac_init() {
  dac s = {0};
  dac_init(&s, "HeLlo");
  EXPECT_STRING_EQ(dac_to_cstr(&s), "HeLlo");
  EXPECT_UINT_EQ(dac_len(&s), strlen("HeLlo"));
  dac_free(&s);

  SUCCESS;
}

void test_dac_eq() {
  dac *s = dac_new("hello");
  dac *s1 = dac_new("hello");
  dac *s2 = dac_new("hell");
  EXPECT_BOOL_EQ(dac_eq(s, s1), true);
  EXPECT_BOOL_EQ(dac_eq(s, s2), false);
  dac_free(s);
  dac_free(s1);
  dac_free(s2);
  SUCCESS;
}

void test_dac_contains() {
  dac *s = dac_new("Hello world worldd 1234");
  dac *s1 = dac_new("ldd 1234");
  dac *s2 = dac_new("dd 12345kj");
  dac *s3 = dac_new("Hello world 12345");
  dac *s4 = dac_new("worldd");
  EXPECT_BOOL_EQ(dac_contains(s, s1), true);
  EXPECT_BOOL_EQ(dac_contains(s, s2), false);
  EXPECT_BOOL_EQ(dac_contains(s, s3), false);
  EXPECT_BOOL_EQ(dac_contains(s, s4), true);
  dac_free(s);
  dac_free(s1);
  dac_free(s2);
  dac_free(s3);
  dac_free(s4);
  SUCCESS;
}

void test_dac_find() {
  dac *s = dac_new("Hello world worldd 1234");
  dac *s1 = dac_new("ldd 1234");
  dac *s2 = dac_new("dd 12345kj");
  dac *s3 = dac_new("Hello world 12345");
  dac *s4 = dac_new("worldd");
  EXPECT_INT_EQ(dac_find(s, s1), 15);
  EXPECT_INT_EQ(dac_find(s, s2), -1);
  EXPECT_INT_EQ(dac_find(s, s3), -1);
  EXPECT_INT_EQ(dac_find(s, s4), 12);
  dac_free(s);
  dac_free(s1);
  dac_free(s2);
  dac_free(s3);
  dac_free(s4);
  SUCCESS;
}

void test_dac_replace() {
  dac *search = dac_new("hello");
  dac *replace1 = dac_new("jim");
  dac *replace2 = dac_new("jimjim");

  dac *s1 = dac_new("Hello hello hello world");
  dac_replace(s1, search, replace1);
  EXPECT_STRING_EQ(dac_to_cstr(s1), "Hello jim hello world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("Hello jim hello world"));

  dac *s2 = dac_new("Hello hello hello world");
  dac_replace(s2, search, replace2);
  EXPECT_STRING_EQ(dac_to_cstr(s2), "Hello jimjim hello world");
  EXPECT_UINT_EQ(dac_len(s2), strlen("Hello jimjim hello world"));
  dac_free(search);
  dac_free(replace1);
  dac_free(replace2);
  dac_free(s1);
  dac_free(s2);
  SUCCESS;
}

void test_dac_replace_all() {
  dac *search = dac_new("hello");
  dac *replace1 = dac_new("jim");
  dac *replace2 = dac_new("jimjim");

  dac *s1 = dac_new("Hello hello hello world");
  dac_replace_all(s1, search, replace1);
  EXPECT_STRING_EQ(dac_to_cstr(s1), "Hello jim jim world");
  EXPECT_UINT_EQ(dac_len(s1), strlen("Hello jim jim world"));

  dac *s2 = dac_new("Hello hello hello world");
  dac_replace_all(s2, search, replace2);
  EXPECT_STRING_EQ(dac_to_cstr(s2), "Hello jimjim jimjim world");
  EXPECT_UINT_EQ(dac_len(s2), strlen("Hello jimjim jimjim world"));
  dac_free(search);
  dac_free(replace1);
  dac_free(replace2);
  dac_free(s1);
  dac_free(s2);
  SUCCESS;
}

void test_dac_append() {
  dac *s = dac_new("hello");
  dac *s2 = dac_new("world");
  dac_append(s, s2);
  EXPECT_STRING_EQ(dac_to_cstr(s), "helloworld");
  EXPECT_UINT_EQ(dac_len(s), strlen("helloworld"));
  EXPECT_STRING_EQ(dac_to_cstr(s), "helloworld");
  EXPECT_UINT_EQ(dac_len(s), strlen("helloworld"));
  dac_free(s);
  dac_free(s2);
  SUCCESS;
}

void test_dac_append_many() {
  dac *s = dac_new("01");
  dac *arr[3] = {
      dac_new("234"),
      dac_new("5678"),
      dac_new("9abcdef"),
  };
  dac_append_many(s, arr, 3);
  EXPECT_STRING_EQ(dac_to_cstr(s), "0123456789abcdef");
  EXPECT_UINT_EQ(dac_len(s), strlen("0123456789abcdef"));
  dac_free(s);
  dac_free_many(arr, 3);
  SUCCESS;
}

void test_dac_join() {
  dac *arr[4] = {
      dac_new("1"),
      dac_new("2"),
      dac_new("3"),
      dac_new("4"),
  };
  dac *delim = dac_new(",.,");
  dac *s = dac_join(arr, DAC_ARRAY_LEN(arr), delim);
  EXPECT_STRING_EQ(dac_to_cstr(s), "1,.,2,.,3,.,4");
  EXPECT_UINT_EQ(dac_len(s), strlen("1,.,2,.,3,.,4"));
  dac_free(s);
  dac_free(delim);
  dac_free_many(arr, 3);
  SUCCESS;
}

void test_dac_starts_with() {
  dac *s = dac_new("Hello world");
  dac *s1 = dac_new("Hello w");
  dac *s2 = dac_new("Hello world2");
  dac *s3 = dac_new("");

  EXPECT_BOOL_EQ(dac_starts_with(s, s1), true);
  EXPECT_BOOL_EQ(dac_starts_with(s, s2), false);
  EXPECT_BOOL_EQ(dac_starts_with(s, s3), false);
  dac_free(s);
  dac_free(s1);
  dac_free(s2);
  dac_free(s3);
  SUCCESS;
}

void test_dac_ends_with() {
  dac *s = dac_new("Hello world");
  dac *s1 = dac_new("o world");
  dac *s2 = dac_new("1Hello world");
  dac *s3 = dac_new("");

  EXPECT_BOOL_EQ(dac_ends_with(s, s1), true);
  EXPECT_BOOL_EQ(dac_ends_with(s, s2), false);
  EXPECT_BOOL_EQ(dac_ends_with(s, s3), false);
  dac_free(s);
  dac_free(s1);
  dac_free(s2);
  dac_free(s3);
  SUCCESS;
}

int main() {
  srand(time(NULL));

  test_dac_new();
  test_dac_init();
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
