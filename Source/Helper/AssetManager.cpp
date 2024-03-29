#include "AssetManager.h"

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif
#include "FileOperator.h"
#include "MeshImportor.h"
#include "TextureOperator.h"
#include "../3rdParty/cereal/archives/xml.hpp"
#include "FBXImportor.h"

using namespace ctd;
using namespace std;

namespace Core
{
	void AssetManager::processScene(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<Scene> sceneRef = sceneMap[fileName];
		
		if (sceneRef.expired())
		{
			sceneMap[fileName] = make_shared<Scene>();
			sceneRef = sceneMap[fileName];
		}
		
		sceneRef.lock()->fullPathName = fileFullPathName;
		sceneRef.lock()->fileName = fileName;
		sceneRef.lock()->fileNameWithExt = fileName;
		sceneRef.lock()->fileNameWithExt += extension;
		sceneRef.lock()->type = FileType_Scene;

		FileOperator::DeserializeXML(fileFullPathName, sceneRef.lock().get());
		sceneRef.lock()->AfterLoad();
	}

	void AssetManager::processPrefab(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<Prefab> prefabRef = prefabMap[fileName];

		if (prefabRef.expired())
		{
			prefabMap[fileName] = make_shared<Prefab>();
			prefabRef = prefabMap[fileName];
		}

		prefabRef.lock()->fullPathName = fileFullPathName;
		prefabRef.lock()->fileName = fileName;
		prefabRef.lock()->fileNameWithExt = fileName;
		prefabRef.lock()->fileNameWithExt += extension;
		prefabRef.lock()->type = FileType_Prefab;

		FileOperator::DeserializeXML(fileFullPathName, prefabRef.lock().get());
		prefabRef.lock()->AfterLoad();
	}

	void AssetManager::processGLShader(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		GLShaderType shaderType;

		if (strcmp(extension, glVertexShaderExt) == 0)
			shaderType = GLShaderType_VertexShader;
		else if (strcmp(extension, glFragmentShaderExt) == 0)
			shaderType = GLShaderType_FragmentShader;
		else
			assert(False);

		weak_ptr<GLSL> shaderRef;

		switch (shaderType)
		{
		case GLShaderType_VertexShader:
			shaderRef = glVertexShaderMap[fileName];
			break;
		case GLShaderType_FragmentShader:
			shaderRef = glFragmentShaderMap[fileName];
			break;
		default:
			break;
		}

		if (shaderRef.expired())
		{
			switch (shaderType)
			{
			case GLShaderType_VertexShader:
				glVertexShaderMap[fileName] = make_shared<GLSL>();
				shaderRef = glVertexShaderMap[fileName];
				break;
			case GLShaderType_FragmentShader:
				glFragmentShaderMap[fileName] = make_shared<GLSL>();
				shaderRef = glFragmentShaderMap[fileName];
				break;
			default:
				break;
			}
		}

		shaderRef.lock()->fullPathName = fileFullPathName;
		shaderRef.lock()->fileName = fileName;
		shaderRef.lock()->fileNameWithExt = fileName;
		shaderRef.lock()->fileNameWithExt += extension;

		if (shaderType == GLShaderType_VertexShader)
			shaderRef.lock()->type = FileType_GLSL_Vertex;
		else if (shaderType == GLShaderType_FragmentShader)
			shaderRef.lock()->type = FileType_GLSL_Fragment;
	}

	void AssetManager::processRLShader(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		RLShaderType shaderType;

		if (strcmp(extension, rlVertexShaderExt) == 0)
			shaderType = RLShaderType_VertexShader;
		else if (strcmp(extension, rlRayShaderExt) == 0)
			shaderType = RLShaderType_RayShader;
		else if (strcmp(extension, rlFrameShaderExt) == 0)
			shaderType = RLShaderType_FrameShader;
		else
			assert(False);

		weak_ptr<RLSL> shaderRef;

		switch (shaderType)
		{
		case RLShaderType_VertexShader:
			shaderRef = rlVertexShaderMap[fileName];
			break;
		case RLShaderType_RayShader:
			shaderRef = rlRayShaderMap[fileName];
			break;
		case RLShaderType_FrameShader:
			shaderRef = rlFrameShaderMap[fileName];
			break;
		default:
			break;
		}

		if (shaderRef.expired())
		{
			switch (shaderType)
			{
			case RLShaderType_VertexShader:
				rlVertexShaderMap[fileName] = make_shared<RLSL>();
				shaderRef = rlVertexShaderMap[fileName];
				break;
			case RLShaderType_RayShader:
				rlRayShaderMap[fileName] = make_shared<RLSL>();
				shaderRef = rlRayShaderMap[fileName];
				break;
			case RLShaderType_FrameShader:
				rlFrameShaderMap[fileName] = make_shared<RLSL>();
				shaderRef = rlFrameShaderMap[fileName];
				break;
			default:
				break;
			}
		}

		shaderRef.lock()->fullPathName = fileFullPathName;
		shaderRef.lock()->fileName = fileName;
		shaderRef.lock()->fileNameWithExt = fileName;
		shaderRef.lock()->fileNameWithExt += extension;

		if (shaderType == RLShaderType_VertexShader)
			shaderRef.lock()->type = FileType_RLSL_Vertex;
		else if (shaderType == RLShaderType_RayShader)
			shaderRef.lock()->type = FileType_RLSL_Ray;
		else if (shaderType == RLShaderType_FrameShader)
			shaderRef.lock()->type = FileType_RLSL_Frame;
	}

	void AssetManager::processMesh(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<StaticMesh> staticMeshRef = meshMap[fileName];

		if (staticMeshRef.expired())
		{
			meshMap[fileName] = make_shared<StaticMesh>();
			staticMeshRef = meshMap[fileName];
		}

		staticMeshRef.lock()->fullPathName = fileFullPathName;
		staticMeshRef.lock()->fileName = fileName;
		staticMeshRef.lock()->fileNameWithExt = fileName;
		staticMeshRef.lock()->fileNameWithExt += extension;
		staticMeshRef.lock()->type = FileType_StaticMesh;
	}

	void AssetManager::processTextureInfo(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<TextureInfo> textureInfoRef = textureInfoMap[fileName];

		if (textureInfoRef.expired())
		{
			textureInfoMap[fileName] = make_shared<TextureInfo>();
			textureInfoRef = textureInfoMap[fileName];
		}

		textureInfoRef.lock()->fullPathName = fileFullPathName;
		textureInfoRef.lock()->fileName = fileName;
		textureInfoRef.lock()->fileNameWithExt = fileName;
		textureInfoRef.lock()->fileNameWithExt += extension;
		textureInfoRef.lock()->type = FileType_TextureInfo;

		FileOperator::DeserializeXML(fileFullPathName, textureInfoRef.lock().get());
		textureInfoRef.lock()->AfterLoad();
	}

	void AssetManager::processTexture(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		if (textureInfoMap.find(fileName) == textureInfoMap.end())
		{
			textureInfoMap[fileName] = make_shared<TextureInfo>();
			weak_ptr<TextureInfo> textureInfoRef = textureInfoMap[fileName];

			ctd::string textureInfoFullPathName = textureFullPath;
			textureInfoFullPathName.append("\\");
			textureInfoFullPathName.append(fileName);
			textureInfoFullPathName.append(textureInfoExt);

			textureInfoRef.lock()->format = TextureFormat_SRGBA32;
			textureInfoRef.lock()->wrapMode = TextureWrapMode_Clamp;
			textureInfoRef.lock()->filterMode = TextureFilterMode_Bilinear;
			textureInfoRef.lock()->fullPathName = textureInfoFullPathName;
			textureInfoRef.lock()->fileName = fileName;
			textureInfoRef.lock()->fileNameWithExt = fileName;
			textureInfoRef.lock()->fileNameWithExt += textureInfoExt;
			textureInfoRef.lock()->type = FileType_TextureInfo;
		}

		//////////////////////////////////////////////////////////////////////////

		weak_ptr<Texture> textureRef = textureMap[fileName];

		if (textureRef.expired())
		{
			textureMap[fileName] = make_shared<Texture>();
			textureRef = textureMap[fileName];
		}

		textureRef.lock()->info = textureInfoMap[fileName];
		textureRef.lock()->fullPathName = fileFullPathName;
		textureRef.lock()->fileName = fileName;
		textureRef.lock()->fileNameWithExt = fileName;
		textureRef.lock()->fileNameWithExt += extension;
		textureRef.lock()->type = FileType_Texture;
	}

	void AssetManager::processLightmap(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		if (textureInfoMap.find(fileName) == textureInfoMap.end())
		{
			textureInfoMap[fileName] = make_shared<TextureInfo>();
			weak_ptr<TextureInfo> textureInfoRef = textureInfoMap[fileName];

			ctd::string textureInfoFullPathName = lightmapFullPath;
			textureInfoFullPathName.append("\\");
			textureInfoFullPathName.append(fileName);
			textureInfoFullPathName.append(textureInfoExt);

			textureInfoRef.lock()->format = TextureFormat_RGBA32;
			textureInfoRef.lock()->wrapMode = TextureWrapMode_Clamp;
			textureInfoRef.lock()->filterMode = TextureFilterMode_Bilinear;
			textureInfoRef.lock()->fullPathName = textureInfoFullPathName;
			textureInfoRef.lock()->fileName = fileName;
			textureInfoRef.lock()->fileNameWithExt = fileName;
			textureInfoRef.lock()->fileNameWithExt += textureInfoExt;
			textureInfoRef.lock()->type = FileType_TextureInfo;
		}

		//////////////////////////////////////////////////////////////////////////

		weak_ptr<Texture> lightmapRef = lightmapMap[fileName];

		if (lightmapRef.expired())
		{
			lightmapMap[fileName] = make_shared<Texture>();
			lightmapRef = lightmapMap[fileName];
		}

		lightmapRef.lock()->info = textureInfoMap[fileName];
		lightmapRef.lock()->fullPathName = fileFullPathName;
		lightmapRef.lock()->fileName = fileName;
		lightmapRef.lock()->fileNameWithExt = fileName;
		lightmapRef.lock()->fileNameWithExt += extension;
		lightmapRef.lock()->type = FileType_Texture;
	}

	void AssetManager::processMaskMap(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<Texture> maskMapRef = maskMapMap[fileName];

		if (maskMapRef.expired())
		{
			maskMapMap[fileName] = make_shared<Texture>();
			maskMapRef = maskMapMap[fileName];
		}

		maskMapRef.lock()->info = std::make_shared<TextureInfo>();
		maskMapRef.lock()->info->format = TextureFormat_RGBA32;
		maskMapRef.lock()->info->wrapMode = TextureWrapMode_Clamp;
		maskMapRef.lock()->info->filterMode = TextureFilterMode_Point;
		maskMapRef.lock()->fullPathName = fileFullPathName;
		maskMapRef.lock()->fileName = fileName;
		maskMapRef.lock()->fileNameWithExt = fileName;
		maskMapRef.lock()->fileNameWithExt += extension;
		maskMapRef.lock()->type = FileType_Texture;
	}

	void AssetManager::processMaterial(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<Material> materialRef = materialMap[fileName];

		if (materialRef.expired())
		{
			materialMap[fileName] = make_shared<Material>();
			materialRef = materialMap[fileName];
		}

		materialRef.lock().get()->fullPathName = fileFullPathName;
		materialRef.lock().get()->fileName = fileName;
		materialRef.lock().get()->fileNameWithExt = fileName;
		materialRef.lock().get()->fileNameWithExt += extension;
		materialRef.lock()->type = FileType_Material;

		//	TODO:	
		//	材质是目前唯一一个BeginUse之前加载入内存的资源
		//	这么做是因为假如不这样,代码就太难写了.
		FileOperator::DeserializeXML(fileFullPathName, materialRef.lock().get());
		materialRef.lock()->AfterLoad();

		if (glVertexShaderMap.find(materialRef.lock()->glVertexShaderName) != glVertexShaderMap.end())
		{
			materialRef.lock()->glVertexShader = glVertexShaderMap[materialRef.lock()->glVertexShaderName];
			materialRef.lock()->glVertexShader.lock()->Attach(materialMap[fileName].get());
		}

		if (glFragmentShaderMap.find(materialRef.lock()->glFragmentShaderName) != glFragmentShaderMap.end())
		{
			materialRef.lock()->glFragmentShader = glFragmentShaderMap[materialRef.lock()->glFragmentShaderName];
			materialRef.lock()->glFragmentShader.lock()->Attach(materialMap[fileName].get());
		}

		if (rlVertexShaderMap.find(materialRef.lock()->rlVertexShaderName) != rlVertexShaderMap.end())
		{
			materialRef.lock()->rlVertexShader = rlVertexShaderMap[materialRef.lock()->rlVertexShaderName];
		}

		if (rlRayShaderMap.find(materialRef.lock()->rlRayShaderName) != rlRayShaderMap.end())
		{
			materialRef.lock()->rlRayShader = rlRayShaderMap[materialRef.lock()->rlRayShaderName];
		}

		if (textureMap.find(materialRef.lock()->albedoTextureName) != textureMap.end())
			materialRef.lock()->albedoTexture = textureMap[materialRef.lock()->albedoTextureName];
		
		if (textureMap.find(materialRef.lock()->normalMapName) != textureMap.end())
			materialRef.lock()->normalMap = textureMap[materialRef.lock()->normalMapName];

		if (textureMap.find(materialRef.lock()->metallicTextureName) != textureMap.end())
			materialRef.lock()->metallicTexture = textureMap[materialRef.lock()->metallicTextureName];

		if (textureMap.find(materialRef.lock()->roughnessTextureName) != textureMap.end())
			materialRef.lock()->roughnessTexture = textureMap[materialRef.lock()->roughnessTextureName];

		if (textureMap.find(materialRef.lock()->aoTextureName) != textureMap.end())
			materialRef.lock()->aoTexture = textureMap[materialRef.lock()->aoTextureName];

		if (textureMap.find(materialRef.lock()->lightmapName) != textureMap.end())
			materialRef.lock()->lightmapTexture = textureMap[materialRef.lock()->lightmapName];
	}

	void AssetManager::processFBX(const ANSICHAR * extension, const ANSICHAR * fileName, const ANSICHAR * fileFullPathName)
	{
		weak_ptr<FBX> fbxRef = fbxMap[fileName];

		if (fbxRef.expired())
		{
			fbxMap[fileName] = make_shared<FBX>();
			fbxRef = fbxMap[fileName];
		}

		fbxRef.lock()->fullPathName = fileFullPathName;
		fbxRef.lock()->fileName = fileName;
		fbxRef.lock()->fileNameWithExt = fileName;
		fbxRef.lock()->fileNameWithExt += extension;
		fbxRef.lock()->type = FileType_FBX;

		ctd::vector<std::unique_ptr<StaticMesh>> staticMeshes;
		ctd::vector<std::unique_ptr<Material>> materials;

		FBXImportor::Instance()->LoadFBX(fileFullPathName, staticMeshes, materials, True);

		vector<ctd::string> meshNames;
		vector<ctd::string> materialNames;

		for (int32 i = 0; i < staticMeshes.size(); ++i)
		{
			ctd::string meshName = staticMeshes[i]->fileName;

			weak_ptr<StaticMesh> staticMeshRef = meshMap[meshName];

			if (staticMeshRef.expired())
			{
				meshMap[meshName] = std::move(staticMeshes[i]);
				staticMeshRef = meshMap[meshName];
			}

			staticMeshRef.lock()->fullPathName = meshFullPath;
			staticMeshRef.lock()->fullPathName += "\\" + staticMeshRef.lock()->fileName + meshExt;
			staticMeshRef.lock()->fileNameWithExt = meshName;
			staticMeshRef.lock()->fileNameWithExt += meshExt;
			staticMeshRef.lock()->type = FileType_StaticMesh;

			meshNames.push_back(meshName);
		}
		
		for (int32 i = 0; i < materials.size(); ++i)
		{
			ctd::string materialName = materials[i]->fileName;

			weak_ptr<Material> materialRef = materialMap[materialName];

			if (materialRef.expired())
			{
				materialMap[materialName] = std::move(materials[i]);
				materialRef = materialMap[materialName];
			}

			materialRef.lock()->fullPathName = materialFullPath;
			materialRef.lock()->fullPathName += "\\" + materialRef.lock()->fileName + materialExt;
			materialRef.lock()->fileNameWithExt = materialName;
			materialRef.lock()->fileNameWithExt += materialExt;
			materialRef.lock()->type = FileType_Material;

			materialRef.lock()->glVertexShaderName = defaultGLVertexShaderName;
			materialRef.lock()->glVertexShader = glVertexShaderMap[defaultGLVertexShaderName];
			materialRef.lock()->glVertexShader.lock()->Attach(materialMap[materialName].get());

			materialRef.lock()->glFragmentShaderName = defaultGLFragmentShaderName;
			materialRef.lock()->glFragmentShader = glFragmentShaderMap[defaultGLFragmentShaderName];
			materialRef.lock()->glFragmentShader.lock()->Attach(materialMap[materialName].get());

			materialRef.lock()->rlVertexShaderName = defaultRLVertexShaderName;
			materialRef.lock()->rlVertexShader = rlVertexShaderMap[defaultRLVertexShaderName];

			materialRef.lock()->rlRayShaderName = defaultRLRayShaderName;
			materialRef.lock()->rlRayShader = rlRayShaderMap[defaultRLRayShaderName];

			materialRef.lock()->albedoTextureName = defaultAlbedoTextureName;
			materialRef.lock()->albedoTexture = textureMap[defaultAlbedoTextureName];

			materialNames.push_back(materialName);
		}
		
		//	创建prefab
		for (int32 i = 0; i < meshNames.size(); ++i)
		{
			ctd::string prefabName = meshNames[i];

			std::weak_ptr<Prefab> prefabRef = prefabMap[prefabName];

			if (prefabRef.expired())
			{
				CreatePrefab(prefabName.c_str());
				prefabRef = prefabMap[prefabName];
			}

			prefabRef.lock()->staticMeshName = meshNames[i];
			prefabRef.lock()->materialName = materialNames[i];
		}
	}

	void AssetManager::processFolder(AssetType assetType, const ANSICHAR * folderFullPathName, const ANSICHAR * extension0, ...)
	{
		string pattern(folderFullPathName);
		pattern.append("\\*");

		WIN32_FIND_DATA data;

		HANDLE hFind;

		if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				string fileName = data.cFileName;

				size_t pos = fileName.find_last_of('.');

				string extension = fileName.substr(pos, fileName.length() - 1);
				transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				
				string fileNameWithoutExt = fileName.substr(0, pos);

				va_list args;
				va_start(args, extension0);
				const ANSICHAR * extensionCursor;

				extensionCursor = extension0;

				while (extensionCursor)
				{
					if (extension == extensionCursor)
					{
						string fullPathName = folderFullPathName;
						fullPathName.append("\\" + fileName);

						switch (assetType)
						{
						case AssetType_Scene:
							processScene(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_Prefab:
							processPrefab(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_GLShader:
							processGLShader(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_RLShader:
							processRLShader(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_StaticMesh:
							processMesh(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_TextureInfo:
							processTextureInfo(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_Texture:
							processTexture(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_Lightmap:
							processLightmap(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_Material:
							processMaterial(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						case AssetType_FBX:
							processFBX(extension.c_str(), fileNameWithoutExt.c_str(), fullPathName.c_str());
							break;
						}

						break;
					}

					extensionCursor = va_arg(args, const ANSICHAR *);
				}

				va_end(args);
			}
			while (FindNextFile(hFind, &data) != 0);

			FindClose(hFind);
		}
	}

	AssetManager::AssetManager()
	{
	}

	void AssetManager::ScanLightmap()
	{
#ifdef PLATFORM_WINDOWS
		processFolder(AssetType_Lightmap, lightmapFullPath, textureTGAExt, texturePNGExt, Null);
#endif
	}

	void AssetManager::Scan()
	{
		//	TODO:	添加新平台编辑器支持时再更改此方法.哪年哪月?
		//			2018/4/11-15:30 北京七棵树创意园b2-2北门.
#ifdef PLATFORM_WINDOWS

		processFolder(AssetType_Scene, sceneFullPath, sceneExt, Null);

		processFolder(AssetType_Prefab, prefabFullPath, prefabExt, Null);

		processFolder(AssetType_GLShader, glShaderFullPath, glVertexShaderExt, glFragmentShaderExt, Null);

		processFolder(AssetType_RLShader, rlShaderFullPath, rlVertexShaderExt, rlRayShaderExt, rlFrameShaderExt, Null);

		processFolder(AssetType_TextureInfo, textureFullPath, textureInfoExt, Null);

		processFolder(AssetType_Texture, textureFullPath, textureTGAExt, texturePNGExt, textureTIFFExt, Null);
		
		processFolder(AssetType_TextureInfo, lightmapFullPath, textureInfoExt, Null);

		processFolder(AssetType_Lightmap, lightmapFullPath, textureTGAExt, texturePNGExt, Null);

		//	FBX的处理要先于mesh和material, 因为加载FBX之后mesh和material会被添加
		//	FBX的处理要后于shader,因为加载FBX之后会处理material
		processFolder(AssetType_FBX, fbxFullPath, fbxExt, Null);

		processFolder(AssetType_Material, materialFullPath, materialExt, Null);
#endif
	}

	void AssetManager::SaveMaterialToDisk()
	{
		for (ctd::map<ctd::string, std::shared_ptr<Material>>::iterator iter = materialMap.begin();
			iter != materialMap.end();
			++iter)
		{
			if (iter->second->fullPathName == "")
			{
				string fullPathName = materialFullPath;
				fullPathName += "\\" + iter->first + materialExt;

				FileOperator::SerializeXML(fullPathName.c_str(), iter->second.get());
			}
			else
				FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}
	}

	void AssetManager::ReloadGLShader()
	{
		for (ctd::map<ctd::string, std::shared_ptr<GLSL>>::iterator iter = glVertexShaderMap.begin();
			iter != glVertexShaderMap.end();
			++iter)
		{
			iter->second->Reload();
		}

		for (ctd::map<ctd::string, std::shared_ptr<GLSL>>::iterator iter = glFragmentShaderMap.begin();
			iter != glFragmentShaderMap.end();
			++iter)
		{
			iter->second->Reload();
		}
	}

	void AssetManager::ReloadRLShader()
	{
		for (ctd::map<ctd::string, std::shared_ptr<RLSL>>::iterator iter = rlVertexShaderMap.begin();
			iter != rlVertexShaderMap.end();
			++iter)
		{
			iter->second->Reload();
		}

		for (ctd::map<ctd::string, std::shared_ptr<RLSL>>::iterator iter = rlRayShaderMap.begin();
			iter != rlRayShaderMap.end();
			++iter)
		{
			iter->second->Reload();
		}

		for (ctd::map<ctd::string, std::shared_ptr<RLSL>>::iterator iter = rlFrameShaderMap.begin();
			iter != rlFrameShaderMap.end();
			++iter)
		{
			iter->second->Reload();
		}
	}

	void AssetManager::ReloadTexture()
	{
		for (ctd::map<ctd::string, std::shared_ptr<Texture>>::iterator iter = textureMap.begin();
			iter != textureMap.end();
			++iter)
		{
			iter->second->Reload();
		}
	}

	void AssetManager::ReloadLightmap()
	{
		for (ctd::map<ctd::string, std::shared_ptr<Texture>>::iterator iter = lightmapMap.begin();
			iter != lightmapMap.end();
			++iter)
		{
			iter->second->Reload();
		}
	}

	void AssetManager::ReloadMaskMap()
	{
		for (ctd::map<ctd::string, std::shared_ptr<Texture>>::iterator iter = maskMapMap.begin();
			iter != maskMapMap.end();
			++iter)
		{
			iter->second->Reload();
		}
	}

	void AssetManager::SaveLightmap(const ANSICHAR * name, TextureFormat textureFormat, uint8 * pPixels, int32 width, int32 height)
	{
		ctd::string fileFullPathName = lightmapFullPath;
		fileFullPathName  += "\\";
		fileFullPathName += name;
		fileFullPathName += texturePNGExt;

		TextureOperator::SavePNG(
			fileFullPathName.c_str(),
			textureFormat,
			pPixels,
			width,
			height);
	}

	void AssetManager::SaveMaskMap(const ANSICHAR * name, TextureFormat textureFormat, uint8 * pPixels, int32 width, int32 height)
	{
		ctd::string fileFullPathName = maskMapFullPath;
		fileFullPathName += "\\";
		fileFullPathName += name;
		fileFullPathName += texturePNGExt;

		TextureOperator::SavePNG(
			fileFullPathName.c_str(),
			textureFormat,
			pPixels,
			width,
			height);
	}

	void AssetManager::CreateScene(const ANSICHAR * pName)
	{
		shared_ptr<Scene> scene = make_shared<Scene>();

		scene->fileName = pName;
		scene->fileNameWithExt = pName;
		scene->fileNameWithExt += sceneExt;
		
		scene->fullPathName = sceneFullPath;
		scene->fullPathName += "\\";
		scene->fullPathName += scene->fileNameWithExt;

		FileOperator::SerializeXML(scene->fullPathName.c_str(), scene);
		
		sceneMap[pName] = scene;
	}

	void AssetManager::CreatePrefab(const ANSICHAR * pName)
	{
		shared_ptr<Prefab> prefab = make_shared<Prefab>();

		prefab->fileName = pName;
		prefab->fileNameWithExt = pName;
		prefab->fileNameWithExt += prefabExt;

		prefab->fullPathName = prefabFullPath;
		prefab->fullPathName += "\\";
		prefab->fullPathName += prefab->fileNameWithExt;

		FileOperator::SerializeXML(prefab->fullPathName.c_str(), prefab);

		prefabMap[pName] = prefab;
	}

	void AssetManager::CreateMaterial(const ANSICHAR * pName)
	{
		shared_ptr<Material> material = make_shared<Material>();
		
		material->fileNameWithExt = pName;
		material->fileNameWithExt += materialExt;
		
		material->fullPathName = materialFullPath;
		material->fullPathName += "\\";
		material->fullPathName += material->fileNameWithExt;
		
		FileOperator::SerializeXML(material->fullPathName.c_str(), material);
		
		materialMap[pName] = material;
	}

	void AssetManager::SaveAll()
	{
		//	Scene
		for (ctd::map<ctd::string, std::shared_ptr<Scene>>::iterator iter = sceneMap.begin();
			iter != sceneMap.end();
			++iter)
		{
			iter->second->BeforeSave();
			FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}

		//	Prefab
		for (ctd::map<ctd::string, std::shared_ptr<Prefab>>::iterator iter = prefabMap.begin();
			iter != prefabMap.end();
			++iter)
		{
			FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}

		//	Material
		for (ctd::map<ctd::string, std::shared_ptr<Material>>::iterator iter = materialMap.begin();
			iter != materialMap.end();
			++iter)
		{
				FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}

		//	Texture Info
		for (ctd::map<ctd::string, std::shared_ptr<TextureInfo>>::iterator iter = textureInfoMap.begin();
			iter != textureInfoMap.end();
			++iter)
		{
			FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}
	}

	AssetManager::~AssetManager()
	{
		for (ctd::map<ctd::string, std::shared_ptr<Material>>::iterator iter = materialMap.begin();
			iter != materialMap.end();
			++iter)
		{
			if (iter->second->fullPathName == "")
			{
				string fullPathName = materialFullPath;
				fullPathName += "\\" + iter->first + materialExt;

				FileOperator::SerializeXML(fullPathName.c_str(), iter->second.get());
			}
			else
				FileOperator::SerializeXML(iter->second->fullPathName.c_str(), iter->second.get());
		}
	}
}