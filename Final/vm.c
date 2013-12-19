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

int main(){
    FILE *ptr_file;
    char buf[1000];
    long address;
    int pageTable [256] = {[0 ... 255] = -1}; //one slot per frame
    long pageNumber;
    long pa;
    long offset;


    ptr_file = fopen("address.txt", "r");
    if (!ptr_file){
        return 1;
    }

    //read in the file
    while (fgets(buf,1000,ptr_file) != NULL){
        sscanf(buf,"%ld", &address);
        pageNumber = getPage(address);
        offset = getOffset(address);

        printf("address:  %ld\n", address);
        printf("page number:  %ld\n", getPage(address));

        //check if the entry is in the TLB
        int currentFrame;
        long hitFrameNumber = checkTLB(pageNumber);
        if (hitFrameNumber != -1){ //there's a hit
            currentFrame = hitFrameNumber;
        }
        else{
            if (tlbIndex == 4){ //implement the lru policy
                printf("LRU!\n");
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
            printf("tlbIndex now:  %d\n", tlbIndex);
            pageTable[pageNumber] = frame;
            frame = frame + 1;
            currentFrame = pageTable[pageNumber];
        }
        //update the ages of everything else
        updateAgePage(pageNumber);
        pa = getPA(currentFrame,offset);
        printf("frame number:  %d\n", currentFrame);
        printf("offset:  %ld\n", offset);
        printf("pa:  %ld\n", pa);
        //printf("pageTable entry:  %d\n", pageTable[pageNumber]);
        printf("\n");
    }
    //print pages
/*    int j;
    for (j = 0; j < 256; j++){
        printf("page:  %d, age:  %ld\n", j, agePage[j]);
    }*/
    //getOldestPage();
    //print out tlb
    int m;
    for (m = 0; m < 4; m++){
        printf("entry:  %d, page: %ld, frame:  %ld\n", m, tlbPage[m], tlbFrame[m]);
    }
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
    printf("    current frame in get PA:  %d\n", frame);
    return (frame * 256) + offset;
}

//check the TLB for the page
long checkTLB(long pageNumber){
    int i;
    for (i = 0; i < 4; i++){
        //if the page is found, return it
        if (tlbPage[i] == pageNumber){
            printf("    HIT!\n");
            printf("    returning frame:  %ld\n", tlbFrame[i]);
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
            printf("i is:  %ld\n", i );
            printf("updating age of %ld to %ld\n", i, agePage[i] + 1);
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
     printf("oldest page is:  %d\n",oldestPage);
     return oldestPage;
}

void updateTLB(int oldPage, long newPage, int newFrame){
    //reset the age of the old page
    agePage[oldPage] = -1;
    int i;
    for (i = 0; i < 4; i++){
        if (tlbPage[i] == oldPage){ //find the old page
            //insert the new page and update
            tlbPage[i] = newPage;
            tlbFrame[i] = newFrame;
            break;
        }
    }
}