#include <cstdlib>
#include <utility>
#include <cassert>
#include <stdexcept>

template<typename Type>
class ArrayPtr{
public:
    ArrayPtr() = default;

    ArrayPtr(const ArrayPtr& other_array) = delete; // prohibit copying for ArrayPtrs

    ArrayPtr& operator=(const ArrayPtr& other_array) = delete; // prohibit assigning for ArrayPtrs
    
    explicit ArrayPtr(std::size_t array_size) : array_size_(array_size) {
        if (array_size > 0){
            try{
                raw_pointer_ = new Type[array_size];
            }
            catch(const std::bad_alloc& e){
                throw;
            }
        }
    }

    explicit ArrayPtr(Type* array_ptr, const std::size_t array_size) : array_size_(array_size){
        assert(array_ptr != nullptr);
        if (array_ptr != raw_pointer_){
            raw_pointer_ = array_ptr;
        }
    }
    explicit ArrayPtr(Type* array_ptr) : raw_pointer_(array_ptr) {} 
    
    ~ArrayPtr(){
        array_size_ = 0;
        delete[] raw_pointer_;
    }

    const Type& operator[](std::size_t index) const noexcept{
        return *(raw_pointer_ + index);
    }

    Type& operator[](std::size_t index) noexcept{
        return *(raw_pointer_ + index);
    }

    explicit operator bool() const noexcept{
        return raw_pointer_;
    }

    Type* Get() const noexcept{
        return raw_pointer_;
    }

    [[nodiscard]] Type* Release() noexcept{
        Type* _tmp_raw_ptr = raw_pointer_;
        raw_pointer_ = nullptr;
        return _tmp_raw_ptr;
    }

    void swap(ArrayPtr& other_array) noexcept{
        Type* _tmp_raw_ptr = raw_pointer_;
        raw_pointer_ = other_array.raw_pointer_;
        other_array.raw_pointer_ = _tmp_raw_ptr;
    }

    void swap(Type* other_raw_ptr_) noexcept{
        Type* _tmp_raw_ptr = raw_pointer_;
        raw_pointer_ = other_raw_ptr_;
        other_raw_ptr_ = _tmp_raw_ptr;
    }



private:
    std::size_t array_size_ = 0;
    Type* raw_pointer_ = nullptr;
};