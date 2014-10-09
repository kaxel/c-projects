/**
 * File: random-sentence-generator.cpp
 * -----------------------------------
 * Presents a short program capable of reading in
 * context-free grammar files and generating arbitrary
 * sentences from them.
 */

#include <iostream>
#include <fstream>
using namespace std;

#include "console.h"
#include "simpio.h"   // for getLine
#include "strlib.h"   // for toLowerCase, trim
#include <cctype>
#include "map.h"
#include "vector.h"
#include "set.h"
#include "random.h"

static const string kGrammarsDirectory = "grammars/";
static const string kGrammarFileExtension = ".g";
static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

static bool isValidGrammarFilename(string & filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    filename = normalizedFileName;
    return !infile.fail();
}

static bool haspunct(string s) {
    bool answer = false;

    for (int i=0; i<s.size(); i++) {
        if (ispunct(s[i])) answer = true;
    }

    return answer;
}



static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename)) return filename;
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}

static void processOneSingleDef(Map <string, string>& defined, Map <string, Vector<string> >& phrases, Vector<string>& names) {

    bool foundone = false;

    //pick the first vector in phrases with no punctuation and process it

    while (!foundone) {

        //pick random name
        int x = randomInteger(0,(names.size()-1));
        bool cleandef = true;
        string defname = "";
        string defval = "";
        int randomdef = 0;
        int local_count = 0;
        int local_max = 0;

        Vector<string> thisphrase = phrases.get(names[x]);
        cout << " found: " << thisphrase[0] << endl;
        local_count++;

        for (int i=1; (i<thisphrase.size()); i++) {
            cout << "def: " << i << " " << thisphrase[i] << endl;
            if (haspunct(thisphrase[i])) cleandef = false;
        }

        cout << "cleandef possible: " << cleandef << endl;

        if (cleandef) {
            randomdef=randomInteger(1, (thisphrase.size()-1));
            //defname=removepunct(thisphrase[0]);
            defname=thisphrase[0];
            defval=thisphrase[randomdef];
            cout << "putting defname " << defname << " for defval " << defval << endl;
            defined.put(defname, defval);
            phrases.remove(names[x]);
            names.remove(x);

            //replace all phrases with reference to defname round 1
            for (string n : names) {

                Vector<string> comparephrase;
                string olddef;
                string newdef;
                string lookup;
                Vector<string> newphrase;

                comparephrase = phrases.get(n);

                for (int i=1; (i<comparephrase.size()); i++) {
                    if (!comparephrase[i].find(defname,0)==string::npos) {
                        olddef = comparephrase[i];
                        lookup = defined.get(n);
                        newdef = olddef.replace(0,lookup.length(), lookup);
                        newphrase.add(n);
                        newphrase.add(newdef);
                        cout << "update " << n << " from " << olddef << " to new val " << newdef << endl;
                        phrases.put(n, newphrase);

                    }
                }
                foundone=true;
            }

        } else {
            cout << " no clean def, skip for now" << endl;
            phrases.remove(names[x]);
            names.remove(x);
        }

        cout << endl;
        cout << defined.size() << " definitions" << endl;
        cout << phrases.size() << " phrases" << endl;
        cout << names.size() << " names" << endl;

        if (names.isEmpty()) break;
        if (defined.size()==1) break;
        if (foundone) break;
    }


}

static void constructDefinitions(Vector <string>& lines, Map <string, string>& defined, Map <string, Vector<string> >& phrases) {
    Vector <string> linescopy = lines;

    int locali = 1;
    Vector<string> thisphrase;
    string name;
    Vector<string> names;

    for (string s : lines) {
        cout << "processing: " << s << " as line: " << locali << " in a phrase." << endl;

        if (locali==1) {
            thisphrase.clear();
            cout << " phrase name: " << s << endl;
            name=s;
            names.add(name);
            thisphrase.add(s);
            locali++;
        } else if (locali>2 && (s!="_")) {
            cout << " phrase definition: " << s << endl;
            thisphrase.add(s);
            locali++;
        } else if (s=="_") {
            phrases.put(name, thisphrase);
            locali=1;
            cout << endl;
        } else {
            locali++;
        }
    }

    cout << phrases.size() << " phrases are defined." << endl;

    while (true) {
        processOneSingleDef(defined, phrases, names);

        if (phrases.isEmpty()) break;
    }

}

static void processFile (Vector <string>& lines, Map <string, string>& defined, Map <string, Vector<string> >& phrases) {
    string s;

    //intro
    while (true) {
        string filename = getFileName();
        if (filename.empty()) break;

        ifstream infile(filename.c_str());
        cout << "opening " << filename << endl << endl;
        while (getline(infile, s)) {

            //each line
            // is it a non terminal, a number, non terminal value, or a space?
            cout << s << " is a";
            if (isdigit(s[0])) {
                cout << "n integer";
                lines.add(s);
            }  else if (ispunct(s[0])) {
                cout << " non terminal";
                lines.add(s);
            } else if (isalpha(s[0])) {
                cout << " value";
                lines.add(s);
            } else {
                cout << " blank space" << endl;
                lines.add("_");
            }
            cout << endl;

        };

        cout << "lines to process: " << lines.size() << endl;
        constructDefinitions(lines, defined, phrases);
    };
}

int main() {

    Vector <string> lines;
    Map <string, Vector<string> > phrases;
    Map <string, string> defined;

    processFile(lines, defined, phrases);

    cout << "generating a poem" << endl;

    
    cout << "Thanks for playing!" << endl;
    return 0;
}
