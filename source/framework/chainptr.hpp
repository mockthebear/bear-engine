#pragma once

/**
    This class consist in an kind of shared_ptr
    It has 3 pointers.
        The first pointer is the resource you might store.
        The second one, points to this resource
        The third points to the second one.
    The class only holds the third pointer. It is because, if you change the resource location (realloc), or just
    simply want to assing an new value and notify all the others, you only have to change the second pointer.
    All object that are direct copy from the original dont will be needed to notify them. They will change by
    the nature of the class.
*/
template <typename T> class chain_ptr{
    public:
        chain_ptr(){
            myHolder = nullptr;
        }
        chain_ptr(T ***addr){
            myHolder = addr;
        };

        void destroy(){
            if (myHolder){
                //Maybe unsafe
                delete [] myHolder[0];
                delete [] myHolder;
                myHolder = nullptr;
            }
        }

        static chain_ptr<T> make(T *TrueData){
            if (TrueData == nullptr){
                return chain_ptr();
            }
            T ***LocalHolder = new T**[1];
            T **MainlyHolder = new T*[1];
            LocalHolder[0] = MainlyHolder;
            MainlyHolder[0] = TrueData;
            return chain_ptr(LocalHolder);
        }

        bool operator!(){
            return !myHolder;
        }
        T *get(){
            if (myHolder == nullptr || (*myHolder) == nullptr){
                return nullptr;
            }
            return *(myHolder[0]);  //This will return the middle pointer object
        }
        void reset(T *data){
            if (myHolder)
                myHolder[0][0] = data;
        }
    private:
        T*** myHolder;
};
