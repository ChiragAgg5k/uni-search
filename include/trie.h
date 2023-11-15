//
// Created by Chirag Aggarwal on 07/09/23.
//

#ifndef UNISEARCH_TRIE_H
#define UNISEARCH_TRIE_H

#include <string>

const int ALPHABET_SIZE = 26;

class Trie {
public:
    Trie();

    ~Trie();

    void insert(const std::string &key);

    bool search(const std::string &key);

private:
    struct TrieNode {
        TrieNode *children[ALPHABET_SIZE]{};
        bool isEndOfWord;

        TrieNode();
    };

    TrieNode *root;
};

std::vector<std::string> findMatches(const Trie &trie, const std::string &input, const std::vector<std::string> &keys);

#endif //UNISEARCH_TRIE_H

