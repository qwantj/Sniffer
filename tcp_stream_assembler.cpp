#include "tcp_stream_assembler.h"
#include <cstring>
#include <algorithm>

TCPStreamAssembler::TCPStreamAssembler(CompleteMessageCallback callback)
    : messageCallback(callback) {
}

void TCPStreamAssembler::processPacket(uint32_t srcIP, uint32_t dstIP, uint16_t srcPort, uint16_t dstPort,
                                       uint32_t seqNum, const uint8_t* data, size_t length) {
    if (length == 0) return;

    // Создаем ключ для потока
    StreamKey key{srcIP, dstIP, srcPort, dstPort};

    // Обновляем время последней активности
    lastActivity[key] = time(nullptr);

    // Получаем или создаем поток
    StreamData& stream = streams[key];

    // Если это первый пакет в потоке, устанавливаем начальный seq
    if (stream.buffer.empty() && stream.assembledData.empty()) {
        stream.expectedSeq = seqNum;
    }

    // Добавляем данные в буфер
    std::vector<uint8_t> packetData(data, data + length);
    stream.buffer[seqNum] = packetData;

    // Пытаемся собрать последовательные пакеты
    checkForCompletedMessages(key, stream);
}

void TCPStreamAssembler::checkForCompletedMessages(const StreamKey& key, StreamData& stream) {
    bool dataAdded = false;

    // Проверяем, есть ли пакеты, которые можно добавить к собранным данным
    auto it = stream.buffer.begin();
    while (it != stream.buffer.end()) {
        if (it->first == stream.expectedSeq) {
            // Добавляем данные
            stream.assembledData.insert(stream.assembledData.end(), it->second.begin(), it->second.end());
            stream.expectedSeq += it->second.size();
            it = stream.buffer.erase(it);
            dataAdded = true;
        } else if (it->first < stream.expectedSeq) {
            // Устаревшие или дублирующиеся данные, удаляем
            it = stream.buffer.erase(it);
        } else {
            // Еще не время для этого пакета
            ++it;
        }
    }

    // Если добавили данные, проверяем, есть ли полные HTTP-сообщения
    if (dataAdded) {
        while (isHTTPMessage(stream.assembledData)) {
            size_t messageLength = getHTTPMessageLength(stream.assembledData);
            if (messageLength == 0 || messageLength > stream.assembledData.size()) {
                break;
            }

            // Извлекаем полное HTTP-сообщение
            std::vector<uint8_t> message(stream.assembledData.begin(), stream.assembledData.begin() + messageLength);

            // Вызываем обратный вызов с полным сообщением
            messageCallback(key, message);

            // Удаляем обработанное сообщение из буфера
            stream.assembledData.erase(stream.assembledData.begin(), stream.assembledData.begin() + messageLength);
        }
    }
}

bool TCPStreamAssembler::isHTTPMessage(const std::vector<uint8_t>& data) {
    if (data.size() < 10) return false;

    // Проверка на HTTP-запрос
    static const char* methods[] = {"GET ", "POST ", "PUT ", "HEAD ", "DELETE ", "OPTIONS "};
    for (const char* method : methods) {
        if (memcmp(data.data(), method, strlen(method)) == 0) {
            return true;
        }
    }

    // Проверка на HTTP-ответ
    if (memcmp(data.data(), "HTTP/", 5) == 0) {
        return true;
    }

    return false;
}

size_t TCPStreamAssembler::getHTTPMessageLength(const std::vector<uint8_t>& data) {
    // Ищем конец заголовков
    for (size_t i = 0; i < data.size() - 3; i++) {
        if (data[i] == '\r' && data[i+1] == '\n' && data[i+2] == '\r' && data[i+3] == '\n') {
            size_t headersEnd = i + 4;

            // Проверяем, есть ли заголовок Content-Length
            std::string headers(reinterpret_cast<const char*>(data.data()), headersEnd);
            size_t contentLengthPos = headers.find("Content-Length:");

            if (contentLengthPos != std::string::npos) {
                size_t valueStart = headers.find_first_not_of(" ", contentLengthPos + 15);
                size_t valueEnd = headers.find("\r\n", valueStart);
                if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                    std::string lengthStr = headers.substr(valueStart, valueEnd - valueStart);
                    size_t contentLength = std::stoul(lengthStr);
                    return headersEnd + contentLength;
                }
            }

            // Если нет Content-Length, возвращаем только длину заголовков
            // (это работает для простых запросов, но не для чанковых ответов)
            return headersEnd;
        }
    }

    return 0; // Еще недостаточно данных
}

void TCPStreamAssembler::clearOldStreams(time_t olderThan) {
    time_t now = time(nullptr);

    auto it = lastActivity.begin();
    while (it != lastActivity.end()) {
        if (now - it->second > olderThan) {
            streams.erase(it->first);
            it = lastActivity.erase(it);
        } else {
            ++it;
        }
    }
}
