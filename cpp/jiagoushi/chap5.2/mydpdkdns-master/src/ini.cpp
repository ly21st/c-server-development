#include <iostream>
#include <cctype>
#include <cstring>

#include "ini.h"

#define MAX_SECTION 50
#define MAX_NAME 50

/* Strip whitespace chars off end of given string, in place. Return s. */
static char *rstrip(char *s) {
  char *p = s + strlen(s);
  while (p > s && isspace((unsigned char) (*--p)))
    *p = '\0';
  return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char *lskip(const char *s) {
  while (*s && isspace((unsigned char) (*s)))
    s++;
  return (char *) s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
static char *find_char_or_comment(const char *s, char c) {
  int was_whitespace = 0;
  while (*s && *s != c && !(was_whitespace && *s == ';')) {
    was_whitespace = isspace((unsigned char) (*s));
    s++;
  }
  return (char *) s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char *strncpy0(char *dest, const char *src, size_t size) {
  strncpy(dest, src, size);
  dest[size - 1] = '\0';
  return dest;
}

/* See documentation in header file. */
int ini_parse_file(std::ifstream &ifs,
                   int (*handler)(void *, const char *, const char *,
                                  const char *),
                   void *user) {

  char line[INI_MAX_LINE];
  char section[MAX_SECTION] = "";
  char prev_name[MAX_NAME] = "";

  char *start;
  char *end;
  char *name;
  char *value;
  int lineno = 0;
  int error = 0;

  /* Scan through file line by line */
  while (ifs.good()) {
    ifs.getline(line, INI_MAX_LINE);
    lineno++;

    start = line;

#if INI_ALLOW_BOM
    if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                       (unsigned char)start[1] == 0xBB &&
                       (unsigned char)start[2] == 0xBF) {
        start += 3;
    }
#endif
    start = lskip(rstrip(start));

    if (*start == ';' || *start == '#') {
      /* Per Python ConfigParser, allow '#' comments at start of line */
    }
#if INI_ALLOW_MULTILINE
      else if (*prev_name && *start && start > line) {
          /* Non-black line with leading whitespace, treat as continuation
             of previous name's value (as per Python ConfigParser). */
          if (!handler(user, section, prev_name, start) && !error)
              error = lineno;
      }
#endif
    else if (*start == '[') {
      /* A "[section]" line */
      end = find_char_or_comment(start + 1, ']');
      if (*end == ']') {
        *end = '\0';
        strncpy0(section, start + 1, sizeof(section));
        *prev_name = '\0';
      } else if (!error) {
        /* No ']' found on section line */
        error = lineno;
      }
    } else if (*start && *start != ';') {
      /* Not a comment, must be a name[=:]value pair */
      end = find_char_or_comment(start, '=');
      if (*end != '=') {
        end = find_char_or_comment(start, ':');
      }
      if (*end == '=' || *end == ':') {
        *end = '\0';
        name = rstrip(start);
        value = lskip(end + 1);
        end = find_char_or_comment(value, '\0');
        if (*end == ';')
          *end = '\0';
        rstrip(value);

        /* Valid name[=:]value pair found, call handler */
        strncpy0(prev_name, name, sizeof(prev_name));
        if (!handler(user, section, name, value) && !error)
          error = lineno;
      } else if (!error) {
        /* No '=' or ':' found on name[=:]value line */
        error = lineno;
      }
    }

#if INI_STOP_ON_FIRST_ERROR
    if (error)
        break;
#endif
  }

  return error;
}

/* See documentation in header file. */
int ini_parse(const char *filename,
              int (*handler)(void *, const char *, const char *, const char *),
              void *user) {
  int error;

  std::ifstream ifs;
  ifs.open(filename, std::ifstream::in);
  error = ini_parse_file(ifs, handler, user);
  ifs.close();
  return error;
}
