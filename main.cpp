#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <list>
#include <ctime>
using namespace std;
int numSize = 0;
int wordSize = 0;
struct triplet{
    string data;
    int tracking;
    int orderOfAppearance;
    triplet(){
        data = "";
        tracking = 0;
        orderOfAppearance = 0;
    }
    bool operator <(triplet t2) const{
        if (tracking < t2.tracking){
            return true;
        }else if(tracking == t2.tracking){
            if (orderOfAppearance <= t2.orderOfAppearance){
                return false;
            }else{
                return true;
            }
        }else{
            return false;
        }
    }
    triplet& operator =(triplet t2){
        data = t2.data;
        tracking = t2.tracking;
        orderOfAppearance = t2.orderOfAppearance;
        return *this;
    }
};
bool tripletComp(triplet t1, triplet t2);
static const unsigned int max_prime = 1301081;
void init(vector<pair<int, char>>& v);
void readFile(vector<pair<int, char>>& v, vector<list<triplet>>& s, vector<list<triplet>>& n,
              vector<int>& realWord, vector<int>& realNum);
void rehash(vector<list<triplet>>& p, vector<int>& realWord, vector<int>& realNum, bool isNum);
void addToHash(vector<list<triplet>>& h, triplet p, vector<int>& realWord, vector<int>& realNum);
void setPrimes(std::vector<unsigned long>& vprimes);
unsigned long prime_below (unsigned long n);
void printData(vector<pair<int, char>>& v, vector<triplet>& s, vector<triplet>& n);
int main() {
    vector<pair<int, char>> charVector;
    vector<list<triplet>> stringVector(101);
    vector<list<triplet>> numVector(101);
    vector<int> realWord;
    vector<int> realNum;
    vector<triplet> sortedWord;
    vector<triplet> sortedNum;
    init(charVector);
    readFile(charVector, stringVector, numVector, realWord, realNum);
    for (int i = 0; i < realWord.size(); ++i) {
        for (int j = 0; j < stringVector[realWord[i]].size(); ++j) {
            while (!stringVector[realWord[i]].empty()){
                sortedWord.push_back(stringVector[realWord[i]].front());
                stringVector[realWord[i]].erase(stringVector[realWord[i]].begin());
            }
        }
    }
    for (int i = 0; i < realNum.size(); ++i) {
        for (int j = 0; j < numVector[realNum[i]].size(); ++j) {
            while (!numVector[realNum[i]].empty()){
                sortedNum.push_back(numVector[realNum[i]].front());
                numVector[realNum[i]].erase(numVector[realNum[i]].begin());
            }
        }
    }
    sort(charVector.begin(), charVector.end(), greater<pair<int, char>>());
    sort(sortedWord.rbegin(), sortedWord.rend(), tripletComp);
    sort(sortedNum.rbegin(), sortedNum.rend(), tripletComp);
    printData(charVector, sortedWord, sortedNum);
    return 0;
}
void init(vector<pair<int, char>>& v){
    //initializes vector of characters where every character has a position in vector based on ascii chart
    //works similar to a hash table very quick to add to vector and using vector sort after reading file
    //will allow top 10 most used  characters to be found quickly
    for (int i = 0; i < 127; ++i) {
        char c = char(i);
        pair<int, char> temp;
        temp.second = c;
        temp.first = 0;
        v.push_back(temp);
    }
}
void readFile(vector<pair<int, char>>& c, vector<list<triplet>>& s, vector<list<triplet>>& n,
              vector<int>& realWord, vector<int>& realNum){
    ifstream infile;
    string temp;
    int order = 0;
    string filename;
    cout << "enter filename" << endl;
    cin >> filename;
    infile.open(filename);
    bool isOpen = infile.is_open();
    while (!isOpen){
        cout << "open failed retype"<<endl;
        cin >> filename;
        infile.open(filename);
        isOpen = infile.is_open();
    }
    string word;
    string num;
    int count=0;
    while (!infile.eof()){
        getline(infile, temp);
        //loops through temp to add to char vector first and detects numbers while looping
        for(int i = 0; i < temp.size(); ++i) {
            c[int(temp[i])].first++;
            if(isdigit(temp[i])){
                num.push_back(temp[i]);
            } else if(!num.empty()){
                triplet numPair;
                numPair.data = num;
                numPair.tracking = 1;
                numPair.orderOfAppearance = order;
                addToHash(n, numPair, realWord, realNum);
                order++;
                num = "";
            }
            if(isalpha(temp[i])){
                word.push_back(tolower(temp[i]));
            }else if(!word.empty()){
                //adds word to hash since it runs into a character that is not alphabetic
                triplet stringPair;
                stringPair.data = word;
                stringPair.tracking = 1;
                stringPair.orderOfAppearance = order;
                addToHash(s, stringPair, realWord, realNum);
                order++;
                word = "";
            }

        }
        if(!word.empty()){
            //adds word to hash since it runs into a character that is not alphabetic
            triplet stringPair;
            stringPair.data = word;
            stringPair.orderOfAppearance = order;
            stringPair.tracking = 1;
            addToHash(s, stringPair, realWord, realNum);
            order++;
        }
        if(!num.empty()){
            triplet numPair;
            numPair.data = num;
            numPair.tracking = 1;
            numPair.orderOfAppearance = order;
            addToHash(n, numPair, realWord, realNum);
            order++;
        }
        //if string has num then loops to find the num and then while loop to find full number
        c[10].first++;
        if (!temp.empty()) {
            count++;
        }
        word="";
        num="";
    }
}
void rehash(vector<list<triplet>>& p, vector<int>& realWord, vector<int>& realNum, bool isNum){
//creates vector of pairs that were deleted
    //creates vector of pairs that were deleted
    vector<triplet> deletedPairs;
    //oldsize variable so we only loop through values that potentially have pairs
    unsigned long oldSize = p.size();
    //double vectorsize based on Dr. Gaitros reccommendation
    unsigned long newSize = oldSize*10;
    //populate VLP with empty lists
    list<triplet> tempList;
    newSize = prime_below(newSize);
    for (int i = oldSize; i < newSize; ++i) {
        p.push_back(tempList);
    }
    if (isNum){
        numSize = 0;
    }else{
        wordSize = 0;
    }

    //loop through all elements to rehash them
    if (!isNum) {
        while (!realWord.empty()) {
            while (!p[realWord[0]].empty()) {
                deletedPairs.push_back(p[realWord[0]].front());
                p[realWord[0]].erase(p[realWord[0]].begin());
            }
            realWord.erase(realWord.begin());
        }
        for (int i = 0; i < deletedPairs.size(); ++i) {
            addToHash(p, deletedPairs[i], realWord, realNum);
        }
    }else{
        while (!realNum.empty()) {
            while (!p[realNum[0]].empty()) {
                deletedPairs.push_back(p[realNum[0]].front());
                p[realNum[0]].erase(p[realNum[0]].begin());
            }
            realNum.erase(realNum.begin());
        }
        for (int i = 0; i < deletedPairs.size(); ++i) {
            addToHash(p, deletedPairs[i], realWord, realNum);
        }
    }
    //deletes deletedpairs so no memory leak :thumbs_up:
    deletedPairs.erase(deletedPairs.begin(), deletedPairs.end());
}

void addToHash(vector<list<triplet>>& h, triplet p, vector<int>& realWord, vector<int>& realNum){
    //creates hash functions so that adding to the vector does not require looping through the vector for a match
    //because cstl doesn't have hash tables only hash functions the vectors are hash tables at home
    //does not include a list will just rehash if there is a duplicate should be very unlikely because
    //the std hash is very good
    auto sFunction = hash<string>();
    size_t key = sFunction.operator()(p.data);
    key = key % h.size();
    if (h[key].empty()){
        h[key].push_back(p);
        if (isalpha(p.data[0])){
            wordSize++;
            realWord.push_back(key);
        }else{
            numSize++;
            realNum.push_back(key);
        }
    }
    else{
        bool contains = false;
        for (auto iterator = h[key].begin(); iterator!=h[key].end(); iterator++) {
            if (iterator->data == p.data){
                iterator->tracking++;
                contains = true;
            }
        }
        if (!contains){
            h[key].push_back(p);
            if (isalpha(p.data[0])){
                wordSize++;
                realWord.push_back(key);
            }else{
                numSize++;
                realNum.push_back(key);
            }
        }
    }
    if ((wordSize > h.size())){
        sort(realWord.begin(), realWord.end());
        rehash(h, realWord, realNum, false);
    }else if(numSize > h.size()){
        sort(realNum.begin(), realNum.end());
        rehash(h, realWord, realNum, true);
    }
}
unsigned long prime_below (unsigned long n)
{
    if (n > max_prime)
    {
        std::cerr << "** input too large for prime_below()\n";
        return 0;
    }
    if (n == max_prime)
    {
        return max_prime;
    }
    if (n <= 1)
    {
        std::cerr << "** input too small \n";
        return 0;
    }

    // now: 2 <= n < max_prime
    std::vector <unsigned long> v (n+1);
    setPrimes(v);
    while (n > 2)
    {
        if (v[n] == 1)
            return n;
        --n;
    }

    return 2;
}

//Sets all prime number indexes to 1. Called by method prime_below(n)
void setPrimes(std::vector<unsigned long>& vprimes)
{
    int i = 0;
    int j = 0;

    vprimes[0] = 0;
    vprimes[1] = 0;
    int n = vprimes.capacity();

    for (i = 2; i < n; ++i)
        vprimes[i] = 1;

    for( i = 2; i*i < n; ++i)
    {
        if (vprimes[i] == 1)
            for(j = i + i ; j < n; j += i)
                vprimes[j] = 0;
    }
}
void printData(vector<pair<int, char>>& v, vector<triplet>& s, vector<triplet>& n){
    for (int i = 0; i < 10; ++i) {
        if (v[i].first == 0){
            break;
        }
        switch (i) {
            case 0:
                if (v[i].second!='\n'){
                    cout << "The most popular character is '" << v[i].second
                         << "' used  " << v[i].first << " times" << endl;
                }
                else{
                    cout << "The most popular character is newline"
                         << " used  " << v[i].first << " times" << endl;
                }
                break;
            case 1:
                if (v[i].second!='\n') {
                    cout << "The 2nd most popular character is '" <<
                         v[i].second << "' used " << v[i].first << " times" << endl;
                }
                else{
                    cout << "The 2nd most popular character is newline"
                         << " used  " << v[i].first << " times" << endl;
                }
                break;
            case 2:
                if (v[i].second!='\n') {
                    cout << "The 3rd most popular character is '"
                         << v[i].second << "' used " << v[i].first << " times" << endl;
                }
                else{
                    cout << "The 3rd most popular character is newline"
                         << " used  " << v[i].first << " times" << endl;
                }
                break;
            default:
                if (v[i].second!='\n') {
                    cout << "The " << i + 1 << "th most popular character is '"
                         << v[i].second << "' used " << v[i].first << " times" << endl;
                } else{
                    cout << "The " << i + 1 << "th most popular character is newline"
                         << " used " << v[i].first << " times" << endl;
                }
                break;
        }
    }
    for (int i = 0; i < s.size(); ++i) {
        if (s[i].data.empty()){
            break;
        }
        if (i >9){
            break;
        }
        switch (i) {
            case 0:
                cout << "The most popular word is '" << s[i].data << "' used  " << s[i].tracking << " times" << endl;
                break;
            case 1:
                cout << "The 2nd most popular word is '" <<
                     s[i].data << "' used " << s[i].tracking << " times" << endl;
                break;
            case 2:
                cout << "The 3rd most popular word is '"
                     << s[i].data << "' used " << s[i].tracking << " times" << endl;
                break;
            default:
                cout << "The " << i+1 << "th most popular word is '"
                     << s[i].data << "' used " << s[i].tracking << " times" << endl;
                break;
        }
    }
    for (int i = 0; i < n.size(); ++i) {
        if (n[i].data.empty()){
            break;
        }
        if (i >9){
            break;
        }
        switch (i) {
            case 0:
                cout << "The most popular number is '" << n[i].data << "' used " << n[i].tracking << " times" << endl;
                break;
            case 1:
                cout << "The 2nd most popular number is '" <<
                     n[i].data << "' used " << n[i].tracking << " times" << endl;
                break;
            case 2:
                cout << "The 3rd most popular number is '"
                     << n[i].data << "' used " << n[i].tracking << " times" << endl;
                break;
            default:
                cout << "The " << i+1 << "th most popular number is '"
                     << n[i].data << "' used " << n[i].tracking << " times" << endl;
                break;
        }
    }
}
bool tripletComp(triplet t1, triplet t2){
    return t1<t2;
}
