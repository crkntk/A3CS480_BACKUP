//
// Created by astro on 3/26/2024.
//

#include "LoadedPage.h"
LoadedPage::LoadedPage(unsigned int vaddr){
    vpn = vaddr;
    bitString = 1 << 15;
    lastAccessTime = 0;
}