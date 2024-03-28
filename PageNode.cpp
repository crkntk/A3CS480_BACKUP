//
// Created by astro on 1/26/2024.
//

#include "PageNode.h"
#include <cctype>
#include <cstring>
#include "PageTable.h"
#include <cmath>
#include "Map.h"

PageNode::PageNode(int bits,int leafFlag, PageTable *currTable,int depth){
    currDepth = depth;
    Table = currTable;
    if(leafFlag ==0) {
        NextLevel = (PageNode **) malloc((pow(2,bits)) * sizeof(PageNode *));
        Table->allocatedMemory += (pow(2,bits)) * sizeof(PageNode *);
        for (int i = 0; i < pow(2,bits); i++) {
            NextLevel[i] = (PageNode *) malloc(sizeof(PageNode *));
            Table->allocatedMemory += sizeof(PageNode *);
            NextLevel[i] = nullptr;
            if (NextLevel[i] == NULL) {
               //printf("Error: Unable to allocate memory for columns in row %d\n", i);
               return ;
           }
        }
    }
    else{
        pageToFrame = (Map *) malloc(pow(2,bits) * sizeof(Map));
        Table->allocatedMemory += pow(2,bits) * sizeof(Map);
        for (int i = 0; i < pow(2,bits); i++) {
            Map *currMap = new Map(-1,-1);
            pageToFrame[i] = *currMap;
        }
    }
}

Map* PageNode::searchMappedPfn(unsigned int virtualAddress) {
    unsigned int shifted;
    unsigned int masked = Table->bitMaskAry[currDepth] & virtualAddress;
    shifted = masked >> Table->shiftAr[currDepth];
        if(currDepth != (Table->numLevels) - 1){
            if(NextLevel[shifted] == NULL) {
                return NULL;
            }
            else{
                return NextLevel[shifted]->searchMappedPfn(virtualAddress);
            }
        }
        else{
            if(pageToFrame[shifted].valid == -1){
                return NULL;
            }
            else {
                return &(pageToFrame[shifted]);
            }
        }
}
