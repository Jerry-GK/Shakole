#include "PostingList.h"

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