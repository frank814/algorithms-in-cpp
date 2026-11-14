// huffman_coding.cpp
// Huffman coding: builds an optimal prefix code tree from character
// frequencies using std::priority_queue, encodes a string to a '0'/'1'
// bit-string and decodes it back to the original text.
//
// Time complexity: O(n + k log k) to encode (n = text length, k = alphabet),
// O(k log k) to build the tree, O(bits) to decode.
// Space complexity: O(k) for the tree and code table.

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <utility>
#include <vector>

class Huffman {
public:
    // Builds the code table from the frequencies of `text`.
    // For a single distinct character the code is defined as "0".
    void build(const std::string& text) {
        codes_.clear();
        std::map<char, int> freq;
        for (char c : text) ++freq[c];

        // Min-heap of (frequency, node) pairs.
        struct Node {
            char ch;          // '\0' for internal nodes
            int freq;
            Node* left;
            Node* right;
            Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
        };

        // Lower frequency first (min-heap); ties broken with std::less so the
        // comparison stays well-defined for unrelated pointers.
        using HeapItem = std::pair<int, Node*>;
        auto cmp = [](const HeapItem& a, const HeapItem& b) {
            if (a.first != b.first) return a.first > b.first;
            return std::less<Node*>()(a.second, b.second);
        };
        std::priority_queue<HeapItem, std::vector<HeapItem>, decltype(cmp)> pq(cmp);

        std::vector<Node*> owned;
        for (const auto& [c, f] : freq) {
            Node* n = new Node(c, f);
            owned.push_back(n);
            pq.push({f, n});
        }

        // Empty input: no symbols, no codes.
        if (pq.empty()) return;

        // Degenerate case: one distinct character -> single code "0".
        if (pq.size() == 1) {
            Node* only = pq.top().second;
            codes_[only->ch] = "0";
            for (Node* n : owned) delete n;
            return;
        }

        while (pq.size() > 1) {
            auto [f1, n1] = pq.top(); pq.pop();
            auto [f2, n2] = pq.top(); pq.pop();
            Node* parent = new Node('\0', f1 + f2);
            parent->left = n1;
            parent->right = n2;
            owned.push_back(parent);
            pq.push({parent->freq, parent});
        }
        Node* root = pq.top().second;

        // Assign codes by walking the tree.
        std::function<void(Node*, std::string)> assign = [&](Node* n, std::string prefix) {
            if (n->left == nullptr && n->right == nullptr) {
                codes_[n->ch] = prefix;
                return;
            }
            assign(n->left, prefix + '0');
            assign(n->right, prefix + '1');
        };
        assign(root, "");

        // Free the nodes; the code table no longer needs the tree.
        for (Node* n : owned) delete n;
    }

    std::string encode(const std::string& text) const {
        std::string bits;
        for (char c : text) {
            auto it = codes_.find(c);
            assert(it != codes_.end());  // build() must run first
            bits += it->second;
        }
        return bits;
    }

    // Decodes a bit-string back to the original text.
    // Handles the single-character alphabet where the tree is degenerate.
    std::string decode(const std::string& bits) const {
        if (codes_.empty()) return "";
        if (codes_.size() == 1) {
            // Every symbol is "0".
            for (char b : bits) assert(b == '0');
            return std::string(bits.size(), codes_.begin()->first);
        }
        // Rebuild a decoding walk using the code table (no stored tree).
        std::string out;
        std::string cur;
        std::map<std::string, char> rev;
        for (const auto& [c, code] : codes_) rev[code] = c;
        for (char b : bits) {
            assert(b == '0' || b == '1');
            cur += b;
            auto it = rev.find(cur);
            if (it != rev.end()) {
                out.push_back(it->second);
                cur.clear();
            }
        }
        assert(cur.empty());  // input must end on a code boundary
        return out;
    }

    const std::map<char, std::string>& codes() const { return codes_; }

private:
    std::map<char, std::string> codes_;
};

// Checks that no code is a prefix of any other code.
bool is_prefix_free(const std::map<char, std::string>& codes) {
    std::vector<std::string> v;
    for (const auto& [c, code] : codes) v.push_back(code);
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < v.size(); ++j) {
            if (i == j) continue;
            if (v[j].compare(0, v[i].size(), v[i]) == 0) return false;
        }
    }
    return true;
}

int main() {
    // --- Round-trip on a typical input --------------------------------------
    {
        Huffman h;
        std::string text = "huffman coding compresses repeated characters";
        h.build(text);
        std::string bits = h.encode(text);
        assert(h.decode(bits) == text);
        assert(is_prefix_free(h.codes()));
        std::cout << "text len " << text.size() << " -> bits "
                  << bits.size() << " (8 bits/char baseline: "
                  << text.size() * 8 << ")\n";
        for (const auto& [c, code] : h.codes()) {
            std::cout << "  '" << c << "' = " << code << '\n';
        }
    }

    // --- Frequencies produce expected optimal lengths -----------------------
    {
        // Frequencies a=5, b=3, c=1 -> 'a' must get the shortest code.
        Huffman h;
        std::string text = "aaaaabbbc";
        h.build(text);
        const auto& c = h.codes();
        assert(c.size() == 3);
        // a=5, b=3, c=1: 'a' merges last so it gets the shallowest code.
        assert(c.at('a').size() == 1);
        assert(c.at('b').size() == 2 && c.at('c').size() == 2);
        assert(c.at('b').size() <= c.at('c').size());
        assert(c.at('c').size() <= c.at('a').size() + 1);
        assert(is_prefix_free(c));
        assert(h.decode(h.encode(text)) == text);
    }

    // --- Round-trip on several inputs ---------------------------------------
    {
        std::vector<std::string> inputs = {
            "",
            "abc",
            "aaaaaa",
            "the quick brown fox jumps over the lazy dog",
            "0123456789" "0123456789" "9876543210",
            std::string(100, 'x') + "yz",
        };
        for (const std::string& s : inputs) {
            Huffman h;
            h.build(s);
            std::string bits = h.encode(s);
            assert(h.decode(bits) == s);
            if (s.size() > 0) assert(is_prefix_free(h.codes()));
        }
        std::cout << "round-trip OK for " << inputs.size() << " inputs\n";
    }

    // --- Single distinct character ------------------------------------------
    {
        Huffman h;
        std::string text = "aaaaaaaaaa";
        h.build(text);
        assert(h.codes().size() == 1);
        assert(h.codes().at('a') == "0");
        std::string bits = h.encode(text);
        assert(bits == "0000000000");
        assert(h.decode(bits) == text);
        std::cout << "single char: code \"0\", round-trip OK\n";
    }

    // --- Equal frequencies ---------------------------------------------------
    {
        Huffman h;
        std::string text = "abcd";  // every frequency = 1
        h.build(text);
        const auto& c = h.codes();
        assert(c.size() == 4);
        // With equal frequencies all leaves share the same depth for k = 4.
        for (const auto& [ch, code] : c) assert(code.size() == 2);
        assert(is_prefix_free(c));
        assert(h.decode(h.encode(text)) == text);
        std::cout << "equal frequencies: all codes length 2, prefix-free OK\n";
    }

    std::cout << "huffman_coding: all tests passed\n";
    return 0;
}
