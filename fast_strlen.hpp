template <class char_type>
constexpr inline size_t fast_strlen(const char_type* _begin)
{
	typedef unsigned long long size_t;

	constexpr size_t mask_high = static_cast<size_t>(0x8080808080808080U); // Works for X64 and X86
	constexpr size_t mask_low = static_cast<size_t>(0x0101010101010101U);
	const size_t* aligned_end = reinterpret_cast<const size_t*>(_begin);
	const char_type* end = _begin;

	// Check 8 (or 4 with x86) bytes at once without simd instructions
	for (size_t data;;) {
		data = *aligned_end++;
		if ((data - mask_low) & (~data) & mask_high) {
			break;
		}
		end = reinterpret_cast<const char_type*>(aligned_end);
	}

	// Count rest one by one
	for (; *end != char_type(); ++end);

	return static_cast<size_t>(end - _begin);
}
