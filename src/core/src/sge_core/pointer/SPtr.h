#pragma once

namespace sge {

template<class T>
class SPtr : public NonCopyable
{
public:
	SPtr() = default;
	~SPtr() noexcept { reset(nullptr); }

	SPtr(T* ptr_)			{ reset(ptr_); }
	void operator=(T* ptr_) { reset(ptr_); }

	SPtr(SPtr&& rhs_)			{ _ptr = rhs_._ptr; rhs_._ptr = nullptr; }
	void operator=(SPtr&& rhs_) { reset(nullptr); _ptr = rhs_.ptr; rhs_._ptr = nullptr; }

		  T* operator->() noexcept				{ return _ptr; }
	const T* operator->() const noexcept		{ return _ptr; }

	operator	   T* () noexcept				{ return _ptr; }
	operator const T* () noexcept				{ return _ptr; }

			T* ptr() noexcept					{ return _ptr; }
	const	T* ptr() const  noexcept			{ return _ptr; }

	void reset(T* ptr_)
	{
		static_assert(std::is_base_of<RefCountBase, T>::value, "");
		if (_ptr == ptr_)
			return;

		if (_ptr)
		{
			auto c = --_ptr->_refCount;
			if (c <= 0)
			{
				sge_delete(_ptr);
			}
		}

		_ptr = ptr_;
		if (ptr_)
		{
			++_ptr->_refCount;
		}
	}

	T* detach() { T* o = _ptr; _ptr = nullptr; return o; }

private:
	T* _ptr = nullptr;
};

}