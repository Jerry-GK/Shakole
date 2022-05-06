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
#include "PostingList.h"
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