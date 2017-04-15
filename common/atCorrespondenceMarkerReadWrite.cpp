#include "atCorrespondenceMarkerReadWrite.h"

atRenderEngine::Utility::atCorrespondenceMarker::atCorrespondenceMarker()
{
}

atRenderEngine::Utility::atCorrespondenceMarker::~atCorrespondenceMarker()
{
}

bool atRenderEngine::Utility::atCorrespondenceMarker::SaveCorrespondenceFile(std::string filePath, std::vector<CorrespondenceMarkerData> markerData)
{
	FILE *fd = fopen(filePath.c_str(), "w");
	if (fd != NULL)
	{
		/* save vertex information */
		for (std::vector<CorrespondenceMarkerData>::iterator it = markerData.begin(); it != markerData.end(); it++)
		{
			fprintf(fd, "%d ,  %d\n",
				it->vertIdxSource, it->vertIdxTarget);
		}

		fclose(fd);
		return true;
	}
	else
	{
		return false;
	}
}
