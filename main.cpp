//Red ID: 826172939
//Jon Lee
#include "PageNode.h"
#include "PageTable.h"
#include "log_helpers.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <queue>
#include "array"
#include "vaddr_tracereader.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Define default values
    int num_accesses = -1;
    int num_frames = 999999;
    int bitstring = 10;
    std::string log_mode = "summary";
    char *trace_file;
    std::string bitsString;

    // Parse optional command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "n:f:b:l:trace.tr")) != -1) {
        switch (opt) {
            case 'n':
                num_accesses = std::stoi(optarg);
                if (num_accesses < 1) {
                    std::cout << ("Number of memory accesses must be a number and greater than 0") << std::endl;
                    return 0;
                }
                break;
            case 'f':
                num_frames = std::stoi(optarg);
                if (num_frames < 1) {
                    std::cout << ("Number of available frames must be a number and greater than 0") << std::endl;
                    return 0;
                }
                break;
            case 'b':
                bitstring = std::stoi(optarg);
                if (bitstring < 1) {
                    std::cout << ("Bit string update interval must be a number and greater than 0") << std::endl;
                    return 0;
                }
                break;
            case 'l':
                log_mode = optarg;
                break;
        }
    }

    // Opening file
    if (optind >= argc) {
        std::cout << "Trace file not provided" << std::endl;
        return 0;
    }
    trace_file = argv[optind++];
    FILE *pfile;
    pfile = fopen(trace_file, "r");
    if (pfile == NULL) {
        std::cout << "Unable to open <<";
        std::cout << trace_file;
        std::cout << ">>" << std::endl;
        return 0;
    }

    // Process level bits
    int total_bits = 0;
    std::queue<int> bitsPerL;
    int ArrrLevelBits[argc - optind];
    int indexArr = 0;
    for (int i = optind; i < argc; ++i) {
        int bits = std::stoi(argv[i]);
        //printf("%d\n",bits); //debuging
        ArrrLevelBits[indexArr] = bits;
        indexArr++;
        bitsPerL.push(bits);
        if (bits < 1) {
            std::cout << "Level " + std::to_string(i - optind) + " page table must have at least 1 bit" << std::endl;
            return 0;
        }
        total_bits += bits;
    }
    if (total_bits > 28) {
        std::cout << "Too many bits used for the page table" << std::endl;
        return 0;
    }
    PageNode *rootNode;
    PageTable *primeTable = new PageTable(bitsPerL.size(), bitsPerL, 32 - total_bits, nullptr, ArrrLevelBits,
                                          num_frames);
    if (bitsPerL.size() > 1) {
        rootNode = new PageNode(bitsPerL.front(), 0, primeTable, 0);
    } else {
        rootNode = new PageNode(bitsPerL.front(), 1, primeTable, 0);
    }
    primeTable->rootNodePtr = &*rootNode;
    primeTable->rootNodePtr->Table = primeTable;
    primeTable->createMasksNShifts(bitsPerL);
    p2AddrTr mtrace;
    unsigned int vAddr;
    int frameNumber = 0;
    int callToInsert;
    int hits = 0;
    int nullsfound = 0;
    int addressProc = 0;
    unsigned long bitsUsed = 0;
    int intervalCounter = 0;
    BitVPN *vpnReplaced;
    int replaced;
    unsigned int replacedBitStr = -1;
    int vpnbeforFault = -1;
    bool hitMiss = false;
    bool firstInterval = true;
    //printf("Total bits initial %d",total_bits);
    int frameToLog = 0;
    if (num_accesses <= 0) {
        while (!feof(pfile)) {
            if (NextAddress(pfile, &mtrace)) //tracef_h - file handle from fopen
            {
                vAddr = mtrace.addr;
                Map *foundMap = primeTable->searchMappedPfn(primeTable, vAddr);
                if (foundMap == NULL) {
                    nullsfound++;
                    replacedBitStr = primeTable->replStructure->loadedPages.at(frameNumber).bitString;
                    vpnReplaced = primeTable->replStructure->handlePageFault(vAddr, frameNumber,
                                                                          intervalCounter < bitstring, addressProc,firstInterval);
                    if (vpnReplaced == NULL) {
                        primeTable->insertMapForVpn2Pfn(primeTable, vAddr, frameNumber);
                    }
                    frameToLog = frameNumber;
                    frameNumber++;
                    bitsUsed += total_bits;
                    hitMiss = false;
                } else {
                    frameToLog = foundMap->frame;
                    primeTable->replStructure->accessPage(vAddr, foundMap->frame, intervalCounter < bitstring,
                                                          addressProc);
                    hitMiss = true;
                }

                if (intervalCounter >= bitstring) {
                    intervalCounter = 0;
                    firstInterval = false;
                }
                if (foundMap != NULL) {
                    hits++;
                }
                callToInsert++;
                addressProc++;
                intervalCounter++;
                if (log_mode == "offset") {
                    unsigned int masked = vAddr & (primeTable->offsetMask);
                    print_num_inHex(masked);
                }
                if (log_mode == "vpns_pfn") {
                    unsigned int *byLevel = (unsigned int *) malloc((primeTable->numLevels) * sizeof(unsigned int));
                    for (int currLevel = 0; currLevel < (primeTable->numLevels); currLevel++) {
                        unsigned int masked = vAddr & (primeTable->bitMaskAry)[currLevel];
                        int shifted;
                        shifted = masked >> (primeTable->shiftAr)[currLevel];
                        byLevel[currLevel] = shifted;
                    }
                    log_vpns_pfn(primeTable->numLevels, byLevel, frameToLog);
                }
                if (log_mode == "va2pa") {
                    unsigned shiftedFrame = frameToLog << primeTable->offsetBits;
                    unsigned int maskedOffset = vAddr & (primeTable->offsetMask);
                    log_va2pa(vAddr, shiftedFrame + maskedOffset);
                }
                if (log_mode == "vpn2pfn_pr") {
                    if(vpnReplaced == NULL){
                        log_mapping(vAddr, frameToLog, -1, -1, hitMiss);
                    }
                    else {
                        log_mapping(vAddr, frameToLog, vpnReplaced->VPN, vpnReplaced->BitString, hitMiss);
                    }
                }

            }

        }
        vpnReplaced->VPN = -1;
    }





        else
        {
            int frameToLog;
            int currAccess = 0;
            while (currAccess < num_accesses) {
                if (NextAddress(pfile, &mtrace)) //tracef_h - file handle from fopen
                {
                    intervalCounter++;
                    vAddr = mtrace.addr;
                    printf("CURRENT VPN THAT IS BEING PROCESSED: %08X\n",vAddr);
                    Map *foundMap = primeTable->searchMappedPfn(primeTable, vAddr);
                    if (foundMap == NULL) {
                        nullsfound++;
                        vpnReplaced = primeTable->replStructure->handlePageFault(vAddr, frameNumber,
                                                                                 intervalCounter == bitstring , addressProc,firstInterval);
                        if (vpnReplaced == NULL) {
                            primeTable->insertMapForVpn2Pfn(primeTable, vAddr, frameNumber);
                        }
                        frameToLog = frameNumber;
                        frameNumber++;
                        bitsUsed += total_bits;
                        hitMiss = false;


                    } else {
                        frameToLog = foundMap->frame;
                        primeTable->replStructure->accessPage(vAddr, foundMap->frame, intervalCounter == bitstring,
                                                              addressProc);
                        hitMiss = true;
                        vpnReplaced = NULL;
                    }
                    //printf("%d This is a boolean \n",intervalCounter >= bitstring);
                    if (foundMap != NULL) {
                        hits++;
                    }
                    callToInsert++;
                }
                addressProc++;
                currAccess++;
                if (log_mode == "offset") {
                    unsigned int masked = vAddr & (primeTable->offsetMask);
                    print_num_inHex(masked);
                }
                if (log_mode == "vpns_pfn") {
                    unsigned int *byLevel = (unsigned int *) malloc((primeTable->numLevels) * sizeof(unsigned int));
                    for (int currLevel = 0; currLevel < (primeTable->numLevels); currLevel++) {
                        unsigned int masked = vAddr & (primeTable->bitMaskAry)[currLevel];
                        int shifted;
                        shifted = masked >> (primeTable->shiftAr)[currLevel];
                        byLevel[currLevel] = shifted;
                    }
                    log_vpns_pfn(primeTable->numLevels, byLevel, frameToLog);
                }
                if (log_mode == "va2pa") {
                    unsigned shiftedFrame = frameToLog << primeTable->offsetBits;
                    unsigned int maskedOffset = vAddr & (primeTable->offsetMask);
                    log_va2pa(vAddr, shiftedFrame + maskedOffset);
                }
                if (log_mode == "vpn2pfn_pr") {
                    if(vpnReplaced==NULL){
                        log_mapping(vAddr, frameToLog, -1, -1, hitMiss);
                    }
                    else {
                        log_mapping(vAddr, frameToLog, vpnReplaced->VPN, vpnReplaced->BitString, hitMiss);
                    }
                }
            printf("THIS IS INTERVAL COUNTER %d \n", intervalCounter);
                if (intervalCounter == bitstring) {
                    intervalCounter = 0;
                    firstInterval = false;
                }

            }
        }
        // Perform actions based on logging mode
        if (log_mode == "va2pa") {
            return 0;
        } else if (log_mode == "vpn2pfn_pr") {
            return 0;
        } else if (log_mode == "vpns_pfn") {
            return 0;
        } else if (log_mode == "offset") {
            std::cout << "print_num_inHex" << std::endl;
            return 0;
        } else if (log_mode == "bitmasks") {
            log_bitmasks(primeTable->numLevels, primeTable->bitMaskAry);
            return 0;
        } else {
            log_summary((primeTable->PageSize) / 4, 0, hits, addressProc, frameNumber,
                        (primeTable->allocatedMemory) / 4);
        }

        return 0;
    }