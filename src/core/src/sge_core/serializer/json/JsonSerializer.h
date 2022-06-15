#pragma once

namespace sge {

template<class SE, class T, class ENABLE>
struct JsonIO;

struct JsonSerializer : public NonCopyable
{
	using This = JsonSerializer;
public:
	JsonSerializer(Json& json_);

	void io(u8& v_) ;
	void io(u16& v_);
	void io(u32& v_);
	void io(u64& v_);

	void io(i8&  v_);
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
	template<class V> void toEnum(const V& value_);
	void toStrView(StrView v_);

	void beginObject();
	void endObject();

	void beginArray();
	void endArray();
	void resizeArray(size_t n_);
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

template<class SE, class T, class ENABLE = void>
struct JsonIO
{
	static void io(SE& se_, T& data_)
	{
		se_.beginObject();
		data_.onJson(se_);
		se_.endObject();
	}
};

template<class SE, class T>
struct JsonIO<SE, T, std::enable_if_t<std::is_enum_v<T>> >
{
	static void io(SE& se_, T& data_)
	{
		se_.toEnum(data_);
	}
};

template<class T, size_t N>
struct JsonIO<JsonSerializer, Vector_<T, N>>
{
	static void io(JsonSerializer& se_, Vector_<T, N>& data_)
	{
		se_.beginArray();

		size_t n = data_.size();
		se_.resizeArray(n);
		for (size_t i = 0; i < n; i++)
		{
			se_.toArrayElement(i, data_[i]);
		}

		se_.endArray();
	}
};

template<size_t N>
struct JsonIO<JsonSerializer, String_<N>>
{
	static void io(JsonSerializer& se_, String_<N>& data_)
	{
		se_.toStrView(data_);
	}
};

#endif	 // JsonIO_Impl

#if 0
#pragma mark ========= JsonSerializer_Impl
#endif // 0
#if 1    // JsonSerializer_Impl

inline JsonSerializer::JsonSerializer(Json& json_) : _json(json_) { _stack.emplace_back(&_json); }

inline void JsonSerializer::io(u8& v_)  { toValue(v_); }
inline void JsonSerializer::io(u16& v_) { toValue(v_); }
inline void JsonSerializer::io(u32& v_) { toValue(v_); }
inline void JsonSerializer::io(u64& v_) { toValue(v_); }

inline void JsonSerializer::io(i8& v_)  { toValue(v_); }
inline void JsonSerializer::io(i16& v_) { toValue(v_); }
inline void JsonSerializer::io(i32& v_) { toValue(v_); }
inline void JsonSerializer::io(i64& v_) { toValue(v_); }

inline void JsonSerializer::io(f32& v_) { toValue(v_); }
inline void JsonSerializer::io(f64& v_) { toValue(v_); }

template<class V> inline
void JsonSerializer::io(V& data_) { JsonIO<This, V>::io(*this, data_); }

template<class V> inline
void JsonSerializer::named_io(const char* name_, V& data_) { toObjectMember(name_, data_); }

template<class V> inline
void JsonSerializer::toValue(V& value_)
{
	auto& cur = _stack.back();
	if (!cur->is_null())
		throw SGE_ERROR("json already contains value");
	*cur = value_;
}

template<class V> inline
void JsonSerializer::toEnum(const V& value_)
{
	toStrView(enumStr(value_));
}

inline void JsonSerializer::toStrView(StrView v_)
{
	auto& cur = _stack.back();
	if (!cur->is_null())
		throw SGE_ERROR("json already contains value");
	*cur = "";
	auto* dst = cur->get_ptr<Json::string_t*>();
	dst->assign(v_.begin(), v_.end());
}

inline void JsonSerializer::beginObject()
{
	auto& cur = _stack.back();
	if (!cur->is_null())
		throw SGE_ERROR("json already contains value");
	*cur = Json::object();
}

inline void JsonSerializer::endObject()
{
	auto& cur = _stack.back();
	if (!cur->is_object())
		throw SGE_ERROR("json is not object");
}

inline void JsonSerializer::beginArray()
{
	auto& cur = _stack.back();
	if (!cur->is_null())
		throw SGE_ERROR("json already contains value");
	*cur = Json::array();
}

inline void JsonSerializer::endArray()
{
	auto& cur = _stack.back();
	if (!cur->is_array())
		throw SGE_ERROR("json is not array");
}

inline void JsonSerializer::resizeArray(size_t n_)
{
	auto& cur = _stack.back();
	if (!cur->is_array())
		throw SGE_ERROR("json is not object");

	auto* arr = cur->get_ptr<Json::array_t*>();
	arr->resize(n_);
}

template<class V> inline 
void JsonSerializer::toArrayElement(size_t index_, V& data_)
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
void JsonSerializer::toObjectMember(const char* name_, V& data_)
{
	auto& obj = _stack.back();
	if (!obj->is_object())
		throw SGE_ERROR("not inside object");

	auto& memberValue = obj->operator[](name_);
	_stack.emplace_back(&memberValue);
	io(data_);
	_stack.pop_back();
}

#endif    // JsonSerializer_Impl

}