#pragma once
#include "JsonSerializer.h"
#include <sge_core/base/Error.h>

namespace sge {

struct JsonDeserializer : public NonCopyable
{
	using This = JsonDeserializer;
public:
	JsonDeserializer(Json& json_);

	void io(bool& v_);

	void io(u8& v_);
	void io(u16& v_);
	void io(u32& v_);
	void io(u64& v_);

	void io(i8& v_);
	void io(i16& v_);
	void io(i32& v_);
	void io(i64& v_);

	void io(f32& v_);
	void io(f64& v_);

	template<class V> void io(V& data_);
	template<class V> void named_io(const char* name_, V& data_);

	template<class SE, class T, class ENABLE> friend struct JsonIO;
protected:
	template<class V> void toValue(V& value_);
	template<class V> void toEnum(V& value_);
	template<class V> void toString(V& value_);
	StrView toStrView();

	void beginObject();
	void endObject();

	size_t beginArray();
	void endArray();
	template<class V> void toArrayElement(size_t index_, V& data_);

	template<class V> void toObjectMember(const char* name_, V& data_);

private:
	Json& _json;
	Vector_<Json*, 64> _stack;
};

#if 0
#pragma mark ========= JsonIO_Impl
#endif // 0
#if 1    // JsonIO_Impl

template<class T, size_t N>
struct JsonIO<JsonDeserializer, Vector_<T, N>>
{
	static void io(JsonDeserializer& se_, Vector_<T, N>& data_)
	{
		auto n = se_.beginArray();
		data_.clear(); // ensure elements dtor get called
		data_.resize(n);
		for (size_t i = 0; i < n; i++)
		{
			se_.toArrayElement(i, data_[i]);
		}
		se_.endArray();
	}
};

template<size_t N>
struct JsonIO<JsonDeserializer, String_<N>>
{
	static void io(JsonDeserializer& se_, String_<N>& data_)
	{
		data_ = se_.toStrView();
	}
};

#endif	 // JsonIO_Impl

#if 0
#pragma mark ========= JsonDeserializer_Impl
#endif // 0
#if 1    // JsonDeserializer_Impl

inline JsonDeserializer::JsonDeserializer(Json& json_) : _json(json_) { _stack.emplace_back(&_json); }

inline void JsonDeserializer::io(bool& v_)  { toValue(v_); }

inline void JsonDeserializer::io(u8& v_)  { toValue(v_); }
inline void JsonDeserializer::io(u16& v_) { toValue(v_); }
inline void JsonDeserializer::io(u32& v_) { toValue(v_); }
inline void JsonDeserializer::io(u64& v_) { toValue(v_); }

inline void JsonDeserializer::io(i8& v_)  { toValue(v_); }
inline void JsonDeserializer::io(i16& v_) { toValue(v_); }
inline void JsonDeserializer::io(i32& v_) { toValue(v_); }
inline void JsonDeserializer::io(i64& v_) { toValue(v_); }

inline void JsonDeserializer::io(f32& v_) { toValue(v_); }
inline void JsonDeserializer::io(f64& v_) { toValue(v_); }

template<class V> inline
void JsonDeserializer::io(V& data_) { JsonIO<This, V>::io(*this, data_); }

template<class V> inline
void JsonDeserializer::named_io(const char* name_, V& data_) { toObjectMember(name_, data_); }

template<class V> inline
void JsonDeserializer::toValue(V& value_)
{
	auto& cur = _stack.back();
	if (cur->is_null())
		throw SGE_ERROR("json do not contains value");
	value_ = *cur;
}

template<class V> inline
void JsonDeserializer::toEnum(V& value_)
{
	auto s = toStrView();
	if (!enumTryParse(value_, s))
	{
		throw SGE_ERROR("error parse enum {}", s);
	}
}

template<class V> inline
void JsonDeserializer::toString(V& value_)
{
	value_ = toStrView();
}

inline StrView JsonDeserializer::toStrView()
{
	auto& cur = _stack.back();
	if (!cur->is_string())
		throw SGE_ERROR("string expected");

	auto* str = cur->get_ptr<Json::string_t*>();
	return StrView(str->data(), str->size());
}


inline void JsonDeserializer::beginObject()
{
	auto& cur = _stack.back();
	if (!cur->is_object())
	{
		throw SGE_ERROR("json is not object");
	}
}

inline void JsonDeserializer::endObject()
{
	auto& cur = _stack.back();
	if (!cur->is_object()) 
	{
		throw SGE_ERROR("endObject");
	}
}

inline size_t JsonDeserializer::beginArray()
{
	auto& cur = _stack.back();
	if (!cur->is_array())
		throw SGE_ERROR("json is not array");
	
	auto* arr = cur->get_ptr<Json::array_t*>();
	return arr->size();
}

inline void JsonDeserializer::endArray()
{
	auto& cur = _stack.back();
	if (!cur->is_array())
		throw SGE_ERROR("json is not array");
}

template<class V> inline
void JsonDeserializer::toArrayElement(size_t index_, V& data_)
{
	auto& cur = _stack.back();
	if (!cur->is_array())
		throw SGE_ERROR("not inside array");

	auto* arr = cur->get_ptr<Json::array_t*>();
	if (index_ >= arr->size())
		throw SGE_ERROR("array index out of array");

	auto& elementValue = arr->at(index_);
	_stack.emplace_back(&elementValue);
	io(data_);
	_stack.pop_back();
}

template<class V> inline
void JsonDeserializer::toObjectMember(const char* name_, V& data_)
{
	auto& obj = _stack.back();
	if (!obj->is_object())
		throw SGE_ERROR("not inside object");

	auto& memberValue = obj->operator[](name_);
	_stack.emplace_back(&memberValue);
	io(data_);
	_stack.pop_back();
}

#endif    // JsonDeserializer_Impl

}