//!

#pragma once

#include <napp/na.hpp>

namespace Feel
{

namespace na
{
using binary = NA::named_argument_t<struct binary_tag>;
using filename = NA::named_argument_t<struct filename_tag>;
using format = NA::named_argument_t<struct format_tag>;
using istream = NA::named_argument_t<struct istream_tag>;
using name = NA::named_argument_t<struct name_tag>;
using ostream = NA::named_argument_t<struct ostream_tag>;
using prefix = NA::named_argument_t<struct prefix_tag>;
using vm = NA::named_argument_t<struct vm_tag>;
}

inline constexpr auto& _binary = NA::identifier<na::binary>;
inline constexpr auto& _filename = NA::identifier<na::filename>;
inline constexpr auto& _format = NA::identifier<na::format>;
inline constexpr auto& _istream = NA::identifier<na::istream>;
inline constexpr auto& _name = NA::identifier<na::name>;
inline constexpr auto& _ostream = NA::identifier<na::ostream>;
inline constexpr auto& _prefix = NA::identifier<na::prefix>;
inline constexpr auto& _vm = NA::identifier<na::vm>;

}
