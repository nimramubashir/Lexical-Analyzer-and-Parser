# include <iostream>
# include <fstream>
# include <string>

using namespace std;

int ptable[28][27];
int rules[56][6];
int stack[100];
int table[36][22];
int accept[36];
int ascii[128];
int rcounter[56];
int scounter = 0;

string type[27] = { "(" , ")" , "int" , "float" , ";" , "for" , "while" , "if" , "identifier" ,"{","}",":=","=="   ,
					"<" , ">" , "!=" , "+" , "-" , "*" ,"/" , ",", "number" , "else" , "<=" , ">=" , "<>" , "$" };

void load_arr()
{
	int x, y = 0;

	for (int i = 0; i < 128; i++)
	{
		ascii[i] = -1;
		if (i < 36)
			accept[i] = -1;
	}

	ifstream read;
	read.open("ascii.txt", ios::in);

	for (int i = 0; i < 128; i++)
	{
		read >> x >> y;
		if (x == i)
			ascii[i] = y;
	}

	read.close();

	read.open("accept.txt", ios::in);

	for (int i = 0; i < 36; i++)
	{
		read >> x;
		accept[i] = x;
	}
	read.close();

	read.open("table.txt", ios::in);

	for (int i = 0; i < 36; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			read >> x;
			table[i][j] = x;
		}
	}
	read.close();
}

bool is_keyword(string arr1)
{
	string arr[16] = { "int","float","constant" , "break" , "continue" , "if" ,"while" , "for" , "switch", "else","void",
						"case","char","do" ,"union","return"};

	bool flag = false;
	for (int i = 0; i < 16; i++)
	{
		if (arr[i] == arr1)
		{
			flag = true;
			break;
		}
	}

	return flag;
}

void create_token(string word, string type)
{
	ofstream write;
	write.open("token.txt", ios::app);

	write << word << "," << type << endl;
	write.close();
}

bool check_error(int& state, char c, int& index, int& pindex, string line, int& count, int& eno)
{
	char var;

	if (line[index] != '\0' && line[index + 1] != '\0')
		var = line[index + 1];
	else
		var = c;

	if (table[state - 1][ascii[c]] == -1 && table[state - 1][ascii[var]] == -1)
	{
		ofstream write;
		string word;
		eno++;


		for (int i = 0, j = pindex; j <= index; j++, i++)
			word = word + line[j];

		write.open("error.txt", ios::app);
		write << "error no:" << eno << " on line number: " << count << " " << word << endl;
		write.close();

		pindex = index;
		state = 1;
		return true;
	}
	else
		return false;
}

bool is_accept(int& state, char c, int& index, int& pindex, string line, int& lcount, int& ecount)
{
	if (accept[state - 1] != 0)
	{
		string word;
		word[0] = '\0';
		if (accept[state - 1] > 0)
		{
			for (int i = 0, j = pindex; line[j] != '\0' && line[j] != ' ' && j <= index; j++, i++)
				word = word + line[j];
			index++;
		}
		else
		{
			// index--; 
			for (int i = 0, j = pindex; line[j] != '\0' && line[j] != ' ' && j <= index; j++, i++)
				word = word + line[j];

		}
		if (accept[state - 1] == 1 || accept[state - 1] == -1)
			create_token(word, "identifier");
		else if (accept[state - 1] == 2 || accept[state - 1] == -2)
			create_token(word, "number");
		else if (accept[state - 1] == 3 || accept[state - 1] == -3)
			create_token(word, "operator");
		else if (accept[state - 1] == 4 || accept[state - 1] == -4)
			create_token(word, "punctuation");
		else if (accept[state - 1] == 5 || accept[state - 1] == -5)
		{
			if (is_keyword(word))
			{
				create_token(word, "keyword");
			}
			else
			{
				ofstream write;
				ecount++;
				write.open("error.txt", ios::app);
				write << "Error no:" << ecount << " on line number: " << lcount << " " << word << endl;
				write.close();
				state = 1;
				pindex = ++index;
				return true;
			}

		}
		state = 1;
		pindex = index;
		return true;
	}
	else
		return false;
}

int find_col(string tv, string tt, string& match)
{
	for (int i = 0; i < 27; i++)
	{
		if (type[i] == tv)
		{
			match = "v";
			return i;
		}
		else if (type[i] == tt)
		{
			match = "t";
			return i;
		}
	}
}

void write_error(string tvalue)
{
	ofstream write;
	write.open("perror.txt", ios::app);
	write << tvalue << endl;
	write.close();
}

void write_rule(int row)
{
	string rule[28] = { "function","arglist","AL`","Arg","Decleration","Type","Identlist",
						"IL`","Stmt","forstmt","f`","f``","ifstmt","elsepart","whilestmt",
						"compoundStmt","Stmtlist","SL`","Expr","Rvalue","RV`","Compare",
						"Mag","Mag`","term","term`","factor" };

	ofstream write;
	write.open("tree.txt", ios::app);

	int r; 

	for (int i = 0; i < rcounter[row]; i++)
	{
		r = rules[row][i]; 

		 if (r > 0)
		{
			r--; 
			write << type[r--];
		}
		else
		{
			r = r * -1; 
			r--; 
			write << rule[r];
		}
		//write << rules[row][i] << " ";
	}
	write << endl;
	write.close();
}

int pop()
{
	if (scounter != 0)
	{
		scounter--;
		return stack[scounter];
	}
	return false;
}

void push(int row)
{
	for (int i = rcounter[row] - 1; i >= 0; i--, scounter++)
	{
		stack[scounter] = rules[row][i];
	}

	write_rule(row);
}

void initialize_arrays()
{
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 27; j++)
		{
			ptable[i][j] = 0;

		}
	}

	for (int i = 0; i < 56; i++)
	{
		rcounter[i] = 0;
		for (int j = 0; j < 6; j++)
		{
			rules[i][j] = 1000;
		}
	}
}

void read_files()
{
	ifstream read;
	read.open("rtable.txt", ios::in);

	if (read.fail())
		cout << " Cannot find the file rtable.txt" << endl;
	else
	{
		for (int i = 0; i < 28; i++)
		{
			for (int j = 0; j < 27; j++)
			{
				read >> ptable[i][j];

			}
		}
	}
	read.close();

	read.open("rules.txt", ios::in);
	if (read.fail())
		cout << " Cannot find the file rules.txt" << endl;
	else
	{
		int file;

		for (int i = 0; i < 56; i++)
		{
			for (int j = 0; j < 7; j++)
			{
				read >> file;

				if (file != 99)
				{
					rules[i][j] = file;
				}
				else
				{
					rcounter[i] = j;
					break;
				}
			}
		}
	}
	read.close();

}

void assess()
{
	string tvalue;
	string ttype;

	int rule = 0;

	ifstream read;
	read.open("token.txt", ios::in);

	if (read.fail())
		cout << " Cannot find the file token.txt" << endl;
	else
	{
		int col = 0;
		int popped = 0;
		int result = 0;

		string match;
		stack[0] = 99;
		stack[1] = 0;
		scounter = scounter + 2;

		while (!read.eof())
		{
			getline(read, tvalue, ',');
			getline(read, ttype);

			col = find_col(tvalue, ttype, match);

			while (1)
			{
				popped = pop();

				if (popped > 0)
				{
					if (popped == 77)
					{
						//pop();
						//break;
					}
					else if (popped == 88)
					{
						write_error(tvalue);
						break;
					}
					else if (popped != 77 && popped != 88)
					{
						popped--;

						if (tvalue == type[popped] || ttype == type[popped])
						{
							cout << type[popped] << " matched." << endl;
							break;
						}
						else
						{
							write_error(tvalue);
							break;
						}
					}
				}
				else
				{
					if (popped != 0)
					{
						popped = popped * -1;
						popped--;
					}
					result = ptable[popped][col];
					result--;
					push(result);
				}
			}

		}
		for (int i = scounter; stack[i] != 99; i--)
			pop();
		read.close();
	}
}

int main()
{
	load_arr();

	string line;
	int lcount = 0;
	int ecount = 0;
	int index = 0;
	int pindex = 0;
	int state = 0;


	bool flag = false;
	bool check = false;
	bool accepted = false;

	char c = '\0';

	ifstream read;
	ofstream write;

	write.open("token.txt", ios::out);
	write.close();

	write.open("error.txt", ios::out);
	write.close();


	read.open("read.txt", ios::in);

	getline(read, line);
	lcount++;

	while (!read.eof())
	{
		state = 1;
		while (1)
		{
			while (1)
			{
				c = line[index];
				if (c != ' ' && c != '\0')
				{
					flag = true;
					break;
				}
				else if (c == ' ')
				{
					int nstate = table[state - 1][ascii[c]];
					if (accept[nstate - 1] != 0)
					{
						is_accept(nstate, c, index, pindex, line, lcount, ecount);
						state = nstate;
					}

					index++;
					pindex = index;
				}
				else if (c == '\0')
				{
					if (accepted == false)
					{

						if (table[state - 1][ascii[c]] != -1)
						{
							state = table[state - 1][ascii[c]];
							if (!is_accept(state, c, index, pindex, line, lcount, ecount))
							{
								if (check_error(state, c, index, pindex, line, lcount, ecount))
									check = true;
							}
							else
								check = true;
						}
						else
						{
							if (check_error(state, c, index, pindex, line, lcount, ecount))
								check = true;
						}
					}
					else
						check = true;

				}
				if (check == true)
					break;

			}

			if (check == true)
				break;

			accepted = false;

			state = table[state - 1][ascii[c]];

			if (!is_accept(state, c, index, pindex, line, lcount, ecount))
			{
				//if (!check_error(state, c, index, pindex, line, lcount, ecount))
				index++;
				//else
					//accepted = true; 
			}
			else
				accepted = true;

			flag = false;


		}
		getline(read, line);
		check = false;
		lcount++;
		pindex = 0;
		index = 0;
	}
	read.close();

	write.open("tree.txt", ios::out);
	write.close();
	write.open("error.txt", ios::out);
	write.close();
	initialize_arrays();
	read_files();
	assess();
	return 0;
}