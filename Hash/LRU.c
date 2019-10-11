#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

vector<ll> address;

ll hex_to_dec(string s)
{
    ll ans = 0;

    for(int i=0;i<(int)s.size();i++)
    {
        if(s[i]>='0'&&s[i]<='9') ans = ans*16 + (s[i]-'0');
        else ans = ans*16 + (s[i]-'A'+10);
    }
    return ans;
}

ll get_block_num(ll x, ll block_size)
{
    return x/block_size;
}

void direct_mapping(ll cache_size, ll block_size)
{
    ll num_cache_blocks = cache_size/block_size;
    ll m[num_cache_blocks];
    for(int i=0;i<num_cache_blocks;i++) m[i] = -1;
    int hits=0,miss=0;

    for(int i=0; i<(int)address.size(); i++)
    {
        ll block_num = get_block_num(address[i], block_size); //in main memory
        ll block_starter = block_num * block_size;            

        if(m[block_num % num_cache_blocks]==-1)
        {
            miss++;
            m[block_num % num_cache_blocks] = block_starter;
        }
        else if(m[block_num % num_cache_blocks]==block_starter) hits++;
        else 
        {
            miss++;
            m[block_num % num_cache_blocks] = block_starter;
        }
    }
    cout<<"         "<<hits<<"/"<<miss<<"         |";
}

void set_associative_lru(ll cache_size, ll block_size)
{
    ll num_cache_blocks = cache_size/(block_size*4);
    ll m[num_cache_blocks][4],counter[num_cache_blocks][4];

    for(int i=0;i<num_cache_blocks;i++) 
        for(int j=0;j<4;j++)
            m[i][j] = -1, counter[i][j] = 0;
    
    int hits=0,miss=0;

    for(int i=0; i<(int)address.size(); i++)
    {
        ll block_num = get_block_num(address[i], block_size);
        ll block_starter = block_num * block_size; 
        int flag = 0;
        int p = block_num % num_cache_blocks;

        /* LRU */
        for(int i=0;i<4;i++)    //cache hit
        {
            if(m[p][i]==block_starter)
            {
                flag++;
                hits++;
                for(int j=0;j<4;j++)
                {
                    if(i==j) counter[p][j] = 0;
                    else counter[p][j] += 1;
                }
                break;
            }
        }

        if(flag) continue;

        for(int i=0;i<4;i++)   //cache miss but cache not full
        {
            if(m[p][i]==-1)
            {
                flag--;
                miss++;
                m[p][i] = block_starter;
                for(int j=0;j<4;j++)
                {
                    if(i==j) counter[p][j] = 0;
                    else counter[p][j] += 1;
                }
                break;
            }
        }

        if(flag) continue;
        
        miss++;
        int k = 0, highest = -1;
        for(int i=0;i<4;i++)   //cache miss but cache is full
        {
            if(counter[p][i]>highest)
            {
                highest = counter[p][i];
                k = i;
            }
        }

        m[p][k] = block_starter;
        for(int j=0;j<4;j++)
        {
            if(j==k) counter[p][j] = 0;
            else counter[p][j] += 1;
        }
    }
    cout<<"              "<<hits<<"/"<<miss<<"                    |";
}

void set_associative_fifo(ll cache_size, ll block_size)
{
    ll num_cache_blocks = cache_size/(block_size*4);
    vector<list<ll> > m(num_cache_blocks);
    
    int hits=0,miss=0;

    for(int i=0; i<(int)address.size(); i++)
    {
        ll block_num = get_block_num(address[i], block_size);
        ll block_starter = block_num * block_size; 
        int flag = 0;
        int p = block_num % num_cache_blocks;
        int n = (int)m[p].size();

        /* fifo */
        for(auto it: m[p])   // cache hit
            if(it == block_starter) 
            {
                hits++;
                flag++;
                break;
            }

        if(flag) continue;

        if(m[p].size()<4)   // cache miss but cache is not full
        {
            miss++;
            m[p].push_back(block_starter);
            continue;
        }

        miss++; 
        m[p].pop_front();   // cache miss but cache is full
        m[p].push_back(block_starter);
    }
    cout<<"             "<<hits<<"/"<<miss<<"                     |";
} 

int main()
{
    ifstream f;
    f.open("address.txt");
    while(f)
    {
        string inp;
        getline(f,inp);
        address.push_back(hex_to_dec(inp));
    }
    f.close();
    ll cache_size[4] = {1024,2048,4096,8192};
    ll block_size[2] = {16,32};

    
    for(int i=0; i<2; i++)
    {
        cout<<"-----------------------------------------------------Block Size: "<<block_size[i]<<"---------------------------------------------------------------\n";
        cout<<"| Cache Size | Direct Mapping(Hits/Miss) | 4-Way Set Associative with LRU(Hits/Miss) | 4-Way Set Associative with FIFO(Hits/Miss)|\n";
        cout<<"----------------------------------------------------------------------------------------------------------------------------------\n";
        for(int j=0; j<4; j++)
        {
            cout<<"|  "<<cache_size[j]<<"      |";
            direct_mapping(cache_size[j], block_size[i]);
            set_associative_lru(cache_size[j], block_size[i]);
            set_associative_fifo(cache_size[j], block_size[i]);
            cout<<"\n";
        }
        cout<<"----------------------------------------------------------------------------------------------------------------------------------\n\n";
    }

    return 0;
}