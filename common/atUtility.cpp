#include "atUtility.h"

#include <iostream>
using namespace atRenderEngine;

GLuint Utility::atLoadShaders(const char * vertexShaderPath, const char * fragmentShaderPath)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexShaderPath, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentShaderPath, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertexShaderPath);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragmentShaderPath);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(std::fmaxl(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	if(InfoLogLength >0)
		fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void Utility::atErrorCallback(int error, const char * description)
{
	std::cerr << description << std::endl;
}
float Utility::SquaredDistance(const Vector3f & a, const Vector3f & b)
{
	float x = a.x() - b.x();
	float y = a.y() - b.y();
	float z = a.z() - b.z();

	return x*x + y* y + z*z;
}
float Utility::SquaredDistanceRect(const BoundingRect *rect, const Vector3f& x0)
{
	int i_dim;
	float dist_sq = 0;

	for (i_dim = 0; i_dim < 3; i_dim++)
	{
		if (x0[i_dim] < rect->min[i_dim]) {
			dist_sq +=
				(rect->min[i_dim] - x0[i_dim]) *
				(rect->min[i_dim] - x0[i_dim]);
		}
		else if (x0[i_dim] > rect->max[i_dim]) {
			dist_sq +=
				(rect->max[i_dim] - x0[i_dim]) *
				(rect->max[i_dim] - x0[i_dim]);
		}
	}

	return dist_sq;
}

Utility::BoundingBoxContains Utility::IsWithinBoundingBox(const BoundingRect & rect, const Vector3f v[3] , int cd)
{
	bool result[3] = { true, true,true };
	BoundingBoxContains containsResult = BoundingBoxContains::BB_NOT_WITHIN;
	for (int i = 0; i < 3; i++)
	{
		result[i] = result[i] && (rect.max[cd] >= v[i][cd] && v[i][cd] >= rect.min[cd]);		
	}

	if (result[0] && result[1] && result[2])
		containsResult = BoundingBoxContains::BB_FULLY_WITHIN;
	else if (result[0] || result[1] || result[2])
		containsResult = BoundingBoxContains::BB_PARTIALLY_WITHIN;

	return containsResult;
}
Utility::BoundingBoxContains Utility::IsWithinBoundingBox(const BoundingRect & rect, const Vector3f v[3])
{
	bool result[3] = { true, true,true };
	BoundingBoxContains containsResult = BoundingBoxContains::BB_NOT_WITHIN;
	for (int i = 0; i < 3; i++)
	{
		result[i] = result[i] && (rect.max.x() >= v[i].x() && v[i].x() >= rect.min.x());
		result[i] = result[i] && (rect.max.y() >= v[i].y() && v[i].y() >= rect.min.y());
		result[i] = result[i] && (rect.max.z() >= v[i].z() && v[i].z() >= rect.min.z());
	}

	if (result[0] && result[1] && result[2])
		containsResult = BoundingBoxContains::BB_FULLY_WITHIN;
	else if (result[0] || result[1] || result[2])
		containsResult = BoundingBoxContains::BB_PARTIALLY_WITHIN;

	return containsResult;
}

void Utility::ScreenPosToWorldRay(
	int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	int screenWidth, int screenHeight,  // Window size, in pixels
	Matrix4f ViewMatrix,               // Camera position and orientation
	Matrix4f ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	Vector3f& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	Vector3f& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
) {

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	Vector4f lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * -2.0f, // [0, 768] -> [-1,1]
		-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	Vector4f lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * -2.0f,
		0.0,
		1.0f
	);


	// The Projection matrix goes from Camera Space to NDC.
	// So inverse(ProjectionMatrix) goes from NDC to Camera Space.
	Matrix4f InverseProjectionMatrix = ProjectionMatrix.inverse();

	// The View Matrix goes from World Space to Camera Space.
	// So inverse(ViewMatrix) goes from Camera Space to World Space.
	Matrix4f InverseViewMatrix = ViewMatrix.inverse();

	Vector4f lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /=lRayStart_camera.w();
	Vector4f lRayStart_world = InverseViewMatrix       * lRayStart_camera; lRayStart_world /= lRayStart_world.w();
	Vector4f lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w();
	Vector4f lRayEnd_world = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w();


	// Faster way (just one inverse)
	//Matrix4f M = glm::inverse(ProjectionMatrix * ViewMatrix);
	//Vector4f lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
	//Vector4f lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

	Vector4f result = lRayEnd_world - lRayStart_world;
	Vector3f lRayDir_world(result.x(), result.y(), result.z());
	lRayDir_world = lRayDir_world.normalized();


	out_origin = Vector3f(lRayStart_world.x(), lRayStart_world.y(), lRayStart_world.z());
	out_direction = Vector3f(lRayDir_world);
}


bool Utility::TestRayOBBIntersection(
	Vector3f ray_origin,        // Ray origin, in world space
	Vector3f ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	Vector3f aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	Vector3f aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	Matrix4f ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
)
{

	//std::cout << "Test OBB  \n";
	//std::cout << "Min : " << aabb_min.x() << " , " << aabb_min.y() << " , " << aabb_min.z() << std::endl;
	//std::cout << "Max : " << aabb_max.x() << " , " << aabb_max.y() << " , " << aabb_max.z() << std::endl;

	/*std::cout << ModelMatrix(0, 0) << "\t" << ModelMatrix(0, 1) << "\t" << ModelMatrix(0, 2) << "\t" << ModelMatrix(0, 3) << std::endl;
	std::cout << ModelMatrix(1, 0) << "\t" << ModelMatrix(1, 1) << "\t" << ModelMatrix(1, 2) << "\t" << ModelMatrix(1, 3) << std::endl;
	std::cout << ModelMatrix(2, 0) << "\t" << ModelMatrix(2, 1) << "\t" << ModelMatrix(2, 2) << "\t" << ModelMatrix(2, 3) << std::endl;
	std::cout << ModelMatrix(3, 0) << "\t" << ModelMatrix(3, 1) << "\t" << ModelMatrix(3, 2) << "\t" << ModelMatrix(3, 3) << std::endl;*/

	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	float tMin = 0.0f;
	float tMax = 100000.0f;

	Vector3f OBBposition_worldspace(ModelMatrix(0,3), ModelMatrix(1,3), ModelMatrix(2,3));

	Vector3f delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		Vector3f xaxis = Vector3f(ModelMatrix(0, 0), ModelMatrix(1, 0), ModelMatrix(2, 0));
		float e = xaxis.dot(delta);
		float f = ray_direction.dot(xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x()) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x()) / f; // Intersection with the "right" plane
											 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

											 // We want t1 to represent the nearest intersection, 
											 // so if it's not the case, invert t1 and t2
			if (t1>t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x() > 0.0f || -e + aabb_max.x() < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		Vector3f yaxis = Vector3f(ModelMatrix(0, 1), ModelMatrix(1, 1), ModelMatrix(2, 1));
		float e = yaxis.dot(delta);
		float f = ray_direction.dot( yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y()) / f;
			float t2 = (e + aabb_max.y()) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y() > 0.0f || -e + aabb_max.y() < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		Vector3f zaxis = Vector3f(ModelMatrix(0, 2), ModelMatrix(1, 2), ModelMatrix(2, 2));
		float e = zaxis.dot( delta);
		float f = ray_direction.dot( zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z()) / f;
			float t2 = (e + aabb_max.z()) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z() > 0.0f || -e + aabb_max.z() < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

}

bool Utility::RayTriangleIntersection(Utility::RayData ray, Vector3f v1, Vector3f v2, Vector3f v3, float& result )
{
#define EPSILON 0.000001

	//int triangle_intersection(const Vec3   V1,  // Triangle vertices
	//	const Vec3   V2,
	//	const Vec3   V3,
	//	const Vec3    O,  //Ray origin
	//	const Vec3    D,  //Ray direction
	//	float* out)
	{
		Vector3f e1, e2;  //Edge1, Edge2
		Vector3f P, Q, T;
		float det, inv_det, u, v;
		float t;

		//Find vectors for two edges sharing V1
		e1 = v2 - v1;//  V2, V1);
		e2 = v3 - v1; // V3, V1);
		//Begin calculating determinant - also used to calculate u parameter
		P =  ray.rayDirection.cross(e2);
		//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
		det = e1.dot(P);
		//NOT CULLING
		if (det > -EPSILON && det < EPSILON) return false;
		inv_det = 1.f / det;

		//calculate distance from V1 to ray origin
		T =  ray.rayOrigin - v1;

		//Calculate u parameter and test bound
		u = T.dot(P) * inv_det;
		//The intersection lies outside of the triangle
		if (u < 0.f || u > 1.f) return false;

		//Prepare to test v parameter
		Q =  T.cross(e1);

		//Calculate V parameter and test bound
		v = ray.rayDirection.dot(Q) * inv_det;
		//The intersection lies outside of the triangle
		if (v < 0.f || u + v  > 1.f) return false;

		t = e2.dot(Q) * inv_det;

		if (t > EPSILON) { //ray intersection
			result = t;
			return true;
		}

		// No hit, no win
		return false;
	}
}
