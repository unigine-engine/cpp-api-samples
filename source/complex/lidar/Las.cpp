#include "Las.h"

#include <UnigineFileSystem.h>
#include <UnigineMathLib.h>

using namespace Unigine;
using namespace Math;

namespace
{
	void fill(void *dst, size_t dst_size, uint8_t value)
	{
		memset(dst, value, dst_size);
	}

	void zero(void *dst, size_t dst_size)
	{
		fill(dst, dst_size, 0);
	}

	void store(void *dst, size_t dst_size, void const *src, size_t src_size)
	{
		#ifdef _WIN32
			memcpy_s(dst, dst_size, src, src_size);
		#else
			if (dst && src && src_size <= dst_size)
			{
				memcpy(dst, src, src_size);
			}
		#endif
	}

	void store(char *dst, size_t dst_size, char const *src)
	{
		#ifdef _WIN32
			strcpy_s(dst, dst_size, src);
		#else
			if (dst && src)
			{
				size_t src_size = strlen(src) + 1;
				src_size = src_size < dst_size ? src_size : dst_size;
				memcpy(dst, src, src_size);
			}
		#endif
	}

	template<typename T, std::enable_if_t<std::is_fundamental<T>::value, bool> = true>
	void store(void *dst, T value)
	{
		store(dst, sizeof(T), &value, sizeof(T));
	}

	template<typename T, std::enable_if_t<std::is_fundamental<T>::value, bool> = true>
	T load(void const *src)
	{
		T result;
		store(&result, sizeof(T), src, sizeof(T));
		return result;
	}

	template<typename T, std::enable_if_t<std::is_fundamental<T>::value, bool> = true>
	void store_bits(void *dst, T bits, int offset, int length)
	{
		T value = load<T>(dst);
		T mask = ((T(1) << T(length)) - T(1)) << T(offset);
		T result = (value & (~mask)) | ((bits << T(offset)) & mask);
		store<T>(dst, result);
	}

	template<typename T, std::enable_if_t<std::is_fundamental<T>::value, bool> = true>
	T load_bits(void const *src, int offset, int length)
	{
		T value = load<T>(src);
		T mask = (T(1) << T(length)) - T(1);
		T result = (value >> T(offset)) & mask;
		return result;
	}
}

namespace Las
{

void LasWriter_v1_4::init()
{
	::zero(&header, sizeof(header));

	::store(header.file_signature, sizeof(header.file_signature), "LASF", 4);

	// header.file_source_id
	// header.global_encoding
	// header.project_id_guid_data_1
	// header.project_id_guid_data_2
	// header.project_id_guid_data_3
	// header.project_id_guid_data_4

	::store(&header.version_major, U8(1));
	::store(&header.version_minor, U8(4));
	::store(header.system_identifier, sizeof(header.system_identifier), "unigine");
	::store(header.generating_software, sizeof(header.generating_software), "cpp_samples/complex/lidar");

	// header.file_creation_day_of_year
	// header.file_creation_year

	::store(&header.header_size, U16(sizeof(header)));
	::store(&header.offset_to_point_data, U32(header.header_size));
	::store(&header.number_of_variable_length_records, U32(0));
	::store(&header.point_data_record_format, U8(0));
	::store(&header.point_data_record_length, U16(sizeof(LasPointDataRecordFormat0_v1_4)));

	// header.legacy_number_of_point_records
	// header.legacy_number_of_point_by_return

	{
		scale = vec3(0.001f);
		iscale = vec3(Math::rcp(scale.x), Math::rcp(scale.y), Math::rcp(scale.z));

		::store(&header.x_scale_factor, F64(scale.x));
		::store(&header.y_scale_factor, F64(scale.y));
		::store(&header.z_scale_factor, F64(scale.z));
	}

	::store(&header.x_offset, F64(0.));
	::store(&header.y_offset, F64(0.));
	::store(&header.z_offset, F64(0.));

	::store(&header.max_x, F64(0.));
	::store(&header.max_y, F64(0.));
	::store(&header.max_z, F64(0.));
	::store(&header.min_x, F64(0.));
	::store(&header.min_y, F64(0.));
	::store(&header.min_z, F64(0.));

	// header.start_of_waveform_data_packet_record
	// header.start_of_first_extended_variable_length_record
	// header.number_of_extended_variable_length_records
	// header.number_of_point_records
	// header.number_of_points_by_return
}

void LasWriter_v1_4::add_point(Unigine::Math::vec3 const& position, float intensity)
{
	auto& record = points.append();

	{
		::zero(&record, sizeof(record));

		::store(&record.x, S32(position.x * iscale.x));
		::store(&record.y, S32(position.y * iscale.y));
		::store(&record.z, S32(position.z * iscale.z));

		{
			using intensity_t = decltype(record.intensity);
			float constexpr max_intensity = static_cast<float>(std::numeric_limits<intensity_t>::max());
			intensity_t converted_intensity = intensity_t(Math::saturate(intensity) * max_intensity);
			::store(&record.intensity, intensity_t(converted_intensity));
		}

		{
			U8 return_number = 1;
			::store_bits(&record.flags, return_number, 0, 3);
		}
		{
			U8 number_of_returns = 1;
			::store_bits(&record.flags, number_of_returns, 3, 3);
		}
		{
			U8 scan_direction_flag = 0;
			::store_bits(&record.flags, scan_direction_flag, 6, 1);
		}
		{
			U8 edge_of_flight_line = 0;
			::store_bits(&record.flags, edge_of_flight_line, 7, 1);
		}

		::store(&record.classification, U8(0));
		::store(&record.scan_angle_rank, S8(0));
		::store(&record.user_data, U8(0));
		::store(&record.point_source_id, U16(0));
	}

	{
		bbox.expand(position);
	}
}

void LasWriter_v1_4::save(Unigine::StringStack<> const& path)
{
	{
		::store(&header.max_x, F64(bbox.maximum.x));
		::store(&header.max_y, F64(bbox.maximum.y));
		::store(&header.max_z, F64(bbox.maximum.z));
		::store(&header.min_x, F64(bbox.minimum.x));
		::store(&header.min_y, F64(bbox.minimum.y));
		::store(&header.min_z, F64(bbox.minimum.z));

		::store(&header.number_of_point_records, U64(points.size()));
		::store(&header.number_of_points_by_return[0], U64(points.size()));
	}

	{
		FilePtr file = File::create();

		file->open(path , "wb+");
		file->write(&header, sizeof(header));

		for (auto const& point : points)
		{
			file->write(&point, sizeof(point));
		}

		file->close();
	}
}

}
