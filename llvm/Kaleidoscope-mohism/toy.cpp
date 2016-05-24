/*
 *  Mohism Research
 */

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace helper {
//Cloning make_unique here until it's standard in C++ 14
//Using a namespace to avoid confliting with MSVC's std::make_unique (which
// ADL can soemtimes find in unqualified calls).
template <class T, class... Args>
static 
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // end namespace helper

//
// Lexer
//

//The lexer returns tolens [0-255] if it is an unknown chraractor,
//otherwise one of these for known things.

enum Token {
    tok_eof = -1,

    // commands
    tok_def = -2,
    tok_extern = -3,

    //primary
    tok_identifier = -4,
    tok_number = -5
};


