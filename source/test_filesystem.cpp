#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std;
using namespace boost::filesystem;

void test_file_size(const char* file);
void test_file_type(const char* file);
void test_foreach();

void print_captures(const string& regex, const string& text);
void test_regex();



int main(int argc, char** argv)
{
	//test_file_size(argv[1]);
	//test_file_type(argv[1]);
	//test_foreach();
	test_regex();

	return 0;
}

void test_foreach()
{
	string hello("Hello, World");
	BOOST_FOREACH(char ch, hello)
	{
		cout << ch << endl;
	}
	cout << endl;
}

void print_captures(const string& regex, const string& text)
{
	boost::regex e(regex);
	boost::smatch what;

	cout << "Expression: \"" << regex << "\"\n";
	cout << "Text:       \"" << text  << "\"\n";
	if (boost::regex_match(text, what, e, boost::match_extra))
	{
		unsigned i, j;
		cout << "**Match found **\n     Sub-Expressions:\n";
		for (i = 0; i < what.size(); ++i)
		{
			cout << "     $" << i << " =\"" << what[i] << "\"\n";
		}
		cout << "   Captures:\n";
		for (i = 0; i < what.size(); ++i)
		{
			cout << "     $" << i << " = {";
			/*
			for (j = 0; j < what.captures(i).size(); ++j)
			{
				if (j)
					cout << ", ";
				else
					cout << " ";
				cout << "\"" << what.captures(i)[j] << "\"";
			}
			//*/
			cout << " }\n";
		}
	}
	else
	{
		cout << "**NO Match found **\n";
	}
}

void test_regex()
{
	print_captures("(([[:lower:]]+)|([[:upper:]]+))+", "aBBcccDDDDDeeeeeeeeee");
	print_captures("(.*)bar|(.*)bah", "abcbar");
	print_captures("(.*)bar|(.*)bah", "abcbah");
	print_captures("^(?:(\\w+)|(?>\\W+))*$", "now is the time for all good men to come to the aid of the party");
}

void test_file_type(const char* file)
{
	path p(file);
	if (exists(p))  //does path p actually exist
	{
		if (is_regular_file(p))
		{
			cout << p << " size is " << file_size(p) << endl;
		}
		else if (is_directory(p))
		{
			cout << p << " is a directory containing : " << endl;
			vector<string> v;
			for (auto&& x : directory_iterator(p))
				v.push_back(x.path().filename().string());
			
			std::sort(v.begin(), v.end());
			//for_each(v.begin(), v.end(), cout << _1 << "  ");
			for (auto&& x : v)
			{
				cout << x << endl;
			}
		}
		else
		{
			cout << p << " exists, but is not a regular file or directory" << endl;
		}
	}
	else
	{
		cout << p << " does not exist" << endl;
	}
}

void test_file_size(const char* file)
{
	assert(file != nullptr);
	cout << file << " " << file_size(file) << endl;
}