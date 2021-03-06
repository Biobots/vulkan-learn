#include <image.hpp>
#include <ray.hpp>
#include <camera.hpp>
#include <screen.hpp>
#include <triangle.hpp>
#include <sphere.hpp>
#include <vector>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600

int main()
{
	Screen scr(vec2(WIDTH, HEIGHT), WIDTH, HEIGHT, vec3(-WIDTH/2, -HEIGHT/2, 0));
	Camera cam(vec3(0, 0, -100));
	Sphere sph(vec3(0, 0, 200), 200);
	Triangle tri({vec3(0, 200, 20), vec3(100, -100, 20), vec3(-100, -100, 20)});
	std::vector<unsigned char> data;
	data.resize(WIDTH*HEIGHT*3);
	for(int y = 0; y < HEIGHT; y++)
	{
		for(int x = 0; x < WIDTH; x++)
		{
			Ray ray = scr.Pixel2RayDirection(x, y, cam);
#if 0
			if(sph.Intersect(ray))
			{
				data[3*(WIDTH*y+x)] = 0;
				data[3*(WIDTH*y+x)+1] = 255;
				data[3*(WIDTH*y+x)+2] = 0;
			}
			else
			{
				data[3*(WIDTH*y+x)] = 255;
				data[3*(WIDTH*y+x)+1] = 255;
				data[3*(WIDTH*y+x)+2] = 255;
			}
#else
			if(tri.Intersect(ray))
			{
				data[3*(WIDTH*y+x)] = 0;
				data[3*(WIDTH*y+x)+1] = 255;
				data[3*(WIDTH*y+x)+2] = 0;
			}
			else
			{
				data[3*(WIDTH*y+x)] = 255;
				data[3*(WIDTH*y+x)+1] = 255;
				data[3*(WIDTH*y+x)+2] = 255;
			}
#endif
		}
	}
	//write_ppm("result.ppm", data, WIDTH, HEIGHT, 3);
	gl_render(data, WIDTH, HEIGHT);
	return 0;
}