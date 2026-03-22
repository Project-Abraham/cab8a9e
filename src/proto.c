#include <stdio.h>


/* losing my mind prototyping memory models instead of profiling */
/* need to create multiple implementations & test them */

/* worries:
 - memory fragmentation (allocation gets harder)
 - memory diffusion (frequent cache invalidation)
 - editability
   - expand a member in place
   - deletes
   - bulk copies
   - undo/redo (dead refs)
*/

/* NOTE: missing some core features of .vmf
 * guids for entities, brushes & brushsides
 * brush_side->displacement_info
these are handy for:
 * error tracing
 * object relationships (targetname still matters tho)
*/

/* NOTE:
no .fgd informed structs for entities yet
we'll get to it when it matters
needs to be a whole system
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


/* memory_manager.h */
/* memory management data-structures */
typedef struct Link_s {
    int            refs;  /* for "garbage collection" */
    int            size;
    void          *data;
    struct Link_s *next;
    struct Link_s *prev;  /* for easier pop */
    /* parent-child relations */
    struct Link_s *parent;
    struct Link_s *first_child;
} Link;

typedef struct LinkPool_s {
    Link *links;
    int   limit;  // sizeof(pool) / sizeof(Link)
    int   count;  // LinkPool.links[count] = new Link;
} LinkPool;

typedef struct DataPool_s {
    void *data;
    int   limit;  // sizeof(data)
    int   used;   // sum(link.size)
} DataPool;


// memcopies with bounds checks
// NOTE: init_link is just `memset(&link, 0, sizeof(Link));`
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
/*
// NOTE: if memory isn't at the end of the pool, we can't truly free it
// can't reallocate it either, we can't identify free spaces
// unless we traverse all links and build a memory map
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


/* map_file.h */
/* level editor file data-structures */
typedef struct Map_s {
    LinkPool  links;
    DataPool  data;
    Link     *brushes;    /* Link<Brush>     */
    Link     *brushsides; /* Link<BrushSide> */
    Link     *entities;   /* Link<Entity>    */
    Link     *keyvalues;  /* Link<KeyValue>  */
    Link     *strings;    /* Link<char[]>    */
} Map;
/* a broad memory group */
/* links can be mixed in one big pool */
/* data will probably be better off in separate pools */
/* we can use a faster alloc() for fixed-width structs */
/*
string alloc() will need to check for contiguous spans
likely combining multiple empty string spans
a difficult and time consuming problem
new allocs should append as long as we can get away with it
*/


typedef struct Brush_s {
    int  first_side;
    int  num_sides;
} Brush;

typedef struct Vec3_s {
    float  x;
    float  y;
    float  z;
} Vec3;

typedef struct Plane_s {
    Vec3   normal;
    float  distance;
} Plane;

typedef struct ProjAxis_s {
    Vec3   axis;
    float  offset;
    float  scale;
} ProjAxis;

typedef struct TexVec_s {
    ProjAxis  s;
    ProjAxis  t;
    float     rotation;
} TexVec;

typedef struct BrushSide_s {
    Plane   plane;
    TexVec  texvec;
    int     material;  /* index into strings */
} BrushSide;

typedef struct Entity_s {
    int  first_keyvalue;
    int  num_keyvalues;
    int  first_brush;
    int  num_brushes;
} Entity;

typedef struct KeyValue_s {
    int  key;    /* index into strings (32 char limit) */
    int  value;  /* index into strings (1024 char limit) */
} KeyValue;


/* TODO: file_io.h */
/* file operations wrappers w/ error codes */
int open(char* path, FILE *file);
int read(FILE *file, int length, char *dest);
int read_until(FILE *file, int length, char terminator, char *dest);
/* error if EOF or length reached; pass if terminator hit */
/* TODO: consume whitespace / newline */
int close(FILE *file);


/* map_file.h */
int load_map(char* path, Map *map);

int add_brush(Map *map, Brush brush, Link *link);
/* assembled in reverse:
int num_sides = 6;
for (i = 0; i < num_sides; i++) {
    int material = add_string(&map, "tools/toolsnodraw");
    Plane plane   = { ... };
    TexVec texvec = { ... };
    BrushSide brushside = { plane, texvec, material };
    (void) add_brushside(&map, brushside);
}
Brush brush = { map.brushes.count, 0 };

add_brush(&map, brush);
*/

/* NOTE:
we can split out checking for matches for searching
this means string pointers double as hashes
*/
int add_string(Map *map, int length, char* string, Link *out);
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
