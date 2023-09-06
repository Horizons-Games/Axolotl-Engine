#ifndef _COMMON_FUNCTIONS_INCLUDE_
#define _COMMON_FUNCTIONS_INCLUDE_

bool InsideBox(vec3 point, vec3 minBox, vec3 maxBox)
{

    //Check if the point is less than max and greater than min
    if (point.x > minBox.x && point.x < maxBox.x &&
        point.y > minBox.y && point.y < maxBox.y &&
        point.z > minBox.z && point.z < maxBox.z)
    {
        return true;
    }

    //If not, then return false
    return false;
}

#endif