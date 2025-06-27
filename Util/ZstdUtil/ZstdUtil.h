//
// Created by zpf on 25-6-20.
//

#ifndef ZSTDUTIL_H
#define ZSTDUTIL_H
#include <string>
#include <zstd.h>



class ZstdUtil
{
public:
    static void CompressFile(const std::string& inputPath,
                             int                compressionLevel = 3,
                             bool               deleteSource     = true);
};



#endif   // ZSTDUTIL_H
