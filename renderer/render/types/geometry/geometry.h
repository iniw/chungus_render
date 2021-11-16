#pragma once

#include "base.h"
#include "rectangle.h"
#include "line.h"

namespace render::geometry {

	template< typename t >
	concept obj = std::derived_from< t, base > && !std::is_same_v< t, base >;

}