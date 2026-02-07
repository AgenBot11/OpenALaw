/**
 * Memory Alignment Checker for ARM Architecture
 * 
 * This module performs runtime checks to detect and prevent SIGBUS errors
 * caused by unaligned memory access on ARM processors.
 * 
 * Designed specifically for Android Bionic libc environment.
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <memory>

#ifdef __arm__  // ARM-specific code
#include <unistd.h>
#endif

class MemoryAlignmentChecker {
public:
    /**
     * Check if a pointer is aligned to the specified boundary
     * @param ptr Pointer to check
     * @param alignment Alignment boundary (e.g., 4 for 4-byte alignment)
     * @return true if aligned, false otherwise
     */
    static bool isAligned(const void* ptr, size_t alignment) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        return (addr % alignment) == 0;
    }

    /**
     * Safely read a value of type T from potentially unaligned memory
     * @param src Source memory location (may be unaligned)
     * @return Value of type T read safely
     */
    template<typename T>
    static T safeRead(const void* src) {
        T value;
        memcpy(&value, src, sizeof(T));
        return value;
    }

    /**
     * Safely write a value of type T to potentially unaligned memory
     * @param dst Destination memory location (may be unaligned)
     * @param value Value to write
     */
    template<typename T>
    static void safeWrite(void* dst, const T& value) {
        memcpy(dst, &value, sizeof(T));
    }

    /**
     * Allocate aligned memory
     * @param size Size of memory to allocate
     * @param alignment Alignment boundary
     * @return Aligned memory pointer
     */
    static void* alignedAlloc(size_t size, size_t alignment) {
#ifdef _POSIX_VERSION
        void* ptr = nullptr;
        int result = posix_memalign(&ptr, alignment, size);
        if (result != 0) {
            throw std::runtime_error("posix_memalign failed");
        }
        return ptr;
#else
        // Fallback for systems without posix_memalign
        size_t totalSize = size + alignment - 1 + sizeof(void*);
        void* rawPtr = malloc(totalSize);
        if (!rawPtr) {
            throw std::runtime_error("malloc failed");
        }

        // Align pointer
        uintptr_t addr = reinterpret_cast<uintptr_t>(rawPtr);
        addr = (addr + sizeof(void*) + alignment - 1) & ~(alignment - 1);

        // Store original pointer just before aligned address
        void** alignedPtr = reinterpret_cast<void**>(addr);
        *(alignedPtr - 1) = rawPtr;

        return alignedPtr;
#endif
    }

    /**
     * Free aligned memory allocated with alignedAlloc
     * @param ptr Pointer to aligned memory
     */
    static void alignedFree(void* ptr) {
        if (!ptr) return;

#ifdef _POSIX_VERSION
        free(ptr);
#else
        // Retrieve original pointer
        void** alignedPtr = static_cast<void**>(ptr);
        void* rawPtr = *(alignedPtr - 1);
        free(rawPtr);
#endif
    }

    /**
     * Validate memory access patterns that could cause SIGBUS on ARM
     * @param data Memory region to validate
     * @param size Size of memory region
     * @param alignment Expected alignment
     * @return Vector of potential misalignment issues
     */
    static std::vector<size_t> validateMemoryAccess(const void* data, size_t size, size_t alignment) {
        std::vector<size_t> misalignedOffsets;
        const uint8_t* byteData = static_cast<const uint8_t*>(data);

        // Check alignment for common data types
        for (size_t i = 0; i <= size - sizeof(uint16_t); ++i) {
            if (i % sizeof(uint16_t) != 0 && !isAligned(byteData + i, sizeof(uint16_t))) {
                // Potential misalignment for 16-bit access
                if (sizeof(uint16_t) > 1) { // Only check if alignment matters
                    misalignedOffsets.push_back(i);
                }
            }
        }

        for (size_t i = 0; i <= size - sizeof(uint32_t); ++i) {
            if (i % sizeof(uint32_t) != 0 && !isAligned(byteData + i, sizeof(uint32_t))) {
                // Potential misalignment for 32-bit access
                if (sizeof(uint32_t) > 1) { // Only check if alignment matters
                    misalignedOffsets.push_back(i);
                }
            }
        }

        for (size_t i = 0; i <= size - sizeof(uint64_t); ++i) {
            if (i % sizeof(uint64_t) != 0 && !isAligned(byteData + i, sizeof(uint64_t))) {
                // Potential misalignment for 64-bit access
                if (sizeof(uint64_t) > 1) { // Only check if alignment matters
                    misalignedOffsets.push_back(i);
                }
            }
        }

        return misalignedOffsets;
    }

    /**
     * Create an aligned buffer with padding to prevent SIGBUS
     * @param originalSize Size of the usable buffer
     * @param alignment Required alignment
     * @return Pair of (aligned buffer pointer, total allocated size)
     */
    static std::pair<void*, size_t> createSafeBuffer(size_t originalSize, size_t alignment = 8) {
        size_t totalSize = originalSize + alignment - 1;
        void* rawBuffer = malloc(totalSize);
        if (!rawBuffer) {
            throw std::runtime_error("Failed to allocate buffer");
        }

        // Align the buffer
        uintptr_t addr = reinterpret_cast<uintptr_t>(rawBuffer);
        addr = (addr + alignment - 1) & ~(alignment - 1);
        void* alignedBuffer = reinterpret_cast<void*>(addr);

        return std::make_pair(alignedBuffer, totalSize);
    }
};

/**
 * Example usage and testing functions
 */
namespace TestSuite {
    void testBasicAlignment() {
        std::cout << "Testing basic alignment functions..." << std::endl;

        alignas(8) uint8_t testData[16];
        void* ptr = testData;

        std::cout << "Data pointer: " << ptr << std::endl;
        std::cout << "Is 4-byte aligned: " << MemoryAlignmentChecker::isAligned(ptr, 4) << std::endl;
        std::cout << "Is 8-byte aligned: " << MemoryAlignmentChecker::isAligned(ptr, 8) << std::endl;

        // Test safe read/write
        uint32_t testValue = 0xDEADBEEF;
        MemoryAlignmentChecker::safeWrite(testData + 1, testValue);  // Intentionally unaligned
        uint32_t readValue = MemoryAlignmentChecker::safeRead<uint32_t>(testData + 1);
        std::cout << "Safe read/write test: " << std::hex << readValue << std::dec << std::endl;
    }

    void testValidation() {
        std::cout << "\nTesting memory validation..." << std::endl;

        uint8_t testBuffer[100];
        auto issues = MemoryAlignmentChecker::validateMemoryAccess(testBuffer, sizeof(testBuffer), 4);
        
        std::cout << "Found " << issues.size() << " potential misalignment issues" << std::endl;
        if (!issues.empty()) {
            std::cout << "First few issue offsets: ";
            for (size_t i = 0; i < std::min(5ul, issues.size()); ++i) {
                std::cout << issues[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    void testSafeBuffer() {
        std::cout << "\nTesting safe buffer creation..." << std::endl;

        auto bufferInfo = MemoryAlignmentChecker::createSafeBuffer(1024, 16);
        std::cout << "Created safe buffer at: " << bufferInfo.first 
                  << ", size: " << bufferInfo.second << std::endl;
        
        // Verify alignment
        bool isAligned = MemoryAlignmentChecker::isAligned(bufferInfo.first, 16);
        std::cout << "Buffer is 16-byte aligned: " << isAligned << std::endl;

        free(reinterpret_cast<uint8_t*>(bufferInfo.first));
    }
}

int main() {
    std::cout << "OpenALaw Memory Alignment Checker for ARM Architecture" << std::endl;
    std::cout << "=====================================================" << std::endl;

    try {
        TestSuite::testBasicAlignment();
        TestSuite::testValidation();
        TestSuite::testSafeBuffer();

        std::cout << "\nAll tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}