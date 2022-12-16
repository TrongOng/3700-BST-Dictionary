#ifndef _BST_DICTIONARY_H
#define _BST_DICTIONARY_H

#include <cstdint> // for uint32_t
#include <stdexcept> // for domain_error

static const uint32_t
    NULL_INDEX = 0xffffffff,
    DEFAULT_INITIAL_CAPACITY = 16;

template <typename KeyType,typename ValueType>
class BSTDictionary {
public:
    explicit BSTDictionary(uint32_t cap = DEFAULT_INITIAL_CAPACITY) {
        //if nTrees = 0 then
        if (nTrees == 0) {
            //allocate space for the siz arrays
            counts = new uint32_t[cap];
            heights = new uint32_t[cap];
            left = new uint32_t[cap];
            right = new uint32_t[cap];
            keys = new KeyType[cap];
            values = new ValueType[cap]; 

            //capacity <- cap
            capacity = cap;

            //generate the free list
            //for i <- start to end - 1 do
            for (int i = 0; i < capacity - 1; i++) {
                left[i] = i + 1;
            }
            //left[end] <- NULL_INDEX end = capacity
            left[capacity-1] = NULL_INDEX;
            //freeListHead <- start (0)
            freeListHead = 0;

            //end if
        }
        nTrees = nTrees + 1;
        root = NULL_INDEX;

    }

    ~BSTDictionary() {
        //nTrees <- nTrees-1
        nTrees = nTrees - 1;
        //if nTrees = 0 then
        if (nTrees == 0) {
            //deelete all shared arrays
            delete[] counts;
            delete[] heights;
            delete[] left;
            delete[] right;
            delete[] keys;
            delete[] values;
        }
        else {
            //prvclear(root)
            prvClear(root);
        } 
    }

    void clear() {

        prvClear(root);
        root = NULL_INDEX;

    }
    uint32_t size() {

        //no tree? no nodes
        if (root == NULL_INDEX) {
            return 0;
        }

        //get node count from root
        return counts[root];
    }

    uint32_t height() {

        //no tree? height is 0? by default
        if (root == NULL_INDEX) {
            return 0;
        }

        //get height from root
        return heights[root];

    }

    bool isEmpty() {

        return root == NULL_INDEX;

    }

    ValueType &search(const KeyType &k) {

        //n <- root
        uint32_t n = root;
        while (n != NULL_INDEX) {
            //if k = keys[n] then
            if (k == keys[n]) {
                return values[n];
            }
            //else if k < keys[n] then
            else if (k < keys[n]) {
                //n <- left[n]
                n = left[n];
            }
            else {
                //n <- right[n]
                n = right[n];
            }
        }
        //throw domain_error
        throw std::domain_error("Search: Key Not Found");

    }

    ValueType &operator[](const KeyType &k) {
        
        //tmp <- prvAllocate()
        uint32_t tmp = prvAllocate();
        //n <- tmp
        uint32_t n = tmp;
        //root <- prvInsert(root, n, k)
        root = prvInsert(root, n, k);

        //if n != tmp then
        if (n != tmp) {
            //prvFree(tmp)
            prvFree(tmp);
        }

        return values[n];


    }

    void remove(const KeyType &k) {

        uint32_t ntbd = NULL_INDEX;
        //recursively detach the key node form the tree
        root = prvRemove(root, ntbd, k);

        prvFree(ntbd);

    }

private:
    uint32_t prvAllocate() {

        //if freeListHead = NULL_INDEX then
        if(freeListHead == NULL_INDEX) {

            //allocate temporary arrays with 2 * capacity elements
            auto tmpCounts = new uint32_t[2*capacity];
            auto tmpHeights = new uint32_t[2*capacity];
            auto tmpLeft = new uint32_t[2*capacity];
            auto tmpRight = new uint32_t[2*capacity];
            auto tmpKeys = new KeyType[2*capacity];
            auto tmpValues = new ValueType[2*capacity];

            //copy data from old arrays to temporary arrays
            for (int i = 0; i < capacity; i++) {

            tmpCounts[i] = counts[i];
            tmpHeights[i] = heights[i];
            tmpLeft[i] = left[i];
            tmpRight[i] = right[i];
            tmpKeys[i] = keys[i];
            tmpValues[i] = values[i];

            }

            //delete old arrays
            delete[] counts;
            delete[] heights;
            delete[] left;
            delete[] right;
            delete[] keys;
            delete[] values;
            
            //points shared pointers to temporary arrays
            counts = tmpCounts;
            heights = tmpHeights;
            left = tmpLeft;
            right = tmpRight;
            keys = tmpKeys;
            values = tmpValues;
            //regenerate the free list
            for (int i = capacity; i < 2*capacity-1; i++) {
                left[i] = i + 1;
            }
            left[2*capacity-1] = NULL_INDEX;

            freeListHead = capacity;

            //capacity <- 2*capacity
            capacity = 2*capacity;

            //end if
        }
        uint32_t tmp = freeListHead;
        freeListHead = left[freeListHead];

        left[tmp] = NULL_INDEX;
        right[tmp] = NULL_INDEX;
        counts[tmp] = 1;
        heights[tmp] = 1;

        //return tmp
        return tmp;


    }

    void prvFree(uint32_t n) {
        left[n] = freeListHead;

        freeListHead = n;
    }

    void prvClear(uint32_t r) {

        //if r is null, nothing to do
        if (r != NULL_INDEX) {
            prvClear(left[r]); //delete the subtrees first
            prvClear(right[r]);
            prvFree(r); //then delete r
        }

    }

    void prvAdjust(uint32_t r) {

        counts[r] = getCount(left[r]) + getCount(right[r]) + 1;

        heights[r] = max(getHeight(left[r]), getHeight(right[r])) + 1;


    }

    uint32_t prvInsert(uint32_t r,uint32_t &n,const KeyType &k) {

        if (r == NULL_INDEX) {
            keys[n] = k;

            return n;
        }
        else if (k == keys[r]) {
            n = r;
        }
        else if (k < keys[r]) {
            left[r] = prvInsert(left[r], n, k);
        }
        else {
            right[r] = prvInsert(right[r], n, k);
        }

        prvAdjust(r);

        return r;

    }

    uint32_t prvRemove(uint32_t r,uint32_t &ntbd,const KeyType &k) {

        if (r == NULL_INDEX) {
            throw std::domain_error("Search: Key Not Found");
        }

        if (k < keys[r]) {
            left[r] = prvRemove(left[r], ntbd, k);
        }
        else if (k > keys[r]) {
            right[r] = prvRemove(right[r], ntbd, k);
        }
        else {
            ntbd = r;

            if (left[r] == NULL_INDEX) {
                if (right[r] == NULL_INDEX) {
                    r = NULL_INDEX;
                }
                else {
                    r = right[r];
                }
            }
            else {
                if (right[r] == NULL_INDEX) {
                    r = left[r];
                }
                else {
                    //reduce two-child case to one-child case
                    if (getHeight(right[r] > getHeight(left[r]))) {
                        uint32_t tmp = right[r];

                        while (left[tmp] != NULL_INDEX) {
                            tmp = left[tmp];
                        }
                        //swap keys[r] and keys[tmp]
                        auto a = keys[r];
                        keys[r] = keys[tmp];
                        keys[tmp] = a;

                        //swap values[r] and values[tmp]
                        auto b = values[r];
                        values[r] = values[tmp];
                        values[tmp] = b;

                        //right[r] <- prvRemove(right[r], ntbd, k)
                        right[r] = prvRemove(right[r], ntbd, k);
                    }
                    else {
                        uint32_t tmp = left[r];

                        while (right[tmp] != NULL_INDEX) {
                            tmp = right[tmp];
                        }
                        //swap keys[r] and keys[tmp]
                        auto a = keys[r];
                        keys[r] = keys[tmp];
                        keys[tmp] = a;

                        //swap values[r] and values[tmp]
                        auto b = values[r];
                        values[r] = values[tmp];
                        values[tmp] = b;

                        //left[r] <- prvRemove(left[r], ntbd, k)
                        left[r] = prvRemove(left[r], ntbd, k);

                    }
                }
            }

        }
        if (r != NULL_INDEX) {
            prvAdjust(r);
        }

        return r;

    }

    uint32_t root; // tree root
    //get Height
    uint32_t getHeight(uint32_t n) {

        //no tree? height is 0? by default
        if (n == NULL_INDEX) {
            return 0;
        }

        //get height from root
        return heights[n];

    }
    //get Count
    uint32_t getCount(uint32_t n) {

        //no tree? height is 0? by default
        if (n == NULL_INDEX) {
            return 0;
        }

        //get height from root
        return counts[n];

    }
    //getMax
    uint32_t max(uint32_t l, uint32_t r) {
        if (l > r) {
            return l;
        }
        else {
            return r;
        }

    }



    static uint32_t // this is the shared data
        *counts, // counts for each node
        *heights, // heights for each node
        *left, // left node indexes
        *right, // right node indexes
        nTrees, // number of BSTs with this key and value type
        capacity, // size of the arrays
        freeListHead; // the head of the unused node list (the free list)

    static KeyType
        *keys; // pool of keys
    static ValueType
        *values; // pool of values
};

template <typename KeyType,typename ValueType>
uint32_t *BSTDictionary<KeyType,ValueType>::counts = nullptr;

template <typename KeyType,typename ValueType>
uint32_t *BSTDictionary<KeyType,ValueType>::heights = nullptr;

template <typename KeyType,typename ValueType>
uint32_t *BSTDictionary<KeyType,ValueType>::left = nullptr;

template <typename KeyType,typename ValueType>
uint32_t *BSTDictionary<KeyType,ValueType>::right = nullptr;

template <typename KeyType,typename ValueType>
uint32_t BSTDictionary<KeyType,ValueType>::nTrees = 0;

template <typename KeyType,typename ValueType>
uint32_t BSTDictionary<KeyType,ValueType>::capacity = 0;

template <typename KeyType,typename ValueType>
uint32_t BSTDictionary<KeyType,ValueType>::freeListHead = 0;

template <typename KeyType,typename ValueType>
KeyType *BSTDictionary<KeyType,ValueType>::keys = nullptr;

template <typename KeyType,typename ValueType>
ValueType *BSTDictionary<KeyType,ValueType>::values = nullptr;

#endif