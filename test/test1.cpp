#include <type_traits>
#include <concepts>

template <typename A>
concept MyConcept = requires(A a, bool b)
{
    { a.one() } ->std::same_as<bool>;
    a.two();
    a.three(b);
};

struct SomeType {
    bool one()
    {
        return true;
    }
    void two() {}
    void three(bool) {}
};

bool foo(MyConcept auto a)
{
    return a.one();
}

void bar()
{
    foo(SomeType());
}

int main()
{
    return 0;
}