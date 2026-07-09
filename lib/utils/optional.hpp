
#ifndef OPTIONAL_HPP
# define OPTIONAL_HPP

template <typename T>
class Optional {
private:
    bool _has;
    T _value;
public:
    Optional() : _has(false), _value() {}
    void set(const T& value) { _value = value; _has = true; }
    bool isSet() const { return _has; }
    const T& get() const { return _value; }
};

#endif