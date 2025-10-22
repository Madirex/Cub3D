#include "../../includes/cub3d_render.h"
#include <sys/time.h>
#include <mlx.h>

/* Time helpers, door animation update and render loop */

/* Return current time in ms */
long fn_get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((long)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

/* Update door animation frame based on timers */
void update_door_animation(t_cub3d *cub)
{
	if (!cub->is_bonus || !cub->door_textures)
		return ;
	cub->door_anim_timer += cub->time_frame;
	if (cub->door_anim_timer >= cub->door_anim_speed)
	{
		cub->door_anim_frame = (cub->door_anim_frame + 1) % 3;
		cub->door_anim_timer = 0.0;
	}
}

/* Main render loop called by MLX */
int render_loop(t_cub3d *cub)
{
	long	current_time;
	t_img	img;

	current_time = fn_get_time_in_ms();
	cub->time_frame = (current_time - cub->time_prev) / 1000.0;
	cub->time_prev = current_time;
	perform_movements(cub);
	update_door_animation(cub);
	img.data = cub->img_data;
	img.size_line = cub->size_line;
	img.bpp = cub->bpp;
	img.bytes_per_pixel = img.bpp / 8;
	raycast_render(cub, &img);
	if (cub->is_bonus)
		draw_minimap(cub, &img);
	mlx_put_image_to_window(cub->mlx, cub->win, cub->img, 0, 0);
	draw_door_prompt(cub);
	return (0);
}