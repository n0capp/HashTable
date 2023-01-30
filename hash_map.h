#pragma once

#include <functional>
#include <memory>

template<typename KeyType, typename ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    struct HashMapEntry {
        std::pair<const KeyType, ValueType> p;
        size_t PSL;

        const KeyType &key() const {
            return p.first;
        }

        ValueType &value() {
            return p.second;
        }

        explicit HashMapEntry(std::pair<const KeyType, ValueType> p, int PSL) : p(p), PSL(PSL) {}

        explicit HashMapEntry(const KeyType key, ValueType value) : p(key, value), PSL(0) {}

        explicit HashMapEntry(std::pair<const KeyType, ValueType> p) : p(p), PSL(0) {}
    };

    using Pointer = std::unique_ptr<HashMapEntry>;
    const size_t SIZE = 1;
    const double MAX_LOAD = 0.6;
    Pointer *buckets;
    std::vector<bool> erased;
    Hash hash;
    size_t size_ = 0;
    size_t cap_ = 1;


    void increaseSize() {
        std::vector<Pointer> elements;
        for (size_t i = 0; i < cap_; ++i) {
            if (buckets[i]) {
                elements.push_back(nullptr);
                swap(elements.back(), buckets[i]);
            }
        }
        delete[] buckets;
        buckets = new Pointer[cap_ * 2];
        erased.assign(cap_ * 2, false);
        for (size_t i = 0; i < cap_ * 2; ++i) {
            buckets[i] = nullptr;
        }
        cap_ *= 2, size_ = 0;
        for (auto &el: elements) {
            insertByPointer(el);
        }
    }


    double getLoadFactor() {
        return (double) size_ / cap_;
    }

    void fit() {
        if (getLoadFactor() > MAX_LOAD) {
            increaseSize();
        }
    }

    void insertByPointer(Pointer &element) {
        element->PSL = 0;
        size_t i;
        for (i = hash(element->key()) % cap_; buckets[i]; i = (i + 1) % cap_) {
            if (buckets[i]->PSL < element->PSL) {
                swap(buckets[i], element);
            }
            ++element->PSL;
        }
        ++size_;
        erased[i] = false;
        swap(buckets[i], element);
        fit();
    }

    size_t getIndex(const KeyType &key) const {
        size_t moved = 0;
        for (size_t i = hash(key) % cap_; moved < cap_; i = (i + 1) % cap_) {
            if ((buckets[i] == nullptr && !erased[i]) ||
                (buckets[i] != nullptr && buckets[i]->key() == key)) {
                return i;
            }
            ++moved;
        }
        return cap_;
    }

public:
    class iterator {
        HashMap<KeyType, ValueType, Hash> *map = nullptr;
        size_t index = 0;


        void jump() {
            for (; index < map->cap_ && !map->buckets[index]; ++index) {}
        }

    public:
        iterator() = default;

        explicit iterator(HashMap *mp, size_t id) {
            map = mp;
            index = id;
            jump();
        }

        explicit iterator(HashMap *mp) {
            map = mp;
            jump();
        }


        iterator &operator++() {
            ++index;
            jump();
            return *this;
        }

        const iterator operator++(int) {
            auto ret = *this;
            ++index;
            jump();
            return ret;
        }

        std::pair<const KeyType, ValueType> *operator->() {
            return &map->buckets[index]->p;
        }

        std::pair<const KeyType, ValueType> &operator*() {
            return map->buckets[index]->p;
        }

        bool operator==(const iterator &other) const {
            return map == other.map && index == other.index;
        }

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }
    };

    class const_iterator {
    private:
        const HashMap<KeyType, ValueType, Hash> *map = nullptr;
        size_t index = 0;

        void jump() {
            for (; index < map->cap_ && !map->buckets[index]; ++index) {}
        }

    public:
        const_iterator() = default;

        explicit const_iterator(const HashMap *mp) {
            map = mp;
            jump();
        }

        explicit const_iterator(const HashMap *mp, size_t id) {
            map = mp;
            index = id;
            jump();
        }


        const_iterator &operator++() {
            ++index;
            jump();
            return *this;
        }

        const const_iterator operator++(int) {
            auto ret = *this;
            ++index;
            jump();
            return ret;
        }


        const std::pair<const KeyType, ValueType> &operator*() const {
            return map->buckets[index]->p;
        }

        const std::pair<const KeyType, ValueType> *operator->() const {
            return &map->buckets[index]->p;
        }

        bool operator==(const const_iterator &other) const {
            return map == other.map && index == other.index;
        }

        bool operator!=(const const_iterator &other) const {
            return !(*this == other);
        }
    };

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> list, Hash hash = Hash()) : hash(hash) {
        buckets = new Pointer[SIZE];
        erased.assign(SIZE, false);
        cap_ = SIZE;
        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = nullptr;
        }
        size_ = 0;
        for (const auto [k, v]: list) {
            insert({k, v});
        }
    }

    explicit HashMap(Hash hash = Hash()) : hash(hash) {
        buckets = new Pointer[SIZE];
        erased.assign(SIZE, false);
        cap_ = SIZE;
        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = nullptr;
        }
        size_ = 0;
    }

    template<class I>
    HashMap(I begin, I end, Hash hash = Hash()) : hash(hash) {
        buckets = new Pointer[SIZE];
        erased.assign(SIZE, false);
        cap_ = SIZE;

        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = nullptr;
        }
        size_ = 0;
        for (auto i = begin; i != end; ++i) {
            insert(*i);
        }
    }


    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    bool count(const KeyType &key) const {
        return find(key) != end();
    }

    void insert(const std::pair<KeyType, ValueType> &p) {
        if (count(p.first)) {
            return;
        }
        Pointer element = std::make_unique<HashMapEntry>(p);
        insertByPointer(element);
    }


    void erase(const KeyType &key) {
        size_t index = getIndex(key);
        if (index != cap_ && buckets[index] != nullptr && buckets[index]->key() == key) {
            --size_;
            erased[index] = true;
            buckets[index].reset();
            buckets[index] = nullptr;
        }
    }

    void clear() {
        size_ = 0;
        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = nullptr;
            erased[i] = false;
        }
    }


    iterator find(const KeyType &key) {
        size_t index = getIndex(key);
        size_t it = cap_;
        if (index != cap_ && buckets[index] != nullptr && buckets[index]->key() == key) {
            it = index;
        }
        return iterator(this, it);
    }

    const_iterator find(const KeyType &key) const {
        size_t index = getIndex(key);
        size_t it = cap_;
        if (index != cap_ && buckets[index] != nullptr && buckets[index]->key() == key) {
            it = index;
        }
        return const_iterator(this, it);
    }

    Hash hash_function() const {
        return hash;
    }

    ValueType &operator[](const KeyType &key) {
        if (!count(key)) {
            this->insert({key, ValueType()});
        }
        size_t index = getIndex(key);
        return buckets[index]->value();
    }

    const ValueType &at(const KeyType &key) const {
        if (!count(key)) {
            throw std::out_of_range("No such element in the map.");
        }
        int index = getIndex(key);
        return buckets[index]->value();
    }

    iterator begin() {
        return iterator(this, 0);
    }

    iterator end() {
        return iterator(this, cap_);
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, cap_);
    }

    explicit HashMap(const HashMap<KeyType, ValueType> &mp) {
        size_ = mp.size();
        cap_ = mp.cap_;
        hash = mp.hash;
        buckets = new Pointer[cap_];
        erased.assign(cap_, false);
        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = (mp.buckets[i] == nullptr ? nullptr : std::make_unique<HashMapEntry>
                    (mp.buckets[i]->p, mp.buckets[i]->PSL));
            erased[i] = mp.erased[i];
        }
    }

    explicit HashMap(const HashMap<KeyType, ValueType> &&mp) {
        size_ = std::move(mp.size());
        cap_ = std::move(mp.cap_);
        hash = std::move(mp.hash);
        buckets = std::move(mp.buckets);
        erased = std::move(mp.erased);
    }

    HashMap &operator=(const HashMap<KeyType, ValueType> &mp) {
        if (buckets == mp.buckets) {
            return *this;
        }
        size_ = mp.size();
        cap_ = mp.cap_;
        hash = mp.hash;
        delete[] buckets;
        buckets = new Pointer[cap_];
        erased.assign(cap_, false);
        for (size_t i = 0; i < cap_; ++i) {
            buckets[i] = (mp.buckets[i] == nullptr ? nullptr : std::make_unique<HashMapEntry>
                    (mp.buckets[i]->p, mp.buckets[i]->PSL));
            erased[i] = mp.erased[i];
        }
        return *this;
    }

    HashMap &operator=(const HashMap<KeyType, ValueType> &&mp) {
        size_ = std::move(mp.size());
        cap_ = std::move(mp.cap_);
        hash = std::move(mp.hash);
        buckets = std::move(mp.buckets);
        erased = std::move(mp.erased);
        return *this;
    }

    ~HashMap() {
        delete[] buckets;
    }


};
