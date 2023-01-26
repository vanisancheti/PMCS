/*
 * File Info: Triplet.h
 * Description: This file contains the declaration of the Triplet class
 *
 * */

#ifndef _TRIPLET_H_
#define _TRIPLET_H_
class Triplet
{
    private:
        int first, second, third;
    public:
        Triplet();
        Triplet(int f, int s, int t);
        void SetFirst(int f);
        void SetSecond(int s);
        void SetThird(int t);
        int GetFirst();
        int GetSecond();
        int GetThird();
        bool Equals(Triplet t);
        void SetValues(int f, int s, int r);
        void Copy(Triplet t) { SetValues(t.GetFirst(), t.GetSecond(), t.GetThird()); }
        void Print();
};
#endif
