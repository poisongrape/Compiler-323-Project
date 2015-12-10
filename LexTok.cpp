//
//  LexTok.cpp
//  lexer
//
//  Created by Christopher Dancarlo Danan and Yuri Van Steenburg based on Chris's code on 9/28/14.
//  Copyright (c) 2014 Christopher Dancarlo Danan & Yuri Van Steenburg. All rights reserved.
//
//Name:				Christopher Dancarlo Danan
//					Yuri Van Steenburg
//Date Began:		September 28, 2014
//Due Date:			N/A (No due date, just updating lexer to class implementation)
//Class:			CPSC 323; Compilers
//Professor:		Mark Murphy
//Project:			N/A (Alternate version of lexer project)
//File Name:		LexTok.cpp
//Purpose:			The implementation file for the LexTok class.
//References:		Previous version of Lexer program.
//Notes:			-Problem running on Chris's Visual Studio 2010. [DONE]
//						-Chris initialized the class's variables in the constructor, which seemed to solve the problem.
//Updates:			September 28, 2014
//						-Yuri converted Chris's first version of the Lexer program to a class implementation.  Chris had a
//						 problem with Visual Studio 2010 giving an abort error after he typed in the file name.
//					October 2, 2014
//						-Initialized the class's variables (known, ch, token, lexeme) so that they are blank.

#include "LexTok.h"

//Function:	Empty constructor
LexTok::LexTok()
{
	token = "";
	lexeme = "";
	known = "";
	ch = ' ';
	newLineCount = 1;

	cout<<"Enter the name of the input file (including extension): ";
	cin>>inputFileName;

	fin.open(inputFileName.c_str());
    fout.open("LexemeTokenList.txt");
}

/*
    Function:	Sort the source code into lexemes and tokens.
    Input:      tok: Token
                lex: Lexeme
                fin: Input file
                fout: Output file
                known: Flag if current character in the stream is stored in a variable or not.
                ch: Holds the current character in the stream.
*/
void LexTok::getTokLex(string& tok, string& lex, ifstream& fin, ofstream& fout, bool& known, char& ch)
{
    char tempPeek;		//Holds the peeked character from the stream (peeked being the next character in stream).
    string trashLine;	//Used to hold the rest of the line if a comment operator is found. This will not be outputted to user.
    
    //Lookup arrays:
    //Holds all possible keywords.
    string keywordList[17] = {"int", "real", "bool", "program", "begin", "end", "function", "read", "write", "if", "else", "elseif", "while", "do", "until", "true", "false"};
    //Holds all possible single operators (operators that do not lead to anything else).
    char singleOperatorsList[12] = {'+', '-', '*', '=', ';', ':', ',', '(', ')', '!', '.', '_'};
    //Holds all branching operators (operators that can lead to different lexemes).
    char branchingOperatorsList[3] = {'/', '>', '<'};
    
    tok = lex = "";			//Restart tok and lex each time the loop iterates.
    
    //If the current character is not known, then get the next character.  This ensures no character in the stream is skipped.
    if(!known)
    {
        fin.get(ch);
        known = true;
    }
    
    //Ignore white space by skipping it and going to next character.
    while(isspace(ch) && fin)
    {
		if(ch == '/0' || ch == '\n')
			newLineCount++;

        fin.get(ch);
        known = true;
    }
    
    //Now to categorize ch (which, at this point, holds the first character of the lexeme) as either alpha, numeric, symbol, or other.
    //If the first character is alphabetical.
    if(isalpha(ch))
    {
        lex = lex + ch;		//Update lex string.
        fin.get(ch);		//Get next character.
        known = true;		//We have a character in ch.
        
        //Need to check if the character is valid for identifiers.
        //If it is valid, then keep getting characters until we find an invalid character.
        while((isalpha(ch) || isdigit(ch) || ch == '_') && fin)
        {
            lex = lex + ch;		//Update lex string.
            fin.get(ch);		//Get next character.
            known = true;		//We know the next character.
        }
        
        //Once we reach this point, then that means that we have an invalid character in ch and have reached the end of the identifier.
        //We must now check if the last character of the identifier is valid (i.e. it is not an underscore).
        lastCharIdentifier(lex, tok);
        
        //Next, check if identifier is a keyword or identifier.
        //If lexeme is in the keyword list, then it is a keyword.
        if(keywordLookup(keywordList, lex))
            tok = "Keyword";
        //If lexeme is not in the keyword list, then it is an identifier.
        else
            if(tok == "") //This line handles if token was labeled as "Invalid" already.
                tok = "ident";
        
        //cout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Output to console.
        fout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Output to file.
    }
    //If the first character is a digit.
    else if(isdigit(ch))
    {
        lex = lex + ch;
        fin.get(ch);
        
        //If there are more digits, then keep adding them until other type of character is found.
        while(isdigit(ch) && fin)
        {
            lex = lex + ch;
            fin.get(ch);
            known = true;
            tok = "int";
        }
        
        //Once we get here, then that means we got a character that was not a digit.
        //Check if it is a '.'
        if(ch == '.')
        {
            //If it is, then we must peek at next the next character.  If next char is a digit, then we are dealing with a real number.
            //If the next character is not a digit, then we have an integer and an operator (.)
            tempPeek = fin.peek();		//Store the peeked char in a temp variable.
            
            //If next character after '.' IS a digit (the token is a "Real" number).
            if(isdigit(tempPeek))
            {
                lex = lex + ch;		//Store the '.' (###.)
                fin.get(ch);		//Get the next character, which we know is a digit.
                
                //Continue to get digits as before until we encounter a nondigit.
                while(isdigit(ch) && fin)
                {
                    lex = lex + ch;
                    fin.get(ch);
                    known = true;
                }
                
                tok = "real";
            }
            //Else next character after '.' is NOT a digit.
            else
            {
                //If next character is NOT a digit, then we already have the lexeme for the integer and the current ch is the '.'.
                //Note that we do NOT advance ch to the next character in the stream, otherwise we would lose the '.'.
                tok = "int";
                known = true;
            }
        }
        //If integer NOT followed by ., then it is an integer.
        else
        {
            tok = "int";
            known = true;
        }
        
        //cout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Write to console.
        fout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Write to file.
    }
    //If the first character is a single operator (i.e. it cannot possibly be one of several things).
    else if(singleOperatorsLookup(singleOperatorsList, ch))
    {
        known = false;		//known will now be false because we did not get the next character - we are using only the current one.
        tok = "Operator";	//We know this is an operator.
        lex = lex + ch;
        //'=' is a relational operator.
        if(ch == '=')
            tok = "Relational Operator";
        
        //cout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;		//Write to console.
        fout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;		//Write to file.
        
       ch = ' ';
        //Since known is false, we need to get next character on the next iteration of the loop.  However, this causes a repetition of
        //lexemes in some cases (e.g. if file ends in "111.").  In this case, the eof has not been reached, so the same character gets
        //printed again.  In order to fix this, I initialized ch to a whitespace character.  This will trigger the whitespace loop, which
        //will find the eof.
    }
    //If the first character is a branching operator (i.e. it may lead to several possible lexemes).
    else if(branchingOperatorsLookup(branchingOperatorsList, ch))
    {
        known = false;	//known will now be false because we did not get the next character - we are using only the current one.
        
        //If ch = '/', need to check if it is a single operator '/', or a comment operator '//'.  If it is a comment operator, we need to move to the next line of stream.
        if(ch == '/')
        {
            tempPeek = fin.peek();	//Peek ahead to the next character in the stream.
            
            //Check if next character is another forward slash (indicating a comment).
            if(tempPeek == '/')
            {
                getline(fin, trashLine); //Store line in a variable but don't do anything with it.  getline will automatically move to next line.
				//ch = ' ';
            }
            //If next character is not a forward slash, then '/' is not a comment but an operator.
            else
            {
                lex = lex + ch;		//Add '/' to lexeme.
                tok = "Operator";	//Single '/' is an operator.
                //cout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;	//Write to console.
                fout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;	//Write to file.
            }
            //If ch = '>', then lexeme could be '>' or '>='.
        }
        else if(ch == '>')
        {
            lex = lex + ch;			//lex = >
            tempPeek = fin.peek();	//Peek ahead to the next character in the stream.
            
            //If there is an equals sign, then the operator is '>='.
            if(tempPeek == '=')
            {
                fin.get(ch);		//Get the '='
                lex = lex + ch;		//lex = '>='
            }
            
            //If there is no equals sign after the '>', then the '>' is a single operator and the next character is the start of something else.
            tok = "Relational Operator";
            //cout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Write to console.
            fout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;		//Write to file.
        }
        //If ch = '<', then lexeme could be '<', '<=', '<>', '<-'.
        else if(ch == '<')
        {
            lex = lex + ch;			//lex = <
            tempPeek = fin.peek();	//Check next character.
            
            //If token is a double character relational operators.
            if(tempPeek == '=' || tempPeek == '>')
            {
                fin.get(ch);		//Get the '=' or '>'
                lex = lex + ch;		//lex = '<=' or '<>'
                tok = "Relational Operator";
            }
            //If token is a double character general operator.
            else if(tempPeek == '-')
            {
                fin.get(ch);		//Get the '-'
                lex = lex + ch;		//lex = '<-'
                tok = "Operator";
            }
            //If peeked character is not a '=', '>', or '-', then '>' is a single character relational operator and peeked character is the start of new lexeme.
            else
            {
                tok = "Relational Operator";
            }
            
            //cout<<"Lexeme = "<<setw(20)<<left<<lex<<"\tToken = "<<tok<<endl;	//Write to console.
            fout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;		//Write to file.
            ch = ' ';
        }
        //If, for some reason, we end up in this loop without a branching operator.
        else
            cout<<"ERROR HAS OCCURED"<<endl;
    }
    //If there is an illegal character inputted from the stream, then we mark it with "ILLEGAL".
    else
    {
        //Ensures that a whitespace character is not accidentally marked as "Invalid" if it is located at the end of the file.
        if(!isspace(ch))
        {
            known = false;
            tok = "Invalid";
            //cout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;	//Write to console.
            fout<<"Lexeme = "<<setw(20)<<left<<ch<<"\tToken = "<<tok<<endl;		//Write to file.
            ch = ' ';
        }
    }
}//End getTokLex

/*
    Function:   Checks if last character of identifier is an underscore.  If it is, then the identifier is
                marked as "invalid".
    Input:      lex:   The identifier we are checking.
                token: Passed as reference so that we can mark the token as "invalid" if the identifier is 
                       invalid.
    Output:     Change token to "invalid" if identifier ends in underscore.
*/
void LexTok::lastCharIdentifier(const string lex, string& token)
{
    char lastChar;		//Will hold the last character of the string that is passed.
    
    lastChar = lex[lex.length()-1];		//Get last character of lex.
    
    //If last character of string for identifier is an underscore, then it is an invalid identifier.
    if(lastChar == '_')
        token = "Invalid";
}//End lastCharIdentifier

/*
    Function:	Checks if operator is in the array for single operators.  Single operators are
                those that do not branch into different possibilities (such as +).
    Input:      singleOps: The array that holds all of the single operators.
                lex: The operator we are checking against the array.
    Output:	Returns true if operator is found in list, else returns false.
*/
bool LexTok::singleOperatorsLookup(const char singleOps[], const char lex)
{
    bool found = false;		//Flag that tells if operator is found in array or not.
    
    for(int i = 0; i < 12; i++)
    {
        if(lex == singleOps[i])
            found = true;
    }
    
    return found;
}//End singleOperatorsLookup

/*
    Function:	Checks if operator is in the array for branching operators.  Branching operators are those
                that may lead to different lexeme possibilities (such as <).
    Input:      branchOps: Array that holds all branching operators.
    Output:     Returns true if operator is found in list, else false.
*/
bool LexTok::branchingOperatorsLookup(const char branchOps[], const char lex)
{
    bool found = false;		//Flag that tells if operator is found in array or not.
    
    for(int i = 0; i < 3; i++)
    {
        if(lex == branchOps[i])
            found = true;
    }
    
    return found;
}//End branchingOperatorsLookup

/*
    Function:   Checks if identifier is in the keyword array.  If it is, then the identifier is a keyword,            
                else it is an identifier.
    Input:      keywords: Array that holds all of the keywords.
                lex: Identifier we are checking against the array.
    Output:     Returns true if identifier is found in list, else returns false.
 */
bool LexTok::keywordLookup(const string keywords[], const string lex)
{
    bool found = false;		//Flag that tells if identifier is found in array or not.
    
    for(int i = 0; i < 17; i++)
    {
        if(lex == keywords[i])
            found = true;
    }
    
    return found;
}//End keywordLookup

/* End of LexTok.cpp */
