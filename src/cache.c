/*
 * cache.c
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cache.h"

/* cache configuration parameters */
static int cache_split = TRUE ;
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_isize = DEFAULT_CACHE_SIZE; 
static int cache_dsize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;

/* cache model data structures */
static Pcache icache ;
static Pcache dcache ;
static cache c1 ;
static cache c2 ;
static cache_stat cache_stat_inst;
static cache_stat cache_stat_data;

/************************************************************/
Pcache_stat get_Inst_stat(){
	return (&(cache_stat_inst)) ;
}
Pcache_stat get_Data_stat(){
	return (&(cache_stat_data)) ;
}

void set_cache_param(param, value)
  int param;
  int value;
{

  switch (param) {  	
	  case CACHE_PARAM_BLOCK_SIZE:
	    cache_block_size = value;
	    words_per_block = value / WORD_SIZE;
	    break;
	  case CACHE_PARAM_USIZE:
	    cache_split = FALSE;
	    cache_usize = value;
	    break;
	  case CACHE_PARAM_ISIZE:
	    cache_split = TRUE;
	    cache_isize = value;
	    break;
	  case CACHE_PARAM_DSIZE:
	    cache_split = TRUE;
	    cache_dsize = value;
	    break;
	  case CACHE_PARAM_ASSOC:
	    cache_assoc = value;
	    break;
	  case CACHE_PARAM_WRITEBACK:
	    cache_writeback = TRUE;
	    break;
	  case CACHE_PARAM_WRITETHROUGH:
	    cache_writeback = FALSE;
	    break;
	  case CACHE_PARAM_WRITEALLOC:
	    cache_writealloc = TRUE;
	    break;
	  case CACHE_PARAM_NOWRITEALLOC:
	    cache_writealloc = FALSE;
	    break;
	  default:
	    printf("error set_cache_param: bad parameter value\n");
	    exit(-1);
  }

}

void init_particular_cache(Pcache p,int size){
	p->size=size ;
	p->associativity=cache_assoc ;
	p->n_sets=(size/(cache_block_size*cache_assoc)) ;
	p->index_mask_offset = (LOG2(words_per_block)+2) ;	
	p->index_mask = ((unsigned) ((p->n_sets)-1))<<(p->index_mask_offset) ; 
	p->LRU_head = (Pcache_line *)malloc(sizeof(Pcache_line)*(p->n_sets)) ;
  	p->LRU_tail = (Pcache_line *)malloc(sizeof(Pcache_line)*(p->n_sets)) ;
	p->set_contents = (int *)malloc(sizeof(int)*(p->n_sets)) ;
	p->contents=0 ;
	int i ;
	for(i=0 ; i<(p->n_sets) ; i++){ 
		p->LRU_head[i]=(Pcache_line)NULL ;
    	p->LRU_tail[i]=(Pcache_line)NULL ;
		p->set_contents[i]=0 ;
	}
}
void init_particular_cache_stat(Pcache_stat st){
	st->accesses=0 ;
	st->misses=0 ;
	st->replacements=0 ;
	st->demand_fetches=0 ;
	st->copies_back=0 ;
}
/************************************************************/

/************************************************************/
void init_cache()
{	
  /* initialize the cache, and cache statistics data structures */
	icache = &c1 ;
	dcache = &c2 ;
	if(cache_split==TRUE){
		init_particular_cache(icache,cache_isize) ;
		init_particular_cache(dcache,cache_dsize) ;
	}else
		init_particular_cache(dcache,cache_usize) ;
		
	init_particular_cache_stat(&cache_stat_inst) ;
	init_particular_cache_stat(&cache_stat_data) ;

}
/************************************************************/

/************************************************************/
void handle_hit(Pcache_line p,Pcache_line *head,Pcache_line *tail){
	delete(head,tail,p) ;
	insert(head,tail,p) ;
}
unsigned cal_index(unsigned addr,int index_mask,int index_mask_offset){
	return ((addr&index_mask)>>index_mask_offset) ;
}

int get_tag(unsigned addr,int n_sets,int index_mask_offset){
	return (addr>>(LOG2(n_sets)+index_mask_offset)) ;
}

int match_tag(unsigned tag,unsigned addr,int n_sets,int index_mask_offset){
	return (tag==get_tag(addr,n_sets,index_mask_offset)) ;
}

void perform_particular_access(unsigned addr,Pcache p,int read,int cache_stat_which){
	Pcache_stat c_stat = (cache_stat_which ? (&cache_stat_data) : (&cache_stat_inst)) ;

	(c_stat->accesses)++ ; 
	unsigned index = cal_index(addr,(p->index_mask),(p->index_mask_offset)) ;
	Pcache_line itr = (p->LRU_head[index]) ;
	while(itr){
		if(match_tag((itr->tag),addr,(p->n_sets),(p->index_mask_offset))){
			/* A hit has occured */
			handle_hit(itr,&(p->LRU_head[index]),&(p->LRU_tail[index])) ;
			if(!read)
				if(cache_writeback==FALSE)
					(c_stat->copies_back)+=words_per_block ;
				else
					(itr->dirty)=1 ;						
			return  ;
		}
		itr = (itr->LRU_next) ;
	}
	/* A miss has occured */
	(c_stat->misses)++ ;	//Increments miss counter
	/* In a case in which we have to write and policy is no write allocate, hence we simply write the data into memory
	   and exit*/
	if(!read && cache_writealloc==FALSE){
		(c_stat->copies_back)+=words_per_block ; 
		return ;
	}

	/*From here there are many things to be done
	1. Since block read is compulsory at this stage hence increment
	   the demand fetches counter.
	2. Create a new Pcache_line, initialize it, insert it at the head.
	3. Increase the valid contents count of the set and the whole cache.
	4. If the policy is no write back and we have to write then 
	   copies back gets incremented.
	*/
	(c_stat->demand_fetches)+=words_per_block ;	
	Pcache_line new_line = (Pcache_line)malloc(sizeof(cache_line)) ;
	new_line->tag = get_tag(addr,p->n_sets,p->index_mask_offset) ;
	new_line->dirty = 0 ;
	insert(&(p->LRU_head[index]),&(p->LRU_tail[index]),new_line) ;
	(p->set_contents[index])++ ;
	(p->contents)++ ;
	if(!read && cache_writeback==FALSE) 
		(c_stat->copies_back)+=words_per_block ;
	
	/*Checking for the length of the list whether it exceeds the 
	  associativitiy or not. */	  	
	int set_cont = (p->set_contents[index]) ;	
	if((set_cont)>(p->associativity)){
		/* In this case one has to remove a Pcache_line from the
		   tail of the list and free the memory of the tail.
		   Also one has to decrease the valid contents count.
		   Increment the replacements counter.
		   And in case the dirty bit is set then increment copies
		   back. 
		*/
		(c_stat->replacements)++ ;
		(p->set_contents[index])-- ;
		(p->contents)-- ;
		Pcache_line tl = p->LRU_tail[index] ;
		if(tl->dirty)
			(c_stat->copies_back)+=words_per_block ;
		delete(&(p->LRU_head[index]),&(p->LRU_tail[index]),tl) ;
		free(tl) ;
	}
}

void perform_access(addr, access_type)
	unsigned addr, access_type;
{
  /* handle an access to the cache */
	switch(access_type){
		case 0 : perform_particular_access(addr,dcache,1,1) ;
		break ;
		case 1 : perform_particular_access(addr,dcache,0,1) ;
		break ;
		case 2 : if(cache_split) 
					perform_particular_access(addr,icache,1,0) ;
				 else
				 	perform_particular_access(addr,dcache,1,0) ;
		break ;
	}
}
/************************************************************/

/************************************************************/
void flush()
{
	int i ;
	for(i=0 ; i<(icache->n_sets) ; i++){
		Pcache_line itr = icache->LRU_head[i] ;
		while(itr){
			if(itr->dirty)
				cache_stat_inst.copies_back+=words_per_block ;
			delete(&(icache->LRU_head[i]),&(icache->LRU_head[i]),itr) ;
			itr = icache->LRU_head[i] ;
		}
	}
	for(i=0 ; i<(dcache->n_sets) ; i++){
		Pcache_line itr = dcache->LRU_head[i] ;
		while(itr){
			if(itr->dirty)
				cache_stat_inst.copies_back+=words_per_block ;
			delete(&(dcache->LRU_head[i]),&(dcache->LRU_head[i]),itr) ;
			itr = dcache->LRU_head[i] ;
		}
	}
}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  if (item->LRU_prev) {
    item->LRU_prev->LRU_next = item->LRU_next;
  } else {
    /* item at head */
    *head = item->LRU_next;
  }

  if (item->LRU_next) {
    item->LRU_next->LRU_prev = item->LRU_prev;
  } else {
    /* item at tail */
    *tail = item->LRU_prev;
  }
}
/************************************************************/

/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  item->LRU_next = *head;
  item->LRU_prev = (Pcache_line)NULL;

  if (item->LRU_next)
    item->LRU_next->LRU_prev = item;
  else
    *tail = item;

  *head = item;
}
/************************************************************/

/************************************************************/
void dump_settings()
{
  printf("*** CACHE SETTINGS ***\n");
  if (cache_split) {
    printf("  Split I- D-cache\n");
    printf("  I-cache size: \t%d\n", cache_isize);
    printf("  D-cache size: \t%d\n", cache_dsize);
  } else {
    printf("  Unified I- D-cache\n");
    printf("  Size: \t%d\n", cache_usize);
  }
  printf("  Associativity: \t%d\n", cache_assoc);
  printf("  Block size: \t%d\n", cache_block_size);
  printf("  Write policy: \t%s\n", 
	 cache_writeback ? "WRITE BACK" : "WRITE THROUGH");
  printf("  Allocation policy: \t%s\n",
	 cache_writealloc ? "WRITE ALLOCATE" : "WRITE NO ALLOCATE");
}
/************************************************************/

/************************************************************/
void print_stats()
{
  printf("\n*** CACHE STATISTICS ***\n");

  printf(" INSTRUCTIONS\n");
  printf("  accesses:  %d\n", cache_stat_inst.accesses);
  printf("  misses:    %d\n", cache_stat_inst.misses);
  if (!cache_stat_inst.accesses)
    printf("  miss rate: 0 (0)\n"); 
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n", 
	 (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses,
	 1.0 - (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses);
  printf("  replace:   %d\n", cache_stat_inst.replacements);

  printf(" DATA\n");
  printf("  accesses:  %d\n", cache_stat_data.accesses);
  printf("  misses:    %d\n", cache_stat_data.misses);
  if (!cache_stat_data.accesses)
    printf("  miss rate: 0 (0)\n"); 
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n", 
	 (float)cache_stat_data.misses / (float)cache_stat_data.accesses,
	 1.0 - (float)cache_stat_data.misses / (float)cache_stat_data.accesses);
  printf("  replace:   %d\n", cache_stat_data.replacements);

  printf(" TRAFFIC (in words)\n");
  printf("  demand fetch:  %d\n", cache_stat_inst.demand_fetches + 
	 cache_stat_data.demand_fetches);
  printf("  copies back:   %d\n", cache_stat_inst.copies_back +
	 cache_stat_data.copies_back);
}
/************************************************************/
