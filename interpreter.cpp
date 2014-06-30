/*******************************************************************************

    Author: Abd Albasset Almamou

    Implementation of an interpreter which can execute a simplified assembler
    subset.The interpreter has a stack, three registers (ax,bx,cx), it can process
    string literals and numerical values”. Commands are shown in the following:

    Command    Arguments        Description
    mov        <dst>, <src>     Moves content of <src> into <dst>
    push       <src>            Moves <src> onto stack
    pop        <dst>            Moves current stack top most element to <dst>
    add        <dst>, <src>     Calculates <dst> = <dst> + <src>
    sub        <dst>, <src>     Calculates <dst> = <dst> - <src>
    div        <dst>, <src>     Calculates <dst> = <dst> / <src>
    mul        <dst>, <src>     Calculates <dst> = <dst> * <src>
    print      <src>            Outputs content of <src> to console

    Hint:
    <src> = register, string literal, numerical value
    <dst> = register

*******************************************************************************/

#include <iostream>
#include <stack>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
using namespace std;

/*******************************************************************************

    Error Handling Classes

*******************************************************************************/

class StackUnderFlow : public runtime_error
{
public:
    StackUnderFlow() : runtime_error("stack underflow!. No need to check args validity.")
    {}
};

class StackOverFlow : public runtime_error
{
public:
    StackOverFlow() : runtime_error("stack overflow!")
    {}
};

class WrongDes : public runtime_error
{
public:
    WrongDes() : runtime_error("destination is not a register.")
    {}
};

class MissArgError : public runtime_error
{
public:
    MissArgError() : runtime_error("missing args")
    {}
};

class CommandError : public runtime_error
{
public:
    CommandError() : runtime_error("unknown command")
    {}
};

class WrongArg : public runtime_error
{
public:
    WrongArg() : runtime_error("wrong argument.")
    {}
};

class MissQuotes : public runtime_error
{
public:
    MissQuotes() : runtime_error("string misses quotes")
    {}
};

class IncompatibleTypes : public runtime_error
{
public:
    IncompatibleTypes() : runtime_error("incompatible arguments types.")
    {}
};
class SubstrationStringError : public runtime_error
{
public:
    SubstrationStringError() : runtime_error("can't subtract a string.")
    {}
};
class DivisionStringError : public runtime_error
{
public:
    DivisionStringError() : runtime_error("can't divide by a string.")
    {}
};
class MultiplicationStringError : public runtime_error
{
public:
    MultiplicationStringError() : runtime_error("can't multiply strings.")
    {}
};

class DivideByZero : public runtime_error
{
public:
    DivideByZero() : runtime_error("impossible to divide by zero.")
    {}
};

/*******************************************************************************

    Forward methods definition

*******************************************************************************/

void catchMsg ( exception &except );
string trimChars ( string const &str, char char_to_remove );

/*******************************************************************************

    Definition of Register class

*******************************************************************************/

enum Tag {STR, NUM};
class Reg
{
private:
    string str;
    float  val;
public:
    enum Tag data_type;
    Reg()
    {
        data_type = STR;
        str = "";
        val = 0.0;
    }
    ~Reg() {}
    string getStr()
    {
        return str;
    }
    float getVal()
    {
        return val;
    }
    // overloading assignment operator
    Reg operator=( string const &srcStr )
    {
        data_type = STR;
        str = srcStr;
        return *this;
    }
    Reg operator=( float const &srcVal )
    {
        data_type = NUM;
        val = srcVal;
        return *this;
    }
    Reg operator=( Reg const &srcReg )
    {
        switch (srcReg.data_type)
        {
            case STR:
            data_type = STR;
            str = srcReg.str;
            break;
            case NUM:
            data_type = NUM;
            val = srcReg.val;
            break;
            default:
            break;
        }
        return *this;
    }
    // overloading addition operator
    Reg operator+( string const &srcStr )
    {
        try
        {
            if ( data_type != STR )
            {
                throw IncompatibleTypes();
            }
            else
            {
                str = str + srcStr;
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator+( float const &srcVal )
    {
        try
        {
            if ( data_type != NUM )
            {
                throw IncompatibleTypes();
            }
            else
            {
                val = val + srcVal;
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator+( Reg const &srcReg )
    {
        try
        {
            if ( data_type != srcReg.data_type )
            {
                throw IncompatibleTypes();
            }
            else
            {
                switch ( data_type )
                {
                    case STR:
                    {
                        if ( str == "" )
                        {
                            if ( srcReg.str == "" )
                            {
                                str = "";
                            }
                            else
                            {
                                str = trimChars(srcReg.str, '"');
                            }
                        }
                        else
                        {
                            if ( srcReg.str == "" )
                            {
                                str = trimChars(str,'"');
                            }
                            else
                            {
                                string str1 = trimChars(str,'"');
                                string str2 = trimChars(srcReg.str, '"');
                                str = str1+str2;
                            }
                        }
                        break;
                    }
                    case NUM:
                    val = val + srcReg.val;
                    break;
                }
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    // overloading subtraction operator
    Reg operator-( string const & )
    {
        try
        {
            switch ( data_type )
            {
                case STR:
                throw SubstrationStringError();
                break;
                case NUM:
                throw IncompatibleTypes();
                break;
                default:
                break;
            }
        }
        catch (SubstrationStringError & subtract_string)
        {
            catchMsg(subtract_string);
            exit(EXIT_FAILURE);
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator-( float const &srcVal )
    {
        try
        {
            switch ( data_type )
            {
                case STR:
                throw IncompatibleTypes();
                break;
                case NUM:
                val = val - srcVal;
                break;
                default:
                break;
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator-( Reg &srcReg )
    {
        try
        {
            if ( data_type != srcReg.data_type )
            {
                throw IncompatibleTypes();
            }
            else
            {
                switch ( srcReg.data_type )
                {
                    case STR:
                    throw SubstrationStringError();
                    break;
                    case NUM:
                    val = val - srcReg.val;
                    break;
                }
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        catch (SubstrationStringError &subtract_string)
        {
            catchMsg(subtract_string);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    // overloading division operator
    Reg operator/( string const & )
    {
        try
        {
            throw DivisionStringError();
        }
        catch (DivisionStringError & divide)
        {
            catchMsg (divide);
            exit(EXIT_FAILURE);
        }
    }
    Reg operator/( float const &srcVal )
    {
        try
        {
            if ( srcVal == 0 )
            {
                throw DivideByZero();
            }
            else
            {
                switch (data_type)
                {
                    case STR:
                    throw DivisionStringError();
                    break;
                    case NUM:
                    val = val / srcVal;
                }
            }
        }
        catch (DivisionStringError &divide_string)
        {
            catchMsg (divide_string);
            exit(EXIT_FAILURE);
        }
        catch (DivideByZero &divide_zero)
        {
            catchMsg (divide_zero);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator/( Reg const &srcReg )
    {
        try
        {
            if ( data_type != srcReg.data_type )
            {
                throw IncompatibleTypes();
            }
            else
            {
                switch (data_type)
                {
                    case STR:
                    throw DivisionStringError();
                    break;
                    case NUM:
                    {
                        if ( srcReg.val == 0 )
                        {
                            throw DivideByZero();
                        }
                        else
                        {
                            val = val / srcReg.val;
                        }
                    break;
                    }
                    default:
                    break;
                }
            }
        }
        catch (DivisionStringError &divide_string)
        {
            catchMsg(divide_string);
            exit(EXIT_FAILURE);
        }
        catch (DivideByZero &divide_zero)
        {
            catchMsg(divide_zero);
            exit(EXIT_FAILURE);
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    // overloading multiplicaiton operator
    Reg operator*( string const & )
    {
        try
        {
            switch (data_type)
            {
                case STR:
                throw MultiplicationStringError();
                break;
                case NUM:
                throw IncompatibleTypes();
                break;
                default:
                break;
            }
        }
        catch (MultiplicationStringError &mul_string)
        {
            catchMsg(mul_string);
            exit(EXIT_FAILURE);
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator*( float const &srcVal )
    {
        try
        {
            switch(data_type)
            {
                case STR:
                throw IncompatibleTypes();
                break;
                case NUM:
                val = val * srcVal;
                break;
                default:
                break;
            }
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
    Reg operator*( Reg const &srcReg )
    {
        try
        {
            if ( data_type != srcReg.data_type )
            {
                throw IncompatibleTypes();
            }
            else
            {
                switch (data_type)
                {
                    case STR:
                    throw MultiplicationStringError();
                    break;
                    case NUM:
                    {
                        if ( srcReg.data_type == STR )
                        {
                            throw IncompatibleTypes();
                        }
                        else
                        {
                            val = val * srcReg.val;
                        }
                        break;
                    }
                    default:
                    break;
                }
            }
        }
        catch (MultiplicationStringError &mul_string)
        {
            catchMsg(mul_string);
            exit(EXIT_FAILURE);
        }
        catch (IncompatibleTypes &incompatible)
        {
            catchMsg(incompatible);
            exit(EXIT_FAILURE);
        }
        return *this;
    }
};

/*******************************************************************************

    Global variables and classes' instantiation

*******************************************************************************/

string filename, des, src;
size_t line_num;
Reg ax, bx, cx, regInterface;
stack<Reg> theStack;

/*******************************************************************************

    General purpose methods

*******************************************************************************/

void initVec ( vector<string> &assembly_commands )
{
    assembly_commands.push_back("add");
    assembly_commands.push_back("div");
    assembly_commands.push_back("mov");
    assembly_commands.push_back("mul");
    assembly_commands.push_back("pop");
    assembly_commands.push_back("print");
    assembly_commands.push_back("push");
    assembly_commands.push_back("sub");
}

void catchMsg ( exception &except )
{
    cout << filename << ":" << line_num << ": " << except.what() << endl;
}

string trimChars ( string const &str, char char_to_remove )
{
    size_t strBegin = str.find_first_not_of(char_to_remove);
    size_t strEnd   = str.find_last_not_of(char_to_remove);
    size_t strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

void printReg ( Reg &reg )
{
    switch (reg.data_type)
    {
        case STR:
        {
            string regStr = reg.getStr();
            if ( regStr == "")
            {
                cout << "" << endl;
            }
            else
            {
                cout << trimChars(regStr,'"') << endl;
            }
            break;
        }
        case NUM:
        cout << reg.getVal() << endl;
        break;
        default:
        break;
    }
}

bool isCmdInList ( vector <string> &cmd_list, string &cmd )
{
    vector <string>::iterator result;
    result = find(cmd_list.begin(), cmd_list.end(), cmd );

    return ( result != cmd_list.end() );
}

bool checkNum ( string const &in )
{
    char *end;
    strtof(in.c_str(), &end);
    if ( *end == 'e' || *end == 'E' || *end == 'f' || *end == 'F' )
    {
        return false;
    }
    else
    return ( !in.empty() && (*end == '\0' ||
                             *end == 'e'  ||
                             *end == 'E'  ||
                             *end == 'f'  ||
                             *end == 'F'  ));
}

float getNum ( string const &in )
{
    return atof(in.c_str());
}

bool checkMissingQuotes ( string &str )
{
    bool firstChar = str[0] == '"';
    bool lastChar  = str[str.length()-1] == '"';
    return ( !firstChar || !lastChar );
}

void srcInRegInterface ( string &src )
{
    // is src a reg?
    if ( src == "ax" )
    {
        regInterface.data_type = ax.data_type;
        regInterface = ax;
        return;
    }
    if ( src == "bx" )
    {
        regInterface.data_type = bx.data_type;
        regInterface = bx;
        return;
    }
    if ( src == "cx" )
    {
        regInterface.data_type = cx.data_type;
        regInterface = cx;
        return;
    }
    // is src a num?
    if ( checkNum(src) )
    {
        regInterface.data_type = NUM;
        regInterface = getNum(src);
        return;
    }
    // src is a string
    try
    {
        if ( src == "")
        {
            throw MissArgError();
        }
        if ( checkMissingQuotes(src) || src == "\"" )
        {
            throw WrongArg();
        }
        else
        {
            // encalsulate the src in a regInterface
            regInterface.data_type = STR;
            if ( src == "\"\"" )
            {
                regInterface = "";
            }
            else
            {
                regInterface = src;
            }
        }
    }
    catch ( WrongArg &wrong_arg)
    {
        catchMsg(wrong_arg);
        exit(EXIT_FAILURE);
    }
    catch ( MissArgError &missing_arg)
    {
        catchMsg(missing_arg);
        exit(EXIT_FAILURE);
    }
    return;
}

/*******************************************************************************

    Assembler methods

*******************************************************************************/

void print ( string &src )
{
    // is src Reg?
    if ( src == "ax" || src == "bx" || src == "cx" )
    {
        if ( src == "ax")
        {
            printReg(ax);
            return;
        }
        if ( src == "bx")
        {
            printReg(bx);
            return;
        }
        if ( src == "cx")
        {
            printReg(cx);
            return;
        }
    }
    else
    {
        if ( checkNum(src) )
        {
            cout << getNum(src) << endl;
            return;
        }
        else
        {
            try
            {
                if ( src == "\"\"" )
                {
                    cout << "" << endl;
                    return;
                }
                if ( checkMissingQuotes (src) || src == "\"" )
                {
                    throw MissQuotes();
                }
                else
                {
                    string clean_quotes = trimChars(src,'"');
                    cout << clean_quotes << endl;
                    return;
                }
            }
            catch(MissQuotes &miss_quotes)
            {
                catchMsg(miss_quotes);
                exit(EXIT_FAILURE);
            }
        }
    }
    return;
}

void push ( string &src )
{
    try
    {
        if ( theStack.size() == numeric_limits<size_t>:: max() )
        {
            throw StackOverFlow();
        }
        else
        {
            //src is a Reg?
            if ( src == "ax" || src == "bx" || src == "cx" )
            {
                if ( src == "ax" )
                {
                    theStack.push(ax);
                    return;
                }
                if ( src == "bx" )
                {
                    theStack.push(bx);
                    return;
                }
                if ( src == "cx" )
                {
                    theStack.push(cx);
                    return;
                }
            }
            // src is num?
            if ( checkNum(src) )
            {
                // encapsulate the num in a regInterface
                regInterface.data_type = NUM;
                regInterface = getNum(src);
                theStack.push(regInterface);
                return;
            }
            // src valid string?
            if ( checkMissingQuotes(src) || src == "\"" )
            {
                throw MissQuotes();
            }
            else
            {
                // encapsulate the str in a regInterface
                regInterface.data_type = STR;
                if ( src == "\"\"" )
                {
                    regInterface = "";
                }
                else
                {
                    regInterface = src;
                }
                theStack.push(regInterface);
                return;
            }
        }
    }
    catch (StackOverFlow &over_flow)
    {
        catchMsg(over_flow);
        exit(EXIT_FAILURE);
    }
    catch (MissQuotes &miss_quotes)
    {
        catchMsg(miss_quotes);
        exit(EXIT_FAILURE);
    }
}

void pop ( string &regName )
{
    try
    {
        if ( theStack.size() == 0 )
        {
            throw StackUnderFlow();
        }
        else
        {
            if ( regName != "ax" && regName != "bx" && regName != "cx" )
            {
                throw WrongDes();
            }
            else
            {
                if ( regName == "ax" )
                {
                    ax = theStack.top();
                    theStack.pop();
                    return;
                }
                if ( regName == "bx" )
                {
                    bx = theStack.top();
                    theStack.pop();
                    return;
                }
                if ( regName == "cx" )
                {
                    cx = theStack.top();
                    theStack.pop();
                    return;
                }
                throw WrongDes();
            }
        }
    }
    catch(StackUnderFlow &under_flow)
    {
        catchMsg(under_flow);
        exit(EXIT_FAILURE);
    }
    catch(WrongDes &wrong_reg)
    {
        catchMsg(wrong_reg);
        exit(EXIT_FAILURE);
    }
}

void mov ( string &des, string &src )
{
    // encapsulate src to a regInterface register
    srcInRegInterface(src);
    try
    {
        if ( des == "ax" )
        {
            ax = regInterface;
            return;
        }
        if ( des == "bx" )
        {
            bx = regInterface;
            return;
        }
        if ( des == "cx" )
        {
            cx = regInterface;
            return;
        }
        else
        {
            throw WrongDes();
        }
    }
    catch (WrongDes &wrong_des)
    {
        catchMsg(wrong_des);
        exit(EXIT_FAILURE);
    }
}

void add ( string &des, string &src )
{
    // encapsulate src to a regInterface register
    srcInRegInterface(src);
    try
    {
        if ( des == "ax" )
        {
            ax = ax + regInterface;
            return;
        }
        if ( des == "bx" )
        {
            bx = bx + regInterface;
            return;
        }
        if ( des == "cx" )
        {
            cx = cx + regInterface;
            return;
        }
        else
        {
            throw WrongDes();
        }
    }
    catch (WrongDes &wrong_des)
    {
        catchMsg(wrong_des);
        exit(EXIT_FAILURE);
    }
}

void sub ( string &des, string &src )
{
    // encapsulate src to a regInterface register
    srcInRegInterface(src);
    try
    {
        if ( des == "ax" )
        {
            ax = ax - regInterface;
            return;
        }
        if ( des == "bx" )
        {
            bx = bx - regInterface;
            return;
        }
        if ( des == "cx" )
        {
            cx = cx - regInterface;
            return;
        }
        else
        {
            throw WrongDes();
        }
    }
    catch (WrongDes &wrong_des)
    {
        catchMsg(wrong_des);
        exit(EXIT_FAILURE);
    }
}

void mul ( string &des, string &src )
{
    // encapsulate src to a regInterface register
    srcInRegInterface(src);
    try
    {
        if ( des == "ax" )
        {
            ax = ax * regInterface;
            return;
        }
        if ( des == "bx" )
        {
            bx = bx * regInterface;
            return;
        }
        if ( des == "cx" )
        {
            cx = cx * regInterface;
            return;
        }
        else
        {
            throw WrongDes();
        }
    }
    catch (WrongDes &wrong_des)
    {
        catchMsg(wrong_des);
        exit(EXIT_FAILURE);
    }
}

void div ( string &des, string &src )
{
    // encapsulate src to a regInterface register
    srcInRegInterface(src);
    try
    {
        if ( des == "ax" )
        {
            ax = ax / regInterface;
            return;
        }
        if ( des == "bx" )
        {
            bx = bx / regInterface;
            return;
        }
        if ( des == "cx" )
        {
            cx = cx / regInterface;
            return;
        }
        else
        {
            throw WrongDes();
        }
    }
    catch (WrongDes &wrong_des)
    {
        catchMsg(wrong_des);
        exit(EXIT_FAILURE);
    }
}

int main (int argc, char const *argv[] )
{
    if ( argc < 2 )
    {
        cout << "Missing file name!"  << endl;
        cout << "usage:\n ./interpreter assembly_file_name" << endl;
        exit(EXIT_FAILURE);
    }
    // local variables
    vector <string> assembly_commands;
    fstream file;
    string raw_line, line, raw_args, args, cmd;
    filename   = argv[argc-1]; // get the file name
    file.open(filename.c_str());

    if ( !file.good() )
    {
        cerr << "File doesn't exist!" << endl;
        exit(EXIT_FAILURE);
    }

    initVec(assembly_commands);

    while ( !file.eof() )
    {
        ++line_num;
        // read the file line by line
        getline(file, raw_line);

        // ignore lines which contain only spaces, tabs, newlines, vertical tabs,
        // feeds and carriage returns
        if ( raw_line.find_first_not_of(" \t\n\v\f\r") == string::npos )
        {
            continue;
        }

        // trim whitespaces
        line  = trimChars(raw_line, ' ');

        // ignore commented lines
        if ( line[0] == '#')
        {
            continue;
        }

        // exctact commands
        stringstream stm_cmds(line);
        getline(stm_cmds, cmd, ' ');

        try
        {
            if ( !isCmdInList(assembly_commands, cmd) )
            {
                throw CommandError();
            }
        }
        catch(CommandError &error_command)
        {
            catchMsg(error_command);
            exit(EXIT_FAILURE);
        }
        // extract args
        size_t begin = line.find(' '); //the first space after a valid cmd
        size_t end   = line.length();

        // check if there are no arguments
        try
        {
            if ( begin == string::npos)
            {
                throw MissArgError();
            }
        } catch (MissArgError &err_arg)
        {
            catchMsg(err_arg);
            exit (EXIT_FAILURE);
        }
        //trim whitespaces from both edges (if exist).
        raw_args = line.substr(begin+1, end);
        args = trimChars(raw_args, ' ');

        // commands that take one arg
        if ( cmd == "print" )
        {
            print(args);
            continue;
        }

        if ( cmd == "push" )
        {
            push(args);
            continue;
        }
        if ( cmd == "pop" )
        {
            pop(args);
            continue;
        }

        // prepare arg1: des, and arg2: src for commands that take two args.
        // tokanizing the args
        // to hold arg1, arg2
        string theArgs[2];
        string s = args;
        istringstream ss(s);
        size_t i = 0;
        while ( !ss.eof() )
        {
          string x;
          getline( ss, x, ',' );
          if ( x == "" ) continue;
          // theArgs[0] = des, theArgs[1] = src
          theArgs[i] = trimChars(x,' ');
          i++;
        }
        des = theArgs[0];
        src = theArgs[1];

        if ( cmd == "mov" )
        {
            mov(des, src);
            continue;
        }

        if ( cmd == "add" )
        {
            add(des, src);
            continue;
        }

        if ( cmd == "sub" )
        {
            sub(des, src);
            continue;
        }

        if ( cmd == "mul" )
        {
            mul(des, src);
            continue;
        }

        if ( cmd == "div" )
        {
            div(des, src);
            continue;
        }
    }

    return 0;
}
