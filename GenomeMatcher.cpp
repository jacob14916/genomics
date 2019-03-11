#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "Trie.h"
#include <algorithm>
using namespace std;


struct DNAPrefix
{
    DNAPrefix();
    Genome& genome;
    int position;
};

class GenomeMatcherImpl
{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
    int m_minSearchLength;
    Trie<DNAPrefix> m_searchTrie;
    vector<const Genome&> m_Genomes; 
};

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength)
:m_minSearchLength(minSearchLength)
{
    // This compiles, but may not be correct
}

void GenomeMatcherImpl::addGenome(const Genome& genome)
{
    m_Genomes.push_back(genome);
    int max_position = genome.length() - m_minSearchLength;
    for (int i = 0; i < max_position; i++) {
        DNAPrefix d;
        d.genome = genome;
        d.position = i;
        string fragment;
        genome.extract(i, m_minSearchLength, fragment);
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
    if (flength < m_minSearchLength) return false;  // This compiles, but may not be correct
    int extralength = flength - m_minSearchLength;
    int minextralen = minimumLength - m_minSearchLength;
    vector<DNAPrefix> prefixes = m_searchTrie.find(fragment.substr(0, m_minSearchLength), exactMatchOnly);
    if (!exactMatchOnly) {
        vector<DNAPrefix> exactPrefixes = m_searchTrie.find(fragment.substr(0, m_minSearchLength), true);
        for (vector<DNAPrefix>::iterator e = exactPrefixes.begin(); e != exactPrefixes.end(); e++) {
            string s;
            if (!((*e).genome.extract((*e).position + m_minSearchLength, extralength, s))) {
                continue;
            }
            int numMisses = 0;
            int lengthOfMatch = m_minSearchLength;
            bool failure = false;
            for (int i = 0; i < extralength; i++) {
                if (s[i] != fragment[m_minSearchLength + i]) {
                    numMisses++;
                }
                if (numMisses == 2) {
                    failure = (i < minextralen);
                    break;
                }
                lengthOfMatch++;
            }
            if (!failure) {
                DNAMatch d;
                d.genomeName = (*e).genome.name;
                d.position = (*e).position;
                d.length = lengthOfMatch;
                matches.push_back(d);
            }
        }
    }
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
    map<string, int> numHitsPerGenome;
    map<string, bool> genomesMatchedThisTime;
    for (vector<const Genome&>::const_iterator g = m_Genomes.begin(); g != m_Genomes.end(); g++) {
        numHitsPerGenome[(*g).name()] = 0;
        genomesMatchedThisTime[(*g).name()] = false;
    }
    for (int i = 0; i <= max_i; i += fragmentMatchLength) {
        string s;
        query.extract(i, fragmentMatchLength, s);
        vector<DNAMatch> matches;
        findGenomesWithThisDNA(s, fragmentMatchLength, exactMatchOnly, matches);

        for (vector<DNAMatch>::iterator m = matches.begin(); m != matches.end(); m++) {
            genomesMatchedThisTime[(*m).genomeName] = true;
        }

        for (vector<const Genome&>::const_iterator g = m_Genomes.begin(); g != m_Genomes.end(); g++) {
            numHitsPerGenome += genomesMatchedThisTime[(*g).name()];
            genomesMatchedThisTime[(*g).name()] = false;
        }
    }

    for (map<string, int>::iterator mi = numHitsPerGenome.begin(); mi != numHitsPerGenome.end(); mi++) {
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
