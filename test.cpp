#include "Trie.h"
#include "provided.h"
#include <iostream>

using namespace std;


int main() {
    GenomeMatcher gm(4);
    Genome g1("hello world", "GATTACAATGTGTGTGTATTAATGAACGACATCGTAT");
    Genome g2("hmm", "CAGCTTCTCGTTAGGGGTTTCTTCCAGCAATTGCTGGATGTAGCGCTTGAC");
    gm.addGenome(g1);
    gm.addGenome(g2);
    // these genomes are not garbaged yet
    vector<DNAMatch> v;
    gm.findGenomesWithThisDNA("GATTT", 4, true, v);
    for (vector<DNAMatch>::iterator i = v.begin(); i != v.end(); i++) {
        cout << i->genomeName << " " << i->length << " " << i->position << endl;
    }

    cout << "Whole genome matching ~~" << endl;
    Genome g3("query", "GGGGATCG");
    vector <GenomeMatch> gv;
    gm.findRelatedGenomes(g1, 4, true, 0.0, gv);
    for (vector<GenomeMatch>::iterator i = gv.begin(); i != gv.end(); i++) {
        cout << i->genomeName << " " << i->percentMatch << endl;
    }
    

    return 0;
}

/*
#include "Trie.h"
#include "provided.h"
#include <iostream>
#include <vector>
using namespace std;

int main() {
    Trie<int> trie;
    trie.insert("hi", 1);
    trie.insert("hi", 2);
    trie.insert("ho", 3);
    trie.insert("hi", 4);
    trie.insert("ha", 5);
    trie.insert("ai", 6);
    bool match = false;
    vector<int> v;
    v = trie.find("ho", match);
    for (int i = 0; i < v.size(); i++) cout << v[i] << " ";
    cout << endl;
    Genome g1("Genome 1", "CGGTGTACNACGACTGGGGATAGAATATCTTGACGTCGTACCGGTTGTAGTCGTTCGACCGAAGGGTTCCGCGCCAGTAC");
    Genome g2("Genome 2", "TAACAGAGCGGTNATATTGTTACGAATCACGTGCGAGACTTAGAGCCAGAATATGAAGTAGTGATTCAGCAACCAAGCGG");
    Genome g3("Genome 3", "TTTTGAGCCAGCGACGCGGCTTGCTTAACGAAGCGGAAGAGTAGGTTGGACACATTNGGCGGCACAGCGCTTTTGAGCCA");
    vector<DNAMatch> matches;
    bool result;
    GenomeMatcher gm(4);
    gm.addGenome(g1);
    gm.addGenome(g2);
    gm.addGenome(g3);
    result = gm.findGenomesWithThisDNA("GAAGGGTT", 4, false, matches);
    for (int i = 0; i < matches.size(); i++) {
        cout << matches[i].genomeName << " of length " << matches[i].length << " at position: " << matches[i].position << endl;
    }
    vector<GenomeMatch> results;
    Genome query("query", "GAAGGGTTAAAAAAAA");
    gm.findRelatedGenomes(query, 4, false, 0, results);
    for (int i = 0; i < results.size(); i++) {
        cout << results[i].genomeName << " with match percentage: " << results[i].percentMatch << endl;
    }
    return 0;
}*/