#pragma once


namespace
{
	constexpr int const_atox(char c)
	{
		return c - '0';
	}
}

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE __attribute__((always_inline))
#endif

template<typename _string_type, size_t _length>
class _Basic__sexo_
{
	using value_type = typename _string_type::value_type;
	static constexpr auto _length_minus_one = _length - 1;

public:
	constexpr ALWAYS_INLINE _Basic__sexo_(value_type const (&str)[_length])
		: _Basic__sexo_(str, std::make_index_sequence<_length_minus_one>())
	{

	}

	inline auto c_str() const
	{
		decrypt();

		return data;
	}

	inline auto str() const
	{
		decrypt();

		return _string_type(data, data + _length_minus_one);
	}

	inline operator _string_type() const
	{
		return str();
	}

private:
	template<size_t... indices>
	constexpr ALWAYS_INLINE _Basic__sexo_(value_type const (&str)[_length], std::index_sequence<indices...>)
		: data{ crypt(str[indices], indices)..., '\0' },
		encrypted(true)
	{

	}

	static constexpr auto sexo_KEY = static_cast<value_type>(
		const_atox(__TIME__[7]) +
		const_atox(__TIME__[6]) * 10 +
		const_atox(__TIME__[4]) * 60 +
		const_atox(__TIME__[3]) * 600 +
		const_atox(__TIME__[1]) * 3600 +
		const_atox(__TIME__[0]) * 36000
		);

	static ALWAYS_INLINE constexpr auto crypt(value_type c, size_t i)
	{
		return static_cast<value_type>(c ^ (sexo_KEY + i));
	}

	inline void decrypt() const
	{
		if (encrypted)
		{
			for (size_t t = 0; t < _length_minus_one; t++)
			{
				data[t] = crypt(data[t], t);
			}
			encrypted = false;
		}
	}

	mutable value_type data[_length];
	mutable bool encrypted;
};
//---------------------------------------------------------------------------
template<size_t _length>
using _sexo_A = _Basic__sexo_<std::string, _length>;
template<size_t _length>
using _sexo_W = _Basic__sexo_<std::wstring, _length>;
template<size_t _length>
using _sexo_U16 = _Basic__sexo_<std::u16string, _length>;
template<size_t _length>
using _sexo_U32 = _Basic__sexo_<std::u32string, _length>;
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator==(const _Basic__sexo_<_string_type, _length>& lhs, const _Basic__sexo_<_string_type, _length2>& rhs)
{
	static_assert(_length == _length2, "_sexo_== different length");

	return _length == _length2 && lhs.str() == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator==(const _string_type& lhs, const _Basic__sexo_<_string_type, _length>& rhs)
{
	return lhs.size() == _length && lhs == rhs.str();
}
//---------------------------------------------------------------------------
template<typename _stream_type, typename _string_type, size_t _length>
inline auto& operator<<(_stream_type& lhs, const _Basic__sexo_<_string_type, _length>& rhs)
{
	lhs << rhs.c_str();

	return lhs;
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length, size_t _length2>
inline auto operator+(const _Basic__sexo_<_string_type, _length>& lhs, const _Basic__sexo_<_string_type, _length2>& rhs)
{
	return lhs.str() + rhs.str();
}
//---------------------------------------------------------------------------
template<typename _string_type, size_t _length>
inline auto operator+(const _string_type& lhs, const _Basic__sexo_<_string_type, _length>& rhs)
{
	return lhs + rhs.str();
}
//---------------------------------------------------------------------------
template<size_t _length>
constexpr ALWAYS_INLINE auto _sexo_(char const (&str)[_length])
{
	return _sexo_A<_length>(str);
}
//---------------------------------------------------------------------------
template<size_t _length>
constexpr ALWAYS_INLINE auto _sexo_(wchar_t const (&str)[_length])
{
	return _sexo_W<_length>(str);
}
//---------------------------------------------------------------------------
template<size_t _length>
constexpr ALWAYS_INLINE auto _sexo_(char16_t const (&str)[_length])
{
	return _sexo_U16<_length>(str);
}
//---------------------------------------------------------------------------
template<size_t _length>
constexpr ALWAYS_INLINE auto _sexo_(char32_t const (&str)[_length])
{
	return _sexo_U32<_length>(str);
}
//---------------------------------------------------------------------------