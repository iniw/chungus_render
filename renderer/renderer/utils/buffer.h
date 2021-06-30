#pragma once

namespace renderer {
	// can add more to this later
	template<typename t>
	struct s_buffer {
	public:
		size_t size;
		size_t capacity;
		t* buffer;
	public:
		constexpr s_buffer() : size(0u), capacity(0u), buffer(nullptr) {}
	};
}