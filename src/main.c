#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RAYMATH_IMPLEMENTATION
#define RAYMATH_STANDALONE
#include <raymath.h>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../include/util.h"
#include "../include/types.h"
#include "../include/camera.h"
#include "../include/renderer.h"
#include "../include/chunk.h"
#include "../include/debug.h"



//------------------------------------------------------------------------

Camera cam;
bool left_click_down = false;
bool right_click_down = false;

typedef struct {
	Vector3 pos;
	Vector3 vel;
	Vector3 acc;
} Player;


//------------------------------------------------------------------------


int test_amount = 0;
float test_last = 0.f;
int main() {
	// Window setup

	int width = 800;
	int height = 800;
	Renderer_init(&cam, width, height);


	Player player = (Player) { 0 };
	player.pos = (Vector3) { CHUNKS_RADIUS / 2.f * CHUNK_LENGTH, CHUNK_HEIGHT + 40.f, CHUNKS_RADIUS / 2.f * CHUNK_LENGTH };

	cam = Camera_create(PI / 2.f, width, height, 0.001f, 3000.f);
	cam.pos = player.pos;
	Vector3 vel;


	float delta = 0;

	float total_time = 0;
	float prev_time = 0;
	int first_loop = 1;

	Vector2 mouse_pos   = { 0 };
	Vector2 mouse_prev  = { 0 };
	Vector2 mouse_delta = { 0 };

	Chunks_init();

	while(!Renderer_shouldClose()) {
		total_time = glfwGetTime();
		delta = total_time - prev_time;
		prev_time = total_time;

		double mouse_x, mouse_y;
		get_cursor(&mouse_x, &mouse_y);

		mouse_pos.x = mouse_x;
		mouse_pos.y = mouse_y;

		mouse_delta.x = mouse_pos.x - mouse_prev.x;
		mouse_delta.y = mouse_pos.y - mouse_prev.y;
		mouse_prev = mouse_pos;

		cam.pitch -= mouse_delta.y * delta * 0.035f;
		cam.yaw += mouse_delta.x * delta * 0.035f;

		if(!get_mouse_button(GLFW_MOUSE_BUTTON_LEFT)) {
			left_click_down = false;
		}

		if(!get_mouse_button(GLFW_MOUSE_BUTTON_RIGHT)) {
			right_click_down = false;
		}

		if(get_key(GLFW_KEY_LEFT_ALT)|| get_key(GLFW_KEY_LEFT_SUPER))
			set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		float move_speed = 6.f;
		if(get_key(GLFW_KEY_LEFT_SHIFT))
			move_speed = 9.f;

		float move_vel_x = 0;
		float move_vel_z = 0;

		if(get_key(GLFW_KEY_W)) {
			move_vel_x += cos(cam.yaw);
			move_vel_z += sin(cam.yaw);
		}

		if(get_key(GLFW_KEY_S)) {
			move_vel_x -= cos(cam.yaw);
			move_vel_z -= sin(cam.yaw);
		}

		if(get_key(GLFW_KEY_A)) {
			move_vel_x += cos(cam.yaw - PI / 2);
			move_vel_z += sin(cam.yaw - PI / 2);
		}

		if(get_key(GLFW_KEY_D)) {
			move_vel_x -= cos(cam.yaw - PI / 2);
			move_vel_z -= sin(cam.yaw - PI / 2);
		}

#define INTERPOLATION_STEPS 10
		for(int i = 0; i < INTERPOLATION_STEPS; i++) {
			float off_x, off_y, off_z = 0;

			off_x = move_vel_x * move_speed * (delta / INTERPOLATION_STEPS);
			off_y = -10 * delta / INTERPOLATION_STEPS;
			off_z = move_vel_z * move_speed * (delta / INTERPOLATION_STEPS);

			cam.pos.x += off_x;
			cam.pos.y += off_y;
			cam.pos.z += off_z;
			if(!Chunks_getBlock((Vector3) { cam.pos.x, cam.pos.y + 2.f, cam.pos.z })) {
				cam.pos.x -= off_x;
				cam.pos.y -= off_y;
				cam.pos.z -= off_z;
				break;
			}

		}


		/* if(Chunks_getBlockFromRay((Vector3) { cam.pos.x + 0.5f, cam.pos.y + 0.5f, cam.pos.z + 0.5f, }, (Vector3) { 0, -1.f, 0 }, 2.f, NULL, NULL, NULL, NULL, NULL)) { */
		/* 	cam.pos.y -= 9 * delta; */
		/* } */

//		if( !Chunks_getBlock((Vector3) { new_x + 0.5f, cam.pos.y + 0.5f, cam.pos.z + 0.5f }) &&
//			!Chunks_getBlock((Vector3) { new_x + 0.5f, cam.pos.y + 1.5f, cam.pos.z + 0.5f }))

//		if( !Chunks_getBlock((Vector3) { cam.pos.x + 0.5f, cam.pos.y + 0.5f, new_z + 0.5f }) &&
//			!Chunks_getBlock((Vector3) { cam.pos.x + 0.5f, cam.pos.y - 1.5f, new_z + 0.5f }))

		if(get_mouse_button(GLFW_MOUSE_BUTTON_LEFT) && !left_click_down) {
			left_click_down = true;

			uint chunk_id;
			Vector3 block_pos;

			if(Chunks_getBlockFromRay((Vector3) { cam.pos.x + .5f, cam.pos.y + .5f, cam.pos.z + .5f }, cam.front, 10.f, NULL, &chunk_id, NULL, &block_pos, NULL)) {
				Chunks_setBlock(chunk_id, world_to_block(block_pos), 0);
			}
		}

		else if(get_mouse_button(GLFW_MOUSE_BUTTON_RIGHT) && !right_click_down) {
			right_click_down = true;
			Vector3 chunk_pos;
			Vector3 block_pos;
			Vector3 normal;

			if(Chunks_getBlockFromRay((Vector3) { cam.pos.x + .5f, cam.pos.y + .5f, cam.pos.z + .5f }, cam.front, 10.f, NULL, NULL, NULL, &block_pos, &normal)) {
				block_pos.x += normal.x;
				block_pos.y += normal.y;
				block_pos.z += normal.z;

				chunk_pos = world_to_chunk(block_pos);

				uint chunk_id = CHUNKS_RADIUS * chunk_pos.z + chunk_pos.x;

				Chunks_setBlock(chunk_id, world_to_block(block_pos), 1);
			}
		}

		cam.pos.x -= (cam.pos.x > CHUNKS_RADIUS * CHUNK_LENGTH) * CHUNKS_RADIUS * CHUNK_LENGTH;
		cam.pos.x += (cam.pos.x < -0.f) * CHUNKS_RADIUS * CHUNK_LENGTH;

		cam.pos.z -= (cam.pos.z > CHUNKS_RADIUS * CHUNK_LENGTH) * CHUNKS_RADIUS * CHUNK_LENGTH;
		cam.pos.z += (cam.pos.z < -0.f) * CHUNKS_RADIUS * CHUNK_LENGTH;

		Camera_update(&cam, (Vector3) { 0, 1, 0 });

		Chunks_update();
		Renderer_clear();
		Renderer_display();
		first_loop = 0;
	}

	Chunks_free();
	Renderer_free();

	//debug_log();
	debug_log_empty();
	return 0;
}

