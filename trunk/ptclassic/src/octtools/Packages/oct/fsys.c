#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
#include "copyright.h"
#include "port.h"
#include "utility.h"

/* should go into port.h */
#if defined(POSIX) || defined(aiws) || defined(SYSV) || defined(linux)
#include <unistd.h>
#else
#include <sys/file.h>
#endif

#include <sys/stat.h>
#include <pwd.h>
#include <sys/param.h>
#include "internal.h"
#include "io_internal.h"
#include "fsys.h"

/* 
 * The fsys interface abstracts the file system from the rest of oct. 
 * Mapping from <cell,view,facet,version> to file names is done here 
 * and here alone.  Management of file permissions and locking is done 
 * here as well.  See the fsys interface description for the 
 * individual functions
 * 
 * For this simple implementation on top of UNIX, the cell and view 
 * are nested directories, with the facets being files.  The name of 
 * the file is "cell/view/facet;version", where cell may be a unix 
 * path name its own right.  No file locking is done and 
 * the commit primitives are ignored.  The cell and view directories 
 * are created at flush time, so creation of bogus directories because 
 * of typo's in the cell and view name are avoided.  Because we dont 
 * create the directories at the beginning, we cant simply uniquely 
 * indentify files by there inode number.  So we indentify them by the 
 * inode number of the  directory containing the cell and then the name 
 * of the cell view facet and version.  This scheme can be fooled by a cell
 * that is actually a symbolic link to a cell at a different location, 
 * so care should be taken.
 * 
 */

extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

static int canonicalize();	/* Forward declaration. */


static char fsys_error_buf[1024];
char *fsys_error_message = fsys_error_buf;

#if !defined(MCC_DMS)
struct desc {
    dev_t device;
    ino_t inode;
    char *file_name;
    char *tail;
};
#else
struct desc {
    char *branch;
    char *config;
    char *file_name;
    char *tail;
};
#endif

/* search backward from ptr in buffer for a PND_CHAR (pathname delimeter) */
    
#define FIND_NEXT_DELIMETER(buffer, ptr)\
    while( *--ptr != PND_CHAR && ptr != buffer) {\
    }\
    if (*ptr != PND_CHAR) {\
	(void) sprintf(fsys_error_buf, "Panic: mangled file_desc in files.c");\
	return 0;\
    }


/*ARGSUSED*/
int
fsys_commit(desc)
char *desc;
{
    return 1;
}

/*ARGSUSED*/
int
fsys_lock(desc)
char *desc;
{
    return 1;
}

/*ARGSUSED*/
int
fsys_unlock(desc)
char *desc;
{
    return 1;
}

#ifdef DEBUG
void
fsys_print_key(file, c_desc)
IOFILE *file;
char *c_desc;
{
    struct desc *desc = (struct desc *) c_desc;
#if !defined(MCC_DMS)    
    (void) IOfprintf(file, "dev = %ld, inode = %d, file = %s, tail = %s",
	    desc->device, desc->inode, desc->file_name, desc->tail);
#else
    if (file == NULL) {
      (void) fprintf (stderr,
                      "file = %s\n\ttail = %s, branch = %s, config = %s",
                      desc->file_name, desc->tail, desc->branch,
                      desc->config);
    } else {
      (void) IOfprintf(file,
                     "file = %s\n\ttail = %s, branch = %s, config = %s",
                     desc->file_name, desc->tail, desc->branch,
                     desc->config);
    }
#endif
    return;
}
#endif

/* 
 * return a IOFILE pointer to the file associated with the descripter c_desc.
 * Create the cell and view directories, if necessary
 */

IOFILE *
fsys_open(c_desc, mode)
char *c_desc;
char *mode;
{
    struct desc *desc = (struct desc *) c_desc;
    char *file_name = desc->file_name;
    IOFILE *stream;
#if !defined(MCC_DMS) && !defined(TDMS)
    char *ptr, *cell_dir, *view_dir;

    ptr = file_name + strlen(file_name);

    FIND_NEXT_DELIMETER(file_name, ptr);
    
    view_dir = ptr;

    FIND_NEXT_DELIMETER(file_name, ptr);
    
    cell_dir = ptr;

    *cell_dir = '\0';
    
    if (access(file_name, F_OK) != 0 && mkdir(file_name, 0777) != 0) {
	(void) sprintf(fsys_error_buf, "Cannot open %s: %s", file_name, sys_msg());
	*cell_dir = PND_CHAR;
	return NULL;
    }
    
    *cell_dir = PND_CHAR;
    *view_dir = '\0';
    
    if (access(file_name, F_OK) != 0 && mkdir(file_name, 0777) != 0) {
	(void) sprintf(fsys_error_buf, "Cannot open %s: %s", file_name, sys_msg());
	*view_dir = PND_CHAR;
	return NULL;
    }
    
    *view_dir = PND_CHAR;
#endif /* !defined(MCC_DMS) && !defined(TDMS) */
    
    stream = IOfopen(file_name, mode);
    
    if (stream == NULL) {
	(void) sprintf(fsys_error_buf, "Cannot open %s: %s", file_name, sys_msg());
    }
    
    return stream;
}


/*
 * close an IOFILE
 */
void
fsys_close(file_handle)
char *file_handle;
{
    IOFILE *stream = (IOFILE *) file_handle;

    (void) IOfclose(stream);
    return;
}

st_table *facet_key_table;

struct fct {
    struct desc *parent_desc;
    char *cell;
    char *view;
    char *facet;
    char *version;
};

#define SAFE_CMP(p1,p2)\
  ((p1) == NIL(char) ? ((p1) == (p2) ? 0 : -1) :\
                       ((p2) == NIL(char) ? 1 : strcmp(p1,p2)))

static int
facet_key_compare(c_a, c_b)
char *c_a;
char *c_b;
{
    struct fct *a = (struct fct *) c_a;
    struct fct *b = (struct fct *) c_b;
    int retval;

    (void) ((retval = (a->parent_desc != b->parent_desc)) ||
              (retval = SAFE_CMP(a->cell, b->cell)) ||
              (retval = SAFE_CMP(a->view, b->view)) ||
              (retval = SAFE_CMP(a->facet, b->facet)) ||
              (retval = SAFE_CMP(a->version, b->version)));

    return retval;
}

#define SAFE_HASH(ptr, max) ((ptr) == NIL(char) ? 0 : st_strhash(ptr,max))
  
static int
facet_key_hash(c_fct, max)
char *c_fct;
int max;
{
    struct fct *fct = (struct fct *) c_fct;
    int retval;

    retval = ((int) fct->parent_desc) +
      SAFE_HASH(fct->cell, max) +
      SAFE_HASH(fct->view, max) +
      SAFE_HASH(fct->facet, max) +
      SAFE_HASH(fct->version, max);
    
    return retval%max;
}

void
oct_init_facet_key_table()
{
    facet_key_table = st_init_table(facet_key_compare, facet_key_hash);
}

    
/* XXX changed from ., .. to .., . */
/* static char *search_path[] = {".", "..", 0}; */
#ifdef TDMS
 /* this is used to find instance cells.  if i have flattened the oct */
 /* directory structure, i don't want to look in the parent directory */
 /* for instances. */
static char *search_path[] = {".", 0};
#else    
static char *search_path[] = {"..", ".", 0};
#endif

#define ABSOLUTE_PATH(name) ((name)[0] == PND_CHAR || (name)[0] == '~')

/* 
 * Figure out the filename associated with user_facet.  If the nameing 
 * is relative (c_parent_desc != nil), then the name is looked up 
 * relative to the directory containing the parent cell or in the 
 * parent cell directory itself.
 *
 * location tells whether to look at the same level (..), one
 * level down (.), or either {FSYS_SIBLING, FSYS_CHILD}
 */

int
fsys_make_key(user_facet, c_file_desc, c_parent_desc, location)
struct octFacet *user_facet;
char **c_file_desc;
char *c_parent_desc;
int location;
{
#if !defined(MCC_DMS)    
    char *last_slash;
    static char desc_buffer[MAXPATHLEN];
    static struct desc desc;
    struct stat stat_buf;
    struct fct fct;
    int len, facet_desc_exists = 0;
    struct desc *parent_desc = (struct desc *) c_parent_desc;
    char *ptr;
    char *resolvedPath;		/* With env vars substituted in (AC) */

    resolvedPath = util_logical_expand( user_facet->cell );
    
    if (ABSOLUTE_PATH(resolvedPath)) {
	if (!canonicalize(desc_buffer, user_facet->cell, "/")) {
	    (void) sprintf(fsys_error_buf, "Can't expand the file name %s",
		    user_facet->cell);
	    return 0;
	}

	last_slash = strrchr(desc_buffer, PND_CHAR);
	
	if (last_slash == NIL(char)) {
	    (void) sprintf(fsys_error_buf,
		    "Panic: %s didn't expand into a absolute pathname.\n",
		    user_facet->cell);
	    return 0;
	}
    } else if (c_parent_desc != (char *) 0) {
	char *parent_tail = strchr(parent_desc->tail+1, PND_CHAR);
	char *cwd = parent_desc->file_name;
	char **path;
#ifdef TDMS
	char rplchar;

	rplchar = *parent_desc->tail;
	parent_tail = parent_desc->tail;
#endif

	/*
	 * hash parent_desc, cell, view, facet, version 
	 *  return: desc for facet
	 */
	fct.parent_desc = parent_desc;
	fct.cell = user_facet->cell;
	fct.view = user_facet->view;
	fct.facet = user_facet->facet;
	fct.version = user_facet->version;

	if (st_lookup(facet_key_table, (char *) &fct, &ptr)) {
	    desc = *((struct desc *) ptr);
	    *c_file_desc = (char *) &desc;
	    return 1;
	}

	facet_desc_exists = 1;

	/*
	 * turn the facet filename (parent_desc->file_name and also cwd)
	 * into the cell directory name
	 */
	*parent_tail = '\0';
	
	for (path = search_path; *path != (char *) 0; path++) {
	    if (canonicalize(desc_buffer, *path, cwd)) {
		len = strlen(desc_buffer);
		desc_buffer[len] = PND_CHAR;
#ifdef _IBMR2
		desc_buffer[len+1] = '\0';
		(void) strcat(desc_buffer, user_facet->cell);
#else
		(void) strcpy(desc_buffer + len + 1, user_facet->cell);
#endif		
		/*
		(void) sprintf(desc_buffer + strlen(desc_buffer), "%c%s",
			       PND_CHAR, user_facet->cell);
		*/
		if (access(desc_buffer, F_OK) == 0) {
		    break;
		}
	    }
	}

	if (*path == (char *) 0) {
	    /*
	     * no path found with cell already existing,
	     * so we search for a writable path
	     */
	    for (path = search_path; *path != (char *) 0; path++) {
		if (canonicalize(desc_buffer, *path, cwd)) {
		    if (access(desc_buffer, F_OK) == 0) {
			break;
		    }
		}
	    }
	    if (*path == (char *) 0) {
#ifndef TDMS		
		*parent_tail = PND_CHAR;
#else
		*parent_tail = rplchar;
#endif
		(void) sprintf(fsys_error_buf, "Can't find cell `%s'",
			oct_facet_name(user_facet));
		return 0;
	    }
	    len = strlen(desc_buffer);
	    desc_buffer[len] = PND_CHAR;
#ifdef _IBMR2
	    desc_buffer[len+1] = '\0';
	    (void) strcat(desc_buffer, user_facet->cell);
#else	    
	    (void) strcpy(desc_buffer + len + 1, user_facet->cell);
#endif	    
	    /*
	    (void) sprintf(desc_buffer + strlen(desc_buffer), "%c%s",
			   PND_CHAR, user_facet->cell);
	    */
	}

#ifndef TDMS	
	*parent_tail = PND_CHAR; /* repair what we did to */
				 /* parent_desc->file_name above */
#else
	*parent_tail = rplchar;
#endif
    } else {
	desc_buffer[0] = '.';
	desc_buffer[1] = PND_CHAR;
#ifdef _IBMR2
	desc_buffer[2] = '\0';
	(void) strcat(desc_buffer, user_facet->cell);
#else	
	(void) strcpy(desc_buffer + 2, user_facet->cell);
#endif	
	/*
	(void) sprintf(desc_buffer, ".%c%s", PND_CHAR, user_facet->cell);
	*/
    }

    last_slash = strrchr(desc_buffer, PND_CHAR);
    *last_slash = '\0';
    /* 
     * if the last_slash is at the beginning of the name, don't
     * do a stat of the parent directory - there is none
     */

    /* XXX should hash like above */
    if (last_slash != desc_buffer) {
	if (stat(desc_buffer, &stat_buf) != 0) {
	    (void) sprintf(fsys_error_buf, "Can't stat directory %s: %s",
			   desc_buffer, sys_msg());
	    return 0;
	}
    }
    *last_slash = PND_CHAR;

    desc.device = stat_buf.st_dev;
    desc.inode = stat_buf.st_ino;

    len = strlen(desc_buffer);
#ifdef TDMS
    desc_buffer[len] = ':';
#else
    desc_buffer[len] = PND_CHAR;
#endif
#ifdef _IBMR2
    desc_buffer[len+1] = '\0';
    (void) strcat(desc_buffer, user_facet->view);
#else    
    (void) strcpy(desc_buffer + len + 1, user_facet->view);
#endif    
    len += strlen(user_facet->view) + 1;
#ifdef TDMS
    desc_buffer[len] = ':';
#else
    desc_buffer[len] = PND_CHAR;
#endif
#ifdef _IBMR2
    desc_buffer[len+1] = '\0';
    (void) strcat(desc_buffer, user_facet->facet);
#else    
    (void) strcpy(desc_buffer + len + 1, user_facet->facet);
#endif    
    len += strlen(user_facet->facet) + 1;
    desc_buffer[len] = ';';
#ifdef _IBMR2
    desc_buffer[len+1] = '\0';
    (void) strcat(desc_buffer,
		  (user_facet->version==NIL(char) ? "" : user_facet->version));
#else    
    (void) strcpy(desc_buffer + len + 1,
		  (user_facet->version==NIL(char) ? "" : user_facet->version));
#endif    
    /*
    (void) sprintf(desc_buffer + strlen(desc_buffer), "%c%s%c%s;%s",
		   PND_CHAR, user_facet->view,
	           PND_CHAR, user_facet->facet,
		   (user_facet->version == NIL(char) ? "" : user_facet->version));
    */
    desc.file_name = desc_buffer;

    desc.tail = last_slash;

    if (facet_desc_exists) {
	struct fct *store;
	struct desc *sdesc;

	store = ALLOC(struct fct, 1);
	*store = fct;

	sdesc = ALLOC(struct desc, 1);
	sdesc->device = desc.device;
	sdesc->inode = desc.inode;
	sdesc->file_name = oct_str_intern(desc.file_name);
	sdesc->tail = oct_str_intern(desc.tail);

	st_insert(facet_key_table, (char *) store, (char *) sdesc);
    }

    *c_file_desc = (char *) &desc;
    return 1;
    
#else /* MCC_DMS is defined */

    char *last_slash;
    static char ret_filename[MAXPATHLEN];
    static char config_buffer[MAXPATHLEN];
    static char branch_buffer[MAXPATHLEN];
    static struct desc desc = {branch_buffer, config_buffer, ret_filename};
    char desc_buffer[MAXPATHLEN];
    dmsWFD *desc_wfd;

    if (ABSOLUTE_PATH(user_facet->cell)) {
	if (!canonicalize(desc_buffer, user_facet->cell, "/")) {
	    (void) sprintf(fsys_error_buf, "Can't expand the file name %s",
		    user_facet->cell);
	    return 0;
	}

	last_slash = strrchr(desc_buffer, PND_CHAR);
	
	if (last_slash == NIL(char)) {
	    (void) sprintf(fsys_error_buf,
		    "Panic: %s didn't expand into a absolute pathname.\n",
		    user_facet->cell);
	    return 0;
	}
	/* For DMS, we have to have the full cell name, so we can lookup */
	/* the descriptor to get the branch and config later */
        (void) sprintf(desc_buffer + strlen(desc_buffer), ":%s:%s%c%s",
                       user_facet->view, user_facet->facet,
                       wfpnTYPE_DELIMITER, wfnOCT_FACET);

    } else if (c_parent_desc != (char *) 0) {
	struct desc *parent_desc = (struct desc *) c_parent_desc;
	char *parent_tail = strchr(parent_desc->tail+1, ':');
	char *cwd = parent_desc->file_name;
	char **path;
	char rplchar;

	if (parent_tail == NULL)
	    parent_tail = parent_desc->tail;
	rplchar = *parent_tail;
	*parent_tail = '\0';  /* turn the facet filename */
			      /* (parent_desc->file_name and also cwd) into */
			      /* the cell directory name */  
	
	for (path = search_path; *path != (char *) 0; path++) {
	    if (canonicalize(desc_buffer, *path, cwd)) {
		/* For DMS, we must have full cell name to get branch and */
		/* config spec later */
                (void) sprintf(desc_buffer + strlen(desc_buffer),
                               "%c%s:%s:%s%c%s", PND_CHAR, user_facet->cell,
                               user_facet->view, user_facet->facet,
                               wfpnTYPE_DELIMITER, wfnOCT_FACET);
                if (wioWFaccess(desc_buffer, F_OK) == 0)
                    break;
	    }
	}

	if (*path == (char *) 0) {
	    /*
	     * no path found with cell already existing,
	     * so we search for a writable path
	     */
	    for (path = search_path; *path != (char *) 0; path++) {
		if (canonicalize(desc_buffer, *path, cwd)) {
                    if (wioWFaccess(desc_buffer, W_OK | R_OK) == 0) {
                        break;
                    }
		}
	    }
	    if (*path == (char *) 0) {
		*parent_tail = rplchar;
		(void) sprintf(fsys_error_buf, "Can't find cell `%s'",
			oct_facet_name(user_facet));
		return 0;
	    }
	    /* For DMS, need the full cell name so we can get branch and */
	    /* config spec later */
            (void) sprintf(desc_buffer + strlen(desc_buffer),
                           "%c%s:%s:%s%c%s", PND_CHAR, user_facet->cell,
                           user_facet->view, user_facet->facet,
                           wfpnTYPE_DELIMITER, wfnOCT_FACET);
	}
	
	/* repair what we did to parent_desc->file_name above */ 
	*parent_tail = rplchar;
    } else {
	/* For DMS, we need the full cell name so we can lookup the branch */
	/* and config spec later */
 	/* This LOOKS like you could combine the two sprintf's into one, but */
 	/* actually they can't.  If wfpnExtractWFN is evaluated BEFORE */
 	/* ptcFullyQualifyWPN (as it is on sun3's), then the static that was */
 	/* previously returned by wfpnExtractWPN has been corrupted (by the */
 	/* call to ptcFullyQualifyWPN).  Thus, the order of evaluation must be */
 	/* explicit. */
         (void) sprintf(desc_buffer, "%s%c",
 		       ptcFullyQualifyWPN(wfpnExtractWPN(user_facet->cell), NIL(char)),
 		       PND_CHAR);
         (void) sprintf(desc_buffer + strlen(desc_buffer), "%s:%s:%s%c%s",
 		       wfpnExtractWFN(user_facet->cell),
                       user_facet->view, user_facet->facet,
                       wfpnTYPE_DELIMITER, wfnOCT_FACET);
    }

    last_slash = strrchr(desc_buffer, PND_CHAR);

    /* For DMS we need the branch and config. We could get them by */
    /* translating, but that is very expensive and introduces re-entrancy to */
    /* this function (DMS uses oct to store meta-data). What we will do */
    /* instead is call a ptc function that just hacks on the pathname string */
    /* and returns the config and branch spec. */
    (void) ptcGetBranchAndConfig(desc_buffer,
				 branch_buffer, config_buffer); 

    (void) strcpy(ret_filename, desc_buffer);

    /* just use the static descriptor, which was filled in with all */
    /* those strcpy calls above */
    desc.tail = strrchr(desc.file_name, PND_CHAR);

    *c_file_desc = (char *) &desc;
    return 1;
#endif /* !defined(MCC_DMS) */
}


/* return the permissions that the user has on the facet file */

octStatus
fsys_resolve(c_file_desc)
char *c_file_desc;
{
    char *tail, *head;
    int mode;
    char desc_buffer[MAXPATHLEN];
    struct desc *file_desc = (struct desc *) c_file_desc;
#if defined(MCC_DMS)
    int access_rights;
#endif
    
    (void) strcpy(desc_buffer, file_desc->file_name);

#if !defined(MCC_DMS)    
    if (access(desc_buffer, F_OK) == 0) {
	mode = FSYS_EXISTS;
	if (access(desc_buffer, R_OK) == 0) {
	    mode |= FSYS_READABLE;
	}
	if (access(desc_buffer, W_OK) == 0) {
	    mode |= FSYS_WRITABLE;
	}
	return mode;
    }

    /*
     * The facet file doesn't exist, make sure it is at least creatable,
     */
    tail = desc_buffer + strlen(desc_buffer);
    
    FIND_NEXT_DELIMETER(desc_buffer, tail);
#ifdef TDMS
    tail++;
#endif
    *tail = '\0';
    if (access(desc_buffer, F_OK) == 0) {
	if (access(desc_buffer, W_OK|X_OK) == 0) {
	    return FSYS_CREATABLE;
	} else {
	    *tail = PND_CHAR;
	    (void) sprintf(fsys_error_buf, "Can't create facet file `%s': %s",
		    desc_buffer, sys_msg());
	    return 0;
	}
    }

#ifndef TDMS
    /*
     * the view directory doesn't exist, make sure it can be created
     */
    
    FIND_NEXT_DELIMETER(desc_buffer, tail);
    *tail = '\0';
    
    if (access(desc_buffer, F_OK) == 0) {
	if (access(desc_buffer, W_OK|X_OK) == 0) {
	    return FSYS_CREATABLE;
	} else {
	    *tail = PND_CHAR;
	    (void) sprintf(fsys_error_buf, "Can't create view directory file `%s': %s",
		    desc_buffer, sys_msg());
	    return 0;
	}
    }

    /*
     * the cell directory doesn't exist, make sure it can be created
     */
    
    tail = strrchr(desc_buffer, PND_CHAR);
    
    if (tail == (char *) 0) {
	head = ".";
    } else if (tail == desc_buffer) {
	head = PND_STRING;
    } else {
	head = desc_buffer;
	*tail = '\0';
    }
    
    if (access(head, F_OK) == 0) {
	if (access(head, W_OK|X_OK) == 0) {
	    return FSYS_CREATABLE;
	} else {
	    *tail = PND_CHAR;
	    (void) sprintf(fsys_error_buf, "Can't create cell directory `%s': %s",
		    desc_buffer, sys_msg());
	    return 0;
	}
    }
    /*
     * Can't even find the directory the cell is in, give up
     */

    (void) sprintf(fsys_error_buf, "Can't find the cell directory `%s': %s",
#ifdef TDMS
		   desc_buffer,
#else
		   head,
#endif
	    sys_msg());
    return 0;
#endif    
    
#else /* defined(MCC_DMS) */
    
    mode = 0;
    access_rights = wioWFaccess(desc_buffer, dmsACCESS | W_OK | R_OK);
    if (access_rights > 0) {
        if (access_rights & dmsEXISTS)
            mode |= FSYS_EXISTS;
        if (access_rights & dmsREAD)
            mode |= FSYS_READABLE;
        if (access_rights & dmsWRITE && !(access_rights & dmsLOCKED_BY_OTHER))
            mode |= FSYS_WRITABLE;
    }

    /* if the read version does *not* exist, but is writable...
     * ...then it is creatable */
    if (!(access_rights & dmsREAD_EXISTS) && mode & FSYS_WRITABLE)
        mode = FSYS_CREATABLE;

    return(mode);
#endif /* defined(MCC_DMS) */
}

/* 
 * fsys_make_key makes a static copy of the key information, save_key 
 * makes a new copy on the heap so it wont get overwritten on the next 
 * call to make_key.  This is an optimization: fsys_make_key has to be called
 * every time we check to see if a cell is in memory, but 
 * fsys_save_key only needs to be called when reading the cell for the 
 * first time.  We save all the string copying and the expensive call 
 * to getwd.
 */
int
fsys_save_key(key, copy)
char *key;
char **copy;
{
    struct desc *desc, *orig = (struct desc *) key;
    char *full_path, *tail;
    char path[1024];
#ifndef SYSV
    extern char *getwd();
#endif
    if (orig->file_name[0] != PND_CHAR) {
	char buffer[MAXPATHLEN];
	int buffer_length;
#if !defined(MCC_DMS)	
#ifdef SYSV
 	if (getcwd(buffer, MAXPATHLEN) == NIL(char)) {
#else
	if (getwd(buffer) == NIL(char)) {
#endif
	    (void) sprintf(fsys_error_buf,"Can't determine current directory");
	    return 0;
	}
#else
	(void) strcpy(buffer, ptcGetWorkingWorkspace());
#endif
	buffer_length = strlen(buffer);
	(void) sprintf(path, "%s%c%s", buffer, PND_CHAR, orig->file_name);
	full_path = oct_str_intern(path);
	tail = full_path + buffer_length + 1 + (orig->tail - orig->file_name);
    } else {
	full_path = oct_str_intern(orig->file_name);
	tail = full_path + (orig->tail - orig->file_name);
    }
    
    desc = ALLOC(struct desc, 1);
#if !defined(MCC_DMS)
    desc->device = orig->device;
    desc->inode = orig->inode;
#else
    /*
     * since the branch and config slots are static buffers, we must intern
     * the strings
     */
    desc->branch = oct_str_intern(orig->branch);
    desc->config = oct_str_intern(orig->config);
#endif
    desc->file_name = full_path;
    desc->tail = tail;
    *copy = (char *) desc;
    return 1;
}


/* inplace cleanup of garbage - assumes what fsys_full_name can return */
static void
cleanup(name)
char *name;
{
    int doit = 1;
    char *ptr, *nptr;

    /* strip out "/./" and "/../" */
    do {
	doit = 0;
	for (ptr = name; *ptr != '\0'; ptr++) {
	    if (*ptr == PND_CHAR && *(ptr+1) == '.' && *(ptr+2) == PND_CHAR) {
		(void) strcpy(ptr, ptr+2);
		doit = 1;
	    }
	    if (*ptr == PND_CHAR && *(ptr+1) == '.' && *(ptr+2) == '.' && *(ptr+3) == PND_CHAR) {
		/* find previous PND_CHAR */
		*ptr = '\0';
		if ((nptr = strrchr(name, PND_CHAR)) == NIL(char)) {
		    *ptr = PND_CHAR;
		    (void) strcpy(name, ptr+4);
		    doit = 1;
		} else {
		    *ptr = PND_CHAR;
		    (void) strcpy(nptr+1, ptr+4);
		    doit = 1;
		}
	    }
	}
    } while (doit);
    return;
}


char *
fsys_full_name(c_key)
char *c_key;
{
    struct desc *key = (struct desc *) c_key;
    char *buffer, *ptr;

    buffer = strsave(key->file_name);

#if !defined(MCC_DMS)    
    /* strip off the last 2 slashes */
    ptr = strrchr(buffer, PND_CHAR);
    *ptr = '\0';
    ptr = strrchr(buffer, PND_CHAR);
    *ptr = '\0';
#else
    /* strip off the last 2 delimiters ":" */
    ptr = strrchr(buffer, ':');
    *ptr = '\0';
    ptr = strrchr(buffer, ':');
    *ptr = '\0';
#endif 
    
    cleanup(buffer);

    return(buffer);
}


int
fsys_free_key(c_key)
char *c_key;
{
    struct desc *key = (struct desc *) c_key;
    oct_str_free(key->file_name);
#if defined(MCC_DMS)
    oct_str_free(key->branch);
    oct_str_free(key->config);
#endif
    FREE(key);
    return 1;
}


int
fsys_compare_key(c_a, c_b)
char *c_a, *c_b;
{
    struct desc *a = (struct desc *) c_a;
    struct desc *b = (struct desc *) c_b;
    
#if !defined(MCC_DMS)
    if (a->device != b->device) {
	return a->device - b->device;
    } else if (a->inode != b->inode) {
	return a->inode - b->inode;
    } else {
	return SAFE_CMP(a->tail, b->tail);
    }
#else
    int i;
    
    if (((i = strcmp(a->file_name, b->file_name)) != 0) ||
	((i = strcmp(a->branch, b->branch)) != 0) ||
	((i = strcmp(a->config, b->config)) != 0))
	return i;
    else
	return 0;
#endif
}
	

int
fsys_hash_key(c_a, modulus)
char *c_a;
int modulus;
{
    struct desc *a = (struct desc *) c_a;
    return st_strhash(a->tail, modulus);
}


/* 
 * canonicalize just does tilde expand and maps the empty path ("") to 
 * the current directory
 */
static int
canonicalize(buffer, path, cwd)
char *buffer;
char *path;
char *cwd;
{
    char *ptr = buffer;

    /* Resolve environment variables */
    path  = util_logical_expand( path );

    switch (path[0]) {
    case '~':
	(void) strcpy(ptr, util_tilde_expand(path));
	if (ptr[0] == '~') {
	    return 0;
	} else {
	    return 1;
	}
    case '\0':			/* Null path. */
	(void) sprintf(buffer, ".%c", PND_CHAR);
	return 1;
    case PND_CHAR:		/* Full path already */
	(void) strcpy(buffer, path); 
	return 1;
    default:
	if (strcmp(path, "..") == 0) {
	    /* strip the last directory name */
	    char *slash;

	    if ((slash = strrchr(cwd, PND_CHAR)) == NIL(char)) {
		return 0;
	    }
	    (void) strncpy(buffer, cwd, slash - cwd);
	    buffer[slash - cwd] = '\0';
	    return 1;
	}
	(void) sprintf(buffer, "%s%c%s", cwd, PND_CHAR, path);
	return 1;
    }
}


#ifdef TEST

#define HELP\
"p - change path, x - change cwd. c - canonicalize name. ?,h - this message\n"
main()
{
    char path[MAXPATHLEN], cell[MAXPATHLEN], cwd[MAXPATHLEN],
         output[MAXPATHLEN];
    char c[2];
    int retval;
    struct octFacet facet;
    
    facet.view = "view";
    facet.facet = "facet";
    facet.version = (char *) 0;
    facet.cell = cell;
    (void) strcpy(path, "");
    (void) strcpy(cwd, ".");

    (void) printf(HELP);
    for(;;) {
	(void) printf("Command: ");
	if (scanf("%1s", c) != 1) exit(0);
	switch (c[0]) {
	case 'x' :
	    (void) printf("New cwd: ");
	    if (scanf("%s", cwd) != 1) {
		exit(-1);
	    }
	    break;
	case 'p' :
	    (void) printf("New path (in quotes): ");
	    if (scanf("%*[ \t\n]\"%[^\"]\"", path) != 1) {
		(void) printf("Bad format for path\n");
		scanf("%*[^\n]");
	    }
	    break;
	case 'c' :
	    (void) printf("Canonicalize: ");
	    if (scanf("%s", cell) != 1) {
		exit(-1);
	    }
	    retval = canonicalize(output, path, cwd);
	    (void) printf("Canonicalize(%s, %s) returns %s (%d)\n",
			  path, cwd, output, retval);
	    break;
	case '?' :
	case 'h' :
	    (void) printf(HELP);
	    break;
	}
    }
}
#endif
 
