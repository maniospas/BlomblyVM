// Memory.cpp
#include "BMemory.h"
#include "common.h"
#include "Future.h"

extern VariableManager variableManager;

BMemory::BMemory(const std::shared_ptr<BMemory>& par) : parent(par), allowMutables(true) {
    if(parent) {
        mapPool = parent->mapPool;
        if(!mapPool->empty()) {
            data = mapPool->top();
            mapPool->pop();
        }
        else 
            data = new tsl::hopscotch_map<int, Data*>();
    }
    else {
        mapPool = new std::stack<tsl::hopscotch_map<int, Data*>*>();
        data = new tsl::hopscotch_map<int, Data*>();
    }
    if (pthread_mutex_init(&memoryLock, nullptr) != 0) {
        std::cerr << "Failed to create a mutex for memory read/write" << std::endl;
        exit(1);
    }
}

// Destructor to ensure that all threads are finished
BMemory::~BMemory() {
    // automatically performed in the delete of locals/data
    for (Future* thread : attached_threads) {
        thread->getResult();
    }
    for(const auto& element : *data)
        if(element.second)
            delete element.second;
    data->clear();
    // transfer remainder map pool elements to the parent
    if(parent) {
        // do this afterwards so that recalling the same methods preallocates things correctly
        parent->mapPool->push(data);
    }
    else {
        while(!mapPool->empty()) {
            delete mapPool->top();
            mapPool->pop();
        }
        delete mapPool;
        delete data;
    }
    pthread_mutex_destroy(&memoryLock);
}

// Lock the memory for thread-safe access
void BMemory::lock() {
    if (parent) {
        parent->lock();
    }
    pthread_mutex_lock(&memoryLock);
}

// Unlock the memory
void BMemory::unlock() {
    pthread_mutex_unlock(&memoryLock);
    if (parent) {
        parent->unlock();
    }
}

// Get a data item with mutability check
Data* BMemory::get(int item) {
    Data* ret = (*data)[item];

    // Handle future values
    if (ret && ret->getType() == FUTURE) {
        Data* prevRet = ret;
        ret = ((Future*)ret)->getResult();
        ret->isMutable = prevRet->isMutable;
        (*data)[item] = ret;
        delete prevRet;
        return ret;
    }

    // If not found locally, check parent memory
    if (!ret && parent) 
        ret = parent->get(item, allowMutables);

    // Missing value error
    if (!ret) {
        std::cerr << "Missing value: " +variableManager.getSymbol(item) << std::endl;
        exit(1);
    }

    return ret;
}

// Get a data item, optionally allowing mutable values
Data* BMemory::get(int item, bool allowMutable) {
    Data* ret = (*data)[item];

    // Handle future values
    if (ret && ret->getType() == FUTURE) {
        Data* prevRet = ret;
        ret = ((Future*)ret)->getResult();
        ret->isMutable = prevRet->isMutable;
        (*data)[item] = ret;
        delete prevRet;
        return ret;
    }

    if(ret) {
        // Handle mutability restrictions
        if (!allowMutable && ret->isMutable) {
            std::cerr << "Mutable symbol cannot be accessed from a nested block: " + variableManager.getSymbol(item) << std::endl;
            exit(1);
            return nullptr;
        }
    }
    else {
        // If not found locally, check parent memory
        if (parent) 
            ret = parent->get(item, allowMutables);
    }

    // Missing value error
    if (!ret) {
        std::cerr << "Missing value: " + variableManager.getSymbol(item) << std::endl;
        exit(1);
    }

    return ret;
}


Data* BMemory::getOrNullShallow(int item) {
    auto it = data->find(item);
    if(it==data->end())
        return nullptr;
    return it->second;
   // Data* ret = (*data)[item];
    /*if (ret && ret->getType() == FUTURE) {
        Data* prevRet = ret;
        ret = ((Future*)ret)->getResult();
        ret->isMutable = prevRet->isMutable;
        (*data)[item] = ret;
        delete prevRet;
        return ret;
    }*/
    //return ret;
}

// Get a data item or return nullptr if not found
Data* BMemory::getOrNull(int item, bool allowMutable) {
    Data* ret = (*data)[item];

    // Handle future values
    if (ret && ret->getType() == FUTURE) {
        Data* prevRet = ret;
        ret = ((Future*)ret)->getResult();
        ret->isMutable = prevRet->isMutable;
        (*data)[item] = ret;
        delete prevRet;
        return ret;
    }

    // Handle mutability restrictions
    if (ret && !allowMutable && ret->isMutable) {
        std::cerr << "Mutable symbol cannot be accessed from a nested block: " + item << std::endl;
        exit(1);
        return nullptr;
    }

    // If not found locally, check parent memory
    if (!ret && parent) {
        ret = parent->getOrNull(item, allowMutables);
    }

    return ret;
}


// Set a data item, ensuring mutability rules are followed
void BMemory::set(int item, Data*value) {
    Data* prev = (*data)[item];
    if(prev==value) 
        return;
    if (prev) {
        if(prev->isMutable) 
            delete prev;
        else {
            std::cerr << "Cannot overwrite final value: " + variableManager.getSymbol(item) << std::endl;
            exit(1);
            return;
        }
    }
    (*data)[item] = value;
}

// Pull data from another Memory object
void BMemory::pull(const std::shared_ptr<BMemory>& other) {
    for (auto& it : *other->data) {
        set(it.first, it.second->shallowCopy());
    }
}

// Replace missing values with those from another Memory object
void BMemory::replaceMissing(const std::shared_ptr<BMemory>& other) {
    for (auto& it : *other->data) 
        if (data->find(it.first)==data->end()) 
            (*data)[it.first] = it.second;
}

// Detach this memory from its parent
void BMemory::detach() {
    if(!parent) {
        while(!mapPool->empty()) {
            delete mapPool->top();
            mapPool->pop();
        }
        delete mapPool;
        mapPool = new std::stack<tsl::hopscotch_map<int, Data*>*>();
    }
    allowMutables = false;
    parent = nullptr;
}

// Detach this memory from its parent but retain reference
void BMemory::detach(const std::shared_ptr<BMemory>& par) {
    if(!parent) {
        while(!mapPool->empty()) {
            delete mapPool->top();
            mapPool->pop();
        }
        delete mapPool;
    }
    if(par)
        mapPool = par->mapPool;
    else
        mapPool = new std::stack<tsl::hopscotch_map<int, Data*>*>();
    allowMutables = false;
    parent = par;
}
