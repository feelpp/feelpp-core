// SPDX-FileCopyrightText: 2025-2026 Université de Strasbourg
// SPDX-FileContributor: Vincent Chabannes
// SPDX-License-Identifier: LGPL-3.0-or-later

//!

#pragma once

#if defined( FEELPP_HAS_LIBASSERT )

#include <libassert/assert.hpp>

#else

#include <cassert>
#include <tuple>

namespace Feel::Core
{
namespace detail
{
template <typename... Ts>
void assertImpl( Ts && ... ts ) { assert( std::get<0>( std::forward_as_tuple( std::forward<Ts>(ts)... ) ) );  }
}
}

#define ASSERT(...) Feel::Core::detail::assertImpl( __VA_ARGS__ );

#endif
