#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct l1_cache
{
    unsigned valid_field[1024];
    unsigned dirty_field[1024];
    uint64_t tag_field[1024];
    char data_field[1024][64];
    int hits;
    int misses;
};

struct l2_cache
{
    unsigned valid_field[16384];
    unsigned dirty_field[16384];
    uint64_t tag_field[16384];
    char data_field[16384][64];
    int hits;
    int misses;
};

uint64_t convert_address(char memory_addr[])
/* Converts the physical 32-bit address in the trace file to the "binary" \\
 * (a uint64 that can have bitwise operations on it) */
{
    uint64_t binary = 0;
    int i = 0;

    while (memory_addr[i] != '\n')
    {
        if (memory_addr[i] <= '9' && memory_addr[i] >= '0')
        {
            binary = (binary * 16) + (memory_addr[i] - '0');
        }
        else
        {
            if (memory_addr[i] == 'a' || memory_addr[i] == 'A')
            {
                binary = (binary * 16) + 10;
            }
            if (memory_addr[i] == 'b' || memory_addr[i] == 'B')
            {
                binary = (binary * 16) + 11;
            }
            if (memory_addr[i] == 'c' || memory_addr[i] == 'C')
            {
                binary = (binary * 16) + 12;
            }
            if (memory_addr[i] == 'd' || memory_addr[i] == 'D')
            {
                binary = (binary * 16) + 13;
            }
            if (memory_addr[i] == 'e' || memory_addr[i] == 'E')
            {
                binary = (binary * 16) + 14;
            }
            if (memory_addr[i] == 'f' || memory_addr[i] == 'F')
            {
                binary = (binary * 16) + 15;
            }
        }
        i++;
    }

#ifdef DBG
    printf("%s converted to %llu\n", memory_addr, binary);
#endif
    return binary;
}

int check_in_l1(uint64_t address, int nway, struct l1_cache *l1)
{   

    uint64_t block_addr = address >> (unsigned)log2(64);
    int set_number = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int start_i = ((int)set_number) * nway;
    int end_i = start_i + nway;
    
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l1->valid_field[i] && l1->tag_field[i] == tag)
        {
            return 1;
        }
    }
    return 0;
}


int check_in_l2(uint64_t address, int nway, struct l2_cache *l2)
{
     uint64_t block_addr = address >> (unsigned)log2(64);
    int set_number = block_addr % 2048;
    uint64_t tag = block_addr >> (unsigned)log2(2048);
    int start_i = ((int)set_number) * nway;
    
    int end_i = start_i + nway;
    
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l2->valid_field[i] && l2->tag_field[i] == tag)
        {
            return 1;
        }
    }
    return 0;
}


void insert_in_l1(uint64_t address, int nway, struct l1_cache *l1)
{
    uint64_t block_addr = address >> (unsigned)log2(64);
    int set_number = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int start_i = ((int)set_number) * nway;
    int end_i = start_i + nway;
    
    int check_empty = 0;
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l1->valid_field[i] == 0)
        {
            check_empty = 1;
        }
    }

    if (check_empty > 0)
    {
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l1->valid_field[i] == 0)
        {
           l1->valid_field[i] = 1;
           l1->tag_field[i] = tag;
           break;
        }
    }
    }
    else
    {
        int randomIndex = (rand() % (end_i - start_i + 1)) + start_i;
        //   printf("Picking a rand variable %d",randomIndex);
        l1->valid_field[randomIndex] = 1;
        l1->tag_field[randomIndex] = tag;
    }
}
void insert_in_l2(uint64_t address, int nway, struct l2_cache *l2)
{
   uint64_t block_addr = address >> (unsigned)log2(64);
    int set_number = block_addr % 512;
    uint64_t tag = block_addr >> (unsigned)log2(512);
    int start_i = ((int)set_number) * nway;
    int end_i = start_i + nway;
    
    int check_empty = 0;
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l2->valid_field[i] == 0)
        {
            check_empty = 1;
        }
    }

    if (check_empty > 0)
    {
    for(int i = start_i ; i < end_i ; i++)
    {
        if (l2->valid_field[i] == 0)
        {
           l2->valid_field[i] = 1;
           l2->tag_field[i] = tag;
           break;
        }
    }
    }
    else
    {
        int randomIndex = (rand() % (end_i - start_i + 1)) + start_i;
        //   printf("Picking a rand variable %d",randomIndex);
        l2->valid_field[randomIndex] = 1;
        l2->tag_field[randomIndex] = tag;
    }
}

int main(int argc, char *argv[])
{
    
    char *trace_file_name;

    FILE *fp;
    trace_file_name = argv[2];
 
    int no_of_blocks_l1 = 1024;
    int no_of_blocks_l2 = 16384;
    int l1_nway = 2;
    int l2_nway = 8;
    
    int no_of_sets_l1 = ((int) 1024 / (int) l1_nway);
    int no_of_sets_l2 = ((int) 2048 / (int) l2_nway);
    
    char mem_request[20];
    uint64_t address;
    struct l1_cache l1;
    struct l2_cache l2;
    for (int i = 0; i < no_of_blocks_l1; i++)
    {
        l1.valid_field[i] = 0;
        l1.dirty_field[i] = 0;
        l1.tag_field[i] = 0;
    }
    for (int i = 0; i < no_of_blocks_l2; i++)
    {
        l2.valid_field[i] = 0;
        l2.dirty_field[i] = 0;
        l2.tag_field[i] = 0;
    }

    l1.hits = 0;
    l1.misses = 0;
    l2.hits = 0;
    l2.misses = 0;

    fp = fopen(trace_file_name, "r");

    
        while (fgets(mem_request, 20, fp) != NULL)
        {
            address = convert_address(mem_request);
            
            
            if (check_in_l1(address, l1_nway, &l1))
            {
                l1.hits++;
                l2.hits++;
            }
            else
            {
              l1.misses++;
 
                if (check_in_l2(address, l2_nway, &l2))
                {
                    l2.hits += 1;
                }
                else
                {
                    l2.misses++;
                    insert_in_l2(address, l2_nway, &l2);
                }
                insert_in_l1(address, l1_nway, &l1);
            }
            }
   
       printf("\n==================================\n");
        printf("Cache type:    l1 Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", l1.hits);
        printf("Cache Misses:  %d\n", l1.misses);
        printf("\n");
        
        int total_sum = l1.hits + l1.misses;
        
        float hit_rate_percentage = ((float)l1.hits/(float)total_sum)*100;
        
        float miss_rate_percentage = ((float)l1.misses/(float)total_sum)*100;
        
     printf("Cache Hit Rate in Percentage %.1f\n", hit_rate_percentage);
     printf("Cache Miss Rate in Percentage  %.1f\n", miss_rate_percentage);  
        printf("\n==================================\n");
        printf("\n==================================\n");
        printf("Cache type:    Direct-Mapped Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", l2.hits);
        printf("Cache Misses:  %d\n", l2.misses);
        printf("\n");
        
        total_sum = l2.hits + l2.misses;
        
        hit_rate_percentage = ((float)l2.hits/(float)total_sum)*100;
        
        miss_rate_percentage = ((float)l2.misses/(float)total_sum)*100;
        
     printf("Cache Hit Rate in Percentage %.1f\n", hit_rate_percentage);
     printf("Cache Miss Rate in Percentage  %.1f\n", miss_rate_percentage);  
        
        fclose(fp);

    return 0;
    }

