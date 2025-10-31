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

/* Private ********************************************************************/
dac dac_new(char *s);
size_t dac_len(dac *s);
char *dac_to_cstr(dac *s);
void dac_append(dac *dest, dac str);
bool dac_contains(dac *s, dac *search);
// TODO:
// void dac_replace(dac *s, dac *r);
// void dac_replace_all(dac *s, dac *r);
void dac_append_str(dac *dest, char *str);
void dac_append_many(dac *dest, dac items[], size_t items_count);
dac dac_join(dac items[], size_t ietms_count, dac *delim);
void dac_free(dac *s);
void dac_free_many(dac *s, size_t count);
bool dac_starts_with(dac *s, dac prefix);
bool dac_ends_with(dac *s, dac suffix);
/******************************************************************************/

/* Private ********************************************************************/
void dac_reserve_capacity(dac *s, size_t expected_capacity);
/******************************************************************************/

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

size_t dac_len(dac *s) { return s->count - 1; }

char *dac_to_cstr(dac *s) { return s->ptr; }

void dac_append(dac *dest, dac item) {
  size_t new_count = dac_len(dest) + dac_len(&item) + 1;
  dac_reserve_capacity(dest, new_count);
  // copy src string at the NULL terminator of dest string
  memcpy(dest->ptr + dac_len(dest), item.ptr, dac_len(&item) + 1);
  dest->count = new_count;
}

bool dac_contains(dac *s, dac *search) {
  for (size_t i = 0; i < s->count; i++) {
    size_t j = 0;
    size_t search_len = dac_len(search);
    if (s->ptr[i] == search->ptr[j]) {
      while (i + j < dac_len(s) && j < search_len &&
             s->ptr[i + j] == search->ptr[j]) {
        j++;
      }
      if (j == search_len) {
        return true;
      }
    }
  }
  return false;
}

void dac_append_str(dac *dest, char *str) {
  size_t new_count = dac_len(dest) + strlen(str) + 1;
  dac_reserve_capacity(dest, new_count);
  // copy src string at the NULL terminator of dest string
  memcpy(dest->ptr + dac_len(dest), str, strlen(str) + 1);
  dest->count = new_count;
}

void dac_append_many(dac *dest, dac *items, size_t items_count) {
  size_t new_count = dac_len(dest);
  for (size_t i = 0; i < items_count; i++) {
    new_count = new_count + items[i].count - 1;
  }
  new_count = new_count + 1; // NULL terminator

  dac_reserve_capacity(dest, new_count);
  for (size_t i = 0; i < items_count; i++) {
    memcpy(dest->ptr + dest->count - 1, items[i].ptr, items[i].count);
    dest->count = dac_len(dest) + dac_len(&items[i]) + 1;
  }
  dest->count = new_count;
}

dac dac_join(dac *items, size_t items_count, dac *delim) {
  dac arr[items_count * 2 - 1];
  size_t arr_len = DAC_ARRAY_LEN(arr);
  for (size_t i = 0; i < arr_len; i++) {
    if (i % 2 == 0) {
      arr[i] = items[i / 2];
    } else {
      arr[i] = *delim;
    }
  }
  dac new_str = dac_new("");
  dac_append_many(&new_str, arr, arr_len);
  return new_str;
}

void dac_free(dac *s) { free(s->ptr); }

void dac_free_many(dac items[], size_t items_count) {
  for (size_t i = 0; i < items_count; i++) {
    free(items[i].ptr);
  }
}

bool dac_starts_with(dac *s, dac prefix) {
  if (dac_len(&prefix) == 0) {
    return dac_len(s) == dac_len(&prefix);
  }

  bool starts_with = true;
  for (size_t i = 0; i < dac_len(&prefix); i++) {
    if (i < dac_len(s) && s->ptr[i] == prefix.ptr[i]) {
      continue;
    }
    return false;
  }
  return starts_with;
}

bool dac_ends_with(dac *s, dac suffix) {
  if (dac_len(&suffix) == 0) {
    return dac_len(s) == dac_len(&suffix);
  }

  bool ends_with = true;
  for (size_t i = 0; i < dac_len(&suffix); i++) {
    if (dac_len(s) - 1 - (dac_len(&suffix) - 1) + i > 0 &&
        dac_len(s) - 1 - (dac_len(&suffix) - 1) + i < dac_len(s) &&
        s->ptr[dac_len(s) - 1 - (dac_len(&suffix) - 1) + i] == suffix.ptr[i]) {
      continue;
    }
    return false;
  }
  return ends_with;
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
