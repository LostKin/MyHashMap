#include <iostream>
#include <list>
#include <stdexcept>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
    private:

        using Iter=typename std::list<std::pair<const KeyType, ValueType>>::iterator;
        using ConstIter=typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

        std::list<std::pair<const KeyType, ValueType>> data;
        size_t sz = 0;
        std::vector<std::vector<Iter> > buckets;
        Hash encode = Hash();
        int limit = 2;

        void reorganise() {
            std::vector<Iter> all_elems;
            for (std::vector<Iter> bucket : buckets) {
                for (Iter elem : bucket) {
                    all_elems.push_back(elem);
                }
            }
            int new_buckets_count = limit * buckets.size();
            buckets.clear();
            buckets.resize(new_buckets_count);
            for (size_t i = 0; i < all_elems.size(); i++) {
                buckets[encode(all_elems[i]->first) % new_buckets_count].push_back(all_elems[i]);
            }
        }

    public:
        typedef Iter iterator;
        typedef ConstIter const_iterator;

        HashMap() {}

        void insert(std::pair<KeyType, ValueType> next) {
            if (buckets.empty()) {
                buckets.resize(1);
            }
            std::pair<KeyType, ValueType> cur = std::pair<KeyType, ValueType>(next);
            size_t num = encode(cur.first);
            size_t pos = num % buckets.size();
            bool is_contained = false;
            for (Iter& i : buckets[pos]) {
                if (i->first == cur.first) {
                    is_contained = true;
                    break;
                }
            }
            if (is_contained) {
                return;
            }
            sz++;
            Iter it = data.insert(data.end(), cur);
            buckets[pos].push_back(it);
            if (sz >= limit * buckets.size()) {
                reorganise();
            }
        }

        template <typename Iterator> HashMap(Iterator first, Iterator last) {
            for (Iterator now = first; now != last; now++) {
                std::pair<KeyType, ValueType> cur = *now;
                insert(cur);
            }
        }

        HashMap(const std::initializer_list<std::pair<KeyType, ValueType> > &lst) {
            for (std::pair<KeyType, ValueType> elem : lst) {
                insert(elem);
            }
        }

        HashMap(HashMap& other) {
            for (Iter now = other.begin(); now != other.end(); now++) {
                insert(*now);
            }
        }

        HashMap(Hash hash_function): encode(hash_function) {}

        HashMap& operator= (HashMap other) {
            data.clear();
            buckets.clear();
            sz = 0;
            for (Iter now = other.begin(); now != other.end(); now++) {
                insert(*now);
            }
            return *this;
        }

        void erase(KeyType key) {
            if (buckets.empty()) {
                return;
            }
            size_t num = encode(key);
            size_t pos = num % buckets.size();
            bool is_contained = false;
            Iter elem = data.end();
            typename std::vector<Iter>::iterator position;
            for (size_t i = 0; i < buckets[pos].size(); i++) {
                if (buckets[pos][i]->first == key) {
                    is_contained = true;
                    position = buckets[pos].begin() + i;
                    elem = buckets[pos][i];
                    break;
                }
            }
            if (!is_contained) {
                return;
            }
            data.erase(elem);
            buckets[pos].erase(position);
            sz--;
        }

        int size() const {
            return sz;
        }

        bool empty() const {
            return sz == 0;
        }

        Hash hash_function() const {
            return encode;
        }

        ConstIter begin() const {
            return data.begin();
        }

        ConstIter end() const {
            return data.end();
        }

        Iter begin() {
            return data.begin();
        }

        Iter end() {
            return data.end();
        }

        Iter find(KeyType key) {
            if (!buckets.size()) {
                return data.end();
            }
            size_t num = encode(key);
            size_t pos = num % buckets.size();
            Iter elem = data.end();
            for (size_t i = 0; i < buckets[pos].size(); i++) {
                if (buckets[pos][i]->first == key) {
                    elem = buckets[pos][i];
                    return elem;
                }
            }
            return elem;
        }

        ConstIter find(KeyType key) const {
            if (!buckets.size()) {
                return data.end();
            }
            size_t num = encode(key);
            size_t pos = num % buckets.size();
            for (size_t i = 0; i < buckets[pos].size(); i++) {
                if (buckets[pos][i]->first == key) {
                    return buckets[pos][i];
                }
            }
            return data.end();
        }

        ValueType& operator[] (KeyType key) {
            insert(std::make_pair(key, ValueType()));
            Iter pos = find(key);
            return pos->second;
        }

        const ValueType& at(KeyType key) const {
            ConstIter pos = find(key);
            if (pos == data.end()) {
                throw std::out_of_range("no such key");
            }
            return pos->second;
        }

        void clear() {
            data.clear();
            buckets.clear();
            sz = 0;
        }
};
