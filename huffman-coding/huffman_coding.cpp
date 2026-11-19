// huffman encode/decode built on a priority queue

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
    // build the code table from the text's frequencies
    void build(const std::string& text) {
        codes_.clear();
        std::map<char, int> freq;
        for (char c : text) ++freq[c];

        struct Node {
            char ch;          // '\0' for internal nodes
            int freq;
            Node* left;
            Node* right;
            Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
        };

        // ties broken with std::less, < on pointers is undefined
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

        if (pq.empty()) return;

        // one distinct char, just use code "0"
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

        std::function<void(Node*, std::string)> assign = [&](Node* n, std::string prefix) {
            if (n->left == nullptr && n->right == nullptr) {
                codes_[n->ch] = prefix;
                return;
            }
            assign(n->left, prefix + '0');
            assign(n->right, prefix + '1');
        };
        assign(root, "");

        // tree not needed anymore
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

    std::string decode(const std::string& bits) const {
        if (codes_.empty()) return "";
        if (codes_.size() == 1) {
            // every symbol is "0"
            for (char b : bits) assert(b == '0');
            return std::string(bits.size(), codes_.begin()->first);
        }
        // walk the reverse table, tree is already freed
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
    // round trip on a sample text
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

    // frequent chars get short codes
    {
        // a=5 b=3 c=1, a should get the shortest code
        Huffman h;
        std::string text = "aaaaabbbc";
        h.build(text);
        const auto& c = h.codes();
        assert(c.size() == 3);
        // 'a' merges last so it stays shallow
        assert(c.at('a').size() == 1);
        assert(c.at('b').size() == 2 && c.at('c').size() == 2);
        assert(c.at('b').size() <= c.at('c').size());
        assert(c.at('c').size() <= c.at('a').size() + 1);
        assert(is_prefix_free(c));
        assert(h.decode(h.encode(text)) == text);
    }

    // round trips over several inputs
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

    // single distinct char
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

    // equal frequencies
    {
        Huffman h;
        std::string text = "abcd";  // every frequency = 1
        h.build(text);
        const auto& c = h.codes();
        assert(c.size() == 4);
        // all leaves end up at the same depth
        for (const auto& [ch, code] : c) assert(code.size() == 2);
        assert(is_prefix_free(c));
        assert(h.decode(h.encode(text)) == text);
        std::cout << "equal frequencies: all codes length 2, prefix-free OK\n";
    }

    std::cout << "huffman_coding: all tests passed\n";
    return 0;
}
