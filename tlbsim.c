// William Rongerude, 2025

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned char valid : 1;
  unsigned char physical_page : 4;
} PageTableEntry;

#define PAGE_TABLE_SIZE 16

typedef struct {
  PageTableEntry entries[PAGE_TABLE_SIZE];
} PageTable;

typedef struct {
  unsigned int valid : 1;
  unsigned int order;
  unsigned char virtual_page_no;
  unsigned char physical_page_no;
} TLB_Entry;

#define DEFAULT_TLB_SIZE 4

void update_tlb_order(TLB_Entry *tlb, int tlb_size, int accessed_index) {
  // update the accessed entry to 0
  int accessed_order = tlb[accessed_index].order;
  tlb[accessed_index].order = 0;

  for (int i = 0; i < tlb_size; i++) {
    if (i != accessed_index && tlb[i].valid && tlb[i].order < accessed_order) {
      tlb[i].order++;
    }
  }
}

int simulate_memory_access(char *filename, int tlb_size) {
  PageTable page_table;
  for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
    page_table.entries[i].valid = (i % 2 == 0);
    page_table.entries[i].physical_page = i;
  }

  TLB_Entry *tlb = (TLB_Entry *)malloc(tlb_size * sizeof(TLB_Entry));

  if (tlb == NULL) {
    fprintf(stderr, "Failed to allocate memory");
    return 1;
  }
  for (int i = 0; i < tlb_size; i++) {
    tlb[i].valid = 0;
    tlb[i].order = i;
  }

  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("Cannot open file");
    free(tlb);
    return 1;
  }

  unsigned short virtual_address;
  int access_size;
  int tlb_hits = 0;
  int tlb_misses = 0;
  int seg_faults = 0;

  while (fscanf(fp, "%hx %d", &virtual_address, &access_size) == 2) {
    // start virtual page number to end virtual page number
    unsigned short start_page = virtual_address >> 12;
    unsigned short end_page = (virtual_address + access_size - 1) >> 12;
    bool invalid_access = false;
    for (unsigned short vpn = start_page;
         vpn <= end_page && vpn < PAGE_TABLE_SIZE; vpn++) {
      if (!page_table.entries[vpn].valid) {
        invalid_access = true;
        break;
      }
    }

    if (invalid_access) {
      seg_faults++;
      continue;
    }

    for (unsigned short vpn = start_page;
         vpn <= end_page && vpn < PAGE_TABLE_SIZE; vpn++) {
      bool hit = false;
      // check if it hits
      for (int i = 0; i < tlb_size; i++) {
        // it's in the tlb
        if (tlb[i].valid && tlb[i].virtual_page_no == vpn) {
          tlb_hits++;
          update_tlb_order(tlb, tlb_size, i);
          hit = true;
          break;
        }
      }
      if (!hit) {
        tlb_misses++;
        int lru_index = 0;
        for (int i = 1; i < tlb_size; i++) {
          if (tlb[i].order > tlb[lru_index].order) {
            lru_index = i;
          }
        }
        // evict entry in tlb
        tlb[lru_index].valid = 1;
        tlb[lru_index].virtual_page_no = vpn;
        tlb[lru_index].physical_page_no = page_table.entries[vpn].physical_page;
        update_tlb_order(tlb, tlb_size, lru_index);
      }
    }
  }
  fclose(fp);
  free(tlb);

  printf("TLB Hits: %d\n", tlb_hits);
  printf("TLB Misses: %d\n", tlb_misses);
  printf("Segfaults: %d\n", segFaults);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <access_trace_file> [tlb_size]\n", argv[0]);
    return 1;
  }
  int tlb_size = DEFAULT_TLB_SIZE;
  if (argc == 3) {
    tlb_size = atoi(argv[2]);
    if (tlb_size < 1) {
      fprintf(stderr, "TLB size must be >= 1\n");
      return 1;
    }
  }
  return simulate_memory_access(argv[1], tlb_size);
}
