#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

int read_trace_element(FILE *infile, unsigned *access_type, unsigned *addr);

int main(int argc, char** argv)
{

  srand(time(0));
  
  
FILE *trace_file;
  
trace_file = fopen(argv[1], "r");
  
unsigned address, read;
  
// 16-bit address
int d_mask = 0x1ff;
int two_mask = 0xff;

int  total_fetches, cache_read_hit= 0, cache_read_miss = 0,cache_write_miss = 0,cache_write_hit = 0, miss_ratio, hit_ratio,i,n,j=0; 
int  two_cache_read_hit= 0, two_cache_read_miss = 0, two_cache_write_miss = 0, two_cache_write_hit = 0;

//Memory = 512kb, Cache= 1kb

//int memory[8],two_memory[8];
int cache[512], two_cache[512];

//size of data = 8 bytes

int data;

// Block size = 8 bytes

// Number of blocks = 128 

unsigned cache_tag[512];

unsigned two_cache_tag[512];

int valid_bit[512], dirty_bit[512], two_valid_bit[512], two_dirty_bit[512];

// Number of bits for block offset = 7

unsigned two_way_tag, direct_mapped_tag;

int two_way_index, direct_mapped_index;

int set_number;

int block_evictions;

int lru_counter[512], h_lru_count[256];

int a;

int found, w_found;

int temp_lru, temp_index;

//Fill the main memory with data values

 /*for(a=0; a<8;a++)
 {
	 memory[a]= (rand()%100)+1;
	 two_memory[a] = memory[a];
 }*/

 for(a=0;a<512;a++)
   {
     cache[a]=0;
	 two_cache[a]=0;
     cache_tag[a]=0;
	 two_cache_tag[a]=0;
     valid_bit[a]=0;
	 two_valid_bit[a]=0;
     dirty_bit[a]=0;
	 two_dirty_bit[a]=0;	

	 if((a%2)==0)
		lru_counter[a]=2;
	 else
		lru_counter[a]=1;	 
 
	 if(a<256)
		 h_lru_count[a] = 2;
   }
 /*
 printf("            						  Initial tables are as follows  \n");
 printf("\n");
 printf("											Direct mapped  \n");
 printf("\n");
					printf(" c b no.	c t value    data in c    v bit  d bit     mem addr      \n");
			printf("\n");
			
			for(a = 0; a<512; a++)
			{
				
				if(a<512)
				{
					printf("  %d  		    %x        %d	           %d    %d ", a, cache_tag[a], cache[a], valid_bit[a], dirty_bit[a]);
			
					printf("         %d			  \n",a);
					
				}
				else
					printf("   							   %d		         \n",a);
				
			}	
 printf("\n");
 printf("											Two way  \n");
 j=0;
 printf("\n");
						printf(" s no.  c b no.	c t value    data in c   LRU  	h_e LRU    v bit   d bit     mem addr     \n");
			printf("\n");
			for(a = 0; a<512; a++)
			{
				
				if(a<512)
				{
					if(a%2==0)
					{
						printf("  %d  	  %d  	   %x  		%d	 %d		%d	  %d   %d ", a/2, a, two_cache_tag[a], two_cache[a], lru_counter[a], h_lru_count[j], two_valid_bit[a], two_dirty_bit[a]);
						j++;
					}
					else
						printf("  %d  	  %d  	   %x  		%d	 %d			  %d   %d ", a/2, a, two_cache_tag[a], two_cache[a], lru_counter[a], two_valid_bit[a], two_dirty_bit[a]);
				
						printf("          %d		  \n",a);
				
				}
				else
					
						printf("    		  								 %d			 \n",a);
			
				
			}	
			
			
 printf("\n");
 //printf("Enter the number of iterations you would like to perform \n");
 //scanf("%d",&n);

// Use random number generator to generate 16-bit address

 printf("\n"); */

while(read_trace_element(trace_file, &read, &address))
	{
		
		
		 
		if(read == 0 || read == 2)
			read = 1; // reading if read was zero or two as per trace
		else
			read = 0; // writing if read was 1 as per trace

		 data = (rand()%100)+1;

	//Direct Mapped
		
	/*	printf("			Direct mapped \n");
		printf("\n");
		if(read)
		{
			printf("			Read Operation \n");
		}
		else
		{
			printf("   Write Operation \n");
			printf("			Data given by processor: %d \n",data );
		}	
		printf("\n");
		printf("			Address generated by processor is: %x \n", address);
		printf("\n"); */
	
	
		direct_mapped_index = (address >> 4) & d_mask;
		direct_mapped_tag = address >> 13;
		
		//printf("			Go to location %d in cache   \n", direct_mapped_index );

		if(read)
		{

			if((direct_mapped_tag == cache_tag[direct_mapped_index]) && valid_bit[direct_mapped_index]) // the address tag matches cache tag and data is valid, i.e it is what we want
			{
				//printf("\n");
				//printf("			Direct mapped Read Hit \n");
				cache_read_hit++;// then its a hit

			// processor uses that data

			}
			else // if its a miss
			{
				// printf("\n");
				// printf("			Direct mapped Read Miss \n");
				 cache_read_miss++;
				 
				 if(dirty_bit[direct_mapped_index] == 1) // if the dirty bit was 1, then only write back the data to memory since its write back
						 {
							 //printf(" write existing data back to memory \n");
							 //memory[(cache_tag[direct_mapped_index] << 2) | direct_mapped_index] = cache[direct_mapped_index]; // writing the existing data to memory
						 }

				 cache[direct_mapped_index] = data; // data loaded from main memory into cache

				 valid_bit[direct_mapped_index] = 1; // now that data is loaded from main memory, valid bit becomes 1 
				 
				 dirty_bit[direct_mapped_index] = 0; // since a fresh data is loaded from main memory, dirty bit is 0;

				 cache_tag[direct_mapped_index] = direct_mapped_tag; // tag is updated in cache with new address' tag

				 // processor uses that data

			}

		}
		else // write statements go over here  
		 //we're implementing write-back and write allocate
		{
			 if(direct_mapped_tag == cache_tag[direct_mapped_index] && valid_bit[direct_mapped_index]) // checking the tag of address to be written to is same as what processor provided 
			   {
					if(cache[direct_mapped_index] != data) // if the data sent by processor is not same as the one already present in that cache location
					{
						 dirty_bit[direct_mapped_index] = 1; // change dirty bit to 1
						 cache[direct_mapped_index] = data; // write the new data in cache
					}
					else					
					{
						dirty_bit[direct_mapped_index] = 0; // if data is already there then dirty bit is 0 since  no change was done
					}
					valid_bit[direct_mapped_index] = 1; // now that we have valid data, the valid bit is 1
					
					cache_write_hit++;
					//printf("\n");
					//printf("			Direct mapped Write Hit \n");
			   }
			 else // its a write miss
			   {
			 
					if(dirty_bit[direct_mapped_index] == 1) // if the dirty bit was 1, then only write back the data to memory since its write back
					 {
						// printf(" write existing data back to memory \n");
						 //memory[(cache_tag[direct_mapped_index] << 2) | direct_mapped_index] = cache[direct_mapped_index]; // writing the existing data to memory
					 }

					// printf(" write new data to memory \n");
					 //memory[address] = data; // write new data to memory

					 cache[direct_mapped_index] = data; // write new data to cache 
					 
					 cache_tag[direct_mapped_index] = direct_mapped_tag; // update the cache with new tag

					 dirty_bit[direct_mapped_index] = 0; // dirty bit is 0 as this is the first time we have the new value from write operation but we put that value in memory just now

					 valid_bit[direct_mapped_index] = 1; // now that valid data is in cache, valid bit becomes 1 
					 
					 cache_write_miss++;
					// printf("\n");
					// printf("			Direct mapped Write Miss \n");
			   }

		}
		/*
			printf("\n");
					printf(" c b no.	c t value    data in c    v bit  d bit     mem addr      \n");
			printf("\n");
			
			for(a = 0; a<512; a++)
			{
				if(a<512)
				{
					printf("  %d  		    %x        %d	           %d    %d ", a, cache_tag[a], cache[a], valid_bit[a], dirty_bit[a]);
			
					printf("         %d			  \n",a);
					
				}
				else
					printf("   							   %d		         \n",a);
				
			}	*/
		
	
	
	   // 2-way set associativity
		
	/*	printf("			Two way set associative cache \n");
		printf("\n");
		if(read)
		{
			printf("			Read Operation \n");
		}
		else
		{
			printf("			Write Operation \n");
			printf("			Data given by processor: %d \n",data );
		}	
		printf("\n");
		printf("			Address generated by processor is: %x \n", address);
		printf("\n"); */
		
		
		two_way_index = (address >> 4)& two_mask;
		two_way_tag = address >> 12;
		
		//printf("			Go to set number %d in cache   \n", two_way_index);
		
		if(read)
		{	
			found = 0;	
			for(a = 0; a < 2; a++) // going to appropriate set and looping through
			{
				if ((two_way_tag == two_cache_tag[(two_way_index << 1) | a]) && two_valid_bit[(two_way_index << 1) | a]) // the address tag matches cache tag and data is valid, i.e it is what we want
				{
					//printf("\n");
					//printf("			Two way Read Hit \n");
					two_cache_read_hit++;// then its a hit
					found = 1; // data was found
					// processor uses that data
					temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
					
					temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on
					
					break;
				}	
			}
			if(!found)  // if its a miss
			{
				two_cache_read_miss++;
				//printf("\n");
				//printf("			Two way Read Miss \n");
				for(a=0;a<2;a++)  // we see if any locations in the set were empty
				{
					if(two_cache[(two_way_index << 1) | a] == 0 && lru_counter[(two_way_index << 1) | a] == h_lru_count[two_way_index]) // place it in empty spot with highest LRU count for empty spot							
						{
							two_cache[(two_way_index << 1) | a] = data; // get the value from memory and store it in location with highest lru value for empty spot
							
							two_cache_tag[(two_way_index << 1) | a] = two_way_tag; // update the tag in cache
							
							two_valid_bit[(two_way_index << 1) | a] = 1; // set the valid bit to 1 as its the first time value is loaded in cache
							
							two_dirty_bit[(two_way_index << 1) | a] = 0; // setting dirty bit to 0 as new value was loaded into cache from memory
							
							h_lru_count[two_way_index]--; // the highest lru count for an empty spot is now decreased by 1
							
							temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
							
							temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on
							
							break; // exiting the loop when our goal was achieved
						}	
				}
				
				if(h_lru_count[two_way_index] == 0) // when set is no longer empty
				{
					
					for(a=0;a<2;a++) // we want to see which location in set has to be replaced
					{
						if (lru_counter[(two_way_index << 1) | a]==1) // check for location which was least recently used
						{
							if(two_dirty_bit[(two_way_index << 1) | a]==1) // check to see if dirty bit was 1
							{
								//printf(" write existing data back to memory \n");
								//two_memory[(two_cache_tag[two_way_index] << 1) | two_way_index] = two_cache[(two_way_index << 1) | a]; // if so, place the cache value into memory
							}
							
							two_cache[(two_way_index << 1) | a] = data; // replace the location which was least recently used with data from memory
							
							two_cache_tag[(two_way_index << 1) | a] = two_way_tag; // update the tag in cache
							
							temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
							
							temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on
							
							two_dirty_bit[(two_way_index << 1) | a] = 0; // setting dirty bit to 0 as new value was loaded into cache from memory
							
							two_valid_bit[(two_way_index << 1) | a] = 1; // data in cache is valid
							
							break; // exiting the loop when our goal was achieved
						}
					}
				}
				
			}	
			
		}
		else // write statements go over here  
		 //we're implementing write-back and write allocate
		{
			
		  w_found=0;
		  
		  for(a=0;a<2;a++) // looping through the set 
		  {
			  if((two_cache_tag[(two_way_index << 1) | a] == two_way_tag) && (two_valid_bit[(two_way_index << 1) | a] == 1))  // seeing if the location we wanna write to (we know this from tag) is already present 
			  {
				  w_found = 1; // we found it
				 // printf("\n");
				 // printf("			Two Way Write Hit \n");
				  two_cache_write_hit++;
				  
				 /* if(!two_valid_bit[(two_way_index << 1) | a]) // the location was empty initially
				  h_lru_count[two_way_index]--;*/
				  
				  two_valid_bit[(two_way_index << 1) | a] = 1; 
				  
				  if(two_cache[(two_way_index << 1) | a] != data) // if the data sent by processor is new, i.e not same as one found for that tag
				  {
					  two_cache[(two_way_index << 1) | a] = data; // write new data in cache
					  two_dirty_bit[(two_way_index << 1) | a] = 1; // set dirty bit to one as new data was found
				  }
				  else
				  { 
					two_dirty_bit[(two_way_index << 1) | a] = 0; // if processor didn't provide new data, set dirty bit to 0
				  }
				  
				temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
				
				temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on						

				break;	
			  }
		  }
		  if(!w_found) // if that tag wasn't found, 
			  
			  {
				  two_cache_write_miss++;
				//  printf("\n");
				 // printf("			Two Way Write Miss \n");
				  
				  for(a=0;a<2;a++) // loop through the set 
				  {
				  
					if(two_cache[(two_way_index << 1) | a]==0 && lru_counter[(two_way_index << 1) | a] == h_lru_count[two_way_index]) // we search for an empty location with highest LRU count for an empty spot
					{
						two_cache[(two_way_index << 1) | a] = data; // load the new data given by processor in cache
						
						//two_memory[address] = data; // write new data to memory
						
						two_cache_tag[(two_way_index << 1) | a] = two_way_tag; // update the tag in cache
						
						two_valid_bit[(two_way_index << 1) | a] = 1; // set the valid bit to 1 as its the first time value is loaded in cache
						
						two_dirty_bit[(two_way_index << 1) | a] = 0; // setting dirty bit to 0 as value loaded in cache was just loaded in memory too
						
						h_lru_count[two_way_index]--; // the highest lru count for an empty spot is now decreased by 1
						
						temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
						
						temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on
						
						break; // exiting the loop when our goal was achieved
					}
				  }
				
				if(h_lru_count[two_way_index] == 0) //all the blocks are occupied
					{
							
						for(a=0;a<2;a++) // we want to see which location in set has to be replaced
						{
							if (lru_counter[(two_way_index << 1) | a] == 1) // check for location which was least recently used
							{
								if(two_dirty_bit[(two_way_index << 1) | a]==1) // check to see if dirty bit was 1
								{
									//printf(" write existing data back to memory \n");
									//two_memory[(two_cache_tag[two_way_index] << 1) | two_way_index] = two_cache[(two_way_index << 1) | a]; // if so, place the cache value into memory
								}
								//two_memory[address] = data; // write new data to memory
								
								two_cache[(two_way_index << 1) | a] = data; // replace the location which was least recently used with data from processor
								
								two_cache_tag[(two_way_index << 1) | a] = two_way_tag; // update the tag in cache
								
								temp_lru = lru_counter[(two_way_index << 1) | a]; // keeping track of the lru value of the location which was operated on
								
								temp_index = (two_way_index << 1) | a; // keeping track of the location which was operated on
								
								two_dirty_bit[(two_way_index << 1) | a] = 0; // setting dirty bit to 0 as the value loaded in cache was also just loaded in memory
								
								two_valid_bit[(two_way_index << 1) | a] = 1; // the data is valid
								
								break; // exiting the loop when our goal was achieved
							}
						}
					}
			  }
									
		}
					
			for(a=0;a<2;a++) // looping through the lru counters for the particular set for updating
				
			{
				if(lru_counter[(two_way_index << 1) | a] > temp_lru) // if any lru value is greater than the value of location operated upon
					lru_counter[(two_way_index << 1) | a]--; // decrement it by 1
			}
			
			lru_counter[temp_index] = 2; // set the lru to be highest for the location which was just operated upon
			/*
			j=0;
			printf("\n");

						printf(" s no.  c b no.	c t value    data in c   LRU  	h_e LRU    v bit   d bit     mem addr     \n");
			printf("\n");
			for(a = 0; a<512; a++)
			{
				if(a<512)
				{
					if(a%2==0)
					{
						printf("  %d  	  %d  	   %x  		%d	 %d		%d	  %d   %d ", a/2, a, two_cache_tag[a], two_cache[a], lru_counter[a], h_lru_count[j], two_valid_bit[a], two_dirty_bit[a]);
						j++;
					}
					else
						printf("  %d  	  %d  	   %x  		%d	 %d			  %d   %d ", a/2, a, two_cache_tag[a], two_cache[a], lru_counter[a], two_valid_bit[a], two_dirty_bit[a]);
				
						printf("          %d		 \n",a);
				
				}
				else
					
						printf("    		  								 %d			 \n",a);
				
			}	*/
			
			
			
			
   }
      
 printf(" The total cache read hits for direct mapped = %d \n",cache_read_hit); 
 printf(" The total cache read misses for direct mapped are = %d \n",cache_read_miss); 
 printf(" The total cache write hit for direct mapped are = %d \n",cache_write_hit); 
 printf(" The total cache write misses for direct mapped are = %d \n",cache_write_miss); 
 printf("\n");
 printf(" The total cache read hits for two way set associativity = %d \n",two_cache_read_hit); 
 printf(" The total cache read misses for two way set associativity = %d \n",two_cache_read_miss); 
 printf(" The total cache write hit for two way set associativity are = %d \n",two_cache_write_hit); 
 printf(" The total cache write misses for two way set associativity are = %d \n",two_cache_write_miss); 

return 0;
}

int read_trace_element(FILE *inFile, unsigned *access_type, unsigned *addr)
{
  int result;
  char c;

  result = fscanf(inFile, "%u %x%c", access_type, addr, &c);
  while (c != '\n') {
    result = fscanf(inFile, "%c", &c);
    if (result == EOF) 
      break;
  }
  if (result != EOF)
    return(1);
  else
    return(0);
}