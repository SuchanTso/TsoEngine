//
//  UUID.cpp
//  TsoEngine
//
//  Created by SuchanTso on 2024/2/2.
//
#include "TPch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Tso {

    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

    UUID::UUID()
        : m_UUID(s_UniformDistribution(s_Engine))
    {
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {
    }

}
