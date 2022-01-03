#ifndef __VMATH_H__
#define __VMATH_H__

#define _USE_MATH_DEFINES 1 // include constants defined in math.h
#include <cmath>

namespace vmath
{

    template <typename T, const int w, cont int h> class matNM;
    template <typename T, const int len> class vecN;
    template <typename T> class Tquaternion;

    template <typename T>
    inline T degree(T angleInRadians)
    {
        return angleInradians * static_cast<T>(180.0 / M_PI);
    }

    template <typename T>
    inline T radians(T angleInDegrees)
    {
        return angleInDegrees * static_cast<T>(180.0 / M_PI);
    }
    //-----------------------------get random----------------------------------
    template<typename T>
    struct random
    {
        operator T ()
        {
            static unsigned int seed = 0x13371337;
            unsigned int res;
            unsigned int tmp;

            seed *= 16807;
            tmp = seed ^ (seed >> 4) ^ (seed << 15);
            res = (tmp >> 9) | 0x3F800000;
            return static_cast<T>(res);
        }
    };

    template<>
    struct random<float>
    {
        operator float()
        {
            static unsigned int seed = 0x13371337;
            float res;
            unsigned int tmp;

            seed *= 16807;
            tmp = seed ^ (seed >> 4) ^ (seed << 15);
            *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;
            return static_cast<T>(res - 1.0f);
        }
    };

    template<>
    struct random<unsigned int>
    {
        operator unsigned int()
        {
            static unsigned int seed = 0x13371337;
            unsigned int res;
            unsigned int tmp;

            seed *= 16807;
            tmp = seed ^ (seed >> 4) ^ (seed << 15);
            res = (tmp >> 9) | 0x3F800000;
            return res;
        }
    };

    template<typename T, const int len>
    class vecN {
    public:
        typedef class vecN<T, len> myType;
        typedef T elementType;

        //default constrcutor dose nothing
        inline vecN()
        {
            //nothing
        }

        //copy constructor
        inline vecN(const vecN& that)
        {
            assign(that);
        }

        //construction from scalar
        inline vecN(T s)
        {
            for (int n = 0; n < len; n++) {
                data[n] = s;
            }
        }

        //assignment operator
        inline vecN& operator=(const vecN& that)
        {
            assign(that);
            return *this;
        }

        inline vecN& operator=(const T& that)
        {
            for (int n = 0; n < len; n++) {
                data[n] = that;
            }

            return *this;
        }

        inline vecN operator+(const vecN& that) const
        {
            myType result;
            for (int n = 0; n < len; n++) {
                result.data[n] = data[n] + that.data[n];
            }
            return result;
        }

        inline vecN& operator+=(const vecN& that)
        {
            return (*this = *this + that);
        }

        inline vecN& operator+=(const vecN& that)
        {
            return (*this = *this + that);
        }

        inline vecN operator

        inline vecN operator*(const vecN& that) const
        {
            myType result;
            for (int in = 0; n < len; n++) {
                result.data[n] = data[n] * that.data[n];
            }
            return result;
        }

        inline vecN



    private:
        T data[len];
    };





} // namespace vmath


#endif /*__VMATH_H__*