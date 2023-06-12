#pragma once

#include <iostream>
#include <stdexcept>
#include <initializer_list>
#include <algorithm>
#include <utility>
#include <iterator>
#include <cassert>

#include "array_ptr.h"

class ReserveProxyObj{
public:
    ReserveProxyObj(size_t reserved_capacity) : capacity_res(reserved_capacity) {}

    size_t GetCapacity() const noexcept{
        return capacity_res;
    } 
private:
    size_t capacity_res;
};

ReserveProxyObj Reserve(size_t reserving_capacity){
    return ReserveProxyObj(reserving_capacity);
}

template<typename Type>
class SimpleVector{
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    explicit SimpleVector() = default;

    explicit SimpleVector(size_t init_size) : size_(init_size), capacity_(init_size){
        _new_V_init(size_);
    }

    explicit SimpleVector(size_t init_size, const Type& init_value) : size_(init_size), capacity_(init_size){
        _new_V_init(size_, init_value);
    }

    explicit SimpleVector(std::initializer_list<Type> init_list) : size_(init_list.size()), capacity_(size_){
        _new_V_init(size_);
        size_t tmp_size = 0;
        for (const Type list_elem : init_list){
            begin_[tmp_size] = list_elem;
            ++tmp_size;
        }
    }

    SimpleVector(SimpleVector&& other_vec) noexcept : size_(std::exchange(other_vec.size_, 0)), capacity_(std::exchange(other_vec.capacity_, 0)) {
        begin_.swap(other_vec.begin_);
    }

    SimpleVector(const SimpleVector& other_vec) : size_(other_vec.GetSize()), capacity_(other_vec.GetCapacity()){
        try{
            begin_.swap(new Type[other_vec.size_]);
            std::copy(other_vec.begin(), other_vec.end(), begin_.Get());
        }
        catch(...){
            throw;
        }
    }

    explicit SimpleVector(const ReserveProxyObj proxy_obj) : size_(0), capacity_(proxy_obj.GetCapacity()) {}
    
    ~SimpleVector() {}

    SimpleVector& operator=(SimpleVector&& rhs){
        if (&rhs != this){
            delete[] begin_.Release();
            begin_.swap(rhs.begin_);
            size_ = std::exchange(rhs.size_, 0);
            capacity_ = std::exchange(rhs.capacity_, 0);
        }
        return *this;
    }

    SimpleVector& operator=(const SimpleVector& rhs){
        if (&rhs != this){
            Iterator _tmp_new_array;
            try{
                 _tmp_new_array = new Type[rhs.GetSize()];
                 std::copy(rhs.begin(), rhs.end(), _tmp_new_array);
            }
            catch(const std::bad_alloc& e){
                 throw;
            }
            delete[] begin_.Release();
            begin_.swap(_tmp_new_array);
            size_ = rhs.GetSize();
            capacity_ = size_;
        }
        return *this;
    }

    const Type& operator[](const size_t index) const noexcept{
        assert(index < size_);
        return *(begin_.Get() + index); 
    }

    Type& operator[](const size_t index) noexcept{
        assert(index < size_);
        return *(begin_.Get() + index); 
    }

    const Type& At(const size_t index) const{
        if (index >= size_){
            throw std::out_of_range("Index " + std::to_string(index) + " does not exist in the vector");
        }
        return *(begin_.Get() + index);
    }

    Type& At(const size_t index){
        if (index >= size_){
            throw std::out_of_range("Index " + std::to_string(index) + " does not exist in the vector");
        }
        return *(begin_.Get() + index);
    }

    size_t GetSize() const noexcept{ 
        return size_; 
    }

    size_t GetCapacity() const noexcept{
        return capacity_; 
    }

    bool IsEmpty() const noexcept{ 
        return size_ == 0; 
    }

    Iterator begin() noexcept{ 
        return begin_.Get(); 
    }

    ConstIterator begin() const noexcept{ 
        return begin_.Get(); 
    }

    Iterator end() noexcept{ 
        return begin_.Get() + size_; 
    }

    ConstIterator end() const noexcept{ 
        return begin_.Get() + size_; 
    }

    ConstIterator cbegin() const noexcept{ 
        return begin_.Get(); 
    }

    ConstIterator cend() const noexcept{ 
        return begin_.Get() + size_; 
    }

    void Clear() noexcept{ 
        size_ = 0; 
    }

    void Reserve(size_t new_capacity){
        if (new_capacity > capacity_){
            _new_mem_block_init_default(new_capacity);
            capacity_ = new_capacity;
        }
    }

    void Resize(size_t new_size){
        if (new_size <= size_){
            size_ = new_size;
        }
        else{
            Iterator _tmp_new_begin = _new_mem_block_init_default(new_size);
            delete[] begin_.Release();
            begin_.swap(_tmp_new_begin);
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    void PopBack() noexcept{
        assert(size_ > 0);
        --size_;
    }

    void PushBack(Type&& value){
        if (size_ < capacity_){
            begin_[size_] = std::move(value);
        }
        else{
            capacity_ = capacity_ != 0 ? capacity_ * 2 : 2;
            Iterator _tmp_new_array = _new_mem_block_init_default(capacity_);
            _tmp_new_array[size_] = std::move(value);
            delete[] begin_.Release();
            begin_.swap(_tmp_new_array);
        }
        ++size_;
    }

    void PushBack(const Type& value){
        if (size_ < capacity_){
            begin_[size_] = value;
        }
        else{
            capacity_ = capacity_ != 0 ? capacity_ * 2 : 2;
            Iterator _tmp_new_array = _new_mem_block_init_default(capacity_);
            _tmp_new_array[size_] = value;
            delete[] begin_.Release();
            begin_.swap(_tmp_new_array);
        }
        ++size_;
    }

    Iterator Erase(ConstIterator pos) noexcept{
        assert(_is_iterator_in_range(begin(), end(), pos));
        assert(size_ > 0);
        size_t pos_index = pos - cbegin();
        std::move(begin_.Get() + pos_index + 1, end(), begin_.Get() + pos_index);
        --size_;
        return begin_.Get() + pos_index;
    }


    Iterator Insert(ConstIterator pos, Type&& value){
        assert(_is_iterator_in_range(cbegin(), cend(), pos));
        size_t pos_index = pos - cbegin();
        if (size_ < capacity_){
            std::move(begin_.Get() + pos_index, end(), begin_.Get() + pos_index + 1);
            begin_[pos_index] = std::move(value);
        }
        else{
            capacity_ = capacity_ != 0 ? capacity_ * 2 : 2;
            Iterator _tmp_new_array = _new_mem_block_init_default(capacity_);
            delete[] begin_.Release();
            begin_.swap(_tmp_new_array);
            std::move(begin_.Get() + pos_index, end(), begin_.Get() + pos_index + 1);
            begin_[pos_index] = std::move(value);
        }
        ++size_;
        return begin_.Get() + pos_index;
    }


    Iterator Insert(ConstIterator pos, const Type& value){
        assert(_is_iterator_in_range(cbegin(), cend(), pos));
        size_t pos_index = pos - cbegin();
        if (size_ < capacity_){
            std::copy(pos, cend(), begin_ + pos_index + 1);
            begin_[pos_index] = value;
        }
        else{
            capacity_ = capacity_ != 0 ? capacity_ * 2 : 2;
            Iterator _tmp_new_array = _new_mem_block_init_default(capacity_);
            delete[] begin_.Release();
            begin_.swap(_tmp_new_array);
            std::copy(begin_.Get() + pos_index, end(), begin_.Get() + pos_index + 1);
            begin_[pos_index] = value;
        }
        ++size_;
        return begin_.Get()+ pos_index;
    }

    void swap(SimpleVector& other_vec) noexcept{
        begin_.swap(other_vec.begin_);
        std::swap(size_, other_vec.size_);
        std::swap(capacity_, other_vec.capacity_);
    }

private:
    void _new_V_init(size_t new_size){
        try{
            begin_.swap(new Type[new_size]);
            std::fill(begin_.Get(), begin_.Get() + new_size, Type{});
        }

        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    void _new_V_init(size_t new_size, const Type& value){
        _new_V_init(new_size);
        std::fill(begin_.Get(), begin_.Get() + new_size, value);
    }

    Iterator _new_mem_block_init_default(size_t new_size){
        Iterator _tmp_new_array;
        try{
            _tmp_new_array = new Type[new_size];
            std::move(begin(), end(), _tmp_new_array);
            for (size_t i = size_; i < new_size; ++i){
                _tmp_new_array[i] = Type{};
            }
        }
        catch(const std::bad_alloc& e){
            throw; 
        }
        return _tmp_new_array;
    }

    static bool _is_iterator_in_range(ConstIterator range_begin, ConstIterator range_end, ConstIterator pos) noexcept{
        return pos >= range_begin && pos <= range_end;     
    }

private:
    ArrayPtr<Type> begin_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};


template<typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return (lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template<typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return !(lhs == rhs); 
}

template<typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return (std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template<typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return !(rhs < lhs);
}

template<typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return rhs < lhs;
}

template<typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
    return !(lhs < rhs);
}