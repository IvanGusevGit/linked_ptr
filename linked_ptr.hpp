//
// Created by ivan on 08.02.19.
//

#ifndef LINKED_PTR_LINKED_PTR_H
#define LINKED_PTR_LINKED_PTR_H

#include <utility>

namespace smart_ptr::details {
    struct linked_ptr_base {
        mutable linked_ptr_base *previous;
        mutable linked_ptr_base *next;
    };
}

namespace smart_ptr {
    template <typename T>
    class linked_ptr : public smart_ptr::details::linked_ptr_base {

    protected:

        template <typename U> friend class linked_ptr;

        typedef linked_ptr<T> this_type;
        T *stored_pointer;

        template<typename U>
        void link(const linked_ptr<U> &other) {
            next = other.next;
            next->previous = this;
            previous = const_cast<linked_ptr<U> *>(&other);
            other.next = this;
        }

    public:

        linked_ptr() : stored_pointer(nullptr) {
            previous = next = this;
        }

        template<typename U>
        explicit linked_ptr(U *p) : stored_pointer(p) {
            previous = next = this;
        }


        linked_ptr(const linked_ptr &other) : stored_pointer(other.stored_pointer) {
            if (stored_pointer)
                link(other);
            else
                previous = next = this;
        }


        template<typename U>
        linked_ptr(const linked_ptr<U> &other) : stored_pointer(other.stored_pointer) {
            if (stored_pointer)
                link(other);
            else {
                previous = next = this;
            }
        }


        ~linked_ptr() {
            reset();
        }


        linked_ptr &operator=(const linked_ptr &linkedPtr) {
            if (linkedPtr.stored_pointer != stored_pointer) {
                reset(linkedPtr.stored_pointer);
                if (linkedPtr.stored_pointer) {
                    link(linkedPtr);
                }
            }
            return *this;
        }

        template<typename U>
        linked_ptr &operator=(const linked_ptr<U> &linkedPtr) {
            if (linkedPtr.stored_pointer != stored_pointer) {
                reset(linkedPtr.stored_pointer);
                if (linkedPtr.stored_pointer)
                    link(linkedPtr);
            }
            return *this;
        }

        template<typename U>
        linked_ptr &operator=(U *pValue) {
            reset(pValue);
            return *this;
        }


        template<typename U>
        void reset(U *pValue) {
            if (pValue != stored_pointer) {
                if (unique()) {
                    delete stored_pointer;
                } else {
                    previous->next = next;
                    next->previous = previous;
                    previous = next = this;
                }
                stored_pointer = pValue;
            }
        }

        void reset() {
            reset((T *) nullptr);
        }


        template<typename U>
        void swap(linked_ptr<U> &other) {
            if (stored_pointer != other.stored_pointer) {
                if (unique() && other.unique()) {
                    swap_uniques(this, &other);
                } else if (unique() && !other.unique()) {
                    swap_unique_non_unique(this, &other);
                } else if (!unique() && other.unique()) {
                    swap_unique_non_unique(&other, this);
                } else if (!unique() && !other.unique()) {
                    swap_non_uniques(this, &other);
                }
            }
        }


        T &operator*() const {
            return *stored_pointer;
        }

        T *operator->() const {
            return stored_pointer;
        }


        T *get() const {
            return stored_pointer;
        }


        int use_count() const {
            int counter = 1;
            for (auto *current = static_cast<const linked_ptr_base *>(this);
                 current->next != static_cast<const linked_ptr_base *>(this); current = current->next)
                ++counter;
            return counter;
        }

        bool unique() const {
            return (next == static_cast<const linked_ptr_base *>(this));
        }


        operator bool() const {
            return static_cast<bool>(stored_pointer);
        }

        bool operator!() {
            return stored_pointer == nullptr;
        }

    private:

        template<typename U, typename V>
        void swap_unique_non_unique(linked_ptr<U> *first, linked_ptr<V> *second) {
            std::swap(first->stored_pointer, second->stored_pointer);
            auto next_nonunique = second->next;
            auto prev_nonunique = second->previous;
            first->next = next_nonunique;
            first->previous = prev_nonunique;
            next_nonunique->previous = first;
            prev_nonunique->next = first;
            second->next = second->previous = second;
        }

        template<typename U, typename V>
        void swap_uniques(linked_ptr<U> *first, linked_ptr<V> *second) {
            std::swap(first->stored_pointer, second->stored_pointer);
        }

        template<typename U, typename V>
        void swap_non_uniques(linked_ptr<U> *first, linked_ptr<V> *second) {
            std::swap(first->stored_pointer, second->stored_pointer);
            std::swap(first->next->previous, second->next->previous);
            std::swap(first->previous->next, second->previous->next);
            std::swap(first->next, second->next);
            std::swap(first->previous, second->previous);
        }

    };

    template<typename T, typename U>
    inline bool operator==(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() == b.get());
    }

    template<typename T, typename U>
    inline bool operator!=(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() != b.get());
    }

    template<typename T, typename U>
    inline bool operator<(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() < b.get());
    }

    template<typename T, typename U>
    inline bool operator<=(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() <= b.get());
    }

    template<typename T, typename U>
    inline bool operator>(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() > b.get());
    }

    template<typename T, typename U>
    inline bool operator>=(const linked_ptr<T> &a, const linked_ptr<U> &b) {
        return (a.get() >= b.get());
    }

    template<typename U>
    linked_ptr<U> make_linked(U *ptr) {
        return linked_ptr<U>(ptr);
    }
}


#endif //LINKED_PTR_LINKED_PTR_H
