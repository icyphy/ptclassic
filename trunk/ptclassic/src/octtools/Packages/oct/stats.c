#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "copyright.h"
#include "port.h"
#include "internal.h"
#include "geo.h"

#include "oct_utils.h"
#include "oct_id.h"

extern char *oct_type_names[];

void oct_dump_stats(), print_cpu_stats();

void
octWriteStats(obj, file)
struct octObject *obj;
FILE *file;
{
    struct facet *facet;

    if (obj->type != OCT_FACET) {
	oct_error("octWriteStats: The argument must be an octFacet object");
	errRaise(OCT_PKG_NAME, OCT_ERROR, octErrorString());
    }

    facet = (struct facet *) oct_id_to_ptr(obj->objectId);

    if (facet == NIL(struct facet)) {
	oct_error("octWriteStats: The first argument's objectId is invalid");
	errRaise(OCT_PKG_NAME, OCT_CORRUPTED_OBJECT, octErrorString());
    }

    oct_dump_stats(facet, file);
    return;
}


struct counts {
    int num;
    int size;
    int additional_size;
    int namesize;
    int num_strings;
    int num_chains;
    double avg_content, avg_content_sq;
    double avg_container, avg_container_sq;
};

struct stats {
    int tot_size;
    int objectsize;
    int namesize;
    int num_chains;
    int num_objects;
    int num_strings;
    int num_mallocs;
    int master_tablesize;
    int xid_tablesize;
    struct counts obj_counts[OCT_MAX_TYPE+1];
    int connections[OCT_MAX_TYPE+1][OCT_MAX_TYPE+1];
};


#define RUN_AVG(ent,slot,val)\
     (ent.slot = ((double)(ent.num-1)/(ent.num)*ent.slot+(double)(val)/(ent.num)))

#define square(val) ((val)*(val))
  
#define STD_DEV(ent, avg, avg_sq)\
     (sqrt(ent.avg_sq - square(ent.avg)))

#define STRLEN(string) (string == NIL(char) ? 0 : strlen(string))
  
/* these macros `hide' the malloc overhead corrections */

#define TOT_SIZE(st) (st.tot_size+st.num_mallocs*4)
#define ID_TABLE_SIZE() (1.5*oct_id_table_size())

#define SMALL_SIZE 256
#define LOG_SMALL_SIZE 8

#define GET_BIN(bin, size){\
    register unsigned long n, log;\
    if ((size) <= 0) (bin) = 0;\
    else if ((size) <= SMALL_SIZE) (bin) = ((size) - 1) / 4 + 1;\
    else {\
	for(log = LOG_SMALL_SIZE, n = ((size)-1) >> log; n > 0; log++, n>>=1);\
	(bin) = log*2 + SMALL_SIZE/4 - 2*LOG_SMALL_SIZE \
	    - ((size) <= (3 << (log-2)));\
    }\
}
/* GET_SIZE -- get the block size from the bin number; this is magic */
#define GET_SIZE(bin, size) {\
    register unsigned long tmp;\
    if ((bin) <= SMALL_SIZE/4) (size) = (bin) * 4;\
    else {\
	tmp = (bin) - SMALL_SIZE/4 + 2*LOG_SMALL_SIZE - 3;\
	(size) = ((tmp & 1) == 0 ? 3 : 4) << (tmp >> 1);\
    }\
}

int
malloc_round(size)
int size;
{
    int bin;
    
    GET_BIN(bin, size);   
    GET_SIZE(bin, size);
    return size;
}

void
do_stats(obj, stats)
generic *obj;
struct stats *stats;
{
    int i = obj->flags.type;
    struct chain *content, *container;
    struct object_desc *obj_desc = &oct_object_descs[i];
    int length;

    stats->obj_counts[i].num++;

    if (obj->flags.type == OCT_PATH || obj->flags.type == OCT_POLYGON) {
	struct geo_points *gp = (struct geo_points *) obj;
	
	stats->obj_counts[i].additional_size +=
	  malloc_round(gp->num_points*sizeof(struct octPoint));
	stats->num_mallocs++;
    }
	
    length = 0;
    content  = obj->contents;
    while (content != NIL(struct chain)) {
	stats->connections[obj->flags.type][content->object->flags.type]++;
	content = content->last;
	length++;
    }

    if (obj_desc->flags&HAS_NAME) {
	int internal_offset = obj_desc->user_offset + obj_desc->name_offset;
	char *name = *(char **)((char *)obj + internal_offset);
	int size = ((STRLEN(name)+4)/4)*4;

	stats->obj_counts[i].namesize += size;
	stats->obj_counts[i].num_strings++;
    }
    
    stats->obj_counts[i].num_chains += length;

    RUN_AVG(stats->obj_counts[i],avg_content,length);
    RUN_AVG(stats->obj_counts[i],avg_content_sq,length*length);

    container = obj->containers;

    length = 0;
    while (container != NIL(struct chain)) {
	container = container->next_chain;
	length++;
    }

    RUN_AVG(stats->obj_counts[i],avg_container,length);
    RUN_AVG(stats->obj_counts[i],avg_container_sq,length*length);
}


#define ST_TABLE_SIZE(tbl, malloc_count)\
  (malloc_count += tbl->num_entries + 2,\
   tbl->num_entries*sizeof(st_table_entry) +\
   malloc_round(tbl->num_bins*sizeof(st_table_entry *)+4)-4 +\
   sizeof(st_table))

void
oct_get_stats(facet, stats)
struct facet *facet;
struct stats *stats;
{
    generic *obj;
    int i,j;

    stats->namesize = stats->objectsize = 0;
    stats->num_chains = stats->num_objects = 0;
    stats->num_strings = stats->num_mallocs = 0;
    
    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	for (j = 1; j <= OCT_MAX_TYPE; j++) {
	    stats->connections[i][j] = 0;
	}
    }

    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	stats->obj_counts[i].num = 0;
	stats->obj_counts[i].size = oct_object_descs[i].internal_size;
	stats->obj_counts[i].additional_size = 0;
	stats->obj_counts[i].namesize = 0;
	stats->obj_counts[i].num_strings = 0;
	stats->obj_counts[i].num_chains = 0;
	stats->obj_counts[i].avg_content = 0;
	stats->obj_counts[i].avg_content_sq = 0;
	stats->obj_counts[i].avg_container = 0;
	stats->obj_counts[i].avg_container_sq = 0; 
    }
    
    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	obj = facet->object_lists[i];
	while(obj != NIL(generic)) {
	    do_stats(obj, stats);
	    obj = obj->last;
	}
    }

    do_stats((generic *) facet, stats);
    
    stats->obj_counts[OCT_MASTER].num = facet->masters->num_entries;

    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	stats->num_objects += stats->obj_counts[i].num;
	stats->namesize += stats->obj_counts[i].namesize;
	stats->num_strings += stats->obj_counts[i].num_strings;
	stats->num_chains += stats->obj_counts[i].num_chains;
	stats->objectsize +=
	  stats->obj_counts[i].num*stats->obj_counts[i].size +
	    stats->obj_counts[i].additional_size;
    }

    stats->num_mallocs = stats->num_objects + stats->num_strings +
      stats->num_chains;
    
    stats->master_tablesize =
      ST_TABLE_SIZE(facet->masters, stats->num_mallocs);
    stats->xid_tablesize = ST_TABLE_SIZE(facet->xid_table, stats->num_mallocs);
    stats->tot_size = stats->master_tablesize + stats->xid_tablesize +
      stats->num_chains*sizeof(struct chain)+stats->namesize +
      stats->objectsize;
}

void
oct_dump_stats(facet, dump_file)
struct facet *facet;
FILE *dump_file;
{
    struct stats f_stats;
    int id_table_size, i, j;
    int tot_other_facets = 0, tot_facet;
    st_generator *gen;
    extern st_generator *oct_init_gen_facet_table();
    char *c_facet, *junk;

    id_table_size = ID_TABLE_SIZE();

    gen = oct_init_gen_facet_table();

    while (st_gen(gen,&junk,&c_facet)) {
	struct facet *other_facet = (struct facet *) c_facet;
	struct stats o_stats;

	if (other_facet != facet && other_facet->facet_flags.all_loaded) {
	    oct_get_stats(other_facet, &o_stats);
	    tot_other_facets += TOT_SIZE(o_stats);
	}
    }
    st_free_gen(gen);
	
    oct_get_stats(facet, &f_stats);
    
    (void) fprintf(dump_file, "%8s %6s %6s %8s %8s %8s %8s %8s %8s %8s\n",
	    "type", "size", "number", "totsize", "linksize", "namesize",
	    "average", "contents", "average", "contain");
    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	(void) fprintf(dump_file,
		"%8.8s %6d %6d %8d %8d %8d %8.3f %8.3f %8.3f %8.3f\n",
		oct_type_names[i]+4,
		f_stats.obj_counts[i].size,
		f_stats.obj_counts[i].num,
		(int)(f_stats.obj_counts[i].num*f_stats.obj_counts[i].size +
		    f_stats.obj_counts[i].additional_size),
		f_stats.obj_counts[i].num_chains*sizeof(struct chain),
		f_stats.obj_counts[i].namesize,
		f_stats.obj_counts[i].avg_content,
		STD_DEV(f_stats.obj_counts[i],avg_content, avg_content_sq),
		f_stats.obj_counts[i].avg_container,
		STD_DEV(f_stats.obj_counts[i],avg_container, avg_container_sq)
		);
    }

    tot_facet = f_stats.tot_size + 4*f_stats.num_mallocs;
    
    (void) fprintf(dump_file, "%8.8s %6s %6d %8d %8d %8d %8.3f (%8d)\n\n",
	    "TOTALS", "", f_stats.num_objects, f_stats.objectsize,
	    (int)(f_stats.num_chains*sizeof(struct chain)),
	    f_stats.namesize, f_stats.num_chains/(double) f_stats.num_objects,
	    f_stats.num_chains);
    (void) fprintf(dump_file,
	    "master_table = %d, xid_table = %d, # mallocs = %d, Grand total = %d\n",
	    f_stats.master_tablesize, f_stats.xid_tablesize,
	    f_stats.num_mallocs, TOT_SIZE(f_stats));

    (void) fprintf(dump_file, "id table = %d, other facets = %d, total oct = %d\n",
	    id_table_size, tot_other_facets,
	    tot_facet + tot_other_facets + id_table_size);

    (void) fprintf(dump_file, "\n%12s ", "containers |");
    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	(void) fprintf(dump_file, "%6.6s ", oct_type_names[i]+4);
    }
    (void) fprintf(dump_file, "\n");
    
    for (i = 1; i <= OCT_MAX_TYPE; i++) {
	(void) fprintf(dump_file, "%11s| ", oct_type_names[i]+4);
	for (j = 1; j <= OCT_MAX_TYPE; j++) {
	    (void) fprintf(dump_file, "%6d ", f_stats.connections[i][j]);
	}
	(void) fprintf(dump_file, "\n");
    }
}

