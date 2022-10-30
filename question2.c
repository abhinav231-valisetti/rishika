/*
 * CS3375 Computer Architecture
 * Course Project
 * Cache Simulator Design and Development
 * FALL 2017
 * By Yong Chen
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char *trace_file_name;

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

void execute_cache(int cache_size,int no_of_blocks, int nway, int block_size)
{
    // Every thing is ready , just call the file and do the hit
    int no_of_sets =  no_of_blocks/ nway;
    struct direct_mapped_cache
    {
        unsigned valid_field[no_of_blocks]; /* Valid field */
        unsigned dirty_field[no_of_blocks]; /* Dirty field; since we don't distinguish writes and \\
                                             reads in this project yet, this field doesn't really matter */
        uint64_t tag_field[no_of_blocks];   /* Tag field */
        int hits;                                  /* Hit count */
        int misses;                                /* Miss count */
    };
    struct direct_mapped_cache d_cache;
    /* Initialization */
    for (int i = 0; i < no_of_blocks; i++)
    {
        d_cache.valid_field[i] = 0;
        d_cache.dirty_field[i] = 0;
        d_cache.tag_field[i] = 0;
    }
    d_cache.hits = 0;
    d_cache.misses = 0;

    char mem_request[20];
    FILE *fp;
    fp = fopen(trace_file_name, "r");
    uint64_t address;

    while (fgets(mem_request, 20, fp) != NULL)
    {
            address = convert_address(mem_request);
            uint64_t block_addr = address >> (unsigned)log2(block_size);
            uint64_t set_number = block_addr % no_of_sets;
            uint64_t tag = block_addr >> (unsigned)log2(no_of_sets);
            
        
            int start_i = ((int)set_number) * nway;
            int end_i = start_i + nway;
           
            int i;
                   
            int check_miss = 1;
            
            int check_empty = 0;
          
           
            for( i = start_i ; i < end_i ; i++)
            {
             
             if(d_cache.valid_field[i] && d_cache.tag_field[i] == tag)
             {
              
               d_cache.hits = d_cache.hits + 1;
               check_miss = 0;
               break;
             }
            }
              
          if (check_miss == 1)
            {
              d_cache.misses +=1;
              
           for( i = start_i ; i < end_i ; i++)
            {
             
             if(d_cache.valid_field[i] == 0)
             {
              
               d_cache.valid_field[i] = 1;
               d_cache.tag_field[i] = tag;
               check_empty = 1;
               break;
             }
            }
             }
            
           
            if (check_empty == 0)
            {
              int randomNumber = rand()%((end_i+1) - start_i) + start_i;
              d_cache.valid_field[randomNumber] = 1;
              d_cache.tag_field[randomNumber] = tag;
            }
        }
        /*Print out the results*/
        printf("\n==================================\n");
        printf("Cache type:    Associative Cache\n");
        printf("==================================\n");
        printf("Cache Hits:    %d\n", d_cache.hits);
        printf("Cache Misses:  %d\n", d_cache.misses);
        printf("\n");
        
        int total_sum = d_cache.hits + d_cache.misses;
        
        float hit_rate_percentage = ((float)d_cache.hits/(float)total_sum)*100;
        
        float miss_rate_percentage = ((float)d_cache.misses/(float)total_sum)*100;
        
     printf("Cache Hit Rate in Percentage %.1f\n", hit_rate_percentage);
     printf("Cache Miss Rate in Percentage  %.1f\n", miss_rate_percentage);  
}


void question1()
{
    
    long cache_size = 32 * 1024;
    int no_of_blocks = 0;
    int block_size;
    printf("Select 16 for 16 bytes cache line size\nSelect 32 for 32 bytes cache line size\n Select 128 for 128 bytes cache line size : \n");
    scanf("%d", &block_size);
    int nway = 0;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &nway);
    
     no_of_blocks = (int)cache_size / block_size;
      
     if (nway == 1)
     {
      nway = no_of_blocks;
     }
     
     
     execute_cache(32, no_of_blocks, nway, block_size);
   
}


void question2()
{
 printf("Select 16 for 16 KB cache size\nSelect 32 for 32 KB cache size\n Select 64 for 64 KB cache size : \n");
    int cache_size;
    scanf("%d", &cache_size);
    
    cache_size = cache_size*1024;
    
    int no_of_blocks = 0;
    int block_size = 64;
    
    int nway = 0;
    printf("Select 1 for fully associative execution, 2 for 2 way execution, 4 for 4 way execution, 8 for 8 way execution: ");
    scanf("%d", &nway);
    
     no_of_blocks = (int)cache_size / block_size;
      
     if (nway == 1)
     {
      nway = no_of_blocks;
     }
     
     
     execute_cache(32, no_of_blocks, nway, block_size);
   
}

int main(int argc, char *argv[])
{
    trace_file_name = argv[2];
    int select_question;
    printf("Select the Question No. to execute 1/2 : ");
    scanf("%d", &select_question);

    switch (select_question)
    {
    case 1:
    
     question1();
   
        break;

    case 2:
    question2();
    break;
        
    }
}
