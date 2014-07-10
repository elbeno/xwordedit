#include <iconv.h>
#include <cctype>
#include <string.h>
#include "string_convert.h"

// ----------------------------------------------------------------------------

static const char *INTERNAL_CHARSET = "iso8859-1";

// ----------------------------------------------------------------------------

StringConverter::StringConverter(const char *fromCode)
{
	mConversionDesc = iconv_open(INTERNAL_CHARSET, fromCode);
}

StringConverter::~StringConverter()
{
	iconv_close(mConversionDesc);
}

// ----------------------------------------------------------------------------

static char *stripAccent(char c, char *pOut)
{
	int ic = c;
	switch (ic)
	{
		case 192: case 193: case 194: case 195:	case 196: case 197:
		case 224: case 225: case 226: case 227: case 228: case 229:
			*pOut++ = 'A'; break;
		case 198: case 230:
			*pOut++ = 'A'; *pOut = 'E'; break;
		case 199: case 231:
			*pOut++ = 'C'; break;
		case 200: case 201: case 202: case 203:
		case 232: case 233: case 234: case 235:
			*pOut++ = 'E'; break;
		case 204: case 205: case 206: case 207:
		case 236: case 237: case 238: case 239:
			*pOut++ = 'I'; break;
		case 209: case 241:
			*pOut++ = 'N'; break;
		case 210: case 211: case 212: case 213: case 214: case 216:
		case 242: case 243: case 244: case 245: case 246: case 248:
			*pOut++ = 'O'; break;
		case 217: case 218: case 219: case 220:
		case 249: case 250: case 251: case 252:
			*pOut++ = 'U'; break;
		case 221: case 255:
			*pOut++ = 'Y'; break;
		default:
			*pOut++ = toupper(c); break;
	}
	return pOut;
}

// ----------------------------------------------------------------------------

void StringConverter::convertCharacterSet(const char *inbuf, char *outbuf, int outbuf_len)
{
	// first, convert to internal character set
	if (mConversionDesc != (iconv_t)-1)
	{
		char *pIn = const_cast<char *>(inbuf);
		char *pOut = outbuf;
		size_t	inbytes = strlen(pIn);
		size_t	outbytes = outbuf_len;
		if (iconv(mConversionDesc, &pIn, &inbytes, &pOut, &outbytes) == (size_t)-1)
		{
			*outbuf = 0;
		}
		else
		{
			*pOut = 0;
		}
	}
}

// ----------------------------------------------------------------------------

void StringConverter::stripAccentsAndNonAlpha(const char *inbuf, char *outbuf, int outbuf_len)
{
	char *pOut = outbuf;
	while (*inbuf && pOut < &outbuf[outbuf_len-2])
	{
		pOut = stripAccent(*inbuf++, pOut);
		if (!isalpha(*(pOut-1)))
			--pOut;
	}
	*pOut = 0;
}

// ----------------------------------------------------------------------------

