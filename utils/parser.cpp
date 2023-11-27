#include <string>
#include <unordered_set>
#include <map>
#include "stringtrim.cpp"


class Parser {
private:
    std::string compiled;
    std::unordered_set<std::string> symbols;
    int topTemp;
    std::string getAssignee(std::string lhs) {
        /**
         * Get the symbol on which the expression assigns a value.
        */
        std::string accumulate;
        int pos = 0;
        int depth = 0;
        while(pos<lhs.size()) {
            if(lhs[pos]=='('||lhs[pos]=='{')
                break;
            if(lhs[pos]=='=') {
                trim(accumulate);
                return accumulate;
            }
            accumulate += lhs[pos];
            pos += 1;
        }
        return "#";
    }
    std::string getValue(std::string rhs) {
        /**
         * Get the string expression (this is guaranteed not to contain an assignee).
        */
        int pos = 0;
        while(pos<rhs.size()) {
            if(rhs[pos]=='('||rhs[pos]=='{')
                break;
            if(rhs[pos]=='='){
                rhs = rhs.substr(pos+1);
                break;
            }
            pos += 1;
        }
        trim(rhs);
        return rhs;
    }
    bool isString(const std::string& value) {
        return value[0]=='"' && value[value.size()-1]=='"';
    }
    bool isBool(const std::string& value) {
        return value=="true" || value=="false";
    }
    bool isInt(const std::string& value) {
        try{
            std:stoi(value);
            return true;
        }
        catch(...) {
            return false;
        }
    }
    bool isFloat(const std::string& value) {
        try{
            std:stof(value);
            return true;
        }
        catch(...) {
            return false;
        }
    }
    void addCommand(std::string& command) {
        /**
         * Parses an indivual command found in a block of code. Called by parse.
        */
        if(command.size()==0)
            return;
        std::string variable = getAssignee(command);
        std::string value = getValue(command);
        bool finalize = false;
        if(variable.substr(0, 6)=="final ") {
            finalize = true;
            variable = variable.substr(6);
            trim(variable);
        }
        size_t pos = value.find('(');
        if(pos == std::string::npos) {
            if(value.size() && symbols.find(value) != symbols.end()) {
                compiled += "copy "+variable+" "+value+"\n";
                return;
            }
            if(value==":") {
                compiled += "inline "+variable+" LAST\n";
                return;
            }
            if(value.size() && value[value.size()-1]==':' && symbols.find(value.substr(0, value.size()-1)) != symbols.end()) {
                compiled += "inline "+variable+" "+value.substr(0, value.size()-1)+"\n";
                return;
            }
            if(variable=="#" || value.size()==0) // flexible parsing for undeclared variables
                return;
            if(isString(value))
                compiled += "BUILTIN "+variable+" "+value+"\n";
            else if(isBool(value))
                compiled += "BUILTIN "+variable+" B"+value+"\n";
            else if(isInt(value))
                compiled += "BUILTIN "+variable+" I"+value+"\n";
            else if(isFloat(value))
                compiled += "BUILTIN "+variable+" F"+value+"\n";
            else if(variable==value) 
                compiled += "copy "+variable+" "+value+"\n";
            else
                compiled += "IS "+value+" "+variable+"\n"; // this is not an actual assembly command but is used to indicate that parsed text is just a varlabe that should be obtained from future usages
        } else {
            std::string args = value.substr(pos+1); // leaving the right parenthesis to be removed during further computations
            value = value.substr(0, pos);
            trim(value);
            if(symbols.find(value) != symbols.end()) {
                std::string argexpr = args.substr(0, args.size()-1);
                trim(argexpr);
                if(argexpr=="")
                    argexpr = "#";
                else if(symbols.find(argexpr) == symbols.end()) {
                    if(argexpr[0]!='{')
                        argexpr = "{"+argexpr+"}";
                    std::string tmp = "_anon"+std::to_string(topTemp);
                    topTemp += 1;
                    Parser tmpParser = Parser(symbols, topTemp);
                    tmpParser.parse(tmp+" = "+argexpr+";");
                    if(tmpParser.toString().substr(0, 3) != "IS "){
                        compiled += tmpParser.toString();
                        argexpr = tmp;
                    }
                }
                compiled += "CALL "+variable+" "+argexpr+" "+value+"\n";
            }
            else {
                std::string argexpr;
                int depth = 0;
                int i = 0;
                bool inString = false;
                std::string accumulate;
                while(i<args.size()) {
                    if(args[i]=='"')
                        inString = !inString;
                    if(inString) {
                        accumulate += args[i];
                        i += 1;
                        continue;
                    }
                    if(depth==0 && (args[i]==',' || i==args.size()-1)) {
                        trim(accumulate);
                        if(symbols.find(accumulate) == symbols.end()) {
                            std::string tmp = "_anon"+std::to_string(topTemp);
                            topTemp += 1;
                            Parser tmpParser = Parser(symbols, topTemp);
                            tmpParser.parse(tmp+" = "+accumulate+";");
                            if(tmpParser.toString().substr(0, 3) != "IS "){
                                compiled += tmpParser.toString();
                                accumulate = tmp;
                                //topTemp = tmpParser.topTemp;
                            }
                        }
                        argexpr += " "+accumulate;
                        accumulate = "";
                        i += 1;
                        continue;
                    }
                    if(args[i]=='(' || args[i]=='{')
                        depth += 1;
                    if(args[i]==')' || args[i]=='}')
                        depth -= 1;
                    accumulate += args[i];
                    i += 1;
                }
                compiled += value+" "+variable+argexpr+"\n";
            }
        }
        if(variable!="#")
            symbols.insert(variable);
        if(finalize) {
            compiled += "FINAL # "+variable+"\n";
        }
    }
public:
    Parser() {
        topTemp = 0;
        symbols.insert("self");
    }
    Parser(std::unordered_set<std::string>& symbs, int topTemps) {
        symbols = symbs;
        topTemp = topTemps;
    }
    void parse(const std::string& code) {
        /**
         * Parses a block of code.
        */
        std::string command;
        int pos = 0;
        int depth = 0;
        bool inString = false;
        while(pos<code.size()) {
            char c = code[pos];
            if(c=='"')
                inString = !inString;
            if(inString) {
                command += c;
                pos += 1;
                continue;
            }
            if(c=='\n')
                c = ' ';
            if(c=='\t')
                c = ' ';
            if(c=='(') 
                depth += 1;
            if(c==')')
                depth -= 1;
            if(depth==0 && c=='{') {
                std::string variable = getAssignee(command);
                if(variable.substr(0, 6)=="final "){
                    variable = variable.substr(6);
                    trim(variable);
                    compiled += "BEGINFINAL "+variable+"\n";
                }
                else
                    compiled += "BEGIN "+variable+"\n";
                if(variable!="#")
                    symbols.insert(variable);
                command = "";
            }
            else if(depth==0 && c=='}') {
                addCommand(command);
                compiled += "END\n";
                command = "";
            }
            else if(depth==0 && c==':') {
                command += c;
                addCommand(command);
                command = "";
            }
            else if(depth==0 && (c==';' || pos==code.size()-1)) {
                addCommand(command);
                command = "";
            }
            else 
                command += c;
            pos += 1;
        }
    }
    std::string toString() const {
        /**
         * Returns the end-result of parsing.
        */
        return compiled;
    }
};



int compile(const std::string& source, const std::string& destination) {
    /**
     * Compiles a blombly file (.bb) written in the namesake programming
     * language to a corresponding virtual machine file (.bbvm).
     * @param source The blombly file path that contains the source code.
     * @param destination The file path on which to write the compiled virtual machine assembly.
     * @return 0 if compilation was completed successfully
    */
    
    // load the source code from the source file
    std::ifstream inputFile(source);
    if (!inputFile.is_open())  {
        std::cerr << "Unable to open file: " << source << std::endl;
        return 1;
    }
    std::string code = "";
    std::string line;
    while (std::getline(inputFile, line)) 
        code += line;
    inputFile.close();
        
    // create a compiled version of the code
    Parser parser;
    parser.parse(code);
    std::string compiled = parser.toString();

    // save the compiled code to the destination file
    std::ofstream outputFile(destination);
    if (!outputFile.is_open())  {
        std::cerr << "Unable to write to file: " << source << std::endl;
        return 1;
    }
    outputFile << compiled;
    outputFile.close();

    // return success code if no errors have occured
    return 0;    
}