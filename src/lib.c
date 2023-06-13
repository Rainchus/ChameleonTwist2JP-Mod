#include "../include/ct2.h"
#include "lz4.h"

void decompress_lz4_ct_default(int srcSize, int savestateCompressedSize, u8* compressBuffer) {
    // Decompress
    const int decompressedSize = LZ4_decompress_safe(
        (const char*) compressBuffer,
        (char*) decompressBuffer,
        savestateCompressedSize,
        srcSize
    );
    if (decompressedSize <= 0) {
        return;
    }

    // Verify
    if (decompressedSize != srcSize) {
        return;
    }
}

s32 compress_lz4_ct_default(const u8* srcData, int srcSize, u8* bufferAddr) { //returns compressed size
    // Compress
    s32 savestateCompressedSize = LZ4_compress_default((const char*) srcData, (char*) bufferAddr, srcSize, 1024 * 1024);

    if (savestateCompressedSize <= 0) {
        return 0;
    }

    return savestateCompressedSize;
}

// Custom implementation of the memmove function
void* my_memmove(void* dest, const void* src, int n) {
  // Cast the destination and source pointers to char pointers
  char* d = (char*) dest;
  const char* s = (const char*) src;

  // If the source and destination pointers are the same, we don't need to do anything
  if (d == s) return dest;

  // If the source and destination pointers overlap, we need to copy the data in reverse order
  // to avoid overwriting data that hasn't been copied yet
  if (d > s && d < s + n) {
    for (size_t i = n; i > 0; i--) {
      d[i - 1] = s[i - 1];
    }
  }
  // Otherwise, we can just copy the data in the normal order
  else {
    for (size_t i = 0; i < n; i++) {
      d[i] = s[i];
    }
  }

  // Return the destination pointer
  return dest;
}

void* my_memset(void* s, int c, int n) {
  // Cast the input pointer to a char pointer
  char* p = (char*) s;

  // Set each byte to the given value
  for (size_t i = 0; i < n; i++) {
    p[i] = c;
  }

  // Return the input pointer
  return s;
}

void* my_memcpy(void* dst, const void* src, int n) {
  // Convert the void pointers to character pointers so that we can
  // perform arithmetic on them. This is allowed because characters
  // are the smallest addressable unit in C.
  char* dst_char = (char*) dst;
  const char* src_char = (const char*) src;

  // Copy the data one byte at a time until we have copied the
  // specified number of bytes.
  for (size_t i = 0; i < n; i++) {
    dst_char[i] = src_char[i];
  }

  // Return a pointer to the destination memory location.
    return dst;
}