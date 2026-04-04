/* Using C89 Standard */
#include <string.h>  /* memcpy */

#include "memory_manager.h"


/* NOTE: init_link is just `memset(&link, 0, sizeof(Link));` */


int add_link(LinkPool *pool, Link *link) {
    if (pool->count == pool->limit)
        return 1;  /* FAIL; TODO: reclaim / realloc */
    memcpy(link, &pool->links[pool->count], sizeof(Link));
    pool->count++;
    return 0;  /* PASS */
}


/* TODO:
if we're splitting an "unallocated" link, how do we keep the links contiguous?
step 0 : | r0 l3 p0 | (refs, length, pointer)
step 1 : | r0 l2 p1 | r1 l1 p0 |
step 2 : | r0 l1 p2 | r1 l1 p0 | r1 l1 p1 |
ok yeah, that's cursed
but do we need links to be contiguous?
yes
right yeah we want need to be reaching the "unallocated" link last
so we update the "unallocated" link to be our data
then create a new "unallocated" link at the end of the chain
got it; so, revised:
step 0 : | r0 l3 p0 |
step 1 : | r1 l1 p0 | r0 l2 p1 |
step 2 : | r1 l1 p0 | r1 l1 p1 | r0 l1 p2 |
*/


#if 0  /* prototype */
void init_mapfile() {
    Link links[0x24000];  /* ~7 MB (~150K Links) */
    char data[64 * 1024 * 1024];  /* 64 MB */

    /* TODO: init links & data to 0 (malloc & memset?) */

    typedef struct Pool_s {
        Link *first_link;
        int   max_links;
        int   links_used;
        void *data_start;
        int   max_data;
        int   data_used;
    } Pool;

    /* NOTE: using source engine limits, but the editor should slightly exceed them:
    8192x brush, 65536x brushside, 8192x entity, 393216 bytes of entdata (48 per ent)
    brushside: 4f (plane) 11f (texvec) 1i (material index) [64 bytes]
    we will need to rethink how brushsides index materials
    need a material table or something, I don't want to use a Link* in Brushside
    tho we'll end up breaking the nice 64 byte alignment with displacements anyway
    */
    int brushsides_size = sizeof(BrushSide) * 65536;
    int strings_size = sizeof(data) - brushsides_size;
    Pool brushes    = { &links[0x00000], 0x02000, 0, NULL, 0, 0 };  /* all links, no data */
    Pool entities   = { &links[0x02000], 0x02000, 0, NULL, 0, 0 };  /* all links, no data */
    Pool brushsides = { &links[0x04000], 0x10000, 0, &data[0], brushsides_size, 0 };
    Pool strings    = { &links[0x14000], 0x10000, 0, &data[brushsides_size], strings_size, 0 };
    /* NOTE: strings link limit might be excessive */
}
#endif


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
