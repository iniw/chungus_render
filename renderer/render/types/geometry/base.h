#pragma once
#include "../color.h"
#include <vector>
#include <memory>
#include "../../drawlist/drawlist.h"
namespace render::geometry {

	struct base {
	protected:

		color m_color;

	public:

		static inline drawlist* m_list = nullptr;

		virtual void draw( ) = 0;

	};

	struct safe_base {
	protected:

		// "Ah! Yes!" Yelled the gentlman, as he rushed into the computer;
		// "A queue of queues!" - his excitement was immeasurable.
		std::vector< std::unique_ptr< base > > m_drawlist;

	public:

		virtual void draw( ) = 0;

	};

}