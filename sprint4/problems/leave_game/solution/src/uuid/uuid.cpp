#include "uuid.h"

namespace uuid
{
    // constexpr UUIDType ZeroUUID{{0}};
    UUIDType NewUUID()
    {
        return boost::uuids::random_generator()();
    }

    std::string UUIDToString(const UUIDType &uuid)
    {
        return to_string(uuid);
    }

    UUIDType UUIDFromString(std::string_view str)
    {
        boost::uuids::string_generator gen;
        return gen(str.begin(), str.end());
    }

    std::string NewStringUUDD()
    {
        return UUIDToString(NewUUID());
    }

}
