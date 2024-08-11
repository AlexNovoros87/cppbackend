#pragma once
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <string>
#include <string_view>
using UUIDType = boost::uuids::uuid;

namespace uuid
{
    // constexpr UUIDType ZeroUUID{{0}};
    UUIDType UUIDFromString(std::string_view str);
    std::string NewStringUUDD();
}
