#pragma once
#ifndef VECTOR_HPP
#define VECTOR_HPP
#include <stdlib.h>
#include <string>
#include <iostream>
#include <new>
#include <cstdlib>
#include "Dalloc.hpp"

#define CHECK assert(Invariant());

//declare iterator
template <class CT, int DIR>
class VectorIt;


template<class T>
class Vector {
private:
	size_t _size;
	size_t _capacity;
	T* _data = nullptr;

	Dalloc<T> _dAlloc;

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

		_data = _dAlloc.allocate(_capacity);

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
			_dAlloc.deallocate(_data, _capacity);
			throw;

		}
	}

	Vector(const char* other) : Vector(std::strlen(other), other, other + std::strlen(other))
	{
		//default
		/*
		_size = 0;
		_capacity = strlen(other);
		_data = new T[_capacity];
		for (int i = 0; i < strlen(other); i++)
		{
			_data[i] = other[i];
			_size++;
		}
		*/
	}

	Vector(const Vector& other) : Vector(other.size(), other.begin(), other.end())
	{
		//default
		/*
		_capacity = other.capacity();
		_data = new T[_capacity];
		for (_size = 0; _size < _capacity; _size++) 
		{
			_data[_size] = other._data[_size];
		}
		*/
	}

	Vector& operator=(const Vector& other) 
	{

		if (other != *this) 
		{
			size_t oldCap = _capacity;
			size_t oldSize = _size; 
			T* temp = _dAlloc.allocate(_capacity); //insert current data into this temp pointer, an excepetion is caught empty current data and replace with temp
			
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
				_dAlloc.deallocate(temp, _capacity);
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
				_dAlloc.deallocate(_data, _capacity);

				_data = temp; 

				return *this;

			}
			for (size_t i = 0; i < oldSize; i++)
				temp[i].~T();
			_dAlloc.deallocate(temp, oldCap);

			
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
			reAllocateRawData(capacity() * 2 + 1);//increase capacity + for variables with 0 size 

		}

		//raw
		try
		{
			new(_data + _size++) T(c);
			//_size++;
		}
		catch (...) 
		{
			_data[--_size].~T();
		}

		//default
		//_data[_size++] = c;
		
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
		
		T* temp = _dAlloc.allocate(newCap);

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
			_dAlloc.deallocate(temp, newCap);
			 
		}

		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		_dAlloc.deallocate(_data, oldCap);
		_data = temp;
		
	}


	void reAllocateData(size_t newCap) 
	{
		size_t oldCap = _capacity;
		_capacity = newCap;
		//raw malloc
		// T* temp = static_cast<T*>(malloc(newCap * sizeof(T)));
		//raw dalloc
		T* temp = _dAlloc.allocate(newCap);

		for (size_t i = 0; i < _size; i++)
		{
			new(temp + i) T(*(_data + i));
			//std::cout << temp << std::endl;
		}
		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		_dAlloc.deallocate(_data, oldCap);
		//raw  malloc 
		//free(_data);
		_data = temp;


		//defualt
		/*
		T* temp = new T[_capacity];
		for (int i = 0; i < size(); i++) 
		{
			temp[i] = _data[i];
		}
		delete[] _data;
		_data = temp;
		*/
	}
	void reserve(size_t n)
	{
		//if (n < capacity())
			//return false; // throw std::out_of_range("N is smaller than capacity");
		if(n > capacity())
		{
			reAllocateRawData(n);
		}

	}
	void shrink_to_fit() 
	{
		//check if nessecary
		if(_capacity > _size)
			reAllocateRawData(size());
	}
	void resize(size_t n) 
	{
		size_t oldSize = size();


		if (n > capacity())
			reAllocateRawData(n);

		//std::cout << "n: " << n << "capacity: " << capacity() << "size: " << size() << "s: " << s << std::endl;
		if (n > size()) {
			try 
			{
				for (_size; _size < n; _size++)
				{
					//raw
					new(_data + _size) T();
					//default
					//_data[_size] = T();
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
			//std::cout << "n: " << n << "capacity: " << capacity() << "size: " << size() << "s: " << s << std::endl;
			for (_size; _size > n;)
				_data[--_size].~T();
			//std::cout << "n: " << n << "capacity: " << capacity() << "size: " << size() << "s: " << s << std::endl;
		}
	}
	~Vector() noexcept
	{
		//default
		//delete[] _data;

		//raw malloc
		//for (size_t i = 0; i < _size; i++)
			//_data[i].~T();
		//free(_data);

		//raw dalloc
		for (size_t i = 0; i < _size; i++)
			_data[i].~T();
		_dAlloc.deallocate(_data, _capacity);
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
		return reverse_iterator(_data + _size - 1);//_data + _size - 1?
	}
	reverse_iterator rend() noexcept
	{
		return reverse_iterator(_data - 1);//_data - 1?
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
		_ptr += DIR; // ++(*this);
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
		_ptr -= DIR; // ++(*this);
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
		return lhs._ptr == rhs._ptr; // Correct? maybe check content of pointers?
	}
	friend bool operator!=(const VectorIt& lhs, const VectorIt& rhs) 
	{
		return !(lhs == rhs);
	}
	friend bool operator<(const VectorIt& lhs, const VectorIt& rhs) 
	{
		//currently checking difference, but maybe incorrect? 
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