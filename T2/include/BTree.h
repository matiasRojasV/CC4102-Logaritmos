#pragma once
#include "BTreeNode.h"
#include <fstream>
#include <string>
#include <vector>

class BTree {
public:
    BTree();
    void insert(Pair value);
    Results rangeSearch(std::int32_t lower, std::int32_t upper, const std::string& filename);
    void writeToDisk(const std::string& filename) const;
    
    std::uint64_t insertionReads() const { return counters_.reads; }
    std::uint64_t insertionWrites() const { return counters_.writes; }
    std::uint64_t searchReads() const { return searchReads_; }
    std::size_t size() const { return nodes_.size(); }
    void resetCounters() { counters_.reset(); searchReads_ = 0; }

private:
    std::vector<BTreeNode> nodes_;
    Counters counters_;
    mutable std::uint64_t searchReads_ = 0;

    int append(const BTreeNode& node);
    void splitChild(int parent, int position);
    void insertNonFull(int node, Pair value);
    void searchNode(std::ifstream& file, int node, std::int32_t lower,
                    std::int32_t upper, Results& result) const;
};