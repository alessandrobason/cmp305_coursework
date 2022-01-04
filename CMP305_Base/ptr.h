#pragma once

#include <assert.h>

// super simple and small unique_ptr implementation, should be faster to compile
// then include <memory> every time

template<typename T>
class ptr {
public:
	ptr() noexcept {}
	ptr(std::nullptr_t) noexcept {}
	explicit ptr(T *raw) noexcept : data(raw) {}

	ptr(const ptr &other) noexcept = delete;
	ptr &operator=(const ptr &other) noexcept = delete;

	ptr(ptr &&other) noexcept {
		*this = std::move(other);
	}

	ptr &operator=(T *raw) noexcept {
		data = raw;
		return *this;
	}

	ptr &operator=(ptr &&other) noexcept {
		data = other.data;
		other.data = nullptr;
		return *this;
	}

	~ptr() noexcept {
		if(data) delete data;
	}

	T *get() const noexcept {
		return data;
	}

	T *release() noexcept {
		T *temp = data;
		data = nullptr;
		return temp;
	}

	void swap(ptr &other) noexcept {
		T *temp = data;
		data = other.data;
		other.data = temp;
	}

	void reset(T *raw) noexcept {
		if (data) delete data;
		data = raw;
	}

	explicit operator bool() const noexcept {
		return data;
	}

	T *operator->() const noexcept {
		assert(data && "data in ptr was not initialized");
		return data;
	}

	T &operator*() const noexcept {
		assert(data && "data in ptr was not initialized");
		return *data;
	}

	operator T*() const noexcept {
		return data;
	}

private:
	T *data = nullptr;
};