//
// Created by astro on 3/26/2024.
//

#ifndef CS480PAGES_LOADEDPAGE_H
#define CS480PAGES_LOADEDPAGE_H

#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_map>
class LoadedPage {
public:
    LoadedPage(unsigned int vaddr);
    unsigned int vpn;
    unsigned int bitString = 1 << 15; // Bit string to track recent accesses
    int lastAccessTime = 0;
};


#endif //CS480PAGES_LOADEDPAGE_H
