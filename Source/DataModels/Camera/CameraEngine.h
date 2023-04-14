#pragma once

#include "Camera.h"
#include "Math/float3x3.h"
class GameObject;

class CameraEngine : public Camera
{
public:
	CameraEngine();
	CameraEngine(const std::unique_ptr<Camera>& camera);
	virtual ~CameraEngine() override;

	bool Update() override;

	void Move();
	void UnlimitedCursor();
	void Zoom();
	void Focus(const OBB& obb);
	void Focus(GameObject* gameObject);
	void Orbit(const OBB& obb);

private:

    bool ContainsNaN(const float3x3& m) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (isnan(m.At(i, j))) {
                    return true;
                }
            }
        }
        return false;
    }

    bool ContainsInf(const float3x3& m) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (isinf(m.At(i, j))) {
                    return true;
                }
            }
        }
        return false;
    }

    void ApplyRotationWithFixedUp(const float3x3& rotationMatrix, const float3& fixedUp)
    {
        float3 oldFront = frustum->Front().Normalized();
        float3 oldUp = frustum->Up().Normalized();

        float3 newFront = rotationMatrix.MulDir(oldFront);
        float3 newRight = fixedUp.Cross(newFront).Normalized();
        float3 newUp = newFront.Cross(newRight);

        frustum->SetFront(newFront);
        frustum->SetUp(newUp);

        rotation = rotationMatrix.ToQuat();
    }



};