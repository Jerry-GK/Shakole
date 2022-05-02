//Implemention of a posting list, maintaining an AVL Tree in it to access terms

/*
IMPORTANT!!!!!!!
Posting list structre:(each record is a row for a certain term)
record[0]----------------------------
record[1]----------------------------
....

record[size-1]-----------------------

For each record:
{
  (int)termID | (string)term | (int)frequency(in all documents) | (bool)isStop
| < docID1, times(frequency in this file, at least one), <pos[0],pos[1],....pos[pos.size()-1]> >
  < docID2, times, <pos[0],pos[1],...pos[pos.size()-1]> >
    .....

  < docIDn, times, <pos[0],pos[1],................pos[pos.size()-1]> >
}

Words with occurence rate higher than a certain value will be marked as stop words

We store <term, termID> in an AVL Tree to access the termID by the string itself.
*/

#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<time.h>
#include<map>
#include"myAVLTree.h"

#define DOCNUM 42
using namespace std;

//after sorted in ascending order of frequency, only this rate of input words will be searched
const double global_threshold_query=1.0;
//if a word occurence rate is more than this value, it's marked as a stop word, not participate in serching
const double global_stop_word_rate=0.0013;

struct _doc_pos
{
    int docID;//document ID
    int times;//times of a certain word that occurs in this document
    vector<int> pos;//occurence position
};

typedef struct _doc_pos doc_pos;
struct _record
{
    int ID;//term ID, also the row number of the record 
    string term;//(word)
    int frequency;//occurence times of term
    bool isStop;//if the term is a  stop word
    vector<doc_pos> occurence;//docID and the position where term appears at this document
};
typedef struct _record record;

class PostingList
{
private:
    int size;//number of records(also number of all distinct words)
    int wordNum;//number of all words(not distinct)
    int stopWordNum;//number of stop words
    vector<record> records;//records vector
    AVLTree wordTree;//this is an AVL tree that stores words and their ID.

public:
    PostingList();
    int getDistinctNonstopWordNum();//return the number of distinct nonstop words
    int getAllWordNum();//return the number of all words
    int findID(string term);//find the ID(or row number in posting list) of a term
    void add(string word,int docID,int pos);//add a word
    void MarkStopWords();//mark stop words in the list, do not search them 
    vector<int> search(vector<string>key);//search a word or phrase
};

PostingList::PostingList()
{
    this->size=0;
    this->wordNum=0;
    this->stopWordNum=0;
}

int PostingList::getDistinctNonstopWordNum()//return the number of distinct nonstop word
{
    return this->size-this->stopWordNum;
}

int PostingList::getAllWordNum()
{
    return this->wordNum;
}

int PostingList::findID(string term)//return the ID of a term. return -1 if not exists
{
    int retID = wordTree.Find(term);
    return retID;
}

void PostingList::add(string word,int docID,int pos)
{
    this->wordNum++;
    int termID=findID(word);
    if(termID!=-1)//add into existed record
    {
        this->records[termID].frequency++;
        for(vector<doc_pos>::iterator it=this->records[termID].occurence.begin();it!=this->records[termID].occurence.end();it++)
        {
            if(it->docID==docID)
            {
                it->pos.push_back(pos);
                it->times++;
                return;
            }
        }
        doc_pos newDocPos;
        newDocPos.docID=docID;
        newDocPos.pos.push_back(pos);
        newDocPos.times=1;
        this->records[termID].occurence.push_back(newDocPos);
    }
    else//create new record(a new term)
    {
        record newRec;
        newRec.ID=this->size;
        newRec.frequency=1;
        newRec.term=word;
        newRec.isStop=false;
        doc_pos newDocPos;
        newDocPos.docID=docID;
        newDocPos.pos.push_back(pos);
        newDocPos.times=1;
        newRec.occurence.push_back(newDocPos);
        this->records.push_back(newRec);
        this->size++;

        wordTree.Insert(make_pair(newRec.term,newRec.ID));
    }
    return;
}

void PostingList::MarkStopWords()
{
    //cout<<this->wordNum*global_stop_word_rate<<endl;
    for(vector<record>::iterator it=this->records.begin();it!=this->records.end();it++)
    {
        if(it->frequency>this->wordNum*global_stop_word_rate)
        {
            //cout<<it->term<<": "<<it->frequency<<endl;
            it->isStop=true;
            this->stopWordNum++;
        }
    }
}

vector<int> PostingList::search(vector<string> key)//return the vector of document IDs of searched key
{
    vector<int> ret_docs;
    //scan first and erase stop words (vector dynamic deletion is a little complicated to implement)
    vector<string>::iterator it=key.begin();
    while(true)
    {
        if(it==key.end())
            break;
        int termID=findID(*it);
        if(termID==-1)
        {
            return ret_docs;
        }
        else if(this->records[termID].isStop==true)// not search stop words
        {
            key.erase(it);
            it=key.begin();
        }
        else
            it++;
    }

    //sort the input word vector "key" in ascending order according to frequency
    int l=key.size();
    if(l==0)
        return ret_docs;

    vector<int> freq;//freq[i] is the frequency of key[i] in all documents
    for(int i=0;i<l;i++)
    {
        int termID=findID(key[i]);
        if(termID==-1)
        {
            return ret_docs;
        }
        else
        {
            freq.push_back(this->records[termID].frequency);
        }
    }
    for(int i=0;i<l;i++)
    {
        int min=freq[i],minIndex=i;
        for(int j=i+1;j<l;j++)
        {
            if(freq[j]<min)
            {
                min=freq[j];
                minIndex=j;
            }
        }
        //swap
        int temp=freq[minIndex];
        freq[minIndex]=freq[i];
        freq[i]=temp;
        string ttemp=key[minIndex];
        key[minIndex]=key[i];
        key[i]=ttemp;
    }

    //threshold for searching
    int cut=(int)(l*global_threshold_query)+1;
    int cut_num=l-cut;
    while((cut_num--)>0)
    {
        key.pop_back();
    }

    //search
    //the first element is possible docID(key[0] determines). the second is how many terms in key appear in the document,
        //if the second equals to key.size(), it means this document contain all searched words
    map<int, int> docID_time;
    int termID=findID(key[0]);
    //first narrow the range of possible documents using key[0] (the term with the lowest frequency)
    for(vector<doc_pos>::iterator it=records[termID].occurence.begin();it!=records[termID].occurence.end();it++)
    {
        docID_time.insert(make_pair(it->docID,1));
    }
    for(int i=1;i<key.size();i++)
    {
        termID=findID(key[i]);
        for(vector<doc_pos>::iterator it=records[termID].occurence.begin();it!=records[termID].occurence.end();it++)
        {
            docID_time.find(it->docID)->second++;
        }
    }

    //return results
    for(map<int,int>::iterator it=docID_time.begin();it!=docID_time.end();it++)
    {
        if(it->second==key.size())//it means doc[it->first] contain all terms of key
        {
            ret_docs.push_back(it->first);
        }
    }
    return ret_docs;
}