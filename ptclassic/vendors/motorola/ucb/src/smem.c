/* $Id$ 
* MODULE NAME: SMEM.C                                                  
*                                                                      
* MODULE DESCRIPTION                                                   
*   This module provides all the simulator functions that deal with    
* memory locations in the external memory map of a dsp device.  It     
* provides simulation of the external memory space for up to 10 dsp    
* devices, and can easily be modified for more devices.  These are     
* the functions currently used with the sim56000 REV1.12 simulator and 
* with the sim3d.c and simnwin.c example programs included with the    
* simulation package.  The module is provided so that the user may     
* customize any special external memory responses - for example the    
* implementation of dual port memory in multiple dsp simulations.      
*                                                                      
*   This implementation of the external memory functions uses          
* a virtual memory structure which represents the DSP memory.          
* The virtual memory structure operates on 256 word blocks of          
* memory.  Each word is contained within a "long integer" data         
* type.  It is assumed that the long integer type will have at         
* least 24 bits.  The least significant 3 bytes will contain           
* a 24 bit DSP word.  The module functions dynamically allocate        
* memory for a block if it is written to.  Memory areas that are       
* unwritten are not allocated memory.  If the memory allocation        
* call fails (no room for the next block), an existing memory          
* block will be stored out to disk file DSP_XMn.MEM.  The virtual      
* memory structure contains information to indicate which blocks       
* are in memory or on disk, and their locations.                       
* Blocks filled with a constant value will not require memory.         
* The most recently referenced block of memory is switched out         
* only as a last resort.                                               
*                                                                      
* MAIN FUNCTIONS: Called from other modules                            
* dsplxmrd(dev,mem,address,buf,fetch): Read device mem address to buf.    
* dsplxmwr(dev,mem,address,value,store): Write device mem address with value.
* dsplxmsave(dev,fp): Save devices external memory to a state file.    
* dsplxmload(dev,fp): Load devices external memory from a state file.  
* dsplxmnew (dev):    Create external memory for a new device.         
* dsplxmfree(dev):    Free external memory structure of a device.      
* dsplxminit(dev):    Initialize a device's external memory.           
* dsp_alloc(nbytes)   Memory allocation call used by simulator.        
*                                                                      
* LOCAL FUNCTIONS:  Lower level functions only used by this module     
* dsplmrd(mem,block,offset,buf): Read virtual memory address to  buf.  
* dsplmwr(mem,block,offset,val): Write virtual memory address with val.
* dsplxmfrb():    Free a virtual memory block after saving it to disk. 
* dsplmark(mem,block):  Keep list of recently used memory blocks.      
* dsplmbsave(mem,fp):   Save a 64k memory map of a dsp to a state file.        
* dsplmbload(mem,fp):   Load a 64k memory map of a dsp from a state file.      
* m_pdisk(mem,block):   Put memory block in virtual memory disk file.  
* m_gdisk(mem,block):   Get memory block from virtual memory disk file.
* m_swapb(): Pick a mem block, put it on disk, return its mem pointer. 
* m_lastb(mem): Last resort virtual memmory block gets put to disk.    
* m_pickb(mem): Pick a virtual memory block to put on disk.            
*/

#include "config.h"
#include "brackets.h"
#include "portab.h"
#include "dspexec.h"
#include "dspsdef.h"
#include "dspsdcl.h"
struct memtype { /* used for virtual memory scheme */
    int recent; /* most recent block referenced */
    int recent2; /* next most recent referenced */
    int highest; /* highest block in memory */
    int lowest;  /* lowest block in memory */
    int disk_tbl[256]; /* Flag that a memory block is on disk. */
    long blok_fil[256]; /* block fill values for constant value mem blocks */
    long *mem_data[256]; /* pointers to allocated blocks of memory */
  };

struct dsp_vmem { /* virtual memory structure */
   FILE *memfn; /* File pointer used when blocks are swapped to disk. */
   int disk_use; /* Flag that the disk is being used for this device. */
   int devindex; /* Device index associated with this memory structure. */
   int dbitmap[100]; /* Disk table bit map showing used disk blocks. */
   struct memtype x_virtual; /* External X, Y and P virtual memory structures.*/
   struct memtype y_virtual;
   struct memtype p_virtual;
   };

static struct dsp_vmem *dsp_vmp[10]; /* pointers to 10 virtual memory structures */
static char *smpxy[10]= /* Virtual memory temporary filenames for each device. */
   {"DSP_XM0","DSP_XM1","DSP_XM2","DSP_XM3",
    "DSP_XM4","DSP_XM5","DSP_XM6","DSP_XM7","DSP_XM8","DSP_XM9"}; 
static struct dsp_vmem *dsp_vmpl;   /* local pointer to virtual memory */
static int blocksz=(256*sizeof(long)); /* size of a block of 256 long integers */
static long *m_gdisk(),*m_swapb(),*m_lastb(),*m_pickb();
static dsplmfrb(),dsplmrd(),dsplmark(),dsplmbsave(),
   dsplmbload(),dsplmwr(),dsplxmfrb(),m_pdisk();
static
long *
m_gdisk(mem,block)
struct memtype *mem;
int block;
{
   /* Get a virtual memory block from disk and put it in memory. */
   int dblock;
   int pos;
   long offset;
   int status;
   int i,j;
   long *bptr;
   static unsigned int bitclr[16]={0xfffe,0xfffd,0xfffb,0xfff7,0xffef,0xffdf,
       0xffbf,0xff7f,0xfeff,0xfdff,0xfbff,0xf7ff,0xefff,0xdfff,0xbfff,0x7fff};

   /* get a block of memory to store the data */
   bptr=(long *) dsp_alloc((unsigned int)blocksz);
   /* read block from disk to memory */
   dblock=mem->disk_tbl[block]-1;
   /* seek to dblock position */
   offset=(long)dblock * (long)blocksz;
   pos=fseek(dsp_vmpl->memfn,offset,0);
   if (pos!=0)
     {
#if FULLSIM
      dsplputs(20,0,"Error seeking to position in disk file DSP_XMn.MEM in m_gdisk",1);
#else
	  (void) printf("\nError seeking to position in disk file DSP_XMn.MEM in m_gdisk");
#endif
	  (void) fclose(dsp_vmpl->memfn);
      exit (1);
     }

   /* read data from disk to buffer */
   status=fread((char *)bptr,blocksz,1,dsp_vmpl->memfn);
   if (status!=1)
     {
#if FULLSIM
      dsplputs(20,0,"Error 1 reading block from disk in m_gdisk.",1);
#else
	 (void) printf("\nError 1 reading block from disk in m_gdisk. Status=%d",status);
#endif
      (void) fclose(dsp_vmpl->memfn);
      exit (1);
     }
   /* mark the disk block as free */
   i=dblock>>4; /* word position in dtab bitmap */
   j=dblock&0xf; /* bit position in dtab bitmap */
   dsp_vmpl->dbitmap[i]&=bitclr[j];

   /* mark the mem structure to indicate the data isn't on disk */
   mem->disk_tbl[block]=0; /* mark block NOT ON DISK */
   mem->mem_data[block]=bptr;
   return (bptr);
}
static
long *
m_pickb(mem)
struct memtype *mem;
{
  int i,j,k,m;
  long *bptr;

  /* Pick a block and write it out to disk, but don't pick the most */
  /* recently referenced blocks. */
  j=mem->recent; /* two most recently referenced blocks */
  m=mem->recent2;
  for(i=mem->lowest,k=mem->highest,bptr=NULL;i<=k && ((i==j)||(i==m)|| !(bptr= mem->mem_data[i])) ;i++);
  if (bptr){
     m_pdisk(mem,i);
     bptr= mem->mem_data[i];
     mem->mem_data[i]=NULL;
     mem->lowest=(i>j)?(j>m)?m:j:(i>m)?m:i+1; /* set to lowest of i,j,m */
     }
  else{
     mem->lowest=(j<m)?j:m; /* It's the only one left. */
     mem->highest=(j>m)?j:m;
     }
  return (bptr);
}

static
long *
m_lastb(mem)
struct memtype *mem;
{
  /* switches out the most recently referenced block of memory */
  /* This is used as a last resort when no other memory blocks */
  /* are available, so mem->highest is set to -1 */

  int i;
  long *bptr;

  i=mem->recent;
  if (bptr=mem->mem_data[i]);
  else {
     i=mem->recent2;
     bptr= mem->mem_data[i];
     }
  mem->recent=mem->recent2;
  if(bptr){
     m_pdisk(mem,i);
     mem->mem_data[i]=NULL;
     }
  mem->highest=mem->lowest=mem->recent2; /* no more blocks in this memory */
  return (bptr);
}



static
long *
m_swapb()
{
  /* Picks a memory block, shoves it to disk and returns a pointer to its */
  /* original memory area. Returns NULL if no blocks found. */
  /* This version tries all possible devices */
  int i;
  long *bptr;
  struct dsp_vmem *dsp_vmtemp;
  dsp_vmtemp=dsp_vmpl;
  for (i=0,bptr=NULL;i<dsp_const.num_dsps;i++){
     if ((dsp_vmpl=dsp_vmp[i])&&
         (((bptr=m_pickb(&dsp_vmpl->p_virtual)))||
          ((bptr=m_pickb(&dsp_vmpl->x_virtual)))||
          ((bptr=m_pickb(&dsp_vmpl->y_virtual))))) break;
      }
  if (!bptr){
     for (i=0;i<8;i++){
       if ((dsp_vmpl=dsp_vmp[i])&&
           (((bptr=m_lastb(&dsp_vmpl->x_virtual)))||
            ((bptr=m_lastb(&dsp_vmpl->y_virtual)))||
            ((bptr=m_lastb(&dsp_vmpl->p_virtual))))) break;
       }
    }
  dsp_vmpl=dsp_vmtemp;
  return (bptr);
}



/*
* dsp_alloc --- allocate memory
*
* All the simulator routines use this function to allocate needed memory.
* This implementation is integrated with the virtual memory scheme and
* will call dsplxmfrb() to shove an external memory block to disk if the
* requested memory is not immediately available from malloc().  This
* function must exit if it can't allocate the needed memory because the
* simulator functions always assume that the memory is allocated.  The
* simulator does not require clearing of the allocated memory.
*/

void *
dsp_alloc(nbytes)
unsigned int nbytes;
{
   void *pointer;

   while (!(pointer =(void *) malloc(nbytes))) {
      	if (!dsplxmfrb()){
#if FULLSIM
      		dsplputs(20,0,"Insufficient memory: dsp_alloc",1);
#else
			(void) printf ("\nInsufficient memory: dsp_alloc");
#endif
            exit (1);
            }
      	}
   return(pointer);
}

/*
* dsplxmfree --- free a devices external memory
*
* Free the external virtual memory structure for the specified device. 
* This should be called after dsplxminit, which will free substructures. 
* The simulator doesn't use this, but it is provided as a counterpart to 
* dsplxmnew() function. 
*/

dsplxmfree(devindex)
int devindex;
{
   free ((char *)dsp_vmp[devindex]);
   dsp_vmp[devindex]=NULL;
}

/*
* dsplxmsave --- save device's external memory to a simulator state file
*
* This function must save the external memory associated with a device
* to a simulator state file.  The file is already opened with a "w+"
* designator.  The entire simulator state except for the external memory
* state has been saved prior to calling this function (it is called from
* within the dsp_save() simulator function as the last step in saving the
* device state to a file.  There is no fixed format in which you must
* save the data,  but the dsplxmload() function must provide the exact
* steps to reload the saved memory state from the same file.  This function
* must return 1 if the save is successful, 0 otherwise.
*/

dsplxmsave(devindex,fp)
FILE *fp; /* Pointer to file already opened with "w+" designator. */
int devindex;
{
   /* Save a device's external memory to a state file.   */
   int writok;
   dsp_vmpl= dsp_vmp[devindex];
   writok= (dsplmbsave(&dsp_vmpl->x_virtual,fp) && 
            dsplmbsave(&dsp_vmpl->y_virtual,fp) &&
            dsplmbsave(&dsp_vmpl->p_virtual,fp));
   return (writok);
}

/*
* dsplxminit --- initialize a device's external memory
*
* This function must reset a device's external memory to an initialized state.
* It is called from the dsp_init() simulator function as part of the process
* of re-initializing a device's state.  This is used in response to the
* simulator RESET S command and when loading a new simulator state with the
* LOAD S command or dsp_load() function call.
*/

dsplxminit(devindex)
int devindex;
{  /* reinitialize external virtual memory for a device. */
   int i;
   if (dsp_vmpl= dsp_vmp[devindex]){
      dsplmfrb(&dsp_vmpl->x_virtual);
      dsplmfrb(&dsp_vmpl->y_virtual);
      dsplmfrb(&dsp_vmpl->p_virtual);
      if (dsp_vmpl->disk_use) (void) fclose (dsp_vmpl->memfn); /* close old virtual memory file */
      dsp_vmpl->memfn=NULL;
      dsp_vmpl->disk_use=0;
      for (i=0;i<100;i++) dsp_vmpl->dbitmap[i]=0;
      }
}

/*
* dsplxmnew --- Create and initialize a device's external memory structure
* 
* This function will be called from the dsp_new() simulator function as
* a part of the procedure of creating a new dsp device.  It must perform
* whatever steps necessary to allocate and initialize the external memory
* associated with the selected device.
*/

dsplxmnew(devindex)
int devindex;
{  int i;
   /* Create a virtual memory structure to represent the external memory */
   dsp_vmp[devindex]=dsp_vmpl=(struct dsp_vmem*)dsp_alloc(sizeof(struct dsp_vmem));
   for (i=0;i<256;i++){
      dsp_vmpl->x_virtual.mem_data[i]=NULL;
      dsp_vmpl->y_virtual.mem_data[i]=NULL;
      dsp_vmpl->p_virtual.mem_data[i]=NULL;
      }
   dsp_vmpl->disk_use=0;
   dsp_vmp[devindex]->devindex=devindex;
   dsplxminit(devindex); /* initialize other members of structure */
}

/*
* dsplxmload --- load device's external memory from a simulator state file
*
* This function is called from dsp_load() as the last step in loading a
* device's state.  The external memory will have already been initialized by
* a call to dsplxminit() prior to calling this function, and the remainder
* of the device state will have been loaded.  The state file will already
* be opened with the "r" designator and the file pointer advanced to the
* position where the external memory state should have been previously stored
* with a dsplxmsave() call.  This function should not close the file.  It
* must return 1 if the load is successful, 0 otherwise.
*/

dsplxmload(devindex,fp) 
FILE *fp;
int devindex;
{
   /* reload external memory blocks from state file */
   int  readok;
   dsp_vmpl=dsp_vmp[devindex];
   readok=(dsplmbload(&dsp_vmpl->x_virtual,fp) &&
           dsplmbload(&dsp_vmpl->y_virtual,fp) &&
           dsplmbload(&dsp_vmpl->p_virtual,fp));
   return (readok);
}

/*
* dsplxmrd --- read an external memory location 
*
* This function must return the value of the specified external memory
* address for the specified device and memory map in to_buf.  It must also
* return 0 as the function return value if the memory location exists, 1
* otherwise.  This particular implementation gets the value from the
* virtual memory structure that defines the entire external memory of the
* device.  This function is called by the dsp device when executing memory
* fetches and by the simulator user interface routines when reading the
* display or breakpoint purposes.  The "fetch" parameter will be set to 1
* if a dsp memory fetch is occurring, 0 otherwise.
*/
/*ARGSUSED*/
dsplxmrd(devindex,memc,address,to_buf,fetch)
int devindex;
int memc; long address, *to_buf;
int fetch; 
{
   /* Read an external memory location. */
   /* ***JCB*** 
    * this function was changed to a null function, since Frigg uses
    * Thor models to simulate all external device memory.   By changing
    * this function to a null function, we cause sim56000 to simulate
    * external memory reads using the device I/O pins.  (The pins were
    * manipulated anyway, but the data bus would not be read if the
    * old version of this function were used.)
    * A more efficient modification would be to remove (disable) the entire
    * external memory simulation module (smem.c) from sim56000.  This 
    * would speed things up, and use less memory.  However, only this
    * function needed to be changed to make sim56000 work with Frigg.
    */
   return (0); 
}

/*
* dsplxmwr --- store a value in a device's external memory location
*
* This function must store the provided value in the specified external
* memory location for the device.  This particular implementation stores
* the value in the virtual memory structure that defines the entire external
* memory of the device.  The user may modify this to implement any special
* features of the external memory, for example dual-port RAM or external
* ROM locations. This function is called by the dsp device when executing
* memory stores and by the simulator user interface routines when modifying
* external memory locations.  The "store" parameter will be set to 1
* if a dsp memory store is occurring, 0 otherwise.
*/
/*ARGSUSED*/
dsplxmwr(devindex,memc,address,value,store)
int devindex,memc;
long address,value;
int store;
{
   /* Write and external memory location */
   int block, offset;
   struct memtype *mem;
   dsp_vmpl=dsp_vmp[devindex];
   
   switch (memc){
    case XMEM: mem= &dsp_vmpl->x_virtual; break;
    case YMEM: mem= &dsp_vmpl->y_virtual; break;
    case PMEM: mem= &dsp_vmpl->p_virtual; break;
    }

   block=(address>>8)&0xff; /* Select the proper block and offset */
   offset=address&0xff;
   dsplmwr(mem,block,offset,value);
}

static
dsplmfrb(mem)
struct memtype *mem;
{
   int i;
   /* free all blocks associated with a dsp 64k external memory space */
   mem->recent=mem->recent2=mem->highest=mem->lowest=0;
   for (i=0;i<256;i++){
      if (mem->mem_data[i]){
         free ((char *)mem->mem_data[i]);
         mem->mem_data[i]=NULL;
         }
      mem->disk_tbl[i]=0;
      mem->blok_fil[i]=0l;
      }
}

static
dsplmrd(mem,block,offset,to_buf)
  struct memtype *mem;
  int block,offset; /* memory start address */
  long *to_buf; /*return buffer location */
  {
  /* This function reads a virtual memory location to_buf */
  long *mptr;

  mptr=(mem->disk_tbl[block])?m_gdisk(mem,block):mem->mem_data[block];
  if (mptr){
     *to_buf= *((mptr)+offset);
     dsplmark(mem,block); /* update in-memory flags */
     }
  else *to_buf=mem->blok_fil[block];
}

static
dsplmark(mem,block)
struct memtype *mem;
int block;
{
   /* Keep track of the two most recently referenced memory blocks per map */
     if (block!=mem->recent&&block!=mem->recent2){
        mem->recent2=mem->recent;
        mem->recent=block;
        }
     if (block>mem->highest) mem->highest=block;
     if (block<mem->lowest) mem->lowest=block;
}

static
dsplmbsave(mem,fp)
struct memtype *mem;
FILE *fp;
{
   /* Stores one 64k dsp external memory space to a simulator state file */
   long *mptr;
   int i,block,writok;

   for (i=0;i<256;i++) (void) fprintf(fp,"%ld\n",mem->blok_fil[i]);
   for (block=0;block<256;block++){
      mptr=(mem->disk_tbl[block])?m_gdisk(mem,block):mem->mem_data[block];
      if (mptr){
         (void) fprintf(fp,"%d\n",block);
         for (i=0;i<256;i++){
            if (!(i&0x7)) (void) fprintf(fp,"\n");
            (void) fprintf(fp,"%lx ",*(mptr+i));
            }
         }
     }
  block= -1;
  writok=(fprintf(fp,"%d\n",block)!=EOF);
  return (writok);
}

static
dsplmbload(mem,fp)
struct memtype *mem;
FILE *fp;
{
   /* Loads one 64k dsp external memory space form a simulator state file */
   int i,block,readok;
   long *mptr;
   for(i=0,mptr=mem->blok_fil;i<256;i++) (void) fscanf(fp,"%ld ",mptr+i);
   while ((readok=fscanf(fp,"%d ",&block)) && (block>=0)){
         mem->mem_data[block]=mptr=(long *) dsp_alloc((unsigned int)blocksz);
         for(i=0;i<256;i++) (void) fscanf(fp,"%lx ",mptr+i);
         dsplmark(mem,block);
         }
   return (readok);
}

static
dsplmwr(mem,block,offset,ldata)
  struct memtype *mem; /* memory type pointer p, x or y */
  int block,offset; 
  long ldata; /* data to be written */
  {
  /* This function writes ldata into a single virtual memory location. */
  /* It will allocate memory for the virtual memory block if the block */
  /* doesn't exist.                                                    */
  /* It will load the block from disk if it is on disk. */

  int i;
  long *mptr; /* pointer to memory block */
  long blockval; /* original block value */

  mptr=(mem->disk_tbl[block])?m_gdisk(mem,block):mem->mem_data[block];

  /* if not in memory or on disk, so the block fill value is in effect */
  if (!mptr){
     blockval=mem->blok_fil[block];
     if(ldata==blockval) return; /* dont overwrite with same data */
     mem->mem_data[block]=mptr=(long *) dsp_alloc((unsigned int)blocksz); /* create new block in memory */
     for(i=0;i<256;i++) *(mptr+i)=blockval; /* fill the block with value */
     } 

  /* write in the individual selected word */
  *(mptr+offset)=ldata;
  dsplmark(mem,block);
}

static
dsplxmfrb()
{
   /* find some block of dsp memory and shove it to disk, free the memory */
   /* return 1 if successful, 0 otherwise.  Any block will do. */

   long *bptr;


   bptr=m_swapb();
   if(bptr){
      free ((char *)bptr);
      return (1);
      }
   return (0);
}
    
static
m_pdisk(mem,block)
struct memtype *mem;
int block;
{
   /* puts the specified memory block on disk and marks the disk bit table */
   /* to indicate it is on disk */
   int dblock; /* disk file destination block */
   int pos;
   long offset;
   int status;
   int i,j,val;
   long *bptr;
   char memfile[81];
   static unsigned int bitset[16]={1,2,4,8,0x10,0x20,0x40,0x80,0x100,0x200,
    0x400,0x800,0x1000,0x2000,0x4000,0x8000};

   /* get next free block in disk */
    for (i=0;(i<100) && (dsp_vmpl->dbitmap[i]==0xffff);i++);
    val=dsp_vmpl->dbitmap[i];
    for (j=0;val&bitset[j];j++);
    dblock=(i<<4)+j;

   /* save a memory block to disk.  */
   if(dsp_vmpl->disk_use==0){
      dsp_path(smpxy[dsp_vmpl->devindex],".MEM",memfile);
      dsp_vmpl->memfn=fopen(memfile,WRITEBIN);
      if (dsp_vmpl->memfn==NULL){
		  (void) fclose(dsp_vmpl->memfn);
#if FULLSIM
         dsplputs(20,0,"Error opening disk file",1);
#else
         (void) printf("\nError opening disk file %s",memfile);
#endif
         exit (1);
        }
      dsp_vmpl->disk_use=1;
     }
   /* seek to dblock position */
 
   offset=(long)dblock * (long)blocksz;
   pos=fseek(dsp_vmpl->memfn,offset,0);
   if (pos){
#if FULLSIM
         dsplputs(20,0,"Error seeking to disk file position",1);
#else
     (void) printf("\nError seeking to disk file position %ld in m_pdisk.",offset);
#endif
     (void) fclose (dsp_vmpl->memfn);
     exit (1);
     }
   bptr=mem->mem_data[block];
   status=fwrite((char *)bptr,blocksz,1,dsp_vmpl->memfn);
   if (status!=1){ /* check for error */
#if FULLSIM
         dsplputs(20,0,"Error writing block to disk file in m_pdisk.",1);
#else
     (void) printf("\nError writing block to disk file in m_pdisk.");
#endif
     (void) fclose (dsp_vmpl->memfn);
     exit (1);
     }

   /* mark the disk file structures appropriately */
   dsp_vmpl->dbitmap[i]|=bitset[j];
   /* mark the memtype structure to indicate the mem area is on disk */
   mem->disk_tbl[block]=dblock+1; 
}

