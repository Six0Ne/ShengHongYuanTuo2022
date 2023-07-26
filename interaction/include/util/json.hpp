/*
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
|  |  |__   |  |  | | | |  version 3.9.1
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2013-2019 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#define INCLUDE_NLOHMANN_JSON_HPP_

#define NLOHMANN_JSON_VERSION_MAJOR 3
#define NLOHMANN_JSON_VERSION_MINOR 9
#define NLOHMANN_JSON_VERSION_PATCH 1

#include <algorithm> // all_of, find, for_each
#include <cstddef> // nullptr_t, ptrdiff_t, size_t
#include <functional> // hash, less
#include <initializer_list> // initializer_list
#ifndef JSON_NO_IO
    #include <iosfwd> // istream, ostream
#endif  // JSON_NO_IO
#include <iterator> // random_access_iterator_tag
#include <memory> // unique_ptr
#include <numeric> // accumulate
#include <string> // string, stoi, to_string
#include <utility> // declval, forward, move, pair, swap
#include <vector> // vector

// #include <nlohmann/adl_serializer.hpp>


#include <type_traits>
#include <utility>

// #include <nlohmann/detail/conversions/from_json.hpp>


#include <algorithm> // transform
#include <array> // array
#include <forward_list> // forward_list
#include <iterator> // inserter, front_inserter, end
#include <map> // map
#include <string> // string
#include <tuple> // tuple, make_tuple
#include <type_traits> // is_arithmetic, is_same, is_enum, underlying_type, is_convertible
#include <unordered_map> // unordered_map
#include <utility> // pair, declval
#include <valarray> // valarray

// #include <nlohmann/detail/exceptions.hpp>


#include <exception> // exception
#include <stdexcept> // runtime_error
#include <string> // to_string
#include <vector> // vector

// #include <nlohmann/detail/value_t.hpp>


#include <array> // array
#include <cstddef> // size_t
#include <cstdint> // uint8_t
#include <string> // string

namespace nlohmann
{
namespace detail
{
///////////////////////////
// JSON type enumeration //
///////////////////////////

/*!
@brief the JSON type enumeration

This enumeration collects the different JSON types. It is internally used to
distinguish the stored values, and the functions @ref basic_json::is_null(),
@ref basic_json::is_object(), @ref basic_json::is_array(),
@ref basic_json::is_string(), @ref basic_json::is_boolean(),
@ref basic_json::is_number() (with @ref basic_json::is_number_integer(),
@ref basic_json::is_number_unsigned(), and @ref basic_json::is_number_float()),
@ref basic_json::is_discarded(), @ref basic_json::is_primitive(), and
@ref basic_json::is_structured() rely on it.

@note There are three enumeration entries (number_integer, number_unsigned, and
number_float), because the library distinguishes these three types for numbers:
@ref basic_json::number_unsigned_t is used for unsigned integers,
@ref basic_json::number_integer_t is used for signed integers, and
@ref basic_json::number_float_t is used for floating-point numbers or to
approximate integers which do not fit in the limits of their respective type.

@sa see @ref basic_json::basic_json(const value_t value_type) -- create a JSON
value with the default value for a given type

@since version 1.0.0
*/
enum class value_t : std::uint8_t
{
    null,             ///< null value
    object,           ///< object (unordered set of name/value pairs)
    array,            ///< array (ordered collection of values)
    string,           ///< string value
    boolean,          ///< boolean value
    number_integer,   ///< number value (signed integer)
    number_unsigned,  ///< number value (unsigned integer)
    number_float,     ///< number value (floating-point)
    binary,           ///< binary array (ordered collection of bytes)
    discarded         ///< discarded by the parser callback function
};

/*!
@brief comparison operator for JSON types

Returns an ordering that is similar to Python:
- order: null < boolean < number < object < array < string < binary
- furthermore, each type is not smaller than itself
- discarded values are not comparable
- binary is represented as a b"" string in python and directly comparable to a
  string; however, making a binary array directly comparable with a string would
  be surprising behavior in a JSON file.

@since version 1.0.0
*/
inline bool operator<(const value_t lhs, const value_t rhs) noexcept
{
    static constexpr std::array<std::uint8_t, 9> order = {{
            0 /* null */, 3 /* object */, 4 /* array */, 5 /* string */,
            1 /* boolean */, 2 /* integer */, 2 /* unsigned */, 2 /* float */,
            6 /* binary */
        }
    };

    const auto l_index = static_cast<std::size_t>(lhs);
    const auto r_index = static_cast<std::size_t>(rhs);
    return l_index < order.size() && r_index < order.size() && order[l_index] < order[r_index];
}
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/string_escape.hpp>


#include <string>
// #include <nlohmann/detail/macro_scope.hpp>


#include <utility> // pair
// #include <nlohmann/thirdparty/hedley/hedley.hpp>


/* Hedley - https://nemequ.github.io/hedley
 * Created by Evan Nemerson <evan@nemerson.com>
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to
 * the public domain worldwide. This software is distributed without
 * any warranty.
 *
 * For details, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 * SPDX-License-Identifier: CC0-1.0
 */

#if !defined(JSON_HEDLEY_VERSION) || (JSON_HEDLEY_VERSION < 15)
#if defined(JSON_HEDLEY_VERSION)
    #undef JSON_HEDLEY_VERSION
#endif
#define JSON_HEDLEY_VERSION 15

#if defined(JSON_HEDLEY_STRINGIFY_EX)
    #undef JSON_HEDLEY_STRINGIFY_EX
#endif
#define JSON_HEDLEY_STRINGIFY_EX(x) #x

#if defined(JSON_HEDLEY_STRINGIFY)
    #undef JSON_HEDLEY_STRINGIFY
#endif
#define JSON_HEDLEY_STRINGIFY(x) JSON_HEDLEY_STRINGIFY_EX(x)

#if defined(JSON_HEDLEY_CONCAT_EX)
    #undef JSON_HEDLEY_CONCAT_EX
#endif
#define JSON_HEDLEY_CONCAT_EX(a,b) a##b

#if defined(JSON_HEDLEY_CONCAT)
    #undef JSON_HEDLEY_CONCAT
#endif
#define JSON_HEDLEY_CONCAT(a,b) JSON_HEDLEY_CONCAT_EX(a,b)

#if defined(JSON_HEDLEY_CONCAT3_EX)
    #undef JSON_HEDLEY_CONCAT3_EX
#endif
#define JSON_HEDLEY_CONCAT3_EX(a,b,c) a##b##c

#if defined(JSON_HEDLEY_CONCAT3)
    #undef JSON_HEDLEY_CONCAT3
#endif
#define JSON_HEDLEY_CONCAT3(a,b,c) JSON_HEDLEY_CONCAT3_EX(a,b,c)

#if defined(JSON_HEDLEY_VERSION_ENCODE)
    #undef JSON_HEDLEY_VERSION_ENCODE
#endif
#define JSON_HEDLEY_VERSION_ENCODE(major,minor,revision) (((major) * 1000000) + ((minor) * 1000) + (revision))

#if defined(JSON_HEDLEY_VERSION_DECODE_MAJOR)
    #undef JSON_HEDLEY_VERSION_DECODE_MAJOR
#endif
#define JSON_HEDLEY_VERSION_DECODE_MAJOR(version) ((version) / 1000000)

#if defined(JSON_HEDLEY_VERSION_DECODE_MINOR)
    #undef JSON_HEDLEY_VERSION_DECODE_MINOR
#endif
#define JSON_HEDLEY_VERSION_DECODE_MINOR(version) (((version) % 1000000) / 1000)

#if defined(JSON_HEDLEY_VERSION_DECODE_REVISION)
    #undef JSON_HEDLEY_VERSION_DECODE_REVISION
#endif
#define JSON_HEDLEY_VERSION_DECODE_REVISION(version) ((version) % 1000)

#if defined(JSON_HEDLEY_GNUC_VERSION)
    #undef JSON_HEDLEY_GNUC_VERSION
#endif
#if defined(__GNUC__) && defined(__GNUC_PATCHLEVEL__)
    #define JSON_HEDLEY_GNUC_VERSION JSON_HEDLEY_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__)
    #define JSON_HEDLEY_GNUC_VERSION JSON_HEDLEY_VERSION_ENCODE(__GNUC__, __GNUC_MINOR__, 0)
#endif

#if defined(JSON_HEDLEY_GNUC_VERSION_CHECK)
    #undef JSON_HEDLEY_GNUC_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_GNUC_VERSION)
    #define JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_GNUC_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_MSVC_VERSION)
    #undef JSON_HEDLEY_MSVC_VERSION
#endif
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 140000000) && !defined(__ICL)
    #define JSON_HEDLEY_MSVC_VERSION JSON_HEDLEY_VERSION_ENCODE(_MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, (_MSC_FULL_VER % 100000) / 100)
#elif defined(_MSC_FULL_VER) && !defined(__ICL)
    #define JSON_HEDLEY_MSVC_VERSION JSON_HEDLEY_VERSION_ENCODE(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, (_MSC_FULL_VER % 10000) / 10)
#elif defined(_MSC_VER) && !defined(__ICL)
    #define JSON_HEDLEY_MSVC_VERSION JSON_HEDLEY_VERSION_ENCODE(_MSC_VER / 100, _MSC_VER % 100, 0)
#endif

#if defined(JSON_HEDLEY_MSVC_VERSION_CHECK)
    #undef JSON_HEDLEY_MSVC_VERSION_CHECK
#endif
#if !defined(JSON_HEDLEY_MSVC_VERSION)
    #define JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (0)
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_FULL_VER >= ((major * 10000000) + (minor * 100000) + (patch)))
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
    #define JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_FULL_VER >= ((major * 1000000) + (minor * 10000) + (patch)))
#else
    #define JSON_HEDLEY_MSVC_VERSION_CHECK(major,minor,patch) (_MSC_VER >= ((major * 100) + (minor)))
#endif

#if defined(JSON_HEDLEY_INTEL_VERSION)
    #undef JSON_HEDLEY_INTEL_VERSION
#endif
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && !defined(__ICL)
    #define JSON_HEDLEY_INTEL_VERSION JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE)
#elif defined(__INTEL_COMPILER) && !defined(__ICL)
    #define JSON_HEDLEY_INTEL_VERSION JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0)
#endif

#if defined(JSON_HEDLEY_INTEL_VERSION_CHECK)
    #undef JSON_HEDLEY_INTEL_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_INTEL_VERSION)
    #define JSON_HEDLEY_INTEL_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_INTEL_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_INTEL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_INTEL_CL_VERSION)
    #undef JSON_HEDLEY_INTEL_CL_VERSION
#endif
#if defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE) && defined(__ICL)
    #define JSON_HEDLEY_INTEL_CL_VERSION JSON_HEDLEY_VERSION_ENCODE(__INTEL_COMPILER, __INTEL_COMPILER_UPDATE, 0)
#endif

#if defined(JSON_HEDLEY_INTEL_CL_VERSION_CHECK)
    #undef JSON_HEDLEY_INTEL_CL_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_INTEL_CL_VERSION)
    #define JSON_HEDLEY_INTEL_CL_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_INTEL_CL_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_INTEL_CL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_PGI_VERSION)
    #undef JSON_HEDLEY_PGI_VERSION
#endif
#if defined(__PGI) && defined(__PGIC__) && defined(__PGIC_MINOR__) && defined(__PGIC_PATCHLEVEL__)
    #define JSON_HEDLEY_PGI_VERSION JSON_HEDLEY_VERSION_ENCODE(__PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__)
#endif

#if defined(JSON_HEDLEY_PGI_VERSION_CHECK)
    #undef JSON_HEDLEY_PGI_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_PGI_VERSION)
    #define JSON_HEDLEY_PGI_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_PGI_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_PGI_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_SUNPRO_VERSION)
    #undef JSON_HEDLEY_SUNPRO_VERSION
#endif
#if defined(__SUNPRO_C) && (__SUNPRO_C > 0x1000)
    #define JSON_HEDLEY_SUNPRO_VERSION JSON_HEDLEY_VERSION_ENCODE((((__SUNPRO_C >> 16) & 0xf) * 10) + ((__SUNPRO_C >> 12) & 0xf), (((__SUNPRO_C >> 8) & 0xf) * 10) + ((__SUNPRO_C >> 4) & 0xf), (__SUNPRO_C & 0xf) * 10)
#elif defined(__SUNPRO_C)
    #define JSON_HEDLEY_SUNPRO_VERSION JSON_HEDLEY_VERSION_ENCODE((__SUNPRO_C >> 8) & 0xf, (__SUNPRO_C >> 4) & 0xf, (__SUNPRO_C) & 0xf)
#elif defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x1000)
    #define JSON_HEDLEY_SUNPRO_VERSION JSON_HEDLEY_VERSION_ENCODE((((__SUNPRO_CC >> 16) & 0xf) * 10) + ((__SUNPRO_CC >> 12) & 0xf), (((__SUNPRO_CC >> 8) & 0xf) * 10) + ((__SUNPRO_CC >> 4) & 0xf), (__SUNPRO_CC & 0xf) * 10)
#elif defined(__SUNPRO_CC)
    #define JSON_HEDLEY_SUNPRO_VERSION JSON_HEDLEY_VERSION_ENCODE((__SUNPRO_CC >> 8) & 0xf, (__SUNPRO_CC >> 4) & 0xf, (__SUNPRO_CC) & 0xf)
#endif

#if defined(JSON_HEDLEY_SUNPRO_VERSION_CHECK)
    #undef JSON_HEDLEY_SUNPRO_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_SUNPRO_VERSION)
    #define JSON_HEDLEY_SUNPRO_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_SUNPRO_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_SUNPRO_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_EMSCRIPTEN_VERSION)
    #undef JSON_HEDLEY_EMSCRIPTEN_VERSION
#endif
#if defined(__EMSCRIPTEN__)
    #define JSON_HEDLEY_EMSCRIPTEN_VERSION JSON_HEDLEY_VERSION_ENCODE(__EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__)
#endif

#if defined(JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK)
    #undef JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_EMSCRIPTEN_VERSION)
    #define JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_EMSCRIPTEN_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_EMSCRIPTEN_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_ARM_VERSION)
    #undef JSON_HEDLEY_ARM_VERSION
#endif
#if defined(__CC_ARM) && defined(__ARMCOMPILER_VERSION)
    #define JSON_HEDLEY_ARM_VERSION JSON_HEDLEY_VERSION_ENCODE(__ARMCOMPILER_VERSION / 1000000, (__ARMCOMPILER_VERSION % 1000000) / 10000, (__ARMCOMPILER_VERSION % 10000) / 100)
#elif defined(__CC_ARM) && defined(__ARMCC_VERSION)
    #define JSON_HEDLEY_ARM_VERSION JSON_HEDLEY_VERSION_ENCODE(__ARMCC_VERSION / 1000000, (__ARMCC_VERSION % 1000000) / 10000, (__ARMCC_VERSION % 10000) / 100)
#endif

#if defined(JSON_HEDLEY_ARM_VERSION_CHECK)
    #undef JSON_HEDLEY_ARM_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_ARM_VERSION)
    #define JSON_HEDLEY_ARM_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_ARM_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_ARM_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_IBM_VERSION)
    #undef JSON_HEDLEY_IBM_VERSION
#endif
#if defined(__ibmxl__)
    #define JSON_HEDLEY_IBM_VERSION JSON_HEDLEY_VERSION_ENCODE(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#elif defined(__xlC__) && defined(__xlC_ver__)
    #define JSON_HEDLEY_IBM_VERSION JSON_HEDLEY_VERSION_ENCODE(__xlC__ >> 8, __xlC__ & 0xff, (__xlC_ver__ >> 8) & 0xff)
#elif defined(__xlC__)
    #define JSON_HEDLEY_IBM_VERSION JSON_HEDLEY_VERSION_ENCODE(__xlC__ >> 8, __xlC__ & 0xff, 0)
#endif

#if defined(JSON_HEDLEY_IBM_VERSION_CHECK)
    #undef JSON_HEDLEY_IBM_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_IBM_VERSION)
    #define JSON_HEDLEY_IBM_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_IBM_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_IBM_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_VERSION)
    #undef JSON_HEDLEY_TI_VERSION
#endif
#if \
    defined(__TI_COMPILER_VERSION__) && \
    ( \
      defined(__TMS470__) || defined(__TI_ARM__) || \
      defined(__MSP430__) || \
      defined(__TMS320C2000__) \
    )
#if (__TI_COMPILER_VERSION__ >= 16000000)
    #define JSON_HEDLEY_TI_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif
#endif

#if defined(JSON_HEDLEY_TI_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_VERSION)
    #define JSON_HEDLEY_TI_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_CL2000_VERSION)
    #undef JSON_HEDLEY_TI_CL2000_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__TMS320C2000__)
    #define JSON_HEDLEY_TI_CL2000_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_CL2000_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_CL2000_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_CL2000_VERSION)
    #define JSON_HEDLEY_TI_CL2000_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_CL2000_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_CL2000_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_CL430_VERSION)
    #undef JSON_HEDLEY_TI_CL430_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__MSP430__)
    #define JSON_HEDLEY_TI_CL430_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_CL430_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_CL430_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_CL430_VERSION)
    #define JSON_HEDLEY_TI_CL430_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_CL430_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_CL430_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_ARMCL_VERSION)
    #undef JSON_HEDLEY_TI_ARMCL_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && (defined(__TMS470__) || defined(__TI_ARM__))
    #define JSON_HEDLEY_TI_ARMCL_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_ARMCL_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_ARMCL_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_ARMCL_VERSION)
    #define JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_ARMCL_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_CL6X_VERSION)
    #undef JSON_HEDLEY_TI_CL6X_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__TMS320C6X__)
    #define JSON_HEDLEY_TI_CL6X_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_CL6X_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_CL6X_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_CL6X_VERSION)
    #define JSON_HEDLEY_TI_CL6X_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_CL6X_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_CL6X_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_CL7X_VERSION)
    #undef JSON_HEDLEY_TI_CL7X_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__C7000__)
    #define JSON_HEDLEY_TI_CL7X_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_CL7X_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_CL7X_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_CL7X_VERSION)
    #define JSON_HEDLEY_TI_CL7X_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_CL7X_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_CL7X_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TI_CLPRU_VERSION)
    #undef JSON_HEDLEY_TI_CLPRU_VERSION
#endif
#if defined(__TI_COMPILER_VERSION__) && defined(__PRU__)
    #define JSON_HEDLEY_TI_CLPRU_VERSION JSON_HEDLEY_VERSION_ENCODE(__TI_COMPILER_VERSION__ / 1000000, (__TI_COMPILER_VERSION__ % 1000000) / 1000, (__TI_COMPILER_VERSION__ % 1000))
#endif

#if defined(JSON_HEDLEY_TI_CLPRU_VERSION_CHECK)
    #undef JSON_HEDLEY_TI_CLPRU_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TI_CLPRU_VERSION)
    #define JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TI_CLPRU_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_CRAY_VERSION)
    #undef JSON_HEDLEY_CRAY_VERSION
#endif
#if defined(_CRAYC)
    #if defined(_RELEASE_PATCHLEVEL)
        #define JSON_HEDLEY_CRAY_VERSION JSON_HEDLEY_VERSION_ENCODE(_RELEASE_MAJOR, _RELEASE_MINOR, _RELEASE_PATCHLEVEL)
    #else
        #define JSON_HEDLEY_CRAY_VERSION JSON_HEDLEY_VERSION_ENCODE(_RELEASE_MAJOR, _RELEASE_MINOR, 0)
    #endif
#endif

#if defined(JSON_HEDLEY_CRAY_VERSION_CHECK)
    #undef JSON_HEDLEY_CRAY_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_CRAY_VERSION)
    #define JSON_HEDLEY_CRAY_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_CRAY_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_CRAY_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_IAR_VERSION)
    #undef JSON_HEDLEY_IAR_VERSION
#endif
#if defined(__IAR_SYSTEMS_ICC__)
    #if __VER__ > 1000
        #define JSON_HEDLEY_IAR_VERSION JSON_HEDLEY_VERSION_ENCODE((__VER__ / 1000000), ((__VER__ / 1000) % 1000), (__VER__ % 1000))
    #else
        #define JSON_HEDLEY_IAR_VERSION JSON_HEDLEY_VERSION_ENCODE(__VER__ / 100, __VER__ % 100, 0)
    #endif
#endif

#if defined(JSON_HEDLEY_IAR_VERSION_CHECK)
    #undef JSON_HEDLEY_IAR_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_IAR_VERSION)
    #define JSON_HEDLEY_IAR_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_IAR_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_IAR_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_TINYC_VERSION)
    #undef JSON_HEDLEY_TINYC_VERSION
#endif
#if defined(__TINYC__)
    #define JSON_HEDLEY_TINYC_VERSION JSON_HEDLEY_VERSION_ENCODE(__TINYC__ / 1000, (__TINYC__ / 100) % 10, __TINYC__ % 100)
#endif

#if defined(JSON_HEDLEY_TINYC_VERSION_CHECK)
    #undef JSON_HEDLEY_TINYC_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_TINYC_VERSION)
    #define JSON_HEDLEY_TINYC_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_TINYC_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_TINYC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_DMC_VERSION)
    #undef JSON_HEDLEY_DMC_VERSION
#endif
#if defined(__DMC__)
    #define JSON_HEDLEY_DMC_VERSION JSON_HEDLEY_VERSION_ENCODE(__DMC__ >> 8, (__DMC__ >> 4) & 0xf, __DMC__ & 0xf)
#endif

#if defined(JSON_HEDLEY_DMC_VERSION_CHECK)
    #undef JSON_HEDLEY_DMC_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_DMC_VERSION)
    #define JSON_HEDLEY_DMC_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_DMC_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_DMC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_COMPCERT_VERSION)
    #undef JSON_HEDLEY_COMPCERT_VERSION
#endif
#if defined(__COMPCERT_VERSION__)
    #define JSON_HEDLEY_COMPCERT_VERSION JSON_HEDLEY_VERSION_ENCODE(__COMPCERT_VERSION__ / 10000, (__COMPCERT_VERSION__ / 100) % 100, __COMPCERT_VERSION__ % 100)
#endif

#if defined(JSON_HEDLEY_COMPCERT_VERSION_CHECK)
    #undef JSON_HEDLEY_COMPCERT_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_COMPCERT_VERSION)
    #define JSON_HEDLEY_COMPCERT_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_COMPCERT_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_COMPCERT_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_PELLES_VERSION)
    #undef JSON_HEDLEY_PELLES_VERSION
#endif
#if defined(__POCC__)
    #define JSON_HEDLEY_PELLES_VERSION JSON_HEDLEY_VERSION_ENCODE(__POCC__ / 100, __POCC__ % 100, 0)
#endif

#if defined(JSON_HEDLEY_PELLES_VERSION_CHECK)
    #undef JSON_HEDLEY_PELLES_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_PELLES_VERSION)
    #define JSON_HEDLEY_PELLES_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_PELLES_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_PELLES_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_MCST_LCC_VERSION)
    #undef JSON_HEDLEY_MCST_LCC_VERSION
#endif
#if defined(__LCC__) && defined(__LCC_MINOR__)
    #define JSON_HEDLEY_MCST_LCC_VERSION JSON_HEDLEY_VERSION_ENCODE(__LCC__ / 100, __LCC__ % 100, __LCC_MINOR__)
#endif

#if defined(JSON_HEDLEY_MCST_LCC_VERSION_CHECK)
    #undef JSON_HEDLEY_MCST_LCC_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_MCST_LCC_VERSION)
    #define JSON_HEDLEY_MCST_LCC_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_MCST_LCC_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_MCST_LCC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_GCC_VERSION)
    #undef JSON_HEDLEY_GCC_VERSION
#endif
#if \
    defined(JSON_HEDLEY_GNUC_VERSION) && \
    !defined(__clang__) && \
    !defined(JSON_HEDLEY_INTEL_VERSION) && \
    !defined(JSON_HEDLEY_PGI_VERSION) && \
    !defined(JSON_HEDLEY_ARM_VERSION) && \
    !defined(JSON_HEDLEY_CRAY_VERSION) && \
    !defined(JSON_HEDLEY_TI_VERSION) && \
    !defined(JSON_HEDLEY_TI_ARMCL_VERSION) && \
    !defined(JSON_HEDLEY_TI_CL430_VERSION) && \
    !defined(JSON_HEDLEY_TI_CL2000_VERSION) && \
    !defined(JSON_HEDLEY_TI_CL6X_VERSION) && \
    !defined(JSON_HEDLEY_TI_CL7X_VERSION) && \
    !defined(JSON_HEDLEY_TI_CLPRU_VERSION) && \
    !defined(__COMPCERT__) && \
    !defined(JSON_HEDLEY_MCST_LCC_VERSION)
    #define JSON_HEDLEY_GCC_VERSION JSON_HEDLEY_GNUC_VERSION
#endif

#if defined(JSON_HEDLEY_GCC_VERSION_CHECK)
    #undef JSON_HEDLEY_GCC_VERSION_CHECK
#endif
#if defined(JSON_HEDLEY_GCC_VERSION)
    #define JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch) (JSON_HEDLEY_GCC_VERSION >= JSON_HEDLEY_VERSION_ENCODE(major, minor, patch))
#else
    #define JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch) (0)
#endif

#if defined(JSON_HEDLEY_HAS_ATTRIBUTE)
    #undef JSON_HEDLEY_HAS_ATTRIBUTE
#endif
#if \
  defined(__has_attribute) && \
  ( \
    (!defined(JSON_HEDLEY_IAR_VERSION) || JSON_HEDLEY_IAR_VERSION_CHECK(8,5,9)) \
  )
#  define JSON_HEDLEY_HAS_ATTRIBUTE(attribute) __has_attribute(attribute)
#else
#  define JSON_HEDLEY_HAS_ATTRIBUTE(attribute) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_ATTRIBUTE)
    #undef JSON_HEDLEY_GNUC_HAS_ATTRIBUTE
#endif
#if defined(__has_attribute)
    #define JSON_HEDLEY_GNUC_HAS_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_HAS_ATTRIBUTE(attribute)
#else
    #define JSON_HEDLEY_GNUC_HAS_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_ATTRIBUTE)
    #undef JSON_HEDLEY_GCC_HAS_ATTRIBUTE
#endif
#if defined(__has_attribute)
    #define JSON_HEDLEY_GCC_HAS_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_HAS_ATTRIBUTE(attribute)
#else
    #define JSON_HEDLEY_GCC_HAS_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_CPP_ATTRIBUTE)
    #undef JSON_HEDLEY_HAS_CPP_ATTRIBUTE
#endif
#if \
    defined(__has_cpp_attribute) && \
    defined(__cplusplus) && \
    (!defined(JSON_HEDLEY_SUNPRO_VERSION) || JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0))
    #define JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute) __has_cpp_attribute(attribute)
#else
    #define JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute) (0)
#endif

#if defined(JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS)
    #undef JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS
#endif
#if !defined(__cplusplus) || !defined(__has_cpp_attribute)
    #define JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) (0)
#elif \
    !defined(JSON_HEDLEY_PGI_VERSION) && \
    !defined(JSON_HEDLEY_IAR_VERSION) && \
    (!defined(JSON_HEDLEY_SUNPRO_VERSION) || JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0)) && \
    (!defined(JSON_HEDLEY_MSVC_VERSION) || JSON_HEDLEY_MSVC_VERSION_CHECK(19,20,0))
    #define JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) JSON_HEDLEY_HAS_CPP_ATTRIBUTE(ns::attribute)
#else
    #define JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(ns,attribute) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE)
    #undef JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE
#endif
#if defined(__has_cpp_attribute) && defined(__cplusplus)
    #define JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) __has_cpp_attribute(attribute)
#else
    #define JSON_HEDLEY_GNUC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE)
    #undef JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE
#endif
#if defined(__has_cpp_attribute) && defined(__cplusplus)
    #define JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) __has_cpp_attribute(attribute)
#else
    #define JSON_HEDLEY_GCC_HAS_CPP_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_BUILTIN)
    #undef JSON_HEDLEY_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define JSON_HEDLEY_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
    #define JSON_HEDLEY_HAS_BUILTIN(builtin) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_BUILTIN)
    #undef JSON_HEDLEY_GNUC_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define JSON_HEDLEY_GNUC_HAS_BUILTIN(builtin,major,minor,patch) __has_builtin(builtin)
#else
    #define JSON_HEDLEY_GNUC_HAS_BUILTIN(builtin,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_BUILTIN)
    #undef JSON_HEDLEY_GCC_HAS_BUILTIN
#endif
#if defined(__has_builtin)
    #define JSON_HEDLEY_GCC_HAS_BUILTIN(builtin,major,minor,patch) __has_builtin(builtin)
#else
    #define JSON_HEDLEY_GCC_HAS_BUILTIN(builtin,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_FEATURE)
    #undef JSON_HEDLEY_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define JSON_HEDLEY_HAS_FEATURE(feature) __has_feature(feature)
#else
    #define JSON_HEDLEY_HAS_FEATURE(feature) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_FEATURE)
    #undef JSON_HEDLEY_GNUC_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define JSON_HEDLEY_GNUC_HAS_FEATURE(feature,major,minor,patch) __has_feature(feature)
#else
    #define JSON_HEDLEY_GNUC_HAS_FEATURE(feature,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_FEATURE)
    #undef JSON_HEDLEY_GCC_HAS_FEATURE
#endif
#if defined(__has_feature)
    #define JSON_HEDLEY_GCC_HAS_FEATURE(feature,major,minor,patch) __has_feature(feature)
#else
    #define JSON_HEDLEY_GCC_HAS_FEATURE(feature,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_EXTENSION)
    #undef JSON_HEDLEY_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define JSON_HEDLEY_HAS_EXTENSION(extension) __has_extension(extension)
#else
    #define JSON_HEDLEY_HAS_EXTENSION(extension) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_EXTENSION)
    #undef JSON_HEDLEY_GNUC_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define JSON_HEDLEY_GNUC_HAS_EXTENSION(extension,major,minor,patch) __has_extension(extension)
#else
    #define JSON_HEDLEY_GNUC_HAS_EXTENSION(extension,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_EXTENSION)
    #undef JSON_HEDLEY_GCC_HAS_EXTENSION
#endif
#if defined(__has_extension)
    #define JSON_HEDLEY_GCC_HAS_EXTENSION(extension,major,minor,patch) __has_extension(extension)
#else
    #define JSON_HEDLEY_GCC_HAS_EXTENSION(extension,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE)
    #undef JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute) __has_declspec_attribute(attribute)
#else
    #define JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE)
    #undef JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) __has_declspec_attribute(attribute)
#else
    #define JSON_HEDLEY_GNUC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE)
    #undef JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE
#endif
#if defined(__has_declspec_attribute)
    #define JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) __has_declspec_attribute(attribute)
#else
    #define JSON_HEDLEY_GCC_HAS_DECLSPEC_ATTRIBUTE(attribute,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_HAS_WARNING)
    #undef JSON_HEDLEY_HAS_WARNING
#endif
#if defined(__has_warning)
    #define JSON_HEDLEY_HAS_WARNING(warning) __has_warning(warning)
#else
    #define JSON_HEDLEY_HAS_WARNING(warning) (0)
#endif

#if defined(JSON_HEDLEY_GNUC_HAS_WARNING)
    #undef JSON_HEDLEY_GNUC_HAS_WARNING
#endif
#if defined(__has_warning)
    #define JSON_HEDLEY_GNUC_HAS_WARNING(warning,major,minor,patch) __has_warning(warning)
#else
    #define JSON_HEDLEY_GNUC_HAS_WARNING(warning,major,minor,patch) JSON_HEDLEY_GNUC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_GCC_HAS_WARNING)
    #undef JSON_HEDLEY_GCC_HAS_WARNING
#endif
#if defined(__has_warning)
    #define JSON_HEDLEY_GCC_HAS_WARNING(warning,major,minor,patch) __has_warning(warning)
#else
    #define JSON_HEDLEY_GCC_HAS_WARNING(warning,major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if \
    (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    defined(__clang__) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(18,4,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,7,0) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(2,0,1) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,1,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,0,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_CRAY_VERSION_CHECK(5,0,0) || \
    JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,17) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(8,0,0) || \
    (JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) && defined(__C99_PRAGMA_OPERATOR))
    #define JSON_HEDLEY_PRAGMA(value) _Pragma(#value)
#elif JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define JSON_HEDLEY_PRAGMA(value) __pragma(value)
#else
    #define JSON_HEDLEY_PRAGMA(value)
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_PUSH)
    #undef JSON_HEDLEY_DIAGNOSTIC_PUSH
#endif
#if defined(JSON_HEDLEY_DIAGNOSTIC_POP)
    #undef JSON_HEDLEY_DIAGNOSTIC_POP
#endif
#if defined(__clang__)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("warning(pop)")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH __pragma(warning(push))
    #define JSON_HEDLEY_DIAGNOSTIC_POP __pragma(warning(pop))
#elif JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("push")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("pop")
#elif \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,4,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,1,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("diag_push")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("diag_pop")
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(2,90,0)
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH _Pragma("warning(push)")
    #define JSON_HEDLEY_DIAGNOSTIC_POP _Pragma("warning(pop)")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_PUSH
    #define JSON_HEDLEY_DIAGNOSTIC_POP
#endif

/* JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_ is for
   HEDLEY INTERNAL USE ONLY.  API subject to change without notice. */
#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_
#endif
#if defined(__cplusplus)
#  if JSON_HEDLEY_HAS_WARNING("-Wc++98-compat")
#    if JSON_HEDLEY_HAS_WARNING("-Wc++17-extensions")
#      if JSON_HEDLEY_HAS_WARNING("-Wc++1z-extensions")
#        define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"") \
    _Pragma("clang diagnostic ignored \"-Wc++1z-extensions\"") \
    xpr \
    JSON_HEDLEY_DIAGNOSTIC_POP
#      else
#        define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"") \
    xpr \
    JSON_HEDLEY_DIAGNOSTIC_POP
#      endif
#    else
#      define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(xpr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wc++98-compat\"") \
    xpr \
    JSON_HEDLEY_DIAGNOSTIC_POP
#    endif
#  endif
#endif
#if !defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(x) x
#endif

#if defined(JSON_HEDLEY_CONST_CAST)
    #undef JSON_HEDLEY_CONST_CAST
#endif
#if defined(__cplusplus)
#  define JSON_HEDLEY_CONST_CAST(T, expr) (const_cast<T>(expr))
#elif \
  JSON_HEDLEY_HAS_WARNING("-Wcast-qual") || \
  JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define JSON_HEDLEY_CONST_CAST(T, expr) (__extension__ ({ \
        JSON_HEDLEY_DIAGNOSTIC_PUSH \
        JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL \
        ((T) (expr)); \
        JSON_HEDLEY_DIAGNOSTIC_POP \
    }))
#else
#  define JSON_HEDLEY_CONST_CAST(T, expr) ((T) (expr))
#endif

#if defined(JSON_HEDLEY_REINTERPRET_CAST)
    #undef JSON_HEDLEY_REINTERPRET_CAST
#endif
#if defined(__cplusplus)
    #define JSON_HEDLEY_REINTERPRET_CAST(T, expr) (reinterpret_cast<T>(expr))
#else
    #define JSON_HEDLEY_REINTERPRET_CAST(T, expr) ((T) (expr))
#endif

#if defined(JSON_HEDLEY_STATIC_CAST)
    #undef JSON_HEDLEY_STATIC_CAST
#endif
#if defined(__cplusplus)
    #define JSON_HEDLEY_STATIC_CAST(T, expr) (static_cast<T>(expr))
#else
    #define JSON_HEDLEY_STATIC_CAST(T, expr) ((T) (expr))
#endif

#if defined(JSON_HEDLEY_CPP_CAST)
    #undef JSON_HEDLEY_CPP_CAST
#endif
#if defined(__cplusplus)
#  if JSON_HEDLEY_HAS_WARNING("-Wold-style-cast")
#    define JSON_HEDLEY_CPP_CAST(T, expr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") \
    ((T) (expr)) \
    JSON_HEDLEY_DIAGNOSTIC_POP
#  elif JSON_HEDLEY_IAR_VERSION_CHECK(8,3,0)
#    define JSON_HEDLEY_CPP_CAST(T, expr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("diag_suppress=Pe137") \
    JSON_HEDLEY_DIAGNOSTIC_POP
#  else
#    define JSON_HEDLEY_CPP_CAST(T, expr) ((T) (expr))
#  endif
#else
#  define JSON_HEDLEY_CPP_CAST(T, expr) (expr)
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wdeprecated-declarations")
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
#elif JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("warning(disable:1478 1786)")
#elif JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED __pragma(warning(disable:1478 1786))
#elif JSON_HEDLEY_PGI_VERSION_CHECK(20,7,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1216,1444,1445")
#elif JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1444")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED __pragma(warning(disable:4996))
#elif JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1215,1444")
#elif \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress 1291,1718")
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) && !defined(__cplusplus)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("error_messages(off,E_DEPRECATED_ATT,E_DEPRECATED_ATT_MESS)")
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) && defined(__cplusplus)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("error_messages(off,symdeprecated,symdeprecated2)")
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("diag_suppress=Pe1444,Pe1215")
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(2,90,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma("warn(disable:2241)")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_DEPRECATED
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
#elif JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("warning(disable:161)")
#elif JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:161))
#elif JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 1675")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"")
#elif JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS __pragma(warning(disable:4068))
#elif \
    JSON_HEDLEY_TI_VERSION_CHECK(16,9,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,3,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 163")
#elif JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 163")
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress=Pe161")
#elif JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma("diag_suppress 161")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wunknown-attributes")
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(4,6,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#elif JSON_HEDLEY_INTEL_VERSION_CHECK(17,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("warning(disable:1292)")
#elif JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES __pragma(warning(disable:1292))
#elif JSON_HEDLEY_MSVC_VERSION_CHECK(19,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES __pragma(warning(disable:5030))
#elif JSON_HEDLEY_PGI_VERSION_CHECK(20,7,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097,1098")
#elif JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097")
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,14,0) && defined(__cplusplus)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("error_messages(off,attrskipunsup)")
#elif \
    JSON_HEDLEY_TI_VERSION_CHECK(18,1,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,3,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1173")
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress=Pe1097")
#elif JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma("diag_suppress 1097")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wcast-qual")
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("clang diagnostic ignored \"-Wcast-qual\"")
#elif JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("warning(disable:2203 2331)")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma("GCC diagnostic ignored \"-Wcast-qual\"")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_CAST_QUAL
#endif

#if defined(JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION)
    #undef JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wunused-function")
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("clang diagnostic ignored \"-Wunused-function\"")
#elif JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#elif JSON_HEDLEY_MSVC_VERSION_CHECK(1,0,0)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION __pragma(warning(disable:4505))
#elif JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION _Pragma("diag_suppress 3142")
#else
    #define JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNUSED_FUNCTION
#endif

#if defined(JSON_HEDLEY_DEPRECATED)
    #undef JSON_HEDLEY_DEPRECATED
#endif
#if defined(JSON_HEDLEY_DEPRECATED_FOR)
    #undef JSON_HEDLEY_DEPRECATED_FOR
#endif
#if \
    JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DEPRECATED(since) __declspec(deprecated("Since " # since))
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __declspec(deprecated("Since " #since "; use " #replacement))
#elif \
    (JSON_HEDLEY_HAS_EXTENSION(attribute_deprecated_with_message) && !defined(JSON_HEDLEY_IAR_VERSION)) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,5,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,13,0) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(18,1,0) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(18,1,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,3,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,3,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_DEPRECATED(since) __attribute__((__deprecated__("Since " #since)))
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __attribute__((__deprecated__("Since " #since "; use " #replacement)))
#elif defined(__cplusplus) && (__cplusplus >= 201402L)
    #define JSON_HEDLEY_DEPRECATED(since) JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[deprecated("Since " #since)]])
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[deprecated("Since " #since "; use " #replacement)]])
#elif \
    JSON_HEDLEY_HAS_ATTRIBUTE(deprecated) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
    JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define JSON_HEDLEY_DEPRECATED(since) __attribute__((__deprecated__))
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __attribute__((__deprecated__))
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    JSON_HEDLEY_PELLES_VERSION_CHECK(6,50,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_DEPRECATED(since) __declspec(deprecated)
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) __declspec(deprecated)
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_DEPRECATED(since) _Pragma("deprecated")
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement) _Pragma("deprecated")
#else
    #define JSON_HEDLEY_DEPRECATED(since)
    #define JSON_HEDLEY_DEPRECATED_FOR(since, replacement)
#endif

#if defined(JSON_HEDLEY_UNAVAILABLE)
    #undef JSON_HEDLEY_UNAVAILABLE
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(warning) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,3,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_UNAVAILABLE(available_since) __attribute__((__warning__("Not available until " #available_since)))
#else
    #define JSON_HEDLEY_UNAVAILABLE(available_since)
#endif

#if defined(JSON_HEDLEY_WARN_UNUSED_RESULT)
    #undef JSON_HEDLEY_WARN_UNUSED_RESULT
#endif
#if defined(JSON_HEDLEY_WARN_UNUSED_RESULT_MSG)
    #undef JSON_HEDLEY_WARN_UNUSED_RESULT_MSG
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(warn_unused_result) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    (JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0) && defined(__cplusplus)) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
    #define JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) __attribute__((__warn_unused_result__))
#elif (JSON_HEDLEY_HAS_CPP_ATTRIBUTE(nodiscard) >= 201907L)
    #define JSON_HEDLEY_WARN_UNUSED_RESULT JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
    #define JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard(msg)]])
#elif JSON_HEDLEY_HAS_CPP_ATTRIBUTE(nodiscard)
    #define JSON_HEDLEY_WARN_UNUSED_RESULT JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
    #define JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[nodiscard]])
#elif defined(_Check_return_) /* SAL */
    #define JSON_HEDLEY_WARN_UNUSED_RESULT _Check_return_
    #define JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg) _Check_return_
#else
    #define JSON_HEDLEY_WARN_UNUSED_RESULT
    #define JSON_HEDLEY_WARN_UNUSED_RESULT_MSG(msg)
#endif

#if defined(JSON_HEDLEY_SENTINEL)
    #undef JSON_HEDLEY_SENTINEL
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(sentinel) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(5,4,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_SENTINEL(position) __attribute__((__sentinel__(position)))
#else
    #define JSON_HEDLEY_SENTINEL(position)
#endif

#if defined(JSON_HEDLEY_NO_RETURN)
    #undef JSON_HEDLEY_NO_RETURN
#endif
#if JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_NO_RETURN __noreturn
#elif \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_NO_RETURN __attribute__((__noreturn__))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define JSON_HEDLEY_NO_RETURN _Noreturn
#elif defined(__cplusplus) && (__cplusplus >= 201103L)
    #define JSON_HEDLEY_NO_RETURN JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[noreturn]])
#elif \
    JSON_HEDLEY_HAS_ATTRIBUTE(noreturn) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,2,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define JSON_HEDLEY_NO_RETURN __attribute__((__noreturn__))
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define JSON_HEDLEY_NO_RETURN _Pragma("does_not_return")
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_NO_RETURN __declspec(noreturn)
#elif JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,0,0) && defined(__cplusplus)
    #define JSON_HEDLEY_NO_RETURN _Pragma("FUNC_NEVER_RETURNS;")
#elif JSON_HEDLEY_COMPCERT_VERSION_CHECK(3,2,0)
    #define JSON_HEDLEY_NO_RETURN __attribute((noreturn))
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(9,0,0)
    #define JSON_HEDLEY_NO_RETURN __declspec(noreturn)
#else
    #define JSON_HEDLEY_NO_RETURN
#endif

#if defined(JSON_HEDLEY_NO_ESCAPE)
    #undef JSON_HEDLEY_NO_ESCAPE
#endif
#if JSON_HEDLEY_HAS_ATTRIBUTE(noescape)
    #define JSON_HEDLEY_NO_ESCAPE __attribute__((__noescape__))
#else
    #define JSON_HEDLEY_NO_ESCAPE
#endif

#if defined(JSON_HEDLEY_UNREACHABLE)
    #undef JSON_HEDLEY_UNREACHABLE
#endif
#if defined(JSON_HEDLEY_UNREACHABLE_RETURN)
    #undef JSON_HEDLEY_UNREACHABLE_RETURN
#endif
#if defined(JSON_HEDLEY_ASSUME)
    #undef JSON_HEDLEY_ASSUME
#endif
#if \
    JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_ASSUME(expr) __assume(expr)
#elif JSON_HEDLEY_HAS_BUILTIN(__builtin_assume)
    #define JSON_HEDLEY_ASSUME(expr) __builtin_assume(expr)
#elif \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0)
    #if defined(__cplusplus)
        #define JSON_HEDLEY_ASSUME(expr) std::_nassert(expr)
    #else
        #define JSON_HEDLEY_ASSUME(expr) _nassert(expr)
    #endif
#endif
#if \
    (JSON_HEDLEY_HAS_BUILTIN(__builtin_unreachable) && (!defined(JSON_HEDLEY_ARM_VERSION))) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,5,0) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(18,10,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(13,1,5) || \
    JSON_HEDLEY_CRAY_VERSION_CHECK(10,0,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_UNREACHABLE() __builtin_unreachable()
#elif defined(JSON_HEDLEY_ASSUME)
    #define JSON_HEDLEY_UNREACHABLE() JSON_HEDLEY_ASSUME(0)
#endif
#if !defined(JSON_HEDLEY_ASSUME)
    #if defined(JSON_HEDLEY_UNREACHABLE)
        #define JSON_HEDLEY_ASSUME(expr) JSON_HEDLEY_STATIC_CAST(void, ((expr) ? 1 : (JSON_HEDLEY_UNREACHABLE(), 1)))
    #else
        #define JSON_HEDLEY_ASSUME(expr) JSON_HEDLEY_STATIC_CAST(void, expr)
    #endif
#endif
#if defined(JSON_HEDLEY_UNREACHABLE)
    #if  \
        JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
        JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0)
        #define JSON_HEDLEY_UNREACHABLE_RETURN(value) return (JSON_HEDLEY_STATIC_CAST(void, JSON_HEDLEY_ASSUME(0)), (value))
    #else
        #define JSON_HEDLEY_UNREACHABLE_RETURN(value) JSON_HEDLEY_UNREACHABLE()
    #endif
#else
    #define JSON_HEDLEY_UNREACHABLE_RETURN(value) return (value)
#endif
#if !defined(JSON_HEDLEY_UNREACHABLE)
    #define JSON_HEDLEY_UNREACHABLE() JSON_HEDLEY_ASSUME(0)
#endif

JSON_HEDLEY_DIAGNOSTIC_PUSH
#if JSON_HEDLEY_HAS_WARNING("-Wpedantic")
    #pragma clang diagnostic ignored "-Wpedantic"
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wc++98-compat-pedantic") && defined(__cplusplus)
    #pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif
#if JSON_HEDLEY_GCC_HAS_WARNING("-Wvariadic-macros",4,0,0)
    #if defined(__clang__)
        #pragma clang diagnostic ignored "-Wvariadic-macros"
    #elif defined(JSON_HEDLEY_GCC_VERSION)
        #pragma GCC diagnostic ignored "-Wvariadic-macros"
    #endif
#endif
#if defined(JSON_HEDLEY_NON_NULL)
    #undef JSON_HEDLEY_NON_NULL
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(nonnull) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0)
    #define JSON_HEDLEY_NON_NULL(...) __attribute__((__nonnull__(__VA_ARGS__)))
#else
    #define JSON_HEDLEY_NON_NULL(...)
#endif
JSON_HEDLEY_DIAGNOSTIC_POP

#if defined(JSON_HEDLEY_PRINTF_FORMAT)
    #undef JSON_HEDLEY_PRINTF_FORMAT
#endif
#if defined(__MINGW32__) && JSON_HEDLEY_GCC_HAS_ATTRIBUTE(format,4,4,0) && !defined(__USE_MINGW_ANSI_STDIO)
    #define JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(ms_printf, string_idx, first_to_check)))
#elif defined(__MINGW32__) && JSON_HEDLEY_GCC_HAS_ATTRIBUTE(format,4,4,0) && defined(__USE_MINGW_ANSI_STDIO)
    #define JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(gnu_printf, string_idx, first_to_check)))
#elif \
    JSON_HEDLEY_HAS_ATTRIBUTE(format) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(5,6,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __attribute__((__format__(__printf__, string_idx, first_to_check)))
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(6,0,0)
    #define JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check) __declspec(vaformat(printf,string_idx,first_to_check))
#else
    #define JSON_HEDLEY_PRINTF_FORMAT(string_idx,first_to_check)
#endif

#if defined(JSON_HEDLEY_CONSTEXPR)
    #undef JSON_HEDLEY_CONSTEXPR
#endif
#if defined(__cplusplus)
    #if __cplusplus >= 201103L
        #define JSON_HEDLEY_CONSTEXPR JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(constexpr)
    #endif
#endif
#if !defined(JSON_HEDLEY_CONSTEXPR)
    #define JSON_HEDLEY_CONSTEXPR
#endif

#if defined(JSON_HEDLEY_PREDICT)
    #undef JSON_HEDLEY_PREDICT
#endif
#if defined(JSON_HEDLEY_LIKELY)
    #undef JSON_HEDLEY_LIKELY
#endif
#if defined(JSON_HEDLEY_UNLIKELY)
    #undef JSON_HEDLEY_UNLIKELY
#endif
#if defined(JSON_HEDLEY_UNPREDICTABLE)
    #undef JSON_HEDLEY_UNPREDICTABLE
#endif
#if JSON_HEDLEY_HAS_BUILTIN(__builtin_unpredictable)
    #define JSON_HEDLEY_UNPREDICTABLE(expr) __builtin_unpredictable((expr))
#endif
#if \
  (JSON_HEDLEY_HAS_BUILTIN(__builtin_expect_with_probability) && !defined(JSON_HEDLEY_PGI_VERSION)) || \
  JSON_HEDLEY_GCC_VERSION_CHECK(9,0,0) || \
  JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define JSON_HEDLEY_PREDICT(expr, value, probability) __builtin_expect_with_probability(  (expr), (value), (probability))
#  define JSON_HEDLEY_PREDICT_TRUE(expr, probability)   __builtin_expect_with_probability(!!(expr),    1   , (probability))
#  define JSON_HEDLEY_PREDICT_FALSE(expr, probability)  __builtin_expect_with_probability(!!(expr),    0   , (probability))
#  define JSON_HEDLEY_LIKELY(expr)                      __builtin_expect                 (!!(expr),    1                  )
#  define JSON_HEDLEY_UNLIKELY(expr)                    __builtin_expect                 (!!(expr),    0                  )
#elif \
  (JSON_HEDLEY_HAS_BUILTIN(__builtin_expect) && !defined(JSON_HEDLEY_INTEL_CL_VERSION)) || \
  JSON_HEDLEY_GCC_VERSION_CHECK(3,0,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  (JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,15,0) && defined(__cplusplus)) || \
  JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,7,0) || \
  JSON_HEDLEY_TI_CL430_VERSION_CHECK(3,1,0) || \
  JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,1,0) || \
  JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
  JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,27) || \
  JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
  JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define JSON_HEDLEY_PREDICT(expr, expected, probability) \
    (((probability) >= 0.9) ? __builtin_expect((expr), (expected)) : (JSON_HEDLEY_STATIC_CAST(void, expected), (expr)))
#  define JSON_HEDLEY_PREDICT_TRUE(expr, probability) \
    (__extension__ ({ \
        double hedley_probability_ = (probability); \
        ((hedley_probability_ >= 0.9) ? __builtin_expect(!!(expr), 1) : ((hedley_probability_ <= 0.1) ? __builtin_expect(!!(expr), 0) : !!(expr))); \
    }))
#  define JSON_HEDLEY_PREDICT_FALSE(expr, probability) \
    (__extension__ ({ \
        double hedley_probability_ = (probability); \
        ((hedley_probability_ >= 0.9) ? __builtin_expect(!!(expr), 0) : ((hedley_probability_ <= 0.1) ? __builtin_expect(!!(expr), 1) : !!(expr))); \
    }))
#  define JSON_HEDLEY_LIKELY(expr)   __builtin_expect(!!(expr), 1)
#  define JSON_HEDLEY_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#  define JSON_HEDLEY_PREDICT(expr, expected, probability) (JSON_HEDLEY_STATIC_CAST(void, expected), (expr))
#  define JSON_HEDLEY_PREDICT_TRUE(expr, probability) (!!(expr))
#  define JSON_HEDLEY_PREDICT_FALSE(expr, probability) (!!(expr))
#  define JSON_HEDLEY_LIKELY(expr) (!!(expr))
#  define JSON_HEDLEY_UNLIKELY(expr) (!!(expr))
#endif
#if !defined(JSON_HEDLEY_UNPREDICTABLE)
    #define JSON_HEDLEY_UNPREDICTABLE(expr) JSON_HEDLEY_PREDICT(expr, 1, 0.5)
#endif

#if defined(JSON_HEDLEY_MALLOC)
    #undef JSON_HEDLEY_MALLOC
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(malloc) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(12,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_MALLOC __attribute__((__malloc__))
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define JSON_HEDLEY_MALLOC _Pragma("returns_new_memory")
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_MALLOC __declspec(restrict)
#else
    #define JSON_HEDLEY_MALLOC
#endif

#if defined(JSON_HEDLEY_PURE)
    #undef JSON_HEDLEY_PURE
#endif
#if \
  JSON_HEDLEY_HAS_ATTRIBUTE(pure) || \
  JSON_HEDLEY_GCC_VERSION_CHECK(2,96,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
  JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
  (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
  (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
  (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
  JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
  JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#  define JSON_HEDLEY_PURE __attribute__((__pure__))
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
#  define JSON_HEDLEY_PURE _Pragma("does_not_write_global_data")
#elif defined(__cplusplus) && \
    ( \
      JSON_HEDLEY_TI_CL430_VERSION_CHECK(2,0,1) || \
      JSON_HEDLEY_TI_CL6X_VERSION_CHECK(4,0,0) || \
      JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) \
    )
#  define JSON_HEDLEY_PURE _Pragma("FUNC_IS_PURE;")
#else
#  define JSON_HEDLEY_PURE
#endif

#if defined(JSON_HEDLEY_CONST)
    #undef JSON_HEDLEY_CONST
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(const) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(2,5,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_CONST __attribute__((__const__))
#elif \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0)
    #define JSON_HEDLEY_CONST _Pragma("no_side_effect")
#else
    #define JSON_HEDLEY_CONST JSON_HEDLEY_PURE
#endif

#if defined(JSON_HEDLEY_RESTRICT)
    #undef JSON_HEDLEY_RESTRICT
#endif
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && !defined(__cplusplus)
    #define JSON_HEDLEY_RESTRICT restrict
#elif \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,1,0) || \
    JSON_HEDLEY_MSVC_VERSION_CHECK(14,0,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    JSON_HEDLEY_PGI_VERSION_CHECK(17,10,0) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,4) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,1,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    (JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,14,0) && defined(__cplusplus)) || \
    JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0) || \
    defined(__clang__) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_RESTRICT __restrict
#elif JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,3,0) && !defined(__cplusplus)
    #define JSON_HEDLEY_RESTRICT _Restrict
#else
    #define JSON_HEDLEY_RESTRICT
#endif

#if defined(JSON_HEDLEY_INLINE)
    #undef JSON_HEDLEY_INLINE
#endif
#if \
    (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) || \
    (defined(__cplusplus) && (__cplusplus >= 199711L))
    #define JSON_HEDLEY_INLINE inline
#elif \
    defined(JSON_HEDLEY_GCC_VERSION) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(6,2,0)
    #define JSON_HEDLEY_INLINE __inline__
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(12,0,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,1,0) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(3,1,0) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,2,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(8,0,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_INLINE __inline
#else
    #define JSON_HEDLEY_INLINE
#endif

#if defined(JSON_HEDLEY_ALWAYS_INLINE)
    #undef JSON_HEDLEY_ALWAYS_INLINE
#endif
#if \
  JSON_HEDLEY_HAS_ATTRIBUTE(always_inline) || \
  JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
  JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
  JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
  JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
  JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
  (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
  (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
  (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
  (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
  JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
  JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
  JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
  JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
  JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
#  define JSON_HEDLEY_ALWAYS_INLINE __attribute__((__always_inline__)) JSON_HEDLEY_INLINE
#elif \
  JSON_HEDLEY_MSVC_VERSION_CHECK(12,0,0) || \
  JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define JSON_HEDLEY_ALWAYS_INLINE __forceinline
#elif defined(__cplusplus) && \
    ( \
      JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
      JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
      JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
      JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
      JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
      JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) \
    )
#  define JSON_HEDLEY_ALWAYS_INLINE _Pragma("FUNC_ALWAYS_INLINE;")
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
#  define JSON_HEDLEY_ALWAYS_INLINE _Pragma("inline=forced")
#else
#  define JSON_HEDLEY_ALWAYS_INLINE JSON_HEDLEY_INLINE
#endif

#if defined(JSON_HEDLEY_NEVER_INLINE)
    #undef JSON_HEDLEY_NEVER_INLINE
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(noinline) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,0,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(10,1,0) || \
    JSON_HEDLEY_TI_VERSION_CHECK(15,12,0) || \
    (JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(4,8,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_ARMCL_VERSION_CHECK(5,2,0) || \
    (JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL2000_VERSION_CHECK(6,4,0) || \
    (JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,0,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL430_VERSION_CHECK(4,3,0) || \
    (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) || \
    JSON_HEDLEY_TI_CL7X_VERSION_CHECK(1,2,0) || \
    JSON_HEDLEY_TI_CLPRU_VERSION_CHECK(2,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10) || \
    JSON_HEDLEY_IAR_VERSION_CHECK(8,10,0)
    #define JSON_HEDLEY_NEVER_INLINE __attribute__((__noinline__))
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(13,10,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_NEVER_INLINE __declspec(noinline)
#elif JSON_HEDLEY_PGI_VERSION_CHECK(10,2,0)
    #define JSON_HEDLEY_NEVER_INLINE _Pragma("noinline")
#elif JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,0,0) && defined(__cplusplus)
    #define JSON_HEDLEY_NEVER_INLINE _Pragma("FUNC_CANNOT_INLINE;")
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
    #define JSON_HEDLEY_NEVER_INLINE _Pragma("inline=never")
#elif JSON_HEDLEY_COMPCERT_VERSION_CHECK(3,2,0)
    #define JSON_HEDLEY_NEVER_INLINE __attribute((noinline))
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(9,0,0)
    #define JSON_HEDLEY_NEVER_INLINE __declspec(noinline)
#else
    #define JSON_HEDLEY_NEVER_INLINE
#endif

#if defined(JSON_HEDLEY_PRIVATE)
    #undef JSON_HEDLEY_PRIVATE
#endif
#if defined(JSON_HEDLEY_PUBLIC)
    #undef JSON_HEDLEY_PUBLIC
#endif
#if defined(JSON_HEDLEY_IMPORT)
    #undef JSON_HEDLEY_IMPORT
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#  define JSON_HEDLEY_PRIVATE
#  define JSON_HEDLEY_PUBLIC   __declspec(dllexport)
#  define JSON_HEDLEY_IMPORT   __declspec(dllimport)
#else
#  if \
    JSON_HEDLEY_HAS_ATTRIBUTE(visibility) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,11,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
    ( \
      defined(__TI_EABI__) && \
      ( \
        (JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,2,0) && defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) || \
        JSON_HEDLEY_TI_CL6X_VERSION_CHECK(7,5,0) \
      ) \
    ) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
#    define JSON_HEDLEY_PRIVATE __attribute__((__visibility__("hidden")))
#    define JSON_HEDLEY_PUBLIC  __attribute__((__visibility__("default")))
#  else
#    define JSON_HEDLEY_PRIVATE
#    define JSON_HEDLEY_PUBLIC
#  endif
#  define JSON_HEDLEY_IMPORT    extern
#endif

#if defined(JSON_HEDLEY_NO_THROW)
    #undef JSON_HEDLEY_NO_THROW
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(nothrow) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,3,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_NO_THROW __attribute__((__nothrow__))
#elif \
    JSON_HEDLEY_MSVC_VERSION_CHECK(13,1,0) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0)
    #define JSON_HEDLEY_NO_THROW __declspec(nothrow)
#else
    #define JSON_HEDLEY_NO_THROW
#endif

#if defined(JSON_HEDLEY_FALL_THROUGH)
    #undef JSON_HEDLEY_FALL_THROUGH
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(fallthrough) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(7,0,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_FALL_THROUGH __attribute__((__fallthrough__))
#elif JSON_HEDLEY_HAS_CPP_ATTRIBUTE_NS(clang,fallthrough)
    #define JSON_HEDLEY_FALL_THROUGH JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[clang::fallthrough]])
#elif JSON_HEDLEY_HAS_CPP_ATTRIBUTE(fallthrough)
    #define JSON_HEDLEY_FALL_THROUGH JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[fallthrough]])
#elif defined(__fallthrough) /* SAL */
    #define JSON_HEDLEY_FALL_THROUGH __fallthrough
#else
    #define JSON_HEDLEY_FALL_THROUGH
#endif

#if defined(JSON_HEDLEY_RETURNS_NON_NULL)
    #undef JSON_HEDLEY_RETURNS_NON_NULL
#endif
#if \
    JSON_HEDLEY_HAS_ATTRIBUTE(returns_nonnull) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(4,9,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_RETURNS_NON_NULL __attribute__((__returns_nonnull__))
#elif defined(_Ret_notnull_) /* SAL */
    #define JSON_HEDLEY_RETURNS_NON_NULL _Ret_notnull_
#else
    #define JSON_HEDLEY_RETURNS_NON_NULL
#endif

#if defined(JSON_HEDLEY_ARRAY_PARAM)
    #undef JSON_HEDLEY_ARRAY_PARAM
#endif
#if \
    defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && \
    !defined(__STDC_NO_VLA__) && \
    !defined(__cplusplus) && \
    !defined(JSON_HEDLEY_PGI_VERSION) && \
    !defined(JSON_HEDLEY_TINYC_VERSION)
    #define JSON_HEDLEY_ARRAY_PARAM(name) (name)
#else
    #define JSON_HEDLEY_ARRAY_PARAM(name)
#endif

#if defined(JSON_HEDLEY_IS_CONSTANT)
    #undef JSON_HEDLEY_IS_CONSTANT
#endif
#if defined(JSON_HEDLEY_REQUIRE_CONSTEXPR)
    #undef JSON_HEDLEY_REQUIRE_CONSTEXPR
#endif
/* JSON_HEDLEY_IS_CONSTEXPR_ is for
   HEDLEY INTERNAL USE ONLY.  API subject to change without notice. */
#if defined(JSON_HEDLEY_IS_CONSTEXPR_)
    #undef JSON_HEDLEY_IS_CONSTEXPR_
#endif
#if \
    JSON_HEDLEY_HAS_BUILTIN(__builtin_constant_p) || \
    JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
    JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
    JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,19) || \
    JSON_HEDLEY_ARM_VERSION_CHECK(4,1,0) || \
    JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
    JSON_HEDLEY_TI_CL6X_VERSION_CHECK(6,1,0) || \
    (JSON_HEDLEY_SUNPRO_VERSION_CHECK(5,10,0) && !defined(__cplusplus)) || \
    JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
    JSON_HEDLEY_MCST_LCC_VERSION_CHECK(1,25,10)
    #define JSON_HEDLEY_IS_CONSTANT(expr) __builtin_constant_p(expr)
#endif
#if !defined(__cplusplus)
#  if \
       JSON_HEDLEY_HAS_BUILTIN(__builtin_types_compatible_p) || \
       JSON_HEDLEY_GCC_VERSION_CHECK(3,4,0) || \
       JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
       JSON_HEDLEY_IBM_VERSION_CHECK(13,1,0) || \
       JSON_HEDLEY_CRAY_VERSION_CHECK(8,1,0) || \
       JSON_HEDLEY_ARM_VERSION_CHECK(5,4,0) || \
       JSON_HEDLEY_TINYC_VERSION_CHECK(0,9,24)
#if defined(__INTPTR_TYPE__)
    #define JSON_HEDLEY_IS_CONSTEXPR_(expr) __builtin_types_compatible_p(__typeof__((1 ? (void*) ((__INTPTR_TYPE__) ((expr) * 0)) : (int*) 0)), int*)
#else
    #include <stdint.h>
    #define JSON_HEDLEY_IS_CONSTEXPR_(expr) __builtin_types_compatible_p(__typeof__((1 ? (void*) ((intptr_t) ((expr) * 0)) : (int*) 0)), int*)
#endif
#  elif \
       ( \
          defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && \
          !defined(JSON_HEDLEY_SUNPRO_VERSION) && \
          !defined(JSON_HEDLEY_PGI_VERSION) && \
          !defined(JSON_HEDLEY_IAR_VERSION)) || \
       (JSON_HEDLEY_HAS_EXTENSION(c_generic_selections) && !defined(JSON_HEDLEY_IAR_VERSION)) || \
       JSON_HEDLEY_GCC_VERSION_CHECK(4,9,0) || \
       JSON_HEDLEY_INTEL_VERSION_CHECK(17,0,0) || \
       JSON_HEDLEY_IBM_VERSION_CHECK(12,1,0) || \
       JSON_HEDLEY_ARM_VERSION_CHECK(5,3,0)
#if defined(__INTPTR_TYPE__)
    #define JSON_HEDLEY_IS_CONSTEXPR_(expr) _Generic((1 ? (void*) ((__INTPTR_TYPE__) ((expr) * 0)) : (int*) 0), int*: 1, void*: 0)
#else
    #include <stdint.h>
    #define JSON_HEDLEY_IS_CONSTEXPR_(expr) _Generic((1 ? (void*) ((intptr_t) * 0) : (int*) 0), int*: 1, void*: 0)
#endif
#  elif \
       defined(JSON_HEDLEY_GCC_VERSION) || \
       defined(JSON_HEDLEY_INTEL_VERSION) || \
       defined(JSON_HEDLEY_TINYC_VERSION) || \
       defined(JSON_HEDLEY_TI_ARMCL_VERSION) || \
       JSON_HEDLEY_TI_CL430_VERSION_CHECK(18,12,0) || \
       defined(JSON_HEDLEY_TI_CL2000_VERSION) || \
       defined(JSON_HEDLEY_TI_CL6X_VERSION) || \
       defined(JSON_HEDLEY_TI_CL7X_VERSION) || \
       defined(JSON_HEDLEY_TI_CLPRU_VERSION) || \
       defined(__clang__)
#    define JSON_HEDLEY_IS_CONSTEXPR_(expr) ( \
        sizeof(void) != \
        sizeof(*( \
                  1 ? \
                  ((void*) ((expr) * 0L) ) : \
((struct { char v[sizeof(void) * 2]; } *) 1) \
                ) \
              ) \
                                            )
#  endif
#endif
#if defined(JSON_HEDLEY_IS_CONSTEXPR_)
    #if !defined(JSON_HEDLEY_IS_CONSTANT)
        #define JSON_HEDLEY_IS_CONSTANT(expr) JSON_HEDLEY_IS_CONSTEXPR_(expr)
    #endif
    #define JSON_HEDLEY_REQUIRE_CONSTEXPR(expr) (JSON_HEDLEY_IS_CONSTEXPR_(expr) ? (expr) : (-1))
#else
    #if !defined(JSON_HEDLEY_IS_CONSTANT)
        #define JSON_HEDLEY_IS_CONSTANT(expr) (0)
    #endif
    #define JSON_HEDLEY_REQUIRE_CONSTEXPR(expr) (expr)
#endif

#if defined(JSON_HEDLEY_BEGIN_C_DECLS)
    #undef JSON_HEDLEY_BEGIN_C_DECLS
#endif
#if defined(JSON_HEDLEY_END_C_DECLS)
    #undef JSON_HEDLEY_END_C_DECLS
#endif
#if defined(JSON_HEDLEY_C_DECL)
    #undef JSON_HEDLEY_C_DECL
#endif
#if defined(__cplusplus)
    #define JSON_HEDLEY_BEGIN_C_DECLS extern "C" {
    #define JSON_HEDLEY_END_C_DECLS }
    #define JSON_HEDLEY_C_DECL extern "C"
#else
    #define JSON_HEDLEY_BEGIN_C_DECLS
    #define JSON_HEDLEY_END_C_DECLS
    #define JSON_HEDLEY_C_DECL
#endif

#if defined(JSON_HEDLEY_STATIC_ASSERT)
    #undef JSON_HEDLEY_STATIC_ASSERT
#endif
#if \
  !defined(__cplusplus) && ( \
      (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)) || \
      (JSON_HEDLEY_HAS_FEATURE(c_static_assert) && !defined(JSON_HEDLEY_INTEL_CL_VERSION)) || \
      JSON_HEDLEY_GCC_VERSION_CHECK(6,0,0) || \
      JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0) || \
      defined(_Static_assert) \
    )
#  define JSON_HEDLEY_STATIC_ASSERT(expr, message) _Static_assert(expr, message)
#elif \
  (defined(__cplusplus) && (__cplusplus >= 201103L)) || \
  JSON_HEDLEY_MSVC_VERSION_CHECK(16,0,0) || \
  JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define JSON_HEDLEY_STATIC_ASSERT(expr, message) JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(static_assert(expr, message))
#else
#  define JSON_HEDLEY_STATIC_ASSERT(expr, message)
#endif

#if defined(JSON_HEDLEY_NULL)
    #undef JSON_HEDLEY_NULL
#endif
#if defined(__cplusplus)
    #if __cplusplus >= 201103L
        #define JSON_HEDLEY_NULL JSON_HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_(nullptr)
    #elif defined(NULL)
        #define JSON_HEDLEY_NULL NULL
    #else
        #define JSON_HEDLEY_NULL JSON_HEDLEY_STATIC_CAST(void*, 0)
    #endif
#elif defined(NULL)
    #define JSON_HEDLEY_NULL NULL
#else
    #define JSON_HEDLEY_NULL ((void*) 0)
#endif

#if defined(JSON_HEDLEY_MESSAGE)
    #undef JSON_HEDLEY_MESSAGE
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
#  define JSON_HEDLEY_MESSAGE(msg) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
    JSON_HEDLEY_PRAGMA(message msg) \
    JSON_HEDLEY_DIAGNOSTIC_POP
#elif \
  JSON_HEDLEY_GCC_VERSION_CHECK(4,4,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define JSON_HEDLEY_MESSAGE(msg) JSON_HEDLEY_PRAGMA(message msg)
#elif JSON_HEDLEY_CRAY_VERSION_CHECK(5,0,0)
#  define JSON_HEDLEY_MESSAGE(msg) JSON_HEDLEY_PRAGMA(_CRI message msg)
#elif JSON_HEDLEY_IAR_VERSION_CHECK(8,0,0)
#  define JSON_HEDLEY_MESSAGE(msg) JSON_HEDLEY_PRAGMA(message(msg))
#elif JSON_HEDLEY_PELLES_VERSION_CHECK(2,0,0)
#  define JSON_HEDLEY_MESSAGE(msg) JSON_HEDLEY_PRAGMA(message(msg))
#else
#  define JSON_HEDLEY_MESSAGE(msg)
#endif

#if defined(JSON_HEDLEY_WARNING)
    #undef JSON_HEDLEY_WARNING
#endif
#if JSON_HEDLEY_HAS_WARNING("-Wunknown-pragmas")
#  define JSON_HEDLEY_WARNING(msg) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    JSON_HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
    JSON_HEDLEY_PRAGMA(clang warning msg) \
    JSON_HEDLEY_DIAGNOSTIC_POP
#elif \
  JSON_HEDLEY_GCC_VERSION_CHECK(4,8,0) || \
  JSON_HEDLEY_PGI_VERSION_CHECK(18,4,0) || \
  JSON_HEDLEY_INTEL_VERSION_CHECK(13,0,0)
#  define JSON_HEDLEY_WARNING(msg) JSON_HEDLEY_PRAGMA(GCC warning msg)
#elif \
  JSON_HEDLEY_MSVC_VERSION_CHECK(15,0,0) || \
  JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
#  define JSON_HEDLEY_WARNING(msg) JSON_HEDLEY_PRAGMA(message(msg))
#else
#  define JSON_HEDLEY_WARNING(msg) JSON_HEDLEY_MESSAGE(msg)
#endif

#if defined(JSON_HEDLEY_REQUIRE)
    #undef JSON_HEDLEY_REQUIRE
#endif
#if defined(JSON_HEDLEY_REQUIRE_MSG)
    #undef JSON_HEDLEY_REQUIRE_MSG
#endif
#if JSON_HEDLEY_HAS_ATTRIBUTE(diagnose_if)
#  if JSON_HEDLEY_HAS_WARNING("-Wgcc-compat")
#    define JSON_HEDLEY_REQUIRE(expr) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wgcc-compat\"") \
    __attribute__((diagnose_if(!(expr), #expr, "error"))) \
    JSON_HEDLEY_DIAGNOSTIC_POP
#    define JSON_HEDLEY_REQUIRE_MSG(expr,msg) \
    JSON_HEDLEY_DIAGNOSTIC_PUSH \
    _Pragma("clang diagnostic ignored \"-Wgcc-compat\"") \
    __attribute__((diagnose_if(!(expr), msg, "error"))) \
    JSON_HEDLEY_DIAGNOSTIC_POP
#  else
#    define JSON_HEDLEY_REQUIRE(expr) __attribute__((diagnose_if(!(expr), #expr, "error")))
#    define JSON_HEDLEY_REQUIRE_MSG(expr,msg) __attribute__((diagnose_if(!(expr), msg, "error")))
#  endif
#else
#  define JSON_HEDLEY_REQUIRE(expr)
#  define JSON_HEDLEY_REQUIRE_MSG(expr,msg)
#endif

#if defined(JSON_HEDLEY_FLAGS)
    #undef JSON_HEDLEY_FLAGS
#endif
#if JSON_HEDLEY_HAS_ATTRIBUTE(flag_enum) && (!defined(__cplusplus) || JSON_HEDLEY_HAS_WARNING("-Wbitfield-enum-conversion"))
    #define JSON_HEDLEY_FLAGS __attribute__((__flag_enum__))
#else
    #define JSON_HEDLEY_FLAGS
#endif

#if defined(JSON_HEDLEY_FLAGS_CAST)
    #undef JSON_HEDLEY_FLAGS_CAST
#endif
#if JSON_HEDLEY_INTEL_VERSION_CHECK(19,0,0)
#  define JSON_HEDLEY_FLAGS_CAST(T, expr) (__extension__ ({ \
        JSON_HEDLEY_DIAGNOSTIC_PUSH \
        _Pragma("warning(disable:188)") \
        ((T) (expr)); \
        JSON_HEDLEY_DIAGNOSTIC_POP \
    }))
#else
#  define JSON_HEDLEY_FLAGS_CAST(T, expr) JSON_HEDLEY_STATIC_CAST(T, expr)
#endif

#if defined(JSON_HEDLEY_EMPTY_BASES)
    #undef JSON_HEDLEY_EMPTY_BASES
#endif
#if \
    (JSON_HEDLEY_MSVC_VERSION_CHECK(19,0,23918) && !JSON_HEDLEY_MSVC_VERSION_CHECK(20,0,0)) || \
    JSON_HEDLEY_INTEL_CL_VERSION_CHECK(2021,1,0)
    #define JSON_HEDLEY_EMPTY_BASES __declspec(empty_bases)
#else
    #define JSON_HEDLEY_EMPTY_BASES
#endif

/* Remaining macros are deprecated. */

#if defined(JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK)
    #undef JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK
#endif
#if defined(__clang__)
    #define JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK(major,minor,patch) (0)
#else
    #define JSON_HEDLEY_GCC_NOT_CLANG_VERSION_CHECK(major,minor,patch) JSON_HEDLEY_GCC_VERSION_CHECK(major,minor,patch)
#endif

#if defined(JSON_HEDLEY_CLANG_HAS_ATTRIBUTE)
    #undef JSON_HEDLEY_CLANG_HAS_ATTRIBUTE
#endif
#define JSON_HEDLEY_CLANG_HAS_ATTRIBUTE(attribute) JSON_HEDLEY_HAS_ATTRIBUTE(attribute)

#if defined(JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE)
    #undef JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE
#endif
#define JSON_HEDLEY_CLANG_HAS_CPP_ATTRIBUTE(attribute) JSON_HEDLEY_HAS_CPP_ATTRIBUTE(attribute)

#if defined(JSON_HEDLEY_CLANG_HAS_BUILTIN)
    #undef JSON_HEDLEY_CLANG_HAS_BUILTIN
#endif
#define JSON_HEDLEY_CLANG_HAS_BUILTIN(builtin) JSON_HEDLEY_HAS_BUILTIN(builtin)

#if defined(JSON_HEDLEY_CLANG_HAS_FEATURE)
    #undef JSON_HEDLEY_CLANG_HAS_FEATURE
#endif
#define JSON_HEDLEY_CLANG_HAS_FEATURE(feature) JSON_HEDLEY_HAS_FEATURE(feature)

#if defined(JSON_HEDLEY_CLANG_HAS_EXTENSION)
    #undef JSON_HEDLEY_CLANG_HAS_EXTENSION
#endif
#define JSON_HEDLEY_CLANG_HAS_EXTENSION(extension) JSON_HEDLEY_HAS_EXTENSION(extension)

#if defined(JSON_HEDLEY_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE)
    #undef JSON_HEDLEY_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE
#endif
#define JSON_HEDLEY_CLANG_HAS_DECLSPEC_ATTRIBUTE(attribute) JSON_HEDLEY_HAS_DECLSPEC_ATTRIBUTE(attribute)

#if defined(JSON_HEDLEY_CLANG_HAS_WARNING)
    #undef JSON_HEDLEY_CLANG_HAS_WARNING
#endif
#define JSON_HEDLEY_CLANG_HAS_WARNING(warning) JSON_HEDLEY_HAS_WARNING(warning)

#endif /* !defined(JSON_HEDLEY_VERSION) || (JSON_HEDLEY_VERSION < X) */


// This file contains all internal macro definitions
// You MUST include macro_unscope.hpp at the end of json.hpp to undef all of them

// exclude unsupported compilers
#if !defined(JSON_SKIP_UNSUPPORTED_COMPILER_CHECK)
    #if defined(__clang__)
        #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
            #error "unsupported Clang version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #elif defined(__GNUC__) && !(defined(__ICC) || defined(__INTEL_COMPILER))
        #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
            #error "unsupported GCC version - see https://github.com/nlohmann/json#supported-compilers"
        #endif
    #endif
#endif

// C++ language standard detection
// if the user manually specified the used c++ version this is skipped
#if !defined(JSON_HAS_CPP_20) && !defined(JSON_HAS_CPP_17) && !defined(JSON_HAS_CPP_14) && !defined(JSON_HAS_CPP_11)
    #if (defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
        #define JSON_HAS_CPP_20
        #define JSON_HAS_CPP_17
        #define JSON_HAS_CPP_14
    #elif (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
        #define JSON_HAS_CPP_17
        #define JSON_HAS_CPP_14
    #elif (defined(__cplusplus) && __cplusplus >= 201402L) || (defined(_HAS_CXX14) && _HAS_CXX14 == 1)
        #define JSON_HAS_CPP_14
    #endif
    // the cpp 11 flag is always specified because it is the minimal required version
    #define JSON_HAS_CPP_11
#endif

// disable documentation warnings on clang
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdocumentation"
#endif

// allow to disable exceptions
#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
    #define JSON_THROW(exception) throw exception
    #define JSON_TRY try
    #define JSON_CATCH(exception) catch(exception)
    #define JSON_INTERNAL_CATCH(exception) catch(exception)
#else
    #include <cstdlib>
    #define JSON_THROW(exception) std::abort()
    #define JSON_TRY if(true)
    #define JSON_CATCH(exception) if(false)
    #define JSON_INTERNAL_CATCH(exception) if(false)
#endif

// override exception macros
#if defined(JSON_THROW_USER)
    #undef JSON_THROW
    #define JSON_THROW JSON_THROW_USER
#endif
#if defined(JSON_TRY_USER)
    #undef JSON_TRY
    #define JSON_TRY JSON_TRY_USER
#endif
#if defined(JSON_CATCH_USER)
    #undef JSON_CATCH
    #define JSON_CATCH JSON_CATCH_USER
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_CATCH_USER
#endif
#if defined(JSON_INTERNAL_CATCH_USER)
    #undef JSON_INTERNAL_CATCH
    #define JSON_INTERNAL_CATCH JSON_INTERNAL_CATCH_USER
#endif

// allow to override assert
#if !defined(JSON_ASSERT)
    #include <cassert> // assert
    #define JSON_ASSERT(x) assert(x)
#endif

// allow to access some private functions (needed by the test suite)
#if defined(JSON_TESTS_PRIVATE)
    #define JSON_PRIVATE_UNLESS_TESTED public
#else
    #define JSON_PRIVATE_UNLESS_TESTED private
#endif

/*!
@brief macro to briefly define a mapping between an enum and JSON
@def NLOHMANN_JSON_SERIALIZE_ENUM
@since version 3.4.0
*/
#define NLOHMANN_JSON_SERIALIZE_ENUM(ENUM_TYPE, ...)                                            \
    template<typename BasicJsonType>                                                            \
    inline void to_json(BasicJsonType& j, const ENUM_TYPE& e)                                   \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [e](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool  \
        {                                                                                       \
            return ej_pair.first == e;                                                          \
        });                                                                                     \
        j = ((it != std::end(m)) ? it : std::begin(m))->second;                                 \
    }                                                                                           \
    template<typename BasicJsonType>                                                            \
    inline void from_json(const BasicJsonType& j, ENUM_TYPE& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!");          \
        static const std::pair<ENUM_TYPE, BasicJsonType> m[] = __VA_ARGS__;                     \
        auto it = std::find_if(std::begin(m), std::end(m),                                      \
                               [&j](const std::pair<ENUM_TYPE, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        });                                                                                     \
        e = ((it != std::end(m)) ? it : std::begin(m))->first;                                  \
    }

// Ugly macros to avoid uglier copy-paste when specializing basic_json. They
// may be removed in the future once the class is split.

#define NLOHMANN_BASIC_JSON_TPL_DECLARATION                                \
    template<template<typename, typename, typename...> class ObjectType,   \
             template<typename, typename...> class ArrayType,              \
             class StringType, class BooleanType, class NumberIntegerType, \
             class NumberUnsignedType, class NumberFloatType,              \
             template<typename> class AllocatorType,                       \
             template<typename, typename = void> class JSONSerializer,     \
             class BinaryType>

#define NLOHMANN_BASIC_JSON_TPL                                            \
    basic_json<ObjectType, ArrayType, StringType, BooleanType,             \
    NumberIntegerType, NumberUnsignedType, NumberFloatType,                \
    AllocatorType, JSONSerializer, BinaryType>

// Macros to simplify conversion from/to types

#define NLOHMANN_JSON_EXPAND( x ) x
#define NLOHMANN_JSON_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME,...) NAME
#define NLOHMANN_JSON_PASTE(...) NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_GET_MACRO(__VA_ARGS__, \
        NLOHMANN_JSON_PASTE64, \
        NLOHMANN_JSON_PASTE63, \
        NLOHMANN_JSON_PASTE62, \
        NLOHMANN_JSON_PASTE61, \
        NLOHMANN_JSON_PASTE60, \
        NLOHMANN_JSON_PASTE59, \
        NLOHMANN_JSON_PASTE58, \
        NLOHMANN_JSON_PASTE57, \
        NLOHMANN_JSON_PASTE56, \
        NLOHMANN_JSON_PASTE55, \
        NLOHMANN_JSON_PASTE54, \
        NLOHMANN_JSON_PASTE53, \
        NLOHMANN_JSON_PASTE52, \
        NLOHMANN_JSON_PASTE51, \
        NLOHMANN_JSON_PASTE50, \
        NLOHMANN_JSON_PASTE49, \
        NLOHMANN_JSON_PASTE48, \
        NLOHMANN_JSON_PASTE47, \
        NLOHMANN_JSON_PASTE46, \
        NLOHMANN_JSON_PASTE45, \
        NLOHMANN_JSON_PASTE44, \
        NLOHMANN_JSON_PASTE43, \
        NLOHMANN_JSON_PASTE42, \
        NLOHMANN_JSON_PASTE41, \
        NLOHMANN_JSON_PASTE40, \
        NLOHMANN_JSON_PASTE39, \
        NLOHMANN_JSON_PASTE38, \
        NLOHMANN_JSON_PASTE37, \
        NLOHMANN_JSON_PASTE36, \
        NLOHMANN_JSON_PASTE35, \
        NLOHMANN_JSON_PASTE34, \
        NLOHMANN_JSON_PASTE33, \
        NLOHMANN_JSON_PASTE32, \
        NLOHMANN_JSON_PASTE31, \
        NLOHMANN_JSON_PASTE30, \
        NLOHMANN_JSON_PASTE29, \
        NLOHMANN_JSON_PASTE28, \
        NLOHMANN_JSON_PASTE27, \
        NLOHMANN_JSON_PASTE26, \
        NLOHMANN_JSON_PASTE25, \
        NLOHMANN_JSON_PASTE24, \
        NLOHMANN_JSON_PASTE23, \
        NLOHMANN_JSON_PASTE22, \
        NLOHMANN_JSON_PASTE21, \
        NLOHMANN_JSON_PASTE20, \
        NLOHMANN_JSON_PASTE19, \
        NLOHMANN_JSON_PASTE18, \
        NLOHMANN_JSON_PASTE17, \
        NLOHMANN_JSON_PASTE16, \
        NLOHMANN_JSON_PASTE15, \
        NLOHMANN_JSON_PASTE14, \
        NLOHMANN_JSON_PASTE13, \
        NLOHMANN_JSON_PASTE12, \
        NLOHMANN_JSON_PASTE11, \
        NLOHMANN_JSON_PASTE10, \
        NLOHMANN_JSON_PASTE9, \
        NLOHMANN_JSON_PASTE8, \
        NLOHMANN_JSON_PASTE7, \
        NLOHMANN_JSON_PASTE6, \
        NLOHMANN_JSON_PASTE5, \
        NLOHMANN_JSON_PASTE4, \
        NLOHMANN_JSON_PASTE3, \
        NLOHMANN_JSON_PASTE2, \
        NLOHMANN_JSON_PASTE1)(__VA_ARGS__))
#define NLOHMANN_JSON_PASTE2(func, v1) func(v1)
#define NLOHMANN_JSON_PASTE3(func, v1, v2) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE2(func, v2)
#define NLOHMANN_JSON_PASTE4(func, v1, v2, v3) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE3(func, v2, v3)
#define NLOHMANN_JSON_PASTE5(func, v1, v2, v3, v4) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE4(func, v2, v3, v4)
#define NLOHMANN_JSON_PASTE6(func, v1, v2, v3, v4, v5) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE5(func, v2, v3, v4, v5)
#define NLOHMANN_JSON_PASTE7(func, v1, v2, v3, v4, v5, v6) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE6(func, v2, v3, v4, v5, v6)
#define NLOHMANN_JSON_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE7(func, v2, v3, v4, v5, v6, v7)
#define NLOHMANN_JSON_PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE8(func, v2, v3, v4, v5, v6, v7, v8)
#define NLOHMANN_JSON_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define NLOHMANN_JSON_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define NLOHMANN_JSON_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define NLOHMANN_JSON_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define NLOHMANN_JSON_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define NLOHMANN_JSON_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define NLOHMANN_JSON_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define NLOHMANN_JSON_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define NLOHMANN_JSON_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define NLOHMANN_JSON_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define NLOHMANN_JSON_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define NLOHMANN_JSON_PASTE21(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE20(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define NLOHMANN_JSON_PASTE22(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE21(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define NLOHMANN_JSON_PASTE23(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE22(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define NLOHMANN_JSON_PASTE24(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE23(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define NLOHMANN_JSON_PASTE25(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE24(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define NLOHMANN_JSON_PASTE26(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE25(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define NLOHMANN_JSON_PASTE27(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE26(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define NLOHMANN_JSON_PASTE28(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE27(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27)
#define NLOHMANN_JSON_PASTE29(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE28(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28)
#define NLOHMANN_JSON_PASTE30(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE29(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29)
#define NLOHMANN_JSON_PASTE31(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE30(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30)
#define NLOHMANN_JSON_PASTE32(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE31(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31)
#define NLOHMANN_JSON_PASTE33(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE32(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32)
#define NLOHMANN_JSON_PASTE34(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE33(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33)
#define NLOHMANN_JSON_PASTE35(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE34(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)
#define NLOHMANN_JSON_PASTE36(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE35(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define NLOHMANN_JSON_PASTE37(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE36(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define NLOHMANN_JSON_PASTE38(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE37(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define NLOHMANN_JSON_PASTE39(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE38(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define NLOHMANN_JSON_PASTE40(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE39(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define NLOHMANN_JSON_PASTE41(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE40(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define NLOHMANN_JSON_PASTE42(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE41(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define NLOHMANN_JSON_PASTE43(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE42(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define NLOHMANN_JSON_PASTE44(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE43(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define NLOHMANN_JSON_PASTE45(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE44(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define NLOHMANN_JSON_PASTE46(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE45(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define NLOHMANN_JSON_PASTE47(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE46(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define NLOHMANN_JSON_PASTE48(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE47(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define NLOHMANN_JSON_PASTE49(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE48(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define NLOHMANN_JSON_PASTE50(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE49(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define NLOHMANN_JSON_PASTE51(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE50(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define NLOHMANN_JSON_PASTE52(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE51(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define NLOHMANN_JSON_PASTE53(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE52(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52)
#define NLOHMANN_JSON_PASTE54(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE53(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53)
#define NLOHMANN_JSON_PASTE55(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE54(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54)
#define NLOHMANN_JSON_PASTE56(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE55(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55)
#define NLOHMANN_JSON_PASTE57(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE56(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56)
#define NLOHMANN_JSON_PASTE58(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE57(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)
#define NLOHMANN_JSON_PASTE59(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE58(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)
#define NLOHMANN_JSON_PASTE60(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE59(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)
#define NLOHMANN_JSON_PASTE61(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE60(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define NLOHMANN_JSON_PASTE62(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE61(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define NLOHMANN_JSON_PASTE63(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE62(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define NLOHMANN_JSON_PASTE64(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63) NLOHMANN_JSON_PASTE2(func, v1) NLOHMANN_JSON_PASTE63(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)

#define NLOHMANN_JSON_TO(v1) nlohmann_json_j[#v1] = nlohmann_json_t.v1;
#define NLOHMANN_JSON_FROM(v1) nlohmann_json_j.at(#v1).get_to(nlohmann_json_t.v1);

/*!
@brief macro
@def NLOHMANN_DEFINE_TYPE_INTRUSIVE
@since version 3.9.0
*/
#define NLOHMANN_DEFINE_TYPE_INTRUSIVE(Type, ...)  \
    friend void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    friend void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

/*!
@brief macro
@def NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
@since version 3.9.0
*/
#define NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Type, ...)  \
    inline void to_json(nlohmann::json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    inline void from_json(const nlohmann::json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

#ifndef JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_USE_IMPLICIT_CONVERSIONS 1
#endif

#if JSON_USE_IMPLICIT_CONVERSIONS
    #define JSON_EXPLICIT
#else
    #define JSON_EXPLICIT explicit
#endif


namespace nlohmann
{
namespace detail
{

/*!
@brief replace all occurrences of a substring by another string

@param[in,out] s  the string to manipulate; changed so that all
               occurrences of @a f are replaced with @a t
@param[in]     f  the substring to replace with @a t
@param[in]     t  the string to replace @a f

@pre The search string @a f must not be empty. **This precondition is
enforced with an assertion.**

@since version 2.0.0
*/
inline void replace_substring(std::string& s, const std::string& f,
                              const std::string& t)
{
    JSON_ASSERT(!f.empty());
    for (auto pos = s.find(f);                // find first occurrence of f
            pos != std::string::npos;         // make sure f was found
            s.replace(pos, f.size(), t),      // replace with t, and
            pos = s.find(f, pos + t.size()))  // find next occurrence of f
    {}
}

/*!
 * @brief string escaping as described in RFC 6901 (Sect. 4)
 * @param[in] s string to escape
 * @return    escaped string
 *
 * Note the order of escaping "~" to "~0" and "/" to "~1" is important.
 */
inline std::string escape(std::string s)
{
    replace_substring(s, "~", "~0");
    replace_substring(s, "/", "~1");
    return s;
}

/*!
 * @brief string unescaping as described in RFC 6901 (Sect. 4)
 * @param[in] s string to unescape
 * @return    unescaped string
 *
 * Note the order of escaping "~1" to "/" and "~0" to "~" is important.
 */
static void unescape(std::string& s)
{
    replace_substring(s, "~1", "/");
    replace_substring(s, "~0", "~");
}

} // namespace detail
} // namespace nlohmann

// #include <nlohmann/detail/input/position_t.hpp>


#include <cstddef> // size_t

namespace nlohmann
{
namespace detail
{
/// struct to capture the start position of the current token
struct position_t
{
    /// the total number of characters read
    std::size_t chars_read_total = 0;
    /// the number of characters read in the current line
    std::size_t chars_read_current_line = 0;
    /// the number of lines read
    std::size_t lines_read = 0;

    /// conversion to size_t to preserve SAX interface
    constexpr operator size_t() const
    {
        return chars_read_total;
    }
};

} // namespace detail
} // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
////////////////
// exceptions //
////////////////

/*!
@brief general exception of the @ref basic_json class

This class is an extension of `std::exception` objects with a member @a id for
exception ids. It is used as the base class for all exceptions thrown by the
@ref basic_json class. This class can hence be used as "wildcard" to catch
exceptions.

Subclasses:
- @ref parse_error for exceptions indicating a parse error
- @ref invalid_iterator for exceptions indicating errors with iterators
- @ref type_error for exceptions indicating executing a member function with
                  a wrong type
- @ref out_of_range for exceptions indicating access out of the defined range
- @ref other_error for exceptions indicating other library errors

@internal
@note To have nothrow-copy-constructible exceptions, we internally use
      `std::runtime_error` which can cope with arbitrary-length error messages.
      Intermediate strings are built with static functions and then passed to
      the actual constructor.
@endinternal

@liveexample{The following code shows how arbitrary library exceptions can be
caught.,exception}

@since version 3.0.0
*/
class exception : public std::exception
{
  public:
    /// returns the explanatory string
    const char* what() const noexcept override
    {
        return m.what();
    }

    /// the id of the exception
    const int id; // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)

  protected:
    JSON_HEDLEY_NON_NULL(3)
    exception(int id_, const char* what_arg) : id(id_), m(what_arg) {}

    static std::string name(const std::string& ename, int id_)
    {
        return "[json.exception." + ename + "." + std::to_string(id_) + "] ";
    }

    template<typename BasicJsonType>
    static std::string diagnostics(const BasicJsonType& leaf_element)
    {
#if JSON_DIAGNOSTICS
        std::vector<std::string> tokens;
        for (const auto* current = &leaf_element; current->m_parent != nullptr; current = current->m_parent)
        {
            switch (current->m_parent->type())
            {
                case value_t::array:
                {
                    for (std::size_t i = 0; i < current->m_parent->m_value.array->size(); ++i)
                    {
                        if (&current->m_parent->m_value.array->operator[](i) == current)
                        {
                            tokens.emplace_back(std::to_string(i));
                            break;
                        }
                    }
                    break;
                }

                case value_t::object:
                {
                    for (const auto& element : *current->m_parent->m_value.object)
                    {
                        if (&element.second == current)
                        {
                            tokens.emplace_back(element.first.c_str());
                            break;
                        }
                    }
                    break;
                }

                default:   // LCOV_EXCL_LINE
                    break; // LCOV_EXCL_LINE
            }
        }

        if (tokens.empty())
        {
            return "";
        }

        return "(" + std::accumulate(tokens.rbegin(), tokens.rend(), std::string{},
                                     [](const std::string & a, const std::string & b)
        {
            return a + "/" + detail::escape(b);
        }) + ") ";
#else
        static_cast<void>(leaf_element);
        return "";
#endif
    }

  private:
    /// an exception object as storage for error messages
    std::runtime_error m;
};

/*!
@brief exception indicating a parse error

This exception is thrown by the library when a parse error occurs. Parse errors
can occur during the deserialization of JSON text, CBOR, MessagePack, as well
as when using JSON Patch.

Member @a byte holds the byte index of the last read character in the input
file.

Exceptions have ids 1xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.parse_error.101 | parse error at 2: unexpected end of input; expected string literal | This error indicates a syntax error while deserializing a JSON text. The error message describes that an unexpected token (character) was encountered, and the member @a byte indicates the error position.
json.exception.parse_error.102 | parse error at 14: missing or wrong low surrogate | JSON uses the `\uxxxx` format to describe Unicode characters. Code points above above 0xFFFF are split into two `\uxxxx` entries ("surrogate pairs"). This error indicates that the surrogate pair is incomplete or contains an invalid code point.
json.exception.parse_error.103 | parse error: code points above 0x10FFFF are invalid | Unicode supports code points up to 0x10FFFF. Code points above 0x10FFFF are invalid.
json.exception.parse_error.104 | parse error: JSON patch must be an array of objects | [RFC 6902](https://tools.ietf.org/html/rfc6902) requires a JSON Patch document to be a JSON document that represents an array of objects.
json.exception.parse_error.105 | parse error: operation must have string member 'op' | An operation of a JSON Patch document must contain exactly one "op" member, whose value indicates the operation to perform. Its value must be one of "add", "remove", "replace", "move", "copy", or "test"; other values are errors.
json.exception.parse_error.106 | parse error: array index '01' must not begin with '0' | An array index in a JSON Pointer ([RFC 6901](https://tools.ietf.org/html/rfc6901)) may be `0` or any number without a leading `0`.
json.exception.parse_error.107 | parse error: JSON pointer must be empty or begin with '/' - was: 'foo' | A JSON Pointer must be a Unicode string containing a sequence of zero or more reference tokens, each prefixed by a `/` character.
json.exception.parse_error.108 | parse error: escape character '~' must be followed with '0' or '1' | In a JSON Pointer, only `~0` and `~1` are valid escape sequences.
json.exception.parse_error.109 | parse error: array index 'one' is not a number | A JSON Pointer array index must be a number.
json.exception.parse_error.110 | parse error at 1: cannot read 2 bytes from vector | When parsing CBOR or MessagePack, the byte vector ends before the complete value has been read.
json.exception.parse_error.112 | parse error at 1: error reading CBOR; last byte: 0xF8 | Not all types of CBOR or MessagePack are supported. This exception occurs if an unsupported byte was read.
json.exception.parse_error.113 | parse error at 2: expected a CBOR string; last byte: 0x98 | While parsing a map key, a value that is not a string has been read.
json.exception.parse_error.114 | parse error: Unsupported BSON record type 0x0F | The parsing of the corresponding BSON record type is not implemented (yet).
json.exception.parse_error.115 | parse error at byte 5: syntax error while parsing UBJSON high-precision number: invalid number text: 1A | A UBJSON high-precision number could not be parsed.

@note For an input with n bytes, 1 is the index of the first character and n+1
      is the index of the terminating null byte or the end of file. This also
      holds true when reading a byte vector (CBOR or MessagePack).

@liveexample{The following code shows how a `parse_error` exception can be
caught.,parse_error}

@sa - @ref exception for the base class of the library exceptions
@sa - @ref invalid_iterator for exceptions indicating errors with iterators
@sa - @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa - @ref out_of_range for exceptions indicating access out of the defined range
@sa - @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class parse_error : public exception
{
  public:
    /*!
    @brief create a parse error exception
    @param[in] id_       the id of the exception
    @param[in] pos       the position where the error occurred (or with
                         chars_read_total=0 if the position cannot be
                         determined)
    @param[in] what_arg  the explanatory string
    @return parse_error object
    */
    template<typename BasicJsonType>
    static parse_error create(int id_, const position_t& pos, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("parse_error", id_) + "parse error" +
                        position_string(pos) + ": " + exception::diagnostics(context) + what_arg;
        return parse_error(id_, pos.chars_read_total, w.c_str());
    }

    template<typename BasicJsonType>
    static parse_error create(int id_, std::size_t byte_, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("parse_error", id_) + "parse error" +
                        (byte_ != 0 ? (" at byte " + std::to_string(byte_)) : "") +
                        ": " + exception::diagnostics(context) + what_arg;
        return parse_error(id_, byte_, w.c_str());
    }

    /*!
    @brief byte index of the parse error

    The byte index of the last read character in the input file.

    @note For an input with n bytes, 1 is the index of the first character and
          n+1 is the index of the terminating null byte or the end of file.
          This also holds true when reading a byte vector (CBOR or MessagePack).
    */
    const std::size_t byte;

  private:
    parse_error(int id_, std::size_t byte_, const char* what_arg)
        : exception(id_, what_arg), byte(byte_) {}

    static std::string position_string(const position_t& pos)
    {
        return " at line " + std::to_string(pos.lines_read + 1) +
               ", column " + std::to_string(pos.chars_read_current_line);
    }
};

/*!
@brief exception indicating errors with iterators

This exception is thrown if iterators passed to a library function do not match
the expected semantics.

Exceptions have ids 2xx.

name / id                           | example message | description
----------------------------------- | --------------- | -------------------------
json.exception.invalid_iterator.201 | iterators are not compatible | The iterators passed to constructor @ref basic_json(InputIT first, InputIT last) are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.202 | iterator does not fit current value | In an erase or insert function, the passed iterator @a pos does not belong to the JSON value for which the function was called. It hence does not define a valid position for the deletion/insertion.
json.exception.invalid_iterator.203 | iterators do not fit current value | Either iterator passed to function @ref erase(IteratorType first, IteratorType last) does not belong to the JSON value from which values shall be erased. It hence does not define a valid range to delete values from.
json.exception.invalid_iterator.204 | iterators out of range | When an iterator range for a primitive type (number, boolean, or string) is passed to a constructor or an erase function, this range has to be exactly (@ref begin(), @ref end()), because this is the only way the single stored value is expressed. All other ranges are invalid.
json.exception.invalid_iterator.205 | iterator out of range | When an iterator for a primitive type (number, boolean, or string) is passed to an erase function, the iterator has to be the @ref begin() iterator, because it is the only way to address the stored value. All other iterators are invalid.
json.exception.invalid_iterator.206 | cannot construct with iterators from null | The iterators passed to constructor @ref basic_json(InputIT first, InputIT last) belong to a JSON null value and hence to not define a valid range.
json.exception.invalid_iterator.207 | cannot use key() for non-object iterators | The key() member function can only be used on iterators belonging to a JSON object, because other types do not have a concept of a key.
json.exception.invalid_iterator.208 | cannot use operator[] for object iterators | The operator[] to specify a concrete offset cannot be used on iterators belonging to a JSON object, because JSON objects are unordered.
json.exception.invalid_iterator.209 | cannot use offsets with object iterators | The offset operators (+, -, +=, -=) cannot be used on iterators belonging to a JSON object, because JSON objects are unordered.
json.exception.invalid_iterator.210 | iterators do not fit | The iterator range passed to the insert function are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.211 | passed iterators may not belong to container | The iterator range passed to the insert function must not be a subrange of the container to insert to.
json.exception.invalid_iterator.212 | cannot compare iterators of different containers | When two iterators are compared, they must belong to the same container.
json.exception.invalid_iterator.213 | cannot compare order of object iterators | The order of object iterators cannot be compared, because JSON objects are unordered.
json.exception.invalid_iterator.214 | cannot get value | Cannot get value for iterator: Either the iterator belongs to a null value or it is an iterator to a primitive type (number, boolean, or string), but the iterator is different to @ref begin().

@liveexample{The following code shows how an `invalid_iterator` exception can be
caught.,invalid_iterator}

@sa - @ref exception for the base class of the library exceptions
@sa - @ref parse_error for exceptions indicating a parse error
@sa - @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa - @ref out_of_range for exceptions indicating access out of the defined range
@sa - @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class invalid_iterator : public exception
{
  public:
    template<typename BasicJsonType>
    static invalid_iterator create(int id_, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("invalid_iterator", id_) + exception::diagnostics(context) + what_arg;
        return invalid_iterator(id_, w.c_str());
    }

  private:
    JSON_HEDLEY_NON_NULL(3)
    invalid_iterator(int id_, const char* what_arg)
        : exception(id_, what_arg) {}
};

/*!
@brief exception indicating executing a member function with a wrong type

This exception is thrown in case of a type error; that is, a library function is
executed on a JSON value whose type does not match the expected semantics.

Exceptions have ids 3xx.

name / id                     | example message | description
----------------------------- | --------------- | -------------------------
json.exception.type_error.301 | cannot create object from initializer list | To create an object from an initializer list, the initializer list must consist only of a list of pairs whose first element is a string. When this constraint is violated, an array is created instead.
json.exception.type_error.302 | type must be object, but is array | During implicit or explicit value conversion, the JSON type must be compatible to the target type. For instance, a JSON string can only be converted into string types, but not into numbers or boolean types.
json.exception.type_error.303 | incompatible ReferenceType for get_ref, actual type is object | To retrieve a reference to a value stored in a @ref basic_json object with @ref get_ref, the type of the reference must match the value type. For instance, for a JSON array, the @a ReferenceType must be @ref array_t &.
json.exception.type_error.304 | cannot use at() with string | The @ref at() member functions can only be executed for certain JSON types.
json.exception.type_error.305 | cannot use operator[] with string | The @ref operator[] member functions can only be executed for certain JSON types.
json.exception.type_error.306 | cannot use value() with string | The @ref value() member functions can only be executed for certain JSON types.
json.exception.type_error.307 | cannot use erase() with string | The @ref erase() member functions can only be executed for certain JSON types.
json.exception.type_error.308 | cannot use push_back() with string | The @ref push_back() and @ref operator+= member functions can only be executed for certain JSON types.
json.exception.type_error.309 | cannot use insert() with | The @ref insert() member functions can only be executed for certain JSON types.
json.exception.type_error.310 | cannot use swap() with number | The @ref swap() member functions can only be executed for certain JSON types.
json.exception.type_error.311 | cannot use emplace_back() with string | The @ref emplace_back() member function can only be executed for certain JSON types.
json.exception.type_error.312 | cannot use update() with string | The @ref update() member functions can only be executed for certain JSON types.
json.exception.type_error.313 | invalid value to unflatten | The @ref unflatten function converts an object whose keys are JSON Pointers back into an arbitrary nested JSON value. The JSON Pointers must not overlap, because then the resulting value would not be well defined.
json.exception.type_error.314 | only objects can be unflattened | The @ref unflatten function only works for an object whose keys are JSON Pointers.
json.exception.type_error.315 | values in object must be primitive | The @ref unflatten function only works for an object whose keys are JSON Pointers and whose values are primitive.
json.exception.type_error.316 | invalid UTF-8 byte at index 10: 0x7E | The @ref dump function only works with UTF-8 encoded strings; that is, if you assign a `std::string` to a JSON value, make sure it is UTF-8 encoded. |
json.exception.type_error.317 | JSON value cannot be serialized to requested format | The dynamic type of the object cannot be represented in the requested serialization format (e.g. a raw `true` or `null` JSON object cannot be serialized to BSON) |

@liveexample{The following code shows how a `type_error` exception can be
caught.,type_error}

@sa - @ref exception for the base class of the library exceptions
@sa - @ref parse_error for exceptions indicating a parse error
@sa - @ref invalid_iterator for exceptions indicating errors with iterators
@sa - @ref out_of_range for exceptions indicating access out of the defined range
@sa - @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class type_error : public exception
{
  public:
    template<typename BasicJsonType>
    static type_error create(int id_, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("type_error", id_) + exception::diagnostics(context) + what_arg;
        return type_error(id_, w.c_str());
    }

  private:
    JSON_HEDLEY_NON_NULL(3)
    type_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
};

/*!
@brief exception indicating access out of the defined range

This exception is thrown in case a library function is called on an input
parameter that exceeds the expected range, for instance in case of array
indices or nonexisting object keys.

Exceptions have ids 4xx.

name / id                       | example message | description
------------------------------- | --------------- | -------------------------
json.exception.out_of_range.401 | array index 3 is out of range | The provided array index @a i is larger than @a size-1.
json.exception.out_of_range.402 | array index '-' (3) is out of range | The special array index `-` in a JSON Pointer never describes a valid element of the array, but the index past the end. That is, it can only be used to add elements at this position, but not to read it.
json.exception.out_of_range.403 | key 'foo' not found | The provided key was not found in the JSON object.
json.exception.out_of_range.404 | unresolved reference token 'foo' | A reference token in a JSON Pointer could not be resolved.
json.exception.out_of_range.405 | JSON pointer has no parent | The JSON Patch operations 'remove' and 'add' can not be applied to the root element of the JSON value.
json.exception.out_of_range.406 | number overflow parsing '10E1000' | A parsed number could not be stored as without changing it to NaN or INF.
json.exception.out_of_range.407 | number overflow serializing '9223372036854775808' | UBJSON and BSON only support integer numbers up to 9223372036854775807. (until version 3.8.0) |
json.exception.out_of_range.408 | excessive array size: 8658170730974374167 | The size (following `#`) of an UBJSON array or object exceeds the maximal capacity. |
json.exception.out_of_range.409 | BSON key cannot contain code point U+0000 (at byte 2) | Key identifiers to be serialized to BSON cannot contain code point U+0000, since the key is stored as zero-terminated c-string |

@liveexample{The following code shows how an `out_of_range` exception can be
caught.,out_of_range}

@sa - @ref exception for the base class of the library exceptions
@sa - @ref parse_error for exceptions indicating a parse error
@sa - @ref invalid_iterator for exceptions indicating errors with iterators
@sa - @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa - @ref other_error for exceptions indicating other library errors

@since version 3.0.0
*/
class out_of_range : public exception
{
  public:
    template<typename BasicJsonType>
    static out_of_range create(int id_, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("out_of_range", id_) + exception::diagnostics(context) + what_arg;
        return out_of_range(id_, w.c_str());
    }

  private:
    JSON_HEDLEY_NON_NULL(3)
    out_of_range(int id_, const char* what_arg) : exception(id_, what_arg) {}
};

/*!
@brief exception indicating other library errors

This exception is thrown in case of errors that cannot be classified with the
other exception types.

Exceptions have ids 5xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.other_error.501 | unsuccessful: {"op":"test","path":"/baz", "value":"bar"} | A JSON Patch operation 'test' failed. The unsuccessful operation is also printed.

@sa - @ref exception for the base class of the library exceptions
@sa - @ref parse_error for exceptions indicating a parse error
@sa - @ref invalid_iterator for exceptions indicating errors with iterators
@sa - @ref type_error for exceptions indicating executing a member function with
                    a wrong type
@sa - @ref out_of_range for exceptions indicating access out of the defined range

@liveexample{The following code shows how an `other_error` exception can be
caught.,other_error}

@since version 3.0.0
*/
class other_error : public exception
{
  public:
    template<typename BasicJsonType>
    static other_error create(int id_, const std::string& what_arg, const BasicJsonType& context)
    {
        std::string w = exception::name("other_error", id_) + exception::diagnostics(context) + what_arg;
        return other_error(id_, w.c_str());
    }

  private:
    JSON_HEDLEY_NON_NULL(3)
    other_error(int id_, const char* what_arg) : exception(id_, what_arg) {}
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/cpp_future.hpp>


#include <cstddef> // size_t
#include <type_traits> // conditional, enable_if, false_type, integral_constant, is_constructible, is_integral, is_same, remove_cv, remove_reference, true_type
#include <utility> // index_sequence, make_index_sequence, index_sequence_for

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

#ifdef JSON_HAS_CPP_14

// the following utilities are natively available in C++14
using std::enable_if_t;
using std::index_sequence;
using std::make_index_sequence;
using std::index_sequence_for;

#else

// alias templates to reduce boilerplate
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

// The following code is taken from https://github.com/abseil/abseil-cpp/blob/10cb35e459f5ecca5b2ff107635da0bfa41011b4/absl/utility/utility.h
// which is part of Google Abseil (https://github.com/abseil/abseil-cpp), licensed under the Apache License 2.0.

//// START OF CODE FROM GOOGLE ABSEIL

// integer_sequence
//
// Class template representing a compile-time integer sequence. An instantiation
// of `integer_sequence<T, Ints...>` has a sequence of integers encoded in its
// type through its template arguments (which is a common need when
// working with C++11 variadic templates). `absl::integer_sequence` is designed
// to be a drop-in replacement for C++14's `std::integer_sequence`.
//
// Example:
//
//   template< class T, T... Ints >
//   void user_function(integer_sequence<T, Ints...>);
//
//   int main()
//   {
//     // user_function's `T` will be deduced to `int` and `Ints...`
//     // will be deduced to `0, 1, 2, 3, 4`.
//     user_function(make_integer_sequence<int, 5>());
//   }
template <typename T, T... Ints>
struct integer_sequence
{
    using value_type = T;
    static constexpr std::size_t size() noexcept
    {
        return sizeof...(Ints);
    }
};

// index_sequence
//
// A helper template for an `integer_sequence` of `size_t`,
// `absl::index_sequence` is designed to be a drop-in replacement for C++14's
// `std::index_sequence`.
template <size_t... Ints>
using index_sequence = integer_sequence<size_t, Ints...>;

namespace utility_internal
{

template <typename Seq, size_t SeqSize, size_t Rem>
struct Extend;

// Note that SeqSize == sizeof...(Ints). It's passed explicitly for efficiency.
template <typename T, T... Ints, size_t SeqSize>
struct Extend<integer_sequence<T, Ints...>, SeqSize, 0>
{
    using type = integer_sequence < T, Ints..., (Ints + SeqSize)... >;
};

template <typename T, T... Ints, size_t SeqSize>
struct Extend<integer_sequence<T, Ints...>, SeqSize, 1>
{
    using type = integer_sequence < T, Ints..., (Ints + SeqSize)..., 2 * SeqSize >;
};

// Recursion helper for 'make_integer_sequence<T, N>'.
// 'Gen<T, N>::type' is an alias for 'integer_sequence<T, 0, 1, ... N-1>'.
template <typename T, size_t N>
struct Gen
{
    using type =
        typename Extend < typename Gen < T, N / 2 >::type, N / 2, N % 2 >::type;
};

template <typename T>
struct Gen<T, 0>
{
    using type = integer_sequence<T>;
};

}  // namespace utility_internal

// Compile-time sequences of integers

// make_integer_sequence
//
// This template alias is equivalent to
// `integer_sequence<int, 0, 1, ..., N-1>`, and is designed to be a drop-in
// replacement for C++14's `std::make_integer_sequence`.
template <typename T, T N>
using make_integer_sequence = typename utility_internal::Gen<T, N>::type;

// make_index_sequence
//
// This template alias is equivalent to `index_sequence<0, 1, ..., N-1>`,
// and is designed to be a drop-in replacement for C++14's
// `std::make_index_sequence`.
template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

// index_sequence_for
//
// Converts a typename pack into an index sequence of the same length, and
// is designed to be a drop-in replacement for C++14's
// `std::index_sequence_for()`
template <typename... Ts>
using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

//// END OF CODE FROM GOOGLE ABSEIL

#endif

// dispatch utility (taken from ranges-v3)
template<unsigned N> struct priority_tag : priority_tag < N - 1 > {};
template<> struct priority_tag<0> {};

// taken from ranges-v3
template<typename T>
struct static_const
{
    static constexpr T value{};
};

template<typename T>
constexpr T static_const<T>::value;

}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/meta/identity_tag.hpp>


namespace nlohmann
{
namespace detail
{
// dispatching helper struct
template <class T> struct identity_tag {};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/meta/type_traits.hpp>


#include <limits> // numeric_limits
#include <type_traits> // false_type, is_constructible, is_integral, is_same, true_type
#include <utility> // declval
#include <tuple> // tuple

// #include <nlohmann/detail/iterators/iterator_traits.hpp>


#include <iterator> // random_access_iterator_tag

// #include <nlohmann/detail/meta/void_t.hpp>


namespace nlohmann
{
namespace detail
{
template<typename ...Ts> struct make_void
{
    using type = void;
};
template<typename ...Ts> using void_t = typename make_void<Ts...>::type;
} // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/meta/cpp_future.hpp>


namespace nlohmann
{
namespace detail
{
template<typename It, typename = void>
struct iterator_types {};

template<typename It>
struct iterator_types <
    It,
    void_t<typename It::difference_type, typename It::value_type, typename It::pointer,
    typename It::reference, typename It::iterator_category >>
{
    using difference_type = typename It::difference_type;
    using value_type = typename It::value_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;
    using iterator_category = typename It::iterator_category;
};

// This is required as some compilers implement std::iterator_traits in a way that
// doesn't work with SFINAE. See https://github.com/nlohmann/json/issues/1341.
template<typename T, typename = void>
struct iterator_traits
{
};

template<typename T>
struct iterator_traits < T, enable_if_t < !std::is_pointer<T>::value >>
            : iterator_types<T>
{
};

template<typename T>
struct iterator_traits<T*, enable_if_t<std::is_object<T>::value>>
{
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};
} // namespace detail
} // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/detected.hpp>


#include <type_traits>

// #include <nlohmann/detail/meta/void_t.hpp>


// https://en.cppreference.com/w/cpp/experimental/is_detected
namespace nlohmann
{
namespace detail
{
struct nonesuch
{
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    nonesuch(nonesuch const&&) = delete;
    void operator=(nonesuch const&) = delete;
    void operator=(nonesuch&&) = delete;
};

template<class Default,
         class AlwaysVoid,
         template<class...> class Op,
         class... Args>
struct detector
{
    using value_t = std::false_type;
    using type = Default;
};

template<class Default, template<class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...>
{
    using value_t = std::true_type;
    using type = Op<Args...>;
};

template<template<class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template<template<class...> class Op, class... Args>
struct is_detected_lazy : is_detected<Op, Args...> { };

template<template<class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

template<class Default, template<class...> class Op, class... Args>
using detected_or = detector<Default, void, Op, Args...>;

template<class Default, template<class...> class Op, class... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

template<class Expected, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template<class To, template<class...> class Op, class... Args>
using is_detected_convertible =
    std::is_convertible<detected_t<Op, Args...>, To>;
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/json_fwd.hpp>
#ifndef INCLUDE_NLOHMANN_JSON_FWD_HPP_
#define INCLUDE_NLOHMANN_JSON_FWD_HPP_

#include <cstdint> // int64_t, uint64_t
#include <map> // map
#include <memory> // allocator
#include <string> // string
#include <vector> // vector

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
namespace nlohmann
{
/*!
@brief default JSONSerializer template argument

This serializer ignores the template arguments and uses ADL
([argument-dependent lookup](https://en.cppreference.com/w/cpp/language/adl))
for serialization.
*/
template<typename T = void, typename SFINAE = void>
struct adl_serializer;

template<template<typename U, typename V, typename... Args> class ObjectType =
         std::map,
         template<typename U, typename... Args> class ArrayType = std::vector,
         class StringType = std::string, class BooleanType = bool,
         class NumberIntegerType = std::int64_t,
         class NumberUnsignedType = std::uint64_t,
         class NumberFloatType = double,
         template<typename U> class AllocatorType = std::allocator,
         template<typename T, typename SFINAE = void> class JSONSerializer =
         adl_serializer,
         class BinaryType = std::vector<std::uint8_t>>
class basic_json;

/*!
@brief JSON Pointer

A JSON pointer defines a string syntax for identifying a specific value
within a JSON document. It can be used with functions `at` and
`operator[]`. Furthermore, JSON pointers are the base for JSON patches.

@sa [RFC 6901](https://tools.ietf.org/html/rfc6901)

@since version 2.0.0
*/
template<typename BasicJsonType>
class json_pointer;

/*!
@brief default JSON class

This type is the default specialization of the @ref basic_json class which
uses the standard template types.

@since version 1.0.0
*/
using json = basic_json<>;

template<class Key, class T, class IgnoredLess, class Allocator>
struct ordered_map;

/*!
@brief ordered JSON class

This type preserves the insertion order of object keys.

@since version 3.9.0
*/
using ordered_json = basic_json<nlohmann::ordered_map>;

}  // namespace nlohmann

#endif  // INCLUDE_NLOHMANN_JSON_FWD_HPP_


namespace nlohmann
{
/*!
@brief detail namespace with internal helper functions

This namespace collects functions that should not be exposed,
implementations of some @ref basic_json methods, and meta-programming helpers.

@since version 2.1.0
*/
namespace detail
{
/////////////
// helpers //
/////////////

// Note to maintainers:
//
// Every trait in this file expects a non CV-qualified type.
// The only exceptions are in the 'aliases for detected' section
// (i.e. those of the form: decltype(T::member_function(std::declval<T>())))
//
// In this case, T has to be properly CV-qualified to constraint the function arguments
// (e.g. to_json(BasicJsonType&, const T&))

template<typename> struct is_basic_json : std::false_type {};

NLOHMANN_BASIC_JSON_TPL_DECLARATION
struct is_basic_json<NLOHMANN_BASIC_JSON_TPL> : std::true_type {};

//////////////////////
// json_ref helpers //
//////////////////////

template<typename>
class json_ref;

template<typename>
struct is_json_ref : std::false_type {};

template<typename T>
struct is_json_ref<json_ref<T>> : std::true_type {};

//////////////////////////
// aliases for detected //
//////////////////////////

template<typename T>
using mapped_type_t = typename T::mapped_type;

template<typename T>
using key_type_t = typename T::key_type;

template<typename T>
using value_type_t = typename T::value_type;

template<typename T>
using difference_type_t = typename T::difference_type;

template<typename T>
using pointer_t = typename T::pointer;

template<typename T>
using reference_t = typename T::reference;

template<typename T>
using iterator_category_t = typename T::iterator_category;

template<typename T>
using iterator_t = typename T::iterator;

template<typename T, typename... Args>
using to_json_function = decltype(T::to_json(std::declval<Args>()...));

template<typename T, typename... Args>
using from_json_function = decltype(T::from_json(std::declval<Args>()...));

template<typename T, typename U>
using get_template_function = decltype(std::declval<T>().template get<U>());

// trait checking if JSONSerializer<T>::from_json(json const&, udt&) exists
template<typename BasicJsonType, typename T, typename = void>
struct has_from_json : std::false_type {};

// trait checking if j.get<T> is valid
// use this trait instead of std::is_constructible or std::is_convertible,
// both rely on, or make use of implicit conversions, and thus fail when T
// has several constructors/operator= (see https://github.com/nlohmann/json/issues/958)
template <typename BasicJsonType, typename T>
struct is_getable
{
    static constexpr bool value = is_detected<get_template_function, const BasicJsonType&, T>::value;
};

template<typename BasicJsonType, typename T>
struct has_from_json < BasicJsonType, T, enable_if_t < !is_basic_json<T>::value >>
{
    using serializer = typename BasicJsonType::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<void, from_json_function, serializer,
        const BasicJsonType&, T&>::value;
};

// This trait checks if JSONSerializer<T>::from_json(json const&) exists
// this overload is used for non-default-constructible user-defined-types
template<typename BasicJsonType, typename T, typename = void>
struct has_non_default_from_json : std::false_type {};

template<typename BasicJsonType, typename T>
struct has_non_default_from_json < BasicJsonType, T, enable_if_t < !is_basic_json<T>::value >>
{
    using serializer = typename BasicJsonType::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<T, from_json_function, serializer,
        const BasicJsonType&>::value;
};

// This trait checks if BasicJsonType::json_serializer<T>::to_json exists
// Do not evaluate the trait when T is a basic_json type, to avoid template instantiation infinite recursion.
template<typename BasicJsonType, typename T, typename = void>
struct has_to_json : std::false_type {};

template<typename BasicJsonType, typename T>
struct has_to_json < BasicJsonType, T, enable_if_t < !is_basic_json<T>::value >>
{
    using serializer = typename BasicJsonType::template json_serializer<T, void>;

    static constexpr bool value =
        is_detected_exact<void, to_json_function, serializer, BasicJsonType&,
        T>::value;
};


///////////////////
// is_ functions //
///////////////////

// https://en.cppreference.com/w/cpp/types/conjunction
template<class...> struct conjunction : std::true_type { };
template<class B1> struct conjunction<B1> : B1 { };
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
: std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

// https://en.cppreference.com/w/cpp/types/negation
template<class B> struct negation : std::integral_constant < bool, !B::value > { };

// Reimplementation of is_constructible and is_default_constructible, due to them being broken for
// std::pair and std::tuple until LWG 2367 fix (see https://cplusplus.github.io/LWG/lwg-defects.html#2367).
// This causes compile errors in e.g. clang 3.5 or gcc 4.9.
template <typename T>
struct is_default_constructible : std::is_default_constructible<T> {};

template <typename T1, typename T2>
struct is_default_constructible<std::pair<T1, T2>>
            : conjunction<is_default_constructible<T1>, is_default_constructible<T2>> {};

template <typename T1, typename T2>
struct is_default_constructible<const std::pair<T1, T2>>
            : conjunction<is_default_constructible<T1>, is_default_constructible<T2>> {};

template <typename... Ts>
struct is_default_constructible<std::tuple<Ts...>>
            : conjunction<is_default_constructible<Ts>...> {};

template <typename... Ts>
struct is_default_constructible<const std::tuple<Ts...>>
            : conjunction<is_default_constructible<Ts>...> {};


template <typename T, typename... Args>
struct is_constructible : std::is_constructible<T, Args...> {};

template <typename T1, typename T2>
struct is_constructible<std::pair<T1, T2>> : is_default_constructible<std::pair<T1, T2>> {};

template <typename T1, typename T2>
struct is_constructible<const std::pair<T1, T2>> : is_default_constructible<const std::pair<T1, T2>> {};

template <typename... Ts>
struct is_constructible<std::tuple<Ts...>> : is_default_constructible<std::tuple<Ts...>> {};

template <typename... Ts>
struct is_constructible<const std::tuple<Ts...>> : is_default_constructible<const std::tuple<Ts...>> {};


template<typename T, typename = void>
struct is_iterator_traits : std::false_type {};

template<typename T>
struct is_iterator_traits<iterator_traits<T>>
{
  private:
    using traits = iterator_traits<T>;

  public:
    static constexpr auto value =
        is_detected<value_type_t, traits>::value &&
        is_detected<difference_type_t, traits>::value &&
        is_detected<pointer_t, traits>::value &&
        is_detected<iterator_category_t, traits>::value &&
        is_detected<reference_t, traits>::value;
};

// The following implementation of is_complete_type is taken from
// https://blogs.msdn.microsoft.com/vcblog/2015/12/02/partial-support-for-expression-sfinae-in-vs-2015-update-1/
// and is written by Xiang Fan who agreed to using it in this library.

template<typename T, typename = void>
struct is_complete_type : std::false_type {};

template<typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type {};

template<typename BasicJsonType, typename CompatibleObjectType,
         typename = void>
struct is_compatible_object_type_impl : std::false_type {};

template<typename BasicJsonType, typename CompatibleObjectType>
struct is_compatible_object_type_impl <
    BasicJsonType, CompatibleObjectType,
    enable_if_t < is_detected<mapped_type_t, CompatibleObjectType>::value&&
    is_detected<key_type_t, CompatibleObjectType>::value >>
{
    using object_t = typename BasicJsonType::object_t;

    // macOS's is_constructible does not play well with nonesuch...
    static constexpr bool value =
        is_constructible<typename object_t::key_type,
        typename CompatibleObjectType::key_type>::value &&
        is_constructible<typename object_t::mapped_type,
        typename CompatibleObjectType::mapped_type>::value;
};

template<typename BasicJsonType, typename CompatibleObjectType>
struct is_compatible_object_type
    : is_compatible_object_type_impl<BasicJsonType, CompatibleObjectType> {};

template<typename BasicJsonType, typename ConstructibleObjectType,
         typename = void>
struct is_constructible_object_type_impl : std::false_type {};

template<typename BasicJsonType, typename ConstructibleObjectType>
struct is_constructible_object_type_impl <
    BasicJsonType, ConstructibleObjectType,
    enable_if_t < is_detected<mapped_type_t, ConstructibleObjectType>::value&&
    is_detected<key_type_t, ConstructibleObjectType>::value >>
{
    using object_t = typename BasicJsonType::object_t;

    static constexpr bool value =
        (is_default_constructible<ConstructibleObjectType>::value &&
         (std::is_move_assignable<ConstructibleObjectType>::value ||
          std::is_copy_assignable<ConstructibleObjectType>::value) &&
         (is_constructible<typename ConstructibleObjectType::key_type,
          typename object_t::key_type>::value &&
          std::is_same <
          typename object_t::mapped_type,
          typename ConstructibleObjectType::mapped_type >::value)) ||
        (has_from_json<BasicJsonType,
         typename ConstructibleObjectType::mapped_type>::value ||
         has_non_default_from_json <
         BasicJsonType,
         typename ConstructibleObjectType::mapped_type >::value);
};

template<typename BasicJsonType, typename ConstructibleObjectType>
struct is_constructible_object_type
    : is_constructible_object_type_impl<BasicJsonType,
      ConstructibleObjectType> {};

template<typename BasicJsonType, typename CompatibleStringType,
         typename = void>
struct is_compatible_string_type_impl : std::false_type {};

template<typename BasicJsonType, typename CompatibleStringType>
struct is_compatible_string_type_impl <
    BasicJsonType, CompatibleStringType,
    enable_if_t<is_detected_exact<typename BasicJsonType::string_t::value_type,
    value_type_t, CompatibleStringType>::value >>
{
    static constexpr auto value =
        is_constructible<typename BasicJsonType::string_t, CompatibleStringType>::value;
};

template<typename BasicJsonType, typename ConstructibleStringType>
struct is_compatible_string_type
    : is_compatible_string_type_impl<BasicJsonType, ConstructibleStringType> {};

template<typename BasicJsonType, typename ConstructibleStringType,
         typename = void>
struct is_constructible_string_type_impl : std::false_type {};

template<typename BasicJsonType, typename ConstructibleStringType>
struct is_constructible_string_type_impl <
    BasicJsonType, ConstructibleStringType,
    enable_if_t<is_detected_exact<typename BasicJsonType::string_t::value_type,
    value_type_t, ConstructibleStringType>::value >>
{
    static constexpr auto value =
        is_constructible<ConstructibleStringType,
        typename BasicJsonType::string_t>::value;
};

template<typename BasicJsonType, typename ConstructibleStringType>
struct is_constructible_string_type
    : is_constructible_string_type_impl<BasicJsonType, ConstructibleStringType> {};

template<typename BasicJsonType, typename CompatibleArrayType, typename = void>
struct is_compatible_array_type_impl : std::false_type {};

template<typename BasicJsonType, typename CompatibleArrayType>
struct is_compatible_array_type_impl <
    BasicJsonType, CompatibleArrayType,
    enable_if_t < is_detected<value_type_t, CompatibleArrayType>::value&&
    is_detected<iterator_t, CompatibleArrayType>::value&&
// This is needed because json_reverse_iterator has a ::iterator type...
// Therefore it is detected as a CompatibleArrayType.
// The real fix would be to have an Iterable concept.
    !is_iterator_traits <
    iterator_traits<CompatibleArrayType >>::value >>
{
    static constexpr bool value =
        is_constructible<BasicJsonType,
        typename CompatibleArrayType::value_type>::value;
};

template<typename BasicJsonType, typename CompatibleArrayType>
struct is_compatible_array_type
    : is_compatible_array_type_impl<BasicJsonType, CompatibleArrayType> {};

template<typename BasicJsonType, typename ConstructibleArrayType, typename = void>
struct is_constructible_array_type_impl : std::false_type {};

template<typename BasicJsonType, typename ConstructibleArrayType>
struct is_constructible_array_type_impl <
    BasicJsonType, ConstructibleArrayType,
    enable_if_t<std::is_same<ConstructibleArrayType,
    typename BasicJsonType::value_type>::value >>
            : std::true_type {};

template<typename BasicJsonType, typename ConstructibleArrayType>
struct is_constructible_array_type_impl <
    BasicJsonType, ConstructibleArrayType,
    enable_if_t < !std::is_same<ConstructibleArrayType,
    typename BasicJsonType::value_type>::value&&
    is_default_constructible<ConstructibleArrayType>::value&&
(std::is_move_assignable<ConstructibleArrayType>::value ||
 std::is_copy_assignable<ConstructibleArrayType>::value)&&
is_detected<value_type_t, ConstructibleArrayType>::value&&
is_detected<iterator_t, ConstructibleArrayType>::value&&
is_complete_type <
detected_t<value_type_t, ConstructibleArrayType >>::value >>
{
    static constexpr bool value =
        // This is needed because json_reverse_iterator has a ::iterator type,
        // furthermore, std::back_insert_iterator (and other iterators) have a
        // base class `iterator`... Therefore it is detected as a
        // ConstructibleArrayType. The real fix would be to have an Iterable
        // concept.
        !is_iterator_traits<iterator_traits<ConstructibleArrayType>>::value &&

        (std::is_same<typename ConstructibleArrayType::value_type,
         typename BasicJsonType::array_t::value_type>::value ||
         has_from_json<BasicJsonType,
         typename ConstructibleArrayType::value_type>::value ||
         has_non_default_from_json <
         BasicJsonType, typename ConstructibleArrayType::value_type >::value);
};

template<typename BasicJsonType, typename ConstructibleArrayType>
struct is_constructible_array_type
    : is_constructible_array_type_impl<BasicJsonType, ConstructibleArrayType> {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType,
         typename = void>
struct is_compatible_integer_type_impl : std::false_type {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type_impl <
    RealIntegerType, CompatibleNumberIntegerType,
    enable_if_t < std::is_integral<RealIntegerType>::value&&
    std::is_integral<CompatibleNumberIntegerType>::value&&
    !std::is_same<bool, CompatibleNumberIntegerType>::value >>
{
    // is there an assert somewhere on overflows?
    using RealLimits = std::numeric_limits<RealIntegerType>;
    using CompatibleLimits = std::numeric_limits<CompatibleNumberIntegerType>;

    static constexpr auto value =
        is_constructible<RealIntegerType,
        CompatibleNumberIntegerType>::value &&
        CompatibleLimits::is_integer &&
        RealLimits::is_signed == CompatibleLimits::is_signed;
};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
    : is_compatible_integer_type_impl<RealIntegerType,
      CompatibleNumberIntegerType> {};

template<typename BasicJsonType, typename CompatibleType, typename = void>
struct is_compatible_type_impl: std::false_type {};

template<typename BasicJsonType, typename CompatibleType>
struct is_compatible_type_impl <
    BasicJsonType, CompatibleType,
    enable_if_t<is_complete_type<CompatibleType>::value >>
{
    static constexpr bool value =
        has_to_json<BasicJsonType, CompatibleType>::value;
};

template<typename BasicJsonType, typename CompatibleType>
struct is_compatible_type
    : is_compatible_type_impl<BasicJsonType, CompatibleType> {};

template<typename T1, typename T2>
struct is_constructible_tuple : std::false_type {};

template<typename T1, typename... Args>
struct is_constructible_tuple<T1, std::tuple<Args...>> : conjunction<is_constructible<T1, Args>...> {};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{
template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename std::nullptr_t& n)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_null()))
    {
        JSON_THROW(type_error::create(302, "type must be null, but is " + std::string(j.type_name()), j));
    }
    n = nullptr;
}

// overloads for basic_json template parameters
template < typename BasicJsonType, typename ArithmeticType,
           enable_if_t < std::is_arithmetic<ArithmeticType>::value&&
                         !std::is_same<ArithmeticType, typename BasicJsonType::boolean_t>::value,
                         int > = 0 >
void get_arithmetic_value(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_unsigned_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_integer_t*>());
            break;
        }
        case value_t::number_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_float_t*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name()), j));
    }
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::boolean_t& b)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_boolean()))
    {
        JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(j.type_name()), j));
    }
    b = *j.template get_ptr<const typename BasicJsonType::boolean_t*>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::string_t& s)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name()), j));
    }
    s = *j.template get_ptr<const typename BasicJsonType::string_t*>();
}

template <
    typename BasicJsonType, typename ConstructibleStringType,
    enable_if_t <
        is_constructible_string_type<BasicJsonType, ConstructibleStringType>::value&&
        !std::is_same<typename BasicJsonType::string_t,
                      ConstructibleStringType>::value,
        int > = 0 >
void from_json(const BasicJsonType& j, ConstructibleStringType& s)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_string()))
    {
        JSON_THROW(type_error::create(302, "type must be string, but is " + std::string(j.type_name()), j));
    }

    s = *j.template get_ptr<const typename BasicJsonType::string_t*>();
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::number_float_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::number_unsigned_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::number_integer_t& val)
{
    get_arithmetic_value(j, val);
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void from_json(const BasicJsonType& j, EnumType& e)
{
    typename std::underlying_type<EnumType>::type val;
    get_arithmetic_value(j, val);
    e = static_cast<EnumType>(val);
}

// forward_list doesn't have an insert method
template<typename BasicJsonType, typename T, typename Allocator,
         enable_if_t<is_getable<BasicJsonType, T>::value, int> = 0>
void from_json(const BasicJsonType& j, std::forward_list<T, Allocator>& l)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }
    l.clear();
    std::transform(j.rbegin(), j.rend(),
                   std::front_inserter(l), [](const BasicJsonType & i)
    {
        return i.template get<T>();
    });
}

// valarray doesn't have an insert method
template<typename BasicJsonType, typename T,
         enable_if_t<is_getable<BasicJsonType, T>::value, int> = 0>
void from_json(const BasicJsonType& j, std::valarray<T>& l)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }
    l.resize(j.size());
    std::transform(j.begin(), j.end(), std::begin(l),
                   [](const BasicJsonType & elem)
    {
        return elem.template get<T>();
    });
}

template<typename BasicJsonType, typename T, std::size_t N>
auto from_json(const BasicJsonType& j, T (&arr)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
-> decltype(j.template get<T>(), void())
{
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = j.at(i).template get<T>();
    }
}

template<typename BasicJsonType>
void from_json_array_impl(const BasicJsonType& j, typename BasicJsonType::array_t& arr, priority_tag<3> /*unused*/)
{
    arr = *j.template get_ptr<const typename BasicJsonType::array_t*>();
}

template<typename BasicJsonType, typename T, std::size_t N>
auto from_json_array_impl(const BasicJsonType& j, std::array<T, N>& arr,
                          priority_tag<2> /*unused*/)
-> decltype(j.template get<T>(), void())
{
    for (std::size_t i = 0; i < N; ++i)
    {
        arr[i] = j.at(i).template get<T>();
    }
}

template<typename BasicJsonType, typename ConstructibleArrayType,
         enable_if_t<
             std::is_assignable<ConstructibleArrayType&, ConstructibleArrayType>::value,
             int> = 0>
auto from_json_array_impl(const BasicJsonType& j, ConstructibleArrayType& arr, priority_tag<1> /*unused*/)
-> decltype(
    arr.reserve(std::declval<typename ConstructibleArrayType::size_type>()),
    j.template get<typename ConstructibleArrayType::value_type>(),
    void())
{
    using std::end;

    ConstructibleArrayType ret;
    ret.reserve(j.size());
    std::transform(j.begin(), j.end(),
                   std::inserter(ret, end(ret)), [](const BasicJsonType & i)
    {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename ConstructibleArrayType::value_type>();
    });
    arr = std::move(ret);
}

template<typename BasicJsonType, typename ConstructibleArrayType,
         enable_if_t<
             std::is_assignable<ConstructibleArrayType&, ConstructibleArrayType>::value,
             int> = 0>
void from_json_array_impl(const BasicJsonType& j, ConstructibleArrayType& arr,
                          priority_tag<0> /*unused*/)
{
    using std::end;

    ConstructibleArrayType ret;
    std::transform(
        j.begin(), j.end(), std::inserter(ret, end(ret)),
        [](const BasicJsonType & i)
    {
        // get<BasicJsonType>() returns *this, this won't call a from_json
        // method when value_type is BasicJsonType
        return i.template get<typename ConstructibleArrayType::value_type>();
    });
    arr = std::move(ret);
}

template < typename BasicJsonType, typename ConstructibleArrayType,
           enable_if_t <
               is_constructible_array_type<BasicJsonType, ConstructibleArrayType>::value&&
               !is_constructible_object_type<BasicJsonType, ConstructibleArrayType>::value&&
               !is_constructible_string_type<BasicJsonType, ConstructibleArrayType>::value&&
               !std::is_same<ConstructibleArrayType, typename BasicJsonType::binary_t>::value&&
               !is_basic_json<ConstructibleArrayType>::value,
               int > = 0 >
auto from_json(const BasicJsonType& j, ConstructibleArrayType& arr)
-> decltype(from_json_array_impl(j, arr, priority_tag<3> {}),
j.template get<typename ConstructibleArrayType::value_type>(),
void())
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }

    from_json_array_impl(j, arr, priority_tag<3> {});
}

template < typename BasicJsonType, typename T, std::size_t... Idx >
std::array<T, sizeof...(Idx)> from_json_inplace_array_impl(BasicJsonType&& j,
        identity_tag<std::array<T, sizeof...(Idx)>> /*unused*/, index_sequence<Idx...> /*unused*/)
{
    return { { std::forward<BasicJsonType>(j).at(Idx).template get<T>()... } };
}

template < typename BasicJsonType, typename T, std::size_t N >
auto from_json(BasicJsonType&& j, identity_tag<std::array<T, N>> tag)
-> decltype(from_json_inplace_array_impl(std::forward<BasicJsonType>(j), tag, make_index_sequence<N> {}))
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }

    return from_json_inplace_array_impl(std::forward<BasicJsonType>(j), tag, make_index_sequence<N> {});
}

template<typename BasicJsonType>
void from_json(const BasicJsonType& j, typename BasicJsonType::binary_t& bin)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_binary()))
    {
        JSON_THROW(type_error::create(302, "type must be binary, but is " + std::string(j.type_name()), j));
    }

    bin = *j.template get_ptr<const typename BasicJsonType::binary_t*>();
}

template<typename BasicJsonType, typename ConstructibleObjectType,
         enable_if_t<is_constructible_object_type<BasicJsonType, ConstructibleObjectType>::value, int> = 0>
void from_json(const BasicJsonType& j, ConstructibleObjectType& obj)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_object()))
    {
        JSON_THROW(type_error::create(302, "type must be object, but is " + std::string(j.type_name()), j));
    }

    ConstructibleObjectType ret;
    const auto* inner_object = j.template get_ptr<const typename BasicJsonType::object_t*>();
    using value_type = typename ConstructibleObjectType::value_type;
    std::transform(
        inner_object->begin(), inner_object->end(),
        std::inserter(ret, ret.begin()),
        [](typename BasicJsonType::object_t::value_type const & p)
    {
        return value_type(p.first, p.second.template get<typename ConstructibleObjectType::mapped_type>());
    });
    obj = std::move(ret);
}

// overload for arithmetic types, not chosen for basic_json template arguments
// (BooleanType, etc..); note: Is it really necessary to provide explicit
// overloads for boolean_t etc. in case of a custom BooleanType which is not
// an arithmetic type?
template < typename BasicJsonType, typename ArithmeticType,
           enable_if_t <
               std::is_arithmetic<ArithmeticType>::value&&
               !std::is_same<ArithmeticType, typename BasicJsonType::number_unsigned_t>::value&&
               !std::is_same<ArithmeticType, typename BasicJsonType::number_integer_t>::value&&
               !std::is_same<ArithmeticType, typename BasicJsonType::number_float_t>::value&&
               !std::is_same<ArithmeticType, typename BasicJsonType::boolean_t>::value,
               int > = 0 >
void from_json(const BasicJsonType& j, ArithmeticType& val)
{
    switch (static_cast<value_t>(j))
    {
        case value_t::number_unsigned:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_unsigned_t*>());
            break;
        }
        case value_t::number_integer:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_integer_t*>());
            break;
        }
        case value_t::number_float:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::number_float_t*>());
            break;
        }
        case value_t::boolean:
        {
            val = static_cast<ArithmeticType>(*j.template get_ptr<const typename BasicJsonType::boolean_t*>());
            break;
        }

        default:
            JSON_THROW(type_error::create(302, "type must be number, but is " + std::string(j.type_name()), j));
    }
}

template<typename BasicJsonType, typename... Args, std::size_t... Idx>
std::tuple<Args...> from_json_tuple_impl_base(BasicJsonType&& j, index_sequence<Idx...> /*unused*/)
{
    return std::make_tuple(std::forward<BasicJsonType>(j).at(Idx).template get<Args>()...);
}

template < typename BasicJsonType, class A1, class A2 >
std::pair<A1, A2> from_json_tuple_impl(BasicJsonType&& j, identity_tag<std::pair<A1, A2>> /*unused*/, priority_tag<0> /*unused*/)
{
    return {std::forward<BasicJsonType>(j).at(0).template get<A1>(),
            std::forward<BasicJsonType>(j).at(1).template get<A2>()};
}

template<typename BasicJsonType, typename A1, typename A2>
void from_json_tuple_impl(BasicJsonType&& j, std::pair<A1, A2>& p, priority_tag<1> /*unused*/)
{
    p = from_json_tuple_impl(std::forward<BasicJsonType>(j), identity_tag<std::pair<A1, A2>> {}, priority_tag<0> {});
}

template<typename BasicJsonType, typename... Args>
std::tuple<Args...> from_json_tuple_impl(BasicJsonType&& j, identity_tag<std::tuple<Args...>> /*unused*/, priority_tag<2> /*unused*/)
{
    return from_json_tuple_impl_base<BasicJsonType, Args...>(std::forward<BasicJsonType>(j), index_sequence_for<Args...> {});
}

template<typename BasicJsonType, typename... Args>
void from_json_tuple_impl(BasicJsonType&& j, std::tuple<Args...>& t, priority_tag<3> /*unused*/)
{
    t = from_json_tuple_impl_base<BasicJsonType, Args...>(std::forward<BasicJsonType>(j), index_sequence_for<Args...> {});
}

template<typename BasicJsonType, typename TupleRelated>
auto from_json(BasicJsonType&& j, TupleRelated&& t)
-> decltype(from_json_tuple_impl(std::forward<BasicJsonType>(j), std::forward<TupleRelated>(t), priority_tag<3> {}))
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }

    return from_json_tuple_impl(std::forward<BasicJsonType>(j), std::forward<TupleRelated>(t), priority_tag<3> {});
}

template < typename BasicJsonType, typename Key, typename Value, typename Compare, typename Allocator,
           typename = enable_if_t < !std::is_constructible <
                                        typename BasicJsonType::string_t, Key >::value >>
void from_json(const BasicJsonType& j, std::map<Key, Value, Compare, Allocator>& m)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }
    m.clear();
    for (const auto& p : j)
    {
        if (JSON_HEDLEY_UNLIKELY(!p.is_array()))
        {
            JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(p.type_name()), j));
        }
        m.emplace(p.at(0).template get<Key>(), p.at(1).template get<Value>());
    }
}

template < typename BasicJsonType, typename Key, typename Value, typename Hash, typename KeyEqual, typename Allocator,
           typename = enable_if_t < !std::is_constructible <
                                        typename BasicJsonType::string_t, Key >::value >>
void from_json(const BasicJsonType& j, std::unordered_map<Key, Value, Hash, KeyEqual, Allocator>& m)
{
    if (JSON_HEDLEY_UNLIKELY(!j.is_array()))
    {
        JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(j.type_name()), j));
    }
    m.clear();
    for (const auto& p : j)
    {
        if (JSON_HEDLEY_UNLIKELY(!p.is_array()))
        {
            JSON_THROW(type_error::create(302, "type must be array, but is " + std::string(p.type_name()), j));
        }
        m.emplace(p.at(0).template get<Key>(), p.at(1).template get<Value>());
    }
}

struct from_json_fn
{
    template<typename BasicJsonType, typename T>
    auto operator()(const BasicJsonType& j, T&& val) const
    noexcept(noexcept(from_json(j, std::forward<T>(val))))
    -> decltype(from_json(j, std::forward<T>(val)))
    {
        return from_json(j, std::forward<T>(val));
    }
};
}  // namespace detail

/// namespace to hold default `from_json` function
/// to see why this is required:
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
constexpr const auto& from_json = detail::static_const<detail::from_json_fn>::value; // NOLINT(misc-definitions-in-headers)
} // namespace
} // namespace nlohmann

// #include <nlohmann/detail/conversions/to_json.hpp>


#include <algorithm> // copy
#include <iterator> // begin, end
#include <string> // string
#include <tuple> // tuple, get
#include <type_traits> // is_same, is_constructible, is_floating_point, is_enum, underlying_type
#include <utility> // move, forward, declval, pair
#include <valarray> // valarray
#include <vector> // vector

// #include <nlohmann/detail/iterators/iteration_proxy.hpp>


#include <cstddef> // size_t
#include <iterator> // input_iterator_tag
#include <string> // string, to_string
#include <tuple> // tuple_size, get, tuple_element
#include <utility> // move

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{
template<typename string_type>
void int_to_string( string_type& target, std::size_t value )
{
    // For ADL
    using std::to_string;
    target = to_string(value);
}
template<typename IteratorType> class iteration_proxy_value
{
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = iteration_proxy_value;
    using pointer = value_type * ;
    using reference = value_type & ;
    using iterator_category = std::input_iterator_tag;
    using string_type = typename std::remove_cv< typename std::remove_reference<decltype( std::declval<IteratorType>().key() ) >::type >::type;

  private:
    /// the iterator
    IteratorType anchor;
    /// an index for arrays (used to create key names)
    std::size_t array_index = 0;
    /// last stringified array index
    mutable std::size_t array_index_last = 0;
    /// a string representation of the array index
    mutable string_type array_index_str = "0";
    /// an empty string (to return a reference for primitive values)
    const string_type empty_str{};

  public:
    explicit iteration_proxy_value(IteratorType it) noexcept
        : anchor(std::move(it))
    {}

    /// dereference operator (needed for range-based for)
    iteration_proxy_value& operator*()
    {
        return *this;
    }

    /// increment operator (needed for range-based for)
    iteration_proxy_value& operator++()
    {
        ++anchor;
        ++array_index;

        return *this;
    }

    /// equality operator (needed for InputIterator)
    bool operator==(const iteration_proxy_value& o) const
    {
        return anchor == o.anchor;
    }

    /// inequality operator (needed for range-based for)
    bool operator!=(const iteration_proxy_value& o) const
    {
        return anchor != o.anchor;
    }

    /// return key of the iterator
    const string_type& key() const
    {
        JSON_ASSERT(anchor.m_object != nullptr);

        switch (anchor.m_object->type())
        {
            // use integer array index as key
            case value_t::array:
            {
                if (array_index != array_index_last)
                {
                    int_to_string( array_index_str, array_index );
                    array_index_last = array_index;
                }
                return array_index_str;
            }

            // use key from the object
            case value_t::object:
                return anchor.key();

            // use an empty key for all primitive types
            default:
                return empty_str;
        }
    }

    /// return value of the iterator
    typename IteratorType::reference value() const
    {
        return anchor.value();
    }
};

/// proxy class for the items() function
template<typename IteratorType> class iteration_proxy
{
  private:
    /// the container to iterate
    typename IteratorType::reference container;

  public:
    /// construct iteration proxy from a container
    explicit iteration_proxy(typename IteratorType::reference cont) noexcept
        : container(cont) {}

    /// return iterator begin (needed for range-based for)
    iteration_proxy_value<IteratorType> begin() noexcept
    {
        return iteration_proxy_value<IteratorType>(container.begin());
    }

    /// return iterator end (needed for range-based for)
    iteration_proxy_value<IteratorType> end() noexcept
    {
        return iteration_proxy_value<IteratorType>(container.end());
    }
};
// Structured Bindings Support
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
template<std::size_t N, typename IteratorType, enable_if_t<N == 0, int> = 0>
auto get(const nlohmann::detail::iteration_proxy_value<IteratorType>& i) -> decltype(i.key())
{
    return i.key();
}
// Structured Bindings Support
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
template<std::size_t N, typename IteratorType, enable_if_t<N == 1, int> = 0>
auto get(const nlohmann::detail::iteration_proxy_value<IteratorType>& i) -> decltype(i.value())
{
    return i.value();
}
}  // namespace detail
}  // namespace nlohmann

// The Addition to the STD Namespace is required to add
// Structured Bindings Support to the iteration_proxy_value class
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
// And see https://github.com/nlohmann/json/pull/1391
namespace std
{
#if defined(__clang__)
    // Fix: https://github.com/nlohmann/json/issues/1401
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wmismatched-tags"
#endif
template<typename IteratorType>
class tuple_size<::nlohmann::detail::iteration_proxy_value<IteratorType>>
            : public std::integral_constant<std::size_t, 2> {};

template<std::size_t N, typename IteratorType>
class tuple_element<N, ::nlohmann::detail::iteration_proxy_value<IteratorType >>
{
  public:
    using type = decltype(
                     get<N>(std::declval <
                            ::nlohmann::detail::iteration_proxy_value<IteratorType >> ()));
};
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
} // namespace std

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{
//////////////////
// constructors //
//////////////////

/*
 * Note all external_constructor<>::construct functions need to call
 * j.m_value.destroy(j.m_type) to avoid a memory leak in case j contains an
 * allocated value (e.g., a string). See bug issue
 * https://github.com/nlohmann/json/issues/2865 for more information.
 */

template<value_t> struct external_constructor;

template<>
struct external_constructor<value_t::boolean>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::boolean_t b) noexcept
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::boolean;
        j.m_value = b;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::string>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::string_t& s)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::string;
        j.m_value = s;
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::string_t&& s)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::string;
        j.m_value = std::move(s);
        j.assert_invariant();
    }

    template < typename BasicJsonType, typename CompatibleStringType,
               enable_if_t < !std::is_same<CompatibleStringType, typename BasicJsonType::string_t>::value,
                             int > = 0 >
    static void construct(BasicJsonType& j, const CompatibleStringType& str)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::string;
        j.m_value.string = j.template create<typename BasicJsonType::string_t>(str);
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::binary>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::binary_t& b)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::binary;
        j.m_value = typename BasicJsonType::binary_t(b);
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::binary_t&& b)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::binary;
        j.m_value = typename BasicJsonType::binary_t(std::move(b));;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_float>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::number_float_t val) noexcept
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::number_float;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_unsigned>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::number_unsigned_t val) noexcept
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::number_unsigned;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::number_integer>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::number_integer_t val) noexcept
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::number_integer;
        j.m_value = val;
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::array>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::array_t& arr)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::array;
        j.m_value = arr;
        j.set_parents();
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::array_t&& arr)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::array;
        j.m_value = std::move(arr);
        j.set_parents();
        j.assert_invariant();
    }

    template < typename BasicJsonType, typename CompatibleArrayType,
               enable_if_t < !std::is_same<CompatibleArrayType, typename BasicJsonType::array_t>::value,
                             int > = 0 >
    static void construct(BasicJsonType& j, const CompatibleArrayType& arr)
    {
        using std::begin;
        using std::end;

        j.m_value.destroy(j.m_type);
        j.m_type = value_t::array;
        j.m_value.array = j.template create<typename BasicJsonType::array_t>(begin(arr), end(arr));
        j.set_parents();
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const std::vector<bool>& arr)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::array;
        j.m_value = value_t::array;
        j.m_value.array->reserve(arr.size());
        for (const bool x : arr)
        {
            j.m_value.array->push_back(x);
            j.set_parent(j.m_value.array->back());
        }
        j.assert_invariant();
    }

    template<typename BasicJsonType, typename T,
             enable_if_t<std::is_convertible<T, BasicJsonType>::value, int> = 0>
    static void construct(BasicJsonType& j, const std::valarray<T>& arr)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::array;
        j.m_value = value_t::array;
        j.m_value.array->resize(arr.size());
        if (arr.size() > 0)
        {
            std::copy(std::begin(arr), std::end(arr), j.m_value.array->begin());
        }
        j.set_parents();
        j.assert_invariant();
    }
};

template<>
struct external_constructor<value_t::object>
{
    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, const typename BasicJsonType::object_t& obj)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::object;
        j.m_value = obj;
        j.set_parents();
        j.assert_invariant();
    }

    template<typename BasicJsonType>
    static void construct(BasicJsonType& j, typename BasicJsonType::object_t&& obj)
    {
        j.m_value.destroy(j.m_type);
        j.m_type = value_t::object;
        j.m_value = std::move(obj);
        j.set_parents();
        j.assert_invariant();
    }

    template < typename BasicJsonType, typename CompatibleObjectType,
               enable_if_t < !std::is_same<CompatibleObjectType, typename BasicJsonType::object_t>::value, int > = 0 >
    static void construct(BasicJsonType& j, const CompatibleObjectType& obj)
    {
        using std::begin;
        using std::end;

        j.m_value.destroy(j.m_type);
        j.m_type = value_t::object;
        j.m_value.object = j.template create<typename BasicJsonType::object_t>(begin(obj), end(obj));
        j.set_parents();
        j.assert_invariant();
    }
};

/////////////
// to_json //
/////////////

template<typename BasicJsonType, typename T,
         enable_if_t<std::is_same<T, typename BasicJsonType::boolean_t>::value, int> = 0>
void to_json(BasicJsonType& j, T b) noexcept
{
    external_constructor<value_t::boolean>::construct(j, b);
}

template<typename BasicJsonType, typename CompatibleString,
         enable_if_t<std::is_constructible<typename BasicJsonType::string_t, CompatibleString>::value, int> = 0>
void to_json(BasicJsonType& j, const CompatibleString& s)
{
    external_constructor<value_t::string>::construct(j, s);
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, typename BasicJsonType::string_t&& s)
{
    external_constructor<value_t::string>::construct(j, std::move(s));
}

template<typename BasicJsonType, typename FloatType,
         enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_json(BasicJsonType& j, FloatType val) noexcept
{
    external_constructor<value_t::number_float>::construct(j, static_cast<typename BasicJsonType::number_float_t>(val));
}

template<typename BasicJsonType, typename CompatibleNumberUnsignedType,
         enable_if_t<is_compatible_integer_type<typename BasicJsonType::number_unsigned_t, CompatibleNumberUnsignedType>::value, int> = 0>
void to_json(BasicJsonType& j, CompatibleNumberUnsignedType val) noexcept
{
    external_constructor<value_t::number_unsigned>::construct(j, static_cast<typename BasicJsonType::number_unsigned_t>(val));
}

template<typename BasicJsonType, typename CompatibleNumberIntegerType,
         enable_if_t<is_compatible_integer_type<typename BasicJsonType::number_integer_t, CompatibleNumberIntegerType>::value, int> = 0>
void to_json(BasicJsonType& j, CompatibleNumberIntegerType val) noexcept
{
    external_constructor<value_t::number_integer>::construct(j, static_cast<typename BasicJsonType::number_integer_t>(val));
}

template<typename BasicJsonType, typename EnumType,
         enable_if_t<std::is_enum<EnumType>::value, int> = 0>
void to_json(BasicJsonType& j, EnumType e) noexcept
{
    using underlying_type = typename std::underlying_type<EnumType>::type;
    external_constructor<value_t::number_integer>::construct(j, static_cast<underlying_type>(e));
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, const std::vector<bool>& e)
{
    external_constructor<value_t::array>::construct(j, e);
}

template < typename BasicJsonType, typename CompatibleArrayType,
           enable_if_t < is_compatible_array_type<BasicJsonType,
                         CompatibleArrayType>::value&&
                         !is_compatible_object_type<BasicJsonType, CompatibleArrayType>::value&&
                         !is_compatible_string_type<BasicJsonType, CompatibleArrayType>::value&&
                         !std::is_same<typename BasicJsonType::binary_t, CompatibleArrayType>::value&&
                         !is_basic_json<CompatibleArrayType>::value,
                         int > = 0 >
void to_json(BasicJsonType& j, const CompatibleArrayType& arr)
{
    external_constructor<value_t::array>::construct(j, arr);
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, const typename BasicJsonType::binary_t& bin)
{
    external_constructor<value_t::binary>::construct(j, bin);
}

template<typename BasicJsonType, typename T,
         enable_if_t<std::is_convertible<T, BasicJsonType>::value, int> = 0>
void to_json(BasicJsonType& j, const std::valarray<T>& arr)
{
    external_constructor<value_t::array>::construct(j, std::move(arr));
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, typename BasicJsonType::array_t&& arr)
{
    external_constructor<value_t::array>::construct(j, std::move(arr));
}

template < typename BasicJsonType, typename CompatibleObjectType,
           enable_if_t < is_compatible_object_type<BasicJsonType, CompatibleObjectType>::value&& !is_basic_json<CompatibleObjectType>::value, int > = 0 >
void to_json(BasicJsonType& j, const CompatibleObjectType& obj)
{
    external_constructor<value_t::object>::construct(j, obj);
}

template<typename BasicJsonType>
void to_json(BasicJsonType& j, typename BasicJsonType::object_t&& obj)
{
    external_constructor<value_t::object>::construct(j, std::move(obj));
}

template <
    typename BasicJsonType, typename T, std::size_t N,
    enable_if_t < !std::is_constructible<typename BasicJsonType::string_t,
                  const T(&)[N]>::value, // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
                  int > = 0 >
void to_json(BasicJsonType& j, const T(&arr)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
{
    external_constructor<value_t::array>::construct(j, arr);
}

template < typename BasicJsonType, typename T1, typename T2, enable_if_t < std::is_constructible<BasicJsonType, T1>::value&& std::is_constructible<BasicJsonType, T2>::value, int > = 0 >
void to_json(BasicJsonType& j, const std::pair<T1, T2>& p)
{
    j = { p.first, p.second };
}

// for https://github.com/nlohmann/json/pull/1134
template<typename BasicJsonType, typename T,
         enable_if_t<std::is_same<T, iteration_proxy_value<typename BasicJsonType::iterator>>::value, int> = 0>
void to_json(BasicJsonType& j, const T& b)
{
    j = { {b.key(), b.value()} };
}

template<typename BasicJsonType, typename Tuple, std::size_t... Idx>
void to_json_tuple_impl(BasicJsonType& j, const Tuple& t, index_sequence<Idx...> /*unused*/)
{
    j = { std::get<Idx>(t)... };
}

template<typename BasicJsonType, typename T, enable_if_t<is_constructible_tuple<BasicJsonType, T>::value, int > = 0>
void to_json(BasicJsonType& j, const T& t)
{
    to_json_tuple_impl(j, t, make_index_sequence<std::tuple_size<T>::value> {});
}

struct to_json_fn
{
    template<typename BasicJsonType, typename T>
    auto operator()(BasicJsonType& j, T&& val) const noexcept(noexcept(to_json(j, std::forward<T>(val))))
    -> decltype(to_json(j, std::forward<T>(val)), void())
    {
        return to_json(j, std::forward<T>(val));
    }
};
}  // namespace detail

/// namespace to hold default `to_json` function
/// to see why this is required:
/// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4381.html
namespace // NOLINT(cert-dcl59-cpp,fuchsia-header-anon-namespaces,google-build-namespaces)
{
constexpr const auto& to_json = detail::static_const<detail::to_json_fn>::value; // NOLINT(misc-definitions-in-headers)
} // namespace
} // namespace nlohmann

// #include <nlohmann/detail/meta/identity_tag.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>


namespace nlohmann
{

template<typename ValueType, typename>
struct adl_serializer
{
    /*!
    @brief convert a JSON value to any value type

    This function is usually called by the `get()` function of the
    @ref basic_json class (either explicit or via conversion operators).

    @note This function is chosen for default-constructible value types.

    @param[in] j        JSON value to read from
    @param[in,out] val  value to write to
    */
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j, TargetType& val) noexcept(
        noexcept(::nlohmann::from_json(std::forward<BasicJsonType>(j), val)))
    -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), val), void())
    {
        ::nlohmann::from_json(std::forward<BasicJsonType>(j), val);
    }

    /*!
    @brief convert a JSON value to any value type

    This function is usually called by the `get()` function of the
    @ref basic_json class (either explicit or via conversion operators).

    @note This function is chosen for value types which are not default-constructible.

    @param[in] j  JSON value to read from

    @return copy of the JSON value, converted to @a ValueType
    */
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto from_json(BasicJsonType && j) noexcept(
    noexcept(::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {})))
    -> decltype(::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {}))
    {
        return ::nlohmann::from_json(std::forward<BasicJsonType>(j), detail::identity_tag<TargetType> {});
    }

    /*!
    @brief convert any value type to a JSON value

    This function is usually called by the constructors of the @ref basic_json
    class.

    @param[in,out] j  JSON value to write to
    @param[in] val    value to read from
    */
    template<typename BasicJsonType, typename TargetType = ValueType>
    static auto to_json(BasicJsonType& j, TargetType && val) noexcept(
        noexcept(::nlohmann::to_json(j, std::forward<TargetType>(val))))
    -> decltype(::nlohmann::to_json(j, std::forward<TargetType>(val)), void())
    {
        ::nlohmann::to_json(j, std::forward<TargetType>(val));
    }
};
}  // namespace nlohmann

// #include <nlohmann/byte_container_with_subtype.hpp>


#include <cstdint> // uint8_t
#include <tuple> // tie
#include <utility> // move

namespace nlohmann
{

/*!
@brief an internal type for a backed binary type

This type extends the template parameter @a BinaryType provided to `basic_json`
with a subtype used by BSON and MessagePack. This type exists so that the user
does not have to specify a type themselves with a specific naming scheme in
order to override the binary type.

@tparam BinaryType container to store bytes (`std::vector<std::uint8_t>` by
                   default)

@since version 3.8.0
*/
template<typename BinaryType>
class byte_container_with_subtype : public BinaryType
{
  public:
    /// the type of the underlying container
    using container_type = BinaryType;

    byte_container_with_subtype() noexcept(noexcept(container_type()))
        : container_type()
    {}

    byte_container_with_subtype(const container_type& b) noexcept(noexcept(container_type(b)))
        : container_type(b)
    {}

    byte_container_with_subtype(container_type&& b) noexcept(noexcept(container_type(std::move(b))))
        : container_type(std::move(b))
    {}

    byte_container_with_subtype(const container_type& b, std::uint8_t subtype_) noexcept(noexcept(container_type(b)))
        : container_type(b)
        , m_subtype(subtype_)
        , m_has_subtype(true)
    {}

    byte_container_with_subtype(container_type&& b, std::uint8_t subtype_) noexcept(noexcept(container_type(std::move(b))))
        : container_type(std::move(b))
        , m_subtype(subtype_)
        , m_has_subtype(true)
    {}

    bool operator==(const byte_container_with_subtype& rhs) const
    {
        return std::tie(static_cast<const BinaryType&>(*this), m_subtype, m_has_subtype) ==
               std::tie(static_cast<const BinaryType&>(rhs), rhs.m_subtype, rhs.m_has_subtype);
    }

    bool operator!=(const byte_container_with_subtype& rhs) const
    {
        return !(rhs == *this);
    }

    /*!
    @brief sets the binary subtype

    Sets the binary subtype of the value, also flags a binary JSON value as
    having a subtype, which has implications for serialization.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @sa see @ref subtype() -- return the binary subtype
    @sa see @ref clear_subtype() -- clears the binary subtype
    @sa see @ref has_subtype() -- returns whether or not the binary value has a
    subtype

    @since version 3.8.0
    */
    void set_subtype(std::uint8_t subtype_) noexcept
    {
        m_subtype = subtype_;
        m_has_subtype = true;
    }

    /*!
    @brief return the binary subtype

    Returns the numerical subtype of the value if it has a subtype. If it does
    not have a subtype, this function will return size_t(-1) as a sentinel
    value.

    @return the numerical subtype of the binary value

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @sa see @ref set_subtype() -- sets the binary subtype
    @sa see @ref clear_subtype() -- clears the binary subtype
    @sa see @ref has_subtype() -- returns whether or not the binary value has a
    subtype

    @since version 3.8.0
    */
    constexpr std::uint8_t subtype() const noexcept
    {
        return m_subtype;
    }

    /*!
    @brief return whether the value has a subtype

    @return whether the value has a subtype

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @sa see @ref subtype() -- return the binary subtype
    @sa see @ref set_subtype() -- sets the binary subtype
    @sa see @ref clear_subtype() -- clears the binary subtype

    @since version 3.8.0
    */
    constexpr bool has_subtype() const noexcept
    {
        return m_has_subtype;
    }

    /*!
    @brief clears the binary subtype

    Clears the binary subtype and flags the value as not having a subtype, which
    has implications for serialization; for instance MessagePack will prefer the
    bin family over the ext family.

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this member function never throws
    exceptions.

    @sa see @ref subtype() -- return the binary subtype
    @sa see @ref set_subtype() -- sets the binary subtype
    @sa see @ref has_subtype() -- returns whether or not the binary value has a
    subtype

    @since version 3.8.0
    */
    void clear_subtype() noexcept
    {
        m_subtype = 0;
        m_has_subtype = false;
    }

  private:
    std::uint8_t m_subtype = 0;
    bool m_has_subtype = false;
};

}  // namespace nlohmann

// #include <nlohmann/detail/conversions/from_json.hpp>

// #include <nlohmann/detail/conversions/to_json.hpp>

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/hash.hpp>


#include <cstdint> // uint8_t
#include <cstddef> // size_t
#include <functional> // hash

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{

// boost::hash_combine
inline std::size_t combine(std::size_t seed, std::size_t h) noexcept
{
    seed ^= h + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
    return seed;
}

/*!
@brief hash a JSON value

The hash function tries to rely on std::hash where possible. Furthermore, the
type of the JSON value is taken into account to have different hash values for
null, 0, 0U, and false, etc.

@tparam BasicJsonType basic_json specialization
@param j JSON value to hash
@return hash value of j
*/
template<typename BasicJsonType>
std::size_t hash(const BasicJsonType& j)
{
    using string_t = typename BasicJsonType::string_t;
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;

    const auto type = static_cast<std::size_t>(j.type());
    switch (j.type())
    {
        case BasicJsonType::value_t::null:
        case BasicJsonType::value_t::discarded:
        {
            return combine(type, 0);
        }

        case BasicJsonType::value_t::object:
        {
            auto seed = combine(type, j.size());
            for (const auto& element : j.items())
            {
                const auto h = std::hash<string_t> {}(element.key());
                seed = combine(seed, h);
                seed = combine(seed, hash(element.value()));
            }
            return seed;
        }

        case BasicJsonType::value_t::array:
        {
            auto seed = combine(type, j.size());
            for (const auto& element : j)
            {
                seed = combine(seed, hash(element));
            }
            return seed;
        }

        case BasicJsonType::value_t::string:
        {
            const auto h = std::hash<string_t> {}(j.template get_ref<const string_t&>());
            return combine(type, h);
        }

        case BasicJsonType::value_t::boolean:
        {
            const auto h = std::hash<bool> {}(j.template get<bool>());
            return combine(type, h);
        }

        case BasicJsonType::value_t::number_integer:
        {
            const auto h = std::hash<number_integer_t> {}(j.template get<number_integer_t>());
            return combine(type, h);
        }

        case BasicJsonType::value_t::number_unsigned:
        {
            const auto h = std::hash<number_unsigned_t> {}(j.template get<number_unsigned_t>());
            return combine(type, h);
        }

        case BasicJsonType::value_t::number_float:
        {
            const auto h = std::hash<number_float_t> {}(j.template get<number_float_t>());
            return combine(type, h);
        }

        case BasicJsonType::value_t::binary:
        {
            auto seed = combine(type, j.get_binary().size());
            const auto h = std::hash<bool> {}(j.get_binary().has_subtype());
            seed = combine(seed, h);
            seed = combine(seed, j.get_binary().subtype());
            for (const auto byte : j.get_binary())
            {
                seed = combine(seed, std::hash<std::uint8_t> {}(byte));
            }
            return seed;
        }

        default:                   // LCOV_EXCL_LINE
            JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
            return 0;              // LCOV_EXCL_LINE
    }
}

}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/input/binary_reader.hpp>


#include <algorithm> // generate_n
#include <array> // array
#include <cmath> // ldexp
#include <cstddef> // size_t
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstdio> // snprintf
#include <cstring> // memcpy
#include <iterator> // back_inserter
#include <limits> // numeric_limits
#include <string> // char_traits, string
#include <utility> // make_pair, move
#include <vector> // vector

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/input/input_adapters.hpp>


#include <array> // array
#include <cstddef> // size_t
#include <cstring> // strlen
#include <iterator> // begin, end, iterator_traits, random_access_iterator_tag, distance, next
#include <memory> // shared_ptr, make_shared, addressof
#include <numeric> // accumulate
#include <string> // string, char_traits
#include <type_traits> // enable_if, is_base_of, is_pointer, is_integral, remove_pointer
#include <utility> // pair, declval

#ifndef JSON_NO_IO
    #include <cstdio>   // FILE *
    #include <istream>  // istream
#endif                  // JSON_NO_IO

// #include <nlohmann/detail/iterators/iterator_traits.hpp>

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
/// the supported input formats
enum class input_format_t { json, cbor, msgpack, ubjson, bson };

////////////////////
// input adapters //
////////////////////

#ifndef JSON_NO_IO
/*!
Input adapter for stdio file access. This adapter read only 1 byte and do not use any
 buffer. This adapter is a very low level adapter.
*/
class file_input_adapter
{
  public:
    using char_type = char;

    JSON_HEDLEY_NON_NULL(2)
    explicit file_input_adapter(std::FILE* f) noexcept
        : m_file(f)
    {}

    // make class move-only
    file_input_adapter(const file_input_adapter&) = delete;
    file_input_adapter(file_input_adapter&&) noexcept = default;
    file_input_adapter& operator=(const file_input_adapter&) = delete;
    file_input_adapter& operator=(file_input_adapter&&) = delete;
    ~file_input_adapter() = default;

    std::char_traits<char>::int_type get_character() noexcept
    {
        return std::fgetc(m_file);
    }

  private:
    /// the file pointer to read from
    std::FILE* m_file;
};


/*!
Input adapter for a (caching) istream. Ignores a UFT Byte Order Mark at
beginning of input. Does not support changing the underlying std::streambuf
in mid-input. Maintains underlying std::istream and std::streambuf to support
subsequent use of standard std::istream operations to process any input
characters following those used in parsing the JSON input.  Clears the
std::istream flags; any input errors (e.g., EOF) will be detected by the first
subsequent call for input from the std::istream.
*/
class input_stream_adapter
{
  public:
    using char_type = char;

    ~input_stream_adapter()
    {
        // clear stream flags; we use underlying streambuf I/O, do not
        // maintain ifstream flags, except eof
        if (is != nullptr)
        {
            is->clear(is->rdstate() & std::ios::eofbit);
        }
    }

    explicit input_stream_adapter(std::istream& i)
        : is(&i), sb(i.rdbuf())
    {}

    // delete because of pointer members
    input_stream_adapter(const input_stream_adapter&) = delete;
    input_stream_adapter& operator=(input_stream_adapter&) = delete;
    input_stream_adapter& operator=(input_stream_adapter&&) = delete;

    input_stream_adapter(input_stream_adapter&& rhs) noexcept
        : is(rhs.is), sb(rhs.sb)
    {
        rhs.is = nullptr;
        rhs.sb = nullptr;
    }

    // std::istream/std::streambuf use std::char_traits<char>::to_int_type, to
    // ensure that std::char_traits<char>::eof() and the character 0xFF do not
    // end up as the same value, eg. 0xFFFFFFFF.
    std::char_traits<char>::int_type get_character()
    {
        auto res = sb->sbumpc();
        // set eof manually, as we don't use the istream interface.
        if (JSON_HEDLEY_UNLIKELY(res == std::char_traits<char>::eof()))
        {
            is->clear(is->rdstate() | std::ios::eofbit);
        }
        return res;
    }

  private:
    /// the associated input stream
    std::istream* is = nullptr;
    std::streambuf* sb = nullptr;
};
#endif  // JSON_NO_IO

// General-purpose iterator-based adapter. It might not be as fast as
// theoretically possible for some containers, but it is extremely versatile.
template<typename IteratorType>
class iterator_input_adapter
{
  public:
    using char_type = typename std::iterator_traits<IteratorType>::value_type;

    iterator_input_adapter(IteratorType first, IteratorType last)
        : current(std::move(first)), end(std::move(last))
    {}

    typename std::char_traits<char_type>::int_type get_character()
    {
        if (JSON_HEDLEY_LIKELY(current != end))
        {
            auto result = std::char_traits<char_type>::to_int_type(*current);
            std::advance(current, 1);
            return result;
        }

        return std::char_traits<char_type>::eof();
    }

  private:
    IteratorType current;
    IteratorType end;

    template<typename BaseInputAdapter, size_t T>
    friend struct wide_string_input_helper;

    bool empty() const
    {
        return current == end;
    }
};


template<typename BaseInputAdapter, size_t T>
struct wide_string_input_helper;

template<typename BaseInputAdapter>
struct wide_string_input_helper<BaseInputAdapter, 4>
{
    // UTF-32
    static void fill_buffer(BaseInputAdapter& input,
                            std::array<std::char_traits<char>::int_type, 4>& utf8_bytes,
                            size_t& utf8_bytes_index,
                            size_t& utf8_bytes_filled)
    {
        utf8_bytes_index = 0;

        if (JSON_HEDLEY_UNLIKELY(input.empty()))
        {
            utf8_bytes[0] = std::char_traits<char>::eof();
            utf8_bytes_filled = 1;
        }
        else
        {
            // get the current character
            const auto wc = input.get_character();

            // UTF-32 to UTF-8 encoding
            if (wc < 0x80)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(wc);
                utf8_bytes_filled = 1;
            }
            else if (wc <= 0x7FF)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xC0u | ((static_cast<unsigned int>(wc) >> 6u) & 0x1Fu));
                utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | (static_cast<unsigned int>(wc) & 0x3Fu));
                utf8_bytes_filled = 2;
            }
            else if (wc <= 0xFFFF)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xE0u | ((static_cast<unsigned int>(wc) >> 12u) & 0x0Fu));
                utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | ((static_cast<unsigned int>(wc) >> 6u) & 0x3Fu));
                utf8_bytes[2] = static_cast<std::char_traits<char>::int_type>(0x80u | (static_cast<unsigned int>(wc) & 0x3Fu));
                utf8_bytes_filled = 3;
            }
            else if (wc <= 0x10FFFF)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xF0u | ((static_cast<unsigned int>(wc) >> 18u) & 0x07u));
                utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | ((static_cast<unsigned int>(wc) >> 12u) & 0x3Fu));
                utf8_bytes[2] = static_cast<std::char_traits<char>::int_type>(0x80u | ((static_cast<unsigned int>(wc) >> 6u) & 0x3Fu));
                utf8_bytes[3] = static_cast<std::char_traits<char>::int_type>(0x80u | (static_cast<unsigned int>(wc) & 0x3Fu));
                utf8_bytes_filled = 4;
            }
            else
            {
                // unknown character
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(wc);
                utf8_bytes_filled = 1;
            }
        }
    }
};

template<typename BaseInputAdapter>
struct wide_string_input_helper<BaseInputAdapter, 2>
{
    // UTF-16
    static void fill_buffer(BaseInputAdapter& input,
                            std::array<std::char_traits<char>::int_type, 4>& utf8_bytes,
                            size_t& utf8_bytes_index,
                            size_t& utf8_bytes_filled)
    {
        utf8_bytes_index = 0;

        if (JSON_HEDLEY_UNLIKELY(input.empty()))
        {
            utf8_bytes[0] = std::char_traits<char>::eof();
            utf8_bytes_filled = 1;
        }
        else
        {
            // get the current character
            const auto wc = input.get_character();

            // UTF-16 to UTF-8 encoding
            if (wc < 0x80)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(wc);
                utf8_bytes_filled = 1;
            }
            else if (wc <= 0x7FF)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xC0u | ((static_cast<unsigned int>(wc) >> 6u)));
                utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | (static_cast<unsigned int>(wc) & 0x3Fu));
                utf8_bytes_filled = 2;
            }
            else if (0xD800 > wc || wc >= 0xE000)
            {
                utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xE0u | ((static_cast<unsigned int>(wc) >> 12u)));
                utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | ((static_cast<unsigned int>(wc) >> 6u) & 0x3Fu));
                utf8_bytes[2] = static_cast<std::char_traits<char>::int_type>(0x80u | (static_cast<unsigned int>(wc) & 0x3Fu));
                utf8_bytes_filled = 3;
            }
            else
            {
                if (JSON_HEDLEY_UNLIKELY(!input.empty()))
                {
                    const auto wc2 = static_cast<unsigned int>(input.get_character());
                    const auto charcode = 0x10000u + (((static_cast<unsigned int>(wc) & 0x3FFu) << 10u) | (wc2 & 0x3FFu));
                    utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(0xF0u | (charcode >> 18u));
                    utf8_bytes[1] = static_cast<std::char_traits<char>::int_type>(0x80u | ((charcode >> 12u) & 0x3Fu));
                    utf8_bytes[2] = static_cast<std::char_traits<char>::int_type>(0x80u | ((charcode >> 6u) & 0x3Fu));
                    utf8_bytes[3] = static_cast<std::char_traits<char>::int_type>(0x80u | (charcode & 0x3Fu));
                    utf8_bytes_filled = 4;
                }
                else
                {
                    utf8_bytes[0] = static_cast<std::char_traits<char>::int_type>(wc);
                    utf8_bytes_filled = 1;
                }
            }
        }
    }
};

// Wraps another input apdater to convert wide character types into individual bytes.
template<typename BaseInputAdapter, typename WideCharType>
class wide_string_input_adapter
{
  public:
    using char_type = char;

    wide_string_input_adapter(BaseInputAdapter base)
        : base_adapter(base) {}

    typename std::char_traits<char>::int_type get_character() noexcept
    {
        // check if buffer needs to be filled
        if (utf8_bytes_index == utf8_bytes_filled)
        {
            fill_buffer<sizeof(WideCharType)>();

            JSON_ASSERT(utf8_bytes_filled > 0);
            JSON_ASSERT(utf8_bytes_index == 0);
        }

        // use buffer
        JSON_ASSERT(utf8_bytes_filled > 0);
        JSON_ASSERT(utf8_bytes_index < utf8_bytes_filled);
        return utf8_bytes[utf8_bytes_index++];
    }

  private:
    BaseInputAdapter base_adapter;

    template<size_t T>
    void fill_buffer()
    {
        wide_string_input_helper<BaseInputAdapter, T>::fill_buffer(base_adapter, utf8_bytes, utf8_bytes_index, utf8_bytes_filled);
    }

    /// a buffer for UTF-8 bytes
    std::array<std::char_traits<char>::int_type, 4> utf8_bytes = {{0, 0, 0, 0}};

    /// index to the utf8_codes array for the next valid byte
    std::size_t utf8_bytes_index = 0;
    /// number of valid bytes in the utf8_codes array
    std::size_t utf8_bytes_filled = 0;
};


template<typename IteratorType, typename Enable = void>
struct iterator_input_adapter_factory
{
    using iterator_type = IteratorType;
    using char_type = typename std::iterator_traits<iterator_type>::value_type;
    using adapter_type = iterator_input_adapter<iterator_type>;

    static adapter_type create(IteratorType first, IteratorType last)
    {
        return adapter_type(std::move(first), std::move(last));
    }
};

template<typename T>
struct is_iterator_of_multibyte
{
    using value_type = typename std::iterator_traits<T>::value_type;
    enum
    {
        value = sizeof(value_type) > 1
    };
};

template<typename IteratorType>
struct iterator_input_adapter_factory<IteratorType, enable_if_t<is_iterator_of_multibyte<IteratorType>::value>>
{
    using iterator_type = IteratorType;
    using char_type = typename std::iterator_traits<iterator_type>::value_type;
    using base_adapter_type = iterator_input_adapter<iterator_type>;
    using adapter_type = wide_string_input_adapter<base_adapter_type, char_type>;

    static adapter_type create(IteratorType first, IteratorType last)
    {
        return adapter_type(base_adapter_type(std::move(first), std::move(last)));
    }
};

// General purpose iterator-based input
template<typename IteratorType>
typename iterator_input_adapter_factory<IteratorType>::adapter_type input_adapter(IteratorType first, IteratorType last)
{
    using factory_type = iterator_input_adapter_factory<IteratorType>;
    return factory_type::create(first, last);
}

// Convenience shorthand from container to iterator
// Enables ADL on begin(container) and end(container)
// Encloses the using declarations in namespace for not to leak them to outside scope

namespace container_input_adapter_factory_impl
{

using std::begin;
using std::end;

template<typename ContainerType, typename Enable = void>
struct container_input_adapter_factory {};

template<typename ContainerType>
struct container_input_adapter_factory< ContainerType,
       void_t<decltype(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>()))>>
       {
           using adapter_type = decltype(input_adapter(begin(std::declval<ContainerType>()), end(std::declval<ContainerType>())));

           static adapter_type create(const ContainerType& container)
{
    return input_adapter(begin(container), end(container));
}
       };

} // namespace container_input_adapter_factory_impl

template<typename ContainerType>
typename container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::adapter_type input_adapter(const ContainerType& container)
{
    return container_input_adapter_factory_impl::container_input_adapter_factory<ContainerType>::create(container);
}

#ifndef JSON_NO_IO
// Special cases with fast paths
inline file_input_adapter input_adapter(std::FILE* file)
{
    return file_input_adapter(file);
}

inline input_stream_adapter input_adapter(std::istream& stream)
{
    return input_stream_adapter(stream);
}

inline input_stream_adapter input_adapter(std::istream&& stream)
{
    return input_stream_adapter(stream);
}
#endif  // JSON_NO_IO

using contiguous_bytes_input_adapter = decltype(input_adapter(std::declval<const char*>(), std::declval<const char*>()));

// Null-delimited strings, and the like.
template < typename CharT,
           typename std::enable_if <
               std::is_pointer<CharT>::value&&
               !std::is_array<CharT>::value&&
               std::is_integral<typename std::remove_pointer<CharT>::type>::value&&
               sizeof(typename std::remove_pointer<CharT>::type) == 1,
               int >::type = 0 >
contiguous_bytes_input_adapter input_adapter(CharT b)
{
    auto length = std::strlen(reinterpret_cast<const char*>(b));
    const auto* ptr = reinterpret_cast<const char*>(b);
    return input_adapter(ptr, ptr + length);
}

template<typename T, std::size_t N>
auto input_adapter(T (&array)[N]) -> decltype(input_adapter(array, array + N)) // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
{
    return input_adapter(array, array + N);
}

// This class only handles inputs of input_buffer_adapter type.
// It's required so that expressions like {ptr, len} can be implicitely casted
// to the correct adapter.
class span_input_adapter
{
  public:
    template < typename CharT,
               typename std::enable_if <
                   std::is_pointer<CharT>::value&&
                   std::is_integral<typename std::remove_pointer<CharT>::type>::value&&
                   sizeof(typename std::remove_pointer<CharT>::type) == 1,
                   int >::type = 0 >
    span_input_adapter(CharT b, std::size_t l)
        : ia(reinterpret_cast<const char*>(b), reinterpret_cast<const char*>(b) + l) {}

    template<class IteratorType,
             typename std::enable_if<
                 std::is_same<typename iterator_traits<IteratorType>::iterator_category, std::random_access_iterator_tag>::value,
                 int>::type = 0>
    span_input_adapter(IteratorType first, IteratorType last)
        : ia(input_adapter(first, last)) {}

    contiguous_bytes_input_adapter&& get()
    {
        return std::move(ia); // NOLINT(hicpp-move-const-arg,performance-move-const-arg)
    }

  private:
    contiguous_bytes_input_adapter ia;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/input/json_sax.hpp>


#include <cstddef>
#include <string> // string
#include <utility> // move
#include <vector> // vector

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{

/*!
@brief SAX interface

This class describes the SAX interface used by @ref nlohmann::json::sax_parse.
Each function is called in different situations while the input is parsed. The
boolean return value informs the parser whether to continue processing the
input.
*/
template<typename BasicJsonType>
struct json_sax
{
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;

    /*!
    @brief a null value was read
    @return whether parsing should proceed
    */
    virtual bool null() = 0;

    /*!
    @brief a boolean value was read
    @param[in] val  boolean value
    @return whether parsing should proceed
    */
    virtual bool boolean(bool val) = 0;

    /*!
    @brief an integer number was read
    @param[in] val  integer value
    @return whether parsing should proceed
    */
    virtual bool number_integer(number_integer_t val) = 0;

    /*!
    @brief an unsigned integer number was read
    @param[in] val  unsigned integer value
    @return whether parsing should proceed
    */
    virtual bool number_unsigned(number_unsigned_t val) = 0;

    /*!
    @brief an floating-point number was read
    @param[in] val  floating-point value
    @param[in] s    raw token value
    @return whether parsing should proceed
    */
    virtual bool number_float(number_float_t val, const string_t& s) = 0;

    /*!
    @brief a string was read
    @param[in] val  string value
    @return whether parsing should proceed
    @note It is safe to move the passed string.
    */
    virtual bool string(string_t& val) = 0;

    /*!
    @brief a binary string was read
    @param[in] val  binary value
    @return whether parsing should proceed
    @note It is safe to move the passed binary.
    */
    virtual bool binary(binary_t& val) = 0;

    /*!
    @brief the beginning of an object was read
    @param[in] elements  number of object elements or -1 if unknown
    @return whether parsing should proceed
    @note binary formats may report the number of elements
    */
    virtual bool start_object(std::size_t elements) = 0;

    /*!
    @brief an object key was read
    @param[in] val  object key
    @return whether parsing should proceed
    @note It is safe to move the passed string.
    */
    virtual bool key(string_t& val) = 0;

    /*!
    @brief the end of an object was read
    @return whether parsing should proceed
    */
    virtual bool end_object() = 0;

    /*!
    @brief the beginning of an array was read
    @param[in] elements  number of array elements or -1 if unknown
    @return whether parsing should proceed
    @note binary formats may report the number of elements
    */
    virtual bool start_array(std::size_t elements) = 0;

    /*!
    @brief the end of an array was read
    @return whether parsing should proceed
    */
    virtual bool end_array() = 0;

    /*!
    @brief a parse error occurred
    @param[in] position    the position in the input where the error occurs
    @param[in] last_token  the last read token
    @param[in] ex          an exception object describing the error
    @return whether parsing should proceed (must return false)
    */
    virtual bool parse_error(std::size_t position,
                             const std::string& last_token,
                             const detail::exception& ex) = 0;

    json_sax() = default;
    json_sax(const json_sax&) = default;
    json_sax(json_sax&&) noexcept = default;
    json_sax& operator=(const json_sax&) = default;
    json_sax& operator=(json_sax&&) noexcept = default;
    virtual ~json_sax() = default;
};


namespace detail
{
/*!
@brief SAX implementation to create a JSON value from SAX events

This class implements the @ref json_sax interface and processes the SAX events
to create a JSON value which makes it basically a DOM parser. The structure or
hierarchy of the JSON value is managed by the stack `ref_stack` which contains
a pointer to the respective array or object for each recursion depth.

After successful parsing, the value that is passed by reference to the
constructor contains the parsed value.

@tparam BasicJsonType  the JSON type
*/
template<typename BasicJsonType>
class json_sax_dom_parser
{
  public:
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;

    /*!
    @param[in,out] r  reference to a JSON value that is manipulated while
                       parsing
    @param[in] allow_exceptions_  whether parse errors yield exceptions
    */
    explicit json_sax_dom_parser(BasicJsonType& r, const bool allow_exceptions_ = true)
        : root(r), allow_exceptions(allow_exceptions_)
    {}

    // make class move-only
    json_sax_dom_parser(const json_sax_dom_parser&) = delete;
    json_sax_dom_parser(json_sax_dom_parser&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    json_sax_dom_parser& operator=(const json_sax_dom_parser&) = delete;
    json_sax_dom_parser& operator=(json_sax_dom_parser&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    ~json_sax_dom_parser() = default;

    bool null()
    {
        handle_value(nullptr);
        return true;
    }

    bool boolean(bool val)
    {
        handle_value(val);
        return true;
    }

    bool number_integer(number_integer_t val)
    {
        handle_value(val);
        return true;
    }

    bool number_unsigned(number_unsigned_t val)
    {
        handle_value(val);
        return true;
    }

    bool number_float(number_float_t val, const string_t& /*unused*/)
    {
        handle_value(val);
        return true;
    }

    bool string(string_t& val)
    {
        handle_value(val);
        return true;
    }

    bool binary(binary_t& val)
    {
        handle_value(std::move(val));
        return true;
    }

    bool start_object(std::size_t len)
    {
        ref_stack.push_back(handle_value(BasicJsonType::value_t::object));

        if (JSON_HEDLEY_UNLIKELY(len != std::size_t(-1) && len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408, "excessive object size: " + std::to_string(len), *ref_stack.back()));
        }

        return true;
    }

    bool key(string_t& val)
    {
        // add null at given key and store the reference for later
        object_element = &(ref_stack.back()->m_value.object->operator[](val));
        return true;
    }

    bool end_object()
    {
        ref_stack.back()->set_parents();
        ref_stack.pop_back();
        return true;
    }

    bool start_array(std::size_t len)
    {
        ref_stack.push_back(handle_value(BasicJsonType::value_t::array));

        if (JSON_HEDLEY_UNLIKELY(len != std::size_t(-1) && len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408, "excessive array size: " + std::to_string(len), *ref_stack.back()));
        }

        return true;
    }

    bool end_array()
    {
        ref_stack.back()->set_parents();
        ref_stack.pop_back();
        return true;
    }

    template<class Exception>
    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/,
                     const Exception& ex)
    {
        errored = true;
        static_cast<void>(ex);
        if (allow_exceptions)
        {
            JSON_THROW(ex);
        }
        return false;
    }

    constexpr bool is_errored() const
    {
        return errored;
    }

  private:
    /*!
    @invariant If the ref stack is empty, then the passed value will be the new
               root.
    @invariant If the ref stack contains a value, then it is an array or an
               object to which we can add elements
    */
    template<typename Value>
    JSON_HEDLEY_RETURNS_NON_NULL
    BasicJsonType* handle_value(Value&& v)
    {
        if (ref_stack.empty())
        {
            root = BasicJsonType(std::forward<Value>(v));
            return &root;
        }

        JSON_ASSERT(ref_stack.back()->is_array() || ref_stack.back()->is_object());

        if (ref_stack.back()->is_array())
        {
            ref_stack.back()->m_value.array->emplace_back(std::forward<Value>(v));
            return &(ref_stack.back()->m_value.array->back());
        }

        JSON_ASSERT(ref_stack.back()->is_object());
        JSON_ASSERT(object_element);
        *object_element = BasicJsonType(std::forward<Value>(v));
        return object_element;
    }

    /// the parsed JSON value
    BasicJsonType& root;
    /// stack to model hierarchy of values
    std::vector<BasicJsonType*> ref_stack {};
    /// helper to hold the reference for the next object element
    BasicJsonType* object_element = nullptr;
    /// whether a syntax error occurred
    bool errored = false;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
};

template<typename BasicJsonType>
class json_sax_dom_callback_parser
{
  public:
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;
    using parser_callback_t = typename BasicJsonType::parser_callback_t;
    using parse_event_t = typename BasicJsonType::parse_event_t;

    json_sax_dom_callback_parser(BasicJsonType& r,
                                 const parser_callback_t cb,
                                 const bool allow_exceptions_ = true)
        : root(r), callback(cb), allow_exceptions(allow_exceptions_)
    {
        keep_stack.push_back(true);
    }

    // make class move-only
    json_sax_dom_callback_parser(const json_sax_dom_callback_parser&) = delete;
    json_sax_dom_callback_parser(json_sax_dom_callback_parser&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    json_sax_dom_callback_parser& operator=(const json_sax_dom_callback_parser&) = delete;
    json_sax_dom_callback_parser& operator=(json_sax_dom_callback_parser&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    ~json_sax_dom_callback_parser() = default;

    bool null()
    {
        handle_value(nullptr);
        return true;
    }

    bool boolean(bool val)
    {
        handle_value(val);
        return true;
    }

    bool number_integer(number_integer_t val)
    {
        handle_value(val);
        return true;
    }

    bool number_unsigned(number_unsigned_t val)
    {
        handle_value(val);
        return true;
    }

    bool number_float(number_float_t val, const string_t& /*unused*/)
    {
        handle_value(val);
        return true;
    }

    bool string(string_t& val)
    {
        handle_value(val);
        return true;
    }

    bool binary(binary_t& val)
    {
        handle_value(std::move(val));
        return true;
    }

    bool start_object(std::size_t len)
    {
        // check callback for object start
        const bool keep = callback(static_cast<int>(ref_stack.size()), parse_event_t::object_start, discarded);
        keep_stack.push_back(keep);

        auto val = handle_value(BasicJsonType::value_t::object, true);
        ref_stack.push_back(val.second);

        // check object limit
        if (ref_stack.back() && JSON_HEDLEY_UNLIKELY(len != std::size_t(-1) && len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408, "excessive object size: " + std::to_string(len), *ref_stack.back()));
        }

        return true;
    }

    bool key(string_t& val)
    {
        BasicJsonType k = BasicJsonType(val);

        // check callback for key
        const bool keep = callback(static_cast<int>(ref_stack.size()), parse_event_t::key, k);
        key_keep_stack.push_back(keep);

        // add discarded value at given key and store the reference for later
        if (keep && ref_stack.back())
        {
            object_element = &(ref_stack.back()->m_value.object->operator[](val) = discarded);
        }

        return true;
    }

    bool end_object()
    {
        if (ref_stack.back())
        {
            if (!callback(static_cast<int>(ref_stack.size()) - 1, parse_event_t::object_end, *ref_stack.back()))
            {
                // discard object
                *ref_stack.back() = discarded;
            }
            else
            {
                ref_stack.back()->set_parents();
            }
        }

        JSON_ASSERT(!ref_stack.empty());
        JSON_ASSERT(!keep_stack.empty());
        ref_stack.pop_back();
        keep_stack.pop_back();

        if (!ref_stack.empty() && ref_stack.back() && ref_stack.back()->is_structured())
        {
            // remove discarded value
            for (auto it = ref_stack.back()->begin(); it != ref_stack.back()->end(); ++it)
            {
                if (it->is_discarded())
                {
                    ref_stack.back()->erase(it);
                    break;
                }
            }
        }

        return true;
    }

    bool start_array(std::size_t len)
    {
        const bool keep = callback(static_cast<int>(ref_stack.size()), parse_event_t::array_start, discarded);
        keep_stack.push_back(keep);

        auto val = handle_value(BasicJsonType::value_t::array, true);
        ref_stack.push_back(val.second);

        // check array limit
        if (ref_stack.back() && JSON_HEDLEY_UNLIKELY(len != std::size_t(-1) && len > ref_stack.back()->max_size()))
        {
            JSON_THROW(out_of_range::create(408, "excessive array size: " + std::to_string(len), *ref_stack.back()));
        }

        return true;
    }

    bool end_array()
    {
        bool keep = true;

        if (ref_stack.back())
        {
            keep = callback(static_cast<int>(ref_stack.size()) - 1, parse_event_t::array_end, *ref_stack.back());
            if (keep)
            {
                ref_stack.back()->set_parents();
            }
            else
            {
                // discard array
                *ref_stack.back() = discarded;
            }
        }

        JSON_ASSERT(!ref_stack.empty());
        JSON_ASSERT(!keep_stack.empty());
        ref_stack.pop_back();
        keep_stack.pop_back();

        // remove discarded value
        if (!keep && !ref_stack.empty() && ref_stack.back()->is_array())
        {
            ref_stack.back()->m_value.array->pop_back();
        }

        return true;
    }

    template<class Exception>
    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/,
                     const Exception& ex)
    {
        errored = true;
        static_cast<void>(ex);
        if (allow_exceptions)
        {
            JSON_THROW(ex);
        }
        return false;
    }

    constexpr bool is_errored() const
    {
        return errored;
    }

  private:
    /*!
    @param[in] v  value to add to the JSON value we build during parsing
    @param[in] skip_callback  whether we should skip calling the callback
               function; this is required after start_array() and
               start_object() SAX events, because otherwise we would call the
               callback function with an empty array or object, respectively.

    @invariant If the ref stack is empty, then the passed value will be the new
               root.
    @invariant If the ref stack contains a value, then it is an array or an
               object to which we can add elements

    @return pair of boolean (whether value should be kept) and pointer (to the
            passed value in the ref_stack hierarchy; nullptr if not kept)
    */
    template<typename Value>
    std::pair<bool, BasicJsonType*> handle_value(Value&& v, const bool skip_callback = false)
    {
        JSON_ASSERT(!keep_stack.empty());

        // do not handle this value if we know it would be added to a discarded
        // container
        if (!keep_stack.back())
        {
            return {false, nullptr};
        }

        // create value
        auto value = BasicJsonType(std::forward<Value>(v));

        // check callback
        const bool keep = skip_callback || callback(static_cast<int>(ref_stack.size()), parse_event_t::value, value);

        // do not handle this value if we just learnt it shall be discarded
        if (!keep)
        {
            return {false, nullptr};
        }

        if (ref_stack.empty())
        {
            root = std::move(value);
            return {true, &root};
        }

        // skip this value if we already decided to skip the parent
        // (https://github.com/nlohmann/json/issues/971#issuecomment-413678360)
        if (!ref_stack.back())
        {
            return {false, nullptr};
        }

        // we now only expect arrays and objects
        JSON_ASSERT(ref_stack.back()->is_array() || ref_stack.back()->is_object());

        // array
        if (ref_stack.back()->is_array())
        {
            ref_stack.back()->m_value.array->emplace_back(std::move(value));
            return {true, &(ref_stack.back()->m_value.array->back())};
        }

        // object
        JSON_ASSERT(ref_stack.back()->is_object());
        // check if we should store an element for the current key
        JSON_ASSERT(!key_keep_stack.empty());
        const bool store_element = key_keep_stack.back();
        key_keep_stack.pop_back();

        if (!store_element)
        {
            return {false, nullptr};
        }

        JSON_ASSERT(object_element);
        *object_element = std::move(value);
        return {true, object_element};
    }

    /// the parsed JSON value
    BasicJsonType& root;
    /// stack to model hierarchy of values
    std::vector<BasicJsonType*> ref_stack {};
    /// stack to manage which values to keep
    std::vector<bool> keep_stack {};
    /// stack to manage which object keys to keep
    std::vector<bool> key_keep_stack {};
    /// helper to hold the reference for the next object element
    BasicJsonType* object_element = nullptr;
    /// whether a syntax error occurred
    bool errored = false;
    /// callback function
    const parser_callback_t callback = nullptr;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
    /// a discarded value for the callback
    BasicJsonType discarded = BasicJsonType::value_t::discarded;
};

template<typename BasicJsonType>
class json_sax_acceptor
{
  public:
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;

    bool null()
    {
        return true;
    }

    bool boolean(bool /*unused*/)
    {
        return true;
    }

    bool number_integer(number_integer_t /*unused*/)
    {
        return true;
    }

    bool number_unsigned(number_unsigned_t /*unused*/)
    {
        return true;
    }

    bool number_float(number_float_t /*unused*/, const string_t& /*unused*/)
    {
        return true;
    }

    bool string(string_t& /*unused*/)
    {
        return true;
    }

    bool binary(binary_t& /*unused*/)
    {
        return true;
    }

    bool start_object(std::size_t /*unused*/ = std::size_t(-1))
    {
        return true;
    }

    bool key(string_t& /*unused*/)
    {
        return true;
    }

    bool end_object()
    {
        return true;
    }

    bool start_array(std::size_t /*unused*/ = std::size_t(-1))
    {
        return true;
    }

    bool end_array()
    {
        return true;
    }

    bool parse_error(std::size_t /*unused*/, const std::string& /*unused*/, const detail::exception& /*unused*/)
    {
        return false;
    }
};
}  // namespace detail

}  // namespace nlohmann

// #include <nlohmann/detail/input/lexer.hpp>


#include <array> // array
#include <clocale> // localeconv
#include <cstddef> // size_t
#include <cstdio> // snprintf
#include <cstdlib> // strtof, strtod, strtold, strtoll, strtoull
#include <initializer_list> // initializer_list
#include <string> // char_traits, string
#include <utility> // move
#include <vector> // vector

// #include <nlohmann/detail/input/input_adapters.hpp>

// #include <nlohmann/detail/input/position_t.hpp>

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
///////////
// lexer //
///////////

template<typename BasicJsonType>
class lexer_base
{
  public:
    /// token types for the parser
    enum class token_type
    {
        uninitialized,    ///< indicating the scanner is uninitialized
        literal_true,     ///< the `true` literal
        literal_false,    ///< the `false` literal
        literal_null,     ///< the `null` literal
        value_string,     ///< a string -- use get_string() for actual value
        value_unsigned,   ///< an unsigned integer -- use get_number_unsigned() for actual value
        value_integer,    ///< a signed integer -- use get_number_integer() for actual value
        value_float,      ///< an floating point number -- use get_number_float() for actual value
        begin_array,      ///< the character for array begin `[`
        begin_object,     ///< the character for object begin `{`
        end_array,        ///< the character for array end `]`
        end_object,       ///< the character for object end `}`
        name_separator,   ///< the name separator `:`
        value_separator,  ///< the value separator `,`
        parse_error,      ///< indicating a parse error
        end_of_input,     ///< indicating the end of the input buffer
        literal_or_value  ///< a literal or the begin of a value (only for diagnostics)
    };

    /// return name of values of type token_type (only used for errors)
    JSON_HEDLEY_RETURNS_NON_NULL
    JSON_HEDLEY_CONST
    static const char* token_type_name(const token_type t) noexcept
    {
        switch (t)
        {
            case token_type::uninitialized:
                return "<uninitialized>";
            case token_type::literal_true:
                return "true literal";
            case token_type::literal_false:
                return "false literal";
            case token_type::literal_null:
                return "null literal";
            case token_type::value_string:
                return "string literal";
            case token_type::value_unsigned:
            case token_type::value_integer:
            case token_type::value_float:
                return "number literal";
            case token_type::begin_array:
                return "'['";
            case token_type::begin_object:
                return "'{'";
            case token_type::end_array:
                return "']'";
            case token_type::end_object:
                return "'}'";
            case token_type::name_separator:
                return "':'";
            case token_type::value_separator:
                return "','";
            case token_type::parse_error:
                return "<parse error>";
            case token_type::end_of_input:
                return "end of input";
            case token_type::literal_or_value:
                return "'[', '{', or a literal";
            // LCOV_EXCL_START
            default: // catch non-enum values
                return "unknown token";
                // LCOV_EXCL_STOP
        }
    }
};
/*!
@brief lexical analysis

This class organizes the lexical analysis during JSON deserialization.
*/
template<typename BasicJsonType, typename InputAdapterType>
class lexer : public lexer_base<BasicJsonType>
{
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using char_type = typename InputAdapterType::char_type;
    using char_int_type = typename std::char_traits<char_type>::int_type;

  public:
    using token_type = typename lexer_base<BasicJsonType>::token_type;

    explicit lexer(InputAdapterType&& adapter, bool ignore_comments_ = false) noexcept
        : ia(std::move(adapter))
        , ignore_comments(ignore_comments_)
        , decimal_point_char(static_cast<char_int_type>(get_decimal_point()))
    {}

    // delete because of pointer members
    lexer(const lexer&) = delete;
    lexer(lexer&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    lexer& operator=(lexer&) = delete;
    lexer& operator=(lexer&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    ~lexer() = default;

  private:
    /////////////////////
    // locales
    /////////////////////

    /// return the locale-dependent decimal point
    JSON_HEDLEY_PURE
    static char get_decimal_point() noexcept
    {
        const auto* loc = localeconv();
        JSON_ASSERT(loc != nullptr);
        return (loc->decimal_point == nullptr) ? '.' : *(loc->decimal_point);
    }

    /////////////////////
    // scan functions
    /////////////////////

    /*!
    @brief get codepoint from 4 hex characters following `\u`

    For input "\u c1 c2 c3 c4" the codepoint is:
      (c1 * 0x1000) + (c2 * 0x0100) + (c3 * 0x0010) + c4
    = (c1 << 12) + (c2 << 8) + (c3 << 4) + (c4 << 0)

    Furthermore, the possible characters '0'..'9', 'A'..'F', and 'a'..'f'
    must be converted to the integers 0x0..0x9, 0xA..0xF, 0xA..0xF, resp. The
    conversion is done by subtracting the offset (0x30, 0x37, and 0x57)
    between the ASCII value of the character and the desired integer value.

    @return codepoint (0x0000..0xFFFF) or -1 in case of an error (e.g. EOF or
            non-hex character)
    */
    int get_codepoint()
    {
        // this function only makes sense after reading `\u`
        JSON_ASSERT(current == 'u');
        int codepoint = 0;

        const auto factors = { 12u, 8u, 4u, 0u };
        for (const auto factor : factors)
        {
            get();

            if (current >= '0' && current <= '9')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x30u) << factor);
            }
            else if (current >= 'A' && current <= 'F')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x37u) << factor);
            }
            else if (current >= 'a' && current <= 'f')
            {
                codepoint += static_cast<int>((static_cast<unsigned int>(current) - 0x57u) << factor);
            }
            else
            {
                return -1;
            }
        }

        JSON_ASSERT(0x0000 <= codepoint && codepoint <= 0xFFFF);
        return codepoint;
    }

    /*!
    @brief check if the next byte(s) are inside a given range

    Adds the current byte and, for each passed range, reads a new byte and
    checks if it is inside the range. If a violation was detected, set up an
    error message and return false. Otherwise, return true.

    @param[in] ranges  list of integers; interpreted as list of pairs of
                       inclusive lower and upper bound, respectively

    @pre The passed list @a ranges must have 2, 4, or 6 elements; that is,
         1, 2, or 3 pairs. This precondition is enforced by an assertion.

    @return true if and only if no range violation was detected
    */
    bool next_byte_in_range(std::initializer_list<char_int_type> ranges)
    {
        JSON_ASSERT(ranges.size() == 2 || ranges.size() == 4 || ranges.size() == 6);
        add(current);

        for (auto range = ranges.begin(); range != ranges.end(); ++range)
        {
            get();
            if (JSON_HEDLEY_LIKELY(*range <= current && current <= *(++range)))
            {
                add(current);
            }
            else
            {
                error_message = "invalid string: ill-formed UTF-8 byte";
                return false;
            }
        }

        return true;
    }

    /*!
    @brief scan a string literal

    This function scans a string according to Sect. 7 of RFC 8259. While
    scanning, bytes are escaped and copied into buffer token_buffer. Then the
    function returns successfully, token_buffer is *not* null-terminated (as it
    may contain \0 bytes), and token_buffer.size() is the number of bytes in the
    string.

    @return token_type::value_string if string could be successfully scanned,
            token_type::parse_error otherwise

    @note In case of errors, variable error_message contains a textual
          description.
    */
    token_type scan_string()
    {
        // reset token_buffer (ignore opening quote)
        reset();

        // we entered the function by reading an open quote
        JSON_ASSERT(current == '\"');

        while (true)
        {
            // get next character
            switch (get())
            {
                // end of file while parsing string
                case std::char_traits<char_type>::eof():
                {
                    error_message = "invalid string: missing closing quote";
                    return token_type::parse_error;
                }

                // closing quote
                case '\"':
                {
                    return token_type::value_string;
                }

                // escapes
                case '\\':
                {
                    switch (get())
                    {
                        // quotation mark
                        case '\"':
                            add('\"');
                            break;
                        // reverse solidus
                        case '\\':
                            add('\\');
                            break;
                        // solidus
                        case '/':
                            add('/');
                            break;
                        // backspace
                        case 'b':
                            add('\b');
                            break;
                        // form feed
                        case 'f':
                            add('\f');
                            break;
                        // line feed
                        case 'n':
                            add('\n');
                            break;
                        // carriage return
                        case 'r':
                            add('\r');
                            break;
                        // tab
                        case 't':
                            add('\t');
                            break;

                        // unicode escapes
                        case 'u':
                        {
                            const int codepoint1 = get_codepoint();
                            int codepoint = codepoint1; // start with codepoint1

                            if (JSON_HEDLEY_UNLIKELY(codepoint1 == -1))
                            {
                                error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                return token_type::parse_error;
                            }

                            // check if code point is a high surrogate
                            if (0xD800 <= codepoint1 && codepoint1 <= 0xDBFF)
                            {
                                // expect next \uxxxx entry
                                if (JSON_HEDLEY_LIKELY(get() == '\\' && get() == 'u'))
                                {
                                    const int codepoint2 = get_codepoint();

                                    if (JSON_HEDLEY_UNLIKELY(codepoint2 == -1))
                                    {
                                        error_message = "invalid string: '\\u' must be followed by 4 hex digits";
                                        return token_type::parse_error;
                                    }

                                    // check if codepoint2 is a low surrogate
                                    if (JSON_HEDLEY_LIKELY(0xDC00 <= codepoint2 && codepoint2 <= 0xDFFF))
                                    {
                                        // overwrite codepoint
                                        codepoint = static_cast<int>(
                                                        // high surrogate occupies the most significant 22 bits
                                                        (static_cast<unsigned int>(codepoint1) << 10u)
                                                        // low surrogate occupies the least significant 15 bits
                                                        + static_cast<unsigned int>(codepoint2)
                                                        // there is still the 0xD800, 0xDC00 and 0x10000 noise
                                                        // in the result so we have to subtract with:
                                                        // (0xD800 << 10) + DC00 - 0x10000 = 0x35FDC00
                                                        - 0x35FDC00u);
                                    }
                                    else
                                    {
                                        error_message = "invalid string: surrogate U+D800..U+DBFF must be followed by U+DC00..U+DFFF";
                                        return token_type::parse_error;
                                    }
                                }
                                else
                                {
                                    error_message = "invalid string: surrogate U+D800..U+DBFF must be followed by U+DC00..U+DFFF";
                                    return token_type::parse_error;
                                }
                            }
                            else
                            {
                                if (JSON_HEDLEY_UNLIKELY(0xDC00 <= codepoint1 && codepoint1 <= 0xDFFF))
                                {
                                    error_message = "invalid string: surrogate U+DC00..U+DFFF must follow U+D800..U+DBFF";
                                    return token_type::parse_error;
                                }
                            }

                            // result of the above calculation yields a proper codepoint
                            JSON_ASSERT(0x00 <= codepoint && codepoint <= 0x10FFFF);

                            // translate codepoint into bytes
                            if (codepoint < 0x80)
                            {
                                // 1-byte characters: 0xxxxxxx (ASCII)
                                add(static_cast<char_int_type>(codepoint));
                            }
                            else if (codepoint <= 0x7FF)
                            {
                                // 2-byte characters: 110xxxxx 10xxxxxx
                                add(static_cast<char_int_type>(0xC0u | (static_cast<unsigned int>(codepoint) >> 6u)));
                                add(static_cast<char_int_type>(0x80u | (static_cast<unsigned int>(codepoint) & 0x3Fu)));
                            }
                            else if (codepoint <= 0xFFFF)
                            {
                                // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
                                add(static_cast<char_int_type>(0xE0u | (static_cast<unsigned int>(codepoint) >> 12u)));
                                add(static_cast<char_int_type>(0x80u | ((static_cast<unsigned int>(codepoint) >> 6u) & 0x3Fu)));
                                add(static_cast<char_int_type>(0x80u | (static_cast<unsigned int>(codepoint) & 0x3Fu)));
                            }
                            else
                            {
                                // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                                add(static_cast<char_int_type>(0xF0u | (static_cast<unsigned int>(codepoint) >> 18u)));
                                add(static_cast<char_int_type>(0x80u | ((static_cast<unsigned int>(codepoint) >> 12u) & 0x3Fu)));
                                add(static_cast<char_int_type>(0x80u | ((static_cast<unsigned int>(codepoint) >> 6u) & 0x3Fu)));
                                add(static_cast<char_int_type>(0x80u | (static_cast<unsigned int>(codepoint) & 0x3Fu)));
                            }

                            break;
                        }

                        // other characters after escape
                        default:
                            error_message = "invalid string: forbidden character after backslash";
                            return token_type::parse_error;
                    }

                    break;
                }

                // invalid control characters
                case 0x00:
                {
                    error_message = "invalid string: control character U+0000 (NUL) must be escaped to \\u0000";
                    return token_type::parse_error;
                }

                case 0x01:
                {
                    error_message = "invalid string: control character U+0001 (SOH) must be escaped to \\u0001";
                    return token_type::parse_error;
                }

                case 0x02:
                {
                    error_message = "invalid string: control character U+0002 (STX) must be escaped to \\u0002";
                    return token_type::parse_error;
                }

                case 0x03:
                {
                    error_message = "invalid string: control character U+0003 (ETX) must be escaped to \\u0003";
                    return token_type::parse_error;
                }

                case 0x04:
                {
                    error_message = "invalid string: control character U+0004 (EOT) must be escaped to \\u0004";
                    return token_type::parse_error;
                }

                case 0x05:
                {
                    error_message = "invalid string: control character U+0005 (ENQ) must be escaped to \\u0005";
                    return token_type::parse_error;
                }

                case 0x06:
                {
                    error_message = "invalid string: control character U+0006 (ACK) must be escaped to \\u0006";
                    return token_type::parse_error;
                }

                case 0x07:
                {
                    error_message = "invalid string: control character U+0007 (BEL) must be escaped to \\u0007";
                    return token_type::parse_error;
                }

                case 0x08:
                {
                    error_message = "invalid string: control character U+0008 (BS) must be escaped to \\u0008 or \\b";
                    return token_type::parse_error;
                }

                case 0x09:
                {
                    error_message = "invalid string: control character U+0009 (HT) must be escaped to \\u0009 or \\t";
                    return token_type::parse_error;
                }

                case 0x0A:
                {
                    error_message = "invalid string: control character U+000A (LF) must be escaped to \\u000A or \\n";
                    return token_type::parse_error;
                }

                case 0x0B:
                {
                    error_message = "invalid string: control character U+000B (VT) must be escaped to \\u000B";
                    return token_type::parse_error;
                }

                case 0x0C:
                {
                    error_message = "invalid string: control character U+000C (FF) must be escaped to \\u000C or \\f";
                    return token_type::parse_error;
                }

                case 0x0D:
                {
                    error_message = "invalid string: control character U+000D (CR) must be escaped to \\u000D or \\r";
                    return token_type::parse_error;
                }

                case 0x0E:
                {
                    error_message = "invalid string: control character U+000E (SO) must be escaped to \\u000E";
                    return token_type::parse_error;
                }

                case 0x0F:
                {
                    error_message = "invalid string: control character U+000F (SI) must be escaped to \\u000F";
                    return token_type::parse_error;
                }

                case 0x10:
                {
                    error_message = "invalid string: control character U+0010 (DLE) must be escaped to \\u0010";
                    return token_type::parse_error;
                }

                case 0x11:
                {
                    error_message = "invalid string: control character U+0011 (DC1) must be escaped to \\u0011";
                    return token_type::parse_error;
                }

                case 0x12:
                {
                    error_message = "invalid string: control character U+0012 (DC2) must be escaped to \\u0012";
                    return token_type::parse_error;
                }

                case 0x13:
                {
                    error_message = "invalid string: control character U+0013 (DC3) must be escaped to \\u0013";
                    return token_type::parse_error;
                }

                case 0x14:
                {
                    error_message = "invalid string: control character U+0014 (DC4) must be escaped to \\u0014";
                    return token_type::parse_error;
                }

                case 0x15:
                {
                    error_message = "invalid string: control character U+0015 (NAK) must be escaped to \\u0015";
                    return token_type::parse_error;
                }

                case 0x16:
                {
                    error_message = "invalid string: control character U+0016 (SYN) must be escaped to \\u0016";
                    return token_type::parse_error;
                }

                case 0x17:
                {
                    error_message = "invalid string: control character U+0017 (ETB) must be escaped to \\u0017";
                    return token_type::parse_error;
                }

                case 0x18:
                {
                    error_message = "invalid string: control character U+0018 (CAN) must be escaped to \\u0018";
                    return token_type::parse_error;
                }

                case 0x19:
                {
                    error_message = "invalid string: control character U+0019 (EM) must be escaped to \\u0019";
                    return token_type::parse_error;
                }

                case 0x1A:
                {
                    error_message = "invalid string: control character U+001A (SUB) must be escaped to \\u001A";
                    return token_type::parse_error;
                }

                case 0x1B:
                {
                    error_message = "invalid string: control character U+001B (ESC) must be escaped to \\u001B";
                    return token_type::parse_error;
                }

                case 0x1C:
                {
                    error_message = "invalid string: control character U+001C (FS) must be escaped to \\u001C";
                    return token_type::parse_error;
                }

                case 0x1D:
                {
                    error_message = "invalid string: control character U+001D (GS) must be escaped to \\u001D";
                    return token_type::parse_error;
                }

                case 0x1E:
                {
                    error_message = "invalid string: control character U+001E (RS) must be escaped to \\u001E";
                    return token_type::parse_error;
                }

                case 0x1F:
                {
                    error_message = "invalid string: control character U+001F (US) must be escaped to \\u001F";
                    return token_type::parse_error;
                }

                // U+0020..U+007F (except U+0022 (quote) and U+005C (backspace))
                case 0x20:
                case 0x21:
                case 0x23:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x28:
                case 0x29:
                case 0x2A:
                case 0x2B:
                case 0x2C:
                case 0x2D:
                case 0x2E:
                case 0x2F:
                case 0x30:
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3A:
                case 0x3B:
                case 0x3C:
                case 0x3D:
                case 0x3E:
                case 0x3F:
                case 0x40:
                case 0x41:
                case 0x42:
                case 0x43:
                case 0x44:
                case 0x45:
                case 0x46:
                case 0x47:
                case 0x48:
                case 0x49:
                case 0x4A:
                case 0x4B:
                case 0x4C:
                case 0x4D:
                case 0x4E:
                case 0x4F:
                case 0x50:
                case 0x51:
                case 0x52:
                case 0x53:
                case 0x54:
                case 0x55:
                case 0x56:
                case 0x57:
                case 0x58:
                case 0x59:
                case 0x5A:
                case 0x5B:
                case 0x5D:
                case 0x5E:
                case 0x5F:
                case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                case 0x64:
                case 0x65:
                case 0x66:
                case 0x67:
                case 0x68:
                case 0x69:
                case 0x6A:
                case 0x6B:
                case 0x6C:
                case 0x6D:
                case 0x6E:
                case 0x6F:
                case 0x70:
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x76:
                case 0x77:
                case 0x78:
                case 0x79:
                case 0x7A:
                case 0x7B:
                case 0x7C:
                case 0x7D:
                case 0x7E:
                case 0x7F:
                {
                    add(current);
                    break;
                }

                // U+0080..U+07FF: bytes C2..DF 80..BF
                case 0xC2:
                case 0xC3:
                case 0xC4:
                case 0xC5:
                case 0xC6:
                case 0xC7:
                case 0xC8:
                case 0xC9:
                case 0xCA:
                case 0xCB:
                case 0xCC:
                case 0xCD:
                case 0xCE:
                case 0xCF:
                case 0xD0:
                case 0xD1:
                case 0xD2:
                case 0xD3:
                case 0xD4:
                case 0xD5:
                case 0xD6:
                case 0xD7:
                case 0xD8:
                case 0xD9:
                case 0xDA:
                case 0xDB:
                case 0xDC:
                case 0xDD:
                case 0xDE:
                case 0xDF:
                {
                    if (JSON_HEDLEY_UNLIKELY(!next_byte_in_range({0x80, 0xBF})))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+0800..U+0FFF: bytes E0 A0..BF 80..BF
                case 0xE0:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0xA0, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+1000..U+CFFF: bytes E1..EC 80..BF 80..BF
                // U+E000..U+FFFF: bytes EE..EF 80..BF 80..BF
                case 0xE1:
                case 0xE2:
                case 0xE3:
                case 0xE4:
                case 0xE5:
                case 0xE6:
                case 0xE7:
                case 0xE8:
                case 0xE9:
                case 0xEA:
                case 0xEB:
                case 0xEC:
                case 0xEE:
                case 0xEF:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+D000..U+D7FF: bytes ED 80..9F 80..BF
                case 0xED:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0x80, 0x9F, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+10000..U+3FFFF F0 90..BF 80..BF 80..BF
                case 0xF0:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0x90, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+40000..U+FFFFF F1..F3 80..BF 80..BF 80..BF
                case 0xF1:
                case 0xF2:
                case 0xF3:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0x80, 0xBF, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // U+100000..U+10FFFF F4 80..8F 80..BF 80..BF
                case 0xF4:
                {
                    if (JSON_HEDLEY_UNLIKELY(!(next_byte_in_range({0x80, 0x8F, 0x80, 0xBF, 0x80, 0xBF}))))
                    {
                        return token_type::parse_error;
                    }
                    break;
                }

                // remaining bytes (80..C1 and F5..FF) are ill-formed
                default:
                {
                    error_message = "invalid string: ill-formed UTF-8 byte";
                    return token_type::parse_error;
                }
            }
        }
    }

    /*!
     * @brief scan a comment
     * @return whether comment could be scanned successfully
     */
    bool scan_comment()
    {
        switch (get())
        {
            // single-line comments skip input until a newline or EOF is read
            case '/':
            {
                while (true)
                {
                    switch (get())
                    {
                        case '\n':
                        case '\r':
                        case std::char_traits<char_type>::eof():
                        case '\0':
                            return true;

                        default:
                            break;
                    }
                }
            }

            // multi-line comments skip input until */ is read
            case '*':
            {
                while (true)
                {
                    switch (get())
                    {
                        case std::char_traits<char_type>::eof():
                        case '\0':
                        {
                            error_message = "invalid comment; missing closing '*/'";
                            return false;
                        }

                        case '*':
                        {
                            switch (get())
                            {
                                case '/':
                                    return true;

                                default:
                                {
                                    unget();
                                    continue;
                                }
                            }
                        }

                        default:
                            continue;
                    }
                }
            }

            // unexpected character after reading '/'
            default:
            {
                error_message = "invalid comment; expecting '/' or '*' after '/'";
                return false;
            }
        }
    }

    JSON_HEDLEY_NON_NULL(2)
    static void strtof(float& f, const char* str, char** endptr) noexcept
    {
        f = std::strtof(str, endptr);
    }

    JSON_HEDLEY_NON_NULL(2)
    static void strtof(double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtod(str, endptr);
    }

    JSON_HEDLEY_NON_NULL(2)
    static void strtof(long double& f, const char* str, char** endptr) noexcept
    {
        f = std::strtold(str, endptr);
    }

    /*!
    @brief scan a number literal

    This function scans a string according to Sect. 6 of RFC 8259.

    The function is realized with a deterministic finite state machine derived
    from the grammar described in RFC 8259. Starting in state "init", the
    input is read and used to determined the next state. Only state "done"
    accepts the number. State "error" is a trap state to model errors. In the
    table below, "anything" means any character but the ones listed before.

    state    | 0        | 1-9      | e E      | +       | -       | .        | anything
    ---------|----------|----------|----------|---------|---------|----------|-----------
    init     | zero     | any1     | [error]  | [error] | minus   | [error]  | [error]
    minus    | zero     | any1     | [error]  | [error] | [error] | [error]  | [error]
    zero     | done     | done     | exponent | done    | done    | decimal1 | done
    any1     | any1     | any1     | exponent | done    | done    | decimal1 | done
    decimal1 | decimal2 | decimal2 | [error]  | [error] | [error] | [error]  | [error]
    decimal2 | decimal2 | decimal2 | exponent | done    | done    | done     | done
    exponent | any2     | any2     | [error]  | sign    | sign    | [error]  | [error]
    sign     | any2     | any2     | [error]  | [error] | [error] | [error]  | [error]
    any2     | any2     | any2     | done     | done    | done    | done     | done

    The state machine is realized with one label per state (prefixed with
    "scan_number_") and `goto` statements between them. The state machine
    contains cycles, but any cycle can be left when EOF is read. Therefore,
    the function is guaranteed to terminate.

    During scanning, the read bytes are stored in token_buffer. This string is
    then converted to a signed integer, an unsigned integer, or a
    floating-point number.

    @return token_type::value_unsigned, token_type::value_integer, or
            token_type::value_float if number could be successfully scanned,
            token_type::parse_error otherwise

    @note The scanner is independent of the current locale. Internally, the
          locale's decimal point is used instead of `.` to work with the
          locale-dependent converters.
    */
    token_type scan_number()  // lgtm [cpp/use-of-goto]
    {
        // reset token_buffer to store the number's bytes
        reset();

        // the type of the parsed number; initially set to unsigned; will be
        // changed if minus sign, decimal point or exponent is read
        token_type number_type = token_type::value_unsigned;

        // state (init): we just found out we need to scan a number
        switch (current)
        {
            case '-':
            {
                add(current);
                goto scan_number_minus;
            }

            case '0':
            {
                add(current);
                goto scan_number_zero;
            }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            // all other characters are rejected outside scan_number()
            default:            // LCOV_EXCL_LINE
                JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
        }

scan_number_minus:
        // state: we just parsed a leading minus sign
        number_type = token_type::value_integer;
        switch (get())
        {
            case '0':
            {
                add(current);
                goto scan_number_zero;
            }

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            default:
            {
                error_message = "invalid number; expected digit after '-'";
                return token_type::parse_error;
            }
        }

scan_number_zero:
        // state: we just parse a zero (maybe with a leading minus sign)
        switch (get())
        {
            case '.':
            {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_any1:
        // state: we just parsed a number 0-9 (maybe with a leading minus sign)
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any1;
            }

            case '.':
            {
                add(decimal_point_char);
                goto scan_number_decimal1;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_decimal1:
        // state: we just parsed a decimal point
        number_type = token_type::value_float;
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_decimal2;
            }

            default:
            {
                error_message = "invalid number; expected digit after '.'";
                return token_type::parse_error;
            }
        }

scan_number_decimal2:
        // we just parsed at least one number after a decimal point
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_decimal2;
            }

            case 'e':
            case 'E':
            {
                add(current);
                goto scan_number_exponent;
            }

            default:
                goto scan_number_done;
        }

scan_number_exponent:
        // we just parsed an exponent
        number_type = token_type::value_float;
        switch (get())
        {
            case '+':
            case '-':
            {
                add(current);
                goto scan_number_sign;
            }

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
            {
                error_message =
                    "invalid number; expected '+', '-', or digit after exponent";
                return token_type::parse_error;
            }
        }

scan_number_sign:
        // we just parsed an exponent sign
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
            {
                error_message = "invalid number; expected digit after exponent sign";
                return token_type::parse_error;
            }
        }

scan_number_any2:
        // we just parsed a number after the exponent or exponent sign
        switch (get())
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                add(current);
                goto scan_number_any2;
            }

            default:
                goto scan_number_done;
        }

scan_number_done:
        // unget the character after the number (we only read it to know that
        // we are done scanning a number)
        unget();

        char* endptr = nullptr; // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        errno = 0;

        // try to parse integers first and fall back to floats
        if (number_type == token_type::value_unsigned)
        {
            const auto x = std::strtoull(token_buffer.data(), &endptr, 10);

            // we checked the number format before
            JSON_ASSERT(endptr == token_buffer.data() + token_buffer.size());

            if (errno == 0)
            {
                value_unsigned = static_cast<number_unsigned_t>(x);
                if (value_unsigned == x)
                {
                    return token_type::value_unsigned;
                }
            }
        }
        else if (number_type == token_type::value_integer)
        {
            const auto x = std::strtoll(token_buffer.data(), &endptr, 10);

            // we checked the number format before
            JSON_ASSERT(endptr == token_buffer.data() + token_buffer.size());

            if (errno == 0)
            {
                value_integer = static_cast<number_integer_t>(x);
                if (value_integer == x)
                {
                    return token_type::value_integer;
                }
            }
        }

        // this code is reached if we parse a floating-point number or if an
        // integer conversion above failed
        strtof(value_float, token_buffer.data(), &endptr);

        // we checked the number format before
        JSON_ASSERT(endptr == token_buffer.data() + token_buffer.size());

        return token_type::value_float;
    }

    /*!
    @param[in] literal_text  the literal text to expect
    @param[in] length        the length of the passed literal text
    @param[in] return_type   the token type to return on success
    */
    JSON_HEDLEY_NON_NULL(2)
    token_type scan_literal(const char_type* literal_text, const std::size_t length,
                            token_type return_type)
    {
        JSON_ASSERT(std::char_traits<char_type>::to_char_type(current) == literal_text[0]);
        for (std::size_t i = 1; i < length; ++i)
        {
            if (JSON_HEDLEY_UNLIKELY(std::char_traits<char_type>::to_char_type(get()) != literal_text[i]))
            {
                error_message = "invalid literal";
                return token_type::parse_error;
            }
        }
        return return_type;
    }

    /////////////////////
    // input management
    /////////////////////

    /// reset token_buffer; current character is beginning of token
    void reset() noexcept
    {
        token_buffer.clear();
        token_string.clear();
        token_string.push_back(std::char_traits<char_type>::to_char_type(current));
    }

    /*
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a
    `std::char_traits<char>::eof()` in that case.  Stores the scanned characters
    for use in error messages.

    @return character read from the input
    */
    char_int_type get()
    {
        ++position.chars_read_total;
        ++position.chars_read_current_line;

        if (next_unget)
        {
            // just reset the next_unget variable and work with current
            next_unget = false;
        }
        else
        {
            current = ia.get_character();
        }

        if (JSON_HEDLEY_LIKELY(current != std::char_traits<char_type>::eof()))
        {
            token_string.push_back(std::char_traits<char_type>::to_char_type(current));
        }

        if (current == '\n')
        {
            ++position.lines_read;
            position.chars_read_current_line = 0;
        }

        return current;
    }

    /*!
    @brief unget current character (read it again on next get)

    We implement unget by setting variable next_unget to true. The input is not
    changed - we just simulate ungetting by modifying chars_read_total,
    chars_read_current_line, and token_string. The next call to get() will
    behave as if the unget character is read again.
    */
    void unget()
    {
        next_unget = true;

        --position.chars_read_total;

        // in case we "unget" a newline, we have to also decrement the lines_read
        if (position.chars_read_current_line == 0)
        {
            if (position.lines_read > 0)
            {
                --position.lines_read;
            }
        }
        else
        {
            --position.chars_read_current_line;
        }

        if (JSON_HEDLEY_LIKELY(current != std::char_traits<char_type>::eof()))
        {
            JSON_ASSERT(!token_string.empty());
            token_string.pop_back();
        }
    }

    /// add a character to token_buffer
    void add(char_int_type c)
    {
        token_buffer.push_back(static_cast<typename string_t::value_type>(c));
    }

  public:
    /////////////////////
    // value getters
    /////////////////////

    /// return integer value
    constexpr number_integer_t get_number_integer() const noexcept
    {
        return value_integer;
    }

    /// return unsigned integer value
    constexpr number_unsigned_t get_number_unsigned() const noexcept
    {
        return value_unsigned;
    }

    /// return floating-point value
    constexpr number_float_t get_number_float() const noexcept
    {
        return value_float;
    }

    /// return current string value (implicitly resets the token; useful only once)
    string_t& get_string()
    {
        return token_buffer;
    }

    /////////////////////
    // diagnostics
    /////////////////////

    /// return position of last read token
    constexpr position_t get_position() const noexcept
    {
        return position;
    }

    /// return the last read token (for errors only).  Will never contain EOF
    /// (an arbitrary value that is not a valid char value, often -1), because
    /// 255 may legitimately occur.  May contain NUL, which should be escaped.
    std::string get_token_string() const
    {
        // escape control characters
        std::string result;
        for (const auto c : token_string)
        {
            if (static_cast<unsigned char>(c) <= '\x1F')
            {
                // escape control characters
                std::array<char, 9> cs{{}};
                (std::snprintf)(cs.data(), cs.size(), "<U+%.4X>", static_cast<unsigned char>(c)); // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
                result += cs.data();
            }
            else
            {
                // add character as is
                result.push_back(static_cast<std::string::value_type>(c));
            }
        }

        return result;
    }

    /// return syntax error message
    JSON_HEDLEY_RETURNS_NON_NULL
    constexpr const char* get_error_message() const noexcept
    {
        return error_message;
    }

    /////////////////////
    // actual scanner
    /////////////////////

    /*!
    @brief skip the UTF-8 byte order mark
    @return true iff there is no BOM or the correct BOM has been skipped
    */
    bool skip_bom()
    {
        if (get() == 0xEF)
        {
            // check if we completely parse the BOM
            return get() == 0xBB && get() == 0xBF;
        }

        // the first character is not the beginning of the BOM; unget it to
        // process is later
        unget();
        return true;
    }

    void skip_whitespace()
    {
        do
        {
            get();
        }
        while (current == ' ' || current == '\t' || current == '\n' || current == '\r');
    }

    token_type scan()
    {
        // initially, skip the BOM
        if (position.chars_read_total == 0 && !skip_bom())
        {
            error_message = "invalid BOM; must be 0xEF 0xBB 0xBF if given";
            return token_type::parse_error;
        }

        // read next character and ignore whitespace
        skip_whitespace();

        // ignore comments
        while (ignore_comments && current == '/')
        {
            if (!scan_comment())
            {
                return token_type::parse_error;
            }

            // skip following whitespace
            skip_whitespace();
        }

        switch (current)
        {
            // structural characters
            case '[':
                return token_type::begin_array;
            case ']':
                return token_type::end_array;
            case '{':
                return token_type::begin_object;
            case '}':
                return token_type::end_object;
            case ':':
                return token_type::name_separator;
            case ',':
                return token_type::value_separator;

            // literals
            case 't':
            {
                std::array<char_type, 4> true_literal = {{char_type('t'), char_type('r'), char_type('u'), char_type('e')}};
                return scan_literal(true_literal.data(), true_literal.size(), token_type::literal_true);
            }
            case 'f':
            {
                std::array<char_type, 5> false_literal = {{char_type('f'), char_type('a'), char_type('l'), char_type('s'), char_type('e')}};
                return scan_literal(false_literal.data(), false_literal.size(), token_type::literal_false);
            }
            case 'n':
            {
                std::array<char_type, 4> null_literal = {{char_type('n'), char_type('u'), char_type('l'), char_type('l')}};
                return scan_literal(null_literal.data(), null_literal.size(), token_type::literal_null);
            }

            // string
            case '\"':
                return scan_string();

            // number
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                return scan_number();

            // end of input (the null byte is needed when parsing from
            // string literals)
            case '\0':
            case std::char_traits<char_type>::eof():
                return token_type::end_of_input;

            // error
            default:
                error_message = "invalid literal";
                return token_type::parse_error;
        }
    }

  private:
    /// input adapter
    InputAdapterType ia;

    /// whether comments should be ignored (true) or signaled as errors (false)
    const bool ignore_comments = false;

    /// the current character
    char_int_type current = std::char_traits<char_type>::eof();

    /// whether the next get() call should just return current
    bool next_unget = false;

    /// the start position of the current token
    position_t position {};

    /// raw input token string (for error messages)
    std::vector<char_type> token_string {};

    /// buffer for variable-length tokens (numbers, strings)
    string_t token_buffer {};

    /// a description of occurred lexer errors
    const char* error_message = "";

    // number values
    number_integer_t value_integer = 0;
    number_unsigned_t value_unsigned = 0;
    number_float_t value_float = 0;

    /// the decimal point
    const char_int_type decimal_point_char = '.';
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/is_sax.hpp>


#include <cstdint> // size_t
#include <utility> // declval
#include <string> // string

// #include <nlohmann/detail/meta/detected.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>


namespace nlohmann
{
namespace detail
{
template<typename T>
using null_function_t = decltype(std::declval<T&>().null());

template<typename T>
using boolean_function_t =
    decltype(std::declval<T&>().boolean(std::declval<bool>()));

template<typename T, typename Integer>
using number_integer_function_t =
    decltype(std::declval<T&>().number_integer(std::declval<Integer>()));

template<typename T, typename Unsigned>
using number_unsigned_function_t =
    decltype(std::declval<T&>().number_unsigned(std::declval<Unsigned>()));

template<typename T, typename Float, typename String>
using number_float_function_t = decltype(std::declval<T&>().number_float(
                                    std::declval<Float>(), std::declval<const String&>()));

template<typename T, typename String>
using string_function_t =
    decltype(std::declval<T&>().string(std::declval<String&>()));

template<typename T, typename Binary>
using binary_function_t =
    decltype(std::declval<T&>().binary(std::declval<Binary&>()));

template<typename T>
using start_object_function_t =
    decltype(std::declval<T&>().start_object(std::declval<std::size_t>()));

template<typename T, typename String>
using key_function_t =
    decltype(std::declval<T&>().key(std::declval<String&>()));

template<typename T>
using end_object_function_t = decltype(std::declval<T&>().end_object());

template<typename T>
using start_array_function_t =
    decltype(std::declval<T&>().start_array(std::declval<std::size_t>()));

template<typename T>
using end_array_function_t = decltype(std::declval<T&>().end_array());

template<typename T, typename Exception>
using parse_error_function_t = decltype(std::declval<T&>().parse_error(
        std::declval<std::size_t>(), std::declval<const std::string&>(),
        std::declval<const Exception&>()));

template<typename SAX, typename BasicJsonType>
struct is_sax
{
  private:
    static_assert(is_basic_json<BasicJsonType>::value,
                  "BasicJsonType must be of type basic_json<...>");

    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;
    using exception_t = typename BasicJsonType::exception;

  public:
    static constexpr bool value =
        is_detected_exact<bool, null_function_t, SAX>::value &&
        is_detected_exact<bool, boolean_function_t, SAX>::value &&
        is_detected_exact<bool, number_integer_function_t, SAX, number_integer_t>::value &&
        is_detected_exact<bool, number_unsigned_function_t, SAX, number_unsigned_t>::value &&
        is_detected_exact<bool, number_float_function_t, SAX, number_float_t, string_t>::value &&
        is_detected_exact<bool, string_function_t, SAX, string_t>::value &&
        is_detected_exact<bool, binary_function_t, SAX, binary_t>::value &&
        is_detected_exact<bool, start_object_function_t, SAX>::value &&
        is_detected_exact<bool, key_function_t, SAX, string_t>::value &&
        is_detected_exact<bool, end_object_function_t, SAX>::value &&
        is_detected_exact<bool, start_array_function_t, SAX>::value &&
        is_detected_exact<bool, end_array_function_t, SAX>::value &&
        is_detected_exact<bool, parse_error_function_t, SAX, exception_t>::value;
};

template<typename SAX, typename BasicJsonType>
struct is_sax_static_asserts
{
  private:
    static_assert(is_basic_json<BasicJsonType>::value,
                  "BasicJsonType must be of type basic_json<...>");

    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;
    using exception_t = typename BasicJsonType::exception;

  public:
    static_assert(is_detected_exact<bool, null_function_t, SAX>::value,
                  "Missing/invalid function: bool null()");
    static_assert(is_detected_exact<bool, boolean_function_t, SAX>::value,
                  "Missing/invalid function: bool boolean(bool)");
    static_assert(is_detected_exact<bool, boolean_function_t, SAX>::value,
                  "Missing/invalid function: bool boolean(bool)");
    static_assert(
        is_detected_exact<bool, number_integer_function_t, SAX,
        number_integer_t>::value,
        "Missing/invalid function: bool number_integer(number_integer_t)");
    static_assert(
        is_detected_exact<bool, number_unsigned_function_t, SAX,
        number_unsigned_t>::value,
        "Missing/invalid function: bool number_unsigned(number_unsigned_t)");
    static_assert(is_detected_exact<bool, number_float_function_t, SAX,
                  number_float_t, string_t>::value,
                  "Missing/invalid function: bool number_float(number_float_t, const string_t&)");
    static_assert(
        is_detected_exact<bool, string_function_t, SAX, string_t>::value,
        "Missing/invalid function: bool string(string_t&)");
    static_assert(
        is_detected_exact<bool, binary_function_t, SAX, binary_t>::value,
        "Missing/invalid function: bool binary(binary_t&)");
    static_assert(is_detected_exact<bool, start_object_function_t, SAX>::value,
                  "Missing/invalid function: bool start_object(std::size_t)");
    static_assert(is_detected_exact<bool, key_function_t, SAX, string_t>::value,
                  "Missing/invalid function: bool key(string_t&)");
    static_assert(is_detected_exact<bool, end_object_function_t, SAX>::value,
                  "Missing/invalid function: bool end_object()");
    static_assert(is_detected_exact<bool, start_array_function_t, SAX>::value,
                  "Missing/invalid function: bool start_array(std::size_t)");
    static_assert(is_detected_exact<bool, end_array_function_t, SAX>::value,
                  "Missing/invalid function: bool end_array()");
    static_assert(
        is_detected_exact<bool, parse_error_function_t, SAX, exception_t>::value,
        "Missing/invalid function: bool parse_error(std::size_t, const "
        "std::string&, const exception&)");
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{

/// how to treat CBOR tags
enum class cbor_tag_handler_t
{
    error,  ///< throw a parse_error exception in case of a tag
    ignore   ///< ignore tags
};

/*!
@brief determine system byte order

@return true if and only if system's byte order is little endian

@note from https://stackoverflow.com/a/1001328/266378
*/
static inline bool little_endianess(int num = 1) noexcept
{
    return *reinterpret_cast<char*>(&num) == 1;
}


///////////////////
// binary reader //
///////////////////

/*!
@brief deserialization of CBOR, MessagePack, and UBJSON values
*/
template<typename BasicJsonType, typename InputAdapterType, typename SAX = json_sax_dom_parser<BasicJsonType>>
class binary_reader
{
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;
    using json_sax_t = SAX;
    using char_type = typename InputAdapterType::char_type;
    using char_int_type = typename std::char_traits<char_type>::int_type;

  public:
    /*!
    @brief create a binary reader

    @param[in] adapter  input adapter to read from
    */
    explicit binary_reader(InputAdapterType&& adapter) noexcept : ia(std::move(adapter))
    {
        (void)detail::is_sax_static_asserts<SAX, BasicJsonType> {};
    }

    // make class move-only
    binary_reader(const binary_reader&) = delete;
    binary_reader(binary_reader&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    binary_reader& operator=(const binary_reader&) = delete;
    binary_reader& operator=(binary_reader&&) = default; // NOLINT(hicpp-noexcept-move,performance-noexcept-move-constructor)
    ~binary_reader() = default;

    /*!
    @param[in] format  the binary format to parse
    @param[in] sax_    a SAX event processor
    @param[in] strict  whether to expect the input to be consumed completed
    @param[in] tag_handler  how to treat CBOR tags

    @return whether parsing was successful
    */
    JSON_HEDLEY_NON_NULL(3)
    bool sax_parse(const input_format_t format,
                   json_sax_t* sax_,
                   const bool strict = true,
                   const cbor_tag_handler_t tag_handler = cbor_tag_handler_t::error)
    {
        sax = sax_;
        bool result = false;

        switch (format)
        {
            case input_format_t::bson:
                result = parse_bson_internal();
                break;

            case input_format_t::cbor:
                result = parse_cbor_internal(true, tag_handler);
                break;

            case input_format_t::msgpack:
                result = parse_msgpack_internal();
                break;

            case input_format_t::ubjson:
                result = parse_ubjson_internal();
                break;

            default:            // LCOV_EXCL_LINE
                JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
        }

        // strict mode: next byte must be EOF
        if (result && strict)
        {
            if (format == input_format_t::ubjson)
            {
                get_ignore_noop();
            }
            else
            {
                get();
            }

            if (JSON_HEDLEY_UNLIKELY(current != std::char_traits<char_type>::eof()))
            {
                return sax->parse_error(chars_read, get_token_string(),
                                        parse_error::create(110, chars_read, exception_message(format, "expected end of input; last byte: 0x" + get_token_string(), "value"), BasicJsonType()));
            }
        }

        return result;
    }

  private:
    //////////
    // BSON //
    //////////

    /*!
    @brief Reads in a BSON-object and passes it to the SAX-parser.
    @return whether a valid BSON-value was passed to the SAX parser
    */
    bool parse_bson_internal()
    {
        std::int32_t document_size{};
        get_number<std::int32_t, true>(input_format_t::bson, document_size);

        if (JSON_HEDLEY_UNLIKELY(!sax->start_object(std::size_t(-1))))
        {
            return false;
        }

        if (JSON_HEDLEY_UNLIKELY(!parse_bson_element_list(/*is_array*/false)))
        {
            return false;
        }

        return sax->end_object();
    }

    /*!
    @brief Parses a C-style string from the BSON input.
    @param[in,out] result  A reference to the string variable where the read
                            string is to be stored.
    @return `true` if the \x00-byte indicating the end of the string was
             encountered before the EOF; false` indicates an unexpected EOF.
    */
    bool get_bson_cstr(string_t& result)
    {
        auto out = std::back_inserter(result);
        while (true)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::bson, "cstring")))
            {
                return false;
            }
            if (current == 0x00)
            {
                return true;
            }
            *out++ = static_cast<typename string_t::value_type>(current);
        }
    }

    /*!
    @brief Parses a zero-terminated string of length @a len from the BSON
           input.
    @param[in] len  The length (including the zero-byte at the end) of the
                    string to be read.
    @param[in,out] result  A reference to the string variable where the read
                            string is to be stored.
    @tparam NumberType The type of the length @a len
    @pre len >= 1
    @return `true` if the string was successfully parsed
    */
    template<typename NumberType>
    bool get_bson_string(const NumberType len, string_t& result)
    {
        if (JSON_HEDLEY_UNLIKELY(len < 1))
        {
            auto last_token = get_token_string();
            return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::bson, "string length must be at least 1, is " + std::to_string(len), "string"), BasicJsonType()));
        }

        return get_string(input_format_t::bson, len - static_cast<NumberType>(1), result) && get() != std::char_traits<char_type>::eof();
    }

    /*!
    @brief Parses a byte array input of length @a len from the BSON input.
    @param[in] len  The length of the byte array to be read.
    @param[in,out] result  A reference to the binary variable where the read
                            array is to be stored.
    @tparam NumberType The type of the length @a len
    @pre len >= 0
    @return `true` if the byte array was successfully parsed
    */
    template<typename NumberType>
    bool get_bson_binary(const NumberType len, binary_t& result)
    {
        if (JSON_HEDLEY_UNLIKELY(len < 0))
        {
            auto last_token = get_token_string();
            return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::bson, "byte array length cannot be negative, is " + std::to_string(len), "binary"), BasicJsonType()));
        }

        // All BSON binary values have a subtype
        std::uint8_t subtype{};
        get_number<std::uint8_t>(input_format_t::bson, subtype);
        result.set_subtype(subtype);

        return get_binary(input_format_t::bson, len, result);
    }

    /*!
    @brief Read a BSON document element of the given @a element_type.
    @param[in] element_type The BSON element type, c.f. http://bsonspec.org/spec.html
    @param[in] element_type_parse_position The position in the input stream,
               where the `element_type` was read.
    @warning Not all BSON element types are supported yet. An unsupported
             @a element_type will give rise to a parse_error.114:
             Unsupported BSON record type 0x...
    @return whether a valid BSON-object/array was passed to the SAX parser
    */
    bool parse_bson_element_internal(const char_int_type element_type,
                                     const std::size_t element_type_parse_position)
    {
        switch (element_type)
        {
            case 0x01: // double
            {
                double number{};
                return get_number<double, true>(input_format_t::bson, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 0x02: // string
            {
                std::int32_t len{};
                string_t value;
                return get_number<std::int32_t, true>(input_format_t::bson, len) && get_bson_string(len, value) && sax->string(value);
            }

            case 0x03: // object
            {
                return parse_bson_internal();
            }

            case 0x04: // array
            {
                return parse_bson_array();
            }

            case 0x05: // binary
            {
                std::int32_t len{};
                binary_t value;
                return get_number<std::int32_t, true>(input_format_t::bson, len) && get_bson_binary(len, value) && sax->binary(value);
            }

            case 0x08: // boolean
            {
                return sax->boolean(get() != 0);
            }

            case 0x0A: // null
            {
                return sax->null();
            }

            case 0x10: // int32
            {
                std::int32_t value{};
                return get_number<std::int32_t, true>(input_format_t::bson, value) && sax->number_integer(value);
            }

            case 0x12: // int64
            {
                std::int64_t value{};
                return get_number<std::int64_t, true>(input_format_t::bson, value) && sax->number_integer(value);
            }

            default: // anything else not supported (yet)
            {
                std::array<char, 3> cr{{}};
                (std::snprintf)(cr.data(), cr.size(), "%.2hhX", static_cast<unsigned char>(element_type)); // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
                return sax->parse_error(element_type_parse_position, std::string(cr.data()), parse_error::create(114, element_type_parse_position, "Unsupported BSON record type 0x" + std::string(cr.data()), BasicJsonType()));
            }
        }
    }

    /*!
    @brief Read a BSON element list (as specified in the BSON-spec)

    The same binary layout is used for objects and arrays, hence it must be
    indicated with the argument @a is_array which one is expected
    (true --> array, false --> object).

    @param[in] is_array Determines if the element list being read is to be
                        treated as an object (@a is_array == false), or as an
                        array (@a is_array == true).
    @return whether a valid BSON-object/array was passed to the SAX parser
    */
    bool parse_bson_element_list(const bool is_array)
    {
        string_t key;

        while (auto element_type = get())
        {
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::bson, "element list")))
            {
                return false;
            }

            const std::size_t element_type_parse_position = chars_read;
            if (JSON_HEDLEY_UNLIKELY(!get_bson_cstr(key)))
            {
                return false;
            }

            if (!is_array && !sax->key(key))
            {
                return false;
            }

            if (JSON_HEDLEY_UNLIKELY(!parse_bson_element_internal(element_type, element_type_parse_position)))
            {
                return false;
            }

            // get_bson_cstr only appends
            key.clear();
        }

        return true;
    }

    /*!
    @brief Reads an array from the BSON input and passes it to the SAX-parser.
    @return whether a valid BSON-array was passed to the SAX parser
    */
    bool parse_bson_array()
    {
        std::int32_t document_size{};
        get_number<std::int32_t, true>(input_format_t::bson, document_size);

        if (JSON_HEDLEY_UNLIKELY(!sax->start_array(std::size_t(-1))))
        {
            return false;
        }

        if (JSON_HEDLEY_UNLIKELY(!parse_bson_element_list(/*is_array*/true)))
        {
            return false;
        }

        return sax->end_array();
    }

    //////////
    // CBOR //
    //////////

    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true) or whether the last read character should
                         be considered instead (false)
    @param[in] tag_handler how CBOR tags should be treated

    @return whether a valid CBOR value was passed to the SAX parser
    */
    bool parse_cbor_internal(const bool get_char,
                             const cbor_tag_handler_t tag_handler)
    {
        switch (get_char ? get() : current)
        {
            // EOF
            case std::char_traits<char_type>::eof():
                return unexpect_eof(input_format_t::cbor, "value");

            // Integer 0x00..0x17 (0..23)
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
                return sax->number_unsigned(static_cast<number_unsigned_t>(current));

            case 0x18: // Unsigned integer (one-byte uint8_t follows)
            {
                std::uint8_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_unsigned(number);
            }

            case 0x19: // Unsigned integer (two-byte uint16_t follows)
            {
                std::uint16_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_unsigned(number);
            }

            case 0x1A: // Unsigned integer (four-byte uint32_t follows)
            {
                std::uint32_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_unsigned(number);
            }

            case 0x1B: // Unsigned integer (eight-byte uint64_t follows)
            {
                std::uint64_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_unsigned(number);
            }

            // Negative integer -1-0x00..-1-0x17 (-1..-24)
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
                return sax->number_integer(static_cast<std::int8_t>(0x20 - 1 - current));

            case 0x38: // Negative integer (one-byte uint8_t follows)
            {
                std::uint8_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_integer(static_cast<number_integer_t>(-1) - number);
            }

            case 0x39: // Negative integer -1-n (two-byte uint16_t follows)
            {
                std::uint16_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_integer(static_cast<number_integer_t>(-1) - number);
            }

            case 0x3A: // Negative integer -1-n (four-byte uint32_t follows)
            {
                std::uint32_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_integer(static_cast<number_integer_t>(-1) - number);
            }

            case 0x3B: // Negative integer -1-n (eight-byte uint64_t follows)
            {
                std::uint64_t number{};
                return get_number(input_format_t::cbor, number) && sax->number_integer(static_cast<number_integer_t>(-1)
                        - static_cast<number_integer_t>(number));
            }

            // Binary data (0x00..0x17 bytes follow)
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58: // Binary data (one-byte uint8_t for n follows)
            case 0x59: // Binary data (two-byte uint16_t for n follow)
            case 0x5A: // Binary data (four-byte uint32_t for n follow)
            case 0x5B: // Binary data (eight-byte uint64_t for n follow)
            case 0x5F: // Binary data (indefinite length)
            {
                binary_t b;
                return get_cbor_binary(b) && sax->binary(b);
            }

            // UTF-8 string (0x00..0x17 bytes follow)
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
            case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
            case 0x7A: // UTF-8 string (four-byte uint32_t for n follow)
            case 0x7B: // UTF-8 string (eight-byte uint64_t for n follow)
            case 0x7F: // UTF-8 string (indefinite length)
            {
                string_t s;
                return get_cbor_string(s) && sax->string(s);
            }

            // array (0x00..0x17 data items follow)
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
                return get_cbor_array(static_cast<std::size_t>(static_cast<unsigned int>(current) & 0x1Fu), tag_handler);

            case 0x98: // array (one-byte uint8_t for n follows)
            {
                std::uint8_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_array(static_cast<std::size_t>(len), tag_handler);
            }

            case 0x99: // array (two-byte uint16_t for n follow)
            {
                std::uint16_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_array(static_cast<std::size_t>(len), tag_handler);
            }

            case 0x9A: // array (four-byte uint32_t for n follow)
            {
                std::uint32_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_array(static_cast<std::size_t>(len), tag_handler);
            }

            case 0x9B: // array (eight-byte uint64_t for n follow)
            {
                std::uint64_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_array(static_cast<std::size_t>(len), tag_handler);
            }

            case 0x9F: // array (indefinite length)
                return get_cbor_array(std::size_t(-1), tag_handler);

            // map (0x00..0x17 pairs of data items follow)
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
                return get_cbor_object(static_cast<std::size_t>(static_cast<unsigned int>(current) & 0x1Fu), tag_handler);

            case 0xB8: // map (one-byte uint8_t for n follows)
            {
                std::uint8_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_object(static_cast<std::size_t>(len), tag_handler);
            }

            case 0xB9: // map (two-byte uint16_t for n follow)
            {
                std::uint16_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_object(static_cast<std::size_t>(len), tag_handler);
            }

            case 0xBA: // map (four-byte uint32_t for n follow)
            {
                std::uint32_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_object(static_cast<std::size_t>(len), tag_handler);
            }

            case 0xBB: // map (eight-byte uint64_t for n follow)
            {
                std::uint64_t len{};
                return get_number(input_format_t::cbor, len) && get_cbor_object(static_cast<std::size_t>(len), tag_handler);
            }

            case 0xBF: // map (indefinite length)
                return get_cbor_object(std::size_t(-1), tag_handler);

            case 0xC6: // tagged item
            case 0xC7:
            case 0xC8:
            case 0xC9:
            case 0xCA:
            case 0xCB:
            case 0xCC:
            case 0xCD:
            case 0xCE:
            case 0xCF:
            case 0xD0:
            case 0xD1:
            case 0xD2:
            case 0xD3:
            case 0xD4:
            case 0xD8: // tagged item (1 bytes follow)
            case 0xD9: // tagged item (2 bytes follow)
            case 0xDA: // tagged item (4 bytes follow)
            case 0xDB: // tagged item (8 bytes follow)
            {
                switch (tag_handler)
                {
                    case cbor_tag_handler_t::error:
                    {
                        auto last_token = get_token_string();
                        return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::cbor, "invalid byte: 0x" + last_token, "value"), BasicJsonType()));
                    }

                    case cbor_tag_handler_t::ignore:
                    {
                        switch (current)
                        {
                            case 0xD8:
                            {
                                std::uint8_t len{};
                                get_number(input_format_t::cbor, len);
                                break;
                            }
                            case 0xD9:
                            {
                                std::uint16_t len{};
                                get_number(input_format_t::cbor, len);
                                break;
                            }
                            case 0xDA:
                            {
                                std::uint32_t len{};
                                get_number(input_format_t::cbor, len);
                                break;
                            }
                            case 0xDB:
                            {
                                std::uint64_t len{};
                                get_number(input_format_t::cbor, len);
                                break;
                            }
                            default:
                                break;
                        }
                        return parse_cbor_internal(true, tag_handler);
                    }

                    default:                 // LCOV_EXCL_LINE
                        JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
                        return false;        // LCOV_EXCL_LINE
                }
            }

            case 0xF4: // false
                return sax->boolean(false);

            case 0xF5: // true
                return sax->boolean(true);

            case 0xF6: // null
                return sax->null();

            case 0xF9: // Half-Precision Float (two-byte IEEE 754)
            {
                const auto byte1_raw = get();
                if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::cbor, "number")))
                {
                    return false;
                }
                const auto byte2_raw = get();
                if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::cbor, "number")))
                {
                    return false;
                }

                const auto byte1 = static_cast<unsigned char>(byte1_raw);
                const auto byte2 = static_cast<unsigned char>(byte2_raw);

                // code from RFC 7049, Appendix D, Figure 3:
                // As half-precision floating-point numbers were only added
                // to IEEE 754 in 2008, today's programming platforms often
                // still only have limited support for them. It is very
                // easy to include at least decoding support for them even
                // without such support. An example of a small decoder for
                // half-precision floating-point numbers in the C language
                // is shown in Fig. 3.
                const auto half = static_cast<unsigned int>((byte1 << 8u) + byte2);
                const double val = [&half]
                {
                    const int exp = (half >> 10u) & 0x1Fu;
                    const unsigned int mant = half & 0x3FFu;
                    JSON_ASSERT(0 <= exp&& exp <= 32);
                    JSON_ASSERT(mant <= 1024);
                    switch (exp)
                    {
                        case 0:
                            return std::ldexp(mant, -24);
                        case 31:
                            return (mant == 0)
                            ? std::numeric_limits<double>::infinity()
                            : std::numeric_limits<double>::quiet_NaN();
                        default:
                            return std::ldexp(mant + 1024, exp - 25);
                    }
                }();
                return sax->number_float((half & 0x8000u) != 0
                                         ? static_cast<number_float_t>(-val)
                                         : static_cast<number_float_t>(val), "");
            }

            case 0xFA: // Single-Precision Float (four-byte IEEE 754)
            {
                float number{};
                return get_number(input_format_t::cbor, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 0xFB: // Double-Precision Float (eight-byte IEEE 754)
            {
                double number{};
                return get_number(input_format_t::cbor, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            default: // anything else (0xFF is handled inside the other types)
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::cbor, "invalid byte: 0x" + last_token, "value"), BasicJsonType()));
            }
        }
    }

    /*!
    @brief reads a CBOR string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.
    Additionally, CBOR's strings with indefinite lengths are supported.

    @param[out] result  created string

    @return whether string creation completed
    */
    bool get_cbor_string(string_t& result)
    {
        if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::cbor, "string")))
        {
            return false;
        }

        switch (current)
        {
            // UTF-8 string (0x00..0x17 bytes follow)
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            {
                return get_string(input_format_t::cbor, static_cast<unsigned int>(current) & 0x1Fu, result);
            }

            case 0x78: // UTF-8 string (one-byte uint8_t for n follows)
            {
                std::uint8_t len{};
                return get_number(input_format_t::cbor, len) && get_string(input_format_t::cbor, len, result);
            }

            case 0x79: // UTF-8 string (two-byte uint16_t for n follow)
            {
                std::uint16_t len{};
                return get_number(input_format_t::cbor, len) && get_string(input_format_t::cbor, len, result);
            }

            case 0x7A: // UTF-8 string (four-byte uint32_t for n follow)
            {
                std::uint32_t len{};
                return get_number(input_format_t::cbor, len) && get_string(input_format_t::cbor, len, result);
            }

            case 0x7B: // UTF-8 string (eight-byte uint64_t for n follow)
            {
                std::uint64_t len{};
                return get_number(input_format_t::cbor, len) && get_string(input_format_t::cbor, len, result);
            }

            case 0x7F: // UTF-8 string (indefinite length)
            {
                while (get() != 0xFF)
                {
                    string_t chunk;
                    if (!get_cbor_string(chunk))
                    {
                        return false;
                    }
                    result.append(chunk);
                }
                return true;
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::cbor, "expected length specification (0x60-0x7B) or indefinite string type (0x7F); last byte: 0x" + last_token, "string"), BasicJsonType()));
            }
        }
    }

    /*!
    @brief reads a CBOR byte array

    This function first reads starting bytes to determine the expected
    byte array length and then copies this number of bytes into the byte array.
    Additionally, CBOR's byte arrays with indefinite lengths are supported.

    @param[out] result  created byte array

    @return whether byte array creation completed
    */
    bool get_cbor_binary(binary_t& result)
    {
        if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::cbor, "binary")))
        {
            return false;
        }

        switch (current)
        {
            // Binary data (0x00..0x17 bytes follow)
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            {
                return get_binary(input_format_t::cbor, static_cast<unsigned int>(current) & 0x1Fu, result);
            }

            case 0x58: // Binary data (one-byte uint8_t for n follows)
            {
                std::uint8_t len{};
                return get_number(input_format_t::cbor, len) &&
                       get_binary(input_format_t::cbor, len, result);
            }

            case 0x59: // Binary data (two-byte uint16_t for n follow)
            {
                std::uint16_t len{};
                return get_number(input_format_t::cbor, len) &&
                       get_binary(input_format_t::cbor, len, result);
            }

            case 0x5A: // Binary data (four-byte uint32_t for n follow)
            {
                std::uint32_t len{};
                return get_number(input_format_t::cbor, len) &&
                       get_binary(input_format_t::cbor, len, result);
            }

            case 0x5B: // Binary data (eight-byte uint64_t for n follow)
            {
                std::uint64_t len{};
                return get_number(input_format_t::cbor, len) &&
                       get_binary(input_format_t::cbor, len, result);
            }

            case 0x5F: // Binary data (indefinite length)
            {
                while (get() != 0xFF)
                {
                    binary_t chunk;
                    if (!get_cbor_binary(chunk))
                    {
                        return false;
                    }
                    result.insert(result.end(), chunk.begin(), chunk.end());
                }
                return true;
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::cbor, "expected length specification (0x40-0x5B) or indefinite binary array type (0x5F); last byte: 0x" + last_token, "binary"), BasicJsonType()));
            }
        }
    }

    /*!
    @param[in] len  the length of the array or std::size_t(-1) for an
                    array of indefinite size
    @param[in] tag_handler how CBOR tags should be treated
    @return whether array creation completed
    */
    bool get_cbor_array(const std::size_t len,
                        const cbor_tag_handler_t tag_handler)
    {
        if (JSON_HEDLEY_UNLIKELY(!sax->start_array(len)))
        {
            return false;
        }

        if (len != std::size_t(-1))
        {
            for (std::size_t i = 0; i < len; ++i)
            {
                if (JSON_HEDLEY_UNLIKELY(!parse_cbor_internal(true, tag_handler)))
                {
                    return false;
                }
            }
        }
        else
        {
            while (get() != 0xFF)
            {
                if (JSON_HEDLEY_UNLIKELY(!parse_cbor_internal(false, tag_handler)))
                {
                    return false;
                }
            }
        }

        return sax->end_array();
    }

    /*!
    @param[in] len  the length of the object or std::size_t(-1) for an
                    object of indefinite size
    @param[in] tag_handler how CBOR tags should be treated
    @return whether object creation completed
    */
    bool get_cbor_object(const std::size_t len,
                         const cbor_tag_handler_t tag_handler)
    {
        if (JSON_HEDLEY_UNLIKELY(!sax->start_object(len)))
        {
            return false;
        }

        if (len != 0)
        {
            string_t key;
            if (len != std::size_t(-1))
            {
                for (std::size_t i = 0; i < len; ++i)
                {
                    get();
                    if (JSON_HEDLEY_UNLIKELY(!get_cbor_string(key) || !sax->key(key)))
                    {
                        return false;
                    }

                    if (JSON_HEDLEY_UNLIKELY(!parse_cbor_internal(true, tag_handler)))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
            else
            {
                while (get() != 0xFF)
                {
                    if (JSON_HEDLEY_UNLIKELY(!get_cbor_string(key) || !sax->key(key)))
                    {
                        return false;
                    }

                    if (JSON_HEDLEY_UNLIKELY(!parse_cbor_internal(true, tag_handler)))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
        }

        return sax->end_object();
    }

    /////////////
    // MsgPack //
    /////////////

    /*!
    @return whether a valid MessagePack value was passed to the SAX parser
    */
    bool parse_msgpack_internal()
    {
        switch (get())
        {
            // EOF
            case std::char_traits<char_type>::eof():
                return unexpect_eof(input_format_t::msgpack, "value");

            // positive fixint
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2A:
            case 0x2B:
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3A:
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5A:
            case 0x5B:
            case 0x5C:
            case 0x5D:
            case 0x5E:
            case 0x5F:
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
            case 0x7E:
            case 0x7F:
                return sax->number_unsigned(static_cast<number_unsigned_t>(current));

            // fixmap
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
                return get_msgpack_object(static_cast<std::size_t>(static_cast<unsigned int>(current) & 0x0Fu));

            // fixarray
            case 0x90:
            case 0x91:
            case 0x92:
            case 0x93:
            case 0x94:
            case 0x95:
            case 0x96:
            case 0x97:
            case 0x98:
            case 0x99:
            case 0x9A:
            case 0x9B:
            case 0x9C:
            case 0x9D:
            case 0x9E:
            case 0x9F:
                return get_msgpack_array(static_cast<std::size_t>(static_cast<unsigned int>(current) & 0x0Fu));

            // fixstr
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            case 0xD9: // str 8
            case 0xDA: // str 16
            case 0xDB: // str 32
            {
                string_t s;
                return get_msgpack_string(s) && sax->string(s);
            }

            case 0xC0: // nil
                return sax->null();

            case 0xC2: // false
                return sax->boolean(false);

            case 0xC3: // true
                return sax->boolean(true);

            case 0xC4: // bin 8
            case 0xC5: // bin 16
            case 0xC6: // bin 32
            case 0xC7: // ext 8
            case 0xC8: // ext 16
            case 0xC9: // ext 32
            case 0xD4: // fixext 1
            case 0xD5: // fixext 2
            case 0xD6: // fixext 4
            case 0xD7: // fixext 8
            case 0xD8: // fixext 16
            {
                binary_t b;
                return get_msgpack_binary(b) && sax->binary(b);
            }

            case 0xCA: // float 32
            {
                float number{};
                return get_number(input_format_t::msgpack, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 0xCB: // float 64
            {
                double number{};
                return get_number(input_format_t::msgpack, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 0xCC: // uint 8
            {
                std::uint8_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_unsigned(number);
            }

            case 0xCD: // uint 16
            {
                std::uint16_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_unsigned(number);
            }

            case 0xCE: // uint 32
            {
                std::uint32_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_unsigned(number);
            }

            case 0xCF: // uint 64
            {
                std::uint64_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_unsigned(number);
            }

            case 0xD0: // int 8
            {
                std::int8_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_integer(number);
            }

            case 0xD1: // int 16
            {
                std::int16_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_integer(number);
            }

            case 0xD2: // int 32
            {
                std::int32_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_integer(number);
            }

            case 0xD3: // int 64
            {
                std::int64_t number{};
                return get_number(input_format_t::msgpack, number) && sax->number_integer(number);
            }

            case 0xDC: // array 16
            {
                std::uint16_t len{};
                return get_number(input_format_t::msgpack, len) && get_msgpack_array(static_cast<std::size_t>(len));
            }

            case 0xDD: // array 32
            {
                std::uint32_t len{};
                return get_number(input_format_t::msgpack, len) && get_msgpack_array(static_cast<std::size_t>(len));
            }

            case 0xDE: // map 16
            {
                std::uint16_t len{};
                return get_number(input_format_t::msgpack, len) && get_msgpack_object(static_cast<std::size_t>(len));
            }

            case 0xDF: // map 32
            {
                std::uint32_t len{};
                return get_number(input_format_t::msgpack, len) && get_msgpack_object(static_cast<std::size_t>(len));
            }

            // negative fixint
            case 0xE0:
            case 0xE1:
            case 0xE2:
            case 0xE3:
            case 0xE4:
            case 0xE5:
            case 0xE6:
            case 0xE7:
            case 0xE8:
            case 0xE9:
            case 0xEA:
            case 0xEB:
            case 0xEC:
            case 0xED:
            case 0xEE:
            case 0xEF:
            case 0xF0:
            case 0xF1:
            case 0xF2:
            case 0xF3:
            case 0xF4:
            case 0xF5:
            case 0xF6:
            case 0xF7:
            case 0xF8:
            case 0xF9:
            case 0xFA:
            case 0xFB:
            case 0xFC:
            case 0xFD:
            case 0xFE:
            case 0xFF:
                return sax->number_integer(static_cast<std::int8_t>(current));

            default: // anything else
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::msgpack, "invalid byte: 0x" + last_token, "value"), BasicJsonType()));
            }
        }
    }

    /*!
    @brief reads a MessagePack string

    This function first reads starting bytes to determine the expected
    string length and then copies this number of bytes into a string.

    @param[out] result  created string

    @return whether string creation completed
    */
    bool get_msgpack_string(string_t& result)
    {
        if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::msgpack, "string")))
        {
            return false;
        }

        switch (current)
        {
            // fixstr
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            {
                return get_string(input_format_t::msgpack, static_cast<unsigned int>(current) & 0x1Fu, result);
            }

            case 0xD9: // str 8
            {
                std::uint8_t len{};
                return get_number(input_format_t::msgpack, len) && get_string(input_format_t::msgpack, len, result);
            }

            case 0xDA: // str 16
            {
                std::uint16_t len{};
                return get_number(input_format_t::msgpack, len) && get_string(input_format_t::msgpack, len, result);
            }

            case 0xDB: // str 32
            {
                std::uint32_t len{};
                return get_number(input_format_t::msgpack, len) && get_string(input_format_t::msgpack, len, result);
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::msgpack, "expected length specification (0xA0-0xBF, 0xD9-0xDB); last byte: 0x" + last_token, "string"), BasicJsonType()));
            }
        }
    }

    /*!
    @brief reads a MessagePack byte array

    This function first reads starting bytes to determine the expected
    byte array length and then copies this number of bytes into a byte array.

    @param[out] result  created byte array

    @return whether byte array creation completed
    */
    bool get_msgpack_binary(binary_t& result)
    {
        // helper function to set the subtype
        auto assign_and_return_true = [&result](std::int8_t subtype)
        {
            result.set_subtype(static_cast<std::uint8_t>(subtype));
            return true;
        };

        switch (current)
        {
            case 0xC4: // bin 8
            {
                std::uint8_t len{};
                return get_number(input_format_t::msgpack, len) &&
                       get_binary(input_format_t::msgpack, len, result);
            }

            case 0xC5: // bin 16
            {
                std::uint16_t len{};
                return get_number(input_format_t::msgpack, len) &&
                       get_binary(input_format_t::msgpack, len, result);
            }

            case 0xC6: // bin 32
            {
                std::uint32_t len{};
                return get_number(input_format_t::msgpack, len) &&
                       get_binary(input_format_t::msgpack, len, result);
            }

            case 0xC7: // ext 8
            {
                std::uint8_t len{};
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, len) &&
                       get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, len, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xC8: // ext 16
            {
                std::uint16_t len{};
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, len) &&
                       get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, len, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xC9: // ext 32
            {
                std::uint32_t len{};
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, len) &&
                       get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, len, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xD4: // fixext 1
            {
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, 1, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xD5: // fixext 2
            {
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, 2, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xD6: // fixext 4
            {
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, 4, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xD7: // fixext 8
            {
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, 8, result) &&
                       assign_and_return_true(subtype);
            }

            case 0xD8: // fixext 16
            {
                std::int8_t subtype{};
                return get_number(input_format_t::msgpack, subtype) &&
                       get_binary(input_format_t::msgpack, 16, result) &&
                       assign_and_return_true(subtype);
            }

            default:           // LCOV_EXCL_LINE
                return false;  // LCOV_EXCL_LINE
        }
    }

    /*!
    @param[in] len  the length of the array
    @return whether array creation completed
    */
    bool get_msgpack_array(const std::size_t len)
    {
        if (JSON_HEDLEY_UNLIKELY(!sax->start_array(len)))
        {
            return false;
        }

        for (std::size_t i = 0; i < len; ++i)
        {
            if (JSON_HEDLEY_UNLIKELY(!parse_msgpack_internal()))
            {
                return false;
            }
        }

        return sax->end_array();
    }

    /*!
    @param[in] len  the length of the object
    @return whether object creation completed
    */
    bool get_msgpack_object(const std::size_t len)
    {
        if (JSON_HEDLEY_UNLIKELY(!sax->start_object(len)))
        {
            return false;
        }

        string_t key;
        for (std::size_t i = 0; i < len; ++i)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!get_msgpack_string(key) || !sax->key(key)))
            {
                return false;
            }

            if (JSON_HEDLEY_UNLIKELY(!parse_msgpack_internal()))
            {
                return false;
            }
            key.clear();
        }

        return sax->end_object();
    }

    ////////////
    // UBJSON //
    ////////////

    /*!
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead

    @return whether a valid UBJSON value was passed to the SAX parser
    */
    bool parse_ubjson_internal(const bool get_char = true)
    {
        return get_ubjson_value(get_char ? get_ignore_noop() : current);
    }

    /*!
    @brief reads a UBJSON string

    This function is either called after reading the 'S' byte explicitly
    indicating a string, or in case of an object key where the 'S' byte can be
    left out.

    @param[out] result   created string
    @param[in] get_char  whether a new character should be retrieved from the
                         input (true, default) or whether the last read
                         character should be considered instead

    @return whether string creation completed
    */
    bool get_ubjson_string(string_t& result, const bool get_char = true)
    {
        if (get_char)
        {
            get();  // TODO(niels): may we ignore N here?
        }

        if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::ubjson, "value")))
        {
            return false;
        }

        switch (current)
        {
            case 'U':
            {
                std::uint8_t len{};
                return get_number(input_format_t::ubjson, len) && get_string(input_format_t::ubjson, len, result);
            }

            case 'i':
            {
                std::int8_t len{};
                return get_number(input_format_t::ubjson, len) && get_string(input_format_t::ubjson, len, result);
            }

            case 'I':
            {
                std::int16_t len{};
                return get_number(input_format_t::ubjson, len) && get_string(input_format_t::ubjson, len, result);
            }

            case 'l':
            {
                std::int32_t len{};
                return get_number(input_format_t::ubjson, len) && get_string(input_format_t::ubjson, len, result);
            }

            case 'L':
            {
                std::int64_t len{};
                return get_number(input_format_t::ubjson, len) && get_string(input_format_t::ubjson, len, result);
            }

            default:
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "expected length type specification (U, i, I, l, L); last byte: 0x" + last_token, "string"), BasicJsonType()));
        }
    }

    /*!
    @param[out] result  determined size
    @return whether size determination completed
    */
    bool get_ubjson_size_value(std::size_t& result)
    {
        switch (get_ignore_noop())
        {
            case 'U':
            {
                std::uint8_t number{};
                if (JSON_HEDLEY_UNLIKELY(!get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'i':
            {
                std::int8_t number{};
                if (JSON_HEDLEY_UNLIKELY(!get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number); // NOLINT(bugprone-signed-char-misuse,cert-str34-c): number is not a char
                return true;
            }

            case 'I':
            {
                std::int16_t number{};
                if (JSON_HEDLEY_UNLIKELY(!get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'l':
            {
                std::int32_t number{};
                if (JSON_HEDLEY_UNLIKELY(!get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            case 'L':
            {
                std::int64_t number{};
                if (JSON_HEDLEY_UNLIKELY(!get_number(input_format_t::ubjson, number)))
                {
                    return false;
                }
                result = static_cast<std::size_t>(number);
                return true;
            }

            default:
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "expected length type specification (U, i, I, l, L) after '#'; last byte: 0x" + last_token, "size"), BasicJsonType()));
            }
        }
    }

    /*!
    @brief determine the type and size for a container

    In the optimized UBJSON format, a type and a size can be provided to allow
    for a more compact representation.

    @param[out] result  pair of the size and the type

    @return whether pair creation completed
    */
    bool get_ubjson_size_type(std::pair<std::size_t, char_int_type>& result)
    {
        result.first = string_t::npos; // size
        result.second = 0; // type

        get_ignore_noop();

        if (current == '$')
        {
            result.second = get();  // must not ignore 'N', because 'N' maybe the type
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::ubjson, "type")))
            {
                return false;
            }

            get_ignore_noop();
            if (JSON_HEDLEY_UNLIKELY(current != '#'))
            {
                if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::ubjson, "value")))
                {
                    return false;
                }
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::ubjson, "expected '#' after type information; last byte: 0x" + last_token, "size"), BasicJsonType()));
            }

            return get_ubjson_size_value(result.first);
        }

        if (current == '#')
        {
            return get_ubjson_size_value(result.first);
        }

        return true;
    }

    /*!
    @param prefix  the previously read or set type prefix
    @return whether value creation completed
    */
    bool get_ubjson_value(const char_int_type prefix)
    {
        switch (prefix)
        {
            case std::char_traits<char_type>::eof():  // EOF
                return unexpect_eof(input_format_t::ubjson, "value");

            case 'T':  // true
                return sax->boolean(true);
            case 'F':  // false
                return sax->boolean(false);

            case 'Z':  // null
                return sax->null();

            case 'U':
            {
                std::uint8_t number{};
                return get_number(input_format_t::ubjson, number) && sax->number_unsigned(number);
            }

            case 'i':
            {
                std::int8_t number{};
                return get_number(input_format_t::ubjson, number) && sax->number_integer(number);
            }

            case 'I':
            {
                std::int16_t number{};
                return get_number(input_format_t::ubjson, number) && sax->number_integer(number);
            }

            case 'l':
            {
                std::int32_t number{};
                return get_number(input_format_t::ubjson, number) && sax->number_integer(number);
            }

            case 'L':
            {
                std::int64_t number{};
                return get_number(input_format_t::ubjson, number) && sax->number_integer(number);
            }

            case 'd':
            {
                float number{};
                return get_number(input_format_t::ubjson, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 'D':
            {
                double number{};
                return get_number(input_format_t::ubjson, number) && sax->number_float(static_cast<number_float_t>(number), "");
            }

            case 'H':
            {
                return get_ubjson_high_precision_number();
            }

            case 'C':  // char
            {
                get();
                if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::ubjson, "char")))
                {
                    return false;
                }
                if (JSON_HEDLEY_UNLIKELY(current > 127))
                {
                    auto last_token = get_token_string();
                    return sax->parse_error(chars_read, last_token, parse_error::create(113, chars_read, exception_message(input_format_t::ubjson, "byte after 'C' must be in range 0x00..0x7F; last byte: 0x" + last_token, "char"), BasicJsonType()));
                }
                string_t s(1, static_cast<typename string_t::value_type>(current));
                return sax->string(s);
            }

            case 'S':  // string
            {
                string_t s;
                return get_ubjson_string(s) && sax->string(s);
            }

            case '[':  // array
                return get_ubjson_array();

            case '{':  // object
                return get_ubjson_object();

            default: // anything else
            {
                auto last_token = get_token_string();
                return sax->parse_error(chars_read, last_token, parse_error::create(112, chars_read, exception_message(input_format_t::ubjson, "invalid byte: 0x" + last_token, "value"), BasicJsonType()));
            }
        }
    }

    /*!
    @return whether array creation completed
    */
    bool get_ubjson_array()
    {
        std::pair<std::size_t, char_int_type> size_and_type;
        if (JSON_HEDLEY_UNLIKELY(!get_ubjson_size_type(size_and_type)))
        {
            return false;
        }

        if (size_and_type.first != string_t::npos)
        {
            if (JSON_HEDLEY_UNLIKELY(!sax->start_array(size_and_type.first)))
            {
                return false;
            }

            if (size_and_type.second != 0)
            {
                if (size_and_type.second != 'N')
                {
                    for (std::size_t i = 0; i < size_and_type.first; ++i)
                    {
                        if (JSON_HEDLEY_UNLIKELY(!get_ubjson_value(size_and_type.second)))
                        {
                            return false;
                        }
                    }
                }
            }
            else
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if (JSON_HEDLEY_UNLIKELY(!parse_ubjson_internal()))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if (JSON_HEDLEY_UNLIKELY(!sax->start_array(std::size_t(-1))))
            {
                return false;
            }

            while (current != ']')
            {
                if (JSON_HEDLEY_UNLIKELY(!parse_ubjson_internal(false)))
                {
                    return false;
                }
                get_ignore_noop();
            }
        }

        return sax->end_array();
    }

    /*!
    @return whether object creation completed
    */
    bool get_ubjson_object()
    {
        std::pair<std::size_t, char_int_type> size_and_type;
        if (JSON_HEDLEY_UNLIKELY(!get_ubjson_size_type(size_and_type)))
        {
            return false;
        }

        string_t key;
        if (size_and_type.first != string_t::npos)
        {
            if (JSON_HEDLEY_UNLIKELY(!sax->start_object(size_and_type.first)))
            {
                return false;
            }

            if (size_and_type.second != 0)
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if (JSON_HEDLEY_UNLIKELY(!get_ubjson_string(key) || !sax->key(key)))
                    {
                        return false;
                    }
                    if (JSON_HEDLEY_UNLIKELY(!get_ubjson_value(size_and_type.second)))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
            else
            {
                for (std::size_t i = 0; i < size_and_type.first; ++i)
                {
                    if (JSON_HEDLEY_UNLIKELY(!get_ubjson_string(key) || !sax->key(key)))
                    {
                        return false;
                    }
                    if (JSON_HEDLEY_UNLIKELY(!parse_ubjson_internal()))
                    {
                        return false;
                    }
                    key.clear();
                }
            }
        }
        else
        {
            if (JSON_HEDLEY_UNLIKELY(!sax->start_object(std::size_t(-1))))
            {
                return false;
            }

            while (current != '}')
            {
                if (JSON_HEDLEY_UNLIKELY(!get_ubjson_string(key, false) || !sax->key(key)))
                {
                    return false;
                }
                if (JSON_HEDLEY_UNLIKELY(!parse_ubjson_internal()))
                {
                    return false;
                }
                get_ignore_noop();
                key.clear();
            }
        }

        return sax->end_object();
    }

    // Note, no reader for UBJSON binary types is implemented because they do
    // not exist

    bool get_ubjson_high_precision_number()
    {
        // get size of following number string
        std::size_t size{};
        auto res = get_ubjson_size_value(size);
        if (JSON_HEDLEY_UNLIKELY(!res))
        {
            return res;
        }

        // get number string
        std::vector<char> number_vector;
        for (std::size_t i = 0; i < size; ++i)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(input_format_t::ubjson, "number")))
            {
                return false;
            }
            number_vector.push_back(static_cast<char>(current));
        }

        // parse number string
        using ia_type = decltype(detail::input_adapter(number_vector));
        auto number_lexer = detail::lexer<BasicJsonType, ia_type>(detail::input_adapter(number_vector), false);
        const auto result_number = number_lexer.scan();
        const auto number_string = number_lexer.get_token_string();
        const auto result_remainder = number_lexer.scan();

        using token_type = typename detail::lexer_base<BasicJsonType>::token_type;

        if (JSON_HEDLEY_UNLIKELY(result_remainder != token_type::end_of_input))
        {
            return sax->parse_error(chars_read, number_string, parse_error::create(115, chars_read, exception_message(input_format_t::ubjson, "invalid number text: " + number_lexer.get_token_string(), "high-precision number"), BasicJsonType()));
        }

        switch (result_number)
        {
            case token_type::value_integer:
                return sax->number_integer(number_lexer.get_number_integer());
            case token_type::value_unsigned:
                return sax->number_unsigned(number_lexer.get_number_unsigned());
            case token_type::value_float:
                return sax->number_float(number_lexer.get_number_float(), std::move(number_string));
            default:
                return sax->parse_error(chars_read, number_string, parse_error::create(115, chars_read, exception_message(input_format_t::ubjson, "invalid number text: " + number_lexer.get_token_string(), "high-precision number"), BasicJsonType()));
        }
    }

    ///////////////////////
    // Utility functions //
    ///////////////////////

    /*!
    @brief get next character from the input

    This function provides the interface to the used input adapter. It does
    not throw in case the input reached EOF, but returns a -'ve valued
    `std::char_traits<char_type>::eof()` in that case.

    @return character read from the input
    */
    char_int_type get()
    {
        ++chars_read;
        return current = ia.get_character();
    }

    /*!
    @return character read from the input after ignoring all 'N' entries
    */
    char_int_type get_ignore_noop()
    {
        do
        {
            get();
        }
        while (current == 'N');

        return current;
    }

    /*
    @brief read a number from the input

    @tparam NumberType the type of the number
    @param[in] format   the current format (for diagnostics)
    @param[out] result  number of type @a NumberType

    @return whether conversion completed

    @note This function needs to respect the system's endianess, because
          bytes in CBOR, MessagePack, and UBJSON are stored in network order
          (big endian) and therefore need reordering on little endian systems.
    */
    template<typename NumberType, bool InputIsLittleEndian = false>
    bool get_number(const input_format_t format, NumberType& result)
    {
        // step 1: read input into array with system's byte order
        std::array<std::uint8_t, sizeof(NumberType)> vec{};
        for (std::size_t i = 0; i < sizeof(NumberType); ++i)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(format, "number")))
            {
                return false;
            }

            // reverse byte order prior to conversion if necessary
            if (is_little_endian != InputIsLittleEndian)
            {
                vec[sizeof(NumberType) - i - 1] = static_cast<std::uint8_t>(current);
            }
            else
            {
                vec[i] = static_cast<std::uint8_t>(current); // LCOV_EXCL_LINE
            }
        }

        // step 2: convert array into number of type T and return
        std::memcpy(&result, vec.data(), sizeof(NumberType));
        return true;
    }

    /*!
    @brief create a string by reading characters from the input

    @tparam NumberType the type of the number
    @param[in] format the current format (for diagnostics)
    @param[in] len number of characters to read
    @param[out] result string created by reading @a len bytes

    @return whether string creation completed

    @note We can not reserve @a len bytes for the result, because @a len
          may be too large. Usually, @ref unexpect_eof() detects the end of
          the input before we run out of string memory.
    */
    template<typename NumberType>
    bool get_string(const input_format_t format,
                    const NumberType len,
                    string_t& result)
    {
        bool success = true;
        for (NumberType i = 0; i < len; i++)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(format, "string")))
            {
                success = false;
                break;
            }
            result.push_back(static_cast<typename string_t::value_type>(current));
        }
        return success;
    }

    /*!
    @brief create a byte array by reading bytes from the input

    @tparam NumberType the type of the number
    @param[in] format the current format (for diagnostics)
    @param[in] len number of bytes to read
    @param[out] result byte array created by reading @a len bytes

    @return whether byte array creation completed

    @note We can not reserve @a len bytes for the result, because @a len
          may be too large. Usually, @ref unexpect_eof() detects the end of
          the input before we run out of memory.
    */
    template<typename NumberType>
    bool get_binary(const input_format_t format,
                    const NumberType len,
                    binary_t& result)
    {
        bool success = true;
        for (NumberType i = 0; i < len; i++)
        {
            get();
            if (JSON_HEDLEY_UNLIKELY(!unexpect_eof(format, "binary")))
            {
                success = false;
                break;
            }
            result.push_back(static_cast<std::uint8_t>(current));
        }
        return success;
    }

    /*!
    @param[in] format   the current format (for diagnostics)
    @param[in] context  further context information (for diagnostics)
    @return whether the last read character is not EOF
    */
    JSON_HEDLEY_NON_NULL(3)
    bool unexpect_eof(const input_format_t format, const char* context) const
    {
        if (JSON_HEDLEY_UNLIKELY(current == std::char_traits<char_type>::eof()))
        {
            return sax->parse_error(chars_read, "<end of file>",
                                    parse_error::create(110, chars_read, exception_message(format, "unexpected end of input", context), BasicJsonType()));
        }
        return true;
    }

    /*!
    @return a string representation of the last read byte
    */
    std::string get_token_string() const
    {
        std::array<char, 3> cr{{}};
        (std::snprintf)(cr.data(), cr.size(), "%.2hhX", static_cast<unsigned char>(current)); // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        return std::string{cr.data()};
    }

    /*!
    @param[in] format   the current format
    @param[in] detail   a detailed error message
    @param[in] context  further context information
    @return a message string to use in the parse_error exceptions
    */
    std::string exception_message(const input_format_t format,
                                  const std::string& detail,
                                  const std::string& context) const
    {
        std::string error_msg = "syntax error while parsing ";

        switch (format)
        {
            case input_format_t::cbor:
                error_msg += "CBOR";
                break;

            case input_format_t::msgpack:
                error_msg += "MessagePack";
                break;

            case input_format_t::ubjson:
                error_msg += "UBJSON";
                break;

            case input_format_t::bson:
                error_msg += "BSON";
                break;

            default:            // LCOV_EXCL_LINE
                JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
        }

        return error_msg + " " + context + ": " + detail;
    }

  private:
    /// input adapter
    InputAdapterType ia;

    /// the current character
    char_int_type current = std::char_traits<char_type>::eof();

    /// the number of characters read
    std::size_t chars_read = 0;

    /// whether we can assume little endianess
    const bool is_little_endian = little_endianess();

    /// the SAX parser
    json_sax_t* sax = nullptr;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/input/input_adapters.hpp>

// #include <nlohmann/detail/input/lexer.hpp>

// #include <nlohmann/detail/input/parser.hpp>


#include <cmath> // isfinite
#include <cstdint> // uint8_t
#include <functional> // function
#include <string> // string
#include <utility> // move
#include <vector> // vector

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/input/input_adapters.hpp>

// #include <nlohmann/detail/input/json_sax.hpp>

// #include <nlohmann/detail/input/lexer.hpp>

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/is_sax.hpp>

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{
////////////
// parser //
////////////

enum class parse_event_t : uint8_t
{
    /// the parser read `{` and started to process a JSON object
    object_start,
    /// the parser read `}` and finished processing a JSON object
    object_end,
    /// the parser read `[` and started to process a JSON array
    array_start,
    /// the parser read `]` and finished processing a JSON array
    array_end,
    /// the parser read a key of a value in an object
    key,
    /// the parser finished reading a JSON value
    value
};

template<typename BasicJsonType>
using parser_callback_t =
    std::function<bool(int /*depth*/, parse_event_t /*event*/, BasicJsonType& /*parsed*/)>;

/*!
@brief syntax analysis

This class implements a recursive descent parser.
*/
template<typename BasicJsonType, typename InputAdapterType>
class parser
{
    using number_integer_t = typename BasicJsonType::number_integer_t;
    using number_unsigned_t = typename BasicJsonType::number_unsigned_t;
    using number_float_t = typename BasicJsonType::number_float_t;
    using string_t = typename BasicJsonType::string_t;
    using lexer_t = lexer<BasicJsonType, InputAdapterType>;
    using token_type = typename lexer_t::token_type;

  public:
    /// a parser reading from an input adapter
    explicit parser(InputAdapterType&& adapter,
                    const parser_callback_t<BasicJsonType> cb = nullptr,
                    const bool allow_exceptions_ = true,
                    const bool skip_comments = false)
        : callback(cb)
        , m_lexer(std::move(adapter), skip_comments)
        , allow_exceptions(allow_exceptions_)
    {
        // read first token
        get_token();
    }

    /*!
    @brief public parser interface

    @param[in] strict      whether to expect the last token to be EOF
    @param[in,out] result  parsed JSON value

    @throw parse_error.101 in case of an unexpected token
    @throw parse_error.102 if to_unicode fails or surrogate error
    @throw parse_error.103 if to_unicode fails
    */
    void parse(const bool strict, BasicJsonType& result)
    {
        if (callback)
        {
            json_sax_dom_callback_parser<BasicJsonType> sdp(result, callback, allow_exceptions);
            sax_parse_internal(&sdp);

            // in strict mode, input must be completely read
            if (strict && (get_token() != token_type::end_of_input))
            {
                sdp.parse_error(m_lexer.get_position(),
                                m_lexer.get_token_string(),
                                parse_error::create(101, m_lexer.get_position(),
                                                    exception_message(token_type::end_of_input, "value"), BasicJsonType()));
            }

            // in case of an error, return discarded value
            if (sdp.is_errored())
            {
                result = value_t::discarded;
                return;
            }

            // set top-level value to null if it was discarded by the callback
            // function
            if (result.is_discarded())
            {
                result = nullptr;
            }
        }
        else
        {
            json_sax_dom_parser<BasicJsonType> sdp(result, allow_exceptions);
            sax_parse_internal(&sdp);

            // in strict mode, input must be completely read
            if (strict && (get_token() != token_type::end_of_input))
            {
                sdp.parse_error(m_lexer.get_position(),
                                m_lexer.get_token_string(),
                                parse_error::create(101, m_lexer.get_position(), exception_message(token_type::end_of_input, "value"), BasicJsonType()));
            }

            // in case of an error, return discarded value
            if (sdp.is_errored())
            {
                result = value_t::discarded;
                return;
            }
        }

        result.assert_invariant();
    }

    /*!
    @brief public accept interface

    @param[in] strict  whether to expect the last token to be EOF
    @return whether the input is a proper JSON text
    */
    bool accept(const bool strict = true)
    {
        json_sax_acceptor<BasicJsonType> sax_acceptor;
        return sax_parse(&sax_acceptor, strict);
    }

    template<typename SAX>
    JSON_HEDLEY_NON_NULL(2)
    bool sax_parse(SAX* sax, const bool strict = true)
    {
        (void)detail::is_sax_static_asserts<SAX, BasicJsonType> {};
        const bool result = sax_parse_internal(sax);

        // strict mode: next byte must be EOF
        if (result && strict && (get_token() != token_type::end_of_input))
        {
            return sax->parse_error(m_lexer.get_position(),
                                    m_lexer.get_token_string(),
                                    parse_error::create(101, m_lexer.get_position(), exception_message(token_type::end_of_input, "value"), BasicJsonType()));
        }

        return result;
    }

  private:
    template<typename SAX>
    JSON_HEDLEY_NON_NULL(2)
    bool sax_parse_internal(SAX* sax)
    {
        // stack to remember the hierarchy of structured values we are parsing
        // true = array; false = object
        std::vector<bool> states;
        // value to avoid a goto (see comment where set to true)
        bool skip_to_state_evaluation = false;

        while (true)
        {
            if (!skip_to_state_evaluation)
            {
                // invariant: get_token() was called before each iteration
                switch (last_token)
                {
                    case token_type::begin_object:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->start_object(std::size_t(-1))))
                        {
                            return false;
                        }

                        // closing } -> we are done
                        if (get_token() == token_type::end_object)
                        {
                            if (JSON_HEDLEY_UNLIKELY(!sax->end_object()))
                            {
                                return false;
                            }
                            break;
                        }

                        // parse key
                        if (JSON_HEDLEY_UNLIKELY(last_token != token_type::value_string))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(), exception_message(token_type::value_string, "object key"), BasicJsonType()));
                        }
                        if (JSON_HEDLEY_UNLIKELY(!sax->key(m_lexer.get_string())))
                        {
                            return false;
                        }

                        // parse separator (:)
                        if (JSON_HEDLEY_UNLIKELY(get_token() != token_type::name_separator))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    parse_error::create(101, m_lexer.get_position(), exception_message(token_type::name_separator, "object separator"), BasicJsonType()));
                        }

                        // remember we are now inside an object
                        states.push_back(false);

                        // parse values
                        get_token();
                        continue;
                    }

                    case token_type::begin_array:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->start_array(std::size_t(-1))))
                        {
                            return false;
                        }

                        // closing ] -> we are done
                        if (get_token() == token_type::end_array)
                        {
                            if (JSON_HEDLEY_UNLIKELY(!sax->end_array()))
                            {
                                return false;
                            }
                            break;
                        }

                        // remember we are now inside an array
                        states.push_back(true);

                        // parse values (no need to call get_token)
                        continue;
                    }

                    case token_type::value_float:
                    {
                        const auto res = m_lexer.get_number_float();

                        if (JSON_HEDLEY_UNLIKELY(!std::isfinite(res)))
                        {
                            return sax->parse_error(m_lexer.get_position(),
                                                    m_lexer.get_token_string(),
                                                    out_of_range::create(406, "number overflow parsing '" + m_lexer.get_token_string() + "'", BasicJsonType()));
                        }

                        if (JSON_HEDLEY_UNLIKELY(!sax->number_float(res, m_lexer.get_string())))
                        {
                            return false;
                        }

                        break;
                    }

                    case token_type::literal_false:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->boolean(false)))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::literal_null:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->null()))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::literal_true:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->boolean(true)))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_integer:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->number_integer(m_lexer.get_number_integer())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_string:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->string(m_lexer.get_string())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::value_unsigned:
                    {
                        if (JSON_HEDLEY_UNLIKELY(!sax->number_unsigned(m_lexer.get_number_unsigned())))
                        {
                            return false;
                        }
                        break;
                    }

                    case token_type::parse_error:
                    {
                        // using "uninitialized" to avoid "expected" message
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(), exception_message(token_type::uninitialized, "value"), BasicJsonType()));
                    }

                    default: // the last token was unexpected
                    {
                        return sax->parse_error(m_lexer.get_position(),
                                                m_lexer.get_token_string(),
                                                parse_error::create(101, m_lexer.get_position(), exception_message(token_type::literal_or_value, "value"), BasicJsonType()));
                    }
                }
            }
            else
            {
                skip_to_state_evaluation = false;
            }

            // we reached this line after we successfully parsed a value
            if (states.empty())
            {
                // empty stack: we reached the end of the hierarchy: done
                return true;
            }

            if (states.back())  // array
            {
                // comma -> next value
                if (get_token() == token_type::value_separator)
                {
                    // parse a new value
                    get_token();
                    continue;
                }

                // closing ]
                if (JSON_HEDLEY_LIKELY(last_token == token_type::end_array))
                {
                    if (JSON_HEDLEY_UNLIKELY(!sax->end_array()))
                    {
                        return false;
                    }

                    // We are done with this array. Before we can parse a
                    // new value, we need to evaluate the new state first.
                    // By setting skip_to_state_evaluation to false, we
                    // are effectively jumping to the beginning of this if.
                    JSON_ASSERT(!states.empty());
                    states.pop_back();
                    skip_to_state_evaluation = true;
                    continue;
                }

                return sax->parse_error(m_lexer.get_position(),
                                        m_lexer.get_token_string(),
                                        parse_error::create(101, m_lexer.get_position(), exception_message(token_type::end_array, "array"), BasicJsonType()));
            }

            // states.back() is false -> object

            // comma -> next value
            if (get_token() == token_type::value_separator)
            {
                // parse key
                if (JSON_HEDLEY_UNLIKELY(get_token() != token_type::value_string))
                {
                    return sax->parse_error(m_lexer.get_position(),
                                            m_lexer.get_token_string(),
                                            parse_error::create(101, m_lexer.get_position(), exception_message(token_type::value_string, "object key"), BasicJsonType()));
                }

                if (JSON_HEDLEY_UNLIKELY(!sax->key(m_lexer.get_string())))
                {
                    return false;
                }

                // parse separator (:)
                if (JSON_HEDLEY_UNLIKELY(get_token() != token_type::name_separator))
                {
                    return sax->parse_error(m_lexer.get_position(),
                                            m_lexer.get_token_string(),
                                            parse_error::create(101, m_lexer.get_position(), exception_message(token_type::name_separator, "object separator"), BasicJsonType()));
                }

                // parse values
                get_token();
                continue;
            }

            // closing }
            if (JSON_HEDLEY_LIKELY(last_token == token_type::end_object))
            {
                if (JSON_HEDLEY_UNLIKELY(!sax->end_object()))
                {
                    return false;
                }

                // We are done with this object. Before we can parse a
                // new value, we need to evaluate the new state first.
                // By setting skip_to_state_evaluation to false, we
                // are effectively jumping to the beginning of this if.
                JSON_ASSERT(!states.empty());
                states.pop_back();
                skip_to_state_evaluation = true;
                continue;
            }

            return sax->parse_error(m_lexer.get_position(),
                                    m_lexer.get_token_string(),
                                    parse_error::create(101, m_lexer.get_position(), exception_message(token_type::end_object, "object"), BasicJsonType()));
        }
    }

    /// get next token from lexer
    token_type get_token()
    {
        return last_token = m_lexer.scan();
    }

    std::string exception_message(const token_type expected, const std::string& context)
    {
        std::string error_msg = "syntax error ";

        if (!context.empty())
        {
            error_msg += "while parsing " + context + " ";
        }

        error_msg += "- ";

        if (last_token == token_type::parse_error)
        {
            error_msg += std::string(m_lexer.get_error_message()) + "; last read: '" +
                         m_lexer.get_token_string() + "'";
        }
        else
        {
            error_msg += "unexpected " + std::string(lexer_t::token_type_name(last_token));
        }

        if (expected != token_type::uninitialized)
        {
            error_msg += "; expected " + std::string(lexer_t::token_type_name(expected));
        }

        return error_msg;
    }

  private:
    /// callback function
    const parser_callback_t<BasicJsonType> callback = nullptr;
    /// the type of the last read token
    token_type last_token = token_type::uninitialized;
    /// the lexer
    lexer_t m_lexer;
    /// whether to throw exceptions in case of errors
    const bool allow_exceptions = true;
};

}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/iterators/internal_iterator.hpp>


// #include <nlohmann/detail/iterators/primitive_iterator.hpp>


#include <cstddef> // ptrdiff_t
#include <limits>  // numeric_limits

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
/*
@brief an iterator for primitive JSON types

This class models an iterator for primitive JSON types (boolean, number,
string). It's only purpose is to allow the iterator/const_iterator classes
to "iterate" over primitive values. Internally, the iterator is modeled by
a `difference_type` variable. Value begin_value (`0`) models the begin,
end_value (`1`) models past the end.
*/
class primitive_iterator_t
{
  private:
    using difference_type = std::ptrdiff_t;
    static constexpr difference_type begin_value = 0;
    static constexpr difference_type end_value = begin_value + 1;

  JSON_PRIVATE_UNLESS_TESTED:
    /// iterator as signed integer type
    difference_type m_it = (std::numeric_limits<std::ptrdiff_t>::min)();

  public:
    constexpr difference_type get_value() const noexcept
    {
        return m_it;
    }

    /// set iterator to a defined beginning
    void set_begin() noexcept
    {
        m_it = begin_value;
    }

    /// set iterator to a defined past the end
    void set_end() noexcept
    {
        m_it = end_value;
    }

    /// return whether the iterator can be dereferenced
    constexpr bool is_begin() const noexcept
    {
        return m_it == begin_value;
    }

    /// return whether the iterator is at end
    constexpr bool is_end() const noexcept
    {
        return m_it == end_value;
    }

    friend constexpr bool operator==(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it == rhs.m_it;
    }

    friend constexpr bool operator<(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it < rhs.m_it;
    }

    primitive_iterator_t operator+(difference_type n) noexcept
    {
        auto result = *this;
        result += n;
        return result;
    }

    friend constexpr difference_type operator-(primitive_iterator_t lhs, primitive_iterator_t rhs) noexcept
    {
        return lhs.m_it - rhs.m_it;
    }

    primitive_iterator_t& operator++() noexcept
    {
        ++m_it;
        return *this;
    }

    primitive_iterator_t const operator++(int) noexcept // NOLINT(readability-const-return-type)
    {
        auto result = *this;
        ++m_it;
        return result;
    }

    primitive_iterator_t& operator--() noexcept
    {
        --m_it;
        return *this;
    }

    primitive_iterator_t const operator--(int) noexcept // NOLINT(readability-const-return-type)
    {
        auto result = *this;
        --m_it;
        return result;
    }

    primitive_iterator_t& operator+=(difference_type n) noexcept
    {
        m_it += n;
        return *this;
    }

    primitive_iterator_t& operator-=(difference_type n) noexcept
    {
        m_it -= n;
        return *this;
    }
};
}  // namespace detail
}  // namespace nlohmann


namespace nlohmann
{
namespace detail
{
/*!
@brief an iterator value

@note This structure could easily be a union, but MSVC currently does not allow
unions members with complex constructors, see https://github.com/nlohmann/json/pull/105.
*/
template<typename BasicJsonType> struct internal_iterator
{
    /// iterator for JSON objects
    typename BasicJsonType::object_t::iterator object_iterator {};
    /// iterator for JSON arrays
    typename BasicJsonType::array_t::iterator array_iterator {};
    /// generic iterator for all other types
    primitive_iterator_t primitive_iterator {};
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/iterators/iter_impl.hpp>


#include <iterator> // iterator, random_access_iterator_tag, bidirectional_iterator_tag, advance, next
#include <type_traits> // conditional, is_const, remove_const

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/iterators/internal_iterator.hpp>

// #include <nlohmann/detail/iterators/primitive_iterator.hpp>

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
namespace detail
{
// forward declare, to be able to friend it later on
template<typename IteratorType> class iteration_proxy;
template<typename IteratorType> class iteration_proxy_value;

/*!
@brief a template for a bidirectional iterator for the @ref basic_json class
This class implements a both iterators (iterator and const_iterator) for the
@ref basic_json class.
@note An iterator is called *initialized* when a pointer to a JSON value has
      been set (e.g., by a constructor or a copy assignment). If the iterator is
      default-constructed, it is *uninitialized* and most methods are undefined.
      **The library uses assertions to detect calls on uninitialized iterators.**
@requirement The class satisfies the following concept requirements:
-
[BidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator):
  The iterator that can be moved can be moved in both directions (i.e.
  incremented and decremented).
@since version 1.0.0, simplified in version 2.0.9, change to bidirectional
       iterators in version 3.0.0 (see https://github.com/nlohmann/json/issues/593)
*/
template<typename BasicJsonType>
class iter_impl
{
    /// the iterator with BasicJsonType of different const-ness
    using other_iter_impl = iter_impl<typename std::conditional<std::is_const<BasicJsonType>::value, typename std::remove_const<BasicJsonType>::type, const BasicJsonType>::type>;
    /// allow basic_json to access private members
    friend other_iter_impl;
    friend BasicJsonType;
    friend iteration_proxy<iter_impl>;
    friend iteration_proxy_value<iter_impl>;

    using object_t = typename BasicJsonType::object_t;
    using array_t = typename BasicJsonType::array_t;
    // make sure BasicJsonType is basic_json or const basic_json
    static_assert(is_basic_json<typename std::remove_const<BasicJsonType>::type>::value,
                  "iter_impl only accepts (const) basic_json");

  public:

    /// The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17.
    /// The C++ Standard has never required user-defined iterators to derive from std::iterator.
    /// A user-defined iterator should provide publicly accessible typedefs named
    /// iterator_category, value_type, difference_type, pointer, and reference.
    /// Note that value_type is required to be non-const, even for constant iterators.
    using iterator_category = std::bidirectional_iterator_tag;

    /// the type of the values when the iterator is dereferenced
    using value_type = typename BasicJsonType::value_type;
    /// a type to represent differences between iterators
    using difference_type = typename BasicJsonType::difference_type;
    /// defines a pointer to the type iterated over (value_type)
    using pointer = typename std::conditional<std::is_const<BasicJsonType>::value,
          typename BasicJsonType::const_pointer,
          typename BasicJsonType::pointer>::type;
    /// defines a reference to the type iterated over (value_type)
    using reference =
        typename std::conditional<std::is_const<BasicJsonType>::value,
        typename BasicJsonType::const_reference,
        typename BasicJsonType::reference>::type;

    iter_impl() = default;
    ~iter_impl() = default;
    iter_impl(iter_impl&&) noexcept = default;
    iter_impl& operator=(iter_impl&&) noexcept = default;

    /*!
    @brief constructor for a given JSON instance
    @param[in] object  pointer to a JSON object for this iterator
    @pre object != nullptr
    @post The iterator is initialized; i.e. `m_object != nullptr`.
    */
    explicit iter_impl(pointer object) noexcept : m_object(object)
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = typename object_t::iterator();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = typename array_t::iterator();
                break;
            }

            default:
            {
                m_it.primitive_iterator = primitive_iterator_t();
                break;
            }
        }
    }

    /*!
    @note The conventional copy constructor and copy assignment are implicitly
          defined. Combined with the following converting constructor and
          assignment, they support: (1) copy from iterator to iterator, (2)
          copy from const iterator to const iterator, and (3) conversion from
          iterator to const iterator. However conversion from const iterator
          to iterator is not defined.
    */

    /*!
    @brief const copy constructor
    @param[in] other const iterator to copy from
    @note This copy constructor had to be defined explicitly to circumvent a bug
          occurring on msvc v19.0 compiler (VS 2015) debug build. For more
          information refer to: https://github.com/nlohmann/json/issues/1608
    */
    iter_impl(const iter_impl<const BasicJsonType>& other) noexcept
        : m_object(other.m_object), m_it(other.m_it)
    {}

    /*!
    @brief converting assignment
    @param[in] other const iterator to copy from
    @return const/non-const iterator
    @note It is not checked whether @a other is initialized.
    */
    iter_impl& operator=(const iter_impl<const BasicJsonType>& other) noexcept
    {
        if (&other != this)
        {
            m_object = other.m_object;
            m_it = other.m_it;
        }
        return *this;
    }

    /*!
    @brief converting constructor
    @param[in] other  non-const iterator to copy from
    @note It is not checked whether @a other is initialized.
    */
    iter_impl(const iter_impl<typename std::remove_const<BasicJsonType>::type>& other) noexcept
        : m_object(other.m_object), m_it(other.m_it)
    {}

    /*!
    @brief converting assignment
    @param[in] other  non-const iterator to copy from
    @return const/non-const iterator
    @note It is not checked whether @a other is initialized.
    */
    iter_impl& operator=(const iter_impl<typename std::remove_const<BasicJsonType>::type>& other) noexcept // NOLINT(cert-oop54-cpp)
    {
        m_object = other.m_object;
        m_it = other.m_it;
        return *this;
    }

  JSON_PRIVATE_UNLESS_TESTED:
    /*!
    @brief set the iterator to the first value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_begin() noexcept
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = m_object->m_value.object->begin();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = m_object->m_value.array->begin();
                break;
            }

            case value_t::null:
            {
                // set to end so begin()==end() is true: null is empty
                m_it.primitive_iterator.set_end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_begin();
                break;
            }
        }
    }

    /*!
    @brief set the iterator past the last value
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    void set_end() noexcept
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                m_it.object_iterator = m_object->m_value.object->end();
                break;
            }

            case value_t::array:
            {
                m_it.array_iterator = m_object->m_value.array->end();
                break;
            }

            default:
            {
                m_it.primitive_iterator.set_end();
                break;
            }
        }
    }

  public:
    /*!
    @brief return a reference to the value pointed to by the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator*() const
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                JSON_ASSERT(m_it.object_iterator != m_object->m_value.object->end());
                return m_it.object_iterator->second;
            }

            case value_t::array:
            {
                JSON_ASSERT(m_it.array_iterator != m_object->m_value.array->end());
                return *m_it.array_iterator;
            }

            case value_t::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value", *m_object));

            default:
            {
                if (JSON_HEDLEY_LIKELY(m_it.primitive_iterator.is_begin()))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value", *m_object));
            }
        }
    }

    /*!
    @brief dereference the iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    pointer operator->() const
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                JSON_ASSERT(m_it.object_iterator != m_object->m_value.object->end());
                return &(m_it.object_iterator->second);
            }

            case value_t::array:
            {
                JSON_ASSERT(m_it.array_iterator != m_object->m_value.array->end());
                return &*m_it.array_iterator;
            }

            default:
            {
                if (JSON_HEDLEY_LIKELY(m_it.primitive_iterator.is_begin()))
                {
                    return m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value", *m_object));
            }
        }
    }

    /*!
    @brief post-increment (it++)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl const operator++(int) // NOLINT(readability-const-return-type)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    /*!
    @brief pre-increment (++it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator++()
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                std::advance(m_it.object_iterator, 1);
                break;
            }

            case value_t::array:
            {
                std::advance(m_it.array_iterator, 1);
                break;
            }

            default:
            {
                ++m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief post-decrement (it--)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl const operator--(int) // NOLINT(readability-const-return-type)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    /*!
    @brief pre-decrement (--it)
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator--()
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
            {
                std::advance(m_it.object_iterator, -1);
                break;
            }

            case value_t::array:
            {
                std::advance(m_it.array_iterator, -1);
                break;
            }

            default:
            {
                --m_it.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief comparison: equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    template < typename IterImpl, detail::enable_if_t < (std::is_same<IterImpl, iter_impl>::value || std::is_same<IterImpl, other_iter_impl>::value), std::nullptr_t > = nullptr >
    bool operator==(const IterImpl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if (JSON_HEDLEY_UNLIKELY(m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers", *m_object));
        }

        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                return (m_it.object_iterator == other.m_it.object_iterator);

            case value_t::array:
                return (m_it.array_iterator == other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator == other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief comparison: not equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    template < typename IterImpl, detail::enable_if_t < (std::is_same<IterImpl, iter_impl>::value || std::is_same<IterImpl, other_iter_impl>::value), std::nullptr_t > = nullptr >
    bool operator!=(const IterImpl& other) const
    {
        return !operator==(other);
    }

    /*!
    @brief comparison: smaller
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<(const iter_impl& other) const
    {
        // if objects are not the same, the comparison is undefined
        if (JSON_HEDLEY_UNLIKELY(m_object != other.m_object))
        {
            JSON_THROW(invalid_iterator::create(212, "cannot compare iterators of different containers", *m_object));
        }

        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(213, "cannot compare order of object iterators", *m_object));

            case value_t::array:
                return (m_it.array_iterator < other.m_it.array_iterator);

            default:
                return (m_it.primitive_iterator < other.m_it.primitive_iterator);
        }
    }

    /*!
    @brief comparison: less than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator<=(const iter_impl& other) const
    {
        return !other.operator < (*this);
    }

    /*!
    @brief comparison: greater than
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>(const iter_impl& other) const
    {
        return !operator<=(other);
    }

    /*!
    @brief comparison: greater than or equal
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    bool operator>=(const iter_impl& other) const
    {
        return !operator<(other);
    }

    /*!
    @brief add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator+=(difference_type i)
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators", *m_object));

            case value_t::array:
            {
                std::advance(m_it.array_iterator, i);
                break;
            }

            default:
            {
                m_it.primitive_iterator += i;
                break;
            }
        }

        return *this;
    }

    /*!
    @brief subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl& operator-=(difference_type i)
    {
        return operator+=(-i);
    }

    /*!
    @brief add to iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl operator+(difference_type i) const
    {
        auto result = *this;
        result += i;
        return result;
    }

    /*!
    @brief addition of distance and iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    friend iter_impl operator+(difference_type i, const iter_impl& it)
    {
        auto result = it;
        result += i;
        return result;
    }

    /*!
    @brief subtract from iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    iter_impl operator-(difference_type i) const
    {
        auto result = *this;
        result -= i;
        return result;
    }

    /*!
    @brief return difference
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    difference_type operator-(const iter_impl& other) const
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(209, "cannot use offsets with object iterators", *m_object));

            case value_t::array:
                return m_it.array_iterator - other.m_it.array_iterator;

            default:
                return m_it.primitive_iterator - other.m_it.primitive_iterator;
        }
    }

    /*!
    @brief access to successor
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference operator[](difference_type n) const
    {
        JSON_ASSERT(m_object != nullptr);

        switch (m_object->m_type)
        {
            case value_t::object:
                JSON_THROW(invalid_iterator::create(208, "cannot use operator[] for object iterators", *m_object));

            case value_t::array:
                return *std::next(m_it.array_iterator, n);

            case value_t::null:
                JSON_THROW(invalid_iterator::create(214, "cannot get value", *m_object));

            default:
            {
                if (JSON_HEDLEY_LIKELY(m_it.primitive_iterator.get_value() == -n))
                {
                    return *m_object;
                }

                JSON_THROW(invalid_iterator::create(214, "cannot get value", *m_object));
            }
        }
    }

    /*!
    @brief return the key of an object iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    const typename object_t::key_type& key() const
    {
        JSON_ASSERT(m_object != nullptr);

        if (JSON_HEDLEY_LIKELY(m_object->is_object()))
        {
            return m_it.object_iterator->first;
        }

        JSON_THROW(invalid_iterator::create(207, "cannot use key() for non-object iterators", *m_object));
    }

    /*!
    @brief return the value of an iterator
    @pre The iterator is initialized; i.e. `m_object != nullptr`.
    */
    reference value() const
    {
        return operator*();
    }

  JSON_PRIVATE_UNLESS_TESTED:
    /// associated JSON instance
    pointer m_object = nullptr;
    /// the actual iterator of the associated instance
    internal_iterator<typename std::remove_const<BasicJsonType>::type> m_it {};
};
} // namespace detail
} // namespace nlohmann

// #include <nlohmann/detail/iterators/iteration_proxy.hpp>

// #include <nlohmann/detail/iterators/json_reverse_iterator.hpp>


#include <cstddef> // ptrdiff_t
#include <iterator> // reverse_iterator
#include <utility> // declval

namespace nlohmann
{
namespace detail
{
//////////////////////
// reverse_iterator //
//////////////////////

/*!
@brief a template for a reverse iterator class

@tparam Base the base iterator type to reverse. Valid types are @ref
iterator (to create @ref reverse_iterator) and @ref const_iterator (to
create @ref const_reverse_iterator).

@requirement The class satisfies the following concept requirements:
-
[BidirectionalIterator](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator):
  The iterator that can be moved can be moved in both directions (i.e.
  incremented and decremented).
- [OutputIterator](https://en.cppreference.com/w/cpp/named_req/OutputIterator):
  It is possible to write to the pointed-to element (only if @a Base is
  @ref iterator).

@since version 1.0.0
*/
template<typename Base>
class json_reverse_iterator : public std::reverse_iterator<Base>
{
  public:
    using difference_type = std::ptrdiff_t;
    /// shortcut to the reverse iterator adapter
    using base_iterator = std::reverse_iterator<Base>;
    /// the reference type for the pointed-to element
    using reference = typename Base::reference;

    /// create reverse iterator from iterator
    explicit json_reverse_iterator(const typename base_iterator::iterator_type& it) noexcept
        : base_iterator(it) {}

    /// create reverse iterator from base class
    explicit json_reverse_iterator(const base_iterator& it) noexcept : base_iterator(it) {}

    /// post-increment (it++)
    json_reverse_iterator const operator++(int) // NOLINT(readability-const-return-type)
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator++(1));
    }

    /// pre-increment (++it)
    json_reverse_iterator& operator++()
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator++());
    }

    /// post-decrement (it--)
    json_reverse_iterator const operator--(int) // NOLINT(readability-const-return-type)
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator--(1));
    }

    /// pre-decrement (--it)
    json_reverse_iterator& operator--()
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator--());
    }

    /// add to iterator
    json_reverse_iterator& operator+=(difference_type i)
    {
        return static_cast<json_reverse_iterator&>(base_iterator::operator+=(i));
    }

    /// add to iterator
    json_reverse_iterator operator+(difference_type i) const
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator+(i));
    }

    /// subtract from iterator
    json_reverse_iterator operator-(difference_type i) const
    {
        return static_cast<json_reverse_iterator>(base_iterator::operator-(i));
    }

    /// return difference
    difference_type operator-(const json_reverse_iterator& other) const
    {
        return base_iterator(*this) - base_iterator(other);
    }

    /// access to successor
    reference operator[](difference_type n) const
    {
        return *(this->operator+(n));
    }

    /// return the key of an object iterator
    auto key() const -> decltype(std::declval<Base>().key())
    {
        auto it = --this->base();
        return it.key();
    }

    /// return the value of an iterator
    reference value() const
    {
        auto it = --this->base();
        return it.operator * ();
    }
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/iterators/primitive_iterator.hpp>

// #include <nlohmann/detail/json_pointer.hpp>


#include <algorithm> // all_of
#include <cctype> // isdigit
#include <limits> // max
#include <numeric> // accumulate
#include <string> // string
#include <utility> // move
#include <vector> // vector

// #include <nlohmann/detail/exceptions.hpp>

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/string_escape.hpp>

// #include <nlohmann/detail/value_t.hpp>


namespace nlohmann
{
template<typename BasicJsonType>
class json_pointer
{
    // allow basic_json to access private members
    NLOHMANN_BASIC_JSON_TPL_DECLARATION
    friend class basic_json;

  public:
    /*!
    @brief create JSON pointer

    Create a JSON pointer according to the syntax described in
    [Section 3 of RFC6901](https://tools.ietf.org/html/rfc6901#section-3).

    @param[in] s  string representing the JSON pointer; if omitted, the empty
                  string is assumed which references the whole JSON value

    @throw parse_error.107 if the given JSON pointer @a s is nonempty and does
                           not begin with a slash (`/`); see example below

    @throw parse_error.108 if a tilde (`~`) in the given JSON pointer @a s is
    not followed by `0` (representing `~`) or `1` (representing `/`); see
    example below

    @liveexample{The example shows the construction several valid JSON pointers
    as well as the exceptional behavior.,json_pointer}

    @since version 2.0.0
    */
    explicit json_pointer(const std::string& s = "")
        : reference_tokens(split(s))
    {}

    /*!
    @brief return a string representation of the JSON pointer

    @invariant For each JSON pointer `ptr`, it holds:
    @code {.cpp}
    ptr == json_pointer(ptr.to_string());
    @endcode

    @return a string representation of the JSON pointer

    @liveexample{The example shows the result of `to_string`.,json_pointer__to_string}

    @since version 2.0.0
    */
    std::string to_string() const
    {
        return std::accumulate(reference_tokens.begin(), reference_tokens.end(),
                               std::string{},
                               [](const std::string & a, const std::string & b)
        {
            return a + "/" + detail::escape(b);
        });
    }

    /// @copydoc to_string()
    operator std::string() const
    {
        return to_string();
    }

    /*!
    @brief append another JSON pointer at the end of this JSON pointer

    @param[in] ptr  JSON pointer to append
    @return JSON pointer with @a ptr appended

    @liveexample{The example shows the usage of `operator/=`.,json_pointer__operator_add}

    @complexity Linear in the length of @a ptr.

    @sa see @ref operator/=(std::string) to append a reference token
    @sa see @ref operator/=(std::size_t) to append an array index
    @sa see @ref operator/(const json_pointer&, const json_pointer&) for a binary operator

    @since version 3.6.0
    */
    json_pointer& operator/=(const json_pointer& ptr)
    {
        reference_tokens.insert(reference_tokens.end(),
                                ptr.reference_tokens.begin(),
                                ptr.reference_tokens.end());
        return *this;
    }

    /*!
    @brief append an unescaped reference token at the end of this JSON pointer

    @param[in] token  reference token to append
    @return JSON pointer with @a token appended without escaping @a token

    @liveexample{The example shows the usage of `operator/=`.,json_pointer__operator_add}

    @complexity Amortized constant.

    @sa see @ref operator/=(const json_pointer&) to append a JSON pointer
    @sa see @ref operator/=(std::size_t) to append an array index
    @sa see @ref operator/(const json_pointer&, std::size_t) for a binary operator

    @since version 3.6.0
    */
    json_pointer& operator/=(std::string token)
    {
        push_back(std::move(token));
        return *this;
    }

    /*!
    @brief append an array index at the end of this JSON pointer

    @param[in] array_idx  array index to append
    @return JSON pointer with @a array_idx appended

    @liveexample{The example shows the usage of `operator/=`.,json_pointer__operator_add}

    @complexity Amortized constant.

    @sa see @ref operator/=(const json_pointer&) to append a JSON pointer
    @sa see @ref operator/=(std::string) to append a reference token
    @sa see @ref operator/(const json_pointer&, std::string) for a binary operator

    @since version 3.6.0
    */
    json_pointer& operator/=(std::size_t array_idx)
    {
        return *this /= std::to_string(array_idx);
    }

    /*!
    @brief create a new JSON pointer by appending the right JSON pointer at the end of the left JSON pointer

    @param[in] lhs  JSON pointer
    @param[in] rhs  JSON pointer
    @return a new JSON pointer with @a rhs appended to @a lhs

    @liveexample{The example shows the usage of `operator/`.,json_pointer__operator_add_binary}

    @complexity Linear in the length of @a lhs and @a rhs.

    @sa see @ref operator/=(const json_pointer&) to append a JSON pointer

    @since version 3.6.0
    */
    friend json_pointer operator/(const json_pointer& lhs,
                                  const json_pointer& rhs)
    {
        return json_pointer(lhs) /= rhs;
    }

    /*!
    @brief create a new JSON pointer by appending the unescaped token at the end of the JSON pointer

    @param[in] ptr  JSON pointer
    @param[in] token  reference token
    @return a new JSON pointer with unescaped @a token appended to @a ptr

    @liveexample{The example shows the usage of `operator/`.,json_pointer__operator_add_binary}

    @complexity Linear in the length of @a ptr.

    @sa see @ref operator/=(std::string) to append a reference token

    @since version 3.6.0
    */
    friend json_pointer operator/(const json_pointer& ptr, std::string token) // NOLINT(performance-unnecessary-value-param)
    {
        return json_pointer(ptr) /= std::move(token);
    }

    /*!
    @brief create a new JSON pointer by appending the array-index-token at the end of the JSON pointer

    @param[in] ptr  JSON pointer
    @param[in] array_idx  array index
    @return a new JSON pointer with @a array_idx appended to @a ptr

    @liveexample{The example shows the usage of `operator/`.,json_pointer__operator_add_binary}

    @complexity Linear in the length of @a ptr.

    @sa see @ref operator/=(std::size_t) to append an array index

    @since version 3.6.0
    */
    friend json_pointer operator/(const json_pointer& ptr, std::size_t array_idx)
    {
        return json_pointer(ptr) /= array_idx;
    }

    /*!
    @brief returns the parent of this JSON pointer

    @return parent of this JSON pointer; in case this JSON pointer is the root,
            the root itself is returned

    @complexity Linear in the length of the JSON pointer.

    @liveexample{The example shows the result of `parent_pointer` for different
    JSON Pointers.,json_pointer__parent_pointer}

    @since version 3.6.0
    */
    json_pointer parent_pointer() const
    {
        if (empty())
        {
            return *this;
        }

        json_pointer res = *this;
        res.pop_back();
        return res;
    }

    /*!
    @brief remove last reference token

    @pre not `empty()`

    @liveexample{The example shows the usage of `pop_back`.,json_pointer__pop_back}

    @complexity Constant.

    @throw out_of_range.405 if JSON pointer has no parent

    @since version 3.6.0
    */
    void pop_back()
    {
        if (JSON_HEDLEY_UNLIKELY(empty()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent", BasicJsonType()));
        }

        reference_tokens.pop_back();
    }

    /*!
    @brief return last reference token

    @pre not `empty()`
    @return last reference token

    @liveexample{The example shows the usage of `back`.,json_pointer__back}

    @complexity Constant.

    @throw out_of_range.405 if JSON pointer has no parent

    @since version 3.6.0
    */
    const std::string& back() const
    {
        if (JSON_HEDLEY_UNLIKELY(empty()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent", BasicJsonType()));
        }

        return reference_tokens.back();
    }

    /*!
    @brief append an unescaped token at the end of the reference pointer

    @param[in] token  token to add

    @complexity Amortized constant.

    @liveexample{The example shows the result of `push_back` for different
    JSON Pointers.,json_pointer__push_back}

    @since version 3.6.0
    */
    void push_back(const std::string& token)
    {
        reference_tokens.push_back(token);
    }

    /// @copydoc push_back(const std::string&)
    void push_back(std::string&& token)
    {
        reference_tokens.push_back(std::move(token));
    }

    /*!
    @brief return whether pointer points to the root document

    @return true iff the JSON pointer points to the root document

    @complexity Constant.

    @exceptionsafety No-throw guarantee: this function never throws exceptions.

    @liveexample{The example shows the result of `empty` for different JSON
    Pointers.,json_pointer__empty}

    @since version 3.6.0
    */
    bool empty() const noexcept
    {
        return reference_tokens.empty();
    }

  private:
    /*!
    @param[in] s  reference token to be converted into an array index

    @return integer representation of @a s

    @throw parse_error.106  if an array index begins with '0'
    @throw parse_error.109  if an array index begins not with a digit
    @throw out_of_range.404 if string @a s could not be converted to an integer
    @throw out_of_range.410 if an array index exceeds size_type
    */
    static typename BasicJsonType::size_type array_index(const std::string& s)
    {
        using size_type = typename BasicJsonType::size_type;

        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_HEDLEY_UNLIKELY(s.size() > 1 && s[0] == '0'))
        {
            JSON_THROW(detail::parse_error::create(106, 0, "array index '" + s + "' must not begin with '0'", BasicJsonType()));
        }

        // error condition (cf. RFC 6901, Sect. 4)
        if (JSON_HEDLEY_UNLIKELY(s.size() > 1 && !(s[0] >= '1' && s[0] <= '9')))
        {
            JSON_THROW(detail::parse_error::create(109, 0, "array index '" + s + "' is not a number", BasicJsonType()));
        }

        std::size_t processed_chars = 0;
        unsigned long long res = 0;  // NOLINT(runtime/int)
        JSON_TRY
        {
            res = std::stoull(s, &processed_chars);
        }
        JSON_CATCH(std::out_of_range&)
        {
            JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + s + "'", BasicJsonType()));
        }

        // check if the string was completely read
        if (JSON_HEDLEY_UNLIKELY(processed_chars != s.size()))
        {
            JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + s + "'", BasicJsonType()));
        }

        // only triggered on special platforms (like 32bit), see also
        // https://github.com/nlohmann/json/pull/2203
        if (res >= static_cast<unsigned long long>((std::numeric_limits<size_type>::max)()))  // NOLINT(runtime/int)
        {
            JSON_THROW(detail::out_of_range::create(410, "array index " + s + " exceeds size_type", BasicJsonType())); // LCOV_EXCL_LINE
        }

        return static_cast<size_type>(res);
    }

  JSON_PRIVATE_UNLESS_TESTED:
    json_pointer top() const
    {
        if (JSON_HEDLEY_UNLIKELY(empty()))
        {
            JSON_THROW(detail::out_of_range::create(405, "JSON pointer has no parent", BasicJsonType()));
        }

        json_pointer result = *this;
        result.reference_tokens = {reference_tokens[0]};
        return result;
    }

  private:
    /*!
    @brief create and return a reference to the pointed to value

    @complexity Linear in the number of reference tokens.

    @throw parse_error.109 if array index is not a number
    @throw type_error.313 if value cannot be unflattened
    */
    BasicJsonType& get_and_create(BasicJsonType& j) const
    {
        auto* result = &j;

        // in case no reference tokens exist, return a reference to the JSON value
        // j which will be overwritten by a primitive value
        for (const auto& reference_token : reference_tokens)
        {
            switch (result->type())
            {
                case detail::value_t::null:
                {
                    if (reference_token == "0")
                    {
                        // start a new array if reference token is 0
                        result = &result->operator[](0);
                    }
                    else
                    {
                        // start a new object otherwise
                        result = &result->operator[](reference_token);
                    }
                    break;
                }

                case detail::value_t::object:
                {
                    // create an entry in the object
                    result = &result->operator[](reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    // create an entry in the array
                    result = &result->operator[](array_index(reference_token));
                    break;
                }

                /*
                The following code is only reached if there exists a reference
                token _and_ the current value is primitive. In this case, we have
                an error situation, because primitive values may only occur as
                single value; that is, with an empty list of reference tokens.
                */
                default:
                    JSON_THROW(detail::type_error::create(313, "invalid value to unflatten", j));
            }
        }

        return *result;
    }

    /*!
    @brief return a reference to the pointed to value

    @note This version does not throw if a value is not present, but tries to
          create nested values instead. For instance, calling this function
          with pointer `"/this/that"` on a null value is equivalent to calling
          `operator[]("this").operator[]("that")` on that value, effectively
          changing the null value to an object.

    @param[in] ptr  a JSON value

    @return reference to the JSON value pointed to by the JSON pointer

    @complexity Linear in the length of the JSON pointer.

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    BasicJsonType& get_unchecked(BasicJsonType* ptr) const
    {
        for (const auto& reference_token : reference_tokens)
        {
            // convert null values to arrays or objects before continuing
            if (ptr->is_null())
            {
                // check if reference token is a number
                const bool nums =
                    std::all_of(reference_token.begin(), reference_token.end(),
                                [](const unsigned char x)
                {
                    return std::isdigit(x);
                });

                // change value to array for numbers or "-" or to object otherwise
                *ptr = (nums || reference_token == "-")
                       ? detail::value_t::array
                       : detail::value_t::object;
            }

            switch (ptr->type())
            {
                case detail::value_t::object:
                {
                    // use unchecked object access
                    ptr = &ptr->operator[](reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    if (reference_token == "-")
                    {
                        // explicitly treat "-" as index beyond the end
                        ptr = &ptr->operator[](ptr->m_value.array->size());
                    }
                    else
                    {
                        // convert array index to number; unchecked access
                        ptr = &ptr->operator[](array_index(reference_token));
                    }
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'", *ptr));
            }
        }

        return *ptr;
    }

    /*!
    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    BasicJsonType& get_checked(BasicJsonType* ptr) const
    {
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type())
            {
                case detail::value_t::object:
                {
                    // note: at performs range check
                    ptr = &ptr->at(reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    if (JSON_HEDLEY_UNLIKELY(reference_token == "-"))
                    {
                        // "-" always fails the range check
                        JSON_THROW(detail::out_of_range::create(402,
                                                                "array index '-' (" + std::to_string(ptr->m_value.array->size()) +
                                                                ") is out of range", *ptr));
                    }

                    // note: at performs range check
                    ptr = &ptr->at(array_index(reference_token));
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'", *ptr));
            }
        }

        return *ptr;
    }

    /*!
    @brief return a const reference to the pointed to value

    @param[in] ptr  a JSON value

    @return const reference to the JSON value pointed to by the JSON
    pointer

    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    const BasicJsonType& get_unchecked(const BasicJsonType* ptr) const
    {
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type())
            {
                case detail::value_t::object:
                {
                    // use unchecked object access
                    ptr = &ptr->operator[](reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    if (JSON_HEDLEY_UNLIKELY(reference_token == "-"))
                    {
                        // "-" cannot be used for const access
                        JSON_THROW(detail::out_of_range::create(402, "array index '-' (" + std::to_string(ptr->m_value.array->size()) + ") is out of range", *ptr));
                    }

                    // use unchecked array access
                    ptr = &ptr->operator[](array_index(reference_token));
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'", *ptr));
            }
        }

        return *ptr;
    }

    /*!
    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    @throw out_of_range.402  if the array index '-' is used
    @throw out_of_range.404  if the JSON pointer can not be resolved
    */
    const BasicJsonType& get_checked(const BasicJsonType* ptr) const
    {
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type())
            {
                case detail::value_t::object:
                {
                    // note: at performs range check
                    ptr = &ptr->at(reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    if (JSON_HEDLEY_UNLIKELY(reference_token == "-"))
                    {
                        // "-" always fails the range check
                        JSON_THROW(detail::out_of_range::create(402,
                                                                "array index '-' (" + std::to_string(ptr->m_value.array->size()) +
                                                                ") is out of range", *ptr));
                    }

                    // note: at performs range check
                    ptr = &ptr->at(array_index(reference_token));
                    break;
                }

                default:
                    JSON_THROW(detail::out_of_range::create(404, "unresolved reference token '" + reference_token + "'", *ptr));
            }
        }

        return *ptr;
    }

    /*!
    @throw parse_error.106   if an array index begins with '0'
    @throw parse_error.109   if an array index was not a number
    */
    bool contains(const BasicJsonType* ptr) const
    {
        for (const auto& reference_token : reference_tokens)
        {
            switch (ptr->type())
            {
                case detail::value_t::object:
                {
                    if (!ptr->contains(reference_token))
                    {
                        // we did not find the key in the object
                        return false;
                    }

                    ptr = &ptr->operator[](reference_token);
                    break;
                }

                case detail::value_t::array:
                {
                    if (JSON_HEDLEY_UNLIKELY(reference_token == "-"))
                    {
                        // "-" always fails the range check
                        return false;
                    }
                    if (JSON_HEDLEY_UNLIKELY(reference_token.size() == 1 && !("0" <= reference_token && reference_token <= "9")))
                    {
                        // invalid char
                        return false;
                    }
                    if (JSON_HEDLEY_UNLIKELY(reference_token.size() > 1))
                    {
                        if (JSON_HEDLEY_UNLIKELY(!('1' <= reference_token[0] && reference_token[0] <= '9')))
                        {
                            // first char should be between '1' and '9'
                            return false;
                        }
                        for (std::size_t i = 1; i < reference_token.size(); i++)
                        {
                            if (JSON_HEDLEY_UNLIKELY(!('0' <= reference_token[i] && reference_token[i] <= '9')))
                            {
                                // other char should be between '0' and '9'
                                return false;
                            }
                        }
                    }

                    const auto idx = array_index(reference_token);
                    if (idx >= ptr->size())
                    {
                        // index out of range
                        return false;
                    }

                    ptr = &ptr->operator[](idx);
                    break;
                }

                default:
                {
                    // we do not expect primitive values if there is still a
                    // reference token to process
                    return false;
                }
            }
        }

        // no reference token left means we found a primitive value
        return true;
    }

    /*!
    @brief split the string input to reference tokens

    @note This function is only called by the json_pointer constructor.
          All exceptions below are documented there.

    @throw parse_error.107  if the pointer is not empty or begins with '/'
    @throw parse_error.108  if character '~' is not followed by '0' or '1'
    */
    static std::vector<std::string> split(const std::string& reference_string)
    {
        std::vector<std::string> result;

        // special case: empty reference string -> no reference tokens
        if (reference_string.empty())
        {
            return result;
        }

        // check if nonempty reference string begins with slash
        if (JSON_HEDLEY_UNLIKELY(reference_string[0] != '/'))
        {
            JSON_THROW(detail::parse_error::create(107, 1, "JSON pointer must be empty or begin with '/' - was: '" + reference_string + "'", BasicJsonType()));
        }

        // extract the reference tokens:
        // - slash: position of the last read slash (or end of string)
        // - start: position after the previous slash
        for (
            // search for the first slash after the first character
            std::size_t slash = reference_string.find_first_of('/', 1),
            // set the beginning of the first reference token
            start = 1;
            // we can stop if start == 0 (if slash == std::string::npos)
            start != 0;
            // set the beginning of the next reference token
            // (will eventually be 0 if slash == std::string::npos)
            start = (slash == std::string::npos) ? 0 : slash + 1,
            // find next slash
            slash = reference_string.find_first_of('/', start))
        {
            // use the text between the beginning of the reference token
            // (start) and the last slash (slash).
            auto reference_token = reference_string.substr(start, slash - start);

            // check reference tokens are properly escaped
            for (std::size_t pos = reference_token.find_first_of('~');
                    pos != std::string::npos;
                    pos = reference_token.find_first_of('~', pos + 1))
            {
                JSON_ASSERT(reference_token[pos] == '~');

                // ~ must be followed by 0 or 1
                if (JSON_HEDLEY_UNLIKELY(pos == reference_token.size() - 1 ||
                                         (reference_token[pos + 1] != '0' &&
                                          reference_token[pos + 1] != '1')))
                {
                    JSON_THROW(detail::parse_error::create(108, 0, "escape character '~' must be followed with '0' or '1'", BasicJsonType()));
                }
            }

            // finally, store the reference token
            detail::unescape(reference_token);
            result.push_back(reference_token);
        }

        return result;
    }

  private:
    /*!
    @param[in] reference_string  the reference string to the current value
    @param[in] value             the value to consider
    @param[in,out] result        the result object to insert values to

    @note Empty objects or arrays are flattened to `null`.
    */
    static void flatten(const std::string& reference_string,
                        const BasicJsonType& value,
                        BasicJsonType& result)
    {
        switch (value.type())
        {
            case detail::value_t::array:
            {
                if (value.m_value.array->empty())
                {
                    // flatten empty array as null
                    result[reference_string] = nullptr;
                }
                else
                {
                    // iterate array and use index as reference string
                    for (std::size_t i = 0; i < value.m_value.array->size(); ++i)
                    {
                        flatten(reference_string + "/" + std::to_string(i),
                                value.m_value.array->operator[](i), result);
                    }
                }
                break;
            }

            case detail::value_t::object:
            {
                if (value.m_value.object->empty())
                {
                    // flatten empty object as null
                    result[reference_string] = nullptr;
                }
                else
                {
                    // iterate object and use keys as reference string
                    for (const auto& element : *value.m_value.object)
                    {
                        flatten(reference_string + "/" + detail::escape(element.first), element.second, result);
                    }
                }
                break;
            }

            default:
            {
                // add primitive value with its reference string
                result[reference_string] = value;
                break;
            }
        }
    }

    /*!
    @param[in] value  flattened JSON

    @return unflattened JSON

    @throw parse_error.109 if array index is not a number
    @throw type_error.314  if value is not an object
    @throw type_error.315  if object values are not primitive
    @throw type_error.313  if value cannot be unflattened
    */
    static BasicJsonType
    unflatten(const BasicJsonType& value)
    {
        if (JSON_HEDLEY_UNLIKELY(!value.is_object()))
        {
            JSON_THROW(detail::type_error::create(314, "only objects can be unflattened", value));
        }

        BasicJsonType result;

        // iterate the JSON object values
        for (const auto& element : *value.m_value.object)
        {
            if (JSON_HEDLEY_UNLIKELY(!element.second.is_primitive()))
            {
                JSON_THROW(detail::type_error::create(315, "values in object must be primitive", element.second));
            }

            // assign value to reference pointed to by JSON pointer; Note that if
            // the JSON pointer is "" (i.e., points to the whole value), function
            // get_and_create returns a reference to result itself. An assignment
            // will then create a primitive value.
            json_pointer(element.first).get_and_create(result) = element.second;
        }

        return result;
    }

    /*!
    @brief compares two JSON pointers for equality

    @param[in] lhs  JSON pointer to compare
    @param[in] rhs  JSON pointer to compare
    @return whether @a lhs is equal to @a rhs

    @complexity Linear in the length of the JSON pointer

    @exceptionsafety No-throw guarantee: this function never throws exceptions.
    */
    friend bool operator==(json_pointer const& lhs,
                           json_pointer const& rhs) noexcept
    {
        return lhs.reference_tokens == rhs.reference_tokens;
    }

    /*!
    @brief compares two JSON pointers for inequality

    @param[in] lhs  JSON pointer to compare
    @param[in] rhs  JSON pointer to compare
    @return whether @a lhs is not equal @a rhs

    @complexity Linear in the length of the JSON pointer

    @exceptionsafety No-throw guarantee: this function never throws exceptions.
    */
    friend bool operator!=(json_pointer const& lhs,
                           json_pointer const& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    /// the reference tokens
    std::vector<std::string> reference_tokens;
};
}  // namespace nlohmann

// #include <nlohmann/detail/json_ref.hpp>


#include <initializer_list>
#include <utility>

// #include <nlohmann/detail/meta/type_traits.hpp>


namespace nlohmann
{
namespace detail
{
template<typename BasicJsonType>
class json_ref
{
  public:
    using value_type = BasicJsonType;

    json_ref(value_type&& value)
        : owned_value(std::move(value))
    {}

    json_ref(const value_type& value)
        : value_ref(&value)
    {}

    json_ref(std::initializer_list<json_ref> init)
        : owned_value(init)
    {}

    template <
        class... Args,
        enable_if_t<std::is_constructible<value_type, Args...>::value, int> = 0 >
    json_ref(Args && ... args)
        : owned_value(std::forward<Args>(args)...)
    {}

    // class should be movable only
    json_ref(json_ref&&) noexcept = default;
    json_ref(const json_ref&) = delete;
    json_ref& operator=(const json_ref&) = delete;
    json_ref& operator=(json_ref&&) = delete;
    ~json_ref() = default;

    value_type moved_or_copied() const
    {
        if (value_ref == nullptr)
        {
            return std::move(owned_value);
        }
        return *value_ref;
    }

    value_type const& operator*() const
    {
        return value_ref ? *value_ref : owned_value;
    }

    value_type const* operator->() const
    {
        return &** this;
    }

  private:
    mutable value_type owned_value = nullptr;
    value_type const* value_ref = nullptr;
};
}  // namespace detail
}  // namespace nlohmann

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/string_escape.hpp>

// #include <nlohmann/detail/meta/cpp_future.hpp>

// #include <nlohmann/detail/meta/type_traits.hpp>

// #include <nlohmann/detail/output/binary_writer.hpp>


#include <algorithm> // reverse
#include <array> // array
#include <cmath> // isnan, isinf
#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t
#include <cstring> // memcpy
#include <limits> // numeric_limits
#include <string> // string
#include <utility> // move

// #include <nlohmann/detail/input/binary_reader.hpp>

// #include <nlohmann/detail/macro_scope.hpp>

// #include <nlohmann/detail/output/output_adapters.hpp>


#include <algorithm> // copy
#include <cstddef> // size_t
#include <iterator> // back_inserter
#include <memory> // shared_ptr, make_shared
#include <string> // basic_string
#include <vector> // vector

#ifndef JSON_NO_IO
    #include <ios>      // streamsize
    #include <ostream>  // basic_ostream
#endif  // JSON_NO_IO

// #include <nlohmann/detail/macro_scope.hpp>


namespace nlohmann
{
namespace detail
{
/// abstract output adapter interface
template<typename CharType> struct output_adapter_protocol
{
    virtual void write_character(CharType c) = 0;
    virtual void write_characters(const CharType* s, std::size_t length) = 0;
    virtual ~output_adapter_protocol() = default;

    output_adapter_protocol() = default;
    output_adapter_protocol(const output_adapter_protocol&) = default;
    output_adapter_protocol(output_adapter_protocol&&) noexcept = default;
    output_adapter_protocol& operator=(const output_adapter_protocol&) = default;
    output_adapter_protocol& operator=(output_adapter_protocol&&) noexcept = default;
};

/// a type to simplify interfaces
template<typename CharType>
using output_adapter_t = std::shared_ptr<output_adapter_protocol<CharType>>;

/// output adapter for byte vectors
template<typename CharType>
class output_vector_adapter : public output_adapter_protocol<CharType>
{
  public:
    explicit output_vector_adapter(std::vector<CharType>& vec) noexcept
        : v(vec)
    {}

    void write_character(CharType c) override
    {
        v.push_back(c);
    }

    JSON_HEDLEY_NON_NULL(2)
    void write_characters(const CharType* s, std::size_t length) override
    {
        std::copy(s, s + length, std::back_inserter(v));
    }

  private:
    std::vector<CharType>& v;
};

#ifndef JSON_NO_IO
/// output adapter for output streams
template<typename CharType>
class output_stream_adapter : public output_adapter_protocol<CharType>
{
  public:
    explicit output_stream_adapter(std::basic_ostream<CharType>& s) noexcept
        : stream(s)
    {}

    void write_character(CharType c) override
    {
        stream.put(c);
    }

    JSON_HEDLEY_NON_NULL(2)
    void write_characters(const CharType* s, std::size_t length) override
    {
        stream.write(s, static_cast<std::streamsize>(length));
    }

  private:
    std::basic_ostream<CharType>& stream;
};
#endif  // JSON_NO_IO

/// output adapter for basic_string
template<typename CharType, typename StringType = std::basic_string<CharType>>
class output_string_adapter : public output_adapter_protocol<CharType>
{
  public:
    explicit output_string_adapter(StringType& s) noexcept
        : str(s)
    {}

    void write_character(CharType c) override
    {
        str.push_back(c);
    }

    JSON_HEDLEY_NON_NULL(2)
    void write_characters(const CharType* s, std::size_t length) override
    {
        str.append(s, length);
    }

  private:
    StringType& str;
};

template<typename CharType, typename StringType = std::basic_string<CharType>>
class output_adapter
{
  public:
    output_adapter(std::vector<CharType>& vec)
        : oa(std::make_shared<output_vector_adapter<CharType>>(vec)) {}

#ifndef JSON_NO_IO
    output_adapter(std::basic_ostream<CharType>& s)
        : oa(std::make_shared<output_stream_adapter<CharType>>(s)) {}
#endif  // JSON_NO_IO

    output_adapter(StringType& s)
        : oa(std::make_shared<output_string_adapter<CharType, StringType>>(s)) {}

    operator output_adapter_t<CharType>()
    {
        return oa;
    }

  private:
    output_adapter_t<CharType> oa = nullptr;
};
}  // namespace detail
}  // namespace nlohmann


namespace nlohmann
{
namespace detail
{
///////////////////
// binary writer //
///////////////////

/*!
@brief serialization to CBOR and MessagePack values
*/
template<typename BasicJsonType, typename CharType>
class binary_writer
{
    using string_t = typename BasicJsonType::string_t;
    using binary_t = typename BasicJsonType::binary_t;
    using number_float_t = typename BasicJsonType::number_float_t;

  public:
    /*!
    @brief create a binary writer

    @param[in] adapter  output adapter to write to
    */
    explicit binary_writer(output_adapter_t<CharType> adapter) : oa(std::move(adapter))
    {
        JSON_ASSERT(oa);
    }

    /*!
    @param[in] j  JSON value to serialize
    @pre       j.type() == value_t::object
    */
    void write_bson(const BasicJsonType& j)
    {
        switch (j.type())
        {
            case value_t::object:
            {
                write_bson_object(*j.m_value.object);
                break;
            }

            default:
            {
                JSON_THROW(type_error::create(317, "to serialize to BSON, top-level type must be object, but is " + std::string(j.type_name()), j));;
            }
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    */
    void write_cbor(const BasicJsonType& j)
    {
        switch (j.type())
        {
            case value_t::null:
            {
                oa->write_character(to_char_type(0xF6));
                break;
            }

            case value_t::boolean:
            {
                oa->write_character(j.m_value.boolean
                                    ? to_char_type(0xF5)
                                    : to_char_type(0xF4));
                break;
            }

            case value_t::number_integer:
            {
                if (j.m_value.number_integer >= 0)
                {
                    // CBOR does not differentiate between positive signed
                    // integers and unsigned integers. Therefore, we used the
                    // code from the value_t::number_unsigned case here.
                    if (j.m_value.number_integer <= 0x17)
                    {
                        write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer <= (std::numeric_limits<std::uint8_t>::max)())
                    {
                        oa->write_character(to_char_type(0x18));
                        write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer <= (std::numeric_limits<std::uint16_t>::max)())
                    {
                        oa->write_character(to_char_type(0x19));
                        write_number(static_cast<std::uint16_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer <= (std::numeric_limits<std::uint32_t>::max)())
                    {
                        oa->write_character(to_char_type(0x1A));
                        write_number(static_cast<std::uint32_t>(j.m_value.number_integer));
                    }
                    else
                    {
                        oa->write_character(to_char_type(0x1B));
                        write_number(static_cast<std::uint64_t>(j.m_value.number_integer));
                    }
                }
                else
                {
                    // The conversions below encode the sign in the first
                    // byte, and the value is converted to a positive number.
                    const auto positive_number = -1 - j.m_value.number_integer;
                    if (j.m_value.number_integer >= -24)
                    {
                        write_number(static_cast<std::uint8_t>(0x20 + positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<std::uint8_t>::max)())
                    {
                        oa->write_character(to_char_type(0x38));
                        write_number(static_cast<std::uint8_t>(positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<std::uint16_t>::max)())
                    {
                        oa->write_character(to_char_type(0x39));
                        write_number(static_cast<std::uint16_t>(positive_number));
                    }
                    else if (positive_number <= (std::numeric_limits<std::uint32_t>::max)())
                    {
                        oa->write_character(to_char_type(0x3A));
                        write_number(static_cast<std::uint32_t>(positive_number));
                    }
                    else
                    {
                        oa->write_character(to_char_type(0x3B));
                        write_number(static_cast<std::uint64_t>(positive_number));
                    }
                }
                break;
            }

            case value_t::number_unsigned:
            {
                if (j.m_value.number_unsigned <= 0x17)
                {
                    write_number(static_cast<std::uint8_t>(j.m_value.number_unsigned));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x18));
                    write_number(static_cast<std::uint8_t>(j.m_value.number_unsigned));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x19));
                    write_number(static_cast<std::uint16_t>(j.m_value.number_unsigned));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x1A));
                    write_number(static_cast<std::uint32_t>(j.m_value.number_unsigned));
                }
                else
                {
                    oa->write_character(to_char_type(0x1B));
                    write_number(static_cast<std::uint64_t>(j.m_value.number_unsigned));
                }
                break;
            }

            case value_t::number_float:
            {
                if (std::isnan(j.m_value.number_float))
                {
                    // NaN is 0xf97e00 in CBOR
                    oa->write_character(to_char_type(0xF9));
                    oa->write_character(to_char_type(0x7E));
                    oa->write_character(to_char_type(0x00));
                }
                else if (std::isinf(j.m_value.number_float))
                {
                    // Infinity is 0xf97c00, -Infinity is 0xf9fc00
                    oa->write_character(to_char_type(0xf9));
                    oa->write_character(j.m_value.number_float > 0 ? to_char_type(0x7C) : to_char_type(0xFC));
                    oa->write_character(to_char_type(0x00));
                }
                else
                {
                    write_compact_float(j.m_value.number_float, detail::input_format_t::cbor);
                }
                break;
            }

            case value_t::string:
            {
                // step 1: write control byte and the string length
                const auto N = j.m_value.string->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<std::uint8_t>(0x60 + N));
                }
                else if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x78));
                    write_number(static_cast<std::uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x79));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x7A));
                    write_number(static_cast<std::uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<std::uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0x7B));
                    write_number(static_cast<std::uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write the string
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.m_value.string->c_str()),
                    j.m_value.string->size());
                break;
            }

            case value_t::array:
            {
                // step 1: write control byte and the array size
                const auto N = j.m_value.array->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<std::uint8_t>(0x80 + N));
                }
                else if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x98));
                    write_number(static_cast<std::uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x99));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x9A));
                    write_number(static_cast<std::uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<std::uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0x9B));
                    write_number(static_cast<std::uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write each element
                for (const auto& el : *j.m_value.array)
                {
                    write_cbor(el);
                }
                break;
            }

            case value_t::binary:
            {
                if (j.m_value.binary->has_subtype())
                {
                    write_number(static_cast<std::uint8_t>(0xd8));
                    write_number(j.m_value.binary->subtype());
                }

                // step 1: write control byte and the binary array size
                const auto N = j.m_value.binary->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<std::uint8_t>(0x40 + N));
                }
                else if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0x58));
                    write_number(static_cast<std::uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0x59));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0x5A));
                    write_number(static_cast<std::uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<std::uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0x5B));
                    write_number(static_cast<std::uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write each element
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.m_value.binary->data()),
                    N);

                break;
            }

            case value_t::object:
            {
                // step 1: write control byte and the object size
                const auto N = j.m_value.object->size();
                if (N <= 0x17)
                {
                    write_number(static_cast<std::uint8_t>(0xA0 + N));
                }
                else if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    oa->write_character(to_char_type(0xB8));
                    write_number(static_cast<std::uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    oa->write_character(to_char_type(0xB9));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    oa->write_character(to_char_type(0xBA));
                    write_number(static_cast<std::uint32_t>(N));
                }
                // LCOV_EXCL_START
                else if (N <= (std::numeric_limits<std::uint64_t>::max)())
                {
                    oa->write_character(to_char_type(0xBB));
                    write_number(static_cast<std::uint64_t>(N));
                }
                // LCOV_EXCL_STOP

                // step 2: write each element
                for (const auto& el : *j.m_value.object)
                {
                    write_cbor(el.first);
                    write_cbor(el.second);
                }
                break;
            }

            default:
                break;
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    */
    void write_msgpack(const BasicJsonType& j)
    {
        switch (j.type())
        {
            case value_t::null: // nil
            {
                oa->write_character(to_char_type(0xC0));
                break;
            }

            case value_t::boolean: // true and false
            {
                oa->write_character(j.m_value.boolean
                                    ? to_char_type(0xC3)
                                    : to_char_type(0xC2));
                break;
            }

            case value_t::number_integer:
            {
                if (j.m_value.number_integer >= 0)
                {
                    // MessagePack does not differentiate between positive
                    // signed integers and unsigned integers. Therefore, we used
                    // the code from the value_t::number_unsigned case here.
                    if (j.m_value.number_unsigned < 128)
                    {
                        // positive fixnum
                        write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint8_t>::max)())
                    {
                        // uint 8
                        oa->write_character(to_char_type(0xCC));
                        write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint16_t>::max)())
                    {
                        // uint 16
                        oa->write_character(to_char_type(0xCD));
                        write_number(static_cast<std::uint16_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint32_t>::max)())
                    {
                        // uint 32
                        oa->write_character(to_char_type(0xCE));
                        write_number(static_cast<std::uint32_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint64_t>::max)())
                    {
                        // uint 64
                        oa->write_character(to_char_type(0xCF));
                        write_number(static_cast<std::uint64_t>(j.m_value.number_integer));
                    }
                }
                else
                {
                    if (j.m_value.number_integer >= -32)
                    {
                        // negative fixnum
                        write_number(static_cast<std::int8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer >= (std::numeric_limits<std::int8_t>::min)() &&
                             j.m_value.number_integer <= (std::numeric_limits<std::int8_t>::max)())
                    {
                        // int 8
                        oa->write_character(to_char_type(0xD0));
                        write_number(static_cast<std::int8_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer >= (std::numeric_limits<std::int16_t>::min)() &&
                             j.m_value.number_integer <= (std::numeric_limits<std::int16_t>::max)())
                    {
                        // int 16
                        oa->write_character(to_char_type(0xD1));
                        write_number(static_cast<std::int16_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer >= (std::numeric_limits<std::int32_t>::min)() &&
                             j.m_value.number_integer <= (std::numeric_limits<std::int32_t>::max)())
                    {
                        // int 32
                        oa->write_character(to_char_type(0xD2));
                        write_number(static_cast<std::int32_t>(j.m_value.number_integer));
                    }
                    else if (j.m_value.number_integer >= (std::numeric_limits<std::int64_t>::min)() &&
                             j.m_value.number_integer <= (std::numeric_limits<std::int64_t>::max)())
                    {
                        // int 64
                        oa->write_character(to_char_type(0xD3));
                        write_number(static_cast<std::int64_t>(j.m_value.number_integer));
                    }
                }
                break;
            }

            case value_t::number_unsigned:
            {
                if (j.m_value.number_unsigned < 128)
                {
                    // positive fixnum
                    write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    // uint 8
                    oa->write_character(to_char_type(0xCC));
                    write_number(static_cast<std::uint8_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    // uint 16
                    oa->write_character(to_char_type(0xCD));
                    write_number(static_cast<std::uint16_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    // uint 32
                    oa->write_character(to_char_type(0xCE));
                    write_number(static_cast<std::uint32_t>(j.m_value.number_integer));
                }
                else if (j.m_value.number_unsigned <= (std::numeric_limits<std::uint64_t>::max)())
                {
                    // uint 64
                    oa->write_character(to_char_type(0xCF));
                    write_number(static_cast<std::uint64_t>(j.m_value.number_integer));
                }
                break;
            }

            case value_t::number_float:
            {
                write_compact_float(j.m_value.number_float, detail::input_format_t::msgpack);
                break;
            }

            case value_t::string:
            {
                // step 1: write control byte and the string length
                const auto N = j.m_value.string->size();
                if (N <= 31)
                {
                    // fixstr
                    write_number(static_cast<std::uint8_t>(0xA0 | N));
                }
                else if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    // str 8
                    oa->write_character(to_char_type(0xD9));
                    write_number(static_cast<std::uint8_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    // str 16
                    oa->write_character(to_char_type(0xDA));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    // str 32
                    oa->write_character(to_char_type(0xDB));
                    write_number(static_cast<std::uint32_t>(N));
                }

                // step 2: write the string
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.m_value.string->c_str()),
                    j.m_value.string->size());
                break;
            }

            case value_t::array:
            {
                // step 1: write control byte and the array size
                const auto N = j.m_value.array->size();
                if (N <= 15)
                {
                    // fixarray
                    write_number(static_cast<std::uint8_t>(0x90 | N));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    // array 16
                    oa->write_character(to_char_type(0xDC));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    // array 32
                    oa->write_character(to_char_type(0xDD));
                    write_number(static_cast<std::uint32_t>(N));
                }

                // step 2: write each element
                for (const auto& el : *j.m_value.array)
                {
                    write_msgpack(el);
                }
                break;
            }

            case value_t::binary:
            {
                // step 0: determine if the binary type has a set subtype to
                // determine whether or not to use the ext or fixext types
                const bool use_ext = j.m_value.binary->has_subtype();

                // step 1: write control byte and the byte string length
                const auto N = j.m_value.binary->size();
                if (N <= (std::numeric_limits<std::uint8_t>::max)())
                {
                    std::uint8_t output_type{};
                    bool fixed = true;
                    if (use_ext)
                    {
                        switch (N)
                        {
                            case 1:
                                output_type = 0xD4; // fixext 1
                                break;
                            case 2:
                                output_type = 0xD5; // fixext 2
                                break;
                            case 4:
                                output_type = 0xD6; // fixext 4
                                break;
                            case 8:
                                output_type = 0xD7; // fixext 8
                                break;
                            case 16:
                                output_type = 0xD8; // fixext 16
                                break;
                            default:
                                output_type = 0xC7; // ext 8
                                fixed = false;
                                break;
                        }

                    }
                    else
                    {
                        output_type = 0xC4; // bin 8
                        fixed = false;
                    }

                    oa->write_character(to_char_type(output_type));
                    if (!fixed)
                    {
                        write_number(static_cast<std::uint8_t>(N));
                    }
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    std::uint8_t output_type = use_ext
                                               ? 0xC8 // ext 16
                                               : 0xC5; // bin 16

                    oa->write_character(to_char_type(output_type));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    std::uint8_t output_type = use_ext
                                               ? 0xC9 // ext 32
                                               : 0xC6; // bin 32

                    oa->write_character(to_char_type(output_type));
                    write_number(static_cast<std::uint32_t>(N));
                }

                // step 1.5: if this is an ext type, write the subtype
                if (use_ext)
                {
                    write_number(static_cast<std::int8_t>(j.m_value.binary->subtype()));
                }

                // step 2: write the byte string
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.m_value.binary->data()),
                    N);

                break;
            }

            case value_t::object:
            {
                // step 1: write control byte and the object size
                const auto N = j.m_value.object->size();
                if (N <= 15)
                {
                    // fixmap
                    write_number(static_cast<std::uint8_t>(0x80 | (N & 0xF)));
                }
                else if (N <= (std::numeric_limits<std::uint16_t>::max)())
                {
                    // map 16
                    oa->write_character(to_char_type(0xDE));
                    write_number(static_cast<std::uint16_t>(N));
                }
                else if (N <= (std::numeric_limits<std::uint32_t>::max)())
                {
                    // map 32
                    oa->write_character(to_char_type(0xDF));
                    write_number(static_cast<std::uint32_t>(N));
                }

                // step 2: write each element
                for (const auto& el : *j.m_value.object)
                {
                    write_msgpack(el.first);
                    write_msgpack(el.second);
                }
                break;
            }

            default:
                break;
        }
    }

    /*!
    @param[in] j  JSON value to serialize
    @param[in] use_count   whether to use '#' prefixes (optimized format)
    @param[in] use_type    whether to use '$' prefixes (optimized format)
    @param[in] add_prefix  whether prefixes need to be used for this value
    */
    void write_ubjson(const BasicJsonType& j, const bool use_count,
                      const bool use_type, const bool add_prefix = true)
    {
        switch (j.type())
        {
            case value_t::null:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('Z'));
                }
                break;
            }

            case value_t::boolean:
            {
                if (add_prefix)
                {
                    oa->write_character(j.m_value.boolean
                                        ? to_char_type('T')
                                        : to_char_type('F'));
                }
                break;
            }

            case value_t::number_integer:
            {
                write_number_with_ubjson_prefix(j.m_value.number_integer, add_prefix);
                break;
            }

            case value_t::number_unsigned:
            {
                write_number_with_ubjson_prefix(j.m_value.number_unsigned, add_prefix);
                break;
            }

            case value_t::number_float:
            {
                write_number_with_ubjson_prefix(j.m_value.number_float, add_prefix);
                break;
            }

            case value_t::string:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('S'));
                }
                write_number_with_ubjson_prefix(j.m_value.string->size(), true);
                oa->write_characters(
                    reinterpret_cast<const CharType*>(j.m_value.string->c_str()),
                    j.m_value.string->size());
                break;
            }

            case value_t::array:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('['));
                }

                bool prefix_required = true;
                if (use_type && !j.m_value.array->empty())
                {
                    JSON_ASSERT(use_count);
                    const CharType first_prefix = ubjson_prefix(j.front());
                    const bool same_prefix = std::all_of(j.begin() + 1, j.end(),
                                                         [this, first_prefix](const BasicJsonType & v)
                    {
                        return ubjson_prefix(v) == first_prefix;
                    });

                    if (same_prefix)
                    {
                        prefix_required = false;
                        oa->write_character(to_char_type('$'));
                        oa->write_character(first_prefix);
                    }
                }

                if (use_count)
                {
                    oa->write_character(to_char_type('#'));
                    write_number_with_ubjson_prefix(j.m_value.array->size(), true);
                }

                for (const auto& el : *j.m_value.array)
                {
                    write_ubjson(el, use_count, use_type, prefix_required);
                }

                if (!use_count)
                {
                    oa->write_character(to_char_type(']'));
                }

                break;
            }

            case value_t::binary:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('['));
                }

                if (use_type && !j.m_value.binary->empty())
                {
                    JSON_ASSERT(use_count);
                    oa->write_character(to_char_type('$'));
                    oa->write_character('U');
                }

                if (use_count)
                {
                    oa->write_character(to_char_type('#'));
                    write_number_with_ubjson_prefix(j.m_value.binary->size(), true);
                }

                if (use_type)
                {
                    oa->write_characters(
                        reinterpret_cast<const CharType*>(j.m_value.binary->data()),
                        j.m_value.binary->size());
                }
                else
                {
                    for (size_t i = 0; i < j.m_value.binary->size(); ++i)
                    {
                        oa->write_character(to_char_type('U'));
                        oa->write_character(j.m_value.binary->data()[i]);
                    }
                }

                if (!use_count)
                {
                    oa->write_character(to_char_type(']'));
                }

                break;
            }

            case value_t::object:
            {
                if (add_prefix)
                {
                    oa->write_character(to_char_type('{'));
                }

                bool prefix_required = true;
                if (use_type && !j.m_value.object->empty())
                {
                    JSON_ASSERT(use_count);
                    const CharType first_prefix = ubjson_prefix(j.front());
                    const bool same_prefix = std::all_of(j.begin(), j.end(),
                                                         [this, first_prefix](const BasicJsonType & v)
                    {
                        return ubjson_prefix(v) == first_prefix;
                    });

                    if (same_prefix)
                    {
                        prefix_required = false;
                        oa->write_character(to_char_type('$'));
                        oa->write_character(first_prefix);
                    }
                }

                if (use_count)
                {
                    oa->write_character(to_char_type('#'));
                    write_number_with_ubjson_prefix(j.m_value.object->size(), true);
                }

                for (const auto& el : *j.m_value.object)
                {
                    write_number_with_ubjson_prefix(el.first.size(), true);
                    oa->write_characters(
                        reinterpret_cast<const CharType*>(el.first.c_str()),
                        el.first.size());
                    write_ubjson(el.second, use_count, use_type, prefix_required);
                }

                if (!use_count)
                {
                    oa->write_character(to_char_type('}'));
                }

                break;
            }

            default:
                break;
        }
    }

  private:
    //////////
    // BSON //
    //////////

    /*!
    @return The size of a BSON document entry header, including the id marker
            and the entry name size (and its null-terminator).
    */
    static std::size_t calc_bson_entry_header_size(const string_t& name, const BasicJsonType& j)
    {
        const auto it = name.find(static_cast<typename string_t::value_type>(0));
        if (JSON_HEDLEY_UNLIKELY(it != BasicJsonType::string_t::npos))
        {
            JSON_THROW(out_of_range::create(409, "BSON key cannot contain code point U+0000 (at byte " + std::to_string(it) + ")", j));
        }

        return /*id*/ 1ul + name.size() + /*zero-terminator*/1u;
    }

    /*!
    @brief Writes the given @a element_type and @a name to the output adapter
    */
    void write_bson_entry_header(const string_t& name,
                                 const std::uint8_t element_type)
    {
        oa->write_character(to_char_type(element_type)); // boolean
        oa->write_characters(
            reinterpret_cast<const CharType*>(name.c_str()),
            name.size() + 1u);
    }

    /*!
    @brief Writes a BSON element with key @a name and boolean value @a value
    */
    void write_bson_boolean(const string_t& name,
                            const bool value)
    {
        write_bson_entry_header(name, 0x08);
        oa->