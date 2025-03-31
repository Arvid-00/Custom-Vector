#pragma once
#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <stdlib.h>
#include <string>
#include <iostream>
#include <new>
#include <cstdlib>

#define CHECK assert(Invariant());

//declare iterator
template <class CT, int DIR>
class VectorIt;


template<class T, class Allocator = std::allocator<T>>
class Vector {
private:
	size_t _size;
	size_t _capacity;
	T* _data = nullptr;

	Allocator alloc;
	//Dalloc<T> _dAlloc;

public:
	using value_type = T;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;
	using iterator = VectorIt < T, +1 >;
	using const_iterator = VectorIt < const T, +1 >;
	using reverse_iterator = VectorIt<T, -1>;
	using const_reverse_iterator = VectorIt<const T, -1>;

	Vector() noexcept
	{
		_size = 0;
		_capacity = 0;
	}

	template<class Titer>
	Vector(size_t newCapacity, const Titer& begin, const Titer& end)
	{
		_capacity = newCapacity;
		_size = 0;
		if (_capacity == 0) {
			_data = nullptr; 
			return; 
		}

		_data = alloc.allocate(_capacity);

		try 
		{
			size_t newSize = end - begin;
			for (; _size < newSize; ++_size)
				new(_data + _size) T(*(begin + _size));
		}
		catch (...) 
		{
			for (_size; _size > 0;)
				_data[--_size].~T();
			alloc.deallocate(_data, _capacity);
			throw;

		}
	}

	Vector(const char* other) : Vector(std::strlen(other), other, other + std::strlen(other))
	{
	}

	Vector(const Vector& other) : Vector(other.size(), other.begin(), other.end())
	{
	}

	Vector& operator=(const Vector& other) 
	{

		if (other != *this) 
		{
			size_t oldCap = _capacity;
			size_t oldSize = _size; 
			T* temp = alloc.allocate(_capacity); 
			
			size_t i = 0;
			try 
			{
				for (i; i < _size; i++)
					new(temp + i) T(*(_data + i));
			}
			catch (...) 
			{
				for (i; i > 0;)
					temp[i].~T();
				alloc.deallocate(temp, _capacity);
				return *this;
			}
			

			try 
			{
				for (_size = 0; _size < other.capacity(); _size++)
				{
					if (size() > capacity())
					{
						reAllocateRawData(capacity() * 2 + 1);
					}
					_data[_size] = other._data[_size];
				}
			}
			catch (...) 
			{
				for (_size; _size > 0;)
					_data[--_size].~T();
				alloc.deallocate(_data, _capacity);

				_data = temp; 

				return *this;

			}
			for (size_t i = 0; i < oldSize; i++)
				temp[i].~T();
			alloc.deallocate(temp, oldCap);

			
			return *this;
		}
		
	}

	Vector (Vector&& other) noexcept 
	{

		_data = other._data;
		_size = other.size();
		_capacity = other.capacity();

		other._data = nullptr;
		other._size = 0;
		other._capacity = 0; 
	}

	Vector& operator=(Vector&& other) noexcept 
	{
		_data = other._data;
		_size = other.size();
		_capacity = other.capacity();

		other._data = nullptr;
		other._capacity = 0;
		other._size = 0;

		return *this;
	}

	T& operator[](size_t i)
	{
		return _data[i];
	}
	const T& operator[](size_t i) const
	{
		return _data[i];
	}

	T& at(size_t i) 
	{
		if (i >= size())
			throw std::out_of_range("out_of_range");
		return _data[i];
	}
	const T& at(size_t i) const
	{
		if (i >= size())
			throw std::out_of_range("out_of_range");
		return _data[i];
	}

	T* data() noexcept 
	{
		return _data;
	}
	const T* data() const noexcept
	{
		return _data;
	}

	size_t size() const noexcept
	{
		return _size;
	}
	size_t capacity() const noexcept
	{
		return _capacity;
	}
	bool Invariant() const
	{
		if (_size > _capacity)
			return false;
		if (_data == nullptr && _capacity > 0)
			return false;
		if (_data != nullptr && _capacity == 0)
			return false;
		return true;
	}
	void push_back(const T& c)
	{
		if (size() >= capacity())
		{
			reAllocateRawData(capacity() * 2 + 1);

		}

		try
		{
			new(_data + _size++) T(c);
		}
		catch (...) 
		{
			_data[--_size].~T();
		}
		
	}
	void pop_back() noexcept 
	{
		_size--; 
	}

	void reAllocateRawData(size_t newCap) 
	{
		size_t oldCap = _capacity;
		_capacity = newCap;
		size_t counter;
		
		T* temp = alloc.allocate(newCap);

		try 
		{
			for (counter = 0; counter < _size; counter++)
			{
				new(temp + counter) T(*(_data + counter));
			}
		}
		catch (...) 
		{
			for (counter; counter > 0;)
				temp[--counter].~T();
			alloc.deallocate(temp, newCap);
			 
		}

		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		alloc.deallocate(_data, oldCap);
		_data = temp;
		
	}


	void reAllocateData(size_t newCap) 
	{
		size_t oldCap = _capacity;
		_capacity = newCap;

		T* temp = alloc.allocate(newCap);

		for (size_t i = 0; i < _size; i++)
		{
			new(temp + i) T(*(_data + i));
		}
		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		alloc.deallocate(_data, oldCap);
		_data = temp;
	}
	void reserve(size_t n)
	{
		if(n > capacity())
		{
			reAllocateRawData(n);
		}

	}
	void shrink_to_fit() 
	{
		if(_capacity > _size)
			reAllocateRawData(size());
	}
	void resize(size_t n) 
	{
		size_t oldSize = size();


		if (n > capacity())
			reAllocateRawData(n);

		if (n > size()) {
			try 
			{
				for (_size; _size < n; _size++)
				{
					new(_data + _size) T();
				}
			}
			catch (...) 
			{
				std::cout << "EXCEPTION THROWN" << std::endl;
				for(oldSize; oldSize <= _size; oldSize++)
					_data[oldSize].~T();
			}
		}
		else 
		{
			for (_size; _size > n;)
				_data[--_size].~T();
		}
	}
	~Vector() noexcept
	{
		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		alloc.deallocate(_data, _capacity);
	}

	friend bool operator==(const Vector& lhs, const Vector& rhs)
	{
		if (lhs.size() != rhs.size())
			return false;
		for (int i = 0; i < lhs.size(); i++)
		{
			if (lhs._data[i] != rhs._data[i])
				return false;
		}
		return true;
	}
	friend bool operator!=(const Vector& lhs, const Vector& rhs) 
	{
		return !(lhs == rhs);
	}
	friend bool operator<(const Vector& lhs, const Vector& rhs)
	{
		for (int i = 0; i < lhs.size() && i < rhs.size(); i++) 
		{
			if (lhs._data[i] < rhs._data[i])
				return true;
		}
		if (lhs.size() < rhs.size())
			return true;
		return false;
	}
	friend bool operator>(const Vector& lhs, const Vector& rhs)
	{
		for (int i = 0; i < lhs.size() && i < rhs.size(); i++)
		{
			if (lhs._data[i] > rhs._data[i])
				return true;
		}
		if (lhs.size() > rhs.size())
			return true;
		return false;
	}
	friend bool operator<=(const Vector& lhs, const Vector& rhs)
	{
		return !(lhs > rhs);
	}
	friend bool operator>=(const Vector& lhs, const Vector& rhs)
	{
		return !(lhs < rhs);
	}

	iterator begin() noexcept
	{
		return iterator(_data);
	}
	iterator end() noexcept
	{
		return iterator(_data + _size);
	}

	reverse_iterator rbegin() noexcept 
	{
		return reverse_iterator(_data + _size - 1);
	}
	reverse_iterator rend() noexcept
	{
		return reverse_iterator(_data - 1);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(_data); 
	}
	const_iterator end() const noexcept
	{
		return const_iterator(_data + _size);
	}
	const_reverse_iterator rbegin() const noexcept 
	{
		return const_reverse_iterator(_data + _size - 1);
	}
	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(_data - 1);
	}

	const_iterator cbegin() const noexcept
	{
		return const_iterator(_data);
	}
	const_iterator cend() const noexcept
	{
		return const_iterator(_data + _size);
	}
	const_reverse_iterator crbegin() const noexcept 
	{
		return const_reverse_iterator(_data  + _size - 1);
	}
	const_reverse_iterator crend() const noexcept 
	{
		return const_reverse_iterator(_data - 1);
	}
};

template <class CT, int DIR>
class VectorIt {
private:
	CT* _ptr;
public:
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = CT;
	using reference = CT&;
	using pointer= CT*;
	VectorIt() 
	{

	}
	VectorIt(pointer p) : _ptr(p)
	{
		//done?
	}
	VectorIt(const VectorIt& other) = default;
	VectorIt& operator=(const VectorIt& other) = default;

	reference operator*() const 
	{
		return *_ptr;
	}
	pointer operator->() const 
	{
		return _ptr; 
	}
	reference operator[](size_t i) const 
	{
		return _ptr[i * DIR];

	}
	VectorIt& operator++() 
	{
		_ptr += DIR;
		return *this;
	}
	VectorIt operator++(int)
	{
		VectorIt iterator = *this;
		_ptr += DIR; 
		return iterator;
	}
	VectorIt& operator--()
	{
		_ptr -= DIR;
		return *this;
	}
	VectorIt operator--(int)
	{
		VectorIt iterator = *this;
		_ptr -= DIR; 
		return iterator;
	}
	VectorIt& operator+=(difference_type i)
	{
		_ptr += i;
		return *this;
	}
	VectorIt operator+(difference_type i) const 
	{
		
		return _ptr + (i * DIR);
	}
	VectorIt operator-(difference_type i) const
	{
		return _ptr - (i * DIR);
	}
	difference_type operator-(const VectorIt& other) const
	{
		return (_ptr - other._ptr) * DIR; 
	}

	friend bool operator==(const VectorIt& lhs, const VectorIt& rhs) 
	{
		return lhs._ptr == rhs._ptr; 
	}
	friend bool operator!=(const VectorIt& lhs, const VectorIt& rhs) 
	{
		return !(lhs == rhs);
	}
	friend bool operator<(const VectorIt& lhs, const VectorIt& rhs) 
	{
		if (lhs - rhs < 0)
			return true;

		return false;
	}
	friend bool operator>(const VectorIt& lhs, const VectorIt& rhs) 
	{
		if (lhs - rhs > 0)
			return true;

		return false;
	}
	friend bool operator>=(const VectorIt& lhs, const VectorIt& rhs)
	{
		return !(lhs < rhs);
	}
	friend bool operator<=(const VectorIt& lhs, const VectorIt& rhs)
	{
		return !(lhs > rhs);
	}

};


template<class T>
void swap(Vector<T>& lhs, Vector<T>& rhs) 
{
	Vector temp(std::move(lhs));
	lhs = std::move(rhs);
	rhs = std::move(temp);
}

template<class T>
std::ostream& operator<<(std::ostream& cout
	, const Vector<T>& other) {
	for (size_t i = 0; i < other.size(); ++i)
		cout << other[i];
	return cout;
}

#endif VECTOR_HPP