// separate chaining hash table, FNV-1a hashing
// doubles the buckets when the load factor goes over 0.75

#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// 64-bit FNV-1a, handles string and int keys
struct Fnv1aHash {
    static constexpr std::uint64_t kOffsetBasis = 14695981039346656037ULL;
    static constexpr std::uint64_t kPrime = 1099511628211ULL;

    std::size_t operator()(const std::string& s) const noexcept {
        std::uint64_t h = kOffsetBasis;
        for (unsigned char c : s) {
            h ^= static_cast<std::uint64_t>(c);
            h *= kPrime;
        }
        return static_cast<std::size_t>(h);
    }

    template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
    std::size_t operator()(T value) const noexcept {
        // widen to 64 bits first, then hash those bytes
        std::uint64_t bits = 0;
        if constexpr (std::is_signed_v<T>) {
            bits = static_cast<std::uint64_t>(static_cast<std::int64_t>(value));
        } else {
            bits = static_cast<std::uint64_t>(value);
        }
        std::uint64_t h = kOffsetBasis;
        for (int i = 0; i < 8; ++i) {
            h ^= (bits >> (8 * i)) & 0xFFULL;
            h *= kPrime;
        }
        return static_cast<std::size_t>(h);
    }
};

template <typename K, typename V>
class HashTable {
public:
    using hasher = Fnv1aHash;

    explicit HashTable(std::size_t bucket_count = 8)
        : buckets_(bucket_count == 0 ? 1 : bucket_count) {}

    // returns false if the key was already there, value gets replaced
    bool insert(const K& key, const V& value) {
        if (contains(key)) {
            Entry& entry = find_entry(key);
            entry.value = value;
            return false;
        }
        buckets_[index_of(key)].push_back(Entry{key, value});
        ++size_;
        if (load_factor() > kMaxLoadFactor) {
            rehash(buckets_.size() * 2);
        }
        return true;
    }

    std::optional<V> get(const K& key) const {
        const Entry* entry = find_entry(key);
        if (entry == nullptr) {
            return std::nullopt;
        }
        return entry->value;
    }

    bool contains(const K& key) const { return find_entry(key) != nullptr; }

    // missing keys get default-constructed, like std::map
    V& operator[](const K& key) {
        if (Entry* entry = find_entry_mut(key)) {
            return entry->value;
        }
        buckets_[index_of(key)].push_back(Entry{key, V{}});
        ++size_;
        if (load_factor() > kMaxLoadFactor) {
            rehash(buckets_.size() * 2);
        }
        // rehash above may have moved it, look again
        return find_entry_mut(key)->value;
    }

    // false if the key wasn't there
    bool erase(const K& key) {
        auto& chain = buckets_[index_of(key)];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->key == key) {
                chain.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    std::size_t bucket_count() const { return buckets_.size(); }
    double load_factor() const {
        return static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

    // exposed so the collision test can find keys that land in one bucket
    std::size_t bucket_index(const K& key) const { return index_of(key); }

private:
    struct Entry {
        K key;
        V value;
    };

    // rehash kicks in once the load factor passes this
    static constexpr double kMaxLoadFactor = 0.75;

    std::size_t index_of(const K& key) const {
        return hasher{}(key) % buckets_.size();
    }

    const Entry* find_entry(const K& key) const {
        for (const Entry& e : buckets_[index_of(key)]) {
            if (e.key == key) {
                return &e;
            }
        }
        return nullptr;
    }

    Entry* find_entry_mut(const K& key) {
        for (Entry& e : buckets_[index_of(key)]) {
            if (e.key == key) {
                return &e;
            }
        }
        return nullptr;
    }

    Entry& find_entry(const K& key) {  // non-const overload used by insert
        for (Entry& e : buckets_[index_of(key)]) {
            if (e.key == key) {
                return e;
            }
        }
        assert(false && "insert() only calls this after contains() == true");
        return buckets_[0].front();  // unreachable; silences compiler
    }

    // spread everything out into a bigger bucket array
    void rehash(std::size_t new_bucket_count) {
        if (new_bucket_count < 1) {
            new_bucket_count = 1;
        }
        std::vector<std::vector<Entry>> fresh(new_bucket_count);
        for (auto& chain : buckets_) {
            for (Entry& e : chain) {
                const std::size_t idx = hasher{}(e.key) % new_bucket_count;
                fresh[idx].push_back(std::move(e));
            }
        }
        buckets_.swap(fresh);
    }

    std::vector<std::vector<Entry>> buckets_;
    std::size_t size_ = 0;
};

static void run_tests() {
    // basic insert / lookup / erase
    HashTable<std::string, int> t;
    assert(t.empty());
    assert(t.size() == 0);
    assert(!t.contains("alpha"));
    assert(t.get("alpha") == std::nullopt);  // missing key

    assert(t.insert("alpha", 1));
    assert(t.insert("beta", 2));
    assert(t.insert("gamma", 3));
    assert(t.size() == 3);
    assert(t.contains("beta"));
    assert(t.get("alpha") == 1);
    assert(t.get("beta") == 2);
    assert(t.get("gamma") == 3);
    assert(t.get("delta") == std::nullopt);  // missing key
    assert(!t.contains("delta"));

    // overwriting keeps the size the same
    assert(!t.insert("beta", 20));
    assert(t.size() == 3);
    assert(t.get("beta") == 20);

    // operator[] on a missing key inserts a default
    assert(t["gamma"] == 3);
    t["gamma"] = 30;
    assert(t.get("gamma") == 30);
    t["newcomer"] = 99;
    assert(t.size() == 4);
    assert(t.get("newcomer") == 99);
    t["defaults"];  // int defaults to 0
    assert(t.get("defaults") == 0);
    assert(t.size() == 5);

    // erased key is gone
    assert(t.erase("beta"));
    assert(!t.contains("beta"));
    assert(t.get("beta") == std::nullopt);
    assert(t.size() == 4);
    assert(!t.erase("beta"));  // second erase fails
    assert(t.size() == 4);

    // int keys, negatives and zero
    HashTable<int, std::string> nums;
    assert(nums.insert(0, "zero"));
    assert(nums.insert(-7, "minus seven"));
    assert(nums.insert(42, "answer"));
    assert(nums.get(0) == "zero");
    assert(nums.get(-7) == "minus seven");
    assert(nums.get(42) == "answer");
    assert(nums.get(1) == std::nullopt);
    assert(nums.erase(-7));
    assert(nums.get(-7) == std::nullopt);
    assert(nums.size() == 2);

    // force 5 keys into bucket 0; load stays under 0.75 so no rehash
    HashTable<int, int> collision(8);
    assert(collision.bucket_count() == 8);
    std::vector<int> colliding;
    for (int candidate = 0;
         colliding.size() < 5 && candidate < 1000000; ++candidate) {
        if ((Fnv1aHash{}(candidate) % 8) == 0) {
            colliding.push_back(candidate);
        }
    }
    assert(colliding.size() == 5);
    for (int key : colliding) {
        assert(collision.bucket_index(key) == 0);  // all in one chain
        assert(collision.insert(key, key * key));
    }
    for (int key : colliding) {
        assert(collision.contains(key));
        assert(collision.get(key) == key * key);
    }
    assert(collision.size() == 5);
    assert(collision.bucket_count() == 8);  // still no rehash
    // the rest of the chain still works
    assert(collision.erase(colliding[2]));
    for (std::size_t i = 0; i < colliding.size(); ++i) {
        if (i == 2) {
            assert(!collision.contains(colliding[i]));
        } else {
            assert(collision.get(colliding[i]) == colliding[i] * colliding[i]);
        }
    }

    // rehash keeps every entry
    HashTable<int, int> big(4);
    const int n = 500;
    for (int i = 0; i < n; ++i) {
        big.insert(i, i * 3 + 1);
        assert(big.load_factor() <= 0.75);  // load stays under 0.75
    }
    assert(big.size() == static_cast<std::size_t>(n));
    assert(big.bucket_count() > 4);  // rehashing must have happened
    for (int i = 0; i < n; ++i) {
        assert(big.get(i) == i * 3 + 1);
    }
    assert(big.get(n) == std::nullopt);
    assert(big.erase(250));
    assert(big.get(250) == std::nullopt);
    assert(big.size() == static_cast<std::size_t>(n) - 1);
    for (int i = 0; i < n; ++i) {
        if (i == 250) {
            continue;
        }
        assert(big.get(i) == i * 3 + 1);
    }

    // string keys, lots of inserts
    HashTable<std::string, int> words(2);
    for (int i = 0; i < 200; ++i) {
        words.insert("key_" + std::to_string(i), i);
        assert(words.load_factor() <= 0.75);
    }
    assert(words.size() == 200);
    for (int i = 0; i < 200; ++i) {
        assert(words.get("key_" + std::to_string(i)) == i);
    }
    assert(words.get("key_200") == std::nullopt);
}

int main() {
    run_tests();
    std::cout << "All hash table tests passed.\n\n";

    // demo
    HashTable<std::string, int> ages;
    ages.insert("Ada", 36);
    ages.insert("Grace", 45);
    ages.insert("Alan", 41);
    ages.insert("Ada", 37);  // overwrite

    std::cout << "size=" << ages.size()
              << "  buckets=" << ages.bucket_count()
              << "  load_factor=" << ages.load_factor() << '\n';
    for (const char* name : {"Ada", "Grace", "Alan", "Edsger"}) {
        std::cout << name << " -> ";
        if (auto v = ages.get(name)) {
            std::cout << *v << '\n';
        } else {
            std::cout << "(absent)\n";
        }
    }

    ages.erase("Grace");
    std::cout << "after erase, Grace -> "
              << (ages.get("Grace") ? "present" : "absent") << '\n';
    std::cout << "final size=" << ages.size()
              << "  load_factor=" << ages.load_factor() << '\n';
    return 0;
}
