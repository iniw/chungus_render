#pragma once
#include <cstring>

namespace renderer {
	template<typename t>
	struct s_buffer {
	public:
		size_t size;
		size_t capacity;
		t* buffer;
	public:
		constexpr s_buffer() : size(0u), capacity(0u), buffer(nullptr) {}
		// not necessary but just wanna be sure we don't leak
		~s_buffer() {
			std::free(buffer);
		}
	public:
		inline void clear() {
			std::free(buffer);
			capacity = 0;
			size = 0;
		}

		inline void copy_to(void* mem, size_t new_capacity) {
			// copy old to new
			std::memcpy(mem, buffer, capacity);
			// free old
			std::free(buffer);
			// swap pointers and set size
			buffer = static_cast<t*>(mem);
			capacity = new_capacity;
		}
	};
}