/*
 * File Info: global_functions.h
 * Description: This file contains the declarations of the functions common to all the files
 * */

#ifndef _GLOBAL_FUNCTIONS_H_
#define _GLOBAL_FUNCTIONS_H_

string GetAsString(int n)
{
    stringstream sstr;
    sstr << n;
    string ans = sstr.str();
    return ans;
}

void Error(char *c_fnName, char *c_errorMsg)
{
    cout << "In Function: " << c_fnName << " Err: " << c_errorMsg << endl;
}

void ErrorExit(char *c_fnName, char *c_errorMsg)
{
    cout << "In Function: " << c_fnName << " Err: " << c_errorMsg << endl;
    exit(0);
}

bool BIsPresentInIntVector(vector<int> array_Vi, int search_i)
{
    //vector<int>::iterator beginIt = array_Vi.find(search_i);
    vector<int>::iterator beginIt = find(array_Vi.begin(), array_Vi.end(), search_i);
    if(beginIt != array_Vi.end())
        return true;
    return false;
}

bool BIsPresentInNodeIDVector(vector<NodeID> array_VnID, NodeID search_nID)
{
    //vector<NodeID>::iterator beginIt = array_VnID.find(search_nID);
    vector<NodeID>::iterator beginIt = find(array_VnID.begin(), array_VnID.end(), search_nID);
    if(beginIt != array_VnID.end())
        return true;
    return false;
}

void PrintLine()
{
    for(int i=0; i<50; i++)
        cout << "-";
    cout << endl;
}

void PrintMap(map<int, int> elements_m)
{
    map<int, int>::iterator begin_mIt = elements_m.begin();
    map<int, int>::iterator end_mIt = elements_m.end();
    while(begin_mIt != end_mIt)
    {
        cout << "(" << (*begin_mIt).first << ", " << (*begin_mIt).second << "), ";
        begin_mIt++;
    }
    cout << endl;
}


int SplitString(const string& input,
       const string& delimiter, vector<int>& results,
       bool includeEmpties=false)
{
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = (int)delimiter.size();
    int isize = (int)input.size();

    if(
        ( isize == 0 )
        ||
        ( sizeS2 == 0 )
    )
    {
        return 0;
    }

    vector<int> positions;

    newPos = (int) (input.find(delimiter, (size_t)0));

    if( newPos < 0 )
    {
        return 0;
    }

    int numFound = 0;

    while( (size_t)newPos != string::npos && newPos >= iPos )
    {
        numFound++;
        positions.push_back(newPos);
        iPos = newPos;
        newPos = (int)input.find (delimiter, (size_t)iPos+sizeS2);
    }
    if( numFound == 0 )
    {
        return 0;
    }
    // std::cout << "/* message end */" << '\n';
    for( int i=0; i <= (int)positions.size(); i++ )
    {
        string s("");
        int offset;
        if( i == 0 )
        {
            s = input.substr((size_t)i, (size_t)positions[i] );
            offset = sizeS2;
        }
        else
        {
          offset = positions[i-1] + sizeS2;
        }
        if( offset < isize )
        {
            if( i == positions.size() )
            {
                s = input.substr((size_t)offset);
            }
            else if( i > 0 )
            {
                s = input.substr( (size_t)positions[i-1] + sizeS2,
                      (size_t)positions[i] - positions[i-1] - sizeS2 );
            }
        }
        if( includeEmpties || ( s.size() > 0 ) )
        {
            int iVal = atoi((char*)s.c_str());
            results.push_back(iVal);
        }
    }
    // std::cout << "end" << '\n';
    return numFound;
}

#endif
