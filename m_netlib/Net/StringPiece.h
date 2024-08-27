#include <string>
#include <algorithm>
#include <string.h>

class StringPiece {
public:
    // Constructors
    StringPiece() : data_(nullptr), size_(0) {}
    StringPiece(const char* str) : data_(str), size_(strlen(str)) {}
    StringPiece(const std::string& str) : data_(str.data()), size_(str.size()) {}
    StringPiece(const char* offset, size_t len) : data_(offset), size_(len) {}

    // Accessors
    const char* data() const { return data_; }
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
    // Returns the ith character
    char operator[](size_t i) const { return data_[i]; }

    // Converts to std::string
    std::string as_string() const { return std::string(data_, size_); }

    // Comparisons
    bool operator==(const StringPiece& x) const {
        return size_ == x.size_ && memcmp(data_, x.data_, size_) == 0;
    }
    bool operator!=(const StringPiece& x) const { return !(*this == x); }

private:
    const char* data_;
    size_t size_;
};

