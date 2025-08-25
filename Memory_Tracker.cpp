//
// Created by andres on 24/8/25.
//

#include "Memory_Tracker.h"

#include "library.h"

#include <iostream>
#include <string>
#include<chrono>
#include<unordered_map>
#include<cstddef>

struct  Info_ubi {
    void* address;
    size_t tamano;
    std ::string archivo;
    int line;
    long long timestamp;



}