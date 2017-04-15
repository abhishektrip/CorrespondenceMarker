#pragma once
#include "atUtility.h"

namespace atRenderEngine
{
	namespace Utility
	{
		struct CorrespondenceMarkerData
		{
			int vertIdxSource, vertIdxTarget; 
		};
		class atCorrespondenceMarker
		{
		public:
			atCorrespondenceMarker();
			~atCorrespondenceMarker();
			bool SaveCorrespondenceFile(std::string filePath, std::vector<CorrespondenceMarkerData> markerData);
		private:

		};
	}
}


