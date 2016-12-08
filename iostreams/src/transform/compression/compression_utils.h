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

#ifndef _IOSTREAMS_COMPRESSION_UTILS_H_
#define _IOSTREAMS_COMPRESSION_UTILS_H_

#ifdef USE_ZLIB
#include "iostreams/transform/compression/compression_level.h"
#include "zlib.h"

namespace iostreams
{
#define GZIP_WINDOW_BITS 16 + MAX_WBITS

	inline int get_zlib_level(CompressionLevel level)
	{
		switch (level)
		{
		case CompressionLevel::FAST:
			return Z_BEST_SPEED;
		case CompressionLevel::BEST:
			return Z_BEST_COMPRESSION;
		default:
			return Z_DEFAULT_COMPRESSION;
		}
	}
}

#endif
#endif