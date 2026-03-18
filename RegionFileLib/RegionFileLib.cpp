#include <stdint.h>
#include <locale.h>
#include <stdlib.h>


#include <util\Windows_ANSI.hpp>
#include <util\MyAssert.hpp>
#include <util\CodeTimer.hpp>

#include "RegionFileLib.hpp"

//int wmain0(int argc, wchar_t *argv[])
//{
//	if (argc != 2)
//	{
//		return -1;
//	}
//
//	MyAssert(setlocale(LC_ALL, ".UTF-8") != NULL);
//
//	CodeTimer ct;
//
//	NBT_Type::Compound cpd;
//	{
//		std::vector<uint8_t> v;
//		ct.Start();
//		MyAssert(NBT_IO::ReadFile(ConvertUtf16ToUtf8<wchar_t, char>(argv[1]), v));
//		ct.Stop();
//		ct.PrintElapsed("ReadFile Time: [", "]\n");
//
//		{
//			std::vector<uint8_t> t = std::move(v);
//			ct.Start();
//			bool b = NBT_IO::DecompressDataNoThrow(v, t, NBT_Print{ NULL,NULL,NULL });
//			ct.Stop();
//			if (!b)
//			{
//				printf("DataDcp Fail, Maybe No Compress\n");
//				v = std::move(t);
//			}
//			else
//			{
//				ct.PrintElapsed("Decompress Time: [", "]\n");
//			}
//		}
//
//		ct.Start();
//		MyAssert(NBT_Reader::ReadNBT(v, 0, cpd));
//		ct.Stop();
//		ct.PrintElapsed("ReadNBT Time: [", "]\n");
//	}
//	
//	system("pause");
//
//	return 0;
//}








int wmain(int argc, wchar_t *argv[])
{
	if (argc != 2)
	{
		return -1;
	}

	MyAssert(setlocale(LC_ALL, ".UTF-8") != NULL);












	system("pause");

	return 0;
}

