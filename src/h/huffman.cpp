#include "huffman.h"

void generateCodes(HuffmanNode *root, const string &code, map<int, string> &codes) {
    if (!root)
        return;
    if (!root -> left && !root -> right)        // leaf node
    {
        codes[root -> symbol] = code;
        return;
    }
    generateCodes(root -> left, code + "0", codes);
    generateCodes(root -> right, code + "1", codes);
}

void destroyHuffmanTree(HuffmanNode *root) {
    if (!root) return;
    destroyHuffmanTree(root->left);
    destroyHuffmanTree(root->right);
    delete root;
}

bool cmp(pair<string, int> a, pair<string, int> b)
{ 
    return a.first.size() == b.first.size() ? a.first < b.first : a.first.size() < b.first.size(); 
}

void huffman(FILE *fp, vector<pair<int, int> > EC, unsigned char HTInformation, int flag)
{
    // (size / skip << 4 + size, amplitude)
    map<int, int> freqMap;
    for (int i = 0; i < EC.size(); i ++)
        freqMap[EC[i].first] ++;

    // put all symbols into a priority queue
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, compareNode> q;
    map<int, int>::iterator i;
    for (i = freqMap.begin(); i != freqMap.end(); i ++)
        q.push(new HuffmanNode(i -> first, i -> second));
    
    // only one symbol
    if (q.size() == 1) 
    {
        HuffmanNode* x = q.top(); q.pop();
        HuffmanNode* root = new HuffmanNode(-1, x -> freq);
        root -> left = x;
        q.push(root);
    }
    
    while (q.size() > 1) 
    {
        HuffmanNode *left = q.top(); q.pop();
        HuffmanNode *right = q.top(); q.pop();
        HuffmanNode *parent = new HuffmanNode(-1, left -> freq + right -> freq);
        parent -> left = left;
        parent -> right = right;
        q.push(parent);
    }
    // create Huffman codes
    HuffmanNode *root = q.top();
    map<int, string> codes;     // HUFFVAL -> code
    generateCodes(root, "", codes);
    // free Huffman tree
    destroyHuffmanTree(root);
    // setDHT
    DHTSegment dht;
    dht.marker = 0xFFC4;
    dht.HTInformation = HTInformation;
    dht.HUFFVAL = (int *)malloc(codes.size() * sizeof(int));
    // 码长为 i + 1 的码字数目
    vector <pair<string, int> > v;  // (code, HUFFVAL)
    map <int, int> bits;            // code -> bits
    for (int i = 0; i < 16; i ++)
    {
        int tot_BITSi = 0;
        for (map<int, string>::iterator it = codes.begin(); it != codes.end(); it ++)
            if (it -> second.size() == i + 1)
            {
                v.push_back(make_pair(it -> second, it -> first));
                bits[it -> first] = i + 1;
                tot_BITSi ++;
            }
        dht.BITS[i] = tot_BITSi;
    }
    sort(v.begin(), v.end(), cmp);
    // for (int i = 0; i < v.size(); i ++)
    //     printf("(%s,%d)", v[i].first.c_str(), v[i].second);
    // printf("\n");
    // transform to Canonical Huffman Codes
    int code = 0;
    map <int, int> CanonicalCodes;  // HUFFVAL -> code
    for (int i = 0; i < v.size(); i ++)
    {
        if (!i)
        {
            CanonicalCodes[v[i].second] = code;
            continue;
        }
        if (v[i].first.size() != v[i - 1].first.size())
        {
            code ++;
            int delta = v[i].first.size() - v[i - 1].first.size();
            while (delta --)
                code = code << 1;
        }
        else
            code ++;
        CanonicalCodes[v[i].second] = code;
    }
    for (int i = 0; i < v.size(); i ++)
        dht.HUFFVAL[i] = v[i].second;
    for (int i = 0; i < v.size(); i ++)
        printf("(%d,%d)", v[i].second, CanonicalCodes[v[i].second]);
    printf("\n");
    // get Huffman codes
    // concat 8 bits as a byte
    int bitCount = 0, bytes = 0;
    vector <unsigned char> byte;
    for (int i = 0; i < EC.size(); i ++)
    {
        int code = CanonicalCodes[EC[i].first], amplitude = EC[i].second;
        bytes = (bytes << bits[EC[i].first]) + code;
        bitCount += bits[EC[i].first];
        if (bitCount >= 8)
        {
            bitCount -= 8;
            byte.push_back(bytes >> bitCount);
            bytes = bytes & ((1 << bitCount) - 1);
        }
        // if (i < 10)
        //     printf("(%d,%d)\n", code, amplitude);
        // 001 -> -6
        // 101 -> 5
        int size = flag == 0 ? EC[i].first : EC[i].first & 15;
        if (amplitude < 0)
            amplitude = (1 << size) + amplitude - 1;
        for (int j = size - 1; j >= 0; j --)
        {
            bytes = (bytes << 1) + ((amplitude >> j) & 1);
            bitCount ++;
            if (bitCount >= 8)
            {
                bitCount -= 8;
                byte.push_back(bytes >> bitCount);
                bytes = bytes & ((1 << bitCount) - 1);
            }
        }
    }
    if (bitCount > 0)           // last byte
    {
        bytes = bytes << (8 - bitCount);
        byte.push_back(bytes);
    }
    if (byte.size() % 2 == 0)   // padding
        byte.push_back(0);
    dht.length = 2 + 4 + 1 + 16 + codes.size() * sizeof(int) + byte.size();
    // print DHT
    fwrite(&dht.marker, sizeof(unsigned short), 1, fp);
    fwrite(&dht.length, sizeof(int), 1, fp);
    fwrite(&dht.HTInformation, sizeof(unsigned char), 1, fp);
    fwrite(dht.BITS, sizeof(unsigned char), 16, fp);
    fwrite(dht.HUFFVAL, sizeof(int), CanonicalCodes.size(), fp);
    // printf("%d %d\n", dht.length, dht.HTInformation);
    for (int i = 0; i < byte.size(); i ++)
    {
        // if (i == 0)
        //     printf("%x\n", byte[i]);
        fwrite(&byte[i], sizeof(unsigned char), 1, fp);
    }
    printf("%ld\n", byte.size());
}

void reconstructHuffman(HuffmanNode *&root, unsigned char *BITS, int *HUFFVAL)
{
    // reconstruct Huffman tree
    root = new HuffmanNode(-1, 0);
    int code = 0, cnt = 0;
    for (int i = 0; i < 16; i ++)
    {
        for (int j = 0; j < BITS[i]; j ++)
        {
            HuffmanNode *p = root;
            int x = code ++;
            for (int k = i; k >= 0; k --)
                if ((x >> k) & 1)
                {
                    if (!p -> right)
                        p -> right = new HuffmanNode(-1, 0);
                    p = p -> right;
                }
                else
                {
                    if (!p -> left)
                        p -> left = new HuffmanNode(-1, 0);
                    p = p -> left;
                }
            p -> symbol = HUFFVAL[cnt ++];
            printf("(%d,%d)", p -> symbol, code - 1);
        }
        if (cnt > 0)
            code = code << 1;
    }
    printf("\n");
}