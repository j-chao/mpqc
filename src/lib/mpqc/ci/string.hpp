#ifndef MPQC_CI_STRING_HPP
#define MPQC_CI_STRING_HPP

#include "mpqc/range.hpp"

#include <bitset>
#include <iostream>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/tuple/tuple.hpp>

#ifdef HAVE_CMPH
#include <cmph.h>
#endif

namespace mpqc {
namespace ci {

    inline int index(int i, int j) {
        if (i > j)
            std::swap(i, j);
        return (i + (j * j + j) / 2);
    }

    struct String {
        template<class > class List;
        class Index;
        class CMPH;
        typedef std::bitset<64> bitset;

        struct orbitals {
            typedef const int* const_iterator;
            const_iterator begin() const {
                return data_;
            }
            const_iterator end() const {
                return data_ + count_;
            }
        private:
            friend class String;
            explicit orbitals(int count) :
                count_(count) {
            }
            int data_[64];
            int count_;
        };

        /// Construct string with 'count' electrons,
        /// eg String(4,2) will generate a string '0011'.
        /// @param size number of orbitals
        /// @param count number of electrons
        String(size_t size, size_t count) {
            MPQC_ASSERT(0 < size && size < 64);
            MPQC_ASSERT(0 < count && count < size);
            this->size_ = size;
            this->count_ = count;
            this->value_ = bitset((1 << count) - 1);
        }

        /// Construct String from a char string.
        /// @warning string is in "natural", NOT bit, order,
        /// i.e. string[0] bit corresponds to String[0] bit
        explicit String(std::string value) {
            //std::cout << "from str " <<  value << std::endl;
            MPQC_ASSERT(value.size() <= 64);
            std::reverse(value.begin(), value.end());
            this->size_ = value.size();
            this->value_ = bitset(value);
            this->count_ = bitset(value).count();
        }

        String(const String &s) {
            this->size_ = s.size_;
            this->count_ = s.count_;
            this->value_ = s.value_;
        }

        const void* data() const {
            return (const char*) &this->value_;
        }

        bool operator[](size_t i) const {
            return this->value_[i];
        }

        size_t size() const {
            return this->size_;
        }

        size_t count() const {
            return this->count_;
        }

        orbitals occ() const {
            orbitals o(this->count());
            for (size_t i = 0, j = 0; i < this->count(); ++i) {
                while (!this->operator[](j))
                    ++j;
                o.data_[i] = j++;
            }
            return o;
        }
        String swap(size_t i, size_t j) const {
            String s = *this;
            //MPQC_ASSERT(s[i] != s[j]);
            s.flip(i);
            s.flip(j);
            //std::swap(s.value_[i], s.value_[j]);
            return s;
        }
        /// return string in bit *representation* order, lowest bit is right-most
        operator std::string() const {
            std::string s(this->size_, '*');
            for (size_t i = 0; i < this->size_; ++i) {
                s[i] = ("01"[this->operator[](i)]);
            }
            return s;
            //return this->value_.to_string<char, std::char_traits<char>, std::allocator<char>>();
        }
        size_t to_ulong() const {
            return this->value_.to_ulong();
        }
        // size_t operator<(const String &b) const {
        //     return (this->to_ulong() > b.to_ulong());
        // }
        size_t operator^(const String &b) const {
            return (b.to_ulong() ^ this->to_ulong());
        }
        size_t operator&(size_t b) const {
            return (b & this->to_ulong());
        }
        static size_t difference(const String &a, const String &b) {
            MPQC_ASSERT(a.size() == b.size());
            MPQC_ASSERT(a.count() == b.count());
            size_t n = String::count(a ^ b);
            //std::cout << std::string(a) << " ^ " << std::string(b) << std::endl;
            MPQC_ASSERT(!(n % 2));
            return n / 2;
        }
        static size_t count(size_t b) {
            return bitset(b).count();
        }
    private:
        bitset value_;
        size_t size_, count_;
        void flip(size_t i) {
            this->value_.flip(i);
        }
    };

    inline std::ostream& operator<<(std::ostream& os, const String &s) {
        os << std::string(s);
        return os;
    }


    /// Computes parity, 1 for even parity, -1 for odd
    inline int sgn(size_t ij) {
        return (ij % 2) ? -1 : 1;
    }


    /// Compute parity of exciting i to j (or j to i).
    /// Counts number of set bits in open interval (i:j)
    /// and returns parity of the pop count.
    inline int sgn(const String &I, int i, int j) {
        uint64_t b = I.to_ulong();
        int n = std::abs(i - j);
        if (j < i) std::swap(i, j);
        b = b & ((uint64_t(1) << j) - 1); // clear high bits
        b = b >> i;  b = b >> 1; // clear low bits by shifting
        size_t p = String::bitset(b).count();
        //size_t p = _mm_popcnt_u64(b);
#if 0
        MPQC_ASSERT(p < I.count());
        size_t q = 0;
        for (int k = std::min(i,j)+1; k < std::max(i,j); ++k) {
            q += I[k];
        }
        if (p != q)
          sc::ExEnv::err0() << sc::scprintf("string %s(%lu) [%i,%i] b=%lu, p=%lu, q=%lu\n",
                                    std::string(I).c_str(), I.to_ulong(), i,j, b, p, q);
        MPQC_ASSERT(p == q);
#endif
        return sgn(p);
    }


    /**
     * String::List represents a set of String objects.
     * The objects are stored as a sequence that can be accessed randomly.
     * String::List also provides a String->index map.
     * @tparam T the type that implements String->index map.
     */
    template<class IndexType>
    struct String::List {
        typedef typename std::vector<String>::iterator iterator;
        typedef typename std::vector<String>::const_iterator const_iterator;
        List(const std::vector<String> &v) {
            data_ = v;
            index_.initialize(data_);
            for (int i = 0; i < v.size(); ++i) {
                perm_.push_back(i);
            }
        }
        size_t size() const {
            return data_.size();
        }
        const_iterator begin() const {
            return data_.begin();
        }
        const_iterator end() const {
            return data_.end();
        }
        iterator begin() {
            return data_.begin();
        }
        iterator end() {
            return data_.end();
        }
        size_t operator[](const String &s) const {
            return perm_[this->index_[s]];
        }
        const String& operator[](size_t i) const {
            return data_[i];
        }
        const String& at(size_t i) const {
            return data_.at(i);
        }
        operator mpqc::range() const {
            return mpqc::range(0, this->size());
        }
        template<class F>
        void sort(F f) {
            std::stable_sort(data_.begin(), data_.end(), f);
            // update permutation for string-to-index look up
            for (int i = 0; i < data_.size(); ++i) {
                perm_[this->index_[data_[i]]] = i;
            }
        }
        const std::vector<String>& data() const { return data_; }
    private:
        std::vector<String> data_;
        std::vector<int> perm_;
        IndexType index_;
        template<class F>
        struct ZipSort {
            F f;
            explicit ZipSort(F f) : f(f) {}
            template <typename Tuple>
            bool operator()(const Tuple &a, const Tuple &b) const {
                return f(boost::get<0>(a), boost::get<0>(b));
            }
        };
    };

#ifdef HAVE_CMPH
    /**
     * implements a sparse String->index map
     */
    struct String::SparseIndex {
        void intitialize(const std::vector<String> &v) {
            cmph_io_adapter_t *source =
                cmph_io_struct_vector_adapter((void*)&v[0], sizeof(v[0]),
                                              0, v[0].bytes(), v.size());
            cmph_config_t *config = cmph_config_new(source);
            cmph_config_set_algo(config, CMPH_BDZ);
            cmph_t *hash = cmph_new(config);
            std::cout << cmph_packed_size(hash) << std::endl;
            this->data_.resize(cmph_packed_size(hash));
            cmph_pack(hash, &this->data_[0]);
            cmph_destroy(hash);
        }
        size_t operator[](const String &s) const {
            return cmph_search_packed((void*)&this->data_[0],
                                      s.data(), s.bytes());
        }
    private:
        std::vector<char> data_;
    };
#endif

    /**
     * implements a dense String->index map, appropriate for a full CI string sets
     */
    struct String::Index {
        static const size_t K = 2;
        // initialize hash
        void initialize(const std::vector<String> &v) {
            String s = v.front();
            size_t size[] = { s.count(), s.size() };
            for (size_t k = 0, i = 0; k < K; ++k) {
                shift_[k] = i;
                mask_[k] = 0;
                while (i < size[k]) {
                    mask_[k] = mask_[k] | (1UL << i);
                    ++i;
                }
            }
            // // (h0,h1,..) - h0 *bits* change slowest
            std::reverse(shift_, shift_ + K);
            std::reverse(mask_, mask_ + K);
            // populate hash buckets
            size_t index[K] = { size_t(-1), size_t(-1) };
            for (size_t i = 0; i < v.size(); ++i) {
                const String &s = v[i];
                for (int k = 0; k < K; ++k) {
                    if ((s & mask_[k]) != index[k]) {
                        index[k] = s & mask_[k];
                        *hash(k, s) = i;
                        for (int j = 0; j < k; ++j) {
                            *hash(k, s) -= *hash(j, s);
                        }
                    }
                }
            }
            // verify hash
            for (size_t i = 0; i < v.size(); ++i) {
                const String &s = v.at(i);
                //std::cout << "CI string " << i << " " << s << std::endl;
                if (this->operator[](s) == i)
                    continue;
                std::cout << i << " != " << this->operator[](s) << std::endl;
                MPQC_ASSERT(0);
            }
        }
        size_t operator[](const String &s) const {
            size_t index = 0;
            for (int k = 0; k < K; ++k) {
                index += *hash(k, s);
            }
            return index;
        }
    private:
        std::vector<String> data_;
        typedef size_t bucket[1 << 16];
        size_t shift_[K], mask_[K];

        struct Bucket {
            static const size_t N = 1 << 16;
            Bucket() {
                data_.resize(N * K, 0);
            }
            size_t* operator[](int k) {
                return &data_[k * N];
            }
            const size_t* operator[](int k) const {
                return &data_[k * N];
            }
        private:
            std::vector<size_t> data_;
        };

        Bucket bucket_;

        size_t* hash(size_t k, const String &s) const {
            size_t b = s & mask_[k];
            return (size_t*) bucket_[k] + (b >> shift_[k]);
        }
    };

    inline String::List<String::Index> strings(size_t no, size_t ne, size_t N = 0) {
        MPQC_ASSERT(no > ne);
        MPQC_ASSERT(ne > 0);
        std::vector<String> v;
        std::string r = String(no, ne);
        std::string s = r;
        do {
            if (N && (String::difference(String(r), String(s))) > N)
                continue;
            v.push_back(String(s));
            //std::cout << s << std::endl;
        }
        // N.B. string representation is reversed
        while (std::next_permutation(s.rbegin(), s.rend()));

        //std::reverse(v.begin(), v.end()); // un-reverse order
        return String::List<String::Index>(v);
    }

}
}

#endif // MPQC_CI_STRING_HPP
