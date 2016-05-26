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

namespace helper
{
//Cloning make_unique here until it's standard in C++ 14
//Using a namespace to avoid confliting with MSVC's std::make_unique (which
// ADL can soemtimes find in unqualified calls).
    template <class T, class... Args>
    static 
        typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
        make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
} // end namespace helper

//
// Lexer
//

//The lexer returns tolens [0-255] if it is an unknown chraractor,
//otherwise one of these for known things.

enum Token
{
    tok_err = 0,
    tok_eof = -1,

    // commands
    tok_def = -2,
    tok_extern = -3,

    //primary
    tok_identifier = -4,
    tok_number = -5
};

static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number

/// gettok - Return the next token from standard input

static int gettok()
{
    static int LastChar = ' ';

    //Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getchar();

    //identifier: [a-zA-Z][a-zA-Z0-9]
    if (isalpha(LastChar))
    {
        IdentifierStr = LastChar;

        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;

        if (IdentifierStr == "def")
            return tok_def;

        if (IdentifierStr == "extern")
            return tok_extern;

        return tok_identifier;
    }

    // Number: [0-9.]
    if (isdigit(LastChar) || LastChar == '.')
    {
        std::string NumStr;
        bool isDotShown = false;
        int countDot = 0;

        do
        {
            if (LastChar == '.')
            {
                countDot++; 
            }

            if (countDot > 1)
                return tok_err;

            NumStr += LastChar;
            LastChar = getchar();
        }
        while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return tok_number;
    }

    // Comment until end of line
    if (LastChar == '#')
    {
        do
            LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }

    // Check for end of file. Don't eat the EOF.
    if (LastChar == EOF)
        return tok_eof;

    // Otherwise, just return the charactor as its ascii value
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}


//
// Abstract Syntax Tree
//

namespace
{
    /// ExprAST - Base class for all expression nodes
    class ExprAST
    {
    public:
        virtual ~ExprAST() {}
    };

    /// NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST
    {
        double Val;
    public:
        NumberExprAST(double Val) : Val(Val) {}
    };

    /// VariableExprAST - Expression class for referencing a variable, like "a"
    class VariableExprAST : public ExprAST
    {
        std::string Name;

    public:
        VariableExprAST(const std::string &Name) : Name(Name) {}
    };

    /// BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST {
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS;

    public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
            : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    };

    /// CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST
    {
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args;

    public:
        CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
            : Callee(Callee), Args(std:move(Args)) {}
    };

    /// PrototypeAST - This class represents the "prototype" for a function,
    /// which captures its name, and its argument names (thus implicitly the number
    /// of arguments the function takes).
    class PrototypeAST
    {
        std::string Name;
        std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string &Name, std::vector<std::string> Args)
            : Name(Name), Args(std::move(Args)) {}
    };

    /// FunctionAST - This class represents a function
    class FunctionAST
    {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                    std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}
    };
}
