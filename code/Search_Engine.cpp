#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<time.h>
#include"PostingList.h"
#include"StemLib/porter2_stemmer.hpp"
using namespace std;
#define DOCNUM 42//total number of documents
#define CATEGORY_NUM 4//number of different categories(not including "all")

PostingList myPostingList;//the posting list
string docTitles[DOCNUM];//titles(the first line for a document)
string docCategory[DOCNUM];//categories(5 categories: all, comedy, history, tragedy, poetry)
string categories[CATEGORY_NUM]=
{
    "comedy",
    "history",
    "tragedy",
    "poetry"
};

//function declarations
string dec2_to_string(int n);//turn an integer between 0 and 99 to two character string (such as 1->"01")
bool isLowerCase(char c);//determine if c is a lowwer case English character
bool isWhite(char c);//detect white
bool isCategory(string cat);//determine if c is a name of existed catogery

void RemoveTokens(string &word);//remove tokens at the front and end of the word, remove ' in it.
void TakeStem(string &word);//restore the stem of input word(refer to codes in the Internet)
string ProcessWord(string &word);//romove tokens, case transfer, stem algorithm.(allowed to refer to the Internet)
vector<string> ProcessPhrase(string phrase);//turn a phrase represented by a string with space into individual word strings

void GenerateDocInfo();//generate document information: title and category
void ReadDoc(int docID);//read a certain document word by word, add into posting list after processing them
void Index();//create inverted file index
void DisplayInterface();//display interface
void Query();//do query



//function implementations
string dec2_to_string(int n)
{
    string ret="00";
    if(n<0||n>=100)
    {
        cout<<"Error:Out of range"<<endl;
    }
    else if(n<10)
    {
        ret[1]=n+'0';
    }
    else
    {
        ret[0]=n/10+'0';
        ret[1]=n%10+'0';
    }
    return ret;
}
bool isLowerCase(char c)
{
    return c>='a'&&c<='z';
}
bool isWhite(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}
bool isCategory(string cat)
{
    string temp;
    temp="all";
    if(cat==ProcessWord(temp))
        return true;
    for(int i=0;i<CATEGORY_NUM;i++)
    {
        temp=categories[i];
        if(cat==ProcessWord(temp))
            return true;
    }
    return false;
}

void RemoveTokens(string &word)
{    
    int l=word.length(),pos;
    //delete 'xxx....
    while((pos=word.find('\''))!=-1)
    {
        l=word.length();
        if(pos==1&&word[0]=='o')
            word.erase(0,2);
        else
            word.erase(pos,l-pos);
    }

    //delete token in the front
    while(word.length()>=1 && !isLowerCase(word[0]))
    {
        word.erase(0,1);
    }
    //delete token at the end of a sentence
    while(word.length()>=1 && !isLowerCase(word[word.length()-1]))
    {
        word.erase(word.length()-1,1);
    }

}
void TakeStem(string &word)
{
    Porter2Stemmer::trim(word);
    Porter2Stemmer::stem(word);
}
string ProcessWord(string &word)
{
    transform(word.begin(),word.end(),word.begin(),::tolower);//transform into lower cases
    RemoveTokens(word);//delete tokens(not including tokens inserted into the word)
    TakeStem(word);//stem algorithm
    return word;
}
vector<string> ProcessPhrase(string phrase)
{
    vector<string> ret;
    string word;
    int pos, wordStart;

    pos = 0;
    while (pos < phrase.length()) {
        while (pos < phrase.length() && isWhite(phrase[pos])) pos++;
        wordStart = pos;
        
        while (pos < phrase.length() && !isWhite(phrase[pos])) pos++;

        if (wordStart < pos)
        {
            string word=phrase.substr(wordStart, pos-wordStart);
            ProcessWord(word);
            if(word.length()!=0)
                ret.push_back(word);
        }
    }
    return ret;
}

void GenerateDocInfo()
{
    for(int i=1;i<DOCNUM;i++)//category info
    {
        if(1<=i&&i<=17)
            docCategory[i-1]=categories[0];
        else if(18<=i&&i<=27)
            docCategory[i-1]=categories[1];
        else if(28<=i&&i<=37)
            docCategory[i-1]=categories[2];
        else if(38<=i&&i<=42)
            docCategory[i-1]=categories[3];
        ProcessWord(docCategory[i-1]);
    }
    for(int i=1;i<=DOCNUM;i++)//title info
    {
        string docName="Documents/"+dec2_to_string(i)+".txt";
        ifstream ifs(docName);
        if(!ifs.is_open())
        {
            cout<<"Error:Can not read document "<<i<<endl;
            return;
        }
        else
        {
            getline(ifs,docTitles[i-1]);
        }
        ifs.close();
    }
}
void ReadDoc(int docID)
{
    string docName="Documents/"+dec2_to_string(docID)+".txt";
    ifstream ifs(docName);
    if(!ifs.is_open())
    {
        cout<<"Error:Can not read document "<<docID<<endl;
        return;
    }
    string word;
    int pos=0;//position of a word

    while(ifs>>word)
    {
        pos++;
        ProcessWord(word);
        if(word.length()!=0)
        {
            myPostingList.add(word,docID,pos);
        }   
    }
    ifs.close();
}
void Index()
{
    double start=clock();
    for(int i=1;i<=DOCNUM;i++)
    {
        cout<<"\n-------------------Indexing Document "<<i<<"-------------------"<<endl;
        ReadDoc(i);
    }
    myPostingList.MarkStopWords();//mark stop words in the list. not searching them
    cout<<"Total time for indexing "<<DOCNUM<<" documents: "<<(clock()-start)/1000<<"s"<<endl;
    cout<<"Total words number: "<<myPostingList.getAllWordNum()<<endl;
    cout<<"Total distinct nonstop words: "<<myPostingList.getDistinctNonstopWordNum()<<endl;
}
void DisplayInterface()
{
    cout<<"\n-------------------------------------Shakole-------------------------------------"<<endl;
    for(int i=0;i<2;i++)
        cout<<"+                                                                               +"<<endl;
    cout<<"+  Welcome to use Shakole search engine                                         +"<<endl;
    cout<<"+  You can add \"=category\" at the end of the searched key to query by category  +"<<endl;
    cout<<"+  All Categories: comedy, history, tragedy, poetry.(\"all\" by default)          +"<<endl;
    for(int i=0;i<2;i++)
        cout<<"+                                                                               +"<<endl;
}
void Query()
{
    while(true)
    {
        string key;
        cout<<"\nPlease enter the key word or phrase(words separated by spaces) you want to search. Enter # to exit:"<<endl;
        getline(cin,key);
        if(key=="#")
        {
            cout<<"Thanks for using Shakole!"<<endl;
            return;
        }
        else//search
        {
            vector<string> words;
            string category="all";
            int pos=key.rfind('=');
            if(pos!=-1)
            {
                category=key.substr(pos,key.length()-pos);
                ProcessWord(category);
                if(!isCategory(category))
                {
                    cout<<"No such category!"<<endl;
                    continue;
                }
                key=key.substr(0,pos);
            }

            words=ProcessPhrase(key);//input word will be processed as well
            vector<int> result_docs;
            result_docs=myPostingList.search(words);//search in the posting list!

            cout<<"Relative Documents:"<<endl;
            int resultNum=0;
            for(vector<int>::iterator it=result_docs.begin();it!=result_docs.end();it++)
            {   
                if(category=="all"||docCategory[*it-1]==category)
                {
                    resultNum++;
                    cout<<"Doc "<<*it<<": "<<docTitles[*it-1]<<endl;
                }
            }   
            if(resultNum==0)
                cout<<"Can't find any result! (maybe stop words)"<<endl;
            else
                cout<<"(Total results: "<<resultNum<<")"<<endl;
        }
    }
}


int main()
{
    GenerateDocInfo();//create the document title table(not necessary in fact) and their category.
    Index();//create the inverted file index for all documents
    DisplayInterface();//display the interface
    Query();//do query

    return 0;
}