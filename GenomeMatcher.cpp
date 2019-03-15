#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "Trie.h"
#include <algorithm>
using namespace std;


struct DNAPrefix
{
    DNAPrefix(const Genome* g, int p):genome(g), position(p) {}
    DNAPrefix& operator=(const DNAPrefix& d) {
        if (&d == this) {
            return *this;
        }
        genome = d.genome;
        position = d.position;
        return *this;
    } 
    const Genome* genome;
    int position;
};

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    ~GenomeMatcherImpl();
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
    int m_minSearchLength;
    Trie<DNAPrefix> m_searchTrie;
    vector<Genome*> m_Genomes; 
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
:m_minSearchLength(minSearchLength)
{
    // This compiles, but may not be correct
}

GenomeMatcherImpl::~GenomeMatcherImpl()
{
    for (vector<Genome*>::iterator g = m_Genomes.begin(); g != m_Genomes.end(); g++) {
        delete *g;
    }
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    Genome* g  = new Genome(genome);
    //cout << "adding genome " << genome.name() << " of size " << genome.length() << endl;
    m_Genomes.push_back(g);
    int max_position = genome.length() - m_minSearchLength;
    for (int i = 0; i < max_position; i++) {
        DNAPrefix d(g, i);
        string fragment;
        genome.extract(i, m_minSearchLength, fragment);
        if (i == 0) {
            //cout << "starts with " << fragment << endl;
        }
        m_searchTrie.insert(fragment, d);
    } 
}

int GenomeMatcherImpl::minimumSearchLength() const
{
    return m_minSearchLength;  // This compiles, but may not be correct
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    int flength = fragment.length();
    if (flength < m_minSearchLength) return false;  
    //cout << "searching " << fragment << " of length " << flength << endl;
    // don't care about the distinction between exactmatchonly and not right here
    vector<DNAPrefix> prefixes = m_searchTrie.find(fragment.substr(0, m_minSearchLength), false);
    //cout << prefixes.size() << " potential matches" << endl;
    unordered_map<string, bool> genomesAlreadyMatched;
    for (vector<Genome*>::const_iterator g = m_Genomes.begin(); g != m_Genomes.end(); g++) {
        genomesAlreadyMatched[(*g)->name()] = false;
    } 
    for (vector<DNAPrefix>::iterator e = prefixes.begin(); e != prefixes.end(); e++) {
        if (genomesAlreadyMatched[e->genome->name()]) continue;
        //cout << "looking at prefix at "<< e->position << " in " << e->genome->name() << endl;
        string s;
        // maxMatchLength = min(flength, length until end of genome)
        int maxMatchLength = e->genome->length() - e->position;
        if (flength < maxMatchLength) maxMatchLength = flength;

        if (maxMatchLength < minimumLength) {
            // oops
            //cout << "fail, maxMatchLength < minimumLength" << endl;
            continue;
        }
        e->genome->extract(e->position, maxMatchLength, s);
        int numMisses = exactMatchOnly; // start with 1 if need exact match, 0 otherwise
        int lengthOfMatch = 0;
        bool failure = false;
        for (int i = 0; i < maxMatchLength; i++) {
            // wack bug, this used to be s[i] != i (?)
            if (s[i] !=  fragment[i]) {
                numMisses++;
            }
            if (numMisses == 2) {
                failure = (i < minimumLength);
                //cout << "possible fail, stopped looking at " << i << " because too many misses" << endl;
                break;
            }
            lengthOfMatch++;
        }
        if (!failure) {
            genomesAlreadyMatched[e->genome->name()] = true;
            DNAMatch d;
            d.genomeName = e->genome->name();
            d.position = e->position;
            d.length = lengthOfMatch;
            matches.push_back(d);
        }
    }
    return true;
}

bool compareGenomeMatches(const GenomeMatch& lhs, const GenomeMatch& rhs) {    
    return (lhs.percentMatch > rhs.percentMatch || 
        (lhs.percentMatch == rhs.percentMatch && lhs.genomeName < rhs.genomeName));
}



bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    int max_i = query.length() - fragmentMatchLength;
    if (max_i < 0) return false;
    results.clear();
    int S = query.length()/fragmentMatchLength; // should be an integer we can divide
    unordered_map<string, int> numHitsPerGenome;
    for (vector<Genome*>::const_iterator g = m_Genomes.begin(); g != m_Genomes.end(); g++) {
        numHitsPerGenome[(*g)->name()] = 0;
    }
    for (int i = 0; i <= max_i; i += fragmentMatchLength) {
        string s;
        query.extract(i, fragmentMatchLength, s);
        vector<DNAMatch> matches;
        findGenomesWithThisDNA(s, fragmentMatchLength, exactMatchOnly, matches);

        for (vector<DNAMatch>::iterator di = matches.begin(); di != matches.end(); di++) {
            numHitsPerGenome[di->genomeName] += 1;
        }
    }

    for (unordered_map<string, int>::iterator mi = numHitsPerGenome.begin(); mi != numHitsPerGenome.end(); mi++) {
        double percentMatch = 100 * ((1.0 * mi->second)/S);
        if (percentMatch >= matchPercentThreshold) {
            GenomeMatch gm;
            gm.genomeName = mi->first;
            gm.percentMatch = percentMatch;
            results.push_back(gm);
        }
    }

    sort(results.begin(), results.end(), compareGenomeMatches);
    return true; 
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
