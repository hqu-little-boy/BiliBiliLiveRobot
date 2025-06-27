//
// Created by zpf on 25-6-20.
//

#include "ZstdUtil.h"

#include "../../Entity/Global/Logger.h"

#include <filesystem>
#include <fstream>
#include <vector>
void ZstdUtil::CompressFile(const std::string& inputPath, int compressionLevel, bool deleteSource)
{
    // 读取原文件内容
    std::ifstream input(inputPath, std::ios::binary);
    if (!input)
    {
        // std::cerr << "Failed to open input file: " << inputPath << std::endl;
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to open input file: {}", inputPath));
        return;
    }

    input.seekg(0, std::ios::end);
    size_t inputSize = input.tellg();
    input.seekg(0, std::ios::beg);

    std::vector<char> inputBuffer(inputSize);
    input.read(inputBuffer.data(), inputSize);
    input.close();

    // 计算最大压缩大小
    size_t            maxCompressedSize = ZSTD_compressBound(inputSize);
    std::vector<char> compressedBuffer(maxCompressedSize);

    // 执行压缩
    size_t compressedSize = ZSTD_compress(compressedBuffer.data(),
                                          maxCompressedSize,
                                          inputBuffer.data(),
                                          inputSize,
                                          compressionLevel);

    if (ZSTD_isError(compressedSize))
    {
        // std::cerr << "Compression error: " << ZSTD_getErrorName(compressedSize) << std::endl;
        LOG_MESSAGE(LogLevel::Error,
                    fmt::format("Compression error: {}", ZSTD_getErrorName(compressedSize)));
        return;
    }

    // 写入压缩后的数据
    auto          outputPath = fmt::format("{}.zst", inputPath);
    std::ofstream output(outputPath, std::ios::binary);
    if (!output)
    {
        // std::cerr << "Failed to open output file: " << outputPath << std::endl;
        LOG_MESSAGE(LogLevel::Error, fmt::format("Failed to open output file: {}", outputPath));
        return;
    }

    output.write(compressedBuffer.data(), compressedSize);
    output.close();

    // std::cout << "Compressed " << inputSize << " bytes to " << compressedSize << " bytes"
    //           << std::endl;
    LOG_MESSAGE(LogLevel::Info,
                fmt::format("Compressed {} bytes to {} bytes", inputSize, compressedSize));
    if (deleteSource)
    {
        // 删除原文件
        if (!std::filesystem::remove(inputPath))
        {
            // std::cerr << "Failed to delete source file: " << inputPath << std::endl;
            LOG_MESSAGE(LogLevel::Error,
                        fmt::format("Failed to delete source file: {}", inputPath));
        }
        else
        {
            // std::cout << "Deleted source file: " << inputPath << std::endl;
            LOG_MESSAGE(LogLevel::Info, fmt::format("Deleted source file: {}", inputPath));
        }
    }
}