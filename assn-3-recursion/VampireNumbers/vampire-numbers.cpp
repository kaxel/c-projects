/**
 * File: vampire-numbers.cpp
 * -------------------------
 * Provides a program that repeatedly prompts the user for positive
 * numbers with an even number of digits and informs them
 * whether the number is a Vampire number or not.
 */

#include <string>    // for string
#include <iostream>  // for cout, endl
using namespace std;

#include "console.h" 
#include "simpio.h" // for getInteger
#include "strlib.h" // for stringToInteger, integerToString
#include "set.h"

static int getPossibleVampireNumber() {
    while (true) {
        int response = getInteger("Enter a positive integer [or 0 to quit]: ");
        if (response >= 0) return response;
        cout << "Ah, sorry.  I need a nonnegative response.  Try again... " << endl;
    }
}

Set<string> generatePermutations(string str) {
    //I copied this function from the reader (hope that's ok!)
    Set <string> result;
    int slength = str.length();

    if (str=="") {
        result += "";
    } else {
        for (int i=0; i<slength; i++) {
            char ch = str[i];
            string rest = str.substr(0, i) + str.substr(i + 1);
            foreach (string s in generatePermutations(rest)) {
                result += ch + s;
            }
        }
    }

    return result;
}

static bool hasMatch(int number, int& first, int& second) {
    string firststr = integerToString(first);
    string secondstr = integerToString(second);

    foreach (string sf in generatePermutations(firststr)) {
        foreach (string ss in generatePermutations(secondstr)) {
            if (stringToInteger(sf)*stringToInteger(ss)==number) {
                first = stringToInteger(sf);
                second = stringToInteger(ss);
                return true;
            }
        }
    }
    return false;

}

static bool isVampireNumberh(int number, int& first, int& second, int pos) {

    string newfirst;
    string newsecond;
    string numstr = integerToString(number);
    char c = numstr[pos];
    int newfirstint;
    int newsecondint;
    int strsize, firstsize, secondsize;
    int fanlen = integerToString(number).size()/2;

    newfirst = integerToString(first) + c;
    newsecond = integerToString(second) + c;

    strsize=numstr.size();
    if (pos==strsize) {
        //reckoning
        firstsize=integerToString(first).size();
        secondsize=integerToString(second).size();
        if (firstsize==fanlen && secondsize==fanlen) {
            if (hasMatch(number, first, second)) {
                cout << "first fang: " << first << " and the second: " << second << endl;
                return true;
            }
        }
    } else {
        newfirstint = stringToInteger(newfirst);
        newsecondint = stringToInteger(newsecond);
        pos++;

        if (isVampireNumberh(number, newfirstint, second, pos)) return true;
        if (isVampireNumberh(number, first, newsecondint, pos)) return true;

    }

    // for some reason, on exiting this function my values for first and second get blown away
    // I have stepped through it many times and can not figure out why. So that is why I am showing the fangs in the helper function
    // might as well show them off while I still have them. I would LOVE to find out why this is happening.
    return false;
}

static bool isVampireNumber(int number, int& first, int& second) {
    // replace this line with your own implementation.  You will want
    // to implement this as a wrapper around a second version of isVampireNumber
    // that does the actual recursion.
    bool isvampire;

    if (integerToString(number).size()%2>0) {
        cout << "you need an even number of integers." << endl;
        return false;
    }

    isvampire = isVampireNumberh(number, first, second, 0);
    return isvampire;
}

int main() {
    cout << "Here's a program that tells you whether or not a "
         << "number you enter is Vampire." << endl << endl;
    while (true) {
        int number = getPossibleVampireNumber();
        if (number == 0) break;
        int first;
        int second;

        if (isVampireNumber(number, first, second)) {
            cout << "Woo! " << number << " is a Vampire number." << endl;
        } else {
            cout << "Nope! The number " << number << " isn't Vampire." << endl << endl;
        }
    }
    cout << endl;
    cout << "All done!" << endl;
    return 0;
}
