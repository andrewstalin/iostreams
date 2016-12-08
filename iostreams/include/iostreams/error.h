// Licensed under the MIT License <http://opensource.org/licenses/MIT>
// Author: Andrew Stalin <andrew.stalin@gmail.com>
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _IOSTREAMS_ERROR_H_
#define _IOSTREAMS_ERROR_H_

#include "liberror/exception.h"

namespace iostreams
{
	namespace errors
	{
		DECLARE_ERROR_INFO(BAD_FILE, 1, "bad file value");
		DECLARE_ERROR_INFO(BAD_FILE_DESCRIPTOR, 2, "bad file number");
		DECLARE_ERROR_INFO(BAD_FILE_ACCESS_VALUE, 3, "invalid FileAccess value");
		DECLARE_ERROR_INFO(BAD_FILE_MODE_VALUE, 4, "invalid FileMode value");
		DECLARE_ERROR_INFO(STREAM_CLOSE, 5, "the stream has already been closed");
		DECLARE_ERROR_INFO(OUT_OF_RANGE, 6, "offset is larger than the data size");
		DECLARE_ERROR_INFO(BUFFER_TOO_SMALL, 7, "insufficient buffer size");
		DECLARE_ERROR_INFO(STREAM_SIZE_TOO_BIG, 8, "max buffer size less than stream size");
		DECLARE_ERROR_INFO(BAD_TRANSFORM_DESTINATION, 9, "source and destination cannot be the same stream");
		DECLARE_ERROR_INFO(BAD_BASE64_CHARACTER, 10, "invalid base64 string character");
		DECLARE_ERROR_INFO(BAD_BASE64_STRING_LENGTH, 11, "invalid base64 string length");
		DECLARE_ERROR_INFO(BAD_HEX_CHARACTER, 12, "invalid hex string character");
		DECLARE_ERROR_INFO(BAD_HEX_STRING_LENGTH, 13, "invalid hex string length");
	}

	class IOStreamsException : public liberror::Exception
	{
	public:
		IOStreamsException(const liberror::ErrorInfo& error_info)
			: Exception(error_info)
		{}

		const char* category() const override { return "IOSTREAMS"; }
	};

#ifdef USE_ZLIB
	class ZLibException : public liberror::Exception
	{
	public:
		ZLibException(int ec, const char* context)
			: liberror::Exception(ec, context)
		{}

		ZLibException(int ec, const char* context, const char* description)
			: liberror::Exception(ec, context, description)
		{}

		const char* category() const override { return "ZLIB"; }
	};
#endif
}

#endif