//================================================================================
//
//		SHPRTWeightCompute.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/2/2012
//
//================================================================================

#include "stdafx.h"

gmtl::Vec3f ConvertCanonicalToDir(const gmtl::Vec2f& canonical)
{
	// TODO: This needs to range uniformly from 0-90 only
	float elevation = 2.0f * gmtl::Math::aCos( gmtl::Math::sqrt(1.0f - canonical[0]) );
	float azimuth = gmtl::Math::TWO_PI * canonical[1];

	gmtl::Vec2f polar;
	polar[0] = elevation;
	polar[1] = azimuth;

	gmtl::Vec3f toReturn;
	SphericalToCartesian(toReturn, polar);
	return toReturn;
}

float GetRandomRange()
{
	return float(::rand())/float(RAND_MAX);
}

struct ParaTexSample
{
	gmtl::Vec3f dir;
	gmtl::Vec3f proj;

	uint sampleCount;
	float weight;
};

Module g_hFileSystem; 

const static uint TEX_SIZE = 64;
const static wchar* FILENAME = _W("shprtparaweights.txt");
const static wchar* FILENAME_DIR = _W("shprtdir.txt");

IFFileSystemPtr g_pFileSystem;

void PrintToFile(const ParaTexSample* pSamples, uint numSamplesPerSide)
{
	IFFilePtr pFile = g_pFileSystem->GetFile(FILENAME);
	pFile->Open(FO_WRITE_ONLY);

	// Write floats
	std::string outStr = "{\n";

	_LOOPi(numSamplesPerSide)
	{
		outStr += "\t";

		_LOOPj(numSamplesPerSide)
		{
			char buffer[256];
			//uint num = pSamples[i * numSamplesPerSide + j].sampleCount;// > 0 ? 7 : 0;
			//sprintf_s(buffer, 255, "%.4d", num);
			sprintf_s(buffer, 255, "%.9ff", pSamples[i * numSamplesPerSide + j].weight);
			outStr += buffer;
			outStr += ", ";
		}

		outStr += "\n";
	}

	outStr += "}";
	
	pFile->Write(outStr.c_str(), (uint) outStr.length());
	pFile->Close();
}

void PrintToFileDir(const gmtl::Vec3f* pSamples, uint numSamplesPerSide)
{
	IFFilePtr pFile = g_pFileSystem->GetFile(FILENAME_DIR);
	pFile->Open(FO_WRITE_ONLY);

	// Write floats
	std::string outStr = "{\n";

	_LOOPi(numSamplesPerSide)
	{
		outStr += "\t";

		_LOOPj(numSamplesPerSide)
		{
			char buffer[256];
			//uint num = pSamples[i * numSamplesPerSide + j].sampleCount;// > 0 ? 7 : 0;
			//sprintf_s(buffer, 255, "%.4d", num);
			sprintf_s(buffer, 255, "{ %.9ff, %.9ff, %.9ff }", pSamples[i * numSamplesPerSide + j][0], pSamples[i * numSamplesPerSide + j][1], pSamples[i * numSamplesPerSide + j][2]);
			outStr += buffer;
			outStr += ", ";
		}

		outStr += "\n";
	}

	outStr += "}";

	pFile->Write(outStr.c_str(), (uint) outStr.length());
	pFile->Close();
}

boolean InitFileSystem()
{
	// Load Win32 Platform
	g_hFileSystem = InitializeModule(_W("FileSystemWin32.dll"), NULL);
	if(!g_hFileSystem) return FALSE;

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(g_hFileSystem, "CreateFileSystem");

	_DEBUG_ASSERT(CreateFileSystem);

	g_pFileSystem = CreateFileSystem();
	g_pFileSystem->Initialize();	

	// Get the application path	
	wchar filePath[256];
	GetModuleFileName(NULL, filePath, 256);
	wstring path = filePath;
	std::transform(path.begin(), path.end(), path.begin(), towlower);

	int index = (int) path.rfind(_W("\\"));
	if(index != wstring::npos)
	{
		path = path.substr(0, index);
	}

	g_pFileSystem->Open(path.c_str());

	return TRUE;
}

void Shutdown()
{
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	ShutdownModule(g_hFileSystem);
}

int main(int argc, char* argv[])
{
	const static uint NUM_SAMPLES = 4096;
	const static float NUM_SAMPLES_RCP = 1.0f / NUM_SAMPLES;

	::srand(GetTickCount());

	InitFileSystem();

	// Generate texture samples
	std::vector<ParaTexSample> texSamples;
	//std::vector<gmtl::Vec3f> dirSamples;
	const float TEX_SIZE_RCP = 1.0f / TEX_SIZE;

	uint totalNumSamples = 0;

	_LOOPi(TEX_SIZE)
	{
		_LOOPj(TEX_SIZE)
		{
			// Find paraboloid space x/y projections
			gmtl::Vec3f proj;
			proj[0] = ((j + 0.5f) * TEX_SIZE_RCP * 2.0f) - 1.0f;
			proj[1] = 1.0f;
			proj[2] = ((1.0f - (i + 0.5f) * TEX_SIZE_RCP) * 2.0f) - 1.0f;
			
			// Project the samples from paraboloid space to world space dir
			// x = 2*u/(v^2+u^2+1)
			// y = 2*v/(v^2+u^2+1)
			// z = -(v^2+u^2-1)/(v^2+u^2+1)
			gmtl::Vec3f dir;
			dir[0] = (2.0f * proj[0]) / (proj[2] * proj[2] + proj[0] * proj[0] + 1.0f);
			dir[1] = -(proj[2] * proj[2] + proj[0] * proj[0] - 1.0f) / (proj[2] * proj[2] + proj[0] * proj[0] + 1.0f);
			dir[2] = (2.0f * proj[2]) / (proj[2] * proj[2] + proj[0] * proj[0] + 1.0f);

			ParaTexSample sample;
			sample.dir = dir;
			sample.proj = proj;
			sample.sampleCount = 0;
			sample.weight = 0.0f;

			//if(dir[1] >= 0.0f)
			//{
			//	sample.sampleCount++;
			//	totalNumSamples++;
			//}

			texSamples.push_back(sample);
		}
	}
			
	_LOOPi(NUM_SAMPLES)
	{
		_LOOPj(NUM_SAMPLES)
		{
			// Generate uniformly distributed samples on the sphere
			gmtl::Vec2f canonical;
			canonical[0] = (i + GetRandomRange()) * NUM_SAMPLES_RCP * 0.5f;	// Only consider hemisphere
			canonical[1] = (j + GetRandomRange()) * NUM_SAMPLES_RCP;
			
			// Convert samples to cartesian
			gmtl::Vec3f dir = ConvertCanonicalToDir(canonical);
			//dirSamples.push_back(dir);

			gmtl::Vec3f paraProj = dir;

			// Project to paraboloid
			float projDiv = paraProj[1] + 1.0f;
			paraProj[0] /= projDiv;
			paraProj[2] /= projDiv;
			paraProj[1] = 1.0f;
			
			// Find texture sample
			float x = paraProj[0] * 0.5f + 0.5f;
			float y = 1.0f - (paraProj[2] * 0.5f + 0.5f);
			x *= TEX_SIZE;
			y *= TEX_SIZE;

			int xIndex = (int) x;
			_DEBUG_ASSERT(xIndex >= 0);
			if(xIndex > TEX_SIZE-1)
				xIndex = TEX_SIZE-1;

			int yIndex = (int) y;
			_DEBUG_ASSERT(yIndex >= 0);
			if(yIndex > TEX_SIZE-1)
				yIndex = TEX_SIZE-1;

			ParaTexSample& sample = texSamples[yIndex * TEX_SIZE + xIndex];
			//_DEBUG_ASSERT(sample.sampleCount > 0);
			sample.sampleCount++;
			totalNumSamples++;
		}
	}

	// Sum samples percentage
	float debugTotalWeight = 0.0f;
	_LOOPi(TEX_SIZE)
	{
		_LOOPj(TEX_SIZE)
		{
			// Compute weights based on the influence of each texture sample
			ParaTexSample& sample = texSamples[i * TEX_SIZE + j];
			sample.weight = ((float) sample.sampleCount) / totalNumSamples;

			// Take into account the integral over the sphere domain
			sample.weight *= (4.0f * gmtl::Math::PI);

			// Take also into account the other side of the hemisphere
			// since our simulation is only done for one side
			sample.weight *= 0.5f;

			debugTotalWeight += sample.weight;
		}
	}

	_DEBUG_ASSERT(gmtl::Math::abs(debugTotalWeight - (2.0f * gmtl::Math::PI)) <= 0.001f);

	PrintToFile(&texSamples[0], TEX_SIZE);
	//PrintToFileDir(&dirSamples[0], NUM_SAMPLES);

	Shutdown();

	// Project the samples to paraboloid space
	// x = 2*u/(v^2+u^2+1)
	// y = 2*v/(v^2+u^2+1)
	// z = -(v^2+u^2-1)/(v^2+u^2+1)

	// Assign the projected samples to the correct texture pixel
	// Count the number of thus assigned samples per texture pixel 
	// Each texture pixel starts with 1 sample already

	// Compute the percentage of samples assigned to each texture pixel
	// IE, percentAssigned = numSamplesInPixel / totalNumberOfSamples

	// Then weight the texture pixel inversely proportional to the percentage of samples assigned
	// Why? We assume unbiased, uniform samples when doing integration, so any biases must be
	// weighted out
	// IE, weight = (1.0f / percentAssigned) / ( SUM( totalInversePercentAssigned ) )

	// Generate the results:
	// 1. texture pixel index
	// 2. weight
	// 3. ray vector (?)

	return 0;
}