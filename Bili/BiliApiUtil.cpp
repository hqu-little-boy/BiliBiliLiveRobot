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

std::vector<uint8_t> BiliApiUtil::MakePack(const std::string_view& body, Operation eOperation)
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
    return headerBuffer;
}

std::list<std::string> BiliApiUtil::Unpack(const std::vector<uint8_t>& buffer)
{
    uint32_t               start     = 0;
    size_t                 bufferLen = buffer.size();
    std::list<std::string> res;
    // BiliApiUtil::HeaderTuple header{BiliApiUtil::UnpackHeader(buffer, 0)};
    // // 获取包体
    // std::string body(buffer.begin() + sizeof(HeaderTuple), buffer.begin() + header.totalSize);
    // start = header.totalSize;
    // res.emplace_back(header, std::move(body));
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
            // // std::string body(buffer.begin() + start + sizeof(HeaderTuple),
            // //                  buffer.begin() + start + header.totalSize);
            // std::string body =
            //     BiliApiUtil::UnpackBody(buffer,
            //                             start + sizeof(HeaderTuple),
            //                             start + header.totalSize,
            //                             static_cast<BiliApiUtil::ZipOperation>(header.version));
            // start += header.totalSize;
            // res.emplace_back(header, std::move(body));
            // LOG_VAR(LogLevel::DEBUG, bufferLen);
            // LOG_VAR(LogLevel::DEBUG, start + sizeof(HeaderTuple));
            // LOG_VAR(LogLevel::DEBUG, start + header.totalSize);
            switch (static_cast<BiliApiUtil::ZipOperation>(header.version))
            {
            case ZipOperation::NormalNone:
            case ZipOperation::HeartBeatNone:
            {
                // std::string body =
                //     BiliApiUtil::UnpackBody(buffer,
                //                             start + sizeof(HeaderTuple),
                //                             start + header.totalSize,
                //                             static_cast<BiliApiUtil::ZipOperation>(header.version));
                std::string body = std::string(buffer.begin() + start + sizeof(HeaderTuple),
                                               buffer.begin() + start + header.totalSize);
                res.emplace_back(std::move(body));
                break;
            }
            case ZipOperation::NormalZlib:
            {
                // LOG_MESSAGE(LogLevel::DEBUG, "ZipOperation::NormalZlib");
                std::stringstream compressedStream(
                    std::string(buffer.begin() + start + sizeof(HeaderTuple),
                                buffer.begin() + start + header.totalSize));
                std::stringstream decompressedStream;

                boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
                in.push(boost::iostreams::zlib_decompressor());
                in.push(compressedStream);
                boost::iostreams::copy(in, decompressedStream);
                std::string            decompressedStreamStr = decompressedStream.str();
                std::vector<uint8_t>   decompressedBuffer(decompressedStreamStr.begin(),
                                                        decompressedStreamStr.end());
                std::list<std::string> decompressedBufferUnpack =
                    BiliApiUtil::Unpack(decompressedBuffer);
                res.insert(
                    res.end(), decompressedBufferUnpack.begin(), decompressedBufferUnpack.end());
                break;
            }
            case ZipOperation::NormalBrotli:
            {
                LOG_MESSAGE(LogLevel::DEBUG, "ZipOperation::NormalBrotli");
                BrotliDecoderState* pBrotliState =
                    BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

                if (!pBrotliState)
                {
                    LOG_MESSAGE(LogLevel::ERROR, "Brotli解压缩失败");
                    break;
                }
                std::vector<uint8_t> compressedData =
                    std::vector<uint8_t>(buffer.begin() + start + sizeof(HeaderTuple),
                                         buffer.begin() + start + header.totalSize);
                std::vector<uint8_t> decompressedData;

                size_t         availableInput = compressedData.size();
                const uint8_t* nextInput      = compressedData.data();
                size_t         availableOut;
                uint8_t*       nextOutput;

                // Define a maximum output buffer size
                const size_t         outputBufferSize = 256;
                std::vector<uint8_t> outputBuffer(outputBufferSize);
                BrotliDecoderResult  result;

                while (true)
                {
                    availableOut = outputBuffer.size();
                    nextOutput   = outputBuffer.data();
                    result       = BrotliDecoderDecompressStream(pBrotliState,
                                                           &availableInput,
                                                           &nextInput,
                                                           &availableOut,
                                                           &nextOutput,
                                                           nullptr);
                    // decompressedData.(reinterpret_cast<const char *>(buffer.data()), );
                    if (outputBuffer.size() - availableOut > 0)
                    {
                        decompressedData.insert(
                            decompressedData.end(),
                            outputBuffer.begin(),
                            outputBuffer.begin() + (outputBuffer.size() - availableOut));
                    }
                    if (result == BROTLI_DECODER_RESULT_ERROR ||
                        result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT ||
                        availableInput == 0 && result == BROTLI_DECODER_RESULT_SUCCESS)
                    {
                        break;
                    }
                }
                if (result != BROTLI_DECODER_RESULT_SUCCESS) {
                    LOG_MESSAGE(LogLevel::ERROR, "Brotli解压缩失败");
                }

                std::list<std::string> decompressedBufferUnpack =
                    BiliApiUtil::Unpack(decompressedData);
                for (auto& str : decompressedBufferUnpack)
                {
                    LOG_MESSAGE(LogLevel::DEBUG, str);
                    // if (str[15] == 'R' && str[16] == 'A' && str[17] == 'N' && str[18] == 'K' &&
                    //     str[19] == '_' && str[20] == 'V' && str[21] == '2')
                    // {
                    //     continue;
                    // }
                    res.emplace_back(std::move(str));
                }
                // res.insert(
                //     res.end(), decompressedBufferUnpack.begin(), decompressedBufferUnpack.end());
                BrotliDecoderDestroyInstance(pBrotliState);
                break;
            }
            }
            start += header.totalSize;
        }
    }
    return res;
}

BiliApiUtil::HeaderTuple BiliApiUtil::UnpackHeader(const std::vector<uint8_t>& buffer,
                                                   unsigned                    front)
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
