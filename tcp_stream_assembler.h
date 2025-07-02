#ifndef TCP_STREAM_ASSEMBLER_H
#define TCP_STREAM_ASSEMBLER_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <ctime>

struct StreamKey {
    uint32_t srcIP;
    uint32_t dstIP;
    uint16_t srcPort;
    uint16_t dstPort;

    bool operator<(const StreamKey& other) const {
        if (srcIP != other.srcIP) return srcIP < other.srcIP;
        if (dstIP != other.dstIP) return dstIP < other.dstIP;
        if (srcPort != other.srcPort) return srcPort < other.srcPort;
        return dstPort < other.dstPort;
    }
};

struct StreamData {
    uint32_t expectedSeq;
    std::map<uint32_t, std::vector<uint8_t>> buffer;
    std::vector<uint8_t> assembledData;
};

class TCPStreamAssembler {
public:
    typedef std::function<void(const StreamKey&, const std::vector<uint8_t>&)> CompleteMessageCallback;

    TCPStreamAssembler(CompleteMessageCallback callback);

    void processPacket(uint32_t srcIP, uint32_t dstIP, uint16_t srcPort, uint16_t dstPort,
                       uint32_t seqNum, const uint8_t* data, size_t length);

    void clearOldStreams(time_t olderThan);

private:
    std::map<StreamKey, StreamData> streams;
    std::map<StreamKey, time_t> lastActivity;
    CompleteMessageCallback messageCallback;

    void checkForCompletedMessages(const StreamKey& key, StreamData& stream);
    bool isHTTPMessage(const std::vector<uint8_t>& data);
    size_t getHTTPMessageLength(const std::vector<uint8_t>& data);
};

#endif // TCP_STREAM_ASSEMBLER_H
