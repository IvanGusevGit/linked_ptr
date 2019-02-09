//
// Created by ivan on 08.02.19.
//

#ifndef LINKED_PTR_LINKED_PTR_H
#define LINKED_PTR_LINKED_PTR_H


namespace smart_ptr::details {
    template<typename T>
    struct default_deleter {
        typedef T value_type;

        void operator()(
                const value_type *p) const
        { delete const_cast<value_type *>(p); }
    };

    template<>
    struct default_deleter<void> {
        typedef void value_type;

        void operator()(
                const void *p) const { delete[] (char *) p; }
    };

    template<>
    struct default_deleter<const void> {
        typedef void value_type;

        void operator()(
                const void *p) const { delete[] (char *) p; }
    };


    struct linked_ptr_base {
        mutable linked_ptr_base *previous;
        mutable linked_ptr_base *next;
    };
}

namespace smart_ptr {
    template<typename T, typename Deleter = smart_ptr::details::default_deleter<T> >
    class linked_ptr : public smart_ptr::details::linked_ptr_base {

    private:

        typedef linked_ptr<T> this_type;

        typedef Deleter deleter_type;


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


        template<typename U, typename D>
        linked_ptr(const linked_ptr<U, D> &other) : stored_pointer(other.stored_pointer) {
            if (stored_pointer)
                link(other);
            else
                previous = next = this;
        }


        ~linked_ptr() {
            reset();
        }


        linked_ptr &operator=(const linked_ptr &linkedPtr) {
            if (linkedPtr.stored_pointer != stored_pointer) {
                reset(linkedPtr.stored_pointer);
                if (linkedPtr.stored_pointer)
                    link(linkedPtr);
            }
            return *this;
        }

        template<typename U, typename D>
        linked_ptr &operator=(const linked_ptr<U, D> &linkedPtr) {
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
                    deleter_type del;
                    del(stored_pointer);
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


        void swap(linked_ptr &other) {
            std::swap(previous, other.previous);
            std::swap(next, other.next);
            std::swap(stored_pointer, other.stored_pointer);
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
            int useCount(1);

            for (const linked_ptr_base *pCurrent = static_cast<const linked_ptr_base *>(this);
                 pCurrent->next != static_cast<const linked_ptr_base *>(this); pCurrent = pCurrent->next)
                ++useCount;

            return useCount;
        }

        bool unique() const {
            return (next == static_cast<const linked_ptr_base *>(this));
        }


        typedef T *(this_type::*bool_)() const;

        operator bool_() const {
            if (stored_pointer)
                return &this_type::get;
            return nullptr;
        }

        bool operator!() {
            return (stored_pointer == nullptr);
        }

    private:

        T *stored_pointer;

        template<typename U, typename D>
        void link(const linked_ptr<U, D> &linkedPtr) {
            // reset state only
            next = linkedPtr.next;
            next->previous = this;
            previous = const_cast<linked_ptr<U, D> *>(&linkedPtr);
            linkedPtr.next = this;
        }

    };
}



#endif //LINKED_PTR_LINKED_PTR_H
