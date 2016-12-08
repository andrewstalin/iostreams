// Licensed under the MIT License <http://opensource.org/licenses/MIT>
// Author: Andrew Stalin andrew.stalin@gmail.com
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "iostreams/file.h"
#include "liberror/exception.h"
#include <fstream>

namespace iostreams
{
	template<typename byte_type>
	class FileStream<byte_type>::FileImpl
	{
	private:
		std::fstream fs_;

	public:
		using size_type = typename FileStream<byte_type>::size_type;
		using count_type = typename FileStream<byte_type>::count_type;
		using off_type = typename FileStream<byte_type>::off_type;


		size_type size() const
		{
			auto current = fs_.tellg();
			fs_.seekg(0, fs_.end);
			auto result = fs_.tellg();
			fs_.seekg(current, fs_.beg);
			return result;
		}

		size_type tell() const
		{
			return static_cast<size_type>(fs_.tellg());
		}

		void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg)
		{
			FileStream<byte_type>::check_seek_range(off, way, tell(), size());

		}

		void resize(size_type new_size)
		{
			auto current_position = tell();

			if (current_position != new_size)
			{
				LARGE_INTEGER pos;
				pos.QuadPart = new_size;
				THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, FILE_BEGIN), WIN32_ERROR("SetFilePointerEx"));
			}

			THROW_IF(!::SetEndOfFile(handle_), WIN32_ERROR("SetEndOfFile"));

			if (current_position != new_size)
			{
				DWORD way{ FILE_BEGIN };

				LARGE_INTEGER pos;
				pos.QuadPart = current_position;

				if (current_position > new_size)
				{
					way = FILE_END;
					pos.QuadPart = 0;
				}

				THROW_IF(!::SetFilePointerEx(handle_, pos, &pos, way), WIN32_ERROR("SetFilePointerEx"));
			}
		}

		count_type read(byte_type* buffer, count_type count)
		{
			fs_.read(reinterpret_cast<char*>(buffer), count);
			return static_cast<count_type>(fs_.gcount());
		}

		count_type write(const byte_type* data, count_type size)
		{
			auto position = fs_.tellp();
			fs_.write(reinterpret_cast<const char*>(data), size);

			if (auto_flush_)
			{
				fs_.flush();
			}

			return static_cast<count_type>(fs_.tellp() - position);
		}
	};
}

