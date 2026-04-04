/* Using C89 Standard */
#pragma once


typedef struct Link_s {
    int            refs;
    int            size;
    void          *data;
    struct Link_s *next;
    struct Link_s *prev;
    struct Link_s *parent;
    struct Link_s *first_child;
} Link;


typedef struct LinkPool_s {
    Link *links;
    int   limit;  /* sizeof(pool) / sizeof(Link) */
    int   count;  /* LinkPool.links[count] = new Link; */
} LinkPool;


typedef struct DataPool_s {
    void *data;
    int   limit;  /* sizeof(data) */
    int   used;   /* sum(link.size) */
} DataPool;

/* NOTE:
we should probably merge Link & Data pools
they will always exist in pairs anyway
function signatures would be a lot simpler if we did this
*/


/* memcopies with bounds checks */
/* NOTE: init_link is just `memset(&link, 0, sizeof(Link));` */
int add_link(LinkPool *pool, Link *link);
/*
if (pool->count == pool->limit)
  return FAIL  // TODO: reclaim / realloc
memcpy(link, &pool->links[pool->count], sizeof(Link))
pool->count++
return PASS
*/
int add_data(DataPool *pool, Link *link, int length, void *data);
/*
if (pool->used + length > pool->limit)
  return FAIL
memcpy(data, (void*)((long)pool->data + pool->used), length)
pool->used += length
*/
void pop_data(DataPool *pool, int length, void *data);
/* NOTE:
if memory isn't at the end of the pool, we can't truly free it
can't reallocate it either, we can't identify free spaces
unless we traverse all links and build a memory map
*/
void pop_link(LinkPool *pool, Link *link);
/*
// remove link from it's relationships
void pop(Link *link) {  // (refs == 0) => KILL
    Link *prev = link->prev;
    Link *next = link->next;
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;
    Link *parent = link->parent;
    if (parent != NULL && parent->first_child == link)
       parent->first_child = next;
}
// NOTE: this mutates relatives, not the link itself
// you can then mutate the link to insert it into new relationships
// we can just append to the tail of a different chain
// feel like I'm re-inventing C++ std::vector here
*/

/* NOTE:
we can split out checking for matches for searching
this means string pointers double as hashes
*/
int add_string(LinkPool *lpool, DataPool *dpool, int length, char* string, Link *out);
/*
Link* match = map->strings

if (match == NULL) {  // empty list
   map->strings = <NEW LINK>
   map->strings->length = length
   map->strings->data = <ALLOC MEMCPY DATA (string)>
   out = map->
   return PASS
}

while (match->next != NULL) {  // strcmp loop
  if length matches
    if string matches
      out = match
      return PASS
  match = match->next
}

// try to alloc new data
if free_tail < length
  return FAIL

link = .append link
  last_string_link->next
memcpy string -> link->data
return PASS
*/

/* worries:
 - memory fragmentation (allocation gets harder)
 - memory diffusion (frequent cache invalidation)
 - editability
   - expand a member in place
   - deletes
   - bulk copies
   - undo/redo (dead refs)
*/
/* NOTE:
we will have to memcpy strings
otherwise we're just copying the whole file into memory
and then adding in nulls to separate them out
basically creating memory fragmentation out of the "compressed" tokens
exceedingly messy
*/

/* TODO: memory management (rough copy of notes from phone)
use unallocated links instead of nulls / empty links
having to check for nulls & init the chain is a waste

visualising memory allocation would be handy for debugging
something like pixel per block & colour by type?

realloc / defrag passes can attempt to merge links
if (this->next->data == (void*)(this->data+this.length))

length might not match data length if we aim for cache alignment
remember! strings of length 32 bytes or less have equivalent perf (todo: confirm)

unallocated links should be determined by refcount (0)
a truly "dead" link would have a NULL pointers, except for data (no relatives)

"keep both" clip tool mode would memcpy each sliced brush & append a side to each copy

links might need to be extended to include GPU refs (link obj to render context)

brushes & entities are basically just links
tho we do need to work out how selection works (3d viewport click -> link pointer)

GPU will also need a memory map of DYNAMIC_DRAW buffers for viewports

the unallocated blocks approach also means we **always** walk the chain to find free memory

we can also have multiple methods for allocating, chosen at runtime (or just profiled independantly)

potential alloc variants:
 - last free link
 - first free link of same size
 - first free link of same size of larger (split, could limit by some scale factor, e.g. 2x)
 - first free link w/ merging (check if neighbours are contiguous and would give us enough size)
 - cache alignment / locality
 - spatial proximity (group objects close together in 3D space)

program states to profile:
 - high create, 0 edit (file load)
 - high usage, high edit (long session)
 - high usage, light edit, big copy
 - copy from another file (session tabs)

exposing a "defrag memory" button for debug would be nice
tho really we should avoid needing to defrag
getting way too paranoid about perf for a progam that doesn't exist yet

would also like to get some numbers of the perf impact of grouping memory spatially
that is, having memory space reflect 3D space where possible
could also look at doing a multipool for "chunks" / octree
section off memory areas so we aren't walking the whole chain for every edit
limiting ourselves to the active edit area is also worth looking at
need a way to mapping perf over time, and a memory access heatmap
...tracking links...
*/

/*
re-using links should be useful for instances
should even be able to edit instances from the top-level file
idk much about instanced rendering, but it should work out
*/

/* NOTE:
this typeless link memory system has all sorts of safety issues
not checking:
 * types
 * circular links
 * self-references
*/
/* NOTE:
entities have 2 child types: brush & key-value (string) pair
key values could be expressed as a pair of strings in sequence
but this is messy, as we aren't marking types clearly
*/

/*
Ent 0
 .refs: 1  # allocated
 .size: 0  # no data
 .data: NULL
 .next: Ent 1
 .prev: NULL
 .parent: NULL
 .first_child: key/value/brush
 !type: entity
*/
/*
could type a type enum:
 - STRING (KEY/VALUE/ASSET_PATH)
 - BRUSH / ENTITY [link w/ no data]
 - BRUSHSIDE (data mapped w/ link)
*/
/*
if we store keyvalues as pairs of links, they must be sequential
not having a strong link between pairs feels bad
but creating & storing a struct containing 2 string pairs also feels wrong
don't really want to add any more complexity to `Link` tho
*/
