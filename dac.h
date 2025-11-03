/******************************************************************************/
/* DAC => Dynamic Array of chars                                              */
/*                                                                            */
/* A simple header only library to handle strings in C.                       */
/*                                                                            */
/* DO NOT USE IN PRODUCTION                                                   */
/*                                                                            */
/* Inspired by:                                                               */
/* - https://github.com/tsoding/nob.h                                         */
/* - https://github.com/nothings/stb                                          */
/******************************************************************************/

#ifndef DAC_H__
#define DAC_H__

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *ptr;    // pointer to the first char of the c string. NULL terminator is
                // included
  size_t count; // len of the c string plus NULL terminator
  size_t capacity; // memory allocated >= len
} dac;

/* Public *********************************************************************/
dac dac_new(char *s);
size_t dac_len(dac s);
char *dac_to_cstr(dac s);
bool dac_eq(dac s1, dac s2);
void dac_append(dac *dest, dac str);
bool dac_contains(dac *s, dac search);
int dac_find(dac *s, dac search);
void dac_replace(dac *s, dac search, dac str);
void dac_replace_all(dac *s, dac search, dac str);
void dac_append_many(dac *dest, dac items[], size_t items_count);
dac dac_join(dac items[], size_t ietms_count, dac delim);
bool dac_starts_with(dac s, dac prefix);
bool dac_ends_with(dac s, dac suffix);
void dac_free(dac *s);
void dac_free_many(dac items[], size_t count);
/******************************************************************************/

/* Private ********************************************************************/
void dac_reserve_capacity(dac *s, size_t expected_capacity);
/******************************************************************************/

// #define DAC_IMPLEMENTATION 1
#ifdef DAC_IMPLEMENTATION

#define DAC_DEFAULT_CAP 256
#define DAC_ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

dac dac_new(char *s) {
  dac str = {0};
  size_t len = strlen(s) + 1;
  dac_reserve_capacity(&str, len);
  memcpy(str.ptr, s, len);
  str.count = len;
  return str;
}

size_t dac_len(dac s) { return s.count - 1; }

char *dac_to_cstr(dac s) { return s.ptr; }

bool dac_eq(dac s1, dac s2) {
  if (dac_len(s1) != dac_len(s2))
    return false;

  for (size_t i = 0; i < dac_len(s1); i++) {
    if (s1.ptr[i] != s2.ptr[i])
      return false;
  }
  return true;
}

void dac_append(dac *dest, dac item) {
  size_t new_count = dac_len(*dest) + dac_len(item) + 1;
  dac_reserve_capacity(dest, new_count);
  // copy src string at the NULL terminator of dest string
  memcpy(dest->ptr + dac_len(*dest), item.ptr, dac_len(item) + 1);
  dest->count = new_count;
}

bool dac_contains(dac *s, dac search) {
  char *s_str = dac_to_cstr(*s);
  size_t s_len = dac_len(*s);
  char *search_str = dac_to_cstr(search);
  size_t search_len = dac_len(search);

  for (size_t i = 0; i < s_len; i++) {
    size_t j = 0;
    if (s->ptr[i] == search.ptr[j]) {
      while (i + j < s_len && j < search_len && s_str[i + j] == search_str[j]) {
        j++;
      }
      if (j == search_len) {
        return true;
      }
    }
  }
  return false;
}

int dac_find(dac *s, dac search) {
  char *s_str = dac_to_cstr(*s);
  size_t s_len = dac_len(*s);
  char *search_str = dac_to_cstr(search);
  size_t search_len = dac_len(search);

  for (size_t i = 0; i < s_len; i++) {
    size_t j = 0;
    if (s->ptr[i] == search.ptr[j]) {
      while (i + j < s_len && j < search_len && s_str[i + j] == search_str[j]) {
        j++;
      }
      if (j == search_len) {
        return i;
      }
    }
  }
  return -1;
}

void dac_replace(dac *s, dac search, dac item) {
  int start = dac_find(s, search);
  if (start >= 0) {
    int delta = dac_len(item) - dac_len(search);
    if (delta > 0) {
      dac_reserve_capacity(s, dac_len(*s) + (dac_len(item) - dac_len(search)) +
                                  1);
      memmove(s->ptr + start + dac_len(item),
              s->ptr + start + dac_len(search), dac_len(*s) + 1 - delta);
      memcpy(s->ptr + start, item.ptr, dac_len(item));
    } else if (delta < 0) {
      memcpy(s->ptr + start, item.ptr, dac_len(item));
      memmove(s->ptr + start + dac_len(item),
              s->ptr + start + dac_len(search), dac_len(*s) + 1 + delta);
    } else {
      memcpy(s->ptr + start, item.ptr, dac_len(item));
    }
    s->count = s->count + delta;
  }
}

void dac_replace_all(dac *s, dac search, dac item) {
  int start = dac_find(s, search);
  while (start >= 0) {
    dac_replace(s, search, item);
    start = dac_find(s, search);
  }
}

void dac_append_many(dac *dest, dac *items, size_t items_count) {
  size_t new_count = dac_len(*dest);
  for (size_t i = 0; i < items_count; i++) {
    new_count = new_count + items[i].count - 1;
  }
  new_count = new_count + 1; // NULL terminator

  dac_reserve_capacity(dest, new_count);
  for (size_t i = 0; i < items_count; i++) {
    memcpy(dest->ptr + dest->count - 1, items[i].ptr, items[i].count);
    dest->count = dac_len(*dest) + dac_len(items[i]) + 1;
  }
  dest->count = new_count;
}

dac dac_join(dac *items, size_t items_count, dac delim) {
  dac arr[items_count * 2 - 1];
  size_t arr_len = DAC_ARRAY_LEN(arr);
  for (size_t i = 0; i < arr_len; i++) {
    if (i % 2 == 0) {
      arr[i] = items[i / 2];
    } else {
      arr[i] = delim;
    }
  }
  dac new_str = dac_new("");
  dac_append_many(&new_str, arr, arr_len);
  return new_str;
}

bool dac_starts_with(dac s, dac prefix) {
  size_t s_len = dac_len(s);
  size_t prefix_len = dac_len(prefix);
  if (prefix_len == 0) {
    return s_len == prefix_len;
  }

  bool starts_with = true;
  for (size_t i = 0; i < prefix_len; i++) {
    if (i < s_len && s.ptr[i] == prefix.ptr[i]) {
      continue;
    }
    return false;
  }
  return starts_with;
}

bool dac_ends_with(dac s, dac suffix) {
  size_t s_len  = dac_len(s);
  size_t suffix_len = dac_len(suffix);
  if (suffix_len == 0) {
    return s_len == suffix_len;
  }

  bool ends_with = true;
  for (size_t i = 0; i < suffix_len; i++) {
    if (s_len - 1 - (suffix_len - 1) + i > 0 &&
        s_len - 1 - (suffix_len - 1) + i < s_len &&
        s.ptr[s_len - 1 - (suffix_len - 1) + i] == suffix.ptr[i]) {
      continue;
    }
    return false;
  }
  return ends_with;
}

void dac_free(dac *s) { free(s->ptr); }

void dac_free_many(dac items[], size_t items_count) {
  for (size_t i = 0; i < items_count; i++) {
    free(items[i].ptr);
  }
}

void dac_reserve_capacity(dac *s, size_t expected_capacity) {
  if (expected_capacity > s->capacity) {
    if (s->capacity == 0)
      s->capacity = DAC_DEFAULT_CAP;

    while (expected_capacity > s->capacity) {
      s->capacity = s->capacity * 2;
    }

    s->ptr = realloc(s->ptr, sizeof(char) * s->capacity);
    assert(s->ptr != NULL && "Not enough memory");
  }
}
#endif // DAC_IMPLEMENTATION

#endif // DAC_H__
