#pragma once

namespace sge {

template<class T>
class ComPtr : public NonCopyable
{
public:
	ComPtr() = default;
	~ComPtr() noexcept { reset(nullptr); }

	ComPtr(const ComPtr& rhs_)	{ reset(rhs_._ptr); }
	ComPtr(ComPtr&& rhs_)		{ _ptr = rhs_._ptr; rhs_._ptr = nullptr; }

	void operator=(const ComPtr& rhs_)	{ reset(rhs_._ptr); }
	void operator=(ComPtr&& rhs_)		{ reset(nullptr); _ptr = rhs_._ptr; rhs_._ptr = nullptr; }

			 T* operator->() noexcept		{ return _ptr; }
	const	 T* operator->() const noexcept { return _ptr; }

	operator	   T* () noexcept			{ return _ptr; }
	operator const T* () const noexcept		{ return _ptr; }

			T* ptr() noexcept				{ return _ptr; }
	const	T* ptr() const  noexcept		{ return _ptr; }

	void reset(T* ptr_)
	{
		if (_ptr == ptr_)
			return;

		if (_ptr)
		{
			_ptr->Release();
			_ptr = nullptr;
		}
		_ptr = ptr_;
		if (_ptr)
		{
			_ptr->AddRef();
		}
	}

	T** ptrForInit() noexcept { reset(nullptr); return &_ptr; }

	T* detach() { T* pOut = _ptr = nullptr; return o; }

private:
	T* _ptr = nullptr;
};

}