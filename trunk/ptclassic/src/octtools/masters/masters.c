/* $Id$ */
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include <pwd.h>

#define PATHNAMESIZE       2048

optionStruct optionList[] = {
    {"v",       0,              "verbose"},
    {"N",	"newpath",	"new path name"},
    {"O",	"oldpath",	"old path name"},
    { OPT_RARG,	"facet_name(s)",	"facet name(s)" },
    {0, 0, 0}};

/* The following are used in prompt() */
#define strsave(s)      (strcpy(malloc((unsigned) (strlen(s)+1)), s))
#define INPUT_LEN       1024
#define PRINTF  (void) printf

char *prompt(pmt, def)
char *pmt;                      /* User prompt   */
char *def;                      /* Default value */
/*
 * Returns a string read from standard input using the prompt `pmt'.
 * If no value is given,  `def' is returned.
 */
{
    static char pmt_buf[INPUT_LEN];

    if (def && (strlen(def) > 0)) {
        PRINTF("%s [%s]: ", pmt, def); /* fflush(stdout); */
        (void) gets(pmt_buf);
        if (strlen(pmt_buf) > 0) return strsave(pmt_buf);
        else if (def) return strsave(def);
        else return NULL;
    } else {
        PRINTF("%s: ", pmt);  /* fflush(stdout); */
        (void) gets(pmt_buf);
        return strsave(pmt_buf);
    }
}



/* Return TRUE if the master name is a Ptolemy system icon
   and hence should not be moved.  It will normally not be listed.
*/
int excluded(name)
char* name;
{
    return (!strncmp(name,"$PTOLEMY/lib/colors/ptolemy/con",31) ||
	    !strncmp(name,"$PTOLEMY/lib/colors/ptolemy/delay",33));
}

char *expandPathName(name)
char* name;
{
	char* pslash;
	int l;
        struct passwd* pwd;

        static char buf[PATHNAMESIZE];
        char *temp, *value;

        if (*name != '~' && *name != '$') return name;

        /* find first / after the env-var or the username */

        pslash = strchr (name, '/');
        if (pslash == NULL) pslash = name + strlen(name);

        /* copy the username or variable name into buf. */

        l = pslash - name - 1;
        strncpy (buf, name + 1, l);
        buf[l] = 0;

        /* if an environment variable, look up the value. */
        if (*name == '$') {
                temp = getenv (buf);
                if (!temp) return name;
		/* in case the value has a tilde or another environment var */
		value = expandPathName(temp);
        }
        else {
                if (pslash == name + 1) {
                        pwd = getpwuid(getuid());
                        if (pwd == 0) {
				fprintf(stderr,"getpwuid doesn't know you!");
                                exit (1);
                        }
                }
                else {
                        pwd = getpwnam(buf);
                        if (pwd == 0) return name;
                }
                value = pwd->pw_dir;
        }
        /* Put in the home directory or value of variable */
        strcpy (buf, value);

        /* add the rest of the name */
        strcat (buf, pslash);
        return buf;
}



/* 
* ohReplaceInstance() is almost good, except that it does not allow
* us to force a change of instance when the terminals have changed.
* The output of this routine may be incorrect, but at least
* we have updated the instances.
*/
void
replaceInstance(newInst, oldInst, force)
    octObject *newInst, *oldInst;
    int force;
{
    octObject new_term, term, facet, newinstance;
    octGenerator gen;
    octStatus status;

    octGetFacet(oldInst, &facet);
    if ( octCreate(&facet, newInst) != OCT_OK ) {
	printf("\nNew path %s is not valid.\n", newInst->contents.instance.master);
	newinstance = *newInst;
	newinstance.contents.instance.master = prompt("Alternate pathname","");
	replaceInstance(&newinstance, oldInst, force);
	return;
    }

    /*
     *  Copy attachements for objects attached to the instance
     */
    ohAttachContents(oldInst, newInst, OCT_ALL_MASK &~ OCT_TERM_MASK);
    ohAttachContainers(oldInst, newInst, OCT_ALL_MASK &~ OCT_FACET_MASK);
    /*
     *  Copy all objects attached to each terminal
     */
    OH_ASSERT(octInitGenContents(oldInst, OCT_TERM_MASK, &gen));
    while ((status = octGenerate(&gen, &term)) == OCT_OK) {
	/* Find corresponding terminal on the new cell */
	new_term = term;
	status = octGetByName(newInst, &new_term);
	if (status == OCT_OK) {
	    ohAttachContents(&term, &new_term, OCT_ALL_MASK);
	    ohAttachContainers(&term, &new_term, OCT_ALL_MASK&~OCT_INSTANCE_MASK);
	} else if (status == OCT_NOT_FOUND) {
	    if ( force ) {
		fprintf( stderr,  "Formal/Actual Terminal mismatch: %s",
			ohFormatName(&term) );
	    } else {
		errRaise(OH_PKG_NAME, OCT_ERROR, "Formal/Actual Terminal mismatch: %s",
			 ohFormatName(&term));
	    }
	} else {
	    OH_ASSERT(status);
	}
    }
    OH_ASSERT(status);
    OH_ASSERT(octDelete(oldInst));		/* delete the old instance */
    return;
}


main(argc, argv)
int argc;
char **argv;
{
    char *input_name, *newpath, *oldpath;
    octObject outfacet, instance, newinstance;
    octGenerator gen;
    int oldlen;
    int option;
    int verbose = 0;
    int force = 0;
    char* forceFacet = 0;
    int count = 0;		/* Used to prevent infinite looping in replace loop. */
    int subst = 0;		/* Number of substitutions. */
    extern int optind;
    extern char *optarg;
    char* filename[PATHNAMESIZE];
    FILE* pipe;

    /* make sure the PTOLEMY environment variable is set. If not, set it. */
    if(!getenv("PTOLEMY")) putenv("PTOLEMY=~ptolemy");

    /* parse command line options*/
    newpath = oldpath = NIL(char);
    
    while ((option = optGetOption(argc, argv)) != EOF) {
	switch(option) {
	case 'r':
	    force = 1;
	    oldpath = "";
	    newpath = "";
	    break;
	case 'O':
	    oldpath = util_strsav(optarg);
	    break;

	case 'N':
	    newpath = util_strsav(optarg);
	    break;
	case 'v':
	    verbose = 1;
	    break;
	}
    }

    /*
     *  Finish the command line argument parsing 
     */
    octBegin();

    if (optind >= argc) {
	printf("usage: masters [-v] [-N newpath] [-O oldpath] facet_name(s)\n");
	printf("\t-v:\tverbose\n");
	printf("\t-N:\tnew path name (for non-interactive run)\n");
	printf("\t-O:\told path name (for non-interactive run)\n");
    } else {
        while (optind < argc) {
	    octObject facet;

	    subst = 0;

	    input_name = argv[optind++];

	    /*
	     * Perform preliminary check for validity of the name
	     */
	    strcpy(filename,input_name);
	    strcat(filename,"/schematic/contents;");
	    if(access(expandPathName(filename),4)) {
		printf("Skipping %s (invalid facet)\n",input_name);
		continue;
	    } else
		printf("Running masters on %s\n",input_name);
    
            /*
             *  Get the input and output facet names
             */
            ohUnpackDefaults(&facet, "a", ":schematic:contents");
            OH_ASSERT(ohUnpackFacetName(&facet, input_name));
        
            /* 
             *  Open the facet 
             */
        
            {
	        octStatus   status = octOpenFacet(&facet);
	        if ( status != OCT_OLD_FACET && status != OCT_INCONSISTENT ) {
	            errRaise( "masters", 1, "Cannot open %s\n%s\n",
		             ohFormatName( &facet ), octErrorString);
	        }
            }
        
            /*
             * If the old and new paths were not listed on the command line,
             * prompt for them.
             */
            if (oldpath == NIL(char)) {
	        oldpath = prompt("Pathname to replace (? for a listing)","");
	        while (*oldpath == '?') {
	            printf("\nPathnames currently found in the facet (minus connectors and delays):\n");
	            OH_ASSERT(octInitGenContents(&facet, OCT_INSTANCE_MASK, &gen));
	            /* Avoid infinite loops. */
	            count = ohCountContents( &facet, OCT_INSTANCE_MASK );
		    pipe = popen("sort -u","w");
	            while (octGenerate(&gen, &instance) == OCT_OK) {
		        if ( count-- == 0 ) break;
		        /* Print only pathnames not pointing to a connector */
		        if (!excluded(instance.contents.instance.master)) {
			    strcpy(filename,instance.contents.instance.master);
			    strcat(filename,"/schematic/interface;");
			    /* check for read access */
			    if(access(expandPathName(filename),4))
			            fprintf(pipe,"INVALID:");
			    else
			            fprintf(pipe,"\t");
			    fprintf(pipe,"%s\n",instance.contents.instance.master);
		        }
	            }
		    pclose(pipe);
	            oldpath = prompt("\nPathname to replace (? for help)","");
	        }
            }
            if (newpath == NIL(char)) {
	        newpath = prompt("New pathname","");
            }
        
            /* verify that a path was specified */
            if ((strlen(newpath) == 0) || (strlen(oldpath) == 0)) {
		printf("No replacement specified.\n");
		exit(0);
	    }

	    printf("\nReplacing pathnames in %s:\n",input_name);
        
            OH_ASSERT(octInitGenContents(&facet, OCT_INSTANCE_MASK, &gen));
        
            oldlen = strlen(oldpath);
        
            /* Avoid infinite loops. */
            count = ohCountContents( &facet, OCT_INSTANCE_MASK );
            while (octGenerate(&gen, &instance) == OCT_OK) {
	        if ( count-- == 0 ) break;
	        if (strncmp(instance.contents.instance.master, oldpath, oldlen) == 0) {
	            char newname[2048];
        
	            (void) strcpy(newname, newpath);
	            (void) strcat(newname, &instance.contents.instance.master[oldlen]);
	            newinstance = instance;
	            newinstance.contents.instance.master = newname;
	            if ( forceFacet ) {
		        newinstance.contents.instance.facet = forceFacet;
	            }
	            if ( verbose ) {
		        printf( "Replacing \n\tOLD: %s\n\tNEW: %s\n", ohFormatName(&instance), ohFormatName(&newinstance) );
		        fflush( stdout );
	            }
	            replaceInstance(&newinstance, &instance, force);
	            subst++;
	        }
            }
            printf( "masters: %d substitutions.\n", subst );
            octFreeGenerator( &gen );
            OH_ASSERT(octCloseFacet(&facet));
	}
    }
    exit(0);
}
