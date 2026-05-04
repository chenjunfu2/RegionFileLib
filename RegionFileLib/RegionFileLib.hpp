#pragma once

#include <nbt_cpp\NBT_All.hpp>

#include <lz4.h>
#include <charconv>
#include <string>
#include <filesystem>
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


struct RegionPos
{
public:
	int64_t i64X = 0, i64Z = 0;

public:

};


struct ReginInfo
{

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
//	RegionPos posRegion{};
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

	//Chunk ToChunk(void)
	//{
	//
	//}

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
	RegionPos posRegion{};

	struct FileHead//8kb
	{
		uint32_t u32ArrSectorInfo[szChunkCount];
		uint32_t u32ArrChunkTimeStamp[szChunkCount];
	};
	static_assert(sizeof(FileHead) == szPageSize * 2);

	struct ChunkHead
	{
		uint32_t u32ChunkSize;//区块大小（字节数），包含标志位
		uint8_t u8Flags;//标志位
	};
	
	struct ChunkData
	{
		std::vector<uint8_t> vChunkStream;
	};

	struct Chunk
	{
		ChunkHead head;
		ChunkData data;
	};

public:
	FileHead stFileHead;
	union
	{
		Chunk stChunkFlat[szChunkCount];
		Chunk stChunk2D[szChunkCol][szChunkRow];
		static_assert(sizeof(stChunkFlat) == sizeof(stChunk2D));
	};
public:
	static bool ReadRegionFromFile(const std::filesystem::path &pathFile, RegionPos &posRegion, std::vector<uint8_t> &vDataStream)
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
		if (std::from_chars(&strNoExtFileName[0], &strNoExtFileName[szDotPos], posRegion.i64X).ec != std::errc{})
		{
			return false;
		}
		if (std::from_chars(&strNoExtFileName[szDotPos], &strNoExtFileName[strNoExtFileName.size()], posRegion.i64Z).ec != std::errc{})
		{
			return false;
		}

		//流式读取文件
		if (!NBT_IO::ReadFile(pathFile, vDataStream))
		{
			return false;
		}

		return true;
	}

	static bool GetChunkRawFromStream(const RegionPos &posRegion, const std::vector<uint8_t> &vDataStream, const ChunkPos &posChunk, ChunkRaw &rawChunk)
	{





	}



	struct ChunkVisitor
	{
	public:
		enum class Control :uint8_t
		{
			Continue,
			Skip,
			Stop,
		};

	public:
		Control VisitSectorChunkMeta(uint32_t u32ChunkSectorOffset, uint8_t u8ChunkSectorSize, uint32_t u32ChunkTimeStamp)
		{
			//u32ChunkSectorOffset 与 u8ChunkSectorSize 都为0则区块未生成

			return Control::Continue;
		}

		Control VisitChunkMeta(uint32_t u32ChunkSize, uint8_t u8ChunkVersion)
		{
			//u32ChunkSize超过1mb则存储在外部

			return Control::Continue;
		}

		Control VisitChunkStream(std::vector<uint8_t> &&vChunkStream)
		{
			return Control::Continue;
		}
	};




	static bool TraverseChunkRawFromStream(const RegionPos &posRegion, const std::vector<uint8_t> &vDataStream)
	{
		if (vDataStream.size() < sizeof(FileHead))
		{
			return false;
		}

		FileHead *pFileHead = (FileHead *)&vDataStream[0];
		size_t szSectorPageStart = sizeof(FileHead) / szPageSize;








	}



	std::string GetRegionFileName(void)
	{

	}

	//Region ToRegion(void)
	//{
	//
	//}

};



