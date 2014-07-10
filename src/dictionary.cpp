#include <algorithm>
#include <functional>
#include <fstream>
#include <cctype>
#include <cstring>
#include "dictionary.h"
#include "string_convert.h"

using namespace std;

// ----------------------------------------------------------------------------

Letter::Letter()
	: mpParent(0)
	, mValue(0)
{	
}

Letter::Letter(Letter *parent, char v)
	: mpParent(parent)
	, mValue(v)
{
	assert(mValue >= 'A' && mValue <= 'Z');
	assert(mpParent);
}

// ----------------------------------------------------------------------------

Letter	*Letter::addChild(char v, bool &already_exists)
{
	assert(v >= 'A' && v <= 'Z');
	
	already_exists = true;
	// does the letter already exist?
	Letter newLetter(this, v);
	list<Letter>::iterator it = find(mChildren.begin(), mChildren.end(), newLetter);
	if (it != mChildren.end())
		return &(*it);
	
	already_exists = false;
	// find the position it should go and insert it
	it = lower_bound(mChildren.begin(), mChildren.end(), newLetter);
	it = mChildren.insert(it, newLetter);		
	return &(*it);
}

// ----------------------------------------------------------------------------

static const unsigned int MAX_WORD_LEN = 64;
static const int ALPHABET_LETTERS = 26;

Dictionary::Dictionary(unsigned int maxWordLen)
	: mNumWords(0)
	, mMaxWordLen(maxWordLen)
{
	assert(mMaxWordLen > 0 && mMaxWordLen < MAX_WORD_LEN);
	
	// max word length is the number of word templates we keep
	mWordTemplates.reserve(mMaxWordLen);
	// word template N has size N+1
	for (unsigned int i = 0; i < mMaxWordLen; ++i)
	{
		mWordTemplates[i] = new TemplateVector(i+1);
		for (unsigned int j = 0; j < i+1; ++j)
		{
			(*mWordTemplates[i])[j] = new LetterSet[ALPHABET_LETTERS];
		}
	}
}

Dictionary::~Dictionary()
{
	for (unsigned int i = 0; i < mMaxWordLen; ++i)
	{
		for (unsigned int j = 0; j < i+1; ++j)
		{
			delete[] (*mWordTemplates[i])[j];
		}
		delete mWordTemplates[i];
	}
}

// ----------------------------------------------------------------------------

void Dictionary::addWord(const char *word)
{
	assert(word != 0);
	
	unsigned int length = strlen(word);
	if (length == 0 || length > mMaxWordLen)
		return;

	// insert into word tree
	bool already_exists = false;
	Letter	*l = &mWords;
	for (const char *pChar = word; *pChar; ++pChar)
	{		
		l = l->addChild(*pChar, already_exists);
	}
	if (already_exists)
		return;
	// l now is the final letter of the word in the word tree
	
	// insert into templates
	TemplateVector	&tv = *mWordTemplates[length-1];
	int i = 0;

	for (const char *pChar = word; *pChar; ++pChar, ++i)
	{
		assert(*pChar >= 'A' && *pChar <= 'Z');
		tv[i][*pChar-'A'].insert(l);
	}
	
	++mNumWords;
}

// ----------------------------------------------------------------------------

void Dictionary::addWordsFromFile(const char *filename, const char *encoding)
{
	ifstream f(filename);
	if (!f.is_open())
		return;

	// one string per line
	StringConverter conv(encoding);
	char str_buf[128];
	str_buf[0] = 0;
	char convert_str_buf[128];
	while (f.good())
	{
		f.getline(str_buf, 128);
		if (str_buf[0])
		{
			// convert string to upper case, eliminate undesirable characters
			conv.convertCharacterSet(str_buf, convert_str_buf, 128);
			conv.stripAccentsAndNonAlpha(convert_str_buf, str_buf, 128);
			addWord(str_buf);
		}
		str_buf[0] = 0;
	}
	
	f.close();
}

// ----------------------------------------------------------------------------

void Dictionary::getMatchingWords(const string pattern, list<string> &matchingList) const
{
	assert(pattern.length() > 0);

	// init the list
	matchingList.clear();

	// compute the intersection of all the relevant pattern sets
	const TemplateVector &tv = *mWordTemplates[pattern.length()-1];

	// skip initial spaces
	const char *pChar = pattern.c_str();
	int i = 0;
	while (*pChar < 'A' || *pChar > 'Z') ++pChar, ++i;

	// initialise the intersection
	vector<const Letter *> working1(tv[i][*pChar-'A'].begin(), tv[i][*pChar-'A'].end());
	vector<const Letter *> working2;
	working2.reserve(working1.size());
	vector<const Letter *> *pW1 = &working1;
	vector<const Letter *> *pW2 = &working2;
	vector<const Letter *>::iterator it;
	for (++pChar, ++i; *pChar; ++pChar, ++i)
	{
		// skip spaces
		if (*pChar < 'A' || *pChar > 'Z')
			continue;
			
		// compute the intersection so far
		it = set_intersection(pW1->begin(), pW1->end(),
							tv[i][*pChar-'A'].begin(), tv[i][*pChar-'A'].end(),
							pW2->begin());
		pW2->erase(it, pW2->end());
		
		// early out if we have the empty set already, otherwise swap pointers and iterate
		if (pW2->empty())
			return;
		swap(pW1, pW2);		
	}
	
	// build the matching string list
	char	wordBuffer[MAX_WORD_LEN + 1];
	wordBuffer[mMaxWordLen+1] = 0;
	for (vector<const Letter *>::iterator it = pW1->begin(); it != pW1->end(); ++it)
	{
		// build up the word backwards in the buffer
		char *pWord = &wordBuffer[mMaxWordLen+1];
		for (const Letter *pLetter = (*it); pLetter->getValue(); pLetter = pLetter->getParent())
		{
			*(--pWord) = pLetter->getValue();
		}
		matchingList.push_back(string(pWord));
	}
}

