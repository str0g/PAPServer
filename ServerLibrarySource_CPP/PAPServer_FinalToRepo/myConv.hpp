#ifndef MYCONV_HPP_INCLUDED
#define MYCONV_HPP_INCLUDED

#include <string>
#include <sstream>

namespace myConv{
    /**
    *Konwertuje dowolna liczbę na string
    */
    template <typename T>
    inline std::string ToString(const T& liczba){
        std::stringstream ss;
        ss << liczba;
        return ss.str();
    }
    //Z dowolnego stringa robi liczbe
    /**
    *Metoda przyjmoje na wejsciu dowolnych łańcuch textowy i przerabia go na liczbę w zdeklarowanym typie
    */
    template <typename T>
    inline T FromString(std::string s_liczba){
        T liczba;
        std::istringstream myStream(s_liczba);
        myStream>>liczba;
        return liczba;
    }
}
#endif // MYCONV_HPP_INCLUDED
