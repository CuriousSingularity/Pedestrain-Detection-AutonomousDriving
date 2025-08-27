/***************************************************************************
 *============= Copyright by Darmstadt University of Applied Sciences =======
 ****************************************************************************
 * Filename        : Cpp20Compatibility.h
 * Author          : Bharath Ramachandraiah (stbhrama@stud.h-da.de)
 * Description     : C++20 compatibility layer for older compilers
 *
 ****************************************************************************/

#ifndef CPP20COMPATIBILITY_H
#define CPP20COMPATIBILITY_H

// Check C++ standard version
#if __cplusplus >= 202002L
    #define HAS_CPP20 1
    #define HAS_CPP17 1
    #define HAS_CPP14 1
#elif __cplusplus >= 201703L
    #define HAS_CPP20 0
    #define HAS_CPP17 1
    #define HAS_CPP14 1
#elif __cplusplus >= 201402L
    #define HAS_CPP20 0
    #define HAS_CPP17 0
    #define HAS_CPP14 1
#else
    #define HAS_CPP20 0
    #define HAS_CPP17 0
    #define HAS_CPP14 0
#endif

#include <type_traits>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// C++20 features with fallbacks
#if HAS_CPP20

// Real C++20 features
#include <concepts>
#include <ranges>
#include <span>
#include <numbers>

// Check if format is available (requires libstdc++12+)
#if __has_include(<format>) && __cpp_lib_format >= 201907L
    #include <format>
    #define HAS_STD_FORMAT 1
#else
    #define HAS_STD_FORMAT 0
#endif

// Check if coroutine is available 
#if __has_include(<coroutine>)
    #include <coroutine>
    #define HAS_STD_COROUTINE 1
#else
    #define HAS_STD_COROUTINE 0
#endif

namespace pedestrian_detection::compat {
    using std::span;
    
    template<typename T>
    using remove_cvref_t = std::remove_cvref_t<T>;
    
    // Feature availability flags
    #define CONCEPTS_AVAILABLE 1
    #define RANGES_AVAILABLE 1
    #define COROUTINES_AVAILABLE HAS_STD_COROUTINE
    #define FORMAT_AVAILABLE HAS_STD_FORMAT
    #define SPAN_AVAILABLE 1
    
    // Format function - use std::format if available, otherwise fallback
    #if HAS_STD_FORMAT
    using std::format;
    #else
    // Fallback format implementation
    template<typename T>
    std::string format(const std::string& fmt, const T& value) {
        std::ostringstream oss;
        size_t pos = fmt.find("{}");
        if (pos != std::string::npos) {
            oss << fmt.substr(0, pos) << value << fmt.substr(pos + 2);
        } else {
            oss << fmt;
        }
        return oss.str();
    }
    
    template<typename T1, typename T2>
    std::string format(const std::string& fmt, const T1& value1, const T2& value2) {
        std::string temp = fmt;
        
        // Replace first {}
        size_t pos1 = temp.find("{}");
        if (pos1 != std::string::npos) {
            std::ostringstream oss1;
            oss1 << value1;
            temp = temp.substr(0, pos1) + oss1.str() + temp.substr(pos1 + 2);
        }
        
        // Replace second {}
        size_t pos2 = temp.find("{}");
        if (pos2 != std::string::npos) {
            std::ostringstream oss2;
            oss2 << value2;
            temp = temp.substr(0, pos2) + oss2.str() + temp.substr(pos2 + 2);
        }
        
        return temp;
    }
    
    template<typename T1, typename T2, typename T3>
    std::string format(const std::string& fmt, const T1& value1, const T2& value2, const T3& value3) {
        std::string temp = fmt;
        size_t pos = 0;
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            std::ostringstream oss1;
            oss1 << value1;
            temp.replace(pos, 2, oss1.str());
            pos += oss1.str().length();
        }
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            std::ostringstream oss2;
            oss2 << value2;
            temp.replace(pos, 2, oss2.str());
            pos += oss2.str().length();
        }
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            std::ostringstream oss3;
            oss3 << value3;
            temp.replace(pos, 2, oss3.str());
        }
        
        return temp;
    }
    #endif
}

#else

// Fallbacks for older standards
#include <sstream>
#include <algorithm>
#include <iterator>

namespace pedestrian_detection::compat {
    
    // Span fallback for C++17/14
    template<typename T>
    class span {
    private:
        T* m_data;
        size_t m_size;
        
    public:
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;
        
        constexpr span() noexcept : m_data(nullptr), m_size(0) {}
        
        constexpr span(T* ptr, size_type count) noexcept : m_data(ptr), m_size(count) {}
        
        template<size_t N>
        constexpr span(T (&arr)[N]) noexcept : m_data(arr), m_size(N) {}
        
        template<typename Container>
        constexpr span(Container& cont) : m_data(cont.data()), m_size(cont.size()) {}
        
        constexpr iterator begin() const noexcept { return m_data; }
        constexpr iterator end() const noexcept { return m_data + m_size; }
        constexpr const_iterator cbegin() const noexcept { return m_data; }
        constexpr const_iterator cend() const noexcept { return m_data + m_size; }
        
        constexpr reference operator[](size_type idx) const { return m_data[idx]; }
        constexpr reference front() const { return m_data[0]; }
        constexpr reference back() const { return m_data[m_size - 1]; }
        constexpr pointer data() const noexcept { return m_data; }
        
        constexpr size_type size() const noexcept { return m_size; }
        constexpr size_type size_bytes() const noexcept { return m_size * sizeof(T); }
        constexpr bool empty() const noexcept { return m_size == 0; }
        
        constexpr span<T> subspan(size_type offset, size_type count = SIZE_MAX) const {
            size_type actual_count = (count == SIZE_MAX) ? m_size - offset : count;
            return span<T>(m_data + offset, actual_count);
        }
        
    private:
        static constexpr size_type SIZE_MAX = static_cast<size_type>(-1);
    };
    
    // remove_cvref_t fallback
    template<typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
    
    // Concepts fallbacks (simplified)
    #define CONCEPTS_AVAILABLE 0
    #define RANGES_AVAILABLE 0
    #define COROUTINES_AVAILABLE 0
    #define FORMAT_AVAILABLE 0
    #define SPAN_AVAILABLE 1
    
    // Simple format function that handles basic {} placeholders
    template<typename T>
    std::string format(const std::string& fmt, const T& value) {
        std::ostringstream oss;
        size_t pos = fmt.find("{}");
        if (pos != std::string::npos) {
            oss << fmt.substr(0, pos) << value << fmt.substr(pos + 2);
        } else {
            oss << fmt;
        }
        return oss.str();
    }
    
    template<typename T1, typename T2>
    std::string format(const std::string& fmt, const T1& value1, const T2& value2) {
        std::ostringstream oss;
        std::string temp = fmt;
        
        // Replace first {}
        size_t pos1 = temp.find("{}");
        if (pos1 != std::string::npos) {
            temp = temp.substr(0, pos1) + std::to_string(value1) + temp.substr(pos1 + 2);
        }
        
        // Replace second {}
        size_t pos2 = temp.find("{}");
        if (pos2 != std::string::npos) {
            temp = temp.substr(0, pos2) + std::to_string(value2) + temp.substr(pos2 + 2);
        }
        
        return temp;
    }
    
    template<typename T1, typename T2, typename T3>
    std::string format(const std::string& fmt, const T1& value1, const T2& value2, const T3& value3) {
        std::ostringstream oss;
        std::string temp = fmt;
        
        // Simple sequential replacement (not perfect but works for basic cases)
        size_t pos = 0;
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            temp.replace(pos, 2, std::to_string(value1));
            pos += std::to_string(value1).length();
        }
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            temp.replace(pos, 2, std::to_string(value2));
            pos += std::to_string(value2).length();
        }
        
        pos = temp.find("{}", pos);
        if (pos != std::string::npos) {
            temp.replace(pos, 2, std::to_string(value3));
        }
        
        return temp;
    }
}

#endif // HAS_CPP20

// Common compatibility macros
#if HAS_CPP17
    #define NODISCARD [[nodiscard]]
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    #define NODISCARD
    #define MAYBE_UNUSED
#endif

#if HAS_CPP14
    #define CONSTEXPR_14 constexpr
#else
    #define CONSTEXPR_14
#endif

// Concept simulation for older compilers
#if !CONCEPTS_AVAILABLE
    #define requires(...) 
    template<typename T> struct DetectionAlgorithm : std::true_type {};
    template<typename T> struct Observable : std::true_type {};
    template<typename T> struct Observer : std::true_type {};
    template<typename T> struct ConfigurationValue : std::true_type {};
#endif

// Coroutines fallback - disable coroutine features
#if !COROUTINES_AVAILABLE
namespace pedestrian_detection::async {
    // Fallback Task that just holds a value
    template<typename T>
    struct Task {
        T value;
        
        Task(T v) : value(std::move(v)) {}
        
        T get() { return std::move(value); }
        bool is_ready() const { return true; }
    };
    
    // Fallback Generator that just holds a vector
    template<typename T>
    struct Generator {
        std::vector<T> values;
        
        Generator(std::vector<T> v) : values(std::move(v)) {}
        
        auto begin() { return values.begin(); }
        auto end() { return values.end(); }
    };
}
#endif

// Ranges fallback - simple filter function
#if !RANGES_AVAILABLE
namespace pedestrian_detection::ranges {
    template<typename Container, typename Predicate>
    auto filter(const Container& container, Predicate pred) {
        std::vector<typename Container::value_type> result;
        std::copy_if(container.begin(), container.end(), std::back_inserter(result), pred);
        return result;
    }
    
    template<typename Container, typename Transform>
    auto transform(const Container& container, Transform trans) {
        using ReturnType = decltype(trans(*container.begin()));
        std::vector<ReturnType> result;
        result.reserve(container.size());
        std::transform(container.begin(), container.end(), std::back_inserter(result), trans);
        return result;
    }
}
#endif

// Consteval fallback
#if !HAS_CPP20
    #define consteval constexpr
    #define constinit 
#endif

#endif /* CPP20COMPATIBILITY_H */