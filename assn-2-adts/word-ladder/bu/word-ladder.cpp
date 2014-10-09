/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "vector.h"
#include "queue.h"
#include "strlib.h"
#include "simpio.h"
#include "set.h"
#include <string>
#include <cctype>
#include "simpio.h"
#include "foreach.h"

using namespace std;


static string getWord(Lexicon& english, string prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response)) return response;
        cout << "Your response needs to be an English word, so please try again." << endl;
    }
}

static Queue<string> DifferentByNum(string matching, Lexicon& english) {
    Queue<string> localQ;
    string newword;
    string matchcopy;
    string alpha = "abcdefghijklmnopqrstuvwxyz";

    for (int wordi = 0; wordi < matching.length(); wordi++) {
        //each letter of word
        for (int alphai = 0; alphai < alpha.length(); alphai++) {
            matchcopy = matching;
            newword = matchcopy.replace(wordi, 1, alpha.substr(alphai,1));
            if (english.contains(newword)) localQ.enqueue(newword);
        };
    };

    return localQ;

};

static Queue<string> NeighborsByOne (string matching, Lexicon& english) {

    Queue<string> localQ;
    Queue<string> thislist = DifferentByNum(matching, english);

    while (true) {
        if (thislist.isEmpty()) break;
        string thismatch = thislist.dequeue();
        localQ.enqueue(thismatch);
    };

    return localQ;
}

static void addQueues(Queue<Stack<string> > & qa, Queue<string>& qb, Stack<string>& thisstack, Set<string>& alreadyadded) {
    while (true) {
        string thisstring;
        Stack<string> addedstack;

        if (qb.isEmpty()) break;
        thisstring = qb.dequeue();
        if (!alreadyadded.contains(thisstring)) {
            addedstack = thisstack;
            addedstack.push(thisstring);
            alreadyadded.add(thisstring);
            qa.enqueue(addedstack);
        };
    };
}

static bool HasTarget (Queue<Stack<string> > potentials, string target) {
    int ssize;
    int psize = potentials.size();
    string compare;


    for (int i=0; i<psize; i++) {
        Stack<string> ss;
        Stack<string> curstack;

        ss = potentials.dequeue();
        curstack = ss;

        ssize = ss.size();
        for (int x=0; x<ssize; x++) {
            compare = ss.pop();

            if (target == compare) {
                cout << "matched: " << endl;
                cout << curstack << endl;
                return true;
            };
        };
    };
    return false;
}



static void generateLadder(Lexicon& english, string start, string end) {
    cout << "... search for a word ladder connecting \"" << start << "\" to \"" << end << "\"." << endl;

    Queue<Stack<string> > potentials;
    Queue<string> added;
    Set<string> alreadyadded;

    // add start
    string nextword;
    Stack<string> thisstack;
    string message;
    thisstack.push(start);
    potentials.enqueue(thisstack);
    alreadyadded.add(start);

    while (true) {
        thisstack = potentials.dequeue();
        nextword = thisstack.peek();
        //cout << "nextword " << nextword << endl;
        added = NeighborsByOne(nextword, english);

        addQueues(potentials, added, thisstack, alreadyadded);

        if (potentials.isEmpty()) {
            cout << "no match." << endl;
            break;
        };

        if (HasTarget(potentials, end)) {
            break;
        };

    };
}



static const string kEnglishLanguageDatafile = "EnglishWords.dat";
static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty()) break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty()) break;
        generateLadder(english, start, end);
    }
}

int main() {

    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    return 0;
}
