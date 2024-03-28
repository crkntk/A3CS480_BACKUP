//
// Created by astro on 3/26/2024.
//

#ifndef CS480PAGES_PAGEREPLACEMENT_H
#define CS480PAGES_PAGEREPLACEMENT_H

#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <unordered_map>
#include "LoadedPage.h"
#include "PageTable.h"
class PageTable;
typedef struct{
    unsigned int BitString = 1 << 15;
    int VPN = -1;
}BitVPN;
class PageReplacement {
public:
    PageReplacement(int totalFrames,PageTable *currTable);
    int numFrames;
    std::map<unsigned int,LoadedPage> loadedPages;
    std::unordered_map<unsigned int, bool> accessedPages;
    PageTable *primeTable;
    void accessPage(unsigned int vpn,int frameNumber, bool intervEnd, int currAddCount);
    void updateBitStrings();
    int findVictimFrame();
    BitVPN * handlePageFault(unsigned int vpn,int frameNumber,bool intervEnd, int currAddCount, bool firstInterval);
};


#endif //CS480PAGES_PAGEREPLACEMENT_H
