#include <utility>
#include <fstream>
#include <string>
#include <stack>
#include <queue>
#include <map>
#include <iostream>
using namespace std;

struct Node {
    Node* one = nullptr; Node* zero = nullptr;
    bool is_ending = false; int cost = 0; char value;

    Node(char letter, int cost, bool is_ending = false) :
        value(letter), cost(cost), is_ending(is_ending) {};

    Node(char letter, int cost, Node* zero, Node* one) :
        value(letter), cost(cost), is_ending(false), one(one), zero(zero) {};

    bool operator<(const Node& second) const {
        return cost > second.cost;
    }
};

class HuffmanCompressor {
private:
    std::map<char, std::pair<int, int>> huffman_table;
    std::string alphabet; Node* tree_root;
    char current_byte = 0; int total = 0;
    std::map<char, int> letters_count;
    std::ofstream output_handle;
    std::ifstream input_handle;

    void store_tree(Node* root);
    void restore_huffman_tree();
    void build_huffman_table();
    void build_huffman_tree();
    void push_bit(int bit);
    void store_alphabet();
    void tree_to_table();
    void trail_bits();
public:
    void decompress(const char* output_file);
    void compress(const char* output_file);

    HuffmanCompressor(const char* input_file) :
        input_handle(std::ifstream(input_file, std::ios::binary)),
        output_handle(std::ofstream()) {};
};
