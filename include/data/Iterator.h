// data/Iterator.h
#ifndef ITERATOR_H
#define ITERATOR_H

#include <memory>
#include <vector>
#include "data/Data.h"
#include "data/Integer.h"

// Thread-safe container for list contents
class IteratorContents {
private:
    pthread_mutex_t memoryLock;

public:
    int size;
    Data* object;
    Integer* pos;
    int locked;
    
    IteratorContents(Data* object);
    ~IteratorContents();
    void lock();
    void unlock();
    void unsafeUnlock();
};

// List class representing a list of data items
class Iterator : public Data {
private:
    std::shared_ptr<IteratorContents> contents;

public:
    explicit Iterator(const std::shared_ptr<IteratorContents>& contents_);
    ~Iterator();

    int getType() const override;
    std::string toString() const override;
    Data* shallowCopy() const override;
    Data* implement(const OperationType operation, BuiltinArgs* args) override;
};

#endif // ITERATOR_H
