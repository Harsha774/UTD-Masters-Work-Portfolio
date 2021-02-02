#include "Curve.h"

Curve::Curve()
{
}

Curve::~Curve()
{
}

void Curve::init()
{
	this->control_points_pos = {
		{ 0.0, 8.5, -2.0 },
		{ -3.0, 11.0, 2.3 },
		{ -6.0, 8.5, -2.5 },
		{ -4.0, 5.5, 2.8 },
		{ 1.0, 2.0, -4.0 },
		{ 4.0, 2.0, 3.0 },
		{ 7.0, 8.0, -2.0 },
		{ 3.0, 10.0, 3.7 }
	};
	calculate_curve();
}

void Curve::calculate_curve()
{
	/*
	since this is a closed curve, I added 1 point in the starting and 2 points at the end to complete catmull rom for all segments
	P7,P0,P1,P2 -> P0 P1 -> segment 0
	P0,P1,P2,P3 -> P1 P2 -> segment 1
	P1,P2,P3,P4 -> P2 P3 -> segment 2
	P2,P3,P4,P5 -> P3 P4 -> segment 3
	P3,P4,P5,P6 -> P4 P5 -> segment 4
	P4,P5,P6,P7 -> P5 P6 -> segment 5
	P5,P6,P7,P0 -> P6 P7 -> segment 6
	P6,P7,P0,P1 -> P7 P0 -> segment 7
	*/
	std::vector<glm::vec3> control_points(this->control_points_pos);
	control_points.insert(control_points.begin(), this->control_points_pos[7]);
	control_points.push_back(this->control_points_pos[0]);
	control_points.push_back(this->control_points_pos[1]);


	glm::vec3 p0, p1, p2, p3;
	float increment = 1.0 / this->num_points_per_segment; //0.005
	//std::cout<< "increment " << increment << std::endl;
	float sum_linear_len = 0.0f;
	int cache_count = 0;
	int control_point_size = control_points.size() - 3;

	for (int p = 0; p < control_point_size; p++) {
		p0 = control_points[p];
		p1 = control_points[p + 1];
		p2 = control_points[p + 2];
		p3 = control_points[p + 3];
		//std::cout << "segment " << (p + 1) % control_point_size << "sum " << sum_linear_len << std::endl;
		for (float x = 0; x <= 1; x += increment) {
			glm::vec3 point = catmull_rom(x, p0, p1, p2, p3);
			float approx_arc_len = glm::sqrt(powf(p1.x - point.x, 2) + powf(p1.y - point.y, 2) + powf(p1.z - point.z, 2));
			sum_linear_len += approx_arc_len;
			this->cache[cache_count].point = point;
			this->cache[cache_count].segment_no = (p) % control_point_size;
			this->cache[cache_count].length = sum_linear_len;
			cache_count += 1;
			this->curve_points_pos.push_back(point);
		}
		//std::cout << cache_count << "segment " << (p + 1) % control_point_size << "sum " << sum_linear_len << std::endl;
	}
	this->cache_size = cache_count;
	
}

glm::vec3 Curve::catmull_rom(float x, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::mat4 catmull_rom_mat = {
		{-0.5,  1.5, -1.5,  0.5},
		{   1, -2.5,    2, -0.5},
		{-0.5,    0,  0.5,    0},
		{   0,    1,    0,    0}
	};

	float xsquare = x * x;
	float xcube = x * x * x;

	glm::vec4 input = { xcube, xsquare, x, 1 };
	glm::mat4x3 points = { p0,p1,p2,p3 };
	//std::cout << glm::to_string(points * catmull_rom_mat * input) << std::endl;
	//column order of 1x4 * 4x4 * 4x3
	return points * catmull_rom_mat * input;
}