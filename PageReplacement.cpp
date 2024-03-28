//
// Created by astro on 3/26/2024.
//

#include "PageReplacement.h"
#include "PageReplacement.h"

PageReplacement::PageReplacement(int totalFrames,PageTable *currTable){
    numFrames = totalFrames;
    primeTable = &(*currTable);
};

void PageReplacement::updateBitStrings() {
    printf("\nSTART UPDATE ON BIT STRINGS\n\n");
    for (int ind = 0; ind<loadedPages.size();ind++) {// Right shift the bit string
        printf("VPN: %08X\n",loadedPages.at(ind).vpn);
        printf("BITSTRING: %08X\n",loadedPages.at(ind).bitString);
        loadedPages.at(ind).bitString = loadedPages.at(ind).bitString >> 1;
        if (accessedPages[ind]) {
            printf("ADDED A 1\n");
            unsigned int AddedBit = 1 << 15;
            loadedPages.at(ind).bitString = loadedPages.at(ind).bitString + AddedBit; // Set the most significant bit if page accessed during NFU period
        }
        printf("BITSTRING AFTER SHIFTING MAY HAVE ADDED: %08X\n",loadedPages.at(ind).bitString);
    }
    printf("END UPDATE BIT STRING\n\n");
    //resets the last access for the ones that were accessed
    for (int ind = 0; ind<accessedPages.size();ind++) {
        if(accessedPages[ind]){
            accessedPages[ind] = false;
        }
    }
}

int PageReplacement::findVictimFrame() {
    int victimFrameIndex = 0;
    unsigned int smallestBitString = loadedPages.at(0).bitString;
    unsigned int smallestLastAccessTime = loadedPages.at(0).lastAccessTime;

    for (int i = 1; i < loadedPages.size(); i++) {
        unsigned int currentBitString = loadedPages.at(i).bitString;
        int currentLastAccessTime = loadedPages.at(i).lastAccessTime;

        if (currentBitString < smallestBitString ||(currentBitString == smallestBitString && currentLastAccessTime < smallestLastAccessTime)) {
            victimFrameIndex = i;
            smallestBitString = currentBitString;
            smallestLastAccessTime = currentLastAccessTime;
        }
    }
    return victimFrameIndex;
}

// Function for the first access to set bitstring
//will be used on ony access
void PageReplacement::accessPage(unsigned int vpn,int frameNumber, bool intervEnd, int currAddCount) {
    // Update accessed pages for current NFU period
    accessedPages[frameNumber] = true;
    if(intervEnd){
        updateBitStrings();
    }
    //increment the last access time
        (loadedPages.at(frameNumber)).lastAccessTime = currAddCount;
}


// Function to handle page fault and perform page replacement
//will be used on inserts in pagetable
BitVPN * PageReplacement::handlePageFault(unsigned int vpn,int frameNumber,bool intervEnd, int currAddCount, bool firstInterval) {
    if (loadedPages.size() < numFrames) {
        if(intervEnd) {
            updateBitStrings();
        }
            // If there are free frames, simply load the page into a free frame
                LoadedPage *pageAdd = new LoadedPage(vpn);
                loadedPages.insert(std::pair<int, LoadedPage>(frameNumber, *pageAdd));
                if(intervEnd) {
                    accessedPages[frameNumber] = false;
                }
                else{
                    accessedPages[frameNumber] = true;
                }
                (loadedPages.at(frameNumber)).lastAccessTime = currAddCount;
            return nullptr;
    }
    else {
        // If all frames are used, perform page replacement
        //increment the last access time
        // Update bit strings before page replacement
        if(intervEnd){
            updateBitStrings();
        }
        // Find the victim frame based on NFU with aging algorithm
        int victimFrameIndex = findVictimFrame();
        unsigned int victFrVpn = (loadedPages.at(victimFrameIndex).vpn);
        unsigned int victFrBitStr = (loadedPages.at(victimFrameIndex).bitString);
        // Replace the victim frame with the new page
        loadedPages.erase(victimFrameIndex);
        LoadedPage *pageAdd = new LoadedPage(vpn);
        loadedPages.insert(std::pair<int, LoadedPage>(victimFrameIndex, *pageAdd));
        if(intervEnd) {
            accessedPages[victimFrameIndex] = false;
        }
        else{
            accessedPages[victimFrameIndex] = true;
        }
        loadedPages.at(victimFrameIndex).lastAccessTime = currAddCount;
        primeTable->replaceNode(victFrVpn,vpn,victimFrameIndex);
        auto *pastBitVpn = new BitVPN();
        pastBitVpn->VPN = victFrVpn;
        pastBitVpn->BitString = victFrBitStr;
        return pastBitVpn;
    }
}