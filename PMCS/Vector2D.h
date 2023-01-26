/*
 * File Info: Vector2D.h
 * Description: This file contains the declaration of the Vector2D class
 *
 * */

#ifndef _VECTOR2D_H_
#define _VECTOR2D_H_

#include <iostream>
#include <vector>

using namespace std;

class Vector2D
{
    private:
            vector<vector<vector<int> > > matrix;
            int noOfRows_i, noOfCols_i;

    public:
            Vector2D();
            Vector2D(int noOfRows_i, int noOfCols_i);

            void Initialize(int noOfRows_i, int noOfCols);
            void Reset();
            void InsertAt(int r_i, int c_i, int val_i);
            bool BFindAt(int r_i, int c_i, int val_i);
            int IGetSizeAt(int r_i, int c_i);

            int IGetNoOfRows() { return noOfRows_i; }
            int IGetNoOfCols() { return noOfCols_i; }
            vector<int> ViGetElementAt(int r_i, int c_i) { return matrix[r_i][c_i]; }

            void Print();
            void Print(int r_i, int c_i);
};

#endif
