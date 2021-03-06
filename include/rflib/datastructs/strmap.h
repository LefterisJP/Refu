/* Taken and adapted from the CCAN project:
 * http://ccodearchive.net/info/strmap.html
 *
 * An ordered map of strings to values
 * It is based on crit-bit trees: http://cr.yp.to/critbit.html
 * Original license is CC0 (Public Domain)
 */
#ifndef RF_STRMAP_H
#define RF_STRMAP_H

#include <rflib/utils/tcon.h>
#include <rflib/utils/typesafe_cb.h>

#include <stdlib.h>
#include <stdbool.h>

struct RFstring;

/**
 * struct strmap - representation of a string map
 *
 * It's exposed here to allow you to embed it and so we can inline the
 * trivial functions.
 */
struct strmap {
    union {
        struct node *n;
        const struct RFstring *s;
    } u;
    void *v;
};

/**
 * STRMAP_MEMBERS - declare members for a type-specific strmap.
 * @type: type for this map's values, or void * for any pointer.
 *
 * You use this to create your own typed strmap for a particular type.
 * You can use an integer type, *but* remember you can't use "0" as a
 * value!
 *
 * Example:
 *  struct strmap_intp {
 *      STRMAP_MEMBERS(int *);
 *  };
 */
#define STRMAP_MEMBERS(type)    \
    struct strmap raw;          \
    TCON(type canary)

/**
 * strmap_init - initialize a string map (empty)
 * @map: the typed strmap to initialize.
 *
 * For completeness; if you've arranged for it to be NULL already you don't
 * need this.
 *
 * Example:
 *  struct strmap_intp map;
 *
 *  strmap_init(&map);
 */
#define strmap_init(map) strmap_init_(&(map)->raw)
static inline void strmap_init_(struct strmap *map)
{
    map->u.n = NULL;
}

/**
 * strmap_empty - is this string map empty?
 * @map: the typed strmap to check.
 *
 * Example:
 *  if (!strmap_empty(&map))
 *      abort();
 */
#define strmap_empty(map) strmap_empty_(&(map)->raw)
static inline bool strmap_empty_(const struct strmap *map)
{
    return map->u.n == NULL;
}

/**
 * strmap_get - get a value from a string map
 * @map: the typed strmap to search.
 * @member: the string to search for.
 *
 * Returns the value, or NULL if it isn't in the map (and sets errno = ENOENT).
 *
 * Example:
 *  int *val = strmap_get(&map, "hello");
 *  if (val)
 *      printf("hello => %i\n", *val);
 */
#define strmap_get(map, member)                                     \
    tcon_cast((map), canary, strmap_get_(&(map)->raw, (member)))
void *strmap_get_(const struct strmap *map, const struct RFstring *member);

/**
 * strmap_add - place a member in the string map.
 * @map: the typed strmap to add to.
 * @member: the string to place in the map.
 * @v: the (non-NULL) value.
 *
 * This returns false if we run out of memory (errno = ENOMEM), or
 * (more normally) if that string already appears in the map (EEXIST).
 *
 * Note that the pointer is placed in the map, the string is not copied.  If
 * you want a copy in the map, use strdup().  Similarly for the value.
 *
 * Example:
 *  val = malloc(sizeof *val);
 *  *val = 17;
 *  if (!strmap_add(&map, "goodbye", val))
 *      printf("goodbye was already in the map\n");
 */
#define strmap_add(map, member, value)              \
    strmap_add_(                                    \
        &tcon_check((map), canary, (value))->raw,   \
        (member),                                   \
        (void *)(value)                             \
    )
bool strmap_add_(
    struct strmap *map,
    const struct RFstring *member,
    const void *value
);

/**
 * strmap_del - remove a member from the string map.
 * @map: the typed strmap to delete from.
 * @member: the string to remove from the map.
 * @valuep: the value (if non-NULL)
 *
 * This returns the string which was passed to strmap_map(), or NULL if
 * it was not in the map (and sets errno = ENOENT).
 *
 * This means that if you allocated a string (eg. using strdup()), you
 * can free it here.  Similarly, the value is returned in @valuep if
 * @valuep is not NULL.
 *
 * Example:
 *  if (!strmap_del(&map, "goodbye", NULL))
 *      printf("goodbye was not in the map?\n");
 */
#define strmap_del(map, member, valuep)                 \
    strmap_del_(                                        \
        &tcon_check_ptr((map), canary, valuep)->raw,    \
        (member),                                       \
        (void **)valuep                                 \
    )
struct RFstring *strmap_del_(
    struct strmap *map,
    const struct RFstring *member,
    void **valuep
);

/**
 * strmap_clear - remove every member from the map.
 * @map: the typed strmap to clear.
 *
 * The map will be empty after this.
 *
 * Example:
 *  strmap_clear(&map);
 */
#define strmap_clear(map) strmap_clear_(&(map)->raw)
void strmap_clear_(struct strmap *map);

/**
 * strmap_iterate - ordered iteration over a map
 * @map: the typed strmap to iterate through.
 * @handle: the function to call.
 * @arg: the argument for the function (types should match).
 *
 * @handle's prototype should be:
 *  bool @handle(const struct RFstring *member, type value, typeof(arg) arg)
 *
 * If @handle returns false, the iteration will stop.
 * You should not alter the map within the @handle function!
 *
 * Example:
 *  struct strmap_intp {
 *      STRMAP_MEMBERS(int *);
 *  };
 *  static bool dump_some(const struct RFstring *member, int *value, int *num)
 *  {
 *      // Only dump out num nodes.
 *      if (*(num--) == 0)
 *          return false;
 *      printf("%s=>%i\n", member, *value);
 *      return true;
 *  }
 *
 *  static void dump_map(const struct strmap_intp *map)
 *  {
 *      int max = 100;
 *      strmap_iterate(map, dump_some, &max);
 *      if (max < 0)
 *          printf("... (truncated to 100 entries)\n");
 *  }
 */
#define strmap_iterate(map, handle, arg)                        \
    strmap_iterate_(                                            \
        &(map)->raw,                                            \
        typesafe_cb_cast(                                       \
            bool (*)(const struct RFstring *, void *, void *),  \
            bool (*) (                                          \
                const struct RFstring *,                        \
                tcon_type((map), canary),                       \
                __typeof__(arg)), (handle)),                    \
        (arg)                                                   \
    )
typedef bool (*strmap_it_cb)(const struct RFstring *, void *, void *);
void strmap_iterate_(const struct strmap *map, strmap_it_cb cb, const void *data);

/**
 * strmap_prefix - return a submap matching a prefix
 * @map: the map.
 * @prefix: the prefix.
 *
 * This returns a pointer into @map, so don't alter @map while using
 * the return value.  You can use strmap_iterate(), strmap_get() or
 * strmap_empty() on the returned pointer.
 *
 * Example:
 *  static void dump_prefix(const struct strmap_intp *map,
 *              const struct RFstring *prefix)
 *  {
 *      int max = 100;
 *      printf("Nodes with prefix %s:\n", prefix);
 *      strmap_iterate(strmap_prefix(map, prefix), dump_some, &max);
 *      if (max < 0)
 *          printf("... (truncated to 100 entries)\n");
 *  }
 */
#if HAVE_TYPEOF
#define strmap_prefix(map, prefix) \
    ((const __typeof__(map))strmap_prefix_(&(map)->raw, (prefix)))
#else
#define strmap_prefix(map, prefix) \
    ((const void *)strmap_prefix_(&(map)->raw, (prefix)))
#endif
const struct strmap *strmap_prefix_(const struct strmap *map,
                                    const struct RFstring *prefix);

#endif
