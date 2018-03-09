#include <vector>
#include <iostream>
#include "ip.h"


int main(void)
{

    std::vector<ip::ipv4> ipv4s = { { { 192, 168, 0, 101 }, 25 },
                                    { { 192, 168, 0, 102 }, 19 },
                                    { { 192, 168, 0, 103 }, 18 },
                                    { { 192, 168, 0, 104 }, 11 },
                                    { { 192, 168, 0, 105 }, 8 } };

    std::vector<ip::ipv6> ipv6s = { { { 0x2001, 0xdb8, 0xface }, 64 },
                                    { { 0x2001, 0xdb9, 0xface }, 64 },
                                    { { 0x2001, 0xdba, 0xface }, 64 },
                                    { { 0x2001, 0xdbb, 0xface }, 64 } };

    for(const auto& ipv4 : ipv4s) {
        std::cout << "IP: " << ipv4 << "\n";
        std::cout << "Network Address: " << network(ipv4) << "\n";
        std::cout << "Broadcast Address: " << broadcast(ipv4) << "\n";
        std::cout << "Subnet mask: " << netmask(ipv4) << "\n\n";
    }
    for(const auto& ipv6 : ipv6s) {
        ip::print(std::cout, ipv6);
        std::cout << "\n";
    }
    return 0;
}
