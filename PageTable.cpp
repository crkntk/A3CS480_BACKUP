//
// Created by astro on 3/19/2024.
//
#include "PageTable.h"
#include <cctype>
#include <cstdlib>
#include <queue>
#include "Map.h"
using namespace std;
PageTable::PageTable(int levels, std::queue <int> bitsPerL, int inpOffset, PageNode *rootPtr, int *levelBits,int maxFrames){
    PageSize = 0;
    for(int i = 0; i< bitsPerL.size();i++){
        PageSize += sizeof(PageNode);
    }
    numLevels = levels;
    offsetBits = inpOffset;
    rootNodePtr  = &*rootPtr;
    createMasksNShifts(bitsPerL);
    ArrayBitPer = &*levelBits;
    allocatedMemory = sizeof(PageTable);
    AddressCount = 0;
    replStructure = new PageReplacement(maxFrames,this);
}

void PageTable::createMasksNShifts(queue <int> bitsPerLevel){
    unsigned int aMask = 1;
    unsigned int numOfMaskBits = 0;
    int totalBitsProcc = 0;
    bitMaskAry = (unsigned int *) malloc(bitsPerLevel.size());
    shiftAr = (int *) malloc(bitsPerLevel.size());
    entryCount = (int *) malloc(bitsPerLevel.size());
    allocatedMemory += 3*bitsPerLevel.size();
    for(int level = 0; level < numLevels;level++){
        int currBitNum = bitsPerLevel.front();
        totalBitsProcc += currBitNum;
        numOfMaskBits = currBitNum;
        for (int b = 1; b < numOfMaskBits; b++)
        {
            aMask = aMask << 1;
            aMask = aMask | 1;
        }
        int leftShift = 32 - totalBitsProcc;
        aMask = aMask << leftShift;
        bitMaskAry[level] = aMask;
        shiftAr[level] = leftShift;
        bitsPerLevel.pop();
        aMask = 1;
    }
    aMask = 1;
    numOfMaskBits = 32 - totalBitsProcc;
    for (int b = 1; b < numOfMaskBits; b++)
    {
        aMask = aMask << 1;
        aMask = aMask | 1;
    }
    offsetMask = aMask;
}
void PageTable::insertMapForVpn2Pfn (PageTable *pagetable, unsigned int virtualAddress, int frame) {
    PageNode currNode = *(pagetable->rootNodePtr);
    unsigned int shifted;
    for(int level =0;level < (pagetable->numLevels) ; level++){
        unsigned int masked = bitMaskAry[level] & virtualAddress;
        shifted = masked >> shiftAr[level];
        if((level < numLevels -2)){
            if(currNode.NextLevel[shifted] == NULL) {
                currNode.NextLevel[shifted] = new PageNode((pagetable->ArrayBitPer)[level + 1], 0, pagetable, level + 1);
            }
            else{
                currNode = *(currNode.NextLevel[shifted]);
                continue;
            }
        }
        else if(level == numLevels - 2){
            if(currNode.NextLevel[shifted] == NULL) {
                currNode.NextLevel[shifted] = new PageNode((pagetable->ArrayBitPer)[level + 1], 1, pagetable, level + 1);
            }
            else{
                currNode = *(currNode.NextLevel[shifted]);
                continue;
            }
        }
        else if(level == numLevels - 1){
            if(currNode.pageToFrame[shifted].valid == -1){
                Map *AddingMap = new Map(1,frame);
                (currNode.pageToFrame[shifted]) = *AddingMap;
                break;
            }
            else{
                break;
            }
        }
        currNode = *(currNode.NextLevel[shifted]);
    }
}

Map * PageTable::searchMappedPfn(PageTable *pageTable, unsigned int virtualAddress){
    return pageTable->rootNodePtr->searchMappedPfn(virtualAddress);
}
unsigned int PageTable::extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift){
    unsigned int shifted;
    unsigned int masked = mask & virtualAddress;
    shifted = masked >> shift;
    return shifted;
}
void PageTable::replaceNode(unsigned int oldVPN, unsigned int newVpn, int frame){
    PageNode currNode = *(rootNodePtr);
    unsigned int shifted;
    for(int level =0;level < (numLevels) ; level++){
        unsigned int masked = bitMaskAry[level] & oldVPN;
        shifted = masked >> shiftAr[level];
        if(level == numLevels - 1){
                currNode.pageToFrame[shifted].valid = -1;
                currNode.pageToFrame[shifted].frame = -1;
                break;
        }
        currNode = *(currNode.NextLevel[shifted]);
    }
    insertMapForVpn2Pfn(this,newVpn,frame);
}