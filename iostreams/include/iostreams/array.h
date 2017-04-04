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

#ifndef _IOSTREAMS_ARRAY_H_
#define _IOSTREAMS_ARRAY_H_

#include "iostreams/stream.h"
#include <vector>

namespace iostreams
{
	template<typename byte_type>
	class ArrayStream : public IStream<byte_type>
	{
	public:
		using size_type = typename IStream<byte_type>::size_type;
		using count_type = typename IStream<byte_type>::count_type;
		using off_type = typename IStream<byte_type>::off_type;

	private:
		std::vector<byte_type> data_;
		size_type position_{ 0 };

	public:
		ArrayStream() {}
		
		explicit ArrayStream(std::vector<byte_type>&& data);

		ArrayStream(ArrayStream&&);
		ArrayStream& operator=(ArrayStream&&);

		ArrayStream(const ArrayStream&) = default;
		ArrayStream& operator=(const ArrayStream&) = default;

		static ArrayStream create(const std::string& str, IFromStringTransform<byte_type>& transformer);

		std::string to_string(IToStringTransform<byte_type>& transformer) const override;

		size_type size() const override { return static_cast<size_type>(data_.size()); }
		size_type capacity() const { return static_cast<size_type>(data_.capacity()); }

		void resize(size_type size) override;
		void reserve(size_type size);
		size_type tell() const override { return position_; }
		void seek(off_type off, std::ios_base::seekdir way = std::ios_base::beg) override;
		count_type write(const byte_type* data, count_type size) override;
		count_type read(byte_type* buffer, count_type count) override;
	};
}

#endif
