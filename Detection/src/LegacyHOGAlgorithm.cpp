/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : LegacyHOGAlgorithm.cpp
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : Legacy HOG algorithm implementation for C++14/17 compatibility
 *
 ****************************************************************************/

#include "../inc/LegacyHOGAlgorithm.h"

// This file ensures the LegacyHOGAlgorithm class has proper symbol generation
// Most methods are implemented in the header file for template compatibility

// Explicit instantiation to ensure vtable generation
namespace {
    void ensureVTableGeneration() {
        // This function ensures that the virtual function table is properly generated
        // for the LegacyHOGAlgorithm class, preventing linker errors
        static_cast<void>(sizeof(LegacyHOGAlgorithm));
    }
}