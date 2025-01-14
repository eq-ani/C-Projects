#include "lru.h"
#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

void lru_init_queue(Set *set) {
  LRUNode *s = NULL;
  LRUNode **pp = &s;  // place to chain in the next node
  for (int i = 0; i < set->line_count; i++) {
    Line *line = &set->lines[i];
    LRUNode *node = (LRUNode *)(malloc(sizeof(LRUNode)));
    node->line = line;
    node->next = NULL;
    (*pp) = node;
    pp = &((*pp)->next);
  }
  set->lru_queue = s;
}

void lru_init(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    lru_init_queue(&sets[i]);
  }
}

void lru_destroy(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    LRUNode *p = sets[i].lru_queue;
    LRUNode *n = p;
    while (p != NULL) {
      p = p->next;
      free(n);
      n = p;
    }
    sets[i].lru_queue = NULL;
  }
}

void lru_fetch(Set *set, unsigned int tag, LRUResult *result) 
{
    LRUNode *curr = set->lru_queue;
    LRUNode *prev = NULL;
    while (curr != NULL) 
    {
        if (curr->line->valid && curr->line->tag == tag) 
        {
            result->line = curr->line;
            result->access = HIT;
            if (prev != NULL) 
            {
                prev->next = curr->next;    
                curr->next = set->lru_queue; 
                set->lru_queue = curr;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    curr = set->lru_queue;
    while (curr != NULL) 
    {
        if (!curr->line->valid) 
        { 
            curr->line->valid = 1;  
            curr->line->tag = tag;  
            result->line = curr->line;
            result->access = COLD_MISS;
            if (curr != set->lru_queue) 
            {
                if (prev != NULL) prev->next = curr->next;
                curr->next = set->lru_queue;
                set->lru_queue = curr;
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    LRUNode *lru_node = set->lru_queue;
    prev = NULL;
    while (lru_node->next != NULL) 
    {
        prev = lru_node;
        lru_node = lru_node->next;
    }
    result->line = lru_node->line;
    result->access = CONFLICT_MISS;
    result->line->tag = tag; 

    if (prev != NULL) 
    {
        prev->next = NULL;         
        lru_node->next = set->lru_queue; 
        set->lru_queue = lru_node;
    }
}
