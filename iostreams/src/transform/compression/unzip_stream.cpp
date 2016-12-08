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

#include "iostreams/transform/compression/unzip_stream.h"
#include "zip.h"
#include "iostreams/error.h"
#include "babel/encoding.h"
#include <cassert>

template<typename byte_type>
unzFile OpenUnZipFile(const std::shared_ptr<iostreams::IStream<byte_type>>& stream)
{
	assert(stream != nullptr);

	auto filefunc = iostreams::get_file_func<byte_type>(stream.get());
	auto uf = ::unzOpen2_64(nullptr, &filefunc);
	THROW_IF(uf == nullptr, iostreams::ZLibException(ZIP_ERRNO, "unzOpen2_64"));
	return uf;
}

iostreams::ZipArchiveEntry GetEntryInfo(unzFile uf)
{
#ifdef _WIN32
	char file_name_in_zip[MAX_PATH];
#elif __linux__
	char file_name_in_zip[PATH_MAX];
#endif
	unz_file_info64 file_info;
	
	auto rc = ::unzGetCurrentFileInfo64(uf, &file_info, file_name_in_zip, sizeof(file_name_in_zip), nullptr, 0, nullptr, 0);
	THROW_IF(rc != UNZ_OK, iostreams::ZLibException(rc, "unzGetCurrentFileInfo64"));

	iostreams::ZipArchiveEntry entry;
#ifdef _WIN32
	entry.name = babel::encode("cp866", "UTF-8", file_name_in_zip, strlen(file_name_in_zip));
	entry.is_directory = (file_info.external_fa & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif __linux__
	entry.name = file_name_in_zip;
#endif

	entry.uncompressed_size = file_info.uncompressed_size;
	entry.compressed_size = file_info.compressed_size;
	return entry;
}

namespace iostreams
{
	template<typename byte_type>
	void UnZipStream<byte_type>::UnZipFileGuard::close()
	{
		if (val != nullptr)
		{
			auto rc = ::unzClose(val);
			THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzClose"));

			val = nullptr;
		}
	}

	template<typename byte_type>
	UnZipStream<byte_type> UnZipStream<byte_type>::create(const std::shared_ptr<stream_type>& stream, const char* entry_name)
	{
		assert(entry_name != nullptr);

		auto uf = OpenUnZipFile(stream);
		int rc{ UNZ_OK };

#ifdef _WIN32
		auto en = babel::encode("UTF-8", "cp866", entry_name, strlen(entry_name));
		rc = ::unzLocateFile(uf, en.c_str(), 0);
#elif __linux__
		rc = ::unzLocateFile(uf, entry_name, 0);
#endif
		THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzLocateFile"));

		rc = ::unzOpenCurrentFile(uf);
		THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzOpenCurrentFile"));

		return UnZipStream(stream, uf);
	}
	
	template<typename byte_type>
	UnZipStream<byte_type> UnZipStream<byte_type>::create(const std::shared_ptr<stream_type>& stream, uint64_t entry_index)
	{
		auto uf = OpenUnZipFile(stream);
		auto rc = ::unzGoToFirstFile(uf);
		THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzGoToFirstFile"));

		while (entry_index > 0)
		{
			rc = ::unzGoToNextFile(uf);
			THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzGoToNextFile"));
			--entry_index;
		}

		rc = ::unzOpenCurrentFile(uf);
		THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzOpenCurrentFile"));

		return UnZipStream(stream, uf);
	}

	template<typename byte_type>
	std::vector<ZipArchiveEntry> UnZipStream<byte_type>::get_entries_info(const std::shared_ptr<stream_type>& stream)
	{
		UnZipFileGuard unzip_file_guard(OpenUnZipFile(stream));
		unz_global_info64 global_info;
		auto rc = ::unzGetGlobalInfo64(unzip_file_guard.val, &global_info);
		THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzGetGlobalInfo64"));

		std::vector<ZipArchiveEntry> entries;

		if (global_info.number_entry > 0)
		{
			rc = ::unzGoToFirstFile(unzip_file_guard.val);
			THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzGoToFirstFile"));

			entries.reserve(static_cast<size_t>(global_info.number_entry));
			uint64_t i{ 0 };

			while(true)
			{
				auto entry = GetEntryInfo(unzip_file_guard.val);
				entry.index = i;
				entries.push_back(entry);

				if (++i < global_info.number_entry)
				{
					rc = ::unzGoToNextFile(unzip_file_guard.val);
					THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzGoToNextFile"));
				}
				else
				{
					break;
				}
			}
		}

		return entries;
	}

	template<typename byte_type>
	ZipArchiveEntry UnZipStream<byte_type>::get_entry_info() const
	{
		THROW_IF(is_close_, IOStreamsException(errors::STREAM_CLOSE));
		return GetEntryInfo(unzip_file_guard_.val);
	}

	template<typename byte_type>
	typename UnZipStream<byte_type>::count_type UnZipStream<byte_type>::read(byte_type* buffer, count_type size)
	{
		assert(buffer != nullptr);

		THROW_IF(is_close_, IOStreamsException(errors::STREAM_CLOSE));
		auto bytes_count = ::unzReadCurrentFile(unzip_file_guard_.val, buffer, size);

		THROW_IF(bytes_count < 0, ZLibException(bytes_count, "unzReadCurrentFile"));
		return static_cast<count_type>(bytes_count);
	}

	template<typename byte_type>
	void UnZipStream<byte_type>::close()
	{
		if (!is_close_)
		{
			is_close_ = true;

			auto rc = ::unzCloseCurrentFile(unzip_file_guard_.val);
			THROW_IF(rc != UNZ_OK, ZLibException(rc, "unzCloseCurrentFile"));

			unzip_file_guard_.close();
		}
	}

	template class UnZipStream<uint8_t>;
	template class UnZipStream<char>;
}

#endif