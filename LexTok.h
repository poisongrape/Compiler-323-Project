//
//  LexTok.h
//  lexer
//
//  Created by Christopher Dancarlo Danan and Yuri Van Steenburg based on Chris's code on 9/28/14.
//  Copyright (c) 2014 Christopher Dancarlo Danan & Yuri Van Steenburg. All rights reserved.

//Name:				Christopher Dancarlo Danan
//					Yuri Van Steenburg
//Date Began:		September 28, 2014
//Due Date:			N/A (No due date, just updating lexer to class implementation)
//Class:			CPSC 323; Compilers
//Professor:		Mark Murphy
//Project:			N/A (Alternate version of lexer project)
//File Name:		LexTok.h
//Purpose:			The header file for the LexTok class.
//References:		Previous version of Lexer program.
//Notes:			-Problem running on Chris's Visual Studio 2010. [DONE]
//						-Chris initialized the class's variables in the constructor, which seemed to solve the problem.
//Updates:			September 28, 2014
//						-Yuri converted Chris's first version of the Lexer program to a class implementation.  Chris had a
//						 problem with Visual Studio 2010 giving an abort error after he typed in the file name.
//					October 2, 2014
//						-Initialized the class's variables (known, ch, token, lexeme) so that they are blank.

#ifndef __lexer__LexTok__
#define __lexer__LexTok__
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fstream>


using namespace std;

class LexTok{
public:
    LexTok();
    string token;			//Holds token.
    string lexeme;			//Holds lexeme.
    string inputFileName;	//Holds name of the input file that holds the source code.
    ifstream fin;			//File to read the source code from.
    ofstream fout;			//File to write the lexer/token pairs to.
    bool known;				//Checks to see if current character is known when loop iterates.
    char ch;				//Used to hold the current character of the input file
	int newLineCount;

    void getTokLex(string& tok, string& lex, ifstream& fin, ofstream& fout, bool& known, char& ch);
    void lastCharIdentifier(const string lex, string& token);
    bool singleOperatorsLookup(const char singleOps[], const char lex);
    bool branchingOperatorsLookup(const char branchOps[], const char lex);
    bool keywordLookup(const string keywords[], const string lex);
};


#endif /* defined(__lexer__LexTok__) */
