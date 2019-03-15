#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
#include <cctype>
using namespace std;

class GenomeImpl
{
public:
    GenomeImpl(const string& nm, const string& sequence);
    GenomeImpl(const GenomeImpl& other);
    GenomeImpl& operator= (const GenomeImpl& other);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int len, string& fragment) const;
private:
    string m_name;
    string m_sequence;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence)
:m_name(nm), m_sequence(sequence) // avoid copying sequence? premature optimization 
{
}

GenomeImpl::GenomeImpl(const GenomeImpl& other) {
    *this = other;
}

GenomeImpl& GenomeImpl::operator= (const GenomeImpl& other) {
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    return *this;
}


bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) 
{
    bool hitEOF = false;
    // consume initial '>' character
    char initialBracket;
    genomeSource.get(initialBracket);
    if (!genomeSource) {
        //cout << "hmm, wack" << endl;
        // empty istream, probably ok?
        return true;
    }
    if (initialBracket != '>') {
        // bad format
        //cout << "doesnt start with bracket??" << endl;
        return false;
    }
    while (!hitEOF) {
        string nm;
        string sequence;
        if (!getline(genomeSource, nm) || nm.length() == 0) {
            // wack
            //cout << "no name ?" << endl;
            return false;
        }
        //cout << "name is " << nm << endl;
        bool hitEndOfLine = false;
        bool veryFirstChar = true;

        for (;;) { // tight loop here
            char c;
            genomeSource.get(c);
            if (!genomeSource) {
                // end of file
                hitEOF = true;
                break;
            }

            c = toupper(c);

            bool hitEndOfSequence = false;
            
            switch (c)
            {     
                case 'A':
                case 'C':
                case 'G':
                case 'N':
                case 'T':
                    hitEndOfLine = false;
                    sequence += c;
                    break;

                case '>':
                    if (veryFirstChar) return false;
                    if (!hitEndOfLine) return false; // must be on its own line
                    hitEndOfSequence = true;
                    break;

                case '\n':
                    if (veryFirstChar) return false;
                    if (hitEndOfLine) return false; // cannot have consecutive newlines
                    hitEndOfLine = true;
                    break;
                default:
                    //uh oh
                    return false;
            }
            if (hitEndOfSequence) {
                break;
            }
            veryFirstChar = false;
        }
        genomes.push_back(Genome(nm, sequence));
    }
    return true;
}

int GenomeImpl::length() const
{
    return m_sequence.length();
}

string GenomeImpl::name() const
{
    return m_name;  // This compiles, but may not be correct
}

bool GenomeImpl::extract(int position, int len, string& fragment) const
{
    if (position < 0 || len < 0) {
        return false;
    }
    if (position + len > length()) {
        return false;
    }
    string f;
    for (int i = 0; i < len; i++) {
        f += m_sequence[position + i];
    }
    fragment = f;
    return true; 
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes) 
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
