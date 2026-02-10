#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <openssl/sha.h>
#include <iomanip>
#include <ctime>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json; 


class Block 
{
private:
    int index; 
    long long nonce; 
    time_t timest; 
    string diff;
    string prev_hash, curr_hash, data;
public:
    Block(int ind, string prev_hs, string curr_hs, string d, string difficuty, time_t timestamp, long long numberUsedOnce)
    {
        index = ind;
        prev_hash = prev_hs;
        data =  d;
        curr_hash = curr_hs;
        diff = difficuty;
        timest = timestamp;
        nonce = numberUsedOnce;
    }
    string get_curr_hash()
    {
        return curr_hash;
    }
    string calc_hash() {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        string aldata = to_string(index) + prev_hash + data + to_string(timest) + to_string(nonce) + diff;
        SHA256((unsigned char*)aldata.c_str(), aldata.length(), hash);
	    stringstream fina_hash;
	    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) fina_hash << hex << setw(2) << setfill('0') << (int)hash[i];

        return fina_hash.str();
    }
    bool mine_block(string prev) {
        curr_hash = calc_hash();
        prev_hash = prev;
        timest = time(nullptr);
        while (curr_hash.substr(0, diff.size()) != diff) {
            ++nonce;
            curr_hash = calc_hash();
        }
        return true;
    }
    string get_prev_hash()
    {
        return prev_hash;
    }
    string get_data()
    {
        return data;
    }
    string get_diff() {
        return diff;
    }
    time_t get_time()
    {
        return timest;
    }
    int get_ind() {
        return index;
    }
    long long get_nonce() {
        return nonce;
    }
};

class Blockchain
{
private:
    vector<Block> chain;

public:
    bool createGenesisBlock()
    {
        chain.emplace_back(0, "Genesis Block", "", "The Beginning", "0f0c", time(nullptr), 0);
        chain[0].mine_block("Genesis Block");
        return true;
    }
    bool newBlock(string d, time_t timestamp = time(nullptr), string prev_hs = "", string curr = "", long long numberUsedOnce = 0, string diff = "default") {
        if (diff == "default") diff = "0f0c";
        chain.emplace_back(chain.size(), prev_hs, curr, d, diff, timestamp, numberUsedOnce);
        return true;
    }
    bool validatchain() {
        if (chain[0].get_curr_hash() != chain[0].calc_hash()) return false;
        if (chain[0].get_diff() != "0f0c") return false;
        for (int i = 1; i < chain.size(); ++i) {
            if (chain[i].get_prev_hash() != chain[i - 1].get_curr_hash()) return false;
            if (chain[i].get_curr_hash() != chain[i].calc_hash()) return false;
            if (chain[i].get_curr_hash().substr(0, chain[i].get_diff().size()) != chain[i].get_diff()) return false;
        }
        return true;
    }
    bool coutAll() {
        for (int i = 0; i < chain.size(); ++i) {
            cout << "-----" << '\n';
            cout << "index: " << chain[i].get_ind() << '\n';
            cout << "prev: " << chain[i].get_prev_hash() << '\n';
            cout << "curr: " << chain[i].get_curr_hash() << '\n';
            cout << "data: " << chain[i].get_data() << '\n';
            cout << "diff: " << chain[i].get_diff() << '\n';
            time_t timed = chain[i].get_time();
            cout << "time: " << timed << " " << ctime(&timed);
            cout << "nonce: " << chain[i].get_nonce() << '\n';
            cout << "-----" << '\n';
        }
        return true;
    }
    bool addblocktojson(json& blockJSon, int i = -1) {
        if (i < 0) i = blockJSon["chain"].size();
        blockJSon["chain"][i]["index"] = i;
        blockJSon["chain"][i]["data"] = chain[i].get_data();
        blockJSon["chain"][i]["time"] = chain[i].get_time();
        blockJSon["chain"][i]["prev_hash"] = chain[i].get_prev_hash();
        blockJSon["chain"][i]["curr_hash"] = chain[i].get_curr_hash();
        blockJSon["chain"][i]["nonce"] = chain[i].get_nonce();
        blockJSon["chain"][i]["diff"] = chain[i].get_diff();
        return true;
    }
    bool mine(int i = -1) {
        if (i < 1) {
            i = chain.size() - 1;
            if (i == 0) {
                cout << "What do you mean by mining genesis blocks again?" << '\n';
                return false;
            }
        }
        if (i >= chain.size()) {
            cout << "What do you mean by mining a block that doesn't exist?\n";
            return false;
        }
        chain[i].mine_block(chain[i - 1].calc_hash());
        return true;
    }
};

int main()
{
    // ios::sync_with_stdio(false);
    // cin.tie(nullptr);
    // cout.tie(nullptr);
    Blockchain chainLeader;
    ifstream blockFileI("blocks.json");
    json blockJSon;
    try {
        blockFileI >> blockJSon;
    }
    catch (json::parse_error&) {
        blockJSon = json::object();
        blockJSon["chain"] = json::array();
        system("touch blocks.json");
    }
    
    for (int i = 0; i < blockJSon["chain"].size(); ++i) {
        chainLeader.newBlock(blockJSon["chain"][i]["data"], blockJSon["chain"][i]["time"], blockJSon["chain"][i]["prev_hash"], blockJSon["chain"][i]["curr_hash"], blockJSon["chain"][i]["nonce"], blockJSon["chain"][i]["diff"]);
    }
    if (blockJSon["chain"].size() <= 0) {
        chainLeader.createGenesisBlock();
        chainLeader.addblocktojson(blockJSon);
    }
    string inp;
    while (inp != "out") {
        cout << "comm&>> ";
        cin >> inp;
        if (inp == "new") {
            cout << "data: ";
            string data;
            cin >> data;
            cout << "\n";
            chainLeader.newBlock(data);
        }
        if (inp == "ls") chainLeader.coutAll();
        if (inp == "mine") {
            int i;
            cout << "Block index [-1 for default]: ";
            cin >> i;
            cout << '\n';
            chainLeader.mine(i);
            chainLeader.addblocktojson(blockJSon, i);
        }
        if (inp == "valida") {
            if (chainLeader.validatchain()) cout << "Chain is valid..." << '\n';
            else cout << "Chain is invalid!" << '\n';
        }
    }
    ofstream blockFileO("blocks.json");
    blockFileO << setw(4) << blockJSon;
    
    return 0;
}

