#pragma once

#include <nbt_cpp\NBT_All.hpp>


/*
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

		扇区信息：
		+--------------------+
		| bit 31~8 | bit 7~0 |
		+--------------------+
		bit 31~8 : 起始扇区偏移 (offset)，从文件开始计算
		bit 7~0  : 占用扇区大小 (size)，无符号数，最大为255扇区

		扇区信息（区块未生成）：
		+--------------------+
		|    bit 31~0 = 0    |
		+--------------------+
		bit 31~0 : 全为0，区块未生成，跳过存储，但是影响区块Pos计算

		时间戳（秒）：
		+----------------+
		|    bit 31~0    |
		+----------------+
		 bit 31~0 : 四字节有符号数，记录最后修改时间，可能存在溢出为负问题，C++版本改为使用无符号存储

		区块数据：
		+-----------------+ +0 (扇区起始(offset * 4096))
		|   长度 (4字节)   |
		+-----------------+ +4
		|   版本 (1字节)   |
		+-----------------+ +5 (头部结束，数据起始)
		| 压缩后的NBT数据  |
		+-----------------+ +n (数据结束位置)
		| 空闲空间(填充零) |
		+-----------------+ +x (4KB边界)(下一个扇区起始)((offset * 4096)+(size * 4096))
		长度：不包含头部5字节

		版本：
		+-----------------+
		| bit 7 | bit 6~0 |
		+-----------------+
		bit 7   : bool值，为[1]则存储在外部MCC文件，为[0]则存储在当前MCA文件
		bit 6~0 : 压缩格式枚举，值与压缩类型关系 [1:GZIP] [2:ZLIB] [3:UNCOMPRESSED] [4:LZ4]


		区块数据（区块过大使用额外区块数据MCC文件存储）：
		+-----------------+ +0 (扇区起始 (offset * 4096))
		|   长度 (4字节)   |
		+-----------------+ +4
		|   版本 (1字节)   |
		+-----------------+ +5 (头部结束)
		| 空闲空间(填充零) |
		+-----------------+ +4095 (下一个扇区起始)((offset * 4096)+(1 * 4096))
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
	int64_t i64X = 0, i64Y = 0;

public:

};


struct RegionPos
{
public:
	int64_t i64X = 0, i64Y = 0;

public:

};


struct ReginInfo
{
	constexpr static inline size_t REGION_CHUNK_COL = 32;//行(y)
	constexpr static inline size_t REGION_CHUNK_ROW = 32;//列(x)
	constexpr static inline size_t REGION_CHUNK_COUNT = REGION_CHUNK_COL * REGION_CHUNK_ROW;//总数
};


struct Chunk
{
public:
	ChunkPos posChunk{};
	uint32_t u32Timestamp{};
	NBT_Type::Compound cpdChunkData{};

public:

};

struct Region : public ReginInfo
{
public:
	RegionPos posRegion{};
	Chunk chunkData[REGION_CHUNK_COL][REGION_CHUNK_ROW]{};



};

struct ChunkRaw
{
public:
	constexpr static inline const char *const pChunkFileExtern = ".mcc";

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
	static bool ReadChunkFile(const std::string &pFile, std::vector<uint8_t> &fStream)
	{

	}

	bool ReadChunkStream(const std::vector<uint8_t> &fStream)
	{

	}

	std::string GetChunkFileName(void)
	{

	}

	Chunk ToChunk(void)
	{

	}

};


//32*32区块=1区域
struct RegionRaw : public ReginInfo
{
public:
	constexpr static inline const char *const pRegionFileExtern = ".mca";

public:
	RegionPos posRegion{};
	ChunkRaw rawChunk[REGION_CHUNK_COL][REGION_CHUNK_ROW]{};

public:
	static bool ReadRegionFile(const std::string &pFile, std::vector<uint8_t> &fStream)
	{
		return NBT_IO::ReadFile(pFile, fStream);
	}

	bool ReadRegionStream(const std::vector<uint8_t> &fStream)
	{







	}

	std::string GetRegionFileName(void)
	{

	}

	std::vector<std::filesystem::path> GetExternChunkFileNameList(void)
	{




	}

	Region ToRegion(void)
	{

	}

};



