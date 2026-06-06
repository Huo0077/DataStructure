#include<iostream>
#include<vector>
using namespace std;
enum State{Empty,Occupied,Deleted};
struct tablestate
{
    int key;
    State state;
    tablestate():state(Empty){}
};
class hash
{
private:
    vector<tablestate> table;
    int currentSize;
    
    int myHash(int key)
    {
        return key % table.size();
    }
public:
    hash(int size = 10)
    {
        table.resize(size);
        currentSize = 0;
    }

    bool search(int val)
    {
        int pos = myHash(val);
        int startpos = pos;
        while(table[pos].state != Empty)
        {
            if(table[pos].state == Occupied && table[pos].key == val)
            return true;
            pos = (pos + 1) % table.size();
            if(pos == startpos)
            break;
        }
        return false;
    }

    void insert(int val)
    {
        if(search(val))
        {
            cout<<"数据已经存在"<<endl;
            return;
        }
        int pos = myHash(val);
        int startpos = pos;
        while(table[pos].state == Occupied)
        {
            pos = (pos + 1) % table.size();
            if(pos == startpos)
            {
                rehash();
                insert(val);
                return;
            }
        }
        table[pos].key = val;
        table[pos].state =  Occupied;
        currentSize++;
    }

    void remove(int val)
    {
        int pos = myHash(val);
        int startpos = pos;
        while(table[pos].state != Empty)
        {
            if(table[pos].state == Occupied && table[pos].key == val)
            {
                table[pos].state = Deleted;
                currentSize--;
                cout<<"已删除"<<endl;
                return;
            }
            pos = (pos + 1) % table.size();
            if(pos == startpos)
            break;
        }
        cout<<"表中不存在目标数"<<endl;
    }

    void rehash()
    {
        vector<tablestate> oldtable = table;
        table.resize(table.size() * 2);
        currentSize = 0;
        for (int i = 0; i < oldtable.size(); i++)
        {
            if (oldtable[i].state == Occupied)
            {
                insert(oldtable[i].key);
            }
        }
    }
};

