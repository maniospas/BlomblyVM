// iterwwatwor.cpp
#include "Iterator.h"
#include "Integer.h"
#include "Data.h"
#include "Boolean.h"
#include "common.h"
#include <iostream>

// ListContents constructor and methods
IteratorContents::IteratorContents(const std::shared_ptr<Data>& object_):object(object_) {
    if (pthread_mutex_init(&memoryLock, nullptr) != 0) {
        std::cerr << "Failed to create a mutex for list read/write" << std::endl;
        exit(1);
    }
    BuiltinArgs args;
    args.arg0 = object_.get();
    args.size = 1;
    pos = std::make_shared<Integer>(0);
    size = ((Integer*)Data::run(LEN, &args).get())->getValue();
    locked = -1; // start from -1 so that first object getting this goes to 0
}

void IteratorContents::lock() {
    if(locked)
        pthread_mutex_lock(&memoryLock);
}

void IteratorContents::unlock() {
    if(locked)
        pthread_mutex_unlock(&memoryLock);
}

void IteratorContents::unsafeUnlock() {
    pthread_mutex_unlock(&memoryLock);
}


// List constructors

Iterator::Iterator(const std::shared_ptr<IteratorContents>& object) : contents(object) {
    contents->lock();
    bool shouldUnlock = contents->locked;
    contents->locked += 1;
    if(shouldUnlock)
        contents->unsafeUnlock();
}

Iterator::~Iterator() {
    contents->lock();
    bool shouldUnlock = contents->locked;
    contents->locked -= 1;
    if(shouldUnlock)
        contents->unsafeUnlock();
}

// Return the type ID
int Iterator::getType() const {
    return ITERATOR;
}

// Convert to string representation
std::string Iterator::toString() const {
    return "iterator";
}

// Create a shallow copy of this List
std::shared_ptr<Data> Iterator::shallowCopy() const {
    return std::make_shared<Iterator>(contents);
}

// Implement the specified operation
std::shared_ptr<Data> Iterator::implement(const OperationType operation, BuiltinArgs* args) {
    if(args->size==1 && operation==NEXT) {
        contents->lock();
        int pos = contents->pos->value;
        if(pos>=contents->size) {
            contents->unlock();
            return nullptr;
        }
        // keep the return type as-is
        args->arg0 = contents->object.get();
        args->arg1 = contents->pos.get();
        args->size = 2;
        contents->unlock();

        std::shared_ptr<Data> ret = Data::run(AT, args); // run outside locks to prevent deadlocks

        contents->lock();
        bool shouldUnlock = contents->locked;
        contents->pos->value += 1; // can get away with not passing a new integer to Data::run because overloaded operators are run in the same thread
        if(shouldUnlock)
            contents->unsafeUnlock();
        return ret;
    }
    if(args->size==1 && operation==LEN)
        return std::make_shared<Integer>(contents->size);
    if(args->size==1 && operation==TOCOPY)
        return std::make_shared<Iterator>(contents);
    if(args->size==1 && operation==TOITER) 
        return std::make_shared<Iterator>(std::make_shared<IteratorContents>(contents->object));

    throw Unimplemented();
}
