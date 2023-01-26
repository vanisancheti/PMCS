/*
 * File Info: Triplet.cpp
 * Description: This file contains the description of the methods of Triplet class
 *
 * */

#include <iostream>
#include "Triplet.h"
using namespace std;

Triplet::Triplet()
{
}

Triplet::Triplet(int f, int s, int t)
{
    first = f;
    second = s;
    third = t;
}

void Triplet::SetFirst(int f)
{
    first = f;
}

void Triplet::SetSecond(int s)
{
    second = s;
}

void Triplet::SetThird(int t)
{
    third = t;
}

int Triplet::GetFirst()
{
    return first;
}

int Triplet::GetSecond()
{
    return second;
}

int Triplet::GetThird()
{
    return third;
}

bool Triplet::Equals(Triplet t)
{
    if(first == t.GetFirst())
        if(second == t.GetSecond())
            if(third == t.GetThird())
                return true;

    return false;
}
void Triplet::SetValues(int f, int s, int r)
{
    first = f;
    second = s;
    third = r;
}

void Triplet::Print()
{ 
    cout << "[" << first << " - " << second << " - " << third << "]" << endl;
}
