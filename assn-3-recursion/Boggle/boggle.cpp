/**
 * File: boggle.cpp
 * ----------------
 * Implements the game of Boggle.
 */
 
#include <cctype>
#include <cmath>
#include <iostream>
using namespace std;

#include "simpio.h"
#include "gwindow.h"
#include "gboggle.h"
#include "random.h"
#include "grid.h"
#include "set.h"
#include "lexicon.h"
#include "vector.h"

const int kBoggleWindowWidth = 650;
const int kBoggleWindowHeight = 350;
const int NORTH=0;
const int NE=1;
const int EAST=2;
const int SE=3;
const int SOUTH=4;
const int SW=5;
const int WEST=6;
const int NW=7;

struct coord {
    int x;
    int y;
};

inline bool operator<(const coord& one, const coord& two) {
    if (one.x != two.x)
        return one.x < two.x;
    else
        return one.y < two.y;
}

const string kStandardCubes[16] = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

const string kBigBoggleCubes[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

static void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

static void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}

static bool responseIsAffirmative(const string& prompt) {
    while (true) {
        string answer = getLine(prompt);
        if (!answer.empty()) {
            switch (toupper(answer[0])) {
                case 'Y': return true;
                case 'N': return false;
            }
        }
        cout << "Please answer yes or no." << endl;
    }
}

static void makeBoard(bool isLarge) {

    if (isLarge) {
        drawBoard(5, 5);
    } else {
        drawBoard(4, 4);
    }

}

static string toupper (string s) {
    string retstr;

    for (int i=0; i<s.size();i++) {
        retstr+=toupper(s[i]);
    }
    return retstr;
}

static void newBoggle (bool isLarge, Vector<string>& cubes, Grid <char>& letterboard) {
    makeBoard(isLarge);
    // cubes are shuffled so we just need to randomize the face
    int cubenum = 0;

    for (int i=0; i<letterboard.nRows; i++) {
        for (int ii=0; ii<letterboard.nRows; ii++) {
            int face = randomInteger(0,5);
            char c = cubes[cubenum][face];
            labelCube(i, ii, c);
            letterboard[i][ii]=c;
            cubenum++;
        }
    }

}

static void newBoggle (bool isLarge, Grid <char>& letterboard, string forced) {
    makeBoard(isLarge);
    // using the forced string to label the cubes
    int cubenum = 0;

    for (int i=0; i<letterboard.nRows; i++) {
        for (int ii=0; ii<letterboard.nRows; ii++) {

            char c = forced[cubenum];
            labelCube(i, ii, c);
            letterboard[i][ii]=c;
            cubenum++;
        }
    }

}


static void shuffle(Vector<string>& cubes) {
    Vector<string> shuffled;

    for (int i=0; i<cubes.size(); i++) {
        int r = randomInteger(i,cubes.size()-1);
        //copy r val to swap, i to r, swap to i
        string swap = cubes[r];
        cubes[r] = cubes[i];
        cubes[i] = swap;
    }

}

static void shake (bool isLarge, Vector<string>& cubes) {

    if (isLarge) {
        for (string s : kBigBoggleCubes) {
            cubes.add(s);
        }
    } else {
        for (string s : kStandardCubes) {
            cubes.add(s);
        }
    }
    shuffle(cubes);
}

static Set<coord> getMatches(Grid <char>& letterboard, char match) {
    Set<coord> retset;
    coord matchpoint;

    for (int i=0; i<letterboard.nRows; i++) {
        for (int ii=0; ii<letterboard.nRows; ii++) {
            if (toupper(letterboard[i][ii])==toupper(match)) {
                matchpoint.x = i;
                matchpoint.y = ii;
                retset.add(matchpoint);
            }
        }
    }
    return retset;
}

static bool isProximateCoord(coord one, coord two) {
    return (abs(one.x-two.x)<=1 && abs(one.y-two.y)<=1);
}

static void clearBoard(Grid <char>& letterboard) {
    for (int i=0; i<letterboard.nRows; i++) {
        for (int ii=0; ii<letterboard.nRows; ii++) {
            highlightCube(i, ii, false);
        }
    }
}

static bool isUniqueCoord (coord c, Stack <coord> thisstack) {
    while (true) {
        if (thisstack.isEmpty()) break;
        coord cx = thisstack.pop();
        if (cx.x==c.x && cx.y == c.y) return false;
    }
    return true;
}

static void highlightStack (Stack <coord> thisstack) {

    Stack <coord> revstack;
    while (true) {
        if (thisstack.isEmpty()) break;
        revstack.push(thisstack.pop());
    }

    while (true) {
        if (revstack.isEmpty()) break;
        coord cx = revstack.pop();
        highlightCube(cx.x, cx.y, true);
        pause(250);
        highlightCube(cx.x, cx.y, false);
    }
}

static bool foundOnBoardh(string answer, Grid <char>& letterboard, int round, Vector <Stack <coord>>& potentials){
    char thisletter = answer[round];
    Set<coord> list = getMatches(letterboard, thisletter);
    bool movingon = false;

    if (potentials.size()==0) {
        for (coord c : list) {
            Stack <coord> newpath;
            newpath.push(c);
            potentials.add(newpath);
            movingon=true;
        }
    } else {
        Vector <Stack <coord>> potentialscopy;
        Stack <coord> thisstack;
        int ssize;

        for (coord c : list) {
            for (int i=0; i<potentials.size(); i++) {
                if (isProximateCoord(c,potentials[i].peek()) && isUniqueCoord(c,potentials[i])) {
                    thisstack = potentials[i];
                    thisstack.push(c);
                    potentialscopy.add(thisstack);
                    movingon=true;
                    //possible success
                    ssize=answer.size();
                    if (round>=ssize-1) {
                        cout << "nice one, human!" << endl;
                        highlightStack(thisstack);
                        potentials=potentialscopy;
                        return true;
                    }
                }
            }
        }
        potentials=potentialscopy;
    }
    if (movingon) {
        return foundOnBoardh(answer, letterboard, ++round, potentials);
    } else return false;

}

static bool foundOnBoard(string answer, Grid <char>& letterboard) {
    int round = 0;
    Vector <Stack <coord>> potentials;

    return foundOnBoardh(answer, letterboard, round, potentials);

}

static void humanRound(Grid <char>& letterboard, Set<string>& humanChoices) {

    Lexicon english("EnglishWords.dat");

    while (true) {
        string answer = getLine("Please enter a word. Enter 0 to finish: ");
        string ucanswer=toupper(answer);
        clearBoard(letterboard);
        if (ucanswer == "0") break;
        if (humanChoices.contains(ucanswer)) {
            cout << "You already entered " << ucanswer << endl;
        } else {
            if (english.contains(ucanswer) && ucanswer.size()>3 && foundOnBoard(ucanswer, letterboard)) {
                humanChoices.add(ucanswer);
                recordWordForPlayer(ucanswer, HUMAN);
            } else {
                // on blank return we display no error message
                if (answer.size()>0) cout << answer << " is not a legal word." << endl;
            }
        }
    }
}

static coord adjacentCoord(coord start, int direction) {
    coord c;

    // I tried this as a switch command but it didn't work that way so I am sorry this function looks ugly!

    if (direction==NORTH) {
        c.x=start.x-1;
        c.y=start.y;
    } else if (direction==NE) {
        c.x=start.x-1;
        c.y=start.y+1;
    } else if (direction==EAST) {
        c.x=start.x;
        c.y=start.y+1;
    } else if (direction==SE) {
        c.x=start.x+1;
        c.y=start.y+1;
    } else if (direction==SOUTH) {
        c.x=start.x+1;
        c.y=start.y;
    } else if (direction==SW) {
        c.x=start.x+1;
        c.y=start.y-1;
    } else if (direction==WEST) {
        c.x=start.x;
        c.y=start.y-1;
    } else if (direction==NW) {
        c.x=start.x-1;
        c.y=start.y-1;
    }
    return c;
}

static bool findWords(Grid <char>& letterboard, coord start, Set<coord> visited, Lexicon& english, string wordpartial, Set<string>& possibles) {
    // it never returns true because it needs to exhaust all options.

    int ssize = letterboard.numCols();
    char c = letterboard[start.x][start.y];

    visited.add(start);
    wordpartial+=c;
    if (english.contains(wordpartial) && wordpartial.size()>=4) possibles.add(wordpartial);
    if (!english.containsPrefix(wordpartial)) return false;

    //for each next cell
    for (int x=0; x<8; x++) {
        coord nextcoord = adjacentCoord(start, x);
        if (!visited.contains(nextcoord) && nextcoord.x>=0 && nextcoord.y>=0 && nextcoord.x<ssize && nextcoord.y<ssize) {
            if (findWords(letterboard, nextcoord, visited, english, wordpartial, possibles)) return true;
        }
    }

    return false;
}

static void computerRound (Grid <char>& letterboard, Set<string>& humanChoices, Set<string>& computerChoices) {

    Lexicon english("EnglishWords.dat");

    for (int i=0; i<letterboard.nRows; i++) {
        for (int ii=0; ii<letterboard.nRows; ii++) {
            Set<string> possibles;
            coord c;
            Set<coord> visited;

            c.x = i;
            c.y = ii;
            findWords(letterboard, c, visited, english, "", possibles);
            computerChoices += possibles;
        }
    }

    for (string s : computerChoices-humanChoices) {
        recordWordForPlayer(s, COMPUTER);
    }

    cout << "the computer found " << computerChoices.size() << " words." << endl;


}

static string getForcedString () {
    string s = getLine("Please enter a string that will represent the cube faces. ");
    return s;
}

int main() {
    GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
    Vector<string> cubes;
    Set<string> humanChoices;
    Set<string> computerChoices;
    Grid<char> letterboard;
    bool isLargeGrid;

    initGBoggle(gw);
    welcome();
    if (responseIsAffirmative("Do you need instructions? ")) {
        giveInstructions();
    }
    if (responseIsAffirmative("Would you like to use a larger board? ")) {
        letterboard.resize(5,5);
        isLargeGrid=true;
    } else {
        letterboard.resize(4,4);
        isLargeGrid=false;
    }

    if (responseIsAffirmative("Would you like to force the board configuration? ")) {
        string s = getForcedString();
        newBoggle(isLargeGrid, letterboard, toupper(s));
    } else {
        shake(isLargeGrid, cubes);
        newBoggle(isLargeGrid, cubes, letterboard);
    }


    humanRound(letterboard, humanChoices);
    computerRound(letterboard, humanChoices, computerChoices);

    cout << "Thank you for playing Boggle." << endl;

    return 0;
}
