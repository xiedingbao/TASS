#include <iostream>
#include <list>
#include <cassert>
#include <stdexcept>

/// \todo Cuz I'm short of time, there is no smart pointers or 
/// even destructors (w/ delete) :) -- THIS IS FAR FROM PRODUCTION CODE, 
/// but it is minimalistic to realize the idea...

/**
* \brief Expression abstract base class
*
* ... has only evaluate function.
*/
struct expression
{
    virtual ~expression() {};
    virtual int operator()() const = 0;
};

struct number_token : public expression
{
    int value_;
    number_token(const int value = 0) : value_(value) {}
    int operator()() const {
        return value_;
    }
};

struct binary_predicate : public expression
{
    expression* left_;
    expression* right_;

    binary_predicate(expression* const left = 0, expression* const right = 0)
    : left_(left), right_(right)
    {}
};

struct plus : public binary_predicate
{
    plus(expression* const left, expression* const right)
    : binary_predicate(left, right)
    {}

    int operator()() const {
        return (*left_)() + (*right_)();
    }
};

struct minus : public binary_predicate
{
    minus(expression* const left, expression* const right)
    : binary_predicate(left, right)
    {}

    int operator()() const {
        return (*left_)() - (*right_)();
    }
};

struct mul : public binary_predicate
{
    mul(expression* const left, expression* const right)
    : binary_predicate(left, right)
    {}

    int operator()() const {
        return (*left_)() * (*right_)();
    }
};

struct div : public binary_predicate
{
    div(expression* const left, expression* const right)
    : binary_predicate(left, right)
    {}

    int operator()() const {
        return (*left_)() / (*right_)();
    }
};

class evaluator
{
public:
    const expression* parse(const char*);                   ///< Parse an expression

private:
    expression* parse_number(const char*&);                 ///< Parse numeric constants
    expression* parse_atom(const char*&);                   ///< Parse nested expression
    expression* parse_summands(const char*&);               ///< Parse '+' and '-' operations
    expression* parse_factors(const char*&);                ///< Parse '*' and '/' operations
};

expression* evaluator::parse_number(const char*& s)
{
    assert("Sanity check" && s && std::isdigit(*s));
    number_token* nt = new number_token(0);
    // Convert number...
    while (*s && std::isdigit(*s))
    {
        nt->value_ = nt->value_ * 10 + *s++ - '0';
    }
    return nt;
}

expression* evaluator::parse_atom(const char*& s)
{
    assert("Sanity check" && s);
    if (*s == 0)
    {
        throw std::runtime_error("Atom parse error: unexpected EOS");
    }
    else if (*s == '(')
    {
        s++;
        expression* atom = parse_summands(s);
        if (*s == ')')
        {
            s++;
            return atom;
        }
        throw std::runtime_error("Atom parse error: unbalanced brackets");

    }
    else if (std::isdigit(*s))
    {
        expression* atom = parse_number(s);
        return atom;
    }
    throw std::runtime_error("Atom parse error: unexpected char");
}

expression* evaluator::parse_factors(const char*& s)
{
    assert("Sanity check" && s);
    expression* left = parse_atom(s);
    while (*s)
    {
        if (*s == '*')
        {
            s++;
            expression* right = parse_atom(s);
            left = new mul(left, right);
            continue;
        }
        else if (*s == '/')
        {
            s++;
            expression* right = parse_atom(s);
            left = new div(left, right);
            continue;
        }
        return left;
    }
    return left;
}

expression* evaluator::parse_summands(const char*& s)
{
    assert("Sanity check" && s);
    expression* left = parse_factors(s);
    while (*s)
    {
        if (*s == '+')
        {
            s++;
            expression* right = parse_factors(s);
            left = new plus(left, right);
            continue;
        }
        else if (*s == '-')
        {
            s++;
            expression* right = parse_factors(s);
            left = new minus(left, right);
            continue;
        }
        return left;
    }
    return left;
}

const expression* evaluator::parse(const char* s)
{
    return parse_summands(s);
}


int evaluate(const char* const e)
{
    evaluator ev;
    const expression* const ex = ev.parse(e);
    assert("Sanity check" && ex);
    return (*ex)();
}

// s= evaluate(“(4+3)*2/5” );
// s= evaluate(“((12+9)/2)*5”);
int main()
{
    {
        int a = evaluate("(4+3)*2/5");
        assert("Unexpected result" && a == 2);
        std::cout << "\"(4+3)*2/5\" = " << a << std::endl;
    }
    {
        int a = evaluate("((12+9)/2)*5");
        assert("Unexpected result" && a == 50);
        std::cout << "\"((12+9)/2)*5\" = " << a << std::endl;
    }
    return 0;
}
