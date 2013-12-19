#include <stdio.h>

//prototypes
long getPage(long address);
long getOffset(long address);
long getPA(int frame, long offset);
long checkTLB(long pageNumber);
void updateAgePage();
int getOldestPage();
void updateTLB(int oldPage, long newPage, int newFrame);

//acts like tlb
long tlbPage [16] = {[0 ... 15] = -1};
long tlbFrame [16] = {[0 ... 15] = -1};
int frame = 0; //keeps track of current frame
int tlbIndex = 0; //keeps track of available slot in tlb
long agePage [256] = {[0 ... 255] = -1}; //keeps track of ages of each page
int TLB_SIZE = 16;

int main(){
    FILE *ptr_file;
    char buf[1000];
    long address;
    int pageTable [256] = {[0 ... 255] = -1}; //one slot per frame
    long pageNumber;
    long pa;
    long offset;
    int hitTLB = 0;
    int counterAddresses = 0;
    int counterTLBMisses = 0;

    ptr_file = fopen("address.txt", "r");
    if (!ptr_file){
        return 1;
    }

    //read in the file
    while (fgets(buf,1000,ptr_file) != NULL){
        sscanf(buf,"%ld", &address);
        counterAddresses = counterAddresses + 1;
        pageNumber = getPage(address);
        offset = getOffset(address);

        //check if the entry is in the TLB
        int currentFrame;
        long hitFrameNumber = checkTLB(pageNumber);
        if (hitFrameNumber != -1){ //there's a hit
            currentFrame = hitFrameNumber;
            hitTLB = 1;
        }
        else{
            hitTLB = 0;
            counterTLBMisses = counterTLBMisses + 1;
            if (tlbIndex == TLB_SIZE){ //implement the lru policy
                //get the page that has the oldest age
                int oldestPage = getOldestPage();
                updateTLB(oldestPage, pageNumber, frame);
            }
            else{
                //add to the TLB
                tlbPage[tlbIndex] = pageNumber;
                tlbFrame[tlbIndex] = frame;
                tlbIndex = tlbIndex + 1;
            }
            pageTable[pageNumber] = frame;
            frame = frame + 1;
            currentFrame = pageTable[pageNumber];
        }
        //update the ages of everything else
        updateAgePage(pageNumber);
        pa = getPA(currentFrame,offset);
        printf("%8ld     ", address);
        printf("%8ld     ", pa);
        if (hitTLB > 0){
            printf("%8s     ", "PAGE-HIT");
            printf("%9s     ", "TLB-HIT");
        }
        else{
            printf("%9s     ", "PAGE-MISS");
            printf("%9s     ", "TLB-MISS");
        }
        printf("\n");
    }
    printf("Number of virtual address = %d\n", counterAddresses);
    printf("Number of page faults = %d\n", counterTLBMisses);
    printf("Number of TLB misses = %d\n", counterTLBMisses);
    fclose(ptr_file);
    return 0;
}

long getPage(long address){
    return address / 256;
}

long getOffset(long address){
    return address % 256;
}

//get the physical address
long getPA(int frame, long offset){
    return (frame * 256) + offset;
}

//check the TLB for the page
long checkTLB(long pageNumber){
    int i;
    for (i = 0; i < TLB_SIZE; i++){
        //if the page is found, return it
        if (tlbPage[i] == pageNumber){
            return tlbFrame[i];
        }
    }
    return -1;
}

void updateAgePage(long pageNumber){
    //set the age to 0
    agePage[pageNumber] = 0;
    //if the page exists then update it
    long i;
    for (i = 0; i < 256; i++){
        if ((agePage[i] != -1) && (i != pageNumber)){
            agePage[i] = agePage[i] + 1;
        }
    }
}

int getOldestPage(){
     int i;
     int max = 0;
     int oldestPage;
     for (i = 0; i < 256; i++){
        if (agePage[i] > max && agePage[i] != -1){
            max = agePage[i];
            oldestPage = i;
        }
     }
     return oldestPage;
}

void updateTLB(int oldPage, long newPage, int newFrame){
    //reset the age of the old page
    agePage[oldPage] = -1;
    int i;
    for (i = 0; i < TLB_SIZE; i++){
        if (tlbPage[i] == oldPage){ //find the old page
            //insert the new page and update
            tlbPage[i] = newPage;
            tlbFrame[i] = newFrame;
            break;
        }
    }
}