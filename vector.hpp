// Created by Matthew Gimlin on July 19, 2022.

// This class is a vector data structure. It also includes
// an internal iterator class for traversing the data.

#pragma once

template<typename T>
class Vector
{
public:
    using SizeType       = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using ValueType      = T;
    using Pointer        = T*;
    using ConstPointer   = const T*;
    using Reference      = T&;
    using ConstReference = const T&;

private:
    Pointer m_Data;
    SizeType m_Size;
    SizeType m_Capacity;

    static const SizeType INITIAL_CAPACITY = 1;

public:
    class Iterator;

    /**
     * @brief Default constructor.
     * 
     * Creates a vector with INITIAL_CAPACITY number of uninitialized elements.
     */
    Vector()
        : m_Data( Allocate(INITIAL_CAPACITY) ),
          m_Size(0),
          m_Capacity(INITIAL_CAPACITY)
    { }

    /**
     * @brief Fill constructor.
     * @param count The capacity of the new vector.
     * 
     * Creates a vector with count number of uninitialized elements.
     */
    explicit Vector(SizeType count)
        : m_Data( Allocate(count) ),
          m_Size(0),
          m_Capacity(count)
    { }

    /**
     * @brief Fill constructor.
     * @param count The capacity of the new vector.
     * @param value The value to initialize all elements with.
     * 
     * Creates a vector with count number of elements initialized with value.
     */
    Vector(SizeType count, ConstReference value)
        : m_Data( Allocate(count) ),
          m_Size(count),
          m_Capacity(count)
    {
        for (SizeType i = 0; i < count; ++i)
            new (&m_Data[i]) ValueType(value);
    }

    /**
     * @brief Copy constructor.
     * @param other The other vector to copy.
     * 
     * Creates a vector by copying the contents of the other.
     */
    Vector(const Vector& other)
    {
        m_Data = Allocate(other.m_Capacity);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;

        for (SizeType i = 0; i < other.m_Size; ++i)
            new (&m_Data[i]) ValueType(other[i]);
    }

    /**
     * @brief Move constructor.
     * @param other The other vector to move.
     * 
     * Creates a vector by moving the contents of the other.
     */
    Vector(Vector&& other)
        : m_Data(other.m_Data),
          m_Size(other.m_Size),
          m_Capacity(other.m_Capacity)
    {
        other.m_Data = Allocate(INITIAL_CAPACITY);
        other.m_Size = 0;
        other.m_Capacity = INITIAL_CAPACITY;
    }

    ~Vector()
    {
        Clear();
        Deallocate();
    }

    /**
     * @brief Copy assignment operator.
     * @param other The vector to copy.
     * 
     * Recreates the vector by copying the contents of the other.
     */
    Vector& operator=(const Vector& other)
    {
        if (this == &other) return *this;

        Clear();
        Deallocate();

        m_Data = Allocate(other.m_Capacity);
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;

        for (SizeType i = 0; i < other.m_Size; ++i)
            new (&m_Data[i]) ValueType(other[i]);
        
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other The vector to move.
     * 
     * Recreates the vector by copying the contents of the other.
     */
    Vector& operator=(Vector&& other)
    {
        if (this == &other) return *this;

        Clear();
        Deallocate();

        m_Data = other.m_Data;
        m_Size = other.m_Size;
        m_Capacity = other.m_Capacity;

        other.m_Data = Allocate(INITIAL_CAPACITY);
        other.m_Size = 0;
        other.m_Capacity = INITIAL_CAPACITY;

        return *this;
    }

    // private member data getters
    SizeType Size() const { return m_Size; }
    bool Empty() const { return m_Size == 0; }
    SizeType Capacity() const { return m_Capacity; }

    // iterators
    Iterator Begin() const { return Iterator(m_Data); }
    Iterator End() const { return Iterator(m_Data + m_Size); }

    // access elements
    Reference operator[](SizeType index) { return m_Data[index]; }
    ConstReference operator[](SizeType index) const { return m_Data[index]; }
    Reference Front() { return *m_Data; }
    ConstReference Front() const { return *m_Data; }
    Reference Back() { return *(m_Data + m_Size - 1); }
    ConstReference Back() const { return *(m_Data + m_Size - 1); }
    
    /**
     * @brief Access elements.
     * @param index The index of the element to access.
     * 
     * Access an element with bounds checking.
     */
    Reference At(SizeType index)
    {
        if (index >= m_Size)
            throw std::out_of_range("The index is outside the vector.");

        return m_Data[index];
    }

    /**
     * @brief Access elements.
     * @param index The index of the element to access.
     * 
     * Access an element with bounds checking.
     */
    ConstReference At(SizeType index) const
    {
        if (index >= m_Size)
            throw std::out_of_range("The index is outside the vector.");

        return m_Data[index];
    }

    /**
     * @brief Deletes all elements of the vector.
     * 
     * Destructs all elements of the vector. Does not deallocate the internal array.
     */
    void Clear()
    {
        for (SizeType i = 0; i < m_Size; ++i)
            m_Data[i].~ValueType();
        
        m_Size = 0;
    }

    /**
     * @brief Inserts an element at the end.
     * @param value The element to push.
     * 
     * Copies value into a new element at the end of the vector.
     */
    void PushBack(const ValueType& value)
    {
        if (m_Size == m_Capacity)
            Reallocate(m_Capacity * 2);
        
        new (&m_Data[m_Size]) ValueType(value);
        ++m_Size;
    }

    /**
     * @brief Inserts an element at the end.
     * @param value The element to push.
     * 
     * Moves value into a new element at the end of the vector.
     */
    void PushBack(ValueType&& value)
    {
        if (m_Size == m_Capacity)
            Reallocate(m_Capacity * 2);
        
        new (&m_Data[m_Size]) ValueType( std::move(value) );
        ++m_Size;
    }

    /**
     * @brief Removes the last element.
     * 
     * Destructs the last element in the vector.
     */
    void PopBack()
    {
        if (m_Size == 0) return;

        --m_Size;
        m_Data[m_Size].~ValueType();
    }

    /**
     * @brief Inserts an element.
     * @param position An iterator at the position to insert.
     * @param value The value to insert.
     * 
     * Inserts a new element at a position, and moves back all elements after it.
     */
    void Insert(Iterator position, ConstReference value)
    {
        DifferenceType index = position - Begin();

        if (m_Size == m_Capacity)
            Reallocate(m_Capacity * 2);

        // move elements to make space for the new one
        for (SizeType i = m_Size; i > index; --i)
        {
            std::cout << m_Data[i] << std::endl;
            new (&m_Data[i]) ValueType( std::move(m_Data[i - 1]) );
        }
        
        m_Data[index] = value;
        ++m_Size;
    }

    /**
     * @brief Inserts an element.
     * @param position An iterator at the position to insert.
     * @param value The value to insert.
     * 
     * Moves all elements after the position, and inserts a new element in the open spot.
     */
    void Insert(Iterator position, ValueType&& value)
    {
        DifferenceType index = position - Begin();

        if (m_Size == m_Capacity)
            Reallocate(m_Capacity * 2);

        // move elements to make space for the new one
        for (SizeType i = m_Size; i > index; --i)
            new (&m_Data[i]) ValueType( std::move(m_Data[i - 1]) );
        
        m_Data[index] = std::move(value);
        ++m_Size;
    }

    /**
     * @brief Removes an element.
     * @param position At iterator at the element to erase.
     * 
     * Removes an element, and moves up all the elements that came after.
     */
    void Erase(Iterator position)
    {
        DifferenceType index = position - Begin();

        for (SizeType i = index; i < (m_Size - 1); ++i)
            new (&m_Data[i]) ValueType( std::move(m_Data[i + 1]) );
        
        --m_Size;
    }

    class Iterator
    {
    private:
        Pointer m_Ptr;
    
    public:
        // default constructor
        Iterator()
            : m_Ptr(nullptr)
        { }

        // parameterized constructor
        explicit Iterator(Pointer ptr)
            : m_Ptr(ptr)
        { }

        // copy assignment operator
        Iterator& operator=(const Iterator& other)
        {
            if (this == &other) return *this;

            m_Ptr = other.m_Ptr;
            return *this;
        }

        Reference operator*() const { return *m_Ptr; }
        Pointer operator->() const { return m_Ptr; }

        Iterator& operator++()
        {
            ++m_Ptr;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator copy = *this;
            ++(*this);
            return *this;
        }

        Iterator& operator--()
        {
            --m_Ptr;
            return *this;
        }

        Iterator operator--(int)
        {
            Iterator copy = *this;
            ++(*this);
            return *this;
        }

        Iterator& operator+=(DifferenceType offset)
        {
            m_Ptr += offset;
            return *this;
        }

        Iterator operator+(DifferenceType offset) const
        {
            Iterator copy = *this;
            return copy += offset;
        }

        Iterator& operator-=(DifferenceType offset)
        {
            m_Ptr -= offset;
            return *this;
        }

        Iterator operator-(DifferenceType offset) const
        {
            Iterator copy = *this;
            return copy -= offset;
        }

        DifferenceType operator-(const Iterator& other) const
        {
            return m_Ptr - other.m_Ptr;
        }

        Reference operator[](DifferenceType offset) const
        {
            return *(*this + offset);
        }

        bool operator==(const Iterator& other) const
        {
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const Iterator& other) const
        {
            return m_Ptr != other.m_Ptr;
        }

        bool operator<(const Iterator& other) const
        {
            return m_Ptr < other.m_Ptr;
        }

        bool operator<=(const Iterator& other) const
        {
            return m_Ptr <= other.m_Ptr;
        }

        bool operator>(const Iterator& other) const
        {
            return m_Ptr > other.m_Ptr;
        }

        bool operator>=(const Iterator& other) const
        {
            return m_Ptr >= other.m_Ptr;
        }
    };

private:
    // allocates a new internal array
    Pointer Allocate(SizeType capacity)
    {
        return (T*) ::operator new(capacity * sizeof(T));
    }

    // deallocates the internal array
    void Deallocate()
    {
        ::operator delete(m_Data, m_Capacity * sizeof(T));
    }

    /**
     * @brief Reallocates the internal array.
     * @param newCapacity The capacity of the new internal array.
     * 
     * Creates a new internal array, and deletes the old one.
     */
    void Reallocate(SizeType newCapacity)
    {
        // the new array should not be smaller than the old one
        if (newCapacity < m_Size)
            throw std::invalid_argument("The new capacity is too small.");
        
        Pointer newArray = Allocate(newCapacity);

        // copy elements into the unitialized array
        // destruct the elements in the old array
        for (SizeType i = 0; i < m_Size; ++i)
        {
            new (&newArray[i]) ValueType( std::move(m_Data[i]) );
            m_Data[i].~ValueType();
        }

        Deallocate();
        m_Data = newArray;
        m_Capacity = newCapacity;
    }
};
