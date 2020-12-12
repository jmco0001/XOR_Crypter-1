#include <map>
#include "huffman.h"

using namespace std;

void HuffmanCompressor::tree_to_table() {
    std::stack<std::pair<Node*, std::pair<int, int>>> call_stack;

    call_stack.push(
        std::make_pair(new Node(*tree_root), std::make_pair(0, 0))
    );

    while (call_stack.size() != 0) {
        auto current = call_stack.top();

        int code = current.second.first;
        int code_len = current.second.second;
        Node* node = current.first;

        call_stack.pop();

        if (node->is_ending) {
            huffman_table[node->value] = std::make_pair(code, code_len);
        }
        else {
            call_stack.push(
                std::make_pair(node->zero, std::make_pair(code << 1, code_len + 1))
            );

            call_stack.push(
                std::make_pair(node->one, std::make_pair((code << 1) | 1, code_len + 1))
            );
        }
    }
}

void HuffmanCompressor::build_huffman_tree() {
    std::priority_queue<Node> nodes;

    for (auto& p : letters_count)
        nodes.push(Node(p.first, p.second, true));

    while (nodes.size() > 1) {
        Node* first = new Node(nodes.top()); nodes.pop();
        Node* second = new Node(nodes.top()); nodes.pop();

        nodes.push(
            Node(0, first->cost + second->cost, first, second)
        );
    }

    tree_root = new Node(nodes.top());
}

void HuffmanCompressor::build_huffman_table() {
    build_huffman_tree();
    tree_to_table();
}

// Storing method taken from neerc.ifmo.ru
void HuffmanCompressor::store_tree(Node* root) {
    if (root->is_ending) {
        alphabet += root->value;
    }
    else {
        push_bit(0);
        store_tree(root->zero);
        push_bit(1);
        store_tree(root->one);
        push_bit(1);
    }
}

void HuffmanCompressor::store_alphabet() {
    output_handle.write(alphabet.c_str(), alphabet.length());
}

void HuffmanCompressor::compress(const char* output_file) {
    output_handle.open(output_file, std::ios::binary); char c;

    if (!input_handle.is_open()) {
        cout << "[-] Unable to open input file" << std::endl;
        return;
    }

    if (!output_handle.is_open()) {
        std::cout << "[-] Unable to create output file" << std::endl;
        return;
    }

    while (input_handle.read(&c, 1)) {
        if (letters_count.count(c) != 0) {
            letters_count[c]++;
        }
        else {
            letters_count[c] = 0;
        }
    }

    input_handle.clear();
    int file_size = input_handle.tellg();

    // Dumping file header and unpacked file size
    output_handle.write("HUFF", 4);
    output_handle.write((char*)&file_size, sizeof(int));

    // Dumping alphabet and tree
    build_huffman_table();
    store_tree(tree_root);
    trail_bits();
    store_alphabet();

    // Dumping compressed data
    input_handle.seekg(0, std::ios::beg);

    while (input_handle.read(&c, 1)) {
        int code = huffman_table[c].first;
        int code_len = huffman_table[c].second;

        int mask = (1 << (code_len - 1));

        while (mask) {
            if (code & mask)
                push_bit(1);
            else
                push_bit(0);

            mask >>= 1;
        }
    }

    trail_bits();

    std::cout << "[+] Successfully packed (" << file_size
        << " -> " << output_handle.tellp() << ")" << std::endl;
}

void HuffmanCompressor::push_bit(int bit) {
    current_byte |= bit;

    if (++total == 8) {
        output_handle.write(&current_byte, 1);
        current_byte = 0; total = 0;
    }

    current_byte <<= 1;
}

void HuffmanCompressor::trail_bits() {
    if (total != 0) {
        current_byte <<= (7 - total);
        output_handle.write(&current_byte, 1);
        current_byte = 0; total = 0;
    }
}

void HuffmanCompressor::restore_huffman_tree() {
    Node* current_node = new Node(0, 0);
    tree_root = current_node; char c;

    std::stack<Node*> path;
    std::vector<Node*> leafs;
    bool tree_completed = false;

    while (!tree_completed) {
        input_handle.read(&c, 1); int mask = 128;

        while (mask) {
            if (tree_completed) {
                mask = 0; continue;
            }

            if ((c & mask) == 0) {
                current_node->zero = new Node(0, 0, true);
                current_node->one = new Node(0, 0, true);
                current_node->is_ending = false;

                path.push(current_node);
                current_node = current_node->zero;
            }
            else {
                Node* parent = path.top();

                if (current_node->is_ending) {
                    leafs.push_back(current_node);
                }

                if (current_node == parent->zero) {
                    current_node = parent->one;
                }
                else {
                    current_node = parent; path.pop();

                    if (path.empty())
                        tree_completed = true;
                }
            }

            mask >>= 1;
        }
    }

    for (auto p : leafs) {
        input_handle.read(&c, 1);
        p->value = c;
    }
}

void HuffmanCompressor::decompress(const char* output_file) {
    output_handle.open(output_file, std::ios::binary); char c;
    char file_header[4]; int file_size = 0; int bytes_written = 0;

    if (!input_handle.is_open()) {
        std::cout << "[-] Unable to open input file" << std::endl;
        return;
    }

    if (!output_handle.is_open()) {
        std::cout << "[-] Unable to create output file" << std::endl;
        return;
    }

    // Checking that the file has been compressed with this program
    input_handle.read(file_header, 4);

    if (strcmp(file_header, "HUFF") != 0) {
        std::cout << "[-] It seems file isn't packed by this program." << std::endl;
        exit(0);
    }

    input_handle.read((char*)&file_size, sizeof(int));

    // First of all, restoring tree
    restore_huffman_tree();

    // And after that, decompress!
    Node* current_node = tree_root;

    while (input_handle.read(&c, 1) && file_size != bytes_written) {
        int mask = 128;

        while (mask) {
            if (current_node->is_ending) {
                output_handle.write(&current_node->value, 1);
                current_node = tree_root; bytes_written++;
            }

            if (c & mask) {
                current_node = current_node->one;
            }
            else {
                current_node = current_node->zero;
            }

            mask >>= 1;
        }
    }

    input_handle.clear(); input_handle.seekg(0, std::ios::end);

    std::cout << "[+] Successfully unpacked (" << input_handle.tellg()
        << " -> " << file_size << ")" << std::endl;
}