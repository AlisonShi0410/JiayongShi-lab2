#include <iostream>
#include "BaseCache.h"
#include <cmath>
using namespace std;

//WRITE ME
//Default constructor to set everything to '0'
BaseCache::BaseCache()
: //numReads(0),
  //numWrites(0), 
  cacheSize(512), 
  associativity(2),
  blockSize(16), 
  cacheLines(nullptr),
  numReadHits(0),
  numReadMisses(0),
  numWriteHits(0),
  numWriteMisses(0)
   {
    
    //calculate
    numSets = cacheSize / (blockSize * associativity);
    indexBits = static_cast<int>(log2(numSets)); 
    tagBits = ADDR_BITS - indexBits - static_cast<int>(log2(blockSize));
    // Dynamically allocate the cache lines 2D array
    cacheLines = new cacheLine*[numSets];
    for (int i = 0; i < numSets; ++i) {
        cacheLines[i] = new cacheLine[associativity];
    }
}

//WRITE ME
//Constructor to initialize cache parameters, create the cache and clears it
BaseCache::BaseCache(uint32_t _cacheSize, uint32_t _associativity, uint32_t _blockSize):
//numReads(0), 
//numWrites(0), 
cacheSize(_cacheSize), 
associativity(_associativity),
blockSize(_blockSize),
cacheLines(nullptr),
numReadHits(0), 
numReadMisses(0),
numWriteHits(0), 
numWriteMisses(0)
 {
    numSets = cacheSize / (blockSize * associativity);
    indexBits = static_cast<int>(log2(numSets));
    tagBits = ADDR_BITS - indexBits - static_cast<int>(log2(blockSize));
    cacheLines = new cacheLine*[numSets];
    for (int i = 0; i < numSets; ++i) {
        cacheLines[i] = new cacheLine[associativity];
    }
}

//WRITE ME
//Set cache base parameters
void BaseCache::setCacheSize(uint32_t _cacheSize) {
    cacheSize = _cacheSize;
}
void BaseCache::setAssociativity(uint32_t _associativity) {
    associativity = _associativity;
}
void BaseCache::setBlockSize(uint32_t _blockSize) {
    blockSize = _blockSize;
}

//WRITE ME
//Get cache base parameters
uint32_t BaseCache::getCacheSize() {
    return cacheSize;
}
uint32_t BaseCache::getAssociativity() {
    return associativity;
}
uint32_t BaseCache::getBlockSize() {
    return blockSize;
}

//WRITE ME
//Get cache access statistics
uint32_t BaseCache::getReadHits() {
    return readHits;
}
uint32_t BaseCache::getReadMisses() {
    return readMisses;
}
uint32_t BaseCache::getWriteHits() {
    return writeHits;
}
uint32_t BaseCache::getWriteMisses() {
    return writeMisses;
}
double BaseCache::getReadHitRate() {
     return static_cast<double>(readHits) / (readHits + readMisses);
}
double BaseCache::getReadMissRate() {
    return static_cast<double>(readMisses) / (readHits + readMisses);
}
double BaseCache::getWriteHitRate() {
    return static_cast<double>(writeHits) / (writeHits + writeMisses);
}
double BaseCache::getWriteMissRate() {
    return static_cast<double>(writeMisses) / (writeHits + writeMisses);
}
double BaseCache::getOverallHitRate() {
    uint32_t totalHits = readHits + writeHits;
    uint32_t totalAccesses = totalHits + readMisses + writeMisses;
    return static_cast<double>(totalHits) / totalAccesses;
}
double BaseCache::getOverallMissRate() {
    uint32_t totalMisses = readMisses + writeMisses;
    uint32_t totalAccesses = totalMisses + readHits + writeHits;
    return static_cast<double>(totalMisses) / totalAccesses;
}

//WRITE ME
//Initialize cache derived parameters
void BaseCache::initDerivedParams() {
    
    unsigned int numLines = numSets * associativity;
    //unsigned int cacheSize = numLines * blockSize;
    
    cacheLines = new cacheLine*[numLines];
    
    for (int i = 0; i < numLines; ++i) {
        cacheLines[i] = new cacheLine;
        cacheLines[i]->tag = 0; 
        cacheLines[i]->lru_counter = 0; 
        memset(cacheLines[i]->data, 0, blockSize);
}
}

//WRITE ME
//Reset cache access statistics
void BaseCache::resetStats() {
    accesses = 0;      
    misses = 0;        
    hit_time = 0;     
    miss_penalty = 0;
}

//WRITE ME
//Create cache and clear it
void BaseCache::createCache() {
     // Clean up existing cache lines if they have been allocated
    if (cacheLines != nullptr) {
        for (int i = 0; i < numSets; ++i) {
            for (int j = 0; j < associativity; ++j) {
                // Clear any other state of the cache line, like dirty bit or reference bit
                cacheLines[i][j].valid = false;
                cacheLines[i][j].tag = 0;
            }
            delete[] cacheLines[i];
        }
        delete[] cacheLines;
    }

    // Recompute derived parameters
    initDerivedParams();

    // Allocate and initialize new cache line array
    cacheLines = new cacheLine*[numSets];
    for (int i = 0; i < numSets; ++i) {
        cacheLines[i] = new cacheLine[associativity];
        for (int j = 0; j < associativity; ++j) {
            // Initialize as invalid
            cacheLines[i][j].valid = false;
            cacheLines[i][j].tag = 0;
        }
    }
}

//WRITE ME
//Reset cache
void BaseCache::clearCache() {
    for (int i = 0; i < numSets; ++i) {
        for (int j = 0; j < associativity; ++j) {
            cacheLines[i][j].valid = false;
            cacheLines[i][j].tag = 0;
        }
    }
}

//WRITE ME
//Read data
//return true if it was a hit, false if it was a miss
//data is only valid if it was a hit, input data pointer
//is not updated upon miss. Make sure to update LRU stack
//bits. You can choose to separate the LRU bits update into
// a separate function that can be used from both read() and write().


uint32_t* BaseCache::fetchDataFromLowerMemory(uint32_t addr) {
    // Simulate fetching data from a lower memory level
    uint32_t* new_data = new uint32_t[1]; // Assume data is an array of one uint32_t for simplicity
    *new_data = 0; // Simulated data
    return new_data;
}



bool BaseCache::read(uint32_t addr, uint32_t *data) {
    // Calculate index and tag based on the address
    uint32_t index = (addr / blockSize) % numSets;
    uint32_t tag = addr / (blockSize * numSets);

    // Track if we have a cache hit
    bool hit = false;

    // Track the least recently used cache line's index and its counter
    int lru_index = 0;
    int lru_counter = INT_MAX;

    // Search for the tag in the cache lines of the set
    for (int i = 0; i < associativity; ++i) {
        if (cacheLines[index][i].valid && cacheLines[index][i].tag == tag) {
            // Cache hit
            hit = true;
            *data = *cacheLines[index][i].data;
            cacheLines[index][i].lru_counter = 0; // Reset the LRU counter for the hit line
            break;
        }
        // Update LRU info if this line is less recently used than the current LRU
        if (cacheLines[index][i].lru_counter > lru_counter) {
            lru_index = i;
            lru_counter = cacheLines[index][i].lru_counter;
        }
    }

    // If it was a miss and we need to replace a line
    if (!hit) {
        // Replace the LRU line
        cacheLines[index][lru_index].valid = true;
        cacheLines[index][lru_index].tag = tag;
        delete[] cacheLines[index][lru_index].data;
        // Simulate fetching data from lower memory level
        // For example, you could call a function here to get the data
        cacheLines[index][lru_index].data = fetchDataFromLowerMemory(addr);
        cacheLines[index][lru_index].lru_counter = 0;
    }

    // Update LRU counters for all other lines in the set
    for (int i = 0; i < associativity; ++i) {
        if (i != lru_index) {
            cacheLines[index][i].lru_counter++;
        }
    }

    // Update statistics
    if (hit) {
        numReadHits++;
    } else {
        numReadMisses++;
    }

    return hit;
}



//WRITE ME
//Write data
//Function returns write hit or miss status. 
bool BaseCache::write(uint32_t addr, uint32_t data) {
    uint32_t index = (addr / blockSize) % numSets;
    uint32_t tag = addr / (blockSize * numSets);
    bool hit = false;
    int lru_index = 0;
    int lru_counter = INT_MAX;

    // Check if we have a hit and update data
    for (int i = 0; i < associativity; ++i) {
        if (cacheLines[index][i].valid && cacheLines[index][i].tag == tag) {
            // Write hit
            hit = true;
            *cacheLines[index][i].data = data;
            cacheLines[index][i].lru_counter = 0; // Reset the LRU counter for the hit line
            break;
        }
        // Update LRU info
        if (cacheLines[index][i].lru_counter > lru_counter) {
            lru_index = i;
            lru_counter = cacheLines[index][i].lru_counter;
        }
    }

    // If it was a miss, we need to replace a line
    if (!hit) {
        // Replace the LRU line
        cacheLines[index][lru_index].valid = true;
        cacheLines[index][lru_index].tag = tag;
        *cacheLines[index][lru_index].data = data;
        cacheLines[index][lru_index].lru_counter = 0;
    }

    // Update LRU counters for all other lines in the set
    for (int i = 0; i < associativity; ++i) {
        if (i != lru_index) {
            cacheLines[index][i].lru_counter++;
        }
    }

    // Update statistics
    if (hit) {
        numWriteHits++;
    } else {
        numWriteMisses++;
    }

    return hit;
}


//WRITE ME
//Destructor to free all allocated memeroy.
BaseCache::~BaseCache() {
    for (int i = 0; i < numSets; ++i) {
        for (int j = 0; j < associativity; ++j) {
            delete cacheLines[i][j].data; // Free memory allocated for data
        }
        delete[] cacheLines[i]; // Free memory allocated for cacheLine pointers
    }
    delete[] cacheLines; // Free the array of pointers
}


