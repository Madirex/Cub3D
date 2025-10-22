#include "../../includes/cub3d_render.h"
/* Door prompt helpers and rendering */

/* Check near/far target logic used for drawing prompt */
int check_near_far_target(t_cub3d *cub, double near_d, double far_d, int *out_x, int *out_y)
{
	if (!get_target_cell_coords(cub, near_d, out_x, out_y))
		return (0);
	if ((int)cub->pos_x == *out_x && (int)cub->pos_y == *out_y)
	{
		if (!get_target_cell_coords(cub, far_d, out_x, out_y))
			return (0);
	}
	return (1);
}

/* Return prompt string based on door cell state */
const char *select_door_message(char cell)
{
	if (cell == 'D')
		return ("Press SPACE to open");
	return ("Press SPACE to close");
}

/* Draw door prompt on screen if applicable */
void draw_door_prompt(t_cub3d *cub)
{
	int			target_mapX;
	int			target_mapY;
	const char	*message;
	char		target_cell;

	if (!check_near_far_target(cub, 0.75, 1.5, &target_mapX, &target_mapY))
		return ;
	target_cell = cub->map[target_mapY][target_mapX];
	if (cub->is_bonus && (target_cell == 'D' || target_cell == 'O'))
	{
		message = select_door_message(target_cell);
		mlx_string_put(cub->mlx, cub->win,
			(WIDTH / 2) - 100, HEIGHT - 50,
			0xFFFFFF, (char *)message);
	}
}