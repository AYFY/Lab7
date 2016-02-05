#include "mem.h"

static mem_t* find_last_block(){
    mem_t* cur_block = HEAP_START;
    while (cur_block->next != NULL)
        cur_block = cur_block->next;
    return cur_block;
}

static mem_t* find_free_block(size_t query){
    mem_t* cur_block = HEAP_START;
    while(cur_block != NULL){
        if (cur_block->capacity >= query & cur_block->is_free == 1)
            return cur_block;
        cur_block = cur_block->next;
    }
    return NULL;
}

static void separate_block(mem_t* cur_block, size_t query){
	mem_t* new_block = (char*)(cur_block) + sizeof(mem_t) + query;
	
	new_block->is_free = 1;
	new_block->capacity = cur_block->capacity - query - sizeof(mem_t);
	new_block->next = cur_block->next;
	
	cur_block->is_free = 0;
	cur_block->capacity = query;
	cur_block->next = new_block;
}

static void* allocate_new_block(mem_t* last_block, size_t query){
	mem_t* new_block = mmap((char*)(last_block) + last_block->capacity + sizeof(mem_t), 
		query, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	
	if (new_block){
		last_block->next = new_block;
		new_block->is_free = 0;
		new_block->capacity = query;
		new_block->next = NULL;
		return (char*)(new_block) + sizeof(mem_t);
	}
	else{
		new_block = mmap(NULL, query, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (new_block){
			last_block->next = new_block;
			new_block->is_free = 1;
			new_block->capacity = query;
			new_block->next = NULL;
			return (char*)(new_block) + sizeof(mem_t);
		}
	}
	return NULL;
}

void* _malloc(size_t query){
    mem_t* good_block = find_free_block(query);
    if (good_block == NULL){
		return allocate_new_block(find_last_block(), query);
    }
    else{
		/* it means that block should be separeted */
		if (good_block->capacity >= sizeof(mem_t) + query)
			separate_block(good_block, query);
		/* there we use block how as is */
		else good_block->is_free = 0;
		
		return (char*)(good_block) + sizeof(mem_t);
    }
}

static mem_t* find_prev_block(mem_t* find_block){
	mem_t* cur_block = HEAP_START;
	while (cur_block != NULL){
		if (cur_block->next == find_block) return cur_block;
		cur_block = cur_block->next;
	}
	return NULL;
}

static void concatenate_blocks(mem_t* prev_block, mem_t* block){
	prev_block->next = block->next;
	prev_block->capacity = prev_block->capacity + block->capacity + sizeof(mem_t);
	prev_block->is_free = 1;
	block->next = NULL;
}

void _free(void* mem){
	mem_t* prev_block = NULL;
	mem_t* next_block = NULL;
	if (mem){
		mem_t* block_for_free = mem - sizeof(mem_t);
		prev_block = find_prev_block(block_for_free);
		next_block = block_for_free->next;
		
		block_for_free->is_free = 1;
		
		if (next_block != NULL && next_block->is_free == 1 &&
			next_block == (char*)(block_for_free) + sizeof(mem_t) + block_for_free->capacity)
			concatenate_blocks(block_for_free, next_block);
		
		if (prev_block != NULL && prev_block->is_free == 1 &&
			prev_block == (char*)(block_for_free) - sizeof(mem_t) - prev_block->capacity)
			concatenate_blocks(prev_block, block_for_free);
	}
}

void* heap_init(size_t initial_size){
    void* ptr = mmap(HEAP_START, initial_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    mem_t* block = ptr;
    block->capacity = initial_size - sizeof(mem_t);
    block->is_free = 1;
    block->next = NULL;
    return ptr + sizeof(mem_t);
}

void memalloc_debug_struct_info(FILE* f, mem_t const * const address){
    size_t i;
    fprintf(f, "start: %p\nsize: %lu\nis_free:%d\n", (void*)address, address->capacity, address->is_free);

    for(i = 0; i < DEBUG_FIRST_BYTES && i < address->capacity; i++)
        fprintf(f, "%hhX", ((char*)address) [sizeof(mem_t) + i]);
    putc('\n', f);
}

void memalloc_debug_heap(FILE* f, mem_t const * ptr){
    for( ; ptr; ptr = ptr->next){
        memalloc_debug_struct_info(f, ptr);
    }
}