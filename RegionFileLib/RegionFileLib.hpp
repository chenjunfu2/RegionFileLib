#pragma once

#include <nbt_cpp\NBT_All.hpp>

#include <lz4.h>
#include <charconv>
#include <string>
#include <filesystem>
#include <fstream>
//#include <span>


/*
		备注：
		所有数据均为大端序

		总体：
		+----------------+ 0x0000 (0)
		|   文件头 (8KB)  |
		+----------------+ 0x2000 (8192)
		|   区块数据区    |
		|  (4KB扇区对齐)  |
		+----------------+ 文件结尾

		文件头：
		+----------------+ 0x0000 (0)
		|  扇区信息*1024  |
		+----------------+ 0x1000 (4096)
		|   时间戳*1024   |
		+----------------+ 0x2000 (8192)

		扇区信息（区块已生成）：
		+--------------------+
		| bit 31~8 | bit 7~0 |
		+--------------------+
		bit 31~8 : 起始扇区偏移 (offset)，从文件头开始计算，偏移为4kb大小的页面数
		bit 7~0  : 占用扇区大小 (size)，无符号数，最大为255扇区（如果区块溢出为mcc文件，则至少占用1扇区，且存在扇区起始偏移位置）

		扇区信息（区块未生成）：
		+--------------------+
		|    bit 31~0 = 0    |
		+--------------------+
		bit 31~0 : 全为0，区块未生成，跳过存储，但是影响区块Pos计算（完全不占用任何额外扇区，仅元数据中存在0）

		时间戳（秒）：
		+----------------+
		|    bit 31~0    |
		+----------------+
		 bit 31~0 : 四字节有符号数，记录最后修改时间，可能存在溢出为负问题，C++版本改为使用无符号存储

		区块数据（长度在1mb(大于256扇区)内）：
		+-----------------+ +0 (扇区起始(offset * 4096))
		|   长度 (4字节)   |
		+-----------------+ +4
		|   标志 (1字节)   |
		+-----------------+ +5 (头部结束，数据起始)
		| 压缩后的NBT数据  |
		+-----------------+ +n (数据结束位置)
		| 空闲空间(填充零) |
		+-----------------+ +x (4KB边界)(下一个扇区起始)((offset * 4096)+(size * 4096))
		长度：不包含头部4字节，但是包含标志1字节，有符号数，但是因为不可大于1mb所以符号无所谓

		标志：
		+-----------------+
		| bit 7 | bit 6~0 |
		+-----------------+
		bit 7   : bool值，为[1]则存储在外部MCC文件，为[0]则存储在当前MCA文件
		bit 6~0 : 压缩格式枚举，值与压缩类型关系 [1:GZIP] [2:ZLIB] [3:UNCOMPRESSED] [4:LZ4]


		区块数据（长度超过1mb(大于256扇区)的区块使用额外区块数据MCC文件存储）：
		+-----------------+ +0 (扇区起始 (offset * 4096))
		|   长度 (4字节)   |
		+-----------------+ +4
		|   标志 (1字节)   |
		+-----------------+ +5 (头部结束)
		| 空闲空间(填充零) |
		+-----------------+ +4095 (下一个扇区起始)((offset * 4096)+(1 * 4096))
		长度固定为：1 （仅包含1字节标志，剩余区块长度由mcc文件长度决定）
		额外区块信息在MCA文件中至少占用1扇区（仅用于存储文件头）
		额外区块数据存储在MCC文件中

		额外区块数据MCC文件：
		+----------------+ 0
		| 压缩后的NBT数据 |
		+----------------+ n
		不进行4KB边界对齐，具体信息在区块数据中


		文件名含义：
		MCA : r.<regionX>.<regionZ>.mca
		MCC : c.<chunkX>.<chunkZ>.mcc

		世界坐标 -> 区块坐标 : 除以16：
		chunkX = blockX >> 4;
		chunkZ = blockZ >> 4;

		区块坐标 -> 区域坐标 : 除以32：
		regionX = chunkX >> 5;
		regionZ = chunkZ >> 5;

		区域内相对坐标 : 取模32：
		regionRelativeX = chunkX & 31;
		regionRelativeZ = chunkZ & 31;

		坐标打包：
		int32_t chunkX, chunkZ;
		uint64_t packetChunkPos;
		packetChunkPos = (chunkX & 0xFFFFFFFFL << 0) | ((chunkZ & 0xFFFFFFFFL) << 32);

		扇区数量：
		sectorCount = (byteCount + 4096 - 1) / 4096;//向上取整
		*/


struct ChunkPos
{
public:
	int64_t i64X = 0, i64Z = 0;

public:

};


//struct Chunk
//{
//public:
//	ChunkPos posChunk{};
//	uint32_t u32Timestamp{};
//	NBT_Type::Compound cpdChunkData{};
//
//public:
//
//};
//
//struct Region : public ReginInfo
//{
//public:
//	RegionPos stRegionPos{};
//	Chunk chunkData[REGION_CHUNK_COL][REGION_CHUNK_ROW]{};
//
//public:
//
//
//};

struct ChunkRaw
{
public:
	constexpr static inline const std::string_view strChunkFileExtern = ".mcc";
	constexpr static inline const std::string_view strChunkFileStart = "c.";

	enum class CompressType : uint8_t
	{
		UNKNOWN = 0,
		GZIP = 1,
		ZLIB = 2,
		UNCOMPRESSED = 3,
		LZ4 = 4,
		ENUM_END,
	};

public:
	ChunkPos posChunk{};
	uint32_t u32Timestamp = 0;
	CompressType enCompressType = CompressType::UNKNOWN;
	bool bExternalStorage = false;
	std::vector<uint8_t> streamChunkData{};

public:
	static bool ReadChunkFromFile(const std::string &pFile)
	{

	}

	std::string GetChunkFileName(void)
	{

	}

};


//32*32区块=1区域
struct RegionRaw
{
public:
	constexpr static inline std::string_view strRegionFileExtern = ".mca";
	constexpr static inline std::string_view strRegionFileStart = "r.";
	constexpr static inline size_t szPageSize = 4096;

	constexpr static inline size_t szChunkCol = 32;//行(y)
	constexpr static inline size_t szChunkRow = 32;//列(x)
	constexpr static inline size_t szChunkCount = szChunkCol * szChunkRow;//总数

public:
	struct RegionPos
	{
		int64_t i64X{};
		int64_t i64Z{};
	};

	struct FileHead//8kb
	{
		uint32_t u32ArrSectorInfo[szChunkCount]{};
		uint32_t u32ArrChunkTimeStamp[szChunkCount]{};
	};
	static_assert(sizeof(FileHead) == szPageSize * 2);

	struct ChunkHead
	{
		uint32_t u32ChunkSize{};//区块大小（字节数），不包含标志位
		uint8_t u8ChunkFlags{};//标志位
	};
	
	struct ChunkData
	{
		std::vector<uint8_t> vChunkStream{};
	};

	struct Chunk
	{
		ChunkHead head{};
		ChunkData data{};
	};

public:
	RegionPos stRegionPos;
	FileHead stFileHead;
	union
	{
		Chunk stChunkFlat[szChunkCount];
		Chunk stChunk2D[szChunkCol][szChunkRow];
	};
	static_assert(sizeof(stChunkFlat) == sizeof(stChunk2D));

public:
	bool ReadRegionFromFile(const std::filesystem::path &pathFile)
	{
		//读取mca文件并确认是否有mcc文件，有则自动打开并读取，否则标注

		//确认文件后缀
		if (pathFile.extension().string() != strRegionFileExtern)
		{
			return false;
		}

		//确认文件前缀
		std::string strNoExtFileName = pathFile.stem().string();
		if (!strNoExtFileName.starts_with(strRegionFileStart))
		{
			return false;
		}

		//解析区域坐标
		strNoExtFileName.erase(0, strRegionFileStart.size());//从头部删掉前缀

		//查找中部的分隔符
		size_t szDotPos = strNoExtFileName.find('.');
		if (szDotPos == strNoExtFileName.npos)
		{
			return false;
		}

		
		//解析坐标
		if (std::from_chars(&strNoExtFileName[0], &strNoExtFileName[szDotPos], stRegionPos.i64X).ec != std::errc{})
		{
			return false;
		}
		if (std::from_chars(&strNoExtFileName[szDotPos], &strNoExtFileName[strNoExtFileName.size()], stRegionPos.i64Z).ec != std::errc{})
		{
			return false;
		}

		//读取文件头（大小端转换）
		std::fstream fRead;
		fRead.open(pathFile, std::ios_base::in | std::ios_base::binary);
		if (!fRead)
		{
			return false;
		}

		//lambda
		auto SimpleRead =
		[&fRead](void *pData, size_t szReadSize) -> bool
		{
			if (fRead.read((char *)pData, szReadSize).gcount() != szReadSize)
			{
				return false;
			}

			return true;
		};

		auto SimpleJump = 
		[&fRead](size_t szJumpPos) -> void
		{
			fRead.seekg(szJumpPos, std::ios_base::beg);
		};

		//读取扇区信息
		if (!SimpleRead(stFileHead.u32ArrSectorInfo, sizeof(stFileHead.u32ArrSectorInfo)))
		{
			return false;
		}
		if (!SimpleRead(stFileHead.u32ArrChunkTimeStamp, sizeof(stFileHead.u32ArrChunkTimeStamp)))
		{
			return false;
		}
		
		//字节序转换
		for (size_t i = 0; i < szChunkCount; ++i)
		{
			stFileHead.u32ArrSectorInfo[i] = NBT_Endian::BigToNativeAny(stFileHead.u32ArrSectorInfo[i]);
			stFileHead.u32ArrChunkTimeStamp[i] = NBT_Endian::BigToNativeAny(stFileHead.u32ArrChunkTimeStamp[i]);
		}
		

		//根据扇区信息，读取区块
		for (size_t i = 0; i < szChunkCount; ++i)
		{
			//使用page size相乘获取实际大小和偏移
			uint64_t u64SectorOffset = (uint64_t)((stFileHead.u32ArrSectorInfo[i] >> 8) & (uint32_t)0x00'FF'FF'FF) * (uint64_t)szPageSize;
			uint32_t u32SectorSize = (uint32_t)(stFileHead.u32ArrSectorInfo[i] & (uint32_t)0x00'00'00'FF) * (uint32_t)szPageSize;

			//跳转到指定起始位置
			SimpleJump(u64SectorOffset);

			//读取4字节长度
			uint32_t u32ChunkSize{};
			if (!SimpleRead(&u32ChunkSize, sizeof(u32ChunkSize)))
			{
				return false;
			}
			u32ChunkSize = NBT_Endian::BigToNativeAny(u32ChunkSize);

			//合法性判断 
			if (u32ChunkSize > u32SectorSize ||//区块大小大于扇区数据大小
				u32ChunkSize == 0)//u32ChunkSize至少包含一个标志位1字节大小，不可为0
			{
				return false;
			}

			//读取区块数据
			//首先读1字节标志位（因为4字节长度包含标志位长度，读取后需要减少）
			uint8_t u8ChunkFlags{};
			if (!SimpleRead(&u8ChunkFlags, sizeof(u8ChunkFlags)))
			{
				return false;
			}
			//注意1字节没有字节序问题，无需转换

			//剩下的就是区块数据长度（不包含标志位）
			--u32ChunkSize;

			//保存一下
			stChunkFlat[i].head.u32ChunkSize = u32ChunkSize;
			stChunkFlat[i].head.u8ChunkFlags = u8ChunkFlags;

			//判断标志位，确定区块是否存储在当前mca文件中
			if ((u8ChunkFlags & 0b1000'0000) == 0b1000'0000)//高位为1，存储在外部
			{
				//TODO: Read MCC
			}
			else
			{
				//直接读取，暂时不考虑压缩相关问题
				stChunkFlat[i].data.vChunkStream.reserve(u32ChunkSize);
				if (!SimpleRead(stChunkFlat[i].data.vChunkStream.data(), sizeof(uint8_t) * u32ChunkSize))
				{
					return false;
				}
			}
		}

		return true;
	}

	std::string GetRegionFileName(void)
	{
		return std::format("{}{}.{}{}", strRegionFileStart, stRegionPos.i64X, stRegionPos.i64Z, strRegionFileExtern);
	}
};



