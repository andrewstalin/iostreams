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

#ifndef _IOSTREAMS_ZIP_H_
#define _IOSTREAMS_ZIP_H_

#ifdef USE_ZLIB

#include "iostreams/stream.h"
#include "contrib/minizip/zip.h"

namespace iostreams
{
	void* ZCALLBACK open_zip_64(void* opaque, const void* filename, int mode);
		
	template<typename byte_type>
	uLong ZCALLBACK read_zip(void* opaque, void* stream, void* buf, uLong size);

	template<typename byte_type>
	uLong ZCALLBACK write_zip(void* opaque, void* stream, const void* buf, uLong size);

	template<typename byte_type>
	ZPOS64_T ZCALLBACK tell_64_zip(void* opaque, void* stream);

	template<typename byte_type>
	long ZCALLBACK seek_64_zip(void* opaque, void* stream, ZPOS64_T offset, int origin);

	int ZCALLBACK close_zip(void* opaque, void* stream);

	int ZCALLBACK error_zip(void* opaque, void* stream);

	template<typename byte_type>
	zlib_filefunc64_def get_file_func(IStream<byte_type>* stream);
}

#endif
#endif