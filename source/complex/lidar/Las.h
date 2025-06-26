#pragma once

#include <stdint.h>
#include <UnigineVector.h>
#include <UnigineMathLib.h>

#define LAS_VERSION_MAJOR 1
#define LAS_VERSION_MINOR 4
#define LAS_VERSION (LAS_VERSION_MINOR | LAS_VERSION_MAJOR << 16)

namespace Las
{

namespace
{
	using U8 = uint8_t;
	using U16 = uint16_t;
	using U32 = uint32_t;
	using U64 = uint64_t;

	using S8 = int8_t;
	using S16 = int16_t;
	using S32 = int32_t;
	using S64 = int64_t;

	using F32 = float;
	using F64 = double;

	using C8 = char;
}

#pragma pack(push, 1)
struct LasHeader_v1_4
{
	C8 file_signature[4];
	U16 file_source_id;
	U16 global_encoding;
	U32 project_id_guid_data_1;
	U16 project_id_guid_data_2;
	U16 project_id_guid_data_3;
	U8 project_id_guid_data_4[8];
	U8 version_major;
	U8 version_minor;
	C8 system_identifier[32];
	C8 generating_software[32];
	U16 file_creation_day_of_year;
	U16 file_creation_year;
	U16 header_size;
	U32 offset_to_point_data;
	U32 number_of_variable_length_records;
	U8 point_data_record_format;
	U16 point_data_record_length;
	U32 legacy_number_of_point_records;
	U32 legacy_number_of_point_by_return[5];
	F64 x_scale_factor;
	F64 y_scale_factor;
	F64 z_scale_factor;
	F64 x_offset;
	F64 y_offset;
	F64 z_offset;
	F64 max_x;
	F64 max_y;
	F64 max_z;
	F64 min_x;
	F64 min_y;
	F64 min_z;
	U64 start_of_waveform_data_packet_record;
	U64 start_of_first_extended_variable_length_record;
	U32 number_of_extended_variable_length_records;
	U64 number_of_point_records;
	U64 number_of_points_by_return[15];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct LasPointDataRecordFormat0_v1_4
{
	S32 x;
	S32 y;
	S32 z;
	U16 intensity;
	U8 flags;
	U8 classification;
	S8 scan_angle_rank;
	U8 user_data;
	U16 point_source_id;
};
#pragma pack(pop)

struct LasWriter_v1_4
{
	void init();
	void add_point(Unigine::Math::vec3 const& position, float intensity);
	void save(Unigine::StringStack<> const& path);

	LasHeader_v1_4 header;
	Unigine::Vector<LasPointDataRecordFormat0_v1_4> points;

	Unigine::Math::vec3 scale;
	Unigine::Math::vec3 iscale;
	Unigine::Math::BoundBox bbox;
};

#if LAS_VERSION == 0x0001'0004
	using LasHeader = LasHeader_v1_4;
	using LasPointDataRecordFormat0 = LasPointDataRecordFormat0_v1_4;
	using LasWriter = LasWriter_v1_4;
#endif

}
