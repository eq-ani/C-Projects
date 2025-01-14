#include "bits.h"
#include "cache.h"

int get_set(Cache *cache, address_type address) 
{
  int change = (1 << cache->set_bits);
  change--;
  return (address >> cache->block_bits) & change;
}

int get_line(Cache *cache, address_type address) 
{
  return address >> (cache->set_bits + cache->block_bits);
}

int get_byte(Cache *cache, address_type address) 
{
  int change = (1 << cache->block_bits);
  change--;
  return address & change;
}