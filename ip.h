#pragma once
#include <iostream>
#include <limits>
#include <type_traits>
#include <utility>
#include <array>
#include <iomanip>
#include <string>
#include <climits>

namespace ip {
    namespace version {
        /*! ipv4 definition:
         *  
         *  size: The size in octets
         *  data_t: Type of the container for the data
         */
        struct v4 {
            enum { size = 4 }; // number of octects

            using value_t = uint8_t;
            using data_t = std::array<value_t, size>;

            /*! format function
             *  
             *  returns an ipv4 string => "127.0.0.1"
             */
            template<typename IpType>
            static std::string format(const IpType& ip) {
                std::ostringstream result;

                for(int i = 0; i < ip.data.size() - 1; ++i) {
                    result << static_cast<int>(ip.data[i]) << ".";
                }
                result << static_cast<int>(*ip.data.rbegin()) << "/" << ip.bitmask;

                return result.str();
            }
        };
        /*! ipv6 definition:
         *  
         *  size: The size in hextets
         *  data_t: Type of the container for the data
         */
        struct v6 {

            enum { size = 8 };

            using value_t = uint16_t;
            using data_t = std::array<value_t, size>;

            /*! format function
             *  
             *  returns an ipv6 string => "2001:db8:face::"
             */
            template<typename IpType>
            static std::string format(const IpType& ip) {
                std::ostringstream result;

                result << std::hex
                       << ip.data[0] << ":";

                /*! Attempt at compression, not really good */
                for(int i = 1; i < ip.data.size()-1; ++i) {
                    if(ip.data[i] == 0 && ip.data[i+1] == 0) {
                        result << "::";
                        ++i; //skip the next hextet
                    }
                    else if(ip.data[i] == 0) {
                        result << ":";
                    }
                    else {
                        result << std::hex
                               << ip.data[i] << ":";
                    }
                }

                if(auto hextet = *ip.data.rbegin(); hextet != 0) {
                    result << std::hex
                           << hextet;
                }

                result << "/" << ip.bitmask;



                return result.str();
            }
        };
    }

    /*! Check the IpType is either ipv4 or ipv6 */
    template<typename IpType, bool = std::disjunction<
                                        std::is_same<IpType, version::v4>,
                                        std::is_same<IpType, version::v6>>::value>
    struct ip : IpType {
        ip(typename IpType::data_t dat = {0}, int bitmsk = 0)
            : data(dat),
              bitmask(bitmsk){}
        typename IpType::data_t data;
        int bitmask;
    };

    /*! especialization for when T is not ipv4 or ipv6
     *  ideally this would print an error message at
     *  compile time.
     */
    template<typename IpType>
    struct ip<IpType, false>
    {
    };

    template<typename IpType>
    std::ostream& print(std::ostream& ostr, IpType ip) {
        ostr << IpType::format(ip);
    }

    /*! Calculate the netmask */
    template<typename IpType>
    IpType netmask(IpType ip) {
        using ip_limit = std::numeric_limits<
                            typename std::remove_reference<decltype(std::declval<IpType>().data[0])>::type>;

        /* This is probably overly complicated
         * Initialize all bits to 1 
         */
        IpType netmask = { { ip_limit::max(),
                             ip_limit::max(),
                             ip_limit::max(),
                             ip_limit::max() }, ip.bitmask };

        constexpr auto IP_SIZE_BITS = IpType::size * sizeof(netmask.data[0]) * CHAR_BIT;
        /* Sizes: ipv4 => 4, ipv6 => 8 */
        auto remaining_bits = IP_SIZE_BITS - ip.bitmask;
        auto i = IpType::size - 1;
        while(remaining_bits > 0 && i >= 0) {
            auto bits_to_shift = std::min(remaining_bits, sizeof(netmask.data[i]) * CHAR_BIT);
            netmask.data[i] = ip_limit::max() << bits_to_shift;
            remaining_bits -= bits_to_shift;
            --i;
        }
        return netmask;
    }
    
    /* Operator overloads for bit operations */
    template<typename IpType>
    IpType operator~(const IpType& ip) {
        IpType res_ip = { {0}, ip.bitmask };

        for(int i = 0; i < ip.data.size(); ++i) {
            res_ip.data[i] = ~(ip.data[i]);
        }

        return res_ip;
    }

    template<typename IpType>
    IpType operator&(const IpType& lhs, const IpType& rhs) {
        IpType res_ip = { {0}, lhs.bitmask };
        for(int i = 0; i < lhs.data.size(); ++i) {
            res_ip.data[i] = lhs.data[i] & rhs.data[i];
        }
        return res_ip;
    }

    template<typename IpType>
    IpType operator|(const IpType& lhs, const IpType& rhs) {
        IpType res_ip = { {0}, lhs.bitmask };
        for(int i = 0; i < lhs.data.size(); ++i) {
            res_ip.data[i] = lhs.data[i] | rhs.data[i];
        }
        return res_ip;
    }

    template<typename IpType>
    IpType operator^(const IpType& lhs, const IpType& rhs) {
        IpType res_ip = { {0}, lhs.bitmask };
        for(int i = 0; i < lhs.data.size(); ++i) {
            res_ip.data[i] = lhs.data[i] ^ rhs.data[i];
        }
        return res_ip;
    }

    /* Using aliases for convience:
     * ip::ipv4 my_ip = { {192, 168, 0, 1}, 24 };
     */

    using ipv4 = ip<version::v4>;
    using ipv6 = ip<version::v6>;

    std::ostream& operator<<(std::ostream& ostr, const ipv4& ip) {
        return ostr << ipv4::format(ip);
    }
    std::ostream& operator<<(std::ostream& ostr, const ipv6& ip) {
        return ostr << ipv6::format(ip);
    }

    ipv4 broadcast(const ipv4& ip) {
        return  ip | ~netmask(ip);
    }
    ipv4 network(const ipv4& ip) {
        return ip & netmask(ip);
    }
}
