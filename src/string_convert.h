#ifndef STRING_CONVERT_H_
#define STRING_CONVERT_H_

// ----------------------------------------------------------------------------

#include <iconv.h>

// ----------------------------------------------------------------------------

class StringConverter
{
	public:
		StringConverter(const char *fromCode);
		~StringConverter();
		
		void	convertCharacterSet(const char *inbuf, char *outbuf, int outbuf_len);
		void	stripAccentsAndNonAlpha(const char *inbuf, char *outbuf, int outbuf_len);
				
	private:
		iconv_t	mConversionDesc;
};

// ----------------------------------------------------------------------------

#endif /*STRING_CONVERT_H_*/
