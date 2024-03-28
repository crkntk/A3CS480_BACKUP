//
// Created by astro on 3/19/2024.
//

#ifndef CS480PAGES_PAGETABLE_H
#define CS480PAGES_PAGETABLE_H
#include "PageNode.h"
#include "PageTable.h"
#include "Map.h"
#include <queue>
#include "PageReplacement.h"
class PageNode;
class PageReplacement;
class PageTable{
public:
    PageTable(int levels, std::queue <int> bitsPerL,int inpOffset, PageNode* rootNode, int *levelBits, int maxFrames);
    int numLevels;
    unsigned int *bitMaskAry;
    unsigned int offsetMask;
    int *shiftAr;
    int *entryCount;
    void createMasksNShifts(std::queue <int> bitsPerLevel);
    int offsetBits;
    PageNode *rootNodePtr;
    int *ArrayBitPer;
    Map * searchMappedPfn(PageTable *pageTable, unsigned int virtualAddress);
    void insertMapForVpn2Pfn (PageTable *pagetable, unsigned int virtualAddress, int frame);
    unsigned int extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
    unsigned long allocatedMemory;
    unsigned long PageSize;
    int AddressCount; // count how many addresses have been processed
    PageReplacement *replStructure;//replacement structure
    void replaceNode(unsigned int oldVPN, unsigned int newVpn, int frame);
};
#endif //CS480PAGES_PAGETABLE_H
