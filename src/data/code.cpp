// Code.cpp
#include "data/Code.h"
#include "common.h"
#include "BMemory.h"

// Constructor to initialize Code object with program segment details
Code::Code(void* programAt, int startAt, int endAt, const std::shared_ptr<BMemory>& declMemory)
    : program(programAt), start(startAt), end(endAt), declarationMemory(declMemory) {
    }
    
Code::~Code() {
}

// Return the type ID
int Code::getType() const {
    return CODE;
}

// Convert to string representation
std::string Code::toString() const {
    return "code from " + std::to_string(start) + " to " + std::to_string(end);
}

// Get the starting position of the code segment
int Code::getStart() const {
    return start;
}

// Get the ending position of the code segment
int Code::getEnd() const {
    return end;
}

// Get the associated program pointer
void* Code::getProgram() const {
    return program;
}

// Get the memory declarations associated with this code
std::shared_ptr<BMemory> Code::getDeclarationMemory() const {
    return declarationMemory;
}

// Create a shallow copy of this Code object
Data* Code::shallowCopy() const {
    return new Code(program, start, end, declarationMemory);
}

// Implement the specified operation for the Code class
Data* Code::implement(const OperationType operation, BuiltinArgs* args)  {
    if (args->size == 1) 
        return new Code(program, start, end, declarationMemory);
    
    // Unimplemented operation
    throw Unimplemented();
}
