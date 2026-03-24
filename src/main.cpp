#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// 组 NXM 头（与抓包一致）：
// word0: V(2) P(1) M(1) CSRC(4) PT(8) SSRC(16)
// counts: 网络序
// reserve: 主机序（代码库里就是直接赋值）
static std::vector<uint8_t> BuildNxmPacket(const std::string &jsonBody, uint8_t pt, uint16_t ssrc)
{
    const uint32_t v = 0;
    const uint32_t p = 0;
    const uint32_t m = 1;
    const uint32_t csrc = 0;

    uint32_t word0 = 0;
    word0 |= (v & 0x3U);
    word0 |= ((p & 0x1U) << 2);
    word0 |= ((m & 0x1U) << 3);
    word0 |= ((csrc & 0xFU) << 4);
    word0 |= ((static_cast<uint32_t>(pt) & 0xFFU) << 8);
    word0 |= ((static_cast<uint32_t>(ssrc) & 0xFFFFU) << 16);

    const uint32_t countsNet = htonl(static_cast<uint32_t>(jsonBody.size()));
    const uint32_t reserve = static_cast<uint32_t>('R'); // 0x00000052

    std::vector<uint8_t> packet;
    packet.resize(12 + jsonBody.size());

    std::memcpy(packet.data() + 0, &word0, sizeof(word0));
    std::memcpy(packet.data() + 4, &countsNet, sizeof(countsNet));
    std::memcpy(packet.data() + 8, &reserve, sizeof(reserve));
    std::memcpy(packet.data() + 12, jsonBody.data(), jsonBody.size());

    return packet;
}

static std::string BuildConnectJson(const std::string &session)
{
    std::ostringstream oss;
    // 这里按你抓包风格给了常见字段；服务端通常至少要 MODULE/OPERATION/SESSION
    oss
        << "{\"MODULE\":\"CERTIFICATE\","
        << "\"OPERATION\":\"CONNECT\","
        << "\"PARAMETER\":{"
        << "\"AUTOCAR\":\"\","
        << "\"AUTONO\":\"0\","
        << "\"CARNUM\":\"X5NPRO_468\","
        << "\"CHANNEL\":12,"
        << "\"CID\":0,"
        << "\"CNAME\":\"CUSTOM_STREAMAX\","
        << "\"CPN\":\"STREAMAX\","
        << "\"DEVCLASS\":4,"
        << "\"DEVNAME\":\"X5NPRO\","
        << "\"DEVTYPE\":1,"
        << "\"DSNO\":\"00BE000468\","
        << "\"EID\":\"\","
        << "\"ENCRYPTTYPE\":0,"
        << "\"ETLS\":0,"
        << "\"EV\":\"V2.0\","
        << "\"FSV\":1,"
        << "\"ICCID\":\"\","
        << "\"LINENO\":\"\","
        << "\"MTYPE\":200,"
        << "\"NET\":0,"
        << "\"PRO\":\"1.0.6\","
        << "\"PV\":1,"
        << "\"SC\":1,"
        << "\"STYPE\":205,"
        << "\"TSE\":1,"
        << "\"UNAME\":\"\","
        << "\"UNO\":\"\""
        << "},"
        << "\"SESSION\":\"" << session << "\""
        << "}\n"; // 末尾 0x0A
    return oss.str();
}

static bool SendAll(int fd, const uint8_t *data, size_t len)
{
    size_t sent = 0;
    while (sent < len)
    {
        const ssize_t n = send(fd, data + sent, len - sent, 0);
        if (n <= 0)
        {
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port> [session]\n";
        return 1;
    }

    const std::string ip = argv[1];
    const int port = std::atoi(argv[2]);
    const std::string session = (argc >= 4) ? argv[3] : "000018AA8ECF9DC0B14676C210D8A96B";

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "socket failed: " << std::strerror(errno) << "\n";
        return 1;
    }

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
    {
        std::cerr << "invalid ip: " << ip << "\n";
        close(fd);
        return 1;
    }

    if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << "connect failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::cout << "[OK] TCP connected to " << ip << ":" << port << "\n";

    const std::string json = BuildConnectJson(session);
    const std::vector<uint8_t> packet = BuildNxmPacket(json, 0, 0); // CONNECT: PT=0, SSRC=0

    if (!SendAll(fd, packet.data(), packet.size()))
    {
        std::cerr << "send failed: " << std::strerror(errno) << "\n";
        close(fd);
        return 1;
    }

    std::cout << "[OK] CONNECT packet sent, bytes=" << packet.size() << "\n";

    // 读一帧回包（演示版）
    uint8_t buf[4096];
    const ssize_t n = recv(fd, buf, sizeof(buf), 0);
    if (n > 0)
    {
        std::cout << "[RECV] " << n << " bytes\n";
        if (n >= 12)
        {
            uint32_t countsNet = 0;
            std::memcpy(&countsNet, buf + 4, 4);
            const uint32_t bodyLen = ntohl(countsNet);
            std::cout << "body_len=" << bodyLen << "\n";

            const size_t headerLen = 12;
            const size_t realBody = (n > static_cast<ssize_t>(headerLen)) ? (n - headerLen) : 0;
            std::string body(reinterpret_cast<char *>(buf + headerLen), realBody);
            std::cout << "body(partial/full):\n" << body << "\n";
        }
    }
    else if (0 == n)
    {
        std::cout << "[INFO] server closed connection\n";
    }
    else
    {
        std::cerr << "recv failed: " << std::strerror(errno) << "\n";
    }

    close(fd);
}