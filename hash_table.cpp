// Separate-chaining hash table template HashTable<K, V>.
//
// - Hashing: 64-bit FNV-1a, provided for std::string and integral keys.
// - Buckets: vector of entry chains; linear scan within a chain for lookup.
// - Automatic rehash whenever the load factor exceeds 0.75 (buckets roughly
//   double), so average operations stay O(1) with good hashing.
//
// Average time: insert/get/contains/erase O(1); worst case O(n) (one chain).
// Space: O(n + buckets).

#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// FNV-1a hasher (64-bit) with overloads for std::string and integral keys.
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
        // Normalize to 64 bits (signed values are sign-extended, so distinct
        // integers map to distinct bit patterns), then FNV-1a those bytes.
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

    // Inserts (key, value). If the key already exists its value is replaced;
    // returns true only when a new key was added.
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

    // Copies out the value for key, or std::nullopt when the key is absent.
    std::optional<V> get(const K& key) const {
        const Entry* entry = find_entry(key);
        if (entry == nullptr) {
            return std::nullopt;
        }
        return entry->value;
    }

    bool contains(const K& key) const { return find_entry(key) != nullptr; }

    // Returns a mutable reference, default-constructing the value if the key
    // is not present yet (std::map-style semantics).
    V& operator[](const K& key) {
        if (Entry* entry = find_entry_mut(key)) {
            return entry->value;
        }
        buckets_[index_of(key)].push_back(Entry{key, V{}});
        ++size_;
        if (load_factor() > kMaxLoadFactor) {
            rehash(buckets_.size() * 2);
        }
        // Re-locate: a rehash may have moved the new entry to another chain.
        return find_entry_mut(key)->value;
    }

    // Removes key if present; returns whether anything was removed.
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

    // Bucket a key currently maps to (exposed so tests can engineer
    // collisions); recomputed from the hash, never stored.
    std::size_t bucket_index(const K& key) const { return index_of(key); }

private:
    struct Entry {
        K key;
        V value;
    };

    // Maximum load factor the table tries to maintain after every insert.
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

    // Rebuilds the bucket array with new_bucket_count chains, redistributing
    // every existing entry. Load factor is preserved or halved.
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
    // --- Basic CRUD on a fresh table -----------------------------------
    HashTable<std::string, int> t;
    assert(t.empty());
    assert(t.size() == 0);
    assert(!t.contains("alpha"));
    assert(t.get("alpha") == std::nullopt);  // missing-key lookup (empty)

    assert(t.insert("alpha", 1));
    assert(t.insert("beta", 2));
    assert(t.insert("gamma", 3));
    assert(t.size() == 3);
    assert(t.contains("beta"));
    assert(t.get("alpha") == 1);
    assert(t.get("beta") == 2);
    assert(t.get("gamma") == 3);
    assert(t.get("delta") == std::nullopt);  // missing key (non-empty table)
    assert(!t.contains("delta"));

    // Overwrite an existing key: size unchanged, value replaced.
    assert(!t.insert("beta", 20));
    assert(t.size() == 3);
    assert(t.get("beta") == 20);

    // operator[] reads, writes, and default-constructs missing keys.
    assert(t["gamma"] == 3);
    t["gamma"] = 30;
    assert(t.get("gamma") == 30);
    t["newcomer"] = 99;
    assert(t.size() == 4);
    assert(t.get("newcomer") == 99);
    t["defaults"];  // default-constructs an int value (0)
    assert(t.get("defaults") == 0);
    assert(t.size() == 5);

    // Erase then get reports absent.
    assert(t.erase("beta"));
    assert(!t.contains("beta"));
    assert(t.get("beta") == std::nullopt);
    assert(t.size() == 4);
    assert(!t.erase("beta"));  // erasing twice reports failure
    assert(t.size() == 4);

    // --- Integral keys, including negatives and zero --------------------
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

    // --- Keys engineered into a single bucket ---------------------------
    // Force 5 keys into bucket 0 of a table that starts with 8 buckets.
    // 5 entries / 8 buckets = 0.625 <= 0.75, so no rehash disturbs the test.
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
    // Erasing one key leaves the rest of the chain intact.
    assert(collision.erase(colliding[2]));
    for (std::size_t i = 0; i < colliding.size(); ++i) {
        if (i == 2) {
            assert(!collision.contains(colliding[i]));
        } else {
            assert(collision.get(colliding[i]) == colliding[i] * colliding[i]);
        }
    }

    // --- Rehash preserves every entry ----------------------------------
    HashTable<int, int> big(4);
    const int n = 500;
    for (int i = 0; i < n; ++i) {
        big.insert(i, i * 3 + 1);
        assert(big.load_factor() <= 0.75);  // invariant holds after each insert
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

    // --- String keys over many entries (rehash path) --------------------
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

    // Deterministic demo.
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
