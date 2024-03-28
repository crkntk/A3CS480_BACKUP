//
// Created by astro on 1/26/2024.
//
#ifndef CS480PAGES_PAGENODE_H
#define CS480PAGES_PAGENODE_H
#include <iostream>
#include <cstring>
#include "PageTable.h"
#include "Map.h"


class PageTable;
class PageNode {
public:
    PageTable *Table;
    PageNode(int bits, int leafFlag, PageTable *currTable, int depth);
    int currDepth;
    PageNode **NextLevel;
    Map *pageToFrame;
    Map * searchMappedPfn (unsigned int virtualAddress);
};

#endif //CS480PAGES_PAGENODE_H
