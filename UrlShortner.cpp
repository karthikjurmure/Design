/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <bits/stdc++.h>
using namespace std;
class UrlMapping {
public:
	string longUrl;
	string shortCode;
	UrlMapping(string url,string code) {
		this->longUrl=url;
		this->shortCode=code;
	}
};
class UrlshortnerService {
private:
	unordered_map<string,string>longToshort;
	unordered_map<string,string>shortTolong;
	long long id=1;
public:
	string encode(string url) {
       if(longToshort.count(url)){
           return longToshort[url];
       }
       string code=genreatecode();
       shortTolong[code]=url;
       longToshort[url]=code;
       return code;
	}
	string decode(string url) {
       if(!shortTolong.count(url)){
           return "INVALID URL";
       }
       return shortTolong[url];
	}
private:
	string genreatecode() {
		string chars =
		    "abcdefghijklmnopqrstuvwxyz"
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		    "0123456789";

		long long num = id++;
		string code;

		while (num >0) {
			code += chars[num % 62];
			num /= 62;
		}

		reverse(code.begin(), code.end());

		return code;
	}
};
int main(){
    UrlshortnerService service;
    string shortUrl = service.encode("https://leetcode.com/problems/two-sum");
    cout << "Short URL: " << shortUrl << endl;

    cout << "Original URL: "
         << service.decode(shortUrl);
    return 0;
}
