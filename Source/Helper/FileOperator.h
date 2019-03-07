#pragma once
#include <BaseInclude.h>
#include <../3rdParty/cereal/types/memory.hpp>
#include <../3rdParty/cereal/archives/xml.hpp>
#include <fstream>

namespace Core
{
	class FileOperator
	{
	public:
		FileOperator();
		static void * MapMemory(const ANSICHAR * fileFullPathName, Bool binaryFile = False);
		static void UnmapMemory(void * pBytes);

		//	���л�,д��
		template <class T>
		static void SerializeXML(const ANSICHAR * fullPathName, T * t)
		{
			std::ofstream os(fullPathName, std::ios::out);
			cereal::XMLOutputArchive archive(os);

			archive(*t);
		}

		//	���л�,д��[����1Ϊ����ָ��]
		template <class T>
		static void SerializeXML(const ANSICHAR * fullPathName, T t)
		{
			std::ofstream os(fullPathName, std::ios::out);
			cereal::XMLOutputArchive archive(os);

			archive(*t.get());
		}

		//	�����л�,��ȡ
		template <class T>
		static void DeserializeXML(const ANSICHAR * fullPathName, T * t)
		{
			std::ifstream is(fullPathName, std::ios::in);

			if (is.is_open())
			{
				cereal::XMLInputArchive inArchive(is);

				inArchive(*t);

				t->fullPathName = fullPathName;
			}
		}

		//	�����л�,��ȡ[����1Ϊ����ָ��]
		template <class T>
		static void DeserializeXML(const ANSICHAR * fullPathName, T t)
		{
			std::ifstream is(fullPathName, std::ios::in);

			if (is.is_open())
			{
				cereal::XMLInputArchive inArchive(is);

				inArchive(*t.get());

				t->fullPathName = fullPathName;
			}
		}

		~FileOperator();
	};
}