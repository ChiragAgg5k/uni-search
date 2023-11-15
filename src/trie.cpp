//
// Created by Chirag Aggarwal on 07/09/23.
//

#include "../include/trie.h"

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    // Implement destructor to free memory if needed.
}

Trie::TrieNode::TrieNode() {
    isEndOfWord = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        children[i] = nullptr;
    }
}

void Trie::insert(const std::string &key) {
    TrieNode *pCrawl = root;
    for (char c: key) {
        int index = c - 'a';
        if (!pCrawl->children[index]) {
            pCrawl->children[index] = new TrieNode();
        }
        pCrawl = pCrawl->children[index];
    }
    pCrawl->isEndOfWord = true;
}

bool Trie::search(const std::string &key) {
    TrieNode *pCrawl = root;
    for (char c: key) {
        int index = c - 'a';
        if (!pCrawl->children[index]) {
            return false;
        }
        pCrawl = pCrawl->children[index];
    }
    return pCrawl->isEndOfWord;
}

// Function to find keys that match the input the most
std::vector<std::string> findMatches(const Trie &trie, const std::string &input, const std::vector<std::string> &keys) {
    std::vector<std::string> matches;

    // Iterate through the trie to find matching keys
    for (const std::string &key: keys) {
        if (key.find(input) == 0) {
            // The key starts with the input, add it to results
            matches.push_back(key);
        }
    }
    return matches;
}