#ifndef MY_VECTOR_HPP
#define MY_VECTOR_HPP

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <initializer_list>
#include <exception>
#include <cassert>

class vector_base
{
    protected:
        std::string* m_begin,
                   * m_last_alloc,
                   * m_end;
    public:
        constexpr vector_base()
        : m_begin(nullptr), m_last_alloc(nullptr), m_end(nullptr) {}

        constexpr vector_base(std::string* b, std::string* l, std::string* e)
        :  m_begin(b), m_last_alloc(l), m_end(e) {}
};

class my_vector : public vector_base
{
    friend bool operator== (const my_vector&, const my_vector&);
    friend bool operator!= (const my_vector&, const my_vector&);
    friend bool operator<  (const my_vector&, const my_vector&);
    friend bool operator<= (const my_vector&, const my_vector&);
    friend bool operator>  (const my_vector&, const my_vector&);
    friend bool operator>= (const my_vector&, const my_vector&);
    
    friend void swap(my_vector&, my_vector&);

    public:
        using size_type       = size_t;
        using value_type      = std::string;
        using difference      = int32_t;
        using reference       = value_type&;
        using const_reference = const value_type&;

        my_vector()
        : vector_base() {}

        inline my_vector(std::initializer_list<std::string>);
        inline my_vector(const my_vector&);                  
        inline my_vector(my_vector&&) noexcept;

        inline my_vector& operator= (const my_vector&);
        inline my_vector& operator= (my_vector&&);

        ~my_vector() {free();}

        const std::string& operator[] (const size_type) const;
        std::string& operator[] (const size_type);

        inline void push_back(const std::string&);
        inline void push_back(std::string&&);

        inline void resize(const uint32_t, const std::string&);

        inline void fill(const std::string&);
        inline void fill_n(const std::string&, const uint32_t);
        inline void fill_n_in(const std::string&, const uint32_t, std::string*);

        bool      empty()    const noexcept {return (m_begin == m_last_alloc) ? true : false;} 
        size_type size()     const noexcept {return m_last_alloc - m_begin;}
        size_type capacity() const noexcept {return m_end - m_begin;}

        std::allocator<std::string> get_allocator() const noexcept 
        {return alloc;}

        inline reference at(const size_type);
        inline const_reference at(const size_type) const;

        inline reference front();
        inline reference back();

        inline const_reference front() const;
        inline const_reference back() const;

        inline std::string* data() noexcept;
        inline const std::string* data() const noexcept;

        std::string* begin() noexcept {return m_begin;}
        std::string* end()   noexcept {return m_last_alloc;}

        const std::string* begin() const noexcept {return m_begin;}
        const std::string* end()   const noexcept {return m_last_alloc;}

        const std::string* cbegin() const noexcept {return m_begin;}
        const std::string* cend()   const noexcept {return m_last_alloc;}

        inline void clear() noexcept;

        inline std::string* insert(std::string*, const_reference);
        inline std::string* insert(std::string*, const std::string*, const std::string*);// insert a range
        inline std::string* insert(std::string*, std::initializer_list<std::string>);// insert a list

    private:
        static std::allocator<std::string> alloc;

        inline void check_memory();
        inline void is_interv_valid(size_type, const char*) const;
        inline void realloc_memory();

        inline void free();
        inline void free(std::string*);

        inline std::pair<std::string*, std::string*>
        alloc_n_copy(const std::string*, const std::string*);

        inline void swap(std::string*, std::string*);
};

// member functions private
std::allocator<std::string> my_vector::alloc;

inline void my_vector::check_memory()
{
    if (size() == capacity())
        realloc_memory();
}

inline void my_vector::realloc_memory()
{
    auto sz = (size() ? size() * 2 : 1);

    auto new_point = alloc.allocate(sz);

    auto track_newp = new_point;
    auto actual_mem = m_begin;

    for (uint32_t count = 0U; count != size(); ++count)
        alloc.construct(track_newp++, std::move(*actual_mem++));

    free();
    
    m_begin      = new_point;
    m_last_alloc = track_newp;
    m_end        = new_point + sz;
}

inline void my_vector::free()
{
    if (m_begin)
    {
        auto point = m_last_alloc;

        for(; point != m_begin; alloc.destroy(--point));

        alloc.deallocate(m_begin, capacity());

        m_begin = m_last_alloc = m_end = nullptr;
    }
}

inline void my_vector::free(std::string* point)
{
    if (m_begin)
    {
        for(; m_last_alloc != point; alloc.destroy(m_last_alloc--));
    }
}

std::pair<std::string*, std::string*> 
my_vector::alloc_n_copy(const std::string* b, const std::string* e)
{
    auto init = alloc.allocate(e - b);

    return {init, std::uninitialized_copy(b, e, init)};
}

inline void my_vector::is_interv_valid(size_type index, const char* msg) const
{
    if (index < 0U || index >= size())
        throw msg;
}

inline void my_vector::swap(std::string* point_1, std::string* point_2)
{
    auto aux = *point_1;
    *point_1 = *point_2;
    *point_2 = aux;
}

// member functions public

my_vector::my_vector(std::initializer_list<std::string> il)
: vector_base()
{
    auto point_memory = alloc.allocate(il.size());

    m_begin = m_last_alloc = point_memory;

    for (auto str : il)
        alloc.construct(m_last_alloc++, std::move(str));
    
    m_end = m_last_alloc;
}

my_vector::my_vector(const my_vector& obj)
: vector_base()
{
    auto pair_points = alloc_n_copy(obj.cbegin(), obj.cend());

    m_begin = pair_points.first;
    m_end   = m_last_alloc = pair_points.second;
}

my_vector::my_vector(my_vector&& t_obj) noexcept
: vector_base(t_obj.m_begin, t_obj.m_last_alloc, t_obj.m_end)
{
    t_obj.m_begin = t_obj.m_last_alloc = t_obj.m_end = nullptr;
}

my_vector& my_vector::operator= (const my_vector& obj)
{
    if (this != &obj)
    {
        auto temp_pair = alloc_n_copy(obj.cbegin(), obj.cend());

        free();

        m_begin = temp_pair.first;
        m_end   = m_last_alloc = temp_pair.second;
    }

    return *this;
}

my_vector& my_vector::operator= (my_vector&& t_obj)
{
    if (this != &t_obj)
    {
        free();

        m_begin      = t_obj.m_begin;
        m_last_alloc = t_obj.m_last_alloc;
        m_end        = t_obj.m_end;

        t_obj.m_begin = t_obj.m_last_alloc = t_obj.m_end = nullptr;
    }

    return *this;
}

std::string& my_vector::operator[] (const size_type index)
{
    is_interv_valid(index, "out of range (\"my_vector::operator[]\")");

    return *(m_begin + index);
}

const std::string& my_vector::operator[] (const size_type index) const
{
    is_interv_valid(index, "out of range (\"my_vector::operator[]\")");

    return *(m_begin + index);
}

void my_vector::push_back(const std::string& str)
{
    check_memory();

    alloc.construct(m_last_alloc++, str);
}

void my_vector::push_back(std::string&& t_str)
{
    check_memory();

    alloc.construct(m_last_alloc++, std::move(t_str));
}

void my_vector::resize(const uint32_t new_size, const std::string& str = "")
{
    try
    {
        if (new_size < size())
        {
            free(m_begin + new_size);

            if (str != "")
                fill(str);
        }
        else if (new_size == size() && str != "")
        {
            fill(str);
        }
        else if (new_size > size() && new_size <= capacity())
        {
            for(;m_last_alloc != m_begin + new_size; ++m_last_alloc)
                *m_last_alloc = str;
        }
        else if (new_size > capacity())
        {
            auto new_memory = alloc.allocate(new_size);

            if (!(new_memory))
                throw std::bad_alloc();

            auto track = new_memory;
            auto elem  = m_begin;

            for(uint32_t count = 0U; count != size(); ++count)
                alloc.construct(track++, std::move(*elem++));
        
            free();

            m_begin      = new_memory;
            m_last_alloc = track;
            m_end        = new_memory + new_size;  

            for(;m_last_alloc != (m_begin + new_size); ++m_last_alloc)
                alloc.construct(m_last_alloc, str);
        }
    }
    catch(std::bad_alloc& exception)
    {
        std::cerr << "[ERROR] Was not able to allocate memory"<< exception.what() 
                  << " " << "(\"my_vector::resize\")" << '\n';

        free();
        exit(EXIT_FAILURE);
    }
}

my_vector::reference my_vector::at (const size_type index)
{   
    try
    {
        if (index < 0 || index >= size())
            throw "[WARNINGS] Out of range (\"my_vector::at\")";
        
        return *(m_begin + index);
    }
    catch (const char*  msg)
    {
        std::cerr << msg;
        free();
        exit(EXIT_FAILURE);
    }
}

my_vector::const_reference my_vector::at (const size_type index) const
{
    try
    {
        if (index < 0 || index >= size())
            throw "[WARNINGS] Out of range (\"my_vector::at\")";
        
        return *(m_begin + index);
    }
    catch (const char*  msg)
    {
        std::cerr << msg;
        auto temp = std::move(*this);
        exit(EXIT_FAILURE);
    }
}

my_vector::reference my_vector::front()
{
    return *m_begin;
}

my_vector::reference my_vector::back()
{
    return *(m_last_alloc - 1);
}

my_vector::const_reference my_vector::front() const
{
    return *m_begin;
}

my_vector::const_reference my_vector::back() const
{
    return *(m_last_alloc - 1);
}

std::string* my_vector::data() noexcept
{
    return m_begin;
}

const std::string* my_vector::data() const noexcept
{
    return m_begin;
}

void my_vector::fill(const std::string& str)
{
    auto track = m_begin;

    for(int32_t count = 0; count < size(); ++count, ++track)
        *track = str;
}

void my_vector::fill_n(const std::string& str, const uint32_t amount)
{
    try
    {
        if (amount > (m_last_alloc - m_begin))
        {
            throw "[ERROR] The quantity is larger than the size of the container (\"my_vector::fill_n\")";
        }
        else
        {
            auto track = m_begin;
            for(; track != m_begin + amount; ++track)
                *track = str;
        }
    }
    catch(const char* msg)
    {
        std::cerr << msg << '\n';

        free();
        exit(EXIT_FAILURE);
    }
}

void my_vector::fill_n_in(const std::string& str, 
                          const uint32_t amount, 
                          std::string* point)
{
    try{
        if (amount > (m_last_alloc - point))
        {
            throw "[ERROR] The quantity is larger than the size of the container (\"my_vector::fill_n_in\")";
        }
        else if (point >= m_begin && point <= m_last_alloc)
        {
            throw "[ERROR] The pointer is outside the interval of the container (\"my_vector::fill_n_in\")";
        }
        else
        {
            auto track = point;
            
            for(; track != point + amount; ++track)
                *track = str;
        }
    }
    catch(const char* msg)
    {
        std::cerr << msg;

        free();
        exit(EXIT_FAILURE);
    }
}

void my_vector::clear() noexcept
{
    for(; m_last_alloc != m_begin; alloc.destroy(--m_last_alloc));
}

std::string* my_vector::insert(std::string* pos, const_reference value)
{
    try
    {
        if (pos < m_begin || pos > m_last_alloc)
            throw "[ERROR] Position is out of the container interval(\"my_vector::insert\")";

        if (pos == m_last_alloc)
        {
            push_back(value);
            return m_last_alloc - 1;
        }
        else
        {
            auto index = pos - m_begin;

            push_back(value);

            auto point = m_last_alloc - 1;

            for(; (point - m_begin) != index; --point)
                swap(point - 1, point);

            return point;
        }
    }
    catch(const char* excp)
    {
        std::cerr << '\n' << excp << '\n';

        free();
        exit(EXIT_FAILURE);
    }
}

std::string* my_vector::insert(std::string* pos, const std::string* b, const std::string* e)
{
    try
    {
        if (pos < m_begin || pos > m_last_alloc)
            throw "[ERROR] Position is out of the container interval(\"my_vector::insert\")";
        
        if (b == e)
            return nullptr;

        if ((capacity() - size()) > e - b)
        {
            if (pos == m_last_alloc)
            {
                for(auto track = b; track != e; ++track)
                    push_back(*track);
                
                return pos;
            }
            else
            {
                for(auto count = 0U; count < (e - b); ++count)
                    push_back("");
                
                auto regress = m_last_alloc - 1;

                for(; regress - (e - b) != pos - 1; --regress)
                    swap(regress - (e - b), regress);

                auto aux = (regress - (e - b)) + 1;

                for(auto count = 0U; count < (e - b); ++count)
                    *(aux + count) = *(b + count);

                return aux;
            }
        }
    }
    catch(const char* excp)
    {
        std::cerr << excp << '\n';

        free();
        exit(EXIT_FAILURE);
    }

    return nullptr;
}

//////// Non-member functions //////////////

inline bool operator == (const my_vector& obj1, const my_vector& obj2)
{
    if(obj1.empty() && obj2.empty())
        return true;

    if (obj1.size() != obj2.size())
        return false;
    else
        for(auto count = 0U; count != obj1.size(); ++count)
            if(obj1[count] != obj2[count])
                return false;
    
    return true;
}

inline bool operator != (const my_vector& obj1, const my_vector& obj2)
{
    return !(obj1 == obj2);
}

inline bool operator < (const my_vector& obj1, const my_vector& obj2)
{
    if(obj1.empty() && obj2.empty())
        return false;
        
    if (obj1.size() < obj2.size())
    {
        return true;
    }
    else if (obj1.size() > obj2.size())
    {
        return false;
    }
    else
    {
        for(my_vector::size_type count = 0U; count != obj1.size(); ++count)
            if(obj1[count] < obj2[count])
                return true;
    }

    return false;
}

inline bool operator <= (const my_vector& obj1, const my_vector& obj2)
{
    if (obj1.empty() && obj2.empty())
        return true;
    
    if (obj1.size() <= obj2.size())
    {
        if (obj1.size() == obj2.size())
        {
            for(my_vector::size_type count = 0U; count < obj1.size(); ++count)
                if (obj1[count] > obj2[count])
                    return false;

            return true;
        }
        else
            return true;
    }
    else
        return false;
}

inline bool operator > (const my_vector& obj1, const my_vector& obj2)
{
    if(obj1.empty() && obj2.empty())
        return false;
        
    if (obj1.size() > obj2.size())
    {
        return true;
    }
    else if (obj1.size() < obj2.size())
    {
        return false;
    }
    else
    {
        for(my_vector::size_type count = 0U; count != obj1.size(); ++count)
            if(obj1[count] > obj2[count])
                return true;
    }

    return false;
}

inline bool operator>= (const my_vector& obj1, const my_vector& obj2)
{
    if(obj1.empty() && obj2.empty())
        return true;
    
    if (obj1.size() >= obj2.size())
    {
        if (obj1.size() == obj2.size())
        {
            for(my_vector::size_type count = 0; count < obj1.size(); ++count)
                if (obj1[count] < obj2[count])
                    return false;
            
            return true;
        }
        else
            return true;
    }
    else
        return false;
}

inline void swap(my_vector& obj1, my_vector& obj2)
{
    using std::swap;

    swap(obj1.m_begin, obj2.m_begin);
    swap(obj1.m_last_alloc, obj2.m_last_alloc);
    swap(obj1.m_end, obj2.m_end);
}

#endif
