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

#ifndef _IOSTREAMS_TRANSFORM_H_
#define _IOSTREAMS_TRANSFORM_H_

#include <functional>

#ifdef __linux__
#include <cstddef>
#endif

namespace iostreams
{
	template<typename source_type, typename destination_type>
	struct ITransform
	{
		using size_type = size_t;
		using TransformHandler = std::function<void(const destination_type* data, size_type size)>;

		virtual void update(const source_type* data, size_type size, const TransformHandler& handler) = 0;
		virtual void update_final(const TransformHandler& handler) = 0;
	};
}
#endif