#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HEAPSIZE 127
#define MAXARGV 80

// save word to the memory heap (as a implicit free list)
static unsigned char heap[HEAPSIZE]={0};
// save the blocks that are either free or allocated
static unsigned char blocklist[HEAPSIZE]={0};
// current block index
static int current_next_index=0;
// current size of the heap, not exceeding 126
static int current_size=0;


int get_min_diff(int req_size) {
    // best-fit for the next free block
    int min_diff = 128;
    int min_index = -1;
    int i=0;
    for (;i<current_size;i++) {
        int blk_size = blocklist[heap[i]]>>1;
        int status = blocklist[heap[i]]&1;
        printf("blocklist item is: %d\n",blocklist[heap[i]]);
        printf("currentIndex: %d, current status: %d, current size: %d\n", i, status, blk_size);
        if (!status && blk_size>req_size && (blk_size-req_size)<min_diff) {
            min_diff = blk_size-req_size;
            min_index=heap[i];
        }
    }
    return min_index;
}

void printHeap() {
    int i=0;
    printf("Heap is: [");
    for(;i<current_size;i++){
        printf("%d,",heap[i]);
    }
    printf("]\n");
    printf("current size: %d\n", current_size);
}

int custom_malloc(int req_size) {
    // printf("In Malloc: %d\n", req_size);
    // blocklist
    unsigned char new_header=req_size<<1|1;
    int best_fit_blk = get_min_diff(req_size);
    
    if (best_fit_blk == -1) {
        printf("%d\n",-1);
        return 0;
    }

    int old_size = (int)(unsigned char)blocklist[best_fit_blk]>>1;

    blocklist[best_fit_blk] = new_header;
    blocklist[best_fit_blk+req_size+1] = (old_size-req_size-1)<<1;
    heap[current_size++]=best_fit_blk+req_size+1;

    printf("%d\n",best_fit_blk+1);
    printHeap();
    return 0;
}


int custom_free(int addr) {
    printf("In Free: %d\n", addr);
    // merge
    int i = 0;
    for (;i<current_size;i++) {
        if (heap[i]==(addr-1)) {
            blocklist[heap[i]]=blocklist[heap[i]]>>1<<1;
            
            // merge if next block is free
            if (!(blocklist[heap[i+1]]&1)) {
                unsigned char next_header = blocklist[heap[i+1]];
                int next_size = (int)(unsigned char)next_header>>1;
                int old_size = (int)(unsigned char)blocklist[heap[i]]>>1;
                int new_size =  old_size + next_size + 1;
                unsigned char new_header = new_size<<1;
                blocklist[heap[i]] = new_header;
                blocklist[heap[i+1]]=0;

                // shift heap array
                current_size--;
                int j=i+1;
                for (;j<current_size;j++) {
                    heap[j]=heap[j+1];
                }

                // printHeap();
            }
            break;
        }
    }
    return 0;
}


int print_blocklist() { 
    // printf("In blocklist\n");
    int i=0;
    for (;i<current_size;i++) {
        int block_num=heap[i];
        int heap_size=blocklist[heap[i]]>>1;
        int status=blocklist[heap[i]]&1;
        printf("%d, %d, %s\n", block_num+1, heap_size, status?"allocated":"free");
        
    }
    return 0; 
}


int writemem(int addr, char* content) {
    printf("in write mem: addr:%d content:%s\n", addr, content);
    // heap
    return 0;
}


int printmem(int addr, int num) {
    printf("in printmem: addr:%d num:%d\n", addr, num);
    return 0;
}


int main(){
    char command_str[MAXARGV];
    // initialize heap
    unsigned char size_char=(HEAPSIZE-1)<<1;
    heap[0]=0; // first block number
    current_size++;
    blocklist[0] = size_char;
    current_next_index++;

    // shell 
    printf(">");
    
    while(fgets(command_str, MAXARGV, stdin)) {
        if (strlen(command_str)) {
            if (strlen(command_str) > 3 && !strncmp(command_str, "quit", 4)) {
                return 0;
            } else if (!strncmp(command_str, "malloc", 6)){
                char* line = strtok(command_str, " ");
                line = strtok(NULL, " ");
                int req_size = atoi(line);
                custom_malloc(req_size);
            } else if (!strncmp(command_str, "free", 4)) {
                char* line = strtok(command_str, " ");
                line = strtok(NULL, " ");
                int addr = atoi(line);
                custom_free(addr);
            } else if (!strncmp(command_str, "blocklist", 9)) {
                print_blocklist();
            } else if (!strncmp(command_str, "writemem", 8)) {
                char* argv = strtok(command_str, " ");
                char* argv2;
                argv = strtok(NULL, " ");
                argv2 = strtok(NULL, " ");
                int addr = atoi(argv);
                // strip newline char
                if (argv2[strlen(argv2)-1]=='\n') {
                    argv2[strlen(argv2)-1]='\0';
                }
                writemem(addr, argv2);
            } else if (!strncmp(command_str, "printmem", 8)) {
                char* argv = strtok(command_str, " ");
                char* argv2;
                argv = strtok(NULL, " ");
                argv2 = strtok(NULL, " ");
                int addr = atoi(argv);
                int num = atoi(argv2);
                printmem(addr, num);
            }
        }
        printf(">");
        
    }

	return 0;
}
