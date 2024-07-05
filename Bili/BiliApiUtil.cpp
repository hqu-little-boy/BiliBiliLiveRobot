//
// Created by zeng on 24-6-25.
//

#include "BiliApiUtil.h"

#include "../Base/Logger.h"

#include <boost/endian/conversion.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <brotli/decode.h>
#include <iostream>

// BrotliDecoderState* BiliApiUtil::brotliState =
//     BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
;
std::string BiliApiUtil::HeaderTuple::ToString() const
{
    return std::format(
        "totalSize: {}, headerSize: {}, version: {}, operationCode: {}, sequenceId: {}",
        totalSize,
        headerSize,
        version,
        operationCode,
        sequenceId);
}

bool BiliApiUtil::MakePack(const std::string_view& body,
                           Operation               eOperation,
                           std::vector<uint8_t>&   res)
{
    // LOG_VAR(LogLevel::DEBUG, body.size());
    // 构建包头
    HeaderTuple header{
        .totalSize  = boost::endian::native_to_big(static_cast<uint32_t>(body.size() + 16)),
        .headerSize = boost::endian::native_to_big(static_cast<uint16_t>(16)),
        .version    = boost::endian::native_to_big(static_cast<uint16_t>(1)),
        .operationCode =
            boost::endian::native_to_big(static_cast<uint32_t>(static_cast<int>(eOperation))),
        .sequenceId = boost::endian::native_to_big(static_cast<uint32_t>(1)),
    };
    // 将包头和包体拼接
    std::vector<uint8_t> headerBuffer(reinterpret_cast<uint8_t*>(&header),
                                      reinterpret_cast<uint8_t*>(&header) + sizeof(HeaderTuple));

    std::vector<uint8_t> bodyBuffer(body.begin(), body.end());
    // std::vector<uint8_t> buffer;
    // buffer.insert(buffer.end(), headerBuffer.begin(), headerBuffer.end());
    headerBuffer.insert(headerBuffer.end(), bodyBuffer.begin(), bodyBuffer.end());
    // // buffer转数组
    // char* res(new char[buffer.size()]);
    // std::copy(buffer.begin(), buffer.end(), res);
    // auto boostBuffer = boost::asio::buffer(res, buffer.size());
    // delete res;
    res = std::move(headerBuffer);
    return true;
}

std::list<std::string> BiliApiUtil::Unpack(const std::span<const uint8_t>& buffer)
{
    uint32_t               start     = 0;
    size_t                 bufferLen = buffer.size();
    std::list<std::string> res;
    while (start < bufferLen)
    {
        BiliApiUtil::HeaderTuple header{BiliApiUtil::UnpackHeader(buffer, start)};
        auto operation = static_cast<BiliApiUtil::Operation>(header.operationCode);
        if (operation == Operation::HEARTBEAT_REPLY)
        {
            // start += header.totalSize - 2;
            for (int temp = start; temp < bufferLen - 1; ++temp)
            {
                if (buffer[temp] == '{' && buffer[temp + 1] == '}')
                {
                    start = temp + 2;
                    break;
                }
            }
        }
        else
        {
            switch (static_cast<BiliApiUtil::ZipOperation>(header.version))
            {
            case ZipOperation::NormalNone:
            case ZipOperation::HeartBeatNone:
            {
                res.emplace_back(BiliApiUtil::UnpackBodyNoneCompress(
                    buffer, start + sizeof(HeaderTuple), start + header.totalSize));
                break;
            }
            case ZipOperation::NormalZlib:
            {
                std::list<std::string> decompressedBufferUnpack = BiliApiUtil::UnpackBodyZlib(
                    buffer, start + sizeof(HeaderTuple), start + header.totalSize);
                // 将decompressedBufferUnpack使用std::move插入到res中
                for (auto& str : decompressedBufferUnpack)
                {
                    res.emplace_back(std::move(str));
                }
                break;
            }
            case ZipOperation::NormalBrotli:
            {
                // LOG_MESSAGE(LogLevel::DEBUG, "ZipOperation::NormalBrotli");
                std::list<std::string> decompressedBufferUnpack = BiliApiUtil::UnpackBodyBrotli(
                    buffer, start + sizeof(HeaderTuple), start + header.totalSize);
                for (auto& str : decompressedBufferUnpack)
                {
                    res.emplace_back(std::move(str));
                }
                break;
            }
            }
            start += header.totalSize;
        }
    }
    return res;
}

BiliApiUtil::HeaderTuple BiliApiUtil::UnpackHeader(const std::span<const uint8_t>& buffer,
                                                   unsigned                        front)
{
    BiliApiUtil::HeaderTuple header{};
    std::copy(
        buffer.begin() + front, buffer.begin() + front + 16, reinterpret_cast<uint8_t*>(&header));
    // 将网络字节序转为本地字节序
    header.totalSize     = boost::endian::big_to_native(header.totalSize);
    header.headerSize    = boost::endian::big_to_native(header.headerSize);
    header.version       = boost::endian::big_to_native(header.version);
    header.operationCode = boost::endian::big_to_native(header.operationCode);
    header.sequenceId    = boost::endian::big_to_native(header.sequenceId);
    return header;
}

std::string BiliApiUtil::UnpackBodyNoneCompress(const std::span<const uint8_t>& buffer,
                                                unsigned front, unsigned end)
{
    return std::string(buffer.begin() + front, buffer.begin() + end);
}

std::list<std::string> BiliApiUtil::UnpackBodyZlib(const std::span<const uint8_t>& buffer,
                                                   unsigned front, unsigned end)
{
    std::stringstream compressedStream(std::string(buffer.begin() + front, buffer.begin() + end));
    std::stringstream decompressedStream;

    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressedStream);
    boost::iostreams::copy(in, decompressedStream);
    std::string_view decompressedStreamStringView = decompressedStream.view();
    // decompressedStream将内部数据构造为std::span<uint_8>
    std::span<const uint8_t> decompressedBuffer(
        reinterpret_cast<const uint8_t*>(decompressedStreamStringView.data()),
        decompressedStreamStringView.size());
    return BiliApiUtil::Unpack(decompressedBuffer);
}

std::list<std::string> BiliApiUtil::UnpackBodyBrotli(const std::span<const uint8_t>& buffer,
                                                     unsigned front, unsigned end)
{
    BrotliDecoderState* pBrotliState = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

    if (!pBrotliState)
    {
        LOG_MESSAGE(LogLevel::ERROR, "Brotli解压缩失败");
        return {};
    }
    std::span<const uint8_t> compressedData(buffer.begin() + front, buffer.begin() + end);
    std::vector<uint8_t>     decompressedData;

    size_t         availableInput = compressedData.size();
    const uint8_t* nextInput      = compressedData.data();
    size_t         availableOut;
    uint8_t*       nextOutput;

    // Define a maximum output buffer size
    constexpr size_t     outputBufferSize = 256;
    std::vector<uint8_t> outputBuffer(outputBufferSize);
    BrotliDecoderResult  result;

    while (true)
    {
        availableOut = outputBuffer.size();
        nextOutput   = outputBuffer.data();
        result       = BrotliDecoderDecompressStream(
            pBrotliState, &availableInput, &nextInput, &availableOut, &nextOutput, nullptr);
        // decompressedData.(reinterpret_cast<const char *>(buffer.data()), );
        if (outputBufferSize - availableOut > 0)
        {
            decompressedData.insert(decompressedData.end(),
                                    outputBuffer.begin(),
                                    outputBuffer.begin() + (outputBufferSize - availableOut));
        }
        if (result == BROTLI_DECODER_RESULT_ERROR ||
            result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT ||
            availableInput == 0 && result == BROTLI_DECODER_RESULT_SUCCESS)
        {
            break;
        }
    }
    if (result != BROTLI_DECODER_RESULT_SUCCESS)
    {
        LOG_MESSAGE(LogLevel::ERROR, "Brotli解压缩失败");
        return {};
    }

    BrotliDecoderDestroyInstance(pBrotliState);
    return BiliApiUtil::Unpack(decompressedData);
}

// std::string BiliApiUtil::UnpackBody(const std::vector<uint8_t>& buffer, unsigned front,
//                                     unsigned end, ZipOperation zipOperation)
// {
//     switch (zipOperation)
//     {
//     case ZipOperation::NormalNone:
//     case ZipOperation::HeartBeatNone:
//     {
//         return std::string(buffer.begin() + front, buffer.begin() + end);
//     }
//     case ZipOperation::NormalZlib:
//     {
//         std::stringstream compressedStream(
//             std::string(buffer.begin() + front, buffer.begin() + end));
//         std::stringstream decompressedStream;
//
//         boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
//         in.push(boost::iostreams::zlib_decompressor());
//         in.push(compressedStream);
//         boost::iostreams::copy(in, decompressedStream);
//         std::string decompressedStreamStr = decompressedStream.str();
//         return std::string(decompressedStreamStr.begin() + 16, decompressedStreamStr.end());
//     }
//     case ZipOperation::NormalBrotli:
//     {
//         return std::string(buffer.begin() + front, buffer.begin() + end);
//     }
//     default:
//     {
//         return std::string(buffer.begin() + front, buffer.begin() + end);
//     }
//     }
// }
