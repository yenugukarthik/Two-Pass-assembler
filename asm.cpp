//Author: 2201CS79
//Name: karthik reddy yenugu
//Authorship:
//This txt file, claims.txt, is part of the mini project of CS209/CS210 .
# include <bits/stdc++.h>
# define OPCODE_BITS 8
# define OPERAND_BITS 24

using namespace std;

map<string,int> opcodes;	//maps opcodes to respective numberical opcodes
map<string,int> labels;		//stores branch labels
map<string,int> values;		//stores the labels declared using data

vector<pair<int,string>> errors,warnings;
string operand_type[][3] = {
        {"ldc", "0", "value"},
        {"adc", "1", "value"},
        {"ldl", "2", "offset"},
        {"stl", "3", "offset"},
        {"ldnl", "4", "offset"},
        {"stnl", "5", "offset"},
        {"add", "6", ""},
        {"sub", "7", ""},
        {"shl", "8", ""},
        {"shr", "9", ""},
        {"adj", "10", "value"},
        {"a2sp", "11", ""},
        {"sp2a", "12", ""},
        {"call", "13", "offset"},
        {"return", "14", ""},
        {"brz", "15", "offset"},
        {"brlz", "16", "offset"},
        {"br", "17", "offset"},
        {"HALT", "18", ""}
    };
vector<string> instructions = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj",
								"a2sp","sp2a","call","return","brz","brlz","br","HALT"};

vector<string> need_operand = {"ldc","adc","ldl","stl","ldnl","stnl","adj","call","brz","brlz","br"};

void initilize(){
    int i=0;
    while(i<19){
        opcodes[instructions[i]] = i;
        i++;
    }
}


// Function to split_instruction a string into a vector of strings based on whitespace
vector<string> split_instruction(const string& str) {
    vector<string> result;
    string word;

    // Iterate through each character in the string
    for (char c : str) {
        // If the character is not a whitespace, add it to the current word
        if (c != ' ') {
            word += c;
        } else {
            // If the character is a whitespace, add the current word to the result
            // and start a new word
            if (!word.empty()) {
                result.push_back(word);
                word.clear();
            }
        }
    }

    // Add the last word if it's not empty
    if (!word.empty()) {
        result.push_back(word);
    }

    return result;
}

int string_to_int(const string& str) {   //convert (hex or int) to int
    char* endptr;
    long value;
    int base = 10; // Default to base 10 for decimal numbers

    // Check if the string starts with '0x' or '0X', indicating a hexadecimal number
    if (str.size() > 2 && (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))) {
        base = 16; // Set base to 16 for hexadecimal
    }

    // Convert the string to a long integer using strtol
    value = strtol(str.c_str(), &endptr, base);

    // Check if the conversion was successful
    if (endptr == str.c_str()) {
        // If the conversion was not successful, return 0 or handle the error as needed
        return 0;
    }

    // If the conversion was partially successful (i.e., some characters were valid),
    // return the value obtained so far
    if (*endptr != '\0') {
        return static_cast<int>(value);
    }

    // Return the converted value as an int
    return static_cast<int>(value);
}

bool is_integer(const std::string& s) {
    if (s.empty()) return false; // Empty string is not a number

    // Skip leading sign if present
    size_t start = 0;
    if (s[0] == '-' || s[0] == '+') {
        start = 1;
    }

    // Check for hexadecimal prefix
    bool isHex = false;
    if (s.size() > start + 2 && s[start] == '0' && (s[start + 1] == 'x' || s[start + 1] == 'X')) {
        start += 2; // Skip the '0x' or '0X' prefix
        isHex = true; // Set flag to indicate hexadecimal
    }

    // Check if the remaining string contains only digits (for both decimal and hexadecimal)
    for (size_t i = start; i < s.size(); ++i) {
        if (!std::isdigit(s[i]) && !(isHex && ((s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F')))) {
            return false; // Found a non-digit character, not a number
        }
    }

    return true; // All characters are digits, so it's a number
}

int counter,line_number;	//counter acts like PC 
string line;			// line_number is to specify the loaction of errors

int main(int argc, char* argv[]){

	initilize();

	// file managemnt 
	if(argc<2){			// checking for noumber of arguments
		cout << "Usage [file name to assemble]" << endl;
		return 1;
	}
	string filename = argv[1];
	size_t pos = filename.find_last_of(".");
    string name = filename.substr(0, pos);
	
	ifstream in(filename);

  	ofstream bin(name+".o",ios:: binary);
  	ofstream listing(name + ".l");

  	counter = 0;
  	line_number = 0;

	//"First pass for storing labels and error detection"

	while(getline(in,line)){
		line_number++;

		string label = "";
		int j = 0;
		int k = line.size();

		//whitespace management and splitting the string
		for(int i=0;i<k;++i){
			if(line[i]==';'){
				k=i;
				break;
			}   
		}
		for(int i=0;i<k;++i){
			if(line[i]==':'){
				label = line.substr(0,i);
				j = i+1;
				break;
			}
		}
		vector<string> current_i = {};
		if(j<k) current_i = split_instruction(line.substr(j,k-j));
		if(label!=""){
			for(int i=0;i<label.size();++i){
				//checking for valid label
				if((label[0]>='0'&&label[0]<='9')||(label[0]<'a'&&label[0]>'z')){
					errors.push_back(make_pair(line_number,"Bogous label name"));
					break;
				}
			}
			bool data=false;
			 for (char c : line) {
            // Check if the current character is the start of the substring "data"
            if (c == 'd') {
                // Check the next characters to see if they match "ata"
                if (line.substr(line.find(c), 4) == "data") {
                    data = true;
                    break; // Exit the loop if "data" is found
                }
            }
        }
			if(labels.find(label)!=labels.end()||values.find(label)!=values.end()){
				errors.push_back(make_pair(line_number,"Duplicate label definition"));
			}else{
				if(data){
					if(current_i.size()>1){
						values[label] = counter;
						counter++;
					}else{
						errors.push_back(make_pair(line_number,"Expected a value to store"));
					}
					continue;
				}
				else labels[label] = counter;
			}
			if(current_i.size()==1) continue;
		}
		if(current_i.size()==0) continue;
		string operation = current_i[0];
		if(find(instructions.begin(),instructions.end(),operation)==instructions.end()){
			errors.push_back(make_pair(line_number,"Bogus mnemonic"));
		}else if(find(need_operand.begin(),need_operand.end(),operation)!=need_operand.end()){
			if(current_i.size()<2){
				errors.push_back(make_pair(line_number,"Expected an operand"));    //checking for missing operand
			}else if(current_i.size()>2){ 
				errors.push_back(make_pair(line_number,"Extra operand "));  //checking for extra operand
			}
		}
		counter++;
	}

	in.clear();		//clear the error state flags of a file stream
	in.seekg(0);	//moves the file read pointer to the start of file
	counter = 0;
	line_number = 0;

	//Second Pass further error detection and machine code generation

	while(getline(in,line)){
		line_number++;

		//making the 8 bit counter string
		stringstream ss;
    	ss << std::setfill('0') << std::setw(8)  << std::hex << std::uppercase << counter;
    	string counter_width8 = ss.str();

		string label = "";
		int j = 0;
		int k = line.size();

		//whitespace management and splitting the string
		for(int i=0;i<line.size();++i){
			if(line[i]==';'){
				k=i;
				break;
			}
		}
		for(int i=0;i<k;++i){
			if(line[i]==':'){
				label = line.substr(0,i);
				j = i+1;
				break;
			}
		}
		vector<string> current_i = {};
		if(j<line.size()) current_i = split_instruction(line.substr(j,k-j));

		//check if line has a label (label/data)
		if(label!=""){
			bool data = (line.find("data")!=string::npos);
			if(data){
				bitset<32> tmp(stoi(current_i[1]));
				bin << tmp.to_string() << endl;
				listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0') << current_i[1] << "\t" << label << ": " << current_i[1] << endl;
				counter++;
				continue;
			}else listing << counter_width8 << "\t\t\t\t" << label << ":" << endl;
		}
		if(current_i.size()<1) continue;
		string operation = current_i[0];
		int opcode = opcodes[operation];
		string operand_str = "";
		if(current_i.size()>1) operand_str = current_i[1];
		int operand = 0;

		//handling branch and calls seperatly 
		if(operation[0]=='b'||operation=="call"){
			if(labels.find(operand_str)==labels.end()){
				errors.push_back(make_pair(line_number,"\"" + operand_str+"\" is not yet defined"));
				continue;
			}
			int offset = labels[current_i[1]]-counter-1;
			operand = offset;
			if(offset==0){
				warnings.push_back(make_pair(line_number,"Infinite Loop detected"));
			}
		}else{
			if(operand_str!=""&&!is_integer(operand_str)){
				if(values.find(operand_str)!=values.end()){
					operand = values[operand_str];
				}else if(labels.find(operand_str)!=labels.end()){
					operand = values[operand_str];
				}else{
					errors.push_back(make_pair(line_number,"Badly formatted number - " + operand_str));
					continue;
				}	
			}else{
				operand = string_to_int(operand_str);
			}
		}

		//Print machine code to the name.o file
		unsigned int binary = ((operand<<OPCODE_BITS)|(opcode));
		bitset<32> binary_bin(binary);
		listing << counter_width8 << "\t" << std::hex << std::uppercase << setw(8) << setfill('0')  << binary << "\t" << current_i[0] << " " << operand_str << endl;
		bin << binary_bin << endl;
		counter++;
	}

	in.close();
	bin.close();
	listing.close();

	//sorting errors on the basis of their line of occurance
	sort(errors.begin(),errors.end());
	sort(warnings.begin(),warnings.end());
	ofstream e_and_w(name+".log");
	for(auto val : warnings){
		e_and_w << "Warning @Line: " << val.first << "\t" << val.second << endl;
	}
	e_and_w << "Total errors: " << errors.size() << endl;
	for(auto val : errors){
		e_and_w << "Error @Line:" << val.first << "\t" << val.second << endl;
	}
	e_and_w.close();

	//if there are errors program exits with error code 1
	if(errors.size()){
		return 1;
	}else return 0;
}
