#ifndef DICTIONARY_H_
#define DICTIONARY_H_

// ----------------------------------------------------------------------------

#include <string>
#include <list>
#include <vector>
#include <set>
#include <cassert>

// ----------------------------------------------------------------------------

class Letter
{
	public:
		Letter();
		Letter(Letter *parent, char v);

		void	setValue(char v) { assert(v >= 'A' && v <= 'Z'); mValue = v; }
		Letter	*addChild(char v, bool &already_exists);
		
		char			getValue() const { return mValue; }
		const Letter	*getParent() const { return mpParent; }
		
		bool operator<(const Letter &rhs) { return mValue < rhs.mValue; }
		bool operator==(const Letter &rhs) { return mValue == rhs.mValue; }
		
	private:
		const Letter 		*mpParent;
		char				mValue;			
		std::list<Letter>	mChildren;
};

// ----------------------------------------------------------------------------

#define DEFAULT_ENCODING	"iso8859-1"

// ----------------------------------------------------------------------------

class Dictionary
{
	public:
		Dictionary(unsigned int maxWordLen);
		~Dictionary();

		void addWord(const char *word);
		void addWordsFromFile(const char *filename, const char *encoding = DEFAULT_ENCODING);
		void getMatchingWords(const std::string pattern, std::list<std::string> &matchingList) const;
		unsigned int getNumWords() const	{ return mNumWords; }
	
	private:
		Letter								mWords;
		
		typedef std::set<const Letter *>	LetterSet;
		typedef std::vector<LetterSet *>	TemplateVector;
		std::vector<TemplateVector *>		mWordTemplates;
		
		unsigned int						mNumWords;
		unsigned int						mMaxWordLen;
};

// ----------------------------------------------------------------------------

#endif /*DICTIONARY_H_*/
