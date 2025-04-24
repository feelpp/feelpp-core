//!

#pragma once

#if defined( FEELPP_HAS_LIBASSERT )

#include <libassert/assert.hpp>

#else

#include <cassert>

namespace Feel
{
namespace detail
{
template <typename... Ts>
void assertImpl( Ts && ... ts ) { assert( std::forward_as_tuple( std::forward<Ts>(ts)... ) );  }
}
}

#define ASSERT(...) Feel::detail::assertImpl( __VA_ARGS__ );

#endif
