//  Created by Christopher Dancarlo Danan and Yuri Van Steenburg based on Chris's code on November 14, 2014.
//  Copyright (c) 2014 Christopher Dancarlo Danan & Yuri Van Steenburg. All rights reserved.

//Name:				Christopher Dancarlo Danan
//					Yuri Van Steenburg
//Compiler Used:	Visual Studio 2010
//***********************************************************************************
//Date Began:		November 5, 2014
//Due Date:			November 14, 2014
//Class:			CPSC 323; Compilers
//Professor:		Mark Murphy
//Project:			Assembly 
//File Name:		main.cpp
//Purpose:			Translate the source code to assembly instructions.
//References:		http://www.cplusplus.com/reference/stack/stack/
//					http://cse.csusb.edu/dick/samples/stl.html#Stacks
//					Sally helped me understand the sample given in class.
//					Professor Mark Murphy's notes and lectures.
//					Yuri helped locate the most frustrating error - a single equal sign.
		
#include <iomanip>
#include <stack>	//Use stack class.

#include "LexTok.h"

LexTok LK; // declare the object of class LexTok as a global variable so that all functions have access to it.

//Prototypes
void Program();
bool symbolTableLookup(string);
void LexMatch(string, string);
void TokMatch(string, string);
void OptDeclList();
void Decl();
void OptFuncList();
void Type();
void VarList(string);
void OptVars(string);
void Func();
void OptParamList();
void Param();
void OptParams();
void OptStmtList();
void Stmt();
void Assign();
void Read();
void Write();
void If();
void While();
void Expr();
void Term();
void EPrime();
void Factor();
void TPrime();
void FuncCall();
void OptArgList();
void OptArgs();
void Condition();
void OptElseIf();
void OptElse();
void RelOp(string&);

string symbolTable[20];			//Symbol table holds declared variables.
int v = 0;						//Increment variable for symbol table.
	
string instructionArray[100];	//Array to hold instructions.  Their line number will be the index + 1.
int d = 0;						//Increment variable for instructionArray.

//While-loop stacks:
stack <int> forwardStack;		//Stack to hold addresses to jump forward to, thus skipping a block of code.
stack <int> backwardStack;		//Stack to hold addresses to jump backward to, thus repeating a block of code.

//If/Ifelse/Else stacks:
stack <int> ifStack;			
stack <int> ifJUMPStack;

int offset = 300;					//Offsets the symbolTable by 300.

int main()
{
    if(LK.fin.is_open())
    {
        //Get the first lexeme-token pair to start the parser.
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        
        Program(); //Start the parser.
        
        //If we have reached eof at this point, then the program was a success.
        //if(LK.fin.eof())
            //cout<<"Program => program OtpDeclList OptFuncList begin OptStmtList end."<<endl;
			//cout<<"CORRECT!!!\n";
    }
    else
    {
        cout<<"ERROR: Cannot open file."<<endl<<"Exiting program. Goodbye."<<endl;
        exit(1);
    }
    
    //Close the I/O files.
    LK.fin.close();
    LK.fout.close();
    
	cout<<"Symbol Table"<<endl<<"------------------------------"<<endl;
	for(int i = 0; i < v; i++)
	{
		cout<<symbolTable[i]<<" "<<(i + offset)<<endl;
	}

	cout<<endl;

	cout<<"Instructions"<<endl<<"---------------------------------"<<endl;
	for(int a = 0; a < d; a++)
	{
		cout<<a+1<<instructionArray[a]<<endl;
	}

    return 0;
}

//Parser starts here.
void Program()
{
    LexMatch("program", LK.lexeme);
    OptDeclList();
    OptFuncList();
    LexMatch("begin", LK.lexeme);
	OptStmtList();
    LexMatch("end", LK.lexeme);
    LexMatch(".", LK.lexeme);
    
	instructionArray[d] = " NOP";
	d++;

	//cout<<"newLineCount = "<<LK.newLineCount<<endl;
    return;
}

//Want to check if lexeme is in symbol table already.
//Returns true if lex is in symbol table, else returns false.
//Input:	lex: Identifier we are checking to see if it is in symbol table.
//Output:	Return true if lex is in symbolTable, else return false.
bool symbolTableLookup(string lex)
{
	for(int i = 0; i < v; i++)
	{
		//If variable already exists in symbol table, then it was already declared.
		if(symbolTable[i] == lex)
		{
			return true;
		}
	}
	return false;
}

//Purpose:	Checks to see if current lexeme matches with the string we send in.
//Input:	s: The string we are matching to the current lexeme.
//			currentLexeme: The current lexeme in the stream.
//Output:	If s does not match with currentLexeme, then output error message and exit(1).
//			Else, simply return.
void LexMatch(string s, string currentLexeme)
{
    if(currentLexeme != s)
    {
        cout<<"ERROR: current lexeme ("<<currentLexeme<<") does not match with \""<<s<<"\".  Exiting program."<<endl;
        exit(1);
    }
    else
    {
        //cout<<currentLexeme<<" matches with "<<s<<endl;
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
    }
}

//Purpose:	Checks to see if current token matches with the string we send in.
//Input:	s: The string we are matching to the current token.
//			currentToken: The current token in the stream.
//Output:	If s does not match with currentToken, then output error message and exit(1).
//			Else, simply return.
void TokMatch(string s, string currentToken)
{
    if(currentToken != s)
    {
        cout<<"ERROR: current token ("<<currentToken<<") does not match with \""<<s<<"\".  Exiting program."<<endl;
        exit(1);
    }
    else
    {
        //cout<<currentToken<<" matches with "<<s<<endl;
        //LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
    }
}

void OptDeclList()
{
    //If the next lexeme is "function" or "begin", then OptDeclList is NULL and we return to parent function (main).
    if(LK.lexeme == "function" || LK.lexeme == "begin")
    {
        //cout<<"OptDeclList => null"<<endl;
        return;
    }
    //If OptDeclList is not NULL, then we call its children.
    Decl();
    OptDeclList();
    
    //cout<<"OptDeclList => Decl OptDeclList"<<endl;
    return;
}

void Decl()
{
	string call = "Decl";	//Tell VarList that we called it from Decl.

    Type();
    VarList(call);
    LexMatch(";", LK.lexeme);
    
    //cout<<"Decl => Type VarList ;"<<endl;
}

void OptFuncList() //Done
{
	//If lexeme = begin, then OptFuncList is null.
    if(LK.lexeme == "begin")
    {
        //cout<<"OptFuncList => null"<<endl;
        return;
    }
	//If not null, then call children.
    Func();
    OptFuncList();

	//cout<<"OptFuncList => Func OptFuncList"<<endl;
}

void Func() //Done
{
    LexMatch("function", LK.lexeme);
    TokMatch("ident", LK.token);
    //cout<<"ident => "<<LK.lexeme<<endl;
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
    LexMatch("(", LK.lexeme);
    OptParamList();
    LexMatch(")", LK.lexeme);
    LexMatch(":", LK.lexeme);
    Type();
    LexMatch(";", LK.lexeme);
    OptDeclList();
    LexMatch("begin", LK.lexeme);
    OptStmtList();
    LexMatch("end", LK.lexeme);
    
    //cout << "Func => function ( OPtParamList ) : Type : OptDecList begin OptStmtList end" << endl;
}

void OptParamList() //Done
{
	//If lexeme = ")"
    if(LK.lexeme == ")")
    {
        //cout<<"OptParamList => null"<<endl;
        return;
    }
    Param();
    OptParams();
    //cout << "OptParamList => Param OptParams"<< endl;
}

void OptParams() //Done
{
    if(LK.lexeme == ")")
    {
        //cout<<"OptParams => null"<<endl;
        return;
    }
    LexMatch(",", LK.lexeme);
    Param();
    OptParams();
    //cout << "OptParms => , Param OptParams"<< endl;
}

void Param() //Done
{
    Type();
    TokMatch("ident", LK.token);
    //cout<<"ident => "<<LK.lexeme<<endl;
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
    
    //cout << "Param => Type ident"<< endl;
    
}

void Type() //Done
{
    if(LK.lexeme == "int" || LK.lexeme == "real" || LK.lexeme == "bool")
    {
        //cout<<"Type => "<<LK.lexeme<<endl;
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        return;
    }
    else
    {
        cout<<"ERROR: Expected an 'int', 'real', or 'bool', but received ("<<LK.lexeme<<").\nExiting program."<<endl;
        exit(1);
    }
    
    return;
}

void VarList(string caller) //Done
{
	bool inSymbolTable;				//Holds true if LK.lexeme is in symbol table, else holds false.
	int addressOfIdent;				//Holds address of the identifier.

	//cout<<"VarList called. Caller is "<<caller<<endl;
    TokMatch("ident", LK.token);
    //cout<<"ident => "<<LK.lexeme<<endl;

	//CHECK IF IN SYMBOL TABLE.
	//	If it is, then ERROR: Variable already declared
	//	Else, add to symbol table
	inSymbolTable = symbolTableLookup(LK.lexeme);
	
	//If caller of VarList is Decl, then we want to make sure that identifier is not in symbol table yet.
	if(caller == "Decl")
	{
		//inSymbolTable = symbolTableLookup(LK.lexeme);
		//If true, then we are declaring a new variable, but that variable has already been previously declared.
		//We do not allow duplicates, so this is ERROR.
		if(inSymbolTable == true)
		{
			cout<<"Error at line number "<<LK.newLineCount<<": "<<LK.lexeme<<" declared more than once."<<endl;
			exit(1);
		}
		//Identifier is not in symbol table yet, so we can declare it.
		else
		{
			symbolTable[v] = LK.lexeme;
			v++;
		}

		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	}
	//If Read called VarList, then we WANT the identifier to be in symbol table already.
	else if(caller == "Read")
	{
		//inSymbolTable = symbolTableLookup(LK.lexeme);
		//If identifier is not in symbol table, then we cannot read from it.
		//So, this is an ERROR.
		if(inSymbolTable == false)
		{
			cout<<"Error at line number "<<LK.newLineCount<<": variable "<<LK.lexeme<<" was not declared."<<endl;
			exit(1);
		}
		//If identifier is in symbol table, then we can read from it.
		else
		{
			instructionArray[d] = " PUSHI";
			d++;

			//Find the address of the identifier.
			for(int i = 0; i < d; i++)
				if(symbolTable[i] == LK.lexeme)
					addressOfIdent = i + offset;

			instructionArray[d] = " POPM " + to_string(static_cast<long long>(addressOfIdent));
			d++;
		}

		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	}

	OptVars(caller);
    
    //cout<<"VarList => ident OptVars"<<endl;
}

void OptVars(string caller) //Done
{
	bool inSymbolTable;				//Holds true if LK.lexeme is in symbol table, else holds false.
	int addressOfIdent;				//Holds address of the identifier.

    if(LK.lexeme == ";" || LK.lexeme == ")")
    {
        //cout<<"OptVars => null"<<endl;
        return;
    }
    
    LexMatch(",", LK.lexeme);
    TokMatch("ident", LK.token);
    //cout<<"ident => "<<LK.lexeme<<endl;

	inSymbolTable = symbolTableLookup(LK.lexeme);
	//Same as VarList.
	if(caller == "Decl")
	{
		//test = symbolTableLookup(LK.lexeme);
		if(inSymbolTable == true)
		{
			cout<<"Error at line number "<<LK.newLineCount<<": "<<LK.lexeme<<" declared more than once."<<endl;
			exit(1);
		}
		else
		{
			symbolTable[v] = LK.lexeme;
			v++;
		}
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	}
	else if(caller == "Read")
	{
		if(inSymbolTable == false)
		{
			cout<<"Error at line number "<<LK.newLineCount<<": variable "<<LK.lexeme<<" was not declared."<<endl;
			exit(1);
		}
		else
		{
			instructionArray[d] = " PUSHI";
			d++;

			for(int i = 0; i < d; i++)
				if(symbolTable[i] == LK.lexeme)
					addressOfIdent = i + offset;

			instructionArray[d] = " POPM " + to_string(static_cast<long long>(addressOfIdent));
			d++;
		}
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	}

    OptVars(caller);

	//cout<<"OptVars => , ident OptVars"<<endl;
}

void OptStmtList() //Done
{
    if(LK.lexeme == "end")
    {
        //cout<<"OptStmtList => null"<<endl;
        return;
    }
    Stmt();
    OptStmtList();
    
    //cout << "OptStmtList => Stmt OptStmtList" << endl;
    
}

void Stmt() //Done
{
    if (LK.token == "ident")
	{		
        Assign();
		//cout<<"Stmt => Assign"<<endl;
	}
    else if (LK.lexeme == "read")
	{
        Read();
		//cout<<"Stmt => Read"<<endl;
	}
    else if (LK.lexeme == "write")
	{
        Write();
		//cout<<"Stmt => Write"<<endl;
	}
    else if (LK.lexeme == "if")
	{
        If();
		//cout<<"Stmt => If"<<endl;
	}
    else if (LK.lexeme == "while")
	{
        While();
		//cout<<"Stmt => While"<<endl;
	}
    else
    {
        //cout << "Error: expected ident, read, write, while" << endl;
        exit(1);
    }
    
    //cout << "Stmt => Assign | Read | Write | If | While" << endl;
}

void Assign() //Done
{
	bool inSymbolTable;		//True if identifier is in symbol table, else false.
	int identAddress = -1;	//Holds address of identifier.

	
    //cout<<"ident => "<<LK.lexeme<<endl;
	inSymbolTable = symbolTableLookup(LK.lexeme);
	//cout<<LK.lexeme<<" returned "<<inSymbolTable<<endl;
	if(inSymbolTable == true)
	{
		//cout<<"Lexeme "<<LK.lexeme<<" is in TRUE and inSymbolTable = "<<inSymbolTable<<endl;
		//Find address of identifier.
		for(int i = 0; i < v; i++)
			if(symbolTable[i] == LK.lexeme)
				identAddress = i + offset;
	}
	else
	{
		cout<<"Error at line number "<<LK.newLineCount<<": variable "<<LK.lexeme<<" was not declared."<<endl;
		exit(1);
	}
	
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
    LexMatch("<-", LK.lexeme);
    Expr();
    LexMatch(";", LK.lexeme);
	
	instructionArray[d] = " POPM " + to_string(static_cast<long long>(identAddress));
	d++;
	
    //cout<<"POPM "<<identAddress<<endl;
    //cout << "Assign => ident <-  Expr ;" << endl;
}

void Read() //Done
{
	string call = "Read";
	//Current lexeme is "read", so get the next one.
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	LexMatch("(", LK.lexeme);
	VarList(call);
	LexMatch(")", LK.lexeme);
	LexMatch(";", LK.lexeme);

	//cout<<"Read => read ( VarList ) ;"<<endl;
}

void Write() //Done
{
	//Current lexeme is "write", so get the next one.
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	LexMatch("(", LK.lexeme);
	Expr();
	LexMatch(")", LK.lexeme);
	LexMatch(";", LK.lexeme);

	instructionArray[d] = " POPO";
	d++;

	//cout<<"Write => write ( Expr ) ;"<<endl;
}

void If() //Done
{
	int addressOfJMPZ;
	int sizeOfStack;
	int test;

	//Current lexeme is "if", so we have to get the next one.
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	LexMatch("(", LK.lexeme);
	Condition(); 
	LexMatch(")", LK.lexeme);

	instructionArray[d] = " JMPZ ???"; //Jump to next else-if or else statement if condition is false.
	ifStack.push(d);
	d++;

	LexMatch("begin", LK.lexeme);
	OptStmtList(); 
	LexMatch("end", LK.lexeme);

	if(LK.lexeme == "elseif" || LK.lexeme == "else")
	{
		instructionArray[d] = " JUMP ???"; //Jump over OptElseIf and OptElse if If statement executed.
		ifJUMPStack.push(d);
		d++;
	}
	
	//Patch JMPZ to skip over if-statement.
	addressOfJMPZ = ifStack.top();
	ifStack.pop();
	instructionArray[addressOfJMPZ] = " JMPZ " + to_string(static_cast<long long>(d+1));

	OptElseIf(); 
	OptElse(); 

	
	//cout<<"Size of ifJUMPStack is "<<ifJUMPStack.size()<<endl;
	sizeOfStack = ifJUMPStack.size();
	for(int i = 0; i < sizeOfStack; i++)
	{
		test = ifJUMPStack.top();
		instructionArray[test] = " JUMP " + to_string(static_cast<long long>(d+1));
		ifJUMPStack.pop();
	}
	
	//cout<<"If => if ( Condition ) begin OptStmtList end OptElseIf OptElse"<<endl;
}

void OptElseIf() //Done
{
	int addressOfJMPZ;

	if(LK.lexeme == "else" || 
		LK.lexeme == "end" || 
		LK.token == "ident" || 
		LK.lexeme == "read" ||
		LK.lexeme == "write" ||
		LK.lexeme == "if" || 
		LK.lexeme == "while")
	{
		//cout<<"OptElseIf => null"<<endl;
		return;
	}
	
	LexMatch("elseif", LK.lexeme);
	LexMatch("(", LK.lexeme);
	Condition();
	LexMatch(")", LK.lexeme);
	LexMatch("begin", LK.lexeme);

	//JMPZ this if the condition is false.
	instructionArray[d] = " JMPZ ???"; //Jump to next else-if or else statement if condition is false.
	ifStack.push(d);
	d++;

	OptStmtList();
	LexMatch("end", LK.lexeme);

	instructionArray[d] = " JUMP ???"; //Jump over OptElseIf and OptElse if If statement executed.
	ifJUMPStack.push(d);
	d++;

	//Patch JMPZ to skip over if-statement.
	addressOfJMPZ = ifStack.top();
	ifStack.pop();
	instructionArray[addressOfJMPZ] = " JMPZ " + to_string(static_cast<long long>(d+1));

	OptElseIf();

	//cout<<"OptElseIf => elseif ( Condition ) begin OptStmtList end OptElseIf"<<endl;

}

void OptElse() //Done
{
	if(LK.lexeme == "end" || 
		LK.token == "ident" || 
		LK.lexeme == "read" ||
		LK.lexeme == "write" ||
		LK.lexeme == "if" || 
		LK.lexeme == "while")
	{
		//cout<<"OptElse => null"<<endl;
		return;
	}
	
	LexMatch("else", LK.lexeme);
	LexMatch("begin", LK.lexeme);
	OptStmtList();
	LexMatch("end", LK.lexeme);

	//cout<<"OptElse => else begin OptStmtList end"<<endl;
}
void While()
{
	int jumpBack;
	//int jumpForward;
	int jmpzAddress;

	//Current lexeme is "while", so get the next one.
    LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	LexMatch("(", LK.lexeme);
	Condition();
	LexMatch(")", LK.lexeme);
	LexMatch("begin", LK.lexeme);
	
	//The JUMPZ will go here.  This is the point where the loop statements execute.  If the condition was false,
	//we want to skip over these lines.
	instructionArray[d] = " JMPZ ???";  //Notice, we do not know what line to jump to yet, so we leave that as a blank for now (indicated with the ???).
	forwardStack.push(d);
	d++;

	OptStmtList();
	LexMatch("end", LK.lexeme);

	jumpBack = backwardStack.top();
	backwardStack.pop();
	instructionArray[d] = " JUMP " + to_string(static_cast<long long>(jumpBack));
	d++;

	//instructionArray[d] = " NOP";
	//jumpForward = d+1;
	//d++;
	jmpzAddress = forwardStack.top();
	forwardStack.pop();
	instructionArray[jmpzAddress] = " JMPZ " + to_string(static_cast<long long>(d+1));

	//cout<<"While => while ( Condition ) begin OptStmtList end"<<endl;
}

void Condition() //Done
{
	string relop; //Will be used to check what assembly instruction to store in instructionArray.

	Expr();

	//At this point, the address of the first expression in the loop is here. So, we need to store the address
	//on the backward stack so we can have a reference to jump back to this particular spot.
	backwardStack.push(d);

	RelOp(relop);
	Expr();

	if(relop == ">")
	{
		instructionArray[d] = " GRTR";
		d++;
	}
	else if(relop == "<")
	{
		instructionArray[d] = " LESS";
		d++;
	}
	else if(relop == "=")
	{
		instructionArray[d] = " EQL";
		d++;
	}
	else if(relop == ">=")
	{
		instructionArray[d] = " LESS";
		d++;
		instructionArray[d] = " NOT";
		d++;
	}
	else if(relop == "<=")
	{
		instructionArray[d] = " GRTR";
		d++;
		instructionArray[d] = " NOT";
		d++;
	}
	else if(relop == "<>")
	{
		instructionArray[d] = " EQL";
		d++;
		instructionArray[d] = " NOT";
		d++;
	}
	//cout<<"Condition => Expr RelOp Expr"<<endl;
}

void RelOp(string &relop) //Done
{
	//cout<<"Current lexeme is "<<LK.lexeme<<endl;
	if(	LK.token == "Relational Operator")
	{
		relop = LK.lexeme;		//Will be used to check what assembly instruction to store in instructionArray.
		//cout<<"RelOp => "<<LK.lexeme<<endl;
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
	}
	else
	{
		cout<<"ERROR: Expected a relational operator, but received ["<<LK.lexeme<<"]"<<endl;
		exit(1);
	}
}
void Expr() //Done
{
    Term(); //Will give the first operand for addition
    EPrime(); //Will give the second operand for addition
    
    //cout << "Expr => Term EPrime" << endl;
}

void EPrime() //Done
{
    if(LK.lexeme == ";" || 
		LK.lexeme == ")" ||
		LK.lexeme == "<" ||
		LK.lexeme == ">" ||
		LK.lexeme == "<=" ||
		LK.lexeme == ">=" ||
		LK.lexeme == "=" ||
		LK.lexeme == "<>")
	{
		//cout<<"EPrime => null"<<endl;
		return;
	}
	else if(LK.lexeme == "+")
	{
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
		Term();
		instructionArray[d] = " ADD";
		d++;
		//cout<<"ADD"<<endl;
		EPrime();

		//cout<<"EPrime => + Term EPrime"<<endl;
	}
	else if(LK.lexeme == "-")
	{
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
		Term();
		instructionArray[d] = " SUB";
		d++;
		//cout<<"SUB"<<endl;
		EPrime();

		//cout<<"EPrime => - Term EPrime"<<endl;
	}
	else
	{
		cout<<"ERROR: Expected + or -, but received ["<<LK.lexeme<<"]\n";
		exit(1);
	}
}
    
void Term() //Done
{
    Factor();
    TPrime();
    
    //cout << "Term => Factor TPrime" << endl;
}

void TPrime() //Done
{
    if(LK.lexeme == "+" || 
		LK.lexeme == "-" || 
		LK.lexeme == ";" || 
		LK.lexeme == ")" ||
		LK.lexeme == "<" ||
		LK.lexeme == ">" ||
		LK.lexeme == "<=" ||
		LK.lexeme == ">=" ||
		LK.lexeme == "=" ||
		LK.lexeme == "<>")
	{
		//cout << "TPrime => null" << endl;
		return;
	}
	else if(LK.lexeme == "*")
	{
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
		Factor();
		instructionArray[d] = " MUL";
		d++;
		//cout<<"MUL"<<endl;
		TPrime();
	
		//cout<<"TPrime => * Factor TPrime"<<endl;
	}
	else if(LK.lexeme == "/")
	{
		LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
		Factor();
		instructionArray[d] = " DIV";
		d++;
		//cout<<"DIV"<<endl;
		TPrime();

		//cout<<"TPrime => / Factor TPrime"<<endl;
	}
	else
	{
		cout<<"ERROR: Expected a '*' or '/' (if factor) or  but received ["<<LK.lexeme<<"]\n";
		exit(1);
	}
}

void Factor() //Done
{
	int addressOfIdent;	//Holds the identifier's address on the symbol table.
	bool inSymbolTable;	//True if identifier is in symbol table, else false.

    if(LK.token == "ident")
    {
        //cout<<"ident => "<<LK.lexeme<<endl;
		//cout<<"Lexeme is "<<LK.lexeme<<endl;
		inSymbolTable = symbolTableLookup(LK.lexeme);
		if(inSymbolTable == true) //Lexeme is already declared in symbol table
		{
			for(int i = 0; i < v; i++)
				if(symbolTable[i] == LK.lexeme)
					addressOfIdent = i + offset;

			//cout<<"Lexeme ["<<LK.lexeme<<"] is in the symbol table at address "<< addressOfIdent<<endl;
			instructionArray[d] = " PUSHM " + to_string(static_cast<long long>(addressOfIdent));
			d++;
			//cout<<"PUSHM "<<addressOfIdent<<endl;
		}
		else
		{
			cout<<"Error at line number "<<LK.newLineCount<<": variable "<<LK.lexeme<<" was not declared."<<endl;
			exit(1);
		}

        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        if(LK.lexeme == "(")
        {
            LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
            FuncCall();
			//cout<<"Factor => FuncCall"<<endl;
			return;
        }
        //cout << "Factor => ident" << endl;
        return;
    }
    else if(LK.token == "int")
    {
		instructionArray[d] = " PUSHV " + LK.lexeme;
		d++;
		//cout<<"PUSHV "<<LK.lexeme<<endl;
        //cout<<"int => "<<LK.lexeme<<endl;
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        //cout << "Factor => int" << endl;
        return;
    }
    else if(LK.token == "real")
    {
		instructionArray[d] = " PUSHI";
		d++;
		//cout<<"PUSHI"<<endl;
        //cout<<"ident => "<<LK.lexeme<<endl;
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        //cout << "Factor => real" << endl;
        return;
    }
    else if(LK.lexeme == "true")
    {
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        //cout << "Factor => true" << endl;
        return;
    }
    else if(LK.lexeme == "false")
    {
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        //cout << "Factor => false" << endl;
        return;
    }
    else if(LK.lexeme == "(")
    {
        LK.getTokLex(LK.token, LK.lexeme, LK.fin, LK.fout, LK.known, LK.ch);
        Expr();
        LexMatch(")", LK.lexeme);
        //cout << "Factor => (Expr)" << endl;
        return;
    }
    else
    {
        cout << "ERROR: Expected ident, int, real, true, false, or (Expr). Received [" << LK.lexeme << "] "<< endl;
        exit(1);
    }
}

void FuncCall() //Done
{
    OptArgList();
    LexMatch(")", LK.lexeme);
    
    //cout << "FuncCall => ident ( OptArgList )" << endl;
}

void OptArgList() //Done
{
    if (LK.lexeme == ")")
    {
        //cout << "OptArgList => null" << endl;
        return;
    }
    Expr();
    OptArgs();
    
    //cout << "OptArgList => Expr OptArgs" << endl;
}

void OptArgs() //Done
{
    if (LK.lexeme == ")")
    {
        //cout << "OptArgs => null" << endl;
        return;
    }
    LexMatch(",", LK.lexeme);
    Expr();
    OptArgs();
    
    //cout << "OptArgs => , Expr OptArgs" << endl;

}
//t
//fflvd

