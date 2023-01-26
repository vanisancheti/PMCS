/*
 * File Info: Vector2D.cpp
 * Description: This file contains the declarations of the methods of Vector2D class
 *
 * */

#include <iostream>
#include <vector>
#include "Vector2D.h"

using namespace std;

Vector2D::Vector2D()
{
    noOfRows_i = noOfCols_i = 0;
}

Vector2D::Vector2D(int noOfRows_i, int noOfCols_i)
{
    this->noOfRows_i = noOfRows_i;
    this->noOfCols_i = noOfCols_i;

    for(int i=0; i < noOfRows_i; i++)
    {
        vector< vector<int> > temp1;
        for(int j=0; j < noOfCols_i; j++)
        {
            vector<int> temp2;
            temp1.push_back(temp2);
        }
        matrix.push_back(temp1);
    }
}

void Vector2D::Initialize(int noOfRows_i, int noOfCols_i)
{
    this->noOfRows_i = noOfRows_i;
    this->noOfCols_i = noOfCols_i;

    for(int i=0; i < noOfRows_i; i++)
    {
        vector< vector<int> > temp1;
        for(int j=0; j < noOfCols_i; j++)
        {
            vector<int> temp2;
            temp1.push_back(temp2);
        }
        matrix.push_back(temp1);
    }
}

void Vector2D::Reset()
{
    for(int i=0; i < noOfRows_i; i++)
        for(int j=0; j < noOfCols_i; j++)
        {
            matrix[i][j].clear();
            //matrix[i][j].~vector<int>();
        }
}
void Vector2D::InsertAt(int r_i, int c_i, int val_i)
{
    //assert(r_i < noOfRows_i);
    //assert(c_i < noOfCols_i);
    matrix[r_i][c_i].push_back(val_i);
}

bool Vector2D::BFindAt(int r_i, int c_i, int val_i)
{
    for(int i=0; i < matrix[r_i][c_i].size(); i++)
        if(matrix[r_i][c_i][i] == val_i)
            return true;

    return false;
}

void Vector2D::Print()
{
    for(int i=0; i < noOfRows_i; i++)
    {
        for(int j=0; j < noOfCols_i; j++)
        {
            cout << "[" << i << "," << j << "] : ";
            for(int k=0; k < matrix[i][j].size(); k++)
                cout << matrix[i][j][k] << ", ";
            cout << endl;
        }
    }
}

void Vector2D::Print(int r_i, int c_i)
{
    //assert(r_i < noOfRows_i);
    //assert(c_i < noOfCols_i);

    cout << "[" << r_i << "," << c_i << "] : ";
    for(int i=0; i < matrix[r_i][c_i].size(); i++)
        cout << matrix[r_i][c_i][i] << ", ";
}
