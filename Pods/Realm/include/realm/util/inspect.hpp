/*************************************************************************
 *
 * REALM CONFIDENTIAL
 * __________________
 *
 *  [2011] - [2015] Realm Inc
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Realm Incorporated and its suppliers,
 * if any.  The intellectual and technical concepts contained
 * herein are proprietary to Realm Incorporated
 * and its suppliers and may be covered by U.S. and Foreign Patents,
 * patents in process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Realm Incorporated.
 *
 **************************************************************************/

#ifndef REALM_UTIL_INSPECT_HPP
#define REALM_UTIL_INSPECT_HPP

namespace realm {
namespace util {

template<class OS, class T>
void inspect_value(OS& os, const T& value)
{
    os << value;
}

template<class OS>
void inspect_value(OS& os, const std::string& value)
{
    // FIXME: Escape the string.
    os << "\"" << value << "\"";
}

template<class OS>
void inspect_value(OS& os, const char* value)
{
    // FIXME: Escape the string.
    os << "\"" << value << "\"";
}

template<class OS>
void inspect_all(OS&)
{
    // No-op
}

/// Convert all arguments to strings, and quote string arguments.
template<class OS, class First, class... Args>
void inspect_all(OS& os, First&& first, Args&&... args)
{
    inspect_value(os, std::forward<First>(first));
    if (sizeof...(Args) != 0) {
        os << ", ";
    }
    inspect_all(os, std::forward<Args>(args)...);
}

} // namespace util
} // namespace realm

#endif // REALM_UTIL_INSPECT_HPP
