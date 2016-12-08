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

#ifdef USE_ZLIB
#include "zip.h"

using namespace iostreams;

void* ZCALLBACK iostreams::open_zip_64(void* opaque, const void* filename, int mode)
{
	return opaque;
}

template<typename byte_type>
uLong ZCALLBACK iostreams::read_zip(void* opaque, void* stream, void* buf, uLong size)
{
	auto stream_ptr = static_cast<IStream<byte_type>*>(stream);
	return stream_ptr->read(static_cast<byte_type*>(buf), size);
}

template<typename byte_type>
uLong ZCALLBACK iostreams::write_zip(void* opaque, void* stream, const void* buf, uLong size)
{
	auto stream_ptr = static_cast<IStream<byte_type>*>(stream);
	stream_ptr->write(static_cast<const byte_type*>(buf), size);
	return size;
}

template<typename byte_type>
ZPOS64_T ZCALLBACK iostreams::tell_64_zip(void* opaque, void* stream)
{
	auto stream_ptr = static_cast<IStream<byte_type>*>(stream);
	return stream_ptr->tell();
}

template<typename byte_type>
long ZCALLBACK iostreams::seek_64_zip(void* opaque, void* stream, ZPOS64_T offset, int origin)
{
	auto stream_ptr = static_cast<IStream<byte_type>*>(stream);
	std::ios_base::seekdir way;

	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:
		way = std::ios_base::cur;
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		way = std::ios_base::end;
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		way = std::ios_base::beg;
		break;
	default:
		return -1;
	}

	stream_ptr->seek(offset, way);
	return 0;
}

int ZCALLBACK iostreams::close_zip(void* opaque, void* stream)
{
	return 0;
}

int ZCALLBACK iostreams::error_zip(void* opaque, void* stream)
{
	return 0;
}

template<typename byte_type>
zlib_filefunc64_def iostreams::get_file_func(IStream<byte_type>* stream)
{
	zlib_filefunc64_def filefunc = { 0 };
	filefunc.zopen64_file = open_zip_64;
	filefunc.zread_file = read_zip<byte_type>;
	filefunc.zwrite_file = write_zip<byte_type>;
	filefunc.ztell64_file = tell_64_zip<byte_type>;
	filefunc.zseek64_file = seek_64_zip<byte_type>;
	filefunc.zclose_file = close_zip;
	filefunc.zerror_file = error_zip;
	filefunc.opaque = static_cast<void*>(stream);
	return filefunc;
}

template zlib_filefunc64_def iostreams::get_file_func<uint8_t>(IStream<uint8_t>* stream);
template zlib_filefunc64_def iostreams::get_file_func<char>(IStream<char>* stream);

#endif