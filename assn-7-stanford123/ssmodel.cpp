/**
 * File: ssmodel.cpp
 * ------------------
 * This file will contain the implementation of the SSModel class (once you
 * have finished it!
 */
 
#include "ssmodel.h"
#include "exp.h"
#include <cctype>
#include <iomanip>

// I WISH I HAD BEEN ABLE TO FINISH.
// I DID THE BEST I COULD WITH THE TIME I HAD.

SSModel::SSModel(int nRows, int nCols, SSView *view) {
    intview=view;
    rows=nRows;
    cols=nCols;
    int counter = 0;
    // correspond ints to a-t (the grid is set at 35 by 20)
    for (char ch : "abcdefghijklmnopqrst") {
        counter+=1;
        charmap.put(ch,counter);
    }
}

SSModel::~SSModel() {}

bool SSModel::nameIsValid(const string& cellname) const {

    char cx = cellname.substr(0,1)[0];
    int ix = stringToInteger(cellname.substr(1));

    if (isalpha(cx) && charmap.get(tolower(cx))<=cols) {
        if (ix>0 && ix <= rows) {
            return true;
        }
    }
	return false;
}

bool SSModel::hasIdentifier(string s) {
    bool apostropheIsOn = false;
    bool foundChar = false;
    for (char c : s) {
        if (c=='"') apostropheIsOn=!apostropheIsOn;
        if (!apostropheIsOn) {
            if (isalpha(c)) {
                foundChar=true;
            } else {
                if (isdigit(c) && foundChar) {
                    return true;
                }
                foundChar=false;
            }
        }
    }
    return false;
}

Vector <string> SSModel::identifiers(string s) {
    bool apostropheIsOn = false;
    bool foundChar = false;
    Vector <string> retvals;
    char lastchar;

    for (char c : s) {
        if (c=='"') apostropheIsOn=!apostropheIsOn;
        if (!apostropheIsOn) {
            if (isalpha(c)) {
                foundChar=true;
            } else {
                if (isdigit(c) && foundChar) {
                    string retstring;
                    retstring += lastchar;
                    retstring += c;
                    //cout << " adding " << retstring << " to retvals" << endl;
                    retvals.add(retstring);
                }
                foundChar=false;
            }
        }
        lastchar=c;
    }
    return retvals;
}


void SSModel::setCell(string cellname, string s) {
    fullcontext.put(cellname, s);
    if (SSModel::hasIdentifier(s)) {
        Set<string> newset;
        // must add relationship (s)
        for (string relationname : identifiers(s)) {
            if (related.containsKey(relationname)) {
                newset = related.get(relationname);
            }
            newset.add(cellname);
            related.put(relationname, newset);
        }
    } else {
        //remove corresponding related memberships
        for (string skey : related) {
            if (related.get(skey).contains(cellname)) related.get(skey).remove(cellname);
            if (related.get(skey).size()==0) related.remove(skey);
        }
    }
}

void SSModel::cellRecalcs(string cellname, TokenScanner& scanner, Expression*& exp) {

    if (related.containsKey(cellname)) {
        cout << related.get(cellname).size() << " updates found." << endl;
        for (string s : related.get(cellname)) {
            string fullcontval = fullcontext.get(s);
            cout << "recalc "  << s << " as " << fullcontval << endl;
            scanner.setInput(fullcontval);
            exp = parseExp(scanner);
            double tempvalue = exp->eval(context);

            printToScreen(s, fullcontval, tempvalue);
            context.setValue(s, tempvalue);
        }
    }
}

bool isWord(string s) {
    for (char c : s) if (isdigit(c)) return false;
    return true;
}

void SSModel::printToScreen(const string& cellname, string& fullstring, double& value) {
    if (value!=0 || !isWord(fullstring)) {
        // print as numeric value
        string sdouble;
        ostringstream stream;
        stream << value;
        sdouble = stream.str();
        intview->displayCell(cellname, sdouble);
    } else {
        // print as raw string (fullcontext)
        intview->displayCell(cellname, fullstring);
    }
}

void SSModel::setCellfromString(const string& cellname, string& fullstring, double& value) {

    printToScreen(cellname, fullstring, value);

    setCell(cellname, fullstring);
    context.setValue(cellname, value);
}

void SSModel::setCellFromScanner(const string& cellname, TokenScanner& scanner) {

    Expression *exp = parseExp(scanner);
    string s = exp->toString();
    double value = exp->eval(context);

    setCellfromString(cellname, s, value);
    cellRecalcs(cellname, scanner, exp);

}

void SSModel::clearAll() {

    for (string skey : fullcontext) {
        string sblank;
        intview->displayCell(skey, sblank);
    }

    EvaluationContext newcontext;
    Map <string, string> newfullcontext;
    Map <string, Set<string>> newrelated;

    context = newcontext;
    fullcontext = newfullcontext;
    related = newrelated;

}

void SSModel::printCellInformation(const string& cellname) const {
    string lside = cellname;
    lside += ':';
    cout << left << setw(16) << lside << fullcontext.get(cellname) << endl;
}

void SSModel::writeToStream(ostream& outfile) const {
    //step through fullcontext
    for (string skey : fullcontext) {
        string s = fullcontext.get(skey);
        string linefull = skey;
        linefull+="=";
        linefull+=s;

        for (char c : linefull) {
            outfile.put(c);
        }
        outfile.put('\n');
    }
}

void SSModel::readFromStream(istream& infile) {

    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.scanStrings();
    string line;

    while (getline(infile, line)) {
        scanner.setInput(line);

        string cellname = scanner.nextToken();
        scanner.nextToken();
        setCellFromScanner(cellname, scanner);
    }


}
