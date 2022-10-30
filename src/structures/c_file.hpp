#pragma once
#include <string_view>
#include <util/memory_view.hpp>

namespace structures{
struct c_file{
    enum class origin{
        set,    // seek from beginning of file
        cur,    // seed from current file pos
        end     // seek from end of file
    };

    c_file(std::string_view filename, std::string_view open_mode) { auto res = fopen_s(&handle, filename.data(), open_mode.data()); if (res) throw std::runtime_error{std::string("c_file() failed to open ") + filename.data()}; }
    ~c_file() {fclose(handle);}
    c_file(const c_file&) = delete;
    c_file& operator=(const c_file&) = delete;
    template<class T>
    size_t read(util::memory_view<T> data) {return fread(data.data(), sizeof(T), data.size(), handle);}
    template<class T>
    size_t write(const util::memory_view<T> data) {return fwrite(data.data(), sizeof(T), data.size(), handle);}
    int seek(long offset, origin origin = origin::set) {return fseek(handle, offset, static_cast<int>(origin));}
    long tell() {return ftell(handle);}
    operator bool() const {return bool(handle);}
    FILE* handle;
};
}