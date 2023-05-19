#include "UglyCam.h"


glm::mat4 UglyCam::getView()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, up);
}