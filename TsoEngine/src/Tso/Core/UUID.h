//
//  UUID.hpp
//  TsoEngine
//
//  Created by SuchanTso on 2024/2/2.
//

#ifndef UUID_hpp
#define UUID_hpp

#pragma once

namespace Tso {

    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };

}

namespace std {
    template <typename T> struct thash;

    template<>
    struct thash<Tso::UUID>
    {
        std::size_t operator()(const Tso::UUID& uuid) const
        {
            return (uint64_t)uuid;
        }
    };

}

#endif /* UUID_hpp */
