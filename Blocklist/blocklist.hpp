#ifndef BOOKSTORE_2023_BLOCKLIST_HPP
#define BOOKSTORE_2023_BLOCKLIST_HPP

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <list>
#include "../utils.hpp"


using std::fstream;

namespace CrazyDave {
    class File {
        char name[65]{'\0'};
        fstream fs;
        bool is_new = false;
    public:
        explicit File(const char *_name) {
            strcpy(name, _name);
            fs.open(name, std::ios::in);
            if (!fs) {
                fs.open(name, std::ios::out);
                is_new = true;
            }
            fs.close();
        }

        void open(std::ios::openmode mode = std::ios::in | std::ios::out) {
            fs.open(name, mode);
        }

        void close() {
            fs.close();
        }

        bool get_is_new() const {
            return is_new;
        }

        template<class T>
        void read(T &dst, size_t size = sizeof(T)) {
            fs.read(reinterpret_cast<char *>(&dst), (long) size);
        }

        template<class T>
        void write(T &src, size_t size = sizeof(T)) {
            fs.write(reinterpret_cast<const char *>(&src), (long) size);
        }

        void seekg(int pos) {
            fs.seekg(pos);
        }

        void seekp(int pos) {
            fs.seekp(pos);
        }
    };

    template<class key_t=CrazyDave::String<65>, class value_t= int>
    class BlockList {
        struct pair {
            key_t key{};
            value_t val{};

            bool operator<(const pair &rhs) const {
                if (key != rhs.key)return key < rhs.key;
                return val < rhs.val;
            }

            friend bool operator<(const pair &lhs, const key_t &rhs_key) {
                return lhs.key < rhs_key;
            }

            friend bool operator<(const key_t &lhs_key, const pair rhs) {
                return lhs_key < rhs.key;
            }
        };

        struct node {
            int pos{}, size{};
            pair max;
        };
        File list_file;
        File data_file;
        std::list<node> list;
        std::vector<node> list_tmp;
        std::vector<int> memory;
        std::vector<bool> allocated;
        int max_pos{};
        int total{};
        const size_t SIZE_OF_NODE = sizeof(node);
        static const int MAX_SIZE = 200;
//        const int MIN_SIZE = 200;
        const size_t SIZE_OF_PAIR = sizeof(pair);

        using iterator = typename std::list<node>::iterator;
        using block = pair[MAX_SIZE];

        block block_cache1;
        block block_cahce2;

        const size_t SIZE_OF_BLOCK = sizeof(block);


        int alloc_node() {
            // 返回可用节点位置
            if (memory.empty()) {
                return max_pos = list.size();
            } else {
                int res = memory.back();
                memory.pop_back();
                return res;
            }
        }

        void write_node(iterator itr, const block &bk) {
            data_file.seekp(itr->pos * SIZE_OF_BLOCK);
            data_file.write(bk, itr->size * SIZE_OF_PAIR);
        }

        void read_node(iterator itr, block &bk) {
            data_file.seekg(itr->pos * SIZE_OF_BLOCK);
            data_file.read(bk, itr->size * SIZE_OF_PAIR);
        }

        void insert(iterator itr, pair &pr) {
            block &bk = block_cache1;
            int size = itr->size;
            read_node(itr, bk);
            int l = std::lower_bound(bk, bk + size, pr) - bk;
            if (l == size) {
                itr->max = bk[l] = pr;
            } else {
                for (int i = size; i > l; --i) {
                    bk[i] = bk[i - 1];
                }
                bk[l] = pr;
            }
            itr->size = ++size;
            ++total;
            if (itr->size < MAX_SIZE - 1) {
                write_node(itr, bk);
            } else {
                // 裂块
                split(itr, bk);
            }
        }

        void remove(iterator itr, pair &pr) {
            int size = itr->size;
            if (size == 1) {
                if (!cmp(itr->max, pr)) {
                    destroy(itr);
                    --total;
                }
                return;
            }
            block &bk = block_cache1;
            read_node(itr, bk);
            int l = binary_search(bk, itr->size, pr);

            if (l < 0)return;
            else if (l == size - 1) {
                itr->max = bk[l - 1];
            } else {
                for (int i = l; i < size - 1; ++i) {
                    bk[i] = bk[i + 1];
                }
            }
            itr->size = --size;
            --total;
            write_node(itr, bk);
        }

        void destroy(iterator itr) {
            memory.push_back(itr->pos);
            list.erase(itr);
        }

        void split(iterator itr, block &cur) {
            block &next = block_cahce2;
            const int size1 = itr->size >> 1;
            int size2 = itr->size - size1;
            for (int i = size1; i < itr->size; ++i) {
                next[i - size1] = cur[i];
            }

            itr = list.insert(++itr, node{alloc_node(), size2, itr->max});
            write_node(itr, next);

            --itr;
            itr->size = size1;
            itr->max = cur[size1 - 1];
            write_node(itr, cur);
        }

    public:
        BlockList(const char *str1, const char *str2) : list_file(str1), data_file(str2) {
            list_file.open();
            data_file.open();
            if (!list_file.get_is_new()) {
                initialise();
            }
        }

        void initialise() {

            int size; // 链表节点个数
            list_file.seekg(0);
            list_file.read(size);
            list_file.read(max_pos);
            list_file.read(total);
            list_tmp.resize(size);
            list_file.read(list_tmp.front(), size * SIZE_OF_NODE);

            allocated.assign(max_pos + 1, false);
            for (auto &x: list_tmp) {
                list.push_back(x);
                allocated[x.pos] = true;
            }

            memory.clear();
            for (int i = 0; i < max_pos; ++i) {
                if (!allocated[i]) {
                    memory.push_back(i);
                }
            }
        }

        ~BlockList() {
            int size = list.size();
            list_tmp.resize(size);
            list_tmp.assign(list.begin(), list.end());
            list_file.seekp(0);
            list_file.write(size);
            list_file.write(max_pos);
            list_file.write(total);
            list_file.write(list_tmp.front(), size * SIZE_OF_NODE);
            list_file.close();
            data_file.close();
        }

        bool get_is_new() {
            return list_file.get_is_new();
        }

        int size() {
            return total;
        }

        void insert(const key_t &key, const value_t &val) {
            pair pr{key, val};
            if (list.empty()) {
                list.push_back(node{alloc_node(), 1, pr});
                block &bk = block_cache1;
                bk[0] = pr;
                write_node(list.begin(), bk);
                ++total;
                return;
            }
            auto itr = list.begin();
            for (; itr != list.end(); ++itr) {
                if (cmp(itr->max, pr) >= 0) {
                    insert(itr, pr);
                    return;
                }
            }
            insert(--itr, pr);
        }

        void remove(const key_t &key, const value_t &val) {
            pair pr{key, val};
            for (auto itr = list.begin(); itr != list.end(); ++itr) {
                if (cmp(itr->max, pr) >= 0) {
                    remove(itr, pr);
                    return;
                }
            }
        }

        std::vector<value_t> find(const key_t &key) {
            std::vector<value_t> res;
            block &bk = block_cache1;
            for (auto itr = list.begin(); itr != list.end(); ++itr) {
                int flag = cmp(itr->max.key, key);
                if (flag >= 0) {
                    read_node(itr, bk);
                    int l = std::lower_bound(bk, bk + itr->size, key) - bk;
                    int r = std::upper_bound(bk, bk + itr->size, key) - bk;
                    for (int i = l; i < r; ++i) {
                        res.push_back(bk[i].val);
                    }
                    if (flag > 1) {
                        return std::move(res);
                    }
                }
            }
            return std::move(res);
        }

        std::vector<value_t> find_all() {
            std::vector<value_t> res;
            block &bk = block_cache1;
            for (auto itr = list.begin(); itr != list.end(); ++itr) {
                read_node(itr, bk);
                for (int i = 0; i < itr->size; ++i) {
                    res.push_back(bk[i].val);
                }
            }
            return std::move(res);
        }
    };
}


#endif //BOOKSTORE_2023_BLOCKLIST_HPP